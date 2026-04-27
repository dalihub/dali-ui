/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include "focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/wheel-event.h>
#include <dali/public-api/object/property-map.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/internal/focus-manager/focus-finder.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali/devel-api/adaptor-framework/accessibility.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace // Unnamed namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_KEYBOARD_FOCUS_MANAGER");
#endif

const char* const FOCUS_BORDER_IMAGE_FILE_NAME = "keyboard_focus.9.png";

// Key name constants for OnKeyEvent
constexpr const char* KEY_NAME_LEFT      = "Left";
constexpr const char* KEY_NAME_RIGHT     = "Right";
constexpr const char* KEY_NAME_UP        = "Up";
constexpr const char* KEY_NAME_DOWN      = "Down";
constexpr const char* KEY_NAME_PRIOR     = "Prior";
constexpr const char* KEY_NAME_NEXT      = "Next";
constexpr const char* KEY_NAME_TAB       = "Tab";
constexpr const char* KEY_NAME_SPACE     = "space";
constexpr const char* KEY_NAME_EMPTY     = "";
constexpr const char* KEY_NAME_BACKSPACE = "Backspace";
constexpr const char* KEY_NAME_ESCAPE    = "Escape";
constexpr const char* KEY_NAME_RETURN    = "Return";

// Logical key name constants for OnKeyEvent
constexpr const char* LOGICAL_KEY_NAME_KP_LEFT  = "KP_Left";
constexpr const char* LOGICAL_KEY_NAME_KP_RIGHT = "KP_Right";
constexpr const char* LOGICAL_KEY_NAME_KP_UP    = "KP_Up";
constexpr const char* LOGICAL_KEY_NAME_KP_DOWN  = "KP_Down";
constexpr const char* LOGICAL_KEY_NAME_KP_PRIOR = "KP_Prior";
constexpr const char* LOGICAL_KEY_NAME_KP_NEXT  = "KP_Next";
constexpr const char* LOGICAL_KEY_NAME_KP_ENTER = "KP_Enter";

BaseHandle Create()
{
  BaseHandle handle = FocusManager::Get();

  if(!handle)
  {
    SingletonService singletonService(SingletonService::Get());
    if(singletonService)
    {
      Ui::FocusManager manager = Ui::FocusManager(new Internal::FocusManager());
      singletonService.Register(typeid(manager), manager);
      handle = manager;
    }
  }

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Ui::FocusManager, Dali::BaseHandle, Create, true)

DALI_SIGNAL_REGISTRATION(Ui, FocusManager, "focusChanged", SIGNAL_FOCUS_CHANGED)
DALI_SIGNAL_REGISTRATION(Ui, FocusManager, "focusGroupChanged", SIGNAL_FOCUS_GROUP_CHANGED)

DALI_TYPE_REGISTRATION_END()

const unsigned int MAX_HISTORY_AMOUNT = 30; ///< Max length of focus history stack

} // unnamed namespace

Ui::FocusManager FocusManager::Get()
{
  Ui::FocusManager manager;

  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    // Check whether the keyboard focus manager is already created
    Dali::BaseHandle handle = singletonService.GetSingleton(typeid(Ui::FocusManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to keyboard focus manager
      manager = Ui::FocusManager(static_cast<FocusManager*>(handle.GetObjectPtr()));
    }
  }

  return manager;
}

FocusManager::FocusManager()
: mFocusChangedSignal(),
  mFocusGroupChangedSignal(),
  mCurrentFocusView(),
  mFocusIndicatorView(),
  mFocusFinderRootView(),
  mFocusHistory(),
  mSlotDelegate(this),
  mCurrentFocusedWindow(),
  mIsFocusIndicatorShown(UNKNOWN),
  mEnableFocusIndicator(ENABLE),
  mAlwaysShowIndicator(ALWAYS_SHOW),
  mFocusGroupLoopEnabled(false),
  mClearFocusOnTouch(true),
  mEnableDefaultAlgorithm(true),
  mClearFocusOnWindowFocusLost(true),
  mCurrentWindowId(0),
  mLastFocusChangeContext()
{
  // TODO: Get FocusIndicatorEnable constant from stylesheet to set mIsFocusIndicatorShown.

  LifecycleController::Get().PreInitSignal().Connect(mSlotDelegate, &FocusManager::OnAdaptorInit);
}

