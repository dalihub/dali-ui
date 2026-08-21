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
#include <dali/integration-api/adaptor-framework/focused-actor-provider.h>
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
#include <dali-ui-foundation/extension-api/ui-config-impl.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/asset-manager/asset-manager.h>
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/internal/focus-manager/focus-finder.h>
#include <dali-ui-foundation/internal/focus-manager/focus-navigation-context-impl.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace ExtensionView   = Dali::Ui::Extension::View;
namespace IntegrationView = Dali::Ui::Integration::View;

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

bool IsDefaultFocusIndicatorSuppressedByStateEffect(View view)
{
  return view && ViewDataImpl::Get(GetImpl(view)).IsDefaultFocusIndicatorSuppressedByStateEffect();
}

bool IsScreenPointInsideView(View view, const Vector2& screenPosition)
{
  if(!view)
  {
    return false;
  }

  float localX = 0.0f;
  float localY = 0.0f;
  if(!view.ScreenToLocal(localX, localY, screenPosition.x, screenPosition.y))
  {
    return false;
  }

  const Vector3 size = view.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  return localX >= 0.0f && localY >= 0.0f && localX <= size.x && localY <= size.y;
}

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

DALI_TYPE_REGISTRATION_END()

const unsigned int MAX_HISTORY_AMOUNT = 30; ///< Max length of focus history stack

} // unnamed namespace

class FocusedActorProviderImpl final : public Dali::Integration::FocusedActorProvider
{
public:
  explicit FocusedActorProviderImpl(FocusManager& focusManager)
  : mFocusManager(focusManager)
  {
  }

  Dali::Actor GetFocusedActor() override
  {
    return mFocusManager.GetCurrentFocusView();
  }

private:
  FocusManager& mFocusManager;
};

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
: mFocusedActorProvider(std::make_unique<FocusedActorProviderImpl>(*this)),
  mFocusChangedSignal(),
  mCurrentFocusView(),
  mTouchFocusCandidate(),
  mFocusIndicatorView(),

  mFocusHistory(),
  mSlotDelegate(this),
  mCurrentFocusedWindow(),
  mLastFocusChangeContext(),
  mFocusIndicationPolicy(&Extension::FocusIndicationPolicy::Default),
  mFocusNavigationFallback(),
  mCurrentWindowId(0),
  mTouchFocusDeviceId(-1),
  mNavigationInProgress(false),
  mDefaultFocusIndicatorEnabled(true),
  mClearFocusIndicationOnTouch(true),
  mClearFocusIndicationOnHover(false),
  mConfigurationLoaded(false),
  mEnableDefaultAlgorithm(true),
  mClearFocusOnWindowFocusLost(true)
{
  Dali::Integration::RegisterFocusedActorProvider(mFocusedActorProvider.get());
  LifecycleController::Get().PreInitSignal().Connect(mSlotDelegate, &FocusManager::OnAdaptorInit);
  ScrollStateObserver::Get().DragStartedSignal().Connect(mSlotDelegate, &FocusManager::ClearTouchFocusCandidate);
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
      (*iter).TouchEventSignal().Connect(mSlotDelegate, &FocusManager::OnTouch);
      (*iter).GetRootLayer().HoverEventSignal().Connect(mSlotDelegate, &FocusManager::OnHover);
      (*iter).WheelEventGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnCustomWheelEvent);
      (*iter).WheelEventSignal().Connect(mSlotDelegate, &FocusManager::OnWheelEvent);
      (*iter).FocusChangedGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneHolderFocusChanged);
      Window window = Window::DownCast(*iter);
      if(window)
      {
        window.FocusChangedSignal().Connect(mSlotDelegate, &FocusManager::OnWindowFocusChanged);
      }
    }

    // Get notified when any new scene holder is created afterwards
    Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneHolderCreated);
  }
}

void FocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder sceneHolder)
{
  sceneHolder.KeyEventSignal().Connect(mSlotDelegate, &FocusManager::OnKeyEvent);
  sceneHolder.TouchEventSignal().Connect(mSlotDelegate, &FocusManager::OnTouch);
  sceneHolder.GetRootLayer().HoverEventSignal().Connect(mSlotDelegate, &FocusManager::OnHover);
  sceneHolder.WheelEventGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnCustomWheelEvent);
  sceneHolder.WheelEventSignal().Connect(mSlotDelegate, &FocusManager::OnWheelEvent);
  sceneHolder.FocusChangedGeneratedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneHolderFocusChanged);
  Window window = Window::DownCast(sceneHolder);
  if(window)
  {
    window.FocusChangedSignal().Connect(mSlotDelegate, &FocusManager::OnWindowFocusChanged);
  }
}

FocusManager::~FocusManager()
{
  Dali::Integration::UnregisterFocusedActorProvider(mFocusedActorProvider.get());
}

void FocusManager::GetConfiguration()
{
  if(UiConfig::HasCurrent())
  {
    const UiConfig config         = UiConfig::GetCurrent();
    mClearFocusIndicationOnTouch  = config.IsClearFocusIndicationOnTouchEnabled();
    mClearFocusIndicationOnHover  = config.IsClearFocusIndicationOnHoverEnabled();
    mDefaultFocusIndicatorEnabled = config.IsDefaultFocusIndicatorEnabled();
    mFocusIndicationPolicy        = GetImpl(config).GetFocusIndicationPolicy();
  }
  mConfigurationLoaded = true;
}

bool FocusManager::SetCurrentFocusView(View view)
{
  return SetCurrentFocusView(view, Ui::InputEvent::Programmatic());
}

bool FocusManager::SetCurrentFocusView(View view, InputEvent cause)
{
  if(mNavigationInProgress)
  {
    DALI_LOG_WARNING("Focus cannot be changed from a focus navigation callback\n");
    return false;
  }
  return view && !view.HasAncestorBlockingFocus() && DoSetCurrentFocusView(view, {Ui::FocusDevice::PROGRAMMATIC, "", cause});
}

bool FocusManager::RequestFocus(View view)
{
  if(mNavigationInProgress)
  {
    DALI_LOG_WARNING("Focus cannot be changed from a focus navigation callback\n");
    return false;
  }

  if(!view)
  {
    return false;
  }

  View resolved = ViewDataImpl::Get(GetImpl(view)).RequestFocus();
  if(resolved)
  {
    return DoSetCurrentFocusView(resolved, {Ui::FocusDevice::PROGRAMMATIC, ""});
  }
  return false;
}

