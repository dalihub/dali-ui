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
#include "keyinput-focus-manager-impl.h"

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <algorithm>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
// Signals

const char* const SIGNAL_KEY_INPUT_FOCUS_CHANGED = "keyInputFocusChanged";

} // namespace

KeyInputFocusManagerImpl::KeyInputFocusManagerImpl()
: mSlotDelegate(this),
  mCurrentFocusView(),
  mCurrentWindowId(0)
{
  // Retrieve all the existing widnows
  Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
  for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
  {
    (*iter).KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManagerImpl::OnKeyEvent);
  }

  // Get notified when any new scene holder is created afterwards
  Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &KeyInputFocusManagerImpl::OnSceneHolderCreated);
}

KeyInputFocusManagerImpl::~KeyInputFocusManagerImpl()
{
}

void KeyInputFocusManagerImpl::OnSceneHolderCreated(Dali::Integration::SceneHolder sceneHolder)
{
  sceneHolder.KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManagerImpl::OnKeyEvent);
}

void KeyInputFocusManagerImpl::SetIndependentWindow(Window window, bool enabled)
{
  mWindowTargets.erase(std::remove_if(mWindowTargets.begin(), mWindowTargets.end(),
                                      [](const WindowKeyTarget& target)
  { return !target.window.GetHandle(); }),
                       mWindowTargets.end());
  for(auto iter = mWindowTargets.begin(); iter != mWindowTargets.end(); ++iter)
  {
    if(iter->window.GetHandle() == window)
    {
      if(!enabled)
      {
        mWindowTargets.erase(iter);
      }
      return;
    }
  }
  if(enabled)
  {
    WindowKeyTarget target;
    target.window = window;
    if(mCurrentFocusView && Dali::Integration::SceneHolder::Get(mCurrentFocusView) == window)
    {
      target.view = mCurrentFocusView;
      mCurrentFocusView.EffectiveVisibilityChangedSignal().Connect(mSlotDelegate, &KeyInputFocusManagerImpl::OnFocusViewVisibilityChanged);
    }
    mWindowTargets.push_back(target);
  }
}

Ui::View KeyInputFocusManagerImpl::GetCurrentFocusView(Window window) const
{
  if(!window)
  {
    return Ui::View();
  }
  for(const auto& target : mWindowTargets)
  {
    if(target.window.GetHandle() == window)
    {
      Ui::View view = target.view.GetHandle();
      return view;
    }
  }
  return mCurrentFocusView && mCurrentFocusView.IsConnectedToScene() &&
             Dali::Integration::SceneHolder::Get(mCurrentFocusView) == window
           ? mCurrentFocusView
           : Ui::View();
}

void KeyInputFocusManagerImpl::SetPrimaryWindow(Window window)
{
  Ui::View previous = mCurrentFocusView;
  Ui::View target   = GetCurrentFocusView(window);
  // Commit before notifying loss so a callback's newer target is never overwritten.
  mCurrentFocusView = target;
  mCurrentWindowId  = target ? static_cast<uint32_t>(window.GetNativeId()) : 0;
  if(previous && previous != target && !HasFocusTarget(previous))
  {
    NotifyFocusLost(previous, Window::DownCast(Dali::Integration::SceneHolder::Get(previous)));
  }
}

bool KeyInputFocusManagerImpl::HasFocusTarget(Ui::View view) const
{
  if(!view)
  {
    return false;
  }
  return mCurrentFocusView == view || std::any_of(mWindowTargets.begin(), mWindowTargets.end(),
                                                  [view](const WindowKeyTarget& target)
  { return target.window.GetHandle() && target.view.GetHandle() == view; });
}

void KeyInputFocusManagerImpl::NotifyFocusLost(Ui::View view, Window window)
{
  view.SceneDisconnectedSignal().Disconnect(mSlotDelegate, &KeyInputFocusManagerImpl::OnFocusViewSceneDisconnection);
  view.EffectiveVisibilityChangedSignal().Disconnect(mSlotDelegate, &KeyInputFocusManagerImpl::OnFocusViewVisibilityChanged);
  auto focusManager = Ui::FocusManager::Get();
  GetImpl(focusManager).NotifyKeyInputFocus(view, false, window);
}