void FocusManager::OnAdaptorInit()
{
  if(Adaptor::IsAvailable())
  {
    // Retrieve all the existing scene holders
    Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
    for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
    {
      (*iter).KeyEventSignal().Connect(mSlotDelegate, &FocusManager::OnKeyEvent);
      (*iter).TouchedSignal().Connect(mSlotDelegate, &FocusManager::OnTouch);
      (*iter).WheelEventGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnCustomWheelEvent);
      (*iter).WheelEventSignal().Connect(mSlotDelegate, &FocusManager::OnWheelEvent);
      (*iter).FocusChangedGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneHolderFocusChanged);
      Window window = Window::DownCast(*iter);
      if(window)
      {
        window.FocusChangeSignal().Connect(mSlotDelegate, &FocusManager::OnWindowFocusChanged);
      }
    }

    // Get notified when any new scene holder is created afterwards
    Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneHolderCreated);
  }
}

void FocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder)
{
  sceneHolder.KeyEventSignal().Connect(mSlotDelegate, &FocusManager::OnKeyEvent);
  sceneHolder.TouchedSignal().Connect(mSlotDelegate, &FocusManager::OnTouch);
  sceneHolder.WheelEventGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnCustomWheelEvent);
  sceneHolder.WheelEventSignal().Connect(mSlotDelegate, &FocusManager::OnWheelEvent);
  sceneHolder.FocusChangedGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneHolderFocusChanged);
  Window window = Window::DownCast(sceneHolder);
  if(window)
  {
    window.FocusChangeSignal().Connect(mSlotDelegate, &FocusManager::OnWindowFocusChanged);
  }
}

FocusManager::~FocusManager()
{
}

void FocusManager::GetConfiguration()
{
  auto uiConfigManager = Integration::UiConfigManager::Get();
  if(uiConfigManager.IsInitialized())
  {
    mAlwaysShowIndicator = uiConfigManager.GetConfig().IsFocusIndicatorAlwaysShown() ? ALWAYS_SHOW : NONE;
  }

  mIsFocusIndicatorShown = (mAlwaysShowIndicator == ALWAYS_SHOW) ? SHOW : HIDE;
  mClearFocusOnTouch     = (mIsFocusIndicatorShown == SHOW) ? false : true;
}

bool FocusManager::SetCurrentFocusView(View view)
{
  return DoSetCurrentFocusView(view, {Ui::FocusDevice::PROGRAMMATIC, ""});
}

bool FocusManager::DoSetCurrentFocusView(View view, const FocusChangeContext& context)
{
  if(mIsFocusIndicatorShown == UNKNOWN)
  {
    GetConfiguration();
  }

  bool                           success = false;
  Dali::Integration::SceneHolder currentWindow;

  // Check whether the view is in the stage and is keyboard focusable.
  if(view && view.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
     view.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED) &&
     view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) &&
     (currentWindow = Dali::Integration::SceneHolder::Get(view))) ///< Note : SceneHolder might not be valid even if view is connected to scene.
                                                                  ///         (e.g. Adaptor Stopped, SceneHolder removed but Scene is still alive)
  {
    // If the parent's KEYBOARD_FOCUSABLE_CHILDREN is false, it cannot have focus.
    Actor parent = view.GetParent();
    while(parent)
    {
      if(!parent.GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN))
      {
        DALI_LOG_DEBUG_INFO("Parent has KEYBOARD_FOCUSABLE_CHILDREN false\n");
        return false;
      }
      parent = parent.GetParent();
    }

    // If developer set focus on same view, doing nothing
    View currentFocusedView = GetCurrentFocusView();
    DALI_LOG_DEBUG_INFO("current focused view : [%p] new focused view : [%p]\n", currentFocusedView.GetObjectPtr(), view.GetObjectPtr());
    if(view == currentFocusedView)
    {
      return true;
    }

    if(currentWindow.GetRootLayer() != mCurrentFocusedWindow.GetHandle())
    {
      Layer rootLayer       = currentWindow.GetRootLayer();
      mCurrentFocusedWindow = rootLayer;
      mCurrentWindowId      = static_cast<uint32_t>(currentWindow.GetNativeId());
    }

    if((mIsFocusIndicatorShown == SHOW) && (mEnableFocusIndicator == ENABLE))
    {
      view.Add(GetFocusIndicatorView());
    }

    view.OffSceneSignal().Connect(mSlotDelegate, &FocusManager::OnSceneDisconnection);

    // Save the current focused view
    mCurrentFocusView = view;

    // Save the last focus change context
    mLastFocusChangeContext = context;

    bool focusedWindowFound = false;
    for(unsigned int i = 0; i < mCurrentFocusViews.size(); i++)
    {
      if(mCurrentFocusViews[i].first == mCurrentFocusedWindow)
      {
        mCurrentFocusViews[i].second = view;
        focusedWindowFound           = true;
        break;
      }
    }
    if(!focusedWindowFound)
    {
      // A new window gains the focus, so store the focused view in that window.
      mCurrentFocusViews.push_back(std::pair<WeakHandle<Layer>, WeakHandle<View>>(mCurrentFocusedWindow, view));
    }

    // Send notification for the change of focus view
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(currentFocusedView, view);
    }

    if(currentFocusedView && currentFocusedView.IsOnScene())
    {
      Internal::KeyInputFocusManager::Get().RemoveFocus(currentFocusedView);
    }

    if(view && view.IsOnScene())
    {
      Internal::KeyInputFocusManager::Get().SetFocus(view);
    }

    // Push Current Focused View to FocusHistory
    mFocusHistory.push_back(view);

    // Delete first element before add new element when Stack is full.
    if(mFocusHistory.size() > MAX_HISTORY_AMOUNT)
    {
      FocusStackIterator beginPos = mFocusHistory.begin();
      mFocusHistory.erase(beginPos);
    }

    DALI_LOG_INFO(gLogFilter, Debug::General, "[%s:%d] SUCCEED\n", __FUNCTION__, __LINE__);
    success = true;
  }
  else
  {
    DALI_LOG_WARNING("[%s:%d] FAILED\n", __FUNCTION__, __LINE__);
  }

  return success;
}

