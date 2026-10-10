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
#include <algorithm>
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
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace ExtensionView   = Dali::Ui::Extension::View;
namespace IntegrationView = Dali::Ui::Integration::View;

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace // Unnamed namespace
{
static constexpr Dali::TypeInfoId gSingletonTypeInfoId = DALI_TYPE_ID(Ui::FocusManager);

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
      singletonService.Register(gSingletonTypeInfoId, manager);
      handle = manager;
    }
  }

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Ui::FocusManager, Dali::BaseHandle, Create, true)

DALI_SIGNAL_REGISTRATION(Ui, FocusManager, "focusChanged", SIGNAL_FOCUS_CHANGED)
DALI_SIGNAL_REGISTRATION(Ui, FocusManager, "windowFocusChanged", SIGNAL_WINDOW_FOCUS_CHANGED)

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
    Dali::BaseHandle handle = singletonService.GetSingleton(gSingletonTypeInfoId);
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
  ScrollStateObserver::Get().WindowDragStartedSignal().Connect(mSlotDelegate, &FocusManager::OnDragStarted);
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
  if(!mConfigurationLoaded)
  {
    GetConfiguration();
  }
  auto sceneHolder = view && view.IsConnectedToScene() ? Dali::Integration::SceneHolder::Get(view) : Dali::Integration::SceneHolder();
  if(!view || !view.IsFocusable() || !view.IsEnabled() || !sceneHolder)
  {
    return false;
  }
  Window     window      = Window::DownCast(sceneHolder);
  const bool independent = IsIndependentFocusEnabled(window);
  if(independent && !CanSetKeyInputFocus(view))
  {
    return false;
  }
  View               previous         = independent ? GetCurrentFocusView(window) : GetCurrentFocusView();
  FocusChangeContext effectiveContext = context;
  if(IsLogicalFocusWindow(sceneHolder) && view != previous)
  {
    const bool previousIndicated    = previous && GetImpl(previous).GetState().Contains(ViewState::FOCUS_INDICATED);
    effectiveContext.focusIndicated = mFocusIndicationPolicy({previous, view, context.device, context.inputEvent,
                                                              previousIndicated, ShouldIndicateFocus(context, previousIndicated)});
  }
  // Policy callbacks can disconnect the target or change its independent policy.
  if(!view.IsConnectedToScene() || Dali::Integration::SceneHolder::Get(view) != sceneHolder ||
     independent != IsIndependentFocusEnabled(window) || (independent && !CanSetKeyInputFocus(view)))
  {
    return false;
  }

  Layer root = sceneHolder.GetRootLayer();
  View  previousStored;
  bool  found = false;
  for(auto& entry : mCurrentFocusViews)
  {
    if(entry.first.GetHandle() == root)
    {
      previousStored = entry.second.GetHandle();
      entry.second   = view;
      found          = true;
      break;
    }
  }
  if(!found)
  {
    mCurrentFocusViews.emplace_back(root, view);
  }
  view.SceneDisconnectedSignal().Connect(mSlotDelegate, &FocusManager::OnStoredFocusViewDisconnection);
  DisconnectFocusViewIfUnused(previousStored);
  if(!IsLogicalFocusWindow(sceneHolder))
  {
    return true;
  }

  const bool global               = IsActiveWindow(sceneHolder);
  View       previousGlobal       = GetCurrentFocusView();
  View       previousLocal        = GetCurrentFocusView(window);
  Window     previousWindow       = previousGlobal ? Window::DownCast(Dali::Integration::SceneHolder::Get(previousGlobal)) : Window();
  const bool retainPreviousGlobal = previousWindow && previousWindow != window && IsIndependentFocusEnabled(previousWindow);
  const bool changed              = previousLocal != view;
  if(!changed && (!global || previousGlobal == view))
  {
    return true;
  }

  if(auto* state = FindIndependentState(window))
  {
    state->focus   = view;
    state->context = effectiveContext;
    state->context.window.Reset();
  }
  if(global)
  {
    mCurrentFocusedWindow   = root;
    mCurrentWindowId        = static_cast<uint32_t>(sceneHolder.GetNativeId());
    mCurrentFocusView       = view;
    mCurrentFocusWindow     = window;
    mLastFocusChangeContext = effectiveContext;
  }
  view.SceneDisconnectedSignal().Connect(mSlotDelegate, &FocusManager::OnSceneDisconnection);
  if(independent)
  {
    view.EffectiveVisibilityChangedSignal().Connect(mSlotDelegate, &FocusManager::OnIndependentViewVisibilityChanged);
  }

  auto keyManager = KeyInputFocusManager::Get();
  if(global && previousGlobal && previousGlobal != view && !retainPreviousGlobal)
  {
    DetachFocusIndicator(previousGlobal);
    keyManager.RemoveFocus(previousGlobal);
    DisconnectFocusViewIfUnused(previousGlobal);
  }
  if(independent && previousLocal && previousLocal != view && previousLocal != previousGlobal)
  {
    DetachFocusIndicator(previousLocal);
    keyManager.RemoveFocus(previousLocal);
    DisconnectFocusViewIfUnused(previousLocal);
  }
  if(GetCurrentFocusView(window) != view || (independent && !CanSetKeyInputFocus(view)))
  {
    return false;
  }

  FocusStack* history = &mFocusHistory;
  if(auto* state = FindIndependentState(window))
  {
    history = &state->history;
  }
  if(history->empty() || history->back().GetHandle() != view)
  {
    history->push_back(view);
    if(history->size() > MAX_HISTORY_AMOUNT)
    {
      history->erase(history->begin());
    }
  }
  if(changed)
  {
    keyManager.SetFocus(view);
  }
  else if(global)
  {
    keyManager.SetPrimaryWindow(window);
  }
  if(GetCurrentFocusView(window) != view)
  {
    return false;
  }
  RefreshFocusIndicator(view);
  if(global && previousWindow && previousWindow != window && previousGlobal && !retainPreviousGlobal)
  {
    mWindowFocusChangedSignal.Emit(previousWindow, previousGlobal, View());
  }
  if(changed && GetCurrentFocusView(window) == view)
  {
    mWindowFocusChangedSignal.Emit(window, previousLocal, view);
  }
  if(global && previousGlobal != view && GetCurrentFocusView() == view)
  {
    mFocusChangedSignal.Emit(previousGlobal, view);
  }
  return true;
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