bool FocusManager::DoSetCurrentFocusView(View view, const FocusChangeContext& context)
{
  bool                           success = false;
  Dali::Integration::SceneHolder currentWindow;

  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  // Check whether the view is in the stage and is keyboard focusable.
  if(view && view.IsFocusable() && view.IsEnabled() && view.IsConnectedToScene() &&
     (currentWindow = Dali::Integration::SceneHolder::Get(view))) ///< Note : SceneHolder might not be valid even if view is connected to scene.
                                                                  ///         (e.g. Adaptor Stopped, SceneHolder removed but Scene is still alive)
  {
    // If developer set focus on same view, doing nothing
    View currentFocusedView = GetCurrentFocusView();
    DALI_LOG_DEBUG_INFO("current focused view : [%p] new focused view : [%p]\n", currentFocusedView.GetObjectPtr(), view.GetObjectPtr());
    if(view == currentFocusedView)
    {
      return true;
    }

    FocusChangeContext effectiveContext       = context;
    const bool         previousFocusIndicated = currentFocusedView && GetImpl(currentFocusedView).GetState().Contains(ViewState::FOCUS_INDICATED);
    const bool         proposedIndicated      = ShouldIndicateFocus(context, previousFocusIndicated);
    effectiveContext.focusIndicated           = mFocusIndicationPolicy({currentFocusedView, view, context.device, context.inputEvent, previousFocusIndicated, proposedIndicated});

    if(currentWindow.GetRootLayer() != mCurrentFocusedWindow.GetHandle())
    {
      Layer rootLayer       = currentWindow.GetRootLayer();
      mCurrentFocusedWindow = rootLayer;
      mCurrentWindowId      = static_cast<uint32_t>(currentWindow.GetNativeId());
    }

    view.SceneDisconnectedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneDisconnection);

    // Save the current focused view
    mCurrentFocusView = view;

    // Save the last focus change context before KeyInputFocusManager notifies Views.
    mLastFocusChangeContext = effectiveContext;

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

    if(currentFocusedView && currentFocusedView.IsConnectedToScene())
    {
      DetachFocusIndicator(currentFocusedView);
      Internal::KeyInputFocusManager::Get().RemoveFocus(currentFocusedView);
    }

    if(view.IsConnectedToScene())
    {
      Internal::KeyInputFocusManager::Get().SetFocus(view);
      RefreshFocusIndicator(view);
    }

    // Send notification for the change of focus view
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(currentFocusedView, view);
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

  if(view && !view.IsConnectedToScene())
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

  if(view && !view.IsConnectedToScene())
  {
    // If the view has been removed from the window, then the window doesn't have any focused view
    view.Reset();
    mCurrentFocusViews.erase(mCurrentFocusViews.begin() + index);
  }

  return view;
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

      if(target && target.IsConnectedToScene())
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
  if(mNavigationInProgress)
  {
    DALI_LOG_WARNING("Nested focus navigation is not allowed from a focus navigation callback\n");
    return false;
  }

  struct NavigationGuard
  {
    explicit NavigationGuard(bool& inProgress)
    : flag(inProgress)
    {
      flag = true;
    }

    ~NavigationGuard()
    {
      flag = false;
    }

    bool& flag;
  } guard(mNavigationInProgress);

  View                   currentFocusView  = GetCurrentFocusView();
  FocusNavigationContext navigationContext = CreateFocusNavigationContext(currentFocusView, direction, context);
  if(!navigationContext)
  {
    DALI_LOG_WARNING("Focus navigation failed because its Window could not be determined\n");
    return false;
  }

  FocusNavigationResult result = FindNextFocusByParentNavigation(currentFocusView, navigationContext).result;

  if(result.GetType() == FocusNavigationResultType::NOT_HANDLED)
  {
    result = FindNextFocusByProperty(currentFocusView, direction);
  }

  if(result.GetType() == FocusNavigationResultType::NOT_HANDLED && mFocusNavigationFallback)
  {
    result = mFocusNavigationFallback.Invoke(currentFocusView, navigationContext);
  }

  if(result.GetType() == FocusNavigationResultType::NOT_HANDLED && mEnableDefaultAlgorithm)
  {
    View candidate = FindNextFocusByFinder(currentFocusView, navigationContext);
    if(candidate)
    {
      result = FocusNavigationResult::MoveTo(candidate);
    }
  }

  return ApplyFocusNavigationResult(result, currentFocusView, navigationContext, context);
}

void FocusManager::SetFocusNavigationFallback(FocusNavigationCallback callback)
{
  if(mNavigationInProgress)
  {
    DALI_LOG_WARNING("The focus navigation fallback cannot be replaced while it is running\n");
    return;
  }
  mFocusNavigationFallback = std::move(callback);
}

FocusNavigationResult FocusManager::FindNextFocusByProperty(View currentFocusView, Ui::FocusDirection direction)
{
  if(!currentFocusView)
  {
    return FocusNavigationResult::NotHandled();
  }

  Property::Index index = Property::INVALID_INDEX;
  switch(direction)
  {
    case Ui::FocusDirection::LEFT:
      index = Ui::View::Property::LEFT_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::RIGHT:
      index = Ui::View::Property::RIGHT_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::UP:
      index = Ui::View::Property::UP_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::DOWN:
      index = Ui::View::Property::DOWN_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::CLOCKWISE:
      index = Ui::View::Property::CLOCKWISE_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::COUNTER_CLOCKWISE:
      index = Ui::View::Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::FORWARD:
      index = Ui::View::Property::FORWARD_FOCUSABLE_VIEW_ID;
      break;
    case Ui::FocusDirection::BACKWARD:
      index = Ui::View::Property::BACKWARD_FOCUSABLE_VIEW_ID;
      break;
    default:
      break;
  }

  if(index != Property::INVALID_INDEX)
  {
    int viewId = currentFocusView.GetProperty(index).Get<int>();
    if(viewId != -1)
    {
      View found;
      if(currentFocusView.GetParent())
      {
        found = View::DownCast(currentFocusView.GetParent().FindChildById(viewId));
      }
      if(!found)
      {
        Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(currentFocusView);
        if(window)
        {
          found = View::DownCast(window.GetRootLayer().FindChildById(viewId));
        }
      }
      return FocusNavigationResult::MoveTo(found);
    }
  }
  return FocusNavigationResult::NotHandled();
}