View FocusManager::GetCurrentFocusView()
{
  View view = mCurrentFocusView.GetHandle();

  if(view && !view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    // If the view has been removed from the stage, then it should not be focused
    view.Reset();
    mCurrentFocusView.Reset();
  }
  return view;
}

View FocusManager::GetFocusViewFromCurrentWindow()
{
  View         view;
  unsigned int index;
  for(index = 0; index < mCurrentFocusViews.size(); index++)
  {
    if(mCurrentFocusViews[index].first == mCurrentFocusedWindow)
    {
      view = mCurrentFocusViews[index].second.GetHandle();
      break;
    }
  }

  if(view && !view.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    // If the view has been removed from the window, then the window doesn't have any focused view
    view.Reset();
    mCurrentFocusViews.erase(mCurrentFocusViews.begin() + index);
  }

  return view;
}

View FocusManager::GetCurrentFocusGroup()
{
  return GetFocusGroup(GetCurrentFocusView());
}

void FocusManager::MoveFocusBackward()
{
  // Find Pre Focused View when the list size is more than 1
  if(mFocusHistory.size() > 1)
  {
    // Delete current focused view in history
    mFocusHistory.pop_back();

    // If pre-focused views are not on stage or deleted, remove them in stack
    while(mFocusHistory.size() > 0)
    {
      // Get pre focused view
      View target = mFocusHistory[mFocusHistory.size() - 1].GetHandle();

      if(target && target.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        // Delete pre focused view in history because it will pushed again by SetCurrentFocusView()
        mFocusHistory.pop_back();
        SetCurrentFocusView(target);
        break;
      }
      else
      {
        // Target is empty handle or off stage. Erase from queue
        mFocusHistory.pop_back();
      }
    }

    // if there is no view which can get focus, then push current focus view in stack again
    if(mFocusHistory.size() == 0)
    {
      View currentFocusedView = GetCurrentFocusView();
      mFocusHistory.push_back(currentFocusedView);
    }
  }
}

bool FocusManager::IsLayoutView(View view) const
{
  return view && GetImpl(view).IsKeyNavigationSupported();
}

Ui::View FocusManager::GetParentLayoutView(View view) const
{
  // Get the view's parent layout view that supports two dimensional keyboard navigation
  Actor rootActor;
  Actor parent;
  if(view)
  {
    Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(view);
    if(window)
    {
      rootActor = window.GetRootLayer();
    }

    parent = view.GetParent();
  }

  while(parent && !IsLayoutView(View::DownCast(parent)) && parent != rootActor)
  {
    parent = parent.GetParent();
  }

  return View::DownCast(parent);
}