bool FocusManager::IsActiveWindow(Dali::Integration::SceneHolder sceneHolder) const
{
  Window window = Window::DownCast(sceneHolder);
  if(!window || !window.IsFocused())
  {
    return false;
  }

  // A new focus-in may precede the old Window's focus-out notification.
  Layer  rootLayer     = mCurrentFocusedWindow.GetHandle();
  Window currentWindow = rootLayer ? Window::DownCast(Dali::Integration::SceneHolder::Get(rootLayer)) : Window();
  return !currentWindow || currentWindow == window || !currentWindow.IsFocused();
}

FocusManager::IndependentFocusState* FocusManager::FindIndependentState(Window window)
{
  for(auto& state : mIndependentFocusStates)
  {
    if(window && state.window.GetHandle() == window)
    {
      return &state;
    }
  }
  return nullptr;
}

const FocusManager::IndependentFocusState* FocusManager::FindIndependentState(Window window) const
{
  for(const auto& state : mIndependentFocusStates)
  {
    if(window && state.window.GetHandle() == window)
    {
      return &state;
    }
  }
  return nullptr;
}

FocusManager::IndependentFocusState* FocusManager::FindIndependentState(View view)
{
  if(view && view.IsConnectedToScene())
  {
    return FindIndependentState(Window::DownCast(Dali::Integration::SceneHolder::Get(view)));
  }
  for(auto& state : mIndependentFocusStates)
  {
    if(view && state.focus.GetHandle() == view)
    {
      return &state;
    }
  }
  return nullptr;
}

bool FocusManager::IsIndependentFocusEnabled(Window window) const
{
  const auto* state = FindIndependentState(window);
  return state && state->enabled;
}

bool FocusManager::CanSetKeyInputFocus(View view) const
{
  if(!view || !view.IsConnectedToScene())
  {
    return false;
  }
  Window window = Window::DownCast(Dali::Integration::SceneHolder::Get(view));
  if(IsIndependentFocusEnabled(window))
  {
    return view.IsFocusable() && view.IsEnabled() &&
           !view.HasAncestorBlockingFocus() && !InputField::DownCast(view) && !InputEditor::DownCast(view);
  }
  return window && IsActiveWindow(Dali::Integration::SceneHolder::Get(window.GetRootLayer()));
}