FocusManager::ParentNavigationResult FocusManager::FindNextFocusByParentNavigation(View currentFocusView, FocusNavigationContext context)
{
  ParentNavigationResult result;

  if(!currentFocusView)
  {
    return result;
  }

  Actor parent = currentFocusView.GetParent();
  while(parent)
  {
    View parentView = View::DownCast(parent);
    if(parentView)
    {
      result.result = ViewDataImpl::Get(GetImpl(parentView)).RequestFocusNavigation(currentFocusView, context);
      if(result.result.GetType() != FocusNavigationResultType::NOT_HANDLED)
      {
        return result;
      }

      if(parentView == context.GetFocusGroup())
      {
        break;
      }
    }
    parent = parent.GetParent();
  }
  return result;
}

FocusNavigationContext FocusManager::CreateFocusNavigationContext(View currentFocusView, Ui::FocusDirection direction, const FocusChangeContext& context)
{
  Window window = context.window;
  if(!window && currentFocusView)
  {
    window = Window::DownCast(Dali::Integration::SceneHolder::Get(currentFocusView));
  }
  if(!window && mCurrentFocusedWindow.GetHandle())
  {
    window = Window::DownCast(Dali::Integration::SceneHolder::Get(mCurrentFocusedWindow.GetHandle()));
  }
  if(!window)
  {
    return FocusNavigationContext();
  }

  FocusNavigationContextImplPtr impl(new FocusNavigationContextImpl(direction,
                                                                    context.device,
                                                                    context.deviceName,
                                                                    context.inputEvent,
                                                                    window,
                                                                    GetFocusGroup(currentFocusView)));
  return FocusNavigationContext(impl.Get());
}

bool FocusManager::ApplyFocusNavigationResult(const FocusNavigationResult& result, View originalFocusView, FocusNavigationContext context, const FocusChangeContext& changeContext)
{
  if(result.GetType() == FocusNavigationResultType::NOT_HANDLED ||
     result.GetType() == FocusNavigationResultType::STAY)
  {
    return false;
  }

  if(GetCurrentFocusView() != originalFocusView)
  {
    DALI_LOG_WARNING("Focus changed while a focus navigation policy was running\n");
    return false;
  }

  View candidate = result.GetCandidate();
  if(!IsValidNavigationCandidate(candidate, context))
  {
    DALI_LOG_WARNING("A focus navigation policy returned a candidate outside its allowed scope\n");
    return false;
  }

  View resolved = ViewDataImpl::Get(GetImpl(candidate)).RequestFocus();
  if(!IsValidNavigationCandidate(resolved, context) || resolved.HasAncestorBlockingFocus())
  {
    DALI_LOG_WARNING("A focus navigation candidate could not resolve to a valid focusable View\n");
    return false;
  }

  if(resolved == originalFocusView)
  {
    return false;
  }

  // FocusChangedSignal handlers retain their existing ability to issue a new
  // focus request after this navigation decision has been fully resolved.
  mNavigationInProgress = false;
  return DoSetCurrentFocusView(resolved, changeContext);
}