Ui::FocusDevice FocusManager::ConvertDeviceClassToKeyboardFocusDevice(Device::Class::Type deviceClass) const
{
  switch(deviceClass)
  {
    case Dali::Device::Class::KEYBOARD:
      return Ui::FocusDevice::KEYBOARD;
    case Dali::Device::Class::MOUSE:
      return Ui::FocusDevice::MOUSE;
    case Dali::Device::Class::TOUCH:
      return Ui::FocusDevice::TOUCH;
    case Dali::Device::Class::PEN:
      return Ui::FocusDevice::PEN;
    case Dali::Device::Class::POINTER:
      return Ui::FocusDevice::POINTER;
    case Dali::Device::Class::GAMEPAD:
      return Ui::FocusDevice::GAMEPAD;
    default:
      return Ui::FocusDevice::UNKNOWN;
  }
}

bool FocusManager::MoveFocus(Ui::FocusDirection direction, const Dali::String& deviceName)
{
  return MoveFocus(direction, {Ui::FocusDevice::PROGRAMMATIC, deviceName});
}

bool FocusManager::MoveFocus(Ui::FocusDirection direction, const FocusChangeContext& context)
{
  View currentFocusView = GetCurrentFocusView();

  bool succeed = false;

  // Go through the view's hierarchy until we find a layout view that knows how to move the focus
  Ui::View layoutView = IsLayoutView(currentFocusView) ? currentFocusView : GetParentLayoutView(currentFocusView);
  while(layoutView && !succeed)
  {
    succeed    = DoMoveFocusWithinLayoutView(layoutView, currentFocusView, direction, context);
    layoutView = GetParentLayoutView(layoutView);
  }

  if(!succeed)
  {
    View nextFocusableView;

    // If the current focused view is valid, then find the next focusable view via the focusable properties.
    if(currentFocusView)
    {
      int             viewId = -1;
      Property::Index index  = Property::INVALID_INDEX;
      Property::Value value;

      // Find property index based upon focus direction
      switch(direction)
      {
        case Ui::FocusDirection::LEFT:
        {
          index = Ui::View::Property::LEFT_FOCUSABLE_VIEW_ID;
          break;
        }
        case Ui::FocusDirection::RIGHT:
        {
          index = Ui::View::Property::RIGHT_FOCUSABLE_VIEW_ID;
          break;
        }
        case Ui::FocusDirection::UP:
        {
          index = Ui::View::Property::UP_FOCUSABLE_VIEW_ID;
          break;
        }
        case Ui::FocusDirection::DOWN:
        {
          index = Ui::View::Property::DOWN_FOCUSABLE_VIEW_ID;
          break;
        }
        case Ui::FocusDirection::CLOCKWISE:
        {
          index = Ui::View::Property::CLOCKWISE_FOCUSABLE_VIEW_ID;
          break;
        }
        case Ui::FocusDirection::COUNTER_CLOCKWISE:
        {
          index = Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID;
          break;
        }
        default:
          break;
      }

      // If the focusable property is set then determine next focusable view
      if(index != Property::INVALID_INDEX)
      {
        value  = currentFocusView.GetProperty(index);
        viewId = value.Get<int>();

        // If view's id is valid then find view from id. The view should be on the stage.
        if(viewId != -1)
        {
          if(currentFocusView.GetParent())
          {
            nextFocusableView = View::DownCast(currentFocusView.GetParent().FindChildById(viewId));
          }

          if(!nextFocusableView)
          {
            Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(currentFocusView);
            if(window)
            {
              nextFocusableView = View::DownCast(window.GetRootLayer().FindChildById(viewId));
            }
          }
        }
      }
    }

    if(!nextFocusableView)
    {
      if(mEnableDefaultAlgorithm)
      {
        Actor rootActor = mFocusFinderRootView.GetHandle();
        if(!rootActor)
        {
          if(currentFocusView)
          {
            // Find the window of the focused view.
            Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(currentFocusView);
            if(window)
            {
              rootActor = window.GetRootLayer();
            }
          }
          else
          {
            // Searches from the currently focused window.
            rootActor = mCurrentFocusedWindow.GetHandle();
          }
        }
        if(rootActor)
        {
          // We should find it among the views nearby.
          nextFocusableView = FocusFinder::GetNearestFocusableView(rootActor, currentFocusView, direction);
        }
      }
    }

    if(nextFocusableView && nextFocusableView.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
       nextFocusableView.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED))
    {
      // Whether the next focusable view is a layout view
      if(IsLayoutView(nextFocusableView))
      {
        // If so, move the focus inside it.
        succeed = DoMoveFocusWithinLayoutView(nextFocusableView, currentFocusView, direction, context);
      }
      if(!succeed)
      {
        // Just set focus to the next focusable view
        succeed = DoSetCurrentFocusView(nextFocusableView, context);
      }
    }
  }

  return succeed;
}