bool FocusManager::IsLogicalFocusWindow(Dali::Integration::SceneHolder sceneHolder) const
{
  Window window = Window::DownCast(sceneHolder);
  return IsActiveWindow(sceneHolder) || IsIndependentFocusEnabled(window);
}

bool FocusManager::SetIndependentFocusEnabled(Window window, bool enabled)
{
  if(!window || !window.GetRootLayer() || mNavigationInProgress)
  {
    return false;
  }
  if(IsIndependentFocusEnabled(window) == enabled)
  {
    return true;
  }
  auto keyManager = KeyInputFocusManager::Get();
  if(enabled)
  {
    // Weak Window identity prevents native-ID reuse from inheriting a policy.
    // Release expired records when another independent Window is configured.
    mIndependentFocusStates.erase(std::remove_if(mIndependentFocusStates.begin(), mIndependentFocusStates.end(),
                                                 [](const IndependentFocusState& entry)
    { return !entry.window.GetHandle(); }),
                                  mIndependentFocusStates.end());
    View actual    = GetCurrentFocusView(window);
    View stored    = GetFocusViewFromWindow(window.GetRootLayer());
    View keyTarget = keyManager.GetCurrentFocusView(window);
    for(View candidate : {actual, stored, keyTarget})
    {
      if(InputField::DownCast(candidate) || InputEditor::DownCast(candidate))
      {
        return false;
      }
    }
    IndependentFocusState* state = FindIndependentState(window);
    if(!state)
    {
      mIndependentFocusStates.emplace_back();
      state = &mIndependentFocusStates.back();
    }
    state->window  = window;
    state->enabled = true;
    state->focus   = actual;
    state->context = mLastFocusChangeContext;
    state->context.window.Reset();
    state->history.clear();
    if(actual)
    {
      state->history.push_back(actual);
      actual.EffectiveVisibilityChangedSignal().Connect(mSlotDelegate, &FocusManager::OnIndependentViewVisibilityChanged);
      if(mFocusIndicatorView && mFocusIndicatorView.GetParent() == actual)
      {
        state->indicator = mFocusIndicatorView;
        mFocusIndicatorView.Reset();
      }
    }
    keyManager.SetIndependentWindow(window, true);
    window.VisibilityChangedSignal().Connect(mSlotDelegate, &FocusManager::OnWindowVisibilityChanged);
  }
  else
  {
    auto* state    = FindIndependentState(window);
    state->enabled = false;
    if(!IsActiveWindow(Dali::Integration::SceneHolder::Get(window.GetRootLayer())))
    {
      ClearWindowFocus(window, true);
    }
    else
    {
      if(state->indicator)
      {
        if(mFocusIndicatorView)
        {
          mFocusIndicatorView.Unparent();
        }
        mFocusIndicatorView = state->indicator;
      }
      View current = state->focus.GetHandle();
      if(current && (mFocusHistory.empty() || mFocusHistory.back().GetHandle() != current))
      {
        mFocusHistory.push_back(current);
      }
    }
    // A focus-loss callback can explicitly enable the Window again.
    if(IsIndependentFocusEnabled(window))
    {
      return true;
    }
    keyManager.SetIndependentWindow(window, false);
    window.VisibilityChangedSignal().Disconnect(mSlotDelegate, &FocusManager::OnWindowVisibilityChanged);
    mIndependentFocusStates.erase(std::remove_if(mIndependentFocusStates.begin(), mIndependentFocusStates.end(),
                                                 [window](const IndependentFocusState& entry)
    { return entry.window.GetHandle() == window; }),
                                  mIndependentFocusStates.end());
  }
  return true;
}

View FocusManager::GetCurrentFocusView(Window window)
{
  if(!window)
  {
    return View();
  }
  View view;
  if(auto* state = FindIndependentState(window))
  {
    view = state->focus.GetHandle();
  }
  else
  {
    view = GetCurrentFocusView();
  }
  return view && view.IsConnectedToScene() && Dali::Integration::SceneHolder::Get(view) == window ? view : View();
}

const FocusManager::FocusChangeContext& FocusManager::FocusChangedContext(View view) const
{
  for(auto iter = mFocusNotificationContexts.rbegin(); iter != mFocusNotificationContexts.rend(); ++iter)
  {
    if(iter->first.GetHandle() == view)
    {
      return iter->second;
    }
  }
  if(view)
  {
    Window window = Window::DownCast(Dali::Integration::SceneHolder::Get(view));
    if(const auto* state = FindIndependentState(window))
    {
      return state->context;
    }
  }
  return mLastFocusChangeContext;
}