void KeyInputFocusManagerImpl::SetFocus(Ui::View view)
{
  if(!view || !view.IsConnectedToScene())
  {
    return;
  }
  Window window       = Window::DownCast(Dali::Integration::SceneHolder::Get(view));
  auto   focusManager = Ui::FocusManager::Get();
  if(!window || (focusManager && !GetImpl(focusManager).CanSetKeyInputFocus(view)))
  {
    return;
  }

  const bool independent     = focusManager && GetImpl(focusManager).IsIndependentFocusEnabled(window);
  const bool active          = !focusManager || GetImpl(focusManager).IsActiveWindow(Dali::Integration::SceneHolder::Get(window.GetRootLayer()));
  Ui::View   previous        = independent ? GetCurrentFocusView(window) : mCurrentFocusView;
  Ui::View   previousPrimary = active && mCurrentFocusView != previous ? mCurrentFocusView : Ui::View();
  if(independent)
  {
    for(auto& target : mWindowTargets)
    {
      if(target.window.GetHandle() == window)
      {
        target.view = view;
        break;
      }
    }
  }
  if(active)
  {
    mCurrentFocusView = view;
    mCurrentWindowId  = static_cast<uint32_t>(window.GetNativeId());
  }
  if(previous != view)
  {
    view.SceneDisconnectedSignal().Connect(mSlotDelegate, &KeyInputFocusManagerImpl::OnFocusViewSceneDisconnection);
    if(independent)
    {
      view.EffectiveVisibilityChangedSignal().Connect(mSlotDelegate, &KeyInputFocusManagerImpl::OnFocusViewVisibilityChanged);
    }
  }
  // Local and primary ownership have both been updated. Release each displaced
  // target once, unless a callback has already made it a current target again.
  for(Ui::View displaced : {previous, previousPrimary})
  {
    if(displaced && displaced != view && !HasFocusTarget(displaced))
    {
      NotifyFocusLost(displaced, Window::DownCast(Dali::Integration::SceneHolder::Get(displaced)));
    }
  }
  // A loss callback may replace or disconnect the newly committed target.
  if(previous == view || GetCurrentFocusView(window) != view)
  {
    return;
  }
  GetImpl(focusManager).NotifyKeyInputFocus(view, true, window);
  if(GetCurrentFocusView(window) == view && !mKeyInputFocusChangedSignal.Empty())
  {
    mKeyInputFocusChangedSignal.Emit(view, previous);
  }
}

void KeyInputFocusManagerImpl::RemoveFocus(Ui::View view)
{
  if(!view)
  {
    return;
  }
  bool   removed = false;
  Window window  = Window::DownCast(Dali::Integration::SceneHolder::Get(view));
  for(auto& target : mWindowTargets)
  {
    if(target.view.GetHandle() == view)
    {
      window = target.window.GetHandle();
      target.view.Reset();
      removed = true;
    }
  }
  if(view == mCurrentFocusView)
  {
    mCurrentFocusView.Reset();
    mCurrentWindowId = 0;
    removed          = true;
  }
  if(removed)
  {
    NotifyFocusLost(view, window);
  }
}

Ui::View KeyInputFocusManagerImpl::GetCurrentFocusView() const
{
  return mCurrentFocusView;
}

uint32_t KeyInputFocusManagerImpl::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManagerImpl::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignal;
}

bool KeyInputFocusManagerImpl::OnKeyEvent(Dali::Integration::SceneHolder sceneHolder, KeyEvent event)
{
  bool consumed = false;

  Window window = Window::DownCast(sceneHolder);
  if(!window || (event.GetWindowId() > 0 && event.GetWindowId() != static_cast<uint32_t>(window.GetNativeId())))
  {
    return false;
  }
  Ui::View view = GetCurrentFocusView(window);
  if(view && view.IsConnectedToScene() && Dali::Integration::SceneHolder::Get(view) == sceneHolder)
  {
    Dali::Actor dispatch = view;
    while(dispatch)
    {
      // If the DISPATCH_KEY_EVENTS is false, it cannot emit key event.
      Ui::View dispatchView = Ui::View::DownCast(dispatch);
      if(dispatchView && !dispatchView.GetProperty<bool>(Ui::View::Property::DISPATCH_KEY_EVENTS))
      {
        return true;
      }
      dispatch = dispatch.GetParent();
    }

    // Notify the view about the key event
    consumed = NotifyKeyEvent(view, event);
  }

  return consumed;
}

bool KeyInputFocusManagerImpl::NotifyKeyEvent(Ui::View view, const KeyEvent& event)
{
  bool consumed = false;

  if(view)
  {
    Dali::Actor oldParent   = view.GetParent();
    auto        sceneHolder = Dali::Integration::SceneHolder::Get(view);
    consumed                = ViewDataImpl::Get(GetImpl(view)).NotifyKeyEvent(event);

    if(!consumed && view.GetParent() == oldParent && Dali::Integration::SceneHolder::Get(view) == sceneHolder)
    {
      Ui::View parent = Ui::View::DownCast(view.GetParent());

      if(parent)
      {
        consumed = NotifyKeyEvent(parent, event);
      }
    }
  }

  return consumed;
}

void KeyInputFocusManagerImpl::OnFocusViewSceneDisconnection(Dali::Actor actor)
{
  RemoveFocus(Dali::Ui::View::DownCast(actor));
}

void KeyInputFocusManagerImpl::OnFocusViewVisibilityChanged(Dali::Actor actor, bool visible)
{
  auto manager = Ui::FocusManager::Get();
  if(!visible && manager && GetImpl(manager).IsIndependentFocusEnabled(Window::DownCast(Dali::Integration::SceneHolder::Get(actor))))
  {
    RemoveFocus(Ui::View::DownCast(actor));
  }
}

bool KeyInputFocusManagerImpl::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker,
                                               const Dali::String& signalName, FunctorDelegate* functor)
{
  bool                      connected(true);
  KeyInputFocusManagerImpl* manager = dynamic_cast<KeyInputFocusManagerImpl*>(object);

  if(manager)
  {
    if(0 == strcmp(signalName.CStr(), SIGNAL_KEY_INPUT_FOCUS_CHANGED))
    {
      manager->KeyInputFocusChangedSignal().Connect(tracker, functor);
    }
    else
    {
      // signalName does not match any signal
      connected = false;
    }
  }

  return connected;
}

} // namespace Internal

} // namespace Ui

} //namespace DALI_NAMESPACE