bool FocusManager::DoMoveFocusWithinLayoutView(Ui::View layoutView, View view, Ui::FocusDirection direction, const FocusChangeContext& context)
{
  // Ask the layout view for the next view to focus
  View nextFocusableView = GetImpl(layoutView).GetNextFocusableView(view, direction, mFocusGroupLoopEnabled);
  if(nextFocusableView)
  {
    if(!(nextFocusableView.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) ||
         nextFocusableView.GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED)))
    {
      // If the view is not focusable, ask the same layout view for the next view to focus
      return DoMoveFocusWithinLayoutView(layoutView, nextFocusableView, direction, context);
    }
    else
    {
      // Whether the next focusable view is a layout view
      if(IsLayoutView(nextFocusableView) && nextFocusableView != layoutView)
      {
        // If so, move the focus inside it.
        return DoMoveFocusWithinLayoutView(nextFocusableView, GetCurrentFocusView(), direction, context);
      }
      else
      {
        // Inform the layout view we will move the focus to what the view returns.
        GetImpl(layoutView).NotifyFocusChangeCommitted(nextFocusableView);
        return DoSetCurrentFocusView(nextFocusableView, context);
      }
    }
  }
  else
  {
    // No more view can be focused in the given direction within the same layout view.
    return false;
  }
}

bool FocusManager::DoMoveFocusToNextFocusGroup(bool forward, const FocusChangeContext& context)
{
  bool succeed = false;

  // Get the parent layout view of the current focus group
  Ui::View parentLayoutView = GetParentLayoutView(GetCurrentFocusGroup());

  while(parentLayoutView && !succeed)
  {
    // If the current focus group has a parent layout view, we can probably automatically
    // move the focus to the next focus group in the forward or backward direction.
    Ui::FocusDirection direction = forward ? Ui::FocusDirection::RIGHT : Ui::FocusDirection::LEFT;
    succeed                      = DoMoveFocusWithinLayoutView(parentLayoutView, GetCurrentFocusView(), direction, context);
    parentLayoutView             = GetParentLayoutView(parentLayoutView);
  }

  if(!mFocusGroupChangedSignal.Empty())
  {
    // Emit a focus group changed signal. The applicaton can move the focus to a new focus group
    mFocusGroupChangedSignal.Emit(GetCurrentFocusView(), forward);
  }

  return succeed;
}

void FocusManager::ClearFocus(View view)
{
  // Reset context for this system-triggered focus loss.
  mLastFocusChangeContext = {};

  if(view)
  {
    DALI_LOG_RELEASE_INFO("ClearFocus id:(%d)\n", view.GetProperty<int32_t>(Dali::Actor::Property::ID));
    view.OffSceneSignal().Disconnect(mSlotDelegate, &FocusManager::OnSceneDisconnection);

    // Remove the view from mCurrentFocusViews if present
    for(auto iter = mCurrentFocusViews.begin(); iter != mCurrentFocusViews.end(); ++iter)
    {
      if(iter->first == mCurrentFocusedWindow && iter->second.GetHandle() == view)
      {
        mCurrentFocusViews.erase(iter);
        break;
      }
    }

    // Send notification for the change of focus view
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(view, Ui::View());
    }

    if(view.IsOnScene())
    {
      Internal::KeyInputFocusManager::Get().RemoveFocus(view);
    }
  }
  mCurrentFocusView.Reset();
}

void FocusManager::ClearFocusIndicator(View view)
{
  if(view)
  {
    if(mFocusIndicatorView)
    {
      view.Remove(mFocusIndicatorView);
    }
  }
  mIsFocusIndicatorShown = (mAlwaysShowIndicator == ALWAYS_SHOW) ? SHOW : HIDE;
}

void FocusManager::ClearFocus()
{
  View view = GetCurrentFocusView();
  ClearFocusIndicator(view);
  ClearFocus(view);
}

void FocusManager::SetFocusGroupLoop(bool enabled)
{
  mFocusGroupLoopEnabled = enabled;
}