void FocusManager::NotifyKeyInputFocus(View view, bool focused, Window window)
{
  FocusChangeContext context = mLastFocusChangeContext;
  if(const auto* state = FindIndependentState(window))
  {
    context = state->context;
  }
  mFocusNotificationContexts.emplace_back(view, context);
  GetImpl(view).NotifyFocusChanged(focused);
  mFocusNotificationContexts.pop_back();
}

void FocusManager::ClearWindowFocus(Window window, bool clearStoredFocus)
{
  if(!window)
  {
    return;
  }
  View view = GetCurrentFocusView(window);
  if(auto* state = FindIndependentState(window))
  {
    view = state->focus.GetHandle();
  }
  auto keyManager = KeyInputFocusManager::Get();
  View keyTarget  = keyManager.GetCurrentFocusView(window);
  DetachFocusIndicator(view);
  if(auto* state = FindIndependentState(window))
  {
    state->focus.Reset();
    state->touchCandidate.Reset();
    state->touchDeviceId = -1;
    state->context       = {};
  }
  const bool global = view && mCurrentFocusView.GetHandle() == view;
  if(global)
  {
    mCurrentFocusView.Reset();
    mCurrentFocusWindow.Reset();
    mLastFocusChangeContext = {};
  }
  if(clearStoredFocus)
  {
    for(auto iter = mCurrentFocusViews.begin(); iter != mCurrentFocusViews.end();)
    {
      if(iter->first.GetHandle() == window.GetRootLayer())
      {
        View stored = iter->second.GetHandle();
        iter        = mCurrentFocusViews.erase(iter);
        DisconnectFocusViewIfUnused(stored);
      }
      else
      {
        ++iter;
      }
    }
  }
  if(view)
  {
    view.SceneDisconnectedSignal().Disconnect(mSlotDelegate, &FocusManager::OnSceneDisconnection);
  }
  keyManager.RemoveFocus(keyTarget);
  DisconnectFocusViewIfUnused(view);
  if(view && !GetCurrentFocusView(window))
  {
    mWindowFocusChangedSignal.Emit(window, view, View());
    if(global && !GetCurrentFocusView())
    {
      mFocusChangedSignal.Emit(view, View());
    }
  }
}

void FocusManager::ClearFocus(Window window)
{
  if(!mNavigationInProgress)
  {
    ClearWindowFocus(window, true);
  }
}

void FocusManager::OnWindowVisibilityChanged(Window window, bool visible)
{
  if(!visible)
  {
    ClearWindowFocus(window, false);
  }
}

void FocusManager::OnIndependentViewVisibilityChanged(Actor actor, bool visible)
{
  if(!visible && IsIndependentFocusEnabled(Window::DownCast(Dali::Integration::SceneHolder::Get(actor))))
  {
    InvalidateFocusView(View::DownCast(actor));
  }
}

bool FocusManager::MoveFocus(Window window, Ui::FocusDirection direction)
{
  return window && MoveFocus(direction, {Ui::FocusDevice::PROGRAMMATIC, "", Ui::InputEvent::Programmatic(), window});
}

void FocusManager::MoveFocusBackward(Window window)
{
  if(!window || mNavigationInProgress)
  {
    return;
  }
  auto getHistory = [this, window]() -> FocusStack&
  {
    if(auto* state = FindIndependentState(window); state && state->enabled)
    {
      return state->history;
    }
    return mFocusHistory;
  };

  const bool independent = IsIndependentFocusEnabled(window);
  View       current     = GetNavigationCursor(window);
  FocusStack candidates  = getHistory();
  for(size_t index = candidates.size(); index > 0u; --index)
  {
    View target = candidates[index - 1u].GetHandle();
    if(!target || target == current || !target.IsConnectedToScene() || Dali::Integration::SceneHolder::Get(target) != window)
    {
      continue;
    }

    // Consume this Window's newer entries before callbacks can append a new
    // request. Keep the selected entry for repeated deferred backward moves.
    FocusStack previousHistory;
    FocusStack pendingHistory;
    {
      auto& history   = getHistory();
      previousHistory = history;
      history.erase(std::remove_if(history.begin() + index, history.end(), [window, independent](const WeakHandle<View>& entry)
      {
        View view = entry.GetHandle();
        return independent || !view || (view.IsConnectedToScene() && Dali::Integration::SceneHolder::Get(view) == window);
      }),
                    history.end());
      pendingHistory = history;
    }
    if(SetCurrentFocusView(target))
    {
      return;
    }

    // A rejected candidate can be skipped, but a reentrant focus or policy
    // change owns the result. Reacquire the history after all callbacks.
    if(IsIndependentFocusEnabled(window) != independent || GetNavigationCursor(window) != current)
    {
      return;
    }
    auto& history = getHistory();
    if(history.size() != pendingHistory.size() ||
       !std::equal(history.begin(), history.end(), pendingHistory.begin(), [](const WeakHandle<View>& left, const WeakHandle<View>& right)
    { return left.GetHandle() == right.GetHandle(); }))
    {
      return;
    }
    history = std::move(previousHistory);
  }
}