bool FocusManager::IsValidNavigationCandidate(View candidate, FocusNavigationContext context) const
{
  if(!candidate || !candidate.IsConnectedToScene())
  {
    return false;
  }

  Dali::Integration::SceneHolder candidateScene   = Dali::Integration::SceneHolder::Get(candidate);
  Window                         candidateWindow  = Window::DownCast(candidateScene);
  Window                         navigationWindow = context.GetWindow();
  if(!candidateWindow || !navigationWindow || candidateWindow.GetRootLayer() != navigationWindow.GetRootLayer())
  {
    return false;
  }

  View focusGroup = context.GetFocusGroup();
  if(focusGroup)
  {
    Actor actor = candidate;
    while(actor && actor != focusGroup)
    {
      actor = actor.GetParent();
    }
    if(actor != focusGroup)
    {
      return false;
    }
  }

  return true;
}

View FocusManager::FindNextFocusByFinder(View currentFocusView, FocusNavigationContext context)
{
  View  focusGroup = context.GetFocusGroup();
  Actor rootActor  = focusGroup ? Actor(focusGroup) : Actor();
  if(!rootActor)
  {
    if(currentFocusView)
    {
      Dali::Integration::SceneHolder window = Dali::Integration::SceneHolder::Get(currentFocusView);
      if(window)
      {
        rootActor = window.GetRootLayer();
      }
    }
    else
    {
      rootActor = context.GetWindow().GetRootLayer();
    }
  }

  if(rootActor)
  {
    Ui::FocusDirection direction = context.GetDirection();
    if(direction == Ui::FocusDirection::FORWARD || direction == Ui::FocusDirection::BACKWARD)
    {
      return FocusFinder::GetNextFocusableViewInOrder(rootActor, currentFocusView, direction);
    }
    else
    {
      return FocusFinder::GetNearestFocusableView(rootActor, currentFocusView, direction);
    }
  }
  return View();
}

void FocusManager::ClearFocus(View view)
{
  // Reset context for this system-triggered focus loss.
  mLastFocusChangeContext = {};

  if(view)
  {
    DALI_LOG_RELEASE_INFO("ClearFocus id:(%d)\n", view.GetProperty<int32_t>(Dali::Actor::Property::ID));
    view.SceneDisconnectedSignal().Disconnect(mSlotDelegate, &FocusManager::OnSceneDisconnection);

    // Remove the view from mCurrentFocusViews if present
    for(auto iter = mCurrentFocusViews.begin(); iter != mCurrentFocusViews.end(); ++iter)
    {
      if(iter->first == mCurrentFocusedWindow && iter->second.GetHandle() == view)
      {
        mCurrentFocusViews.erase(iter);
        break;
      }
    }

    if(view.IsConnectedToScene())
    {
      Internal::KeyInputFocusManager::Get().RemoveFocus(view);
    }

    // Send notification for the change of focus view
    if(!mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(view, Ui::View());
    }
  }
  mCurrentFocusView.Reset();
}

void FocusManager::DetachFocusIndicator(View view)
{
  if(view && mFocusIndicatorView)
  {
    view.Remove(mFocusIndicatorView);
  }
}

void FocusManager::SetFocusIndicated(View view, bool indicated, InputEvent cause)
{
  if(view)
  {
    const bool focused = GetImpl(view).GetState().Contains(ViewState::FOCUSED);
    ExtensionView::SetState(GetImpl(view), ViewState::FOCUS_INDICATED, indicated && focused, cause);
    RefreshFocusIndicator(view);
  }
}

void FocusManager::SetFocusIndicationWithPolicy(View focusedView, bool proposedIndicated, FocusDevice device, InputEvent inputEvent)
{
  if(focusedView)
  {
    const bool previousFocusIndicated = GetImpl(focusedView).GetState().Contains(ViewState::FOCUS_INDICATED);
    const bool indicated              = mFocusIndicationPolicy({focusedView, focusedView, device, inputEvent, previousFocusIndicated, proposedIndicated});
    SetFocusIndicated(focusedView, indicated, inputEvent);
  }
}

void FocusManager::ClearTouchFocusCandidate()
{
  mTouchFocusCandidate.Reset();
  mTouchFocusDeviceId = -1;
}