bool FocusManager::GetFocusGroupLoop() const
{
  return mFocusGroupLoopEnabled;
}

void FocusManager::SetAsFocusGroup(View view, bool isFocusGroup)
{
  if(view)
  {
    GetImpl(view).SetAsFocusGroup(isFocusGroup);
  }
}

bool FocusManager::IsFocusGroup(View view) const
{
  if(view)
  {
    return GetImpl(view).IsFocusGroup();
  }
  return false;
}

View FocusManager::GetFocusGroup(View view)
{
  // Go through the view's hierarchy to check which focus group the view belongs to
  Actor actor = view;
  while(actor && !IsFocusGroup(View::DownCast(actor)))
  {
    actor = actor.GetParent();
  }

  return View::DownCast(actor);
}

void FocusManager::SetFocusIndicatorActor(View indicator)
{
  if(mFocusIndicatorView != indicator)
  {
    View currentFocusView = GetCurrentFocusView();
    if(currentFocusView)
    {
      // The new focus indicator should be added to the current focused view immediately
      if(mFocusIndicatorView)
      {
        currentFocusView.Remove(mFocusIndicatorView);
      }

      if(indicator)
      {
        currentFocusView.Add(indicator);
      }
    }

    mFocusIndicatorView = indicator;
  }
}

View FocusManager::GetFocusIndicatorView()
{
  if(!mFocusIndicatorView)
  {
    // Create the default if it hasn't been set and one that's shared by all the keyboard focusable views
    const std::string imageDirPath = AssetManager::GetDaliImagePath();
    mFocusIndicatorView            = Ui::ImageView::New()
                            .SetResourceUrl(Dali::Integration::ToDaliString(imageDirPath + FOCUS_BORDER_IMAGE_FILE_NAME))
                            .SetFittingMode(FittingMode::FILL)
                            .SetRequestedWidth(MATCH_PARENT)
                            .SetRequestedHeight(MATCH_PARENT)
                            .SetLayoutMode(LayoutMode::STANDALONE);
  }

  return mFocusIndicatorView;
}

uint32_t FocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

void FocusManager::OnKeyEvent(const KeyEvent& event)
{
  if(mCurrentFocusedWindow.GetHandle())
  {
    // If it is a key event that occurred in another window, it returns.
    uint32_t eventWindowId = event.GetWindowId();
    if(eventWindowId > 0 && GetCurrentWindowId() != eventWindowId)
    {
      DALI_LOG_RELEASE_INFO("CurrentFocusedWindow id %d, window ID where key event occurred %d : key event skip\n", GetCurrentWindowId(), eventWindowId);
      return;
    }
  }

  const Dali::String& keyName        = event.GetKeyName();
  const Dali::String& logicalKeyName = event.GetLogicalKey();
  Ui::FocusDevice     device         = Ui::FocusDevice::KEYBOARD;
  FocusChangeContext  context        = {device, event.GetDeviceName(), Ui::InputEvent::New(event)};

  if(mIsFocusIndicatorShown == UNKNOWN)
  {
    GetConfiguration();
  }

  bool isFocusStartableKey = false;

  if(event.GetState() == KeyEvent::DOWN)
  {
    if(keyName == KEY_NAME_LEFT || logicalKeyName == LOGICAL_KEY_NAME_KP_LEFT)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards left
        MoveFocus(Ui::FocusDirection::LEFT, context);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_RIGHT || logicalKeyName == LOGICAL_KEY_NAME_KP_RIGHT)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards right
        MoveFocus(Ui::FocusDirection::RIGHT, context);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_UP || logicalKeyName == LOGICAL_KEY_NAME_KP_UP)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards up
        MoveFocus(Ui::FocusDirection::UP, context);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_DOWN || logicalKeyName == LOGICAL_KEY_NAME_KP_DOWN)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards down
        MoveFocus(Ui::FocusDirection::DOWN, context);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_PRIOR || logicalKeyName == LOGICAL_KEY_NAME_KP_PRIOR)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards the previous page
        MoveFocus(Ui::FocusDirection::PAGE_UP, context);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_NEXT || logicalKeyName == LOGICAL_KEY_NAME_KP_NEXT)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Move the focus towards the next page
        MoveFocus(Ui::FocusDirection::PAGE_DOWN, context);
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_TAB)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // "Tab" key changes the focus group in the forward direction and
        // "Shift-Tab" key changes it in the backward direction.
        if(!DoMoveFocusToNextFocusGroup(!event.IsShiftModifier(), context))
        {
          // If the focus group is not changed, Move the focus towards forward, "Shift-Tap" key moves the focus towards backward.
          MoveFocus(event.IsShiftModifier() ? Ui::FocusDirection::BACKWARD : Ui::FocusDirection::FORWARD, context);
        }
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_SPACE)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_EMPTY)
    {
      // Check the fake key event for evas-plugin case
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }

      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_BACKSPACE)
    {
      // Emit signal to go back to the previous view???
    }
    else if(keyName == KEY_NAME_ESCAPE)
    {
    }
  }
  else if(event.GetState() == KeyEvent::UP)
  {
    if(keyName == KEY_NAME_RETURN || logicalKeyName == LOGICAL_KEY_NAME_KP_ENTER)
    {
      if(mIsFocusIndicatorShown == HIDE)
      {
        // Show focus indicator
        mIsFocusIndicatorShown = SHOW;
      }
      else
      {
        // Enter key press on focused view is handled by the key event signal, not by FocusManager.
      }

      isFocusStartableKey = true;
    }
  }

  if(isFocusStartableKey && mIsFocusIndicatorShown == SHOW)
  {
    View focusedView = GetCurrentFocusView();
    if(focusedView)
    {
      if(mEnableFocusIndicator == ENABLE)
      {
        // Make sure the focused view is highlighted
        focusedView.Add(GetFocusIndicatorView());
      }
    }
    else if(!mEnableDefaultAlgorithm)
    {
      // No view is focused but keyboard focus is activated by the key press
      // Let's try to move the initial focus
      MoveFocus(Ui::FocusDirection::RIGHT, context);
    }
  }
}