void FocusManager::ClearFocusIndication(Window window)
{
  SetFocusIndicated(GetCurrentFocusView(window), false, InputEvent::Programmatic());
}

Ui::FocusManager::WindowFocusChangedSignalType& FocusManager::WindowFocusChangedSignal()
{
  return mWindowFocusChangedSignal;
}

View FocusManager::GetFocusViewFromWindow(Layer rootLayer)
{
  View         view;
  unsigned int index;
  for(index = 0; index < mCurrentFocusViews.size(); index++)
  {
    if(mCurrentFocusViews[index].first.GetHandle() == rootLayer)
    {
      view = mCurrentFocusViews[index].second.GetHandle();
      break;
    }
  }

  auto sceneHolder = view && view.IsConnectedToScene() ? Dali::Integration::SceneHolder::Get(view) : Dali::Integration::SceneHolder();
  if(index < mCurrentFocusViews.size() && (!sceneHolder || sceneHolder.GetRootLayer() != rootLayer))
  {
    // Discard a removed View or a target that now belongs to another Window.
    mCurrentFocusViews.erase(mCurrentFocusViews.begin() + index);
    DisconnectFocusViewIfUnused(view);
    view.Reset();
  }

  return view;
}

void FocusManager::MoveFocusBackward()
{
  if(mNavigationInProgress)
  {
    DALI_LOG_WARNING("Backward navigation is not allowed from a focus navigation callback\n");
    return;
  }

  if(auto* state = FindIndependentState(GetCurrentFocusView()))
  {
    MoveFocusBackward(state->window.GetHandle());
    return;
  }

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
        // A deferred setter succeeds without appending history. Keep the chosen
        // entry so repeated backward requests consume one step, not two.
        if(SetCurrentFocusView(target))
        {
          break;
        }
        mFocusHistory.pop_back();
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
      if(currentFocusedView)
      {
        mFocusHistory.push_back(currentFocusedView);
      }
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

  Layer                  rootLayer         = context.window ? context.window.GetRootLayer() : mCurrentFocusedWindow.GetHandle();
  Window                 navigationWindow  = rootLayer ? Window::DownCast(Dali::Integration::SceneHolder::Get(rootLayer)) : Window();
  View                   currentFocusView  = GetNavigationCursor(navigationWindow);
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

View FocusManager::GetNavigationCursor(Window window)
{
  if(!window)
  {
    return GetCurrentFocusView();
  }
  return IsIndependentFocusEnabled(window) ? GetCurrentFocusView(window) : GetFocusViewFromWindow(window.GetRootLayer());
}

bool FocusManager::ApplyFocusNavigationResult(const FocusNavigationResult& result, View originalFocusView, FocusNavigationContext context, const FocusChangeContext& changeContext)
{
  if(result.GetType() == FocusNavigationResultType::NOT_HANDLED ||
     result.GetType() == FocusNavigationResultType::STAY)
  {
    return false;
  }

  if(GetNavigationCursor(context.GetWindow()) != originalFocusView)
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

void FocusManager::ClearFocus(View view, bool clearStoredFocus)
{
  // Reset context for this system-triggered focus loss.
  mLastFocusChangeContext = {};

  View storedView;
  if(clearStoredFocus)
  {
    // Explicit clear also removes a deferred replacement in this Window.
    for(auto iter = mCurrentFocusViews.begin(); iter != mCurrentFocusViews.end(); ++iter)
    {
      if(iter->first == mCurrentFocusedWindow)
      {
        storedView = iter->second.GetHandle();
        mCurrentFocusViews.erase(iter);
        break;
      }
    }
  }

  Window previousWindow = mCurrentFocusWindow.GetHandle();
  mCurrentFocusView.Reset();
  mCurrentFocusWindow.Reset();
  if(view)
  {
    Window window = Window::DownCast(Dali::Integration::SceneHolder::Get(view));
    DALI_LOG_RELEASE_INFO("ClearFocus id:(%d)\n", view.GetProperty<int32_t>(Dali::Actor::Property::ID));
    if(!window)
    {
      window = previousWindow;
    }
    // A focus-lost callback can remove this View. Stop actual-focus observation
    // before notifying it to avoid recursive ClearFocus, but keep the separate
    // stored-target observer so that removal still cancels its reservation.
    view.SceneDisconnectedSignal().Disconnect(mSlotDelegate, &FocusManager::OnSceneDisconnection);
    Internal::KeyInputFocusManager::Get().RemoveFocus(view);

    if(window)
    {
      mWindowFocusChangedSignal.Emit(window, view, View());
    }
    // A loss callback may already have established a new global target.
    if(!GetCurrentFocusView() && !mFocusChangedSignal.Empty())
    {
      mFocusChangedSignal.Emit(view, Ui::View());
    }
  }
  // Focus-out clears actual focus but retains its reservation. Keep observing
  // that saved View so removal while the Window is inactive cancels the record.
  DisconnectFocusViewIfUnused(view);
  DisconnectFocusViewIfUnused(storedView);
}

void FocusManager::DisconnectFocusViewIfUnused(View view)
{
  if(view && view != mCurrentFocusView.GetHandle())
  {
    for(const auto& state : mIndependentFocusStates)
    {
      if(state.focus.GetHandle() == view)
      {
        return;
      }
    }
    for(const auto& entry : mCurrentFocusViews)
    {
      if(entry.second.GetHandle() == view)
      {
        return;
      }
    }
    view.SceneDisconnectedSignal().Disconnect(mSlotDelegate, &FocusManager::OnSceneDisconnection);
    view.SceneDisconnectedSignal().Disconnect(mSlotDelegate, &FocusManager::OnStoredFocusViewDisconnection);
    view.EffectiveVisibilityChangedSignal().Disconnect(mSlotDelegate, &FocusManager::OnIndependentViewVisibilityChanged);
  }
}

void FocusManager::DetachFocusIndicator(View view)
{
  if(view)
  {
    auto* state     = FindIndependentState(view);
    View  indicator = state ? state->indicator : mFocusIndicatorView;
    if(indicator && indicator.GetParent() == view)
    {
      indicator.Unparent();
    }
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

void FocusManager::OnDragStarted(Window window)
{
  if(auto* state = FindIndependentState(window))
  {
    state->touchCandidate.Reset();
    state->touchDeviceId = -1;
  }
  View candidate = mTouchFocusCandidate.GetHandle();
  if(!window || !candidate || !candidate.IsConnectedToScene() || Dali::Integration::SceneHolder::Get(candidate) == window)
  {
    ClearTouchFocusCandidate();
  }
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
  if(auto* state = FindIndependentState(view))
  {
    ClearWindowFocus(state->window.GetHandle(), true);
    return;
  }
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
  return GetFocusIndicatorView(GetCurrentFocusView());
}

View FocusManager::GetFocusIndicatorView(View view)
{
  auto* state     = FindIndependentState(view);
  View& indicator = state ? state->indicator : mFocusIndicatorView;
  if(!indicator)
  {
    const std::string imageDirPath = Dali::Ui::Integration::AssetManager::GetDaliImagePath();
    Ui::ImageView     image        = Ui::ImageView::New();
    image.SetResourceUrl(Dali::Integration::ToDaliString(imageDirPath + FOCUS_BORDER_IMAGE_FILE_NAME));
    image.SetFittingMode(Ui::Image::FittingMode::FILL);
    indicator = image;
    indicator.SetRequestedWidth(MATCH_PARENT);
    indicator.SetRequestedHeight(MATCH_PARENT);
    indicator.SetLayoutMode(LayoutMode::STANDALONE);
  }
  return indicator;
}

uint32_t FocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

void FocusManager::OnKeyEvent(Dali::Integration::SceneHolder sceneHolder, KeyEvent event)
{
  // Injected keys may navigate a background Window's stored target, but
  // must belong to the SceneHolder that delivered them.
  uint32_t eventWindowId = event.GetWindowId();
  if(eventWindowId > 0 && eventWindowId != static_cast<uint32_t>(sceneHolder.GetNativeId()))
  {
    return;
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
  View focusViewBeforeKey  = GetCurrentFocusView(Window::DownCast(sceneHolder));

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

  if(isFocusStartableKey && IsLogicalFocusWindow(sceneHolder))
  {
    View focusedView = GetCurrentFocusView(Window::DownCast(sceneHolder));
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
  Window     window         = Window::DownCast(sceneHolder);
  const bool independent    = IsIndependentFocusEnabled(window);
  auto       clearCandidate = [this, window, independent]()
  {
    if(independent)
    {
      if(auto* state = FindIndependentState(window))
      {
        state->touchCandidate.Reset();
        state->touchDeviceId = -1;
      }
    }
    else
    {
      ClearTouchFocusCandidate();
    }
  };
  if(touch.GetPointCount() < 1)
  {
    clearCandidate();
    return;
  }
  switch(touch.GetState(0))
  {
    case PointState::DOWN:
    {
      clearCandidate();
      View hitView     = View::DownCast(touch.GetHitActor(0));
      View focusedView = GetCurrentFocusView(window);
      if(mClearFocusIndicationOnTouch)
      {
        SetFocusIndicationWithPolicy(focusedView, false, ConvertDeviceClassToKeyboardFocusDevice(touch.GetDeviceClass(0)), Ui::InputEvent::New(touch));
      }
      if(hitView && hitView != GetCurrentFocusView(window) && hitView.IsFocusable() && hitView.IsFocusOnTouchEnabled() && !hitView.HasAncestorBlockingFocus())
      {
        if(independent)
        {
          if(auto* state = FindIndependentState(window))
          {
            state->touchCandidate = hitView;
            state->touchDeviceId  = touch.GetDeviceId(0);
          }
        }
        else
        {
          mTouchFocusCandidate = hitView;
          mTouchFocusDeviceId  = touch.GetDeviceId(0);
        }
      }
      break;
    }
    case PointState::UP:
    {
      View    candidate = mTouchFocusCandidate.GetHandle();
      int32_t deviceId  = mTouchFocusDeviceId;
      if(independent)
      {
        auto* state = FindIndependentState(window);
        candidate   = state ? state->touchCandidate.GetHandle() : View();
        deviceId    = state ? state->touchDeviceId : -1;
      }
      if(candidate && touch.GetDeviceId(0) == deviceId && View::DownCast(touch.GetHitActor(0)) == candidate &&
         candidate.IsFocusable() && candidate.IsFocusOnTouchEnabled() && !candidate.HasAncestorBlockingFocus())
      {
        DoSetCurrentFocusView(candidate, {ConvertDeviceClassToKeyboardFocusDevice(touch.GetDeviceClass(0)), touch.GetDeviceName(0), Ui::InputEvent::New(touch), window});
      }
      clearCandidate();
      break;
    }
    case PointState::INTERRUPTED:
    case PointState::LEAVE:
      clearCandidate();
      break;
    default:
      break;
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
    View focusedView     = GetCurrentFocusView(Window::DownCast(Dali::Integration::SceneHolder::Get(actor)));
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
  View view     = GetCurrentFocusView(Window::DownCast(sceneHolder));
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
  if(focusIn)
  {
    // Change Current Focused Window
    Layer rootLayer       = window.GetRootLayer();
    mCurrentFocusedWindow = rootLayer;
    mCurrentWindowId      = static_cast<uint32_t>(Dali::Integration::SceneHolder::Get(rootLayer).GetNativeId());

    // Get Current Focused View from window
    View currentFocusedView = IsIndependentFocusEnabled(window) ? GetCurrentFocusView(window) : View();
    if(!currentFocusedView)
    {
      currentFocusedView = GetFocusViewFromWindow(rootLayer);
    }
    // Focus-in can arrive while a navigation callback is running. The public
    // setter rejects requests during navigation; treating that rejection as an
    // invalid target here would clear valid actual focus and its saved record.
    // Validate and restore directly for this platform event, while keeping the
    // public setters' restriction on application requests during navigation.
    if(currentFocusedView && (IsIndependentFocusEnabled(window) || currentFocusedView.IsVisible()) && !currentFocusedView.HasAncestorBlockingFocus() &&
       DoSetCurrentFocusView(currentFocusedView, {Ui::FocusDevice::PROGRAMMATIC, "", Ui::InputEvent::Programmatic()}))
    {
      RefreshFocusIndicator(currentFocusedView);
    }
    else
    {
      // No valid stored target: discard this Window's record and release
      // any actual focus retained by the clear-on-loss=false policy.
      View  previousView  = GetCurrentFocusView();
      auto* previousState = FindIndependentState(previousView);
      if(previousState && previousState->enabled)
      {
        mCurrentFocusView.Reset();
        mCurrentFocusWindow.Reset();
        mLastFocusChangeContext = {};
        KeyInputFocusManager::Get().SetPrimaryWindow(window);
        if(previousView)
        {
          mFocusChangedSignal.Emit(previousView, View());
        }
      }
      else
      {
        DetachFocusIndicator(previousView);
        ClearFocus(previousView);
        KeyInputFocusManager::Get().SetPrimaryWindow(window);
      }
    }
  }
}

void FocusManager::OnSceneHolderFocusChanged(Dali::Integration::SceneHolder sceneHolder, bool focusIn)
{
  Window window = Window::DownCast(sceneHolder);
  if(window)
  {
    if(!focusIn && !IsIndependentFocusEnabled(window) && mCurrentFocusedWindow.GetHandle() == window.GetRootLayer() && mClearFocusOnWindowFocusLost)
    {
      // Keep the Window as the navigation scope, and preserve its record.
      View currentView = GetCurrentFocusView();
      DetachFocusIndicator(currentView);
      ClearFocus(currentView, false);
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
  else if(0 == strcmp(signalName.CStr(), SIGNAL_WINDOW_FOCUS_CHANGED))
  {
    manager->WindowFocusChangedSignal().Connect(tracker, functor);
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
  std::vector<View> targets;
  for(auto& state : mIndependentFocusStates)
  {
    if(!enabled && state.indicator)
    {
      state.indicator.Unparent();
    }
    targets.push_back(state.focus.GetHandle());
  }
  RefreshFocusIndicator(GetCurrentFocusView());
  for(View target : targets)
  {
    RefreshFocusIndicator(target);
  }
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
  Window window = view ? Window::DownCast(Dali::Integration::SceneHolder::Get(view)) : Window();
  if(!view || view != GetCurrentFocusView(window))
  {
    return;
  }

  const bool focusIndicated = GetImpl(view).GetState().Contains(ViewState::FOCUS_INDICATED);
  if(mDefaultFocusIndicatorEnabled && focusIndicated && !IsDefaultFocusIndicatorSuppressedByStateEffect(view))
  {
    view.Add(GetFocusIndicatorView(view));
  }
  else
  {
    DetachFocusIndicator(view);
  }
}

void FocusManager::OnSceneDisconnection(Dali::Actor actor)
{
  InvalidateFocusView(View::DownCast(actor));
}

void FocusManager::InvalidateFocusView(View view)
{
  OnStoredFocusViewDisconnection(view);
  if(auto* state = FindIndependentState(view))
  {
    Window window = state->window.GetHandle();
    if(state->focus.GetHandle() == view)
    {
      ClearWindowFocus(window, false);
    }
  }
  else if(view && view == mCurrentFocusView.GetHandle())
  {
    DetachFocusIndicator(view);
    ClearFocus(view, false);
  }
  KeyInputFocusManager::Get().RemoveFocus(view);
  DisconnectFocusViewIfUnused(view);
}

void FocusManager::OnStoredFocusViewDisconnection(Dali::Actor actor)
{
  View view = View::DownCast(actor);
  if(!view)
  {
    return;
  }

  // Disabling/removing retained A must not cancel a different deferred B.
  // Conversely, invalidating B must cancel its reservation without clearing A.
  // An explicit ClearFocus() intentionally has the wider cancellation contract.
  for(auto iter = mCurrentFocusViews.begin(); iter != mCurrentFocusViews.end();)
  {
    if(iter->second.GetHandle() == view)
    {
      iter = mCurrentFocusViews.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
  DisconnectFocusViewIfUnused(view);
}

} // namespace Internal

} // namespace Ui

} //namespace DALI_NAMESPACE