bool FocusManager::ShouldIndicateFocus(const FocusChangeContext& context, bool previousFocusIndicated) const
{
  switch(context.device)
  {
    case Ui::FocusDevice::KEYBOARD:
    case Ui::FocusDevice::GAMEPAD:
    case Ui::FocusDevice::WHEEL:
      return true;
    case Ui::FocusDevice::PROGRAMMATIC:
      return previousFocusIndicated;
    default:
      return false;
  }
}

void FocusManager::ClearFocus()
{
  View view = GetCurrentFocusView();
  DetachFocusIndicator(view);
  ClearFocus(view);
}

void FocusManager::ClearFocusIndication(InputEvent cause)
{
  View view = GetCurrentFocusView();
  if(view)
  {
    SetFocusIndicated(view, false, cause);
  }
}

void FocusManager::SetAsFocusGroup(View view, bool isFocusGroup)
{
  if(view)
  {
    ViewDataImpl::Get(GetImpl(view)).SetAsFocusGroup(isFocusGroup);
  }
}

bool FocusManager::IsFocusGroup(View view) const
{
  if(view)
  {
    return ViewDataImpl::Get(GetImpl(view)).IsFocusGroup();
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

View FocusManager::GetFocusIndicatorView()
{
  if(!mFocusIndicatorView)
  {
    // Create the default if it hasn't been set and one that's shared by all the keyboard focusable views
    const std::string imageDirPath        = Dali::Ui::Integration::AssetManager::GetDaliImagePath();
    Ui::ImageView     focusIndicatorImage = Ui::ImageView::New();
    focusIndicatorImage.SetResourceUrl(Dali::Integration::ToDaliString(imageDirPath + FOCUS_BORDER_IMAGE_FILE_NAME));
    focusIndicatorImage.SetFittingMode(Ui::Image::FittingMode::FILL);
    mFocusIndicatorView = focusIndicatorImage;
    mFocusIndicatorView.SetRequestedWidth(MATCH_PARENT);
    mFocusIndicatorView.SetRequestedHeight(MATCH_PARENT);
    mFocusIndicatorView.SetLayoutMode(LayoutMode::STANDALONE);
  }

  return mFocusIndicatorView;
}

uint32_t FocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

void FocusManager::OnKeyEvent(Dali::Integration::SceneHolder sceneHolder, KeyEvent event)
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
  FocusChangeContext  context        = {device, event.GetDeviceName(), Ui::InputEvent::New(event), Window::DownCast(sceneHolder)};

  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  bool isFocusStartableKey = false;
  bool navigationRequested = false;
  View focusViewBeforeKey  = GetCurrentFocusView();

  if(event.GetState() == KeyEvent::DOWN)
  {
    if(keyName == KEY_NAME_LEFT || logicalKeyName == LOGICAL_KEY_NAME_KP_LEFT)
    {
      // Move the focus towards left
      MoveFocus(Ui::FocusDirection::LEFT, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_RIGHT || logicalKeyName == LOGICAL_KEY_NAME_KP_RIGHT)
    {
      // Move the focus towards right
      MoveFocus(Ui::FocusDirection::RIGHT, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_UP || logicalKeyName == LOGICAL_KEY_NAME_KP_UP)
    {
      // Move the focus towards up
      MoveFocus(Ui::FocusDirection::UP, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_DOWN || logicalKeyName == LOGICAL_KEY_NAME_KP_DOWN)
    {
      // Move the focus towards down
      MoveFocus(Ui::FocusDirection::DOWN, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_PRIOR || logicalKeyName == LOGICAL_KEY_NAME_KP_PRIOR)
    {
      // Move the focus towards the previous page
      MoveFocus(Ui::FocusDirection::PAGE_UP, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_NEXT || logicalKeyName == LOGICAL_KEY_NAME_KP_NEXT)
    {
      // Move the focus towards the next page
      MoveFocus(Ui::FocusDirection::PAGE_DOWN, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_TAB)
    {
      // "Tab" key moves the focus in the forward direction,
      // "Shift-Tab" key moves it in the backward direction.
      MoveFocus(event.IsShiftModifier() ? Ui::FocusDirection::BACKWARD : Ui::FocusDirection::FORWARD, context);

      isFocusStartableKey = true;
      navigationRequested = true;
    }
    else if(keyName == KEY_NAME_SPACE)
    {
      isFocusStartableKey = true;
    }
    else if(keyName == KEY_NAME_EMPTY)
    {
      // Check the fake key event for evas-plugin case
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
      // Enter key press on focused view is handled by the key event signal, not by FocusManager.

      isFocusStartableKey = true;
    }
  }

  if(isFocusStartableKey)
  {
    View focusedView = GetCurrentFocusView();
    if(focusedView)
    {
      if(focusedView == focusViewBeforeKey)
      {
        SetFocusIndicationWithPolicy(focusedView, true, device, context.inputEvent);
      }
    }
    else if(!navigationRequested && !mEnableDefaultAlgorithm)
    {
      // No view is focused but keyboard focus is activated by the key press
      // Let's try to move the initial focus
      MoveFocus(Ui::FocusDirection::RIGHT, context);
    }
  }
}

void FocusManager::OnTouch(Dali::Integration::SceneHolder sceneHolder, TouchEvent touch)
{
  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  if(touch.GetPointCount() < 1)
  {
    ClearTouchFocusCandidate();
    return;
  }

  switch(touch.GetState(0))
  {
    case PointState::DOWN:
    {
      ClearTouchFocusCandidate();

      // If you touch the currently focused view again, you don't need to do SetCurrentFocusView again.
      View hitView = View::DownCast(touch.GetHitActor(0));
      if(mClearFocusIndicationOnTouch)
      {
        View focusedView = GetCurrentFocusView();
        SetFocusIndicationWithPolicy(focusedView, false, ConvertDeviceClassToKeyboardFocusDevice(touch.GetDeviceClass(0)), Ui::InputEvent::New(touch));
      }

      if(hitView && hitView == GetCurrentFocusView())
      {
        return;
      }

      // If FOCUSABLE and FOCUS_ON_TOUCH is true, set focus view on touch release.
      if(hitView && hitView.IsFocusable() && hitView.IsFocusOnTouchEnabled() && !hitView.HasAncestorBlockingFocus())
      {
        mTouchFocusCandidate = hitView;
        mTouchFocusDeviceId  = touch.GetDeviceId(0);
      }
      break;
    }
    case PointState::UP:
    {
      View candidate = mTouchFocusCandidate.GetHandle();
      if(candidate && touch.GetDeviceId(0) == mTouchFocusDeviceId)
      {
        View hitView = View::DownCast(touch.GetHitActor(0));
        if(hitView == candidate && candidate.IsFocusable() && candidate.IsFocusOnTouchEnabled() && !candidate.HasAncestorBlockingFocus())
        {
          Ui::FocusDevice device = ConvertDeviceClassToKeyboardFocusDevice(touch.GetDeviceClass(0));
          DoSetCurrentFocusView(candidate, {device, touch.GetDeviceName(0), Ui::InputEvent::New(touch)});
        }
      }
      ClearTouchFocusCandidate();
      break;
    }
    case PointState::INTERRUPTED:
    case PointState::LEAVE:
    {
      ClearTouchFocusCandidate();
      break;
    }
    default:
    {
      break;
    }
  }
}

bool FocusManager::OnHover(Actor actor, HoverEvent hover)
{
  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  if(!mClearFocusIndicationOnHover)
  {
    return false;
  }

  if(hover.GetPointCount() > 0u)
  {
    View focusedView     = GetCurrentFocusView();
    View hitView         = View::DownCast(hover.GetHitActor(0u));
    bool leftFocusedView = focusedView && ((hitView && !hitView.IsEffectivelyFocused()) || (!hitView && !IsScreenPointInsideView(focusedView, hover.GetScreenPosition(0u))));
    if(leftFocusedView)
    {
      SetFocusIndicationWithPolicy(focusedView, false, Ui::FocusDevice::MOUSE, Ui::InputEvent::New(hover));
    }
  }
  return false;
}

void FocusManager::OnWheelEvent(Dali::Integration::SceneHolder sceneHolder, WheelEvent event)
{
  if(event.GetType() == Dali::WheelEvent::CUSTOM_WHEEL)
  {
    Ui::FocusDirection direction = (event.GetDelta() > 0) ? Ui::FocusDirection::CLOCKWISE : Ui::FocusDirection::COUNTER_CLOCKWISE;
    // Move the focus
    MoveFocus(direction, {Ui::FocusDevice::WHEEL, "", Ui::InputEvent::New(event), Window::DownCast(sceneHolder)});
  }
}

bool FocusManager::OnCustomWheelEvent(Dali::Integration::SceneHolder sceneHolder, WheelEvent event)
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
      // Any connected callback consuming the event consumes it for all of them.
      consumed = view.WheelEventSignal().EmitOr(view, event);
      if(consumed)
      {
        DALI_LOG_RELEASE_INFO("[WheelEvent] delta(%d) consumed by View id(%d), name(%s) at View::WheelEventSignal\n",
                              event.GetDelta(),
                              view.GetProperty<int32_t>(Dali::Actor::Property::ID),
                              view.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
      }
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
      RefreshFocusIndicator(currentFocusedView);
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
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void FocusManager::SetDefaultFocusIndicatorEnabled(bool enabled)
{
  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  if(!enabled && mFocusIndicatorView)
  {
    mFocusIndicatorView.Unparent();
  }

  mDefaultFocusIndicatorEnabled = enabled;
  mConfigurationLoaded          = true;
  RefreshFocusIndicator(GetCurrentFocusView());
}

bool FocusManager::IsDefaultFocusIndicatorEnabled() const
{
  return mDefaultFocusIndicatorEnabled;
}

void FocusManager::EnableDefaultAlgorithm(bool enable)
{
  mEnableDefaultAlgorithm = enable;
}

bool FocusManager::IsDefaultAlgorithmEnabled() const
{
  return mEnableDefaultAlgorithm;
}

void FocusManager::SetClearFocusOnWindowFocusLost(bool enabled)
{
  mClearFocusOnWindowFocusLost = enabled;
}

bool FocusManager::GetClearFocusOnWindowFocusLost() const
{
  return mClearFocusOnWindowFocusLost;
}

void FocusManager::SetClearFocusIndicationOnTouch(bool clear)
{
  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  mClearFocusIndicationOnTouch = clear;
}

bool FocusManager::IsClearFocusIndicationOnTouchEnabled() const
{
  return mClearFocusIndicationOnTouch;
}

void FocusManager::SetClearFocusIndicationOnHover(bool clear)
{
  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }

  mClearFocusIndicationOnHover = clear;
}

bool FocusManager::IsClearFocusIndicationOnHoverEnabled() const
{
  return mClearFocusIndicationOnHover;
}

void FocusManager::RefreshFocusIndicator(View view)
{
  if(!view || view != GetCurrentFocusView())
  {
    return;
  }

  const bool focusIndicated = GetImpl(view).GetState().Contains(ViewState::FOCUS_INDICATED);
  if(mDefaultFocusIndicatorEnabled && focusIndicated && !IsDefaultFocusIndicatorSuppressedByStateEffect(view))
  {
    view.Add(GetFocusIndicatorView());
  }
  else
  {
    DetachFocusIndicator(view);
  }
}

void FocusManager::OnSceneDisconnection(Dali::Actor actor)
{
  View view = View::DownCast(actor);
  if(view && view == mCurrentFocusView.GetHandle())
  {
    DALI_LOG_RELEASE_INFO("ClearFocus due to view id:(%d) removed from scene\n", view.GetProperty<int32_t>(Dali::Actor::Property::ID));
    DetachFocusIndicator(view);
    ClearFocus(view);
  }
}

} // namespace Internal

} // namespace Ui

} // namespace Dali