void FocusManager::OnTouch(const TouchEvent& touch)
{
  // if mIsFocusIndicatorShown is UNKNOWN, it means Configuration is not loaded.
  // Try to load configuration.
  if(mIsFocusIndicatorShown == UNKNOWN)
  {
    GetConfiguration();
  }

  // Clear the focus when user touch the screen.
  // We only do this on a Down event, otherwise the clear action may override a manually focused actor.
  if(((touch.GetPointCount() < 1) || (touch.GetState(0) == PointState::DOWN)))
  {
    Ui::FocusDevice device = ConvertDeviceClassToKeyboardFocusDevice(touch.GetDeviceClass(0));

    // If you touch the currently focused view again, you don't need to do SetCurrentFocusView again.
    View hitView = View::DownCast(touch.GetHitActor(0));
    if(hitView && hitView == GetCurrentFocusView())
    {
      return;
    }
    // If mClearFocusOnTouch is false, do not clear the focus indicator even if user touch the screen.
    if(mClearFocusOnTouch)
    {
      ClearFocusIndicator(GetCurrentFocusView());
    }

    // If KEYBOARD_FOCUSABLE and TOUCH_FOCUSABLE is true, set focus view
    if(hitView && hitView.GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE) &&
       hitView.GetProperty<bool>(DevelActor::Property::TOUCH_FOCUSABLE))
    {
      DoSetCurrentFocusView(hitView, {device, touch.GetDeviceName(0), Ui::InputEvent::New(touch)});
    }
  }
}

void FocusManager::OnWheelEvent(const WheelEvent& event)
{
  if(event.GetType() == Dali::WheelEvent::CUSTOM_WHEEL)
  {
    Ui::FocusDirection direction = (event.GetDelta() > 0) ? Ui::FocusDirection::CLOCKWISE : Ui::FocusDirection::COUNTER_CLOCKWISE;
    // Move the focus
    MoveFocus(direction, {Ui::FocusDevice::WHEEL, "", Ui::InputEvent::New(event)});
  }
}

bool FocusManager::OnCustomWheelEvent(const WheelEvent& event)
{
  bool consumed = false;
  View view     = GetCurrentFocusView();
  if(view)
  {
    // Notify the view about the wheel event
    consumed = EmitCustomWheelSignals(view, event);
  }
  return consumed;
}

bool FocusManager::EmitCustomWheelSignals(View view, const WheelEvent& event)
{
  bool consumed = false;

  if(view)
  {
    Dali::Actor oldParent(view.GetParent());

    // Only do the conversion and emit the signal if the view's wheel signal has connections.
    if(!view.WheelEventSignal().Empty())
    {
      // Emit the signal to the parent
      consumed = view.WheelEventSignal().Emit(view, event);
    }
    // if view doesn't consume WheelEvent, give WheelEvent to its parent.
    if(!consumed)
    {
      // The view may have been removed/reparented during the signal callbacks.
      Dali::Actor parent = view.GetParent();

      if(parent && (parent == oldParent))
      {
        consumed = EmitCustomWheelSignals(View::DownCast(parent), event);
      }
    }
  }

  return consumed;
}

void FocusManager::OnWindowFocusChanged(Window window, bool focusIn)
{
  if(focusIn && mCurrentFocusedWindow.GetHandle() != window.GetRootLayer())
  {
    // Change Current Focused Window
    Layer rootLayer       = window.GetRootLayer();
    mCurrentFocusedWindow = rootLayer;
    mCurrentWindowId      = static_cast<uint32_t>(Dali::Integration::SceneHolder::Get(rootLayer).GetNativeId());

    // Get Current Focused View from window
    View currentFocusedView = GetFocusViewFromCurrentWindow();
    if(currentFocusedView)
    {
      SetCurrentFocusView(currentFocusedView);

      if(mEnableFocusIndicator == ENABLE)
      {
        // Make sure the focused view is highlighted
        currentFocusedView.Add(GetFocusIndicatorView());
        mIsFocusIndicatorShown = SHOW;
      }
    }
  }
}

void FocusManager::OnSceneHolderFocusChanged(Dali::Integration::SceneHolder sceneHolder, bool focusIn)
{
  Window window = Window::DownCast(sceneHolder);
  if(window)
  {
    if(!focusIn && mCurrentFocusedWindow.GetHandle() == window.GetRootLayer() && mClearFocusOnWindowFocusLost)
    {
      mCurrentFocusedWindow.Reset();
      ClearFocus();
    }
  }
}

Ui::FocusManager::FocusChangedSignalType& FocusManager::FocusChangedSignal()
{
  return mFocusChangedSignal;
}

Ui::FocusManager::FocusGroupChangedSignalType& FocusManager::FocusGroupChangedSignal()
{
  return mFocusGroupChangedSignal;
}

const FocusManager::FocusChangeContext& FocusManager::FocusChangedContext() const
{
  return mLastFocusChangeContext;
}

bool FocusManager::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor)
{
  Dali::BaseHandle handle(object);

  bool          connected(true);
  FocusManager* manager = static_cast<FocusManager*>(object); // TypeRegistry guarantees that this is the correct type.

  if(0 == strcmp(signalName.CStr(), SIGNAL_FOCUS_CHANGED))
  {
    manager->FocusChangedSignal().Connect(tracker, functor);
  }
  else if(0 == strcmp(signalName.CStr(), SIGNAL_FOCUS_GROUP_CHANGED))
  {
    manager->FocusGroupChangedSignal().Connect(tracker, functor);
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void FocusManager::EnableFocusIndicator(bool enable)
{
  if(!enable && mFocusIndicatorView)
  {
    mFocusIndicatorView.Unparent();
  }

  mEnableFocusIndicator = enable ? ENABLE : DISABLE;
}

bool FocusManager::IsFocusIndicatorEnabled() const
{
  return (mEnableFocusIndicator == ENABLE);
}

void FocusManager::EnableDefaultAlgorithm(bool enable)
{
  mEnableDefaultAlgorithm = enable;
}

bool FocusManager::IsDefaultAlgorithmEnabled() const
{
  return mEnableDefaultAlgorithm;
}

void FocusManager::SetFocusFinderRootView(View view)
{
  mFocusFinderRootView = view;
}

void FocusManager::ResetFocusFinderRootView()
{
  mFocusFinderRootView.Reset();
}

void FocusManager::SetClearFocusOnWindowFocusLost(bool enabled)
{
  mClearFocusOnWindowFocusLost = enabled;
}

bool FocusManager::GetClearFocusOnWindowFocusLost() const
{
  return mClearFocusOnWindowFocusLost;
}

void FocusManager::OnSceneDisconnection(Dali::Actor actor)
{
  View view = View::DownCast(actor);
  if(view && view == mCurrentFocusView.GetHandle())
  {
    DALI_LOG_RELEASE_INFO("ClearFocus due to view id:(%d) removed from scene\n", view.GetProperty<int32_t>(Dali::Actor::Property::ID));
    ClearFocusIndicator(view);
    ClearFocus(view);
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
