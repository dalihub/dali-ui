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
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/controls/control-impl.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/adaptor-framework/scene-holder.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/layer.h>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
namespace Dali
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

KeyInputFocusManager::KeyInputFocusManager()
: mSlotDelegate(this),
  mCurrentFocusView(),
  mCurrentWindowId(0)
{
  // Retrieve all the existing widnows
  Dali::SceneHolderList sceneHolders = Adaptor::Get().GetSceneHolders();
  for(auto iter = sceneHolders.begin(); iter != sceneHolders.end(); ++iter)
  {
    (*iter).KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
  }

  // Get notified when any new scene holder is created afterwards
  Adaptor::Get().WindowCreatedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnSceneHolderCreated);
}

KeyInputFocusManager::~KeyInputFocusManager()
{
}

void KeyInputFocusManager::OnSceneHolderCreated(Dali::Integration::SceneHolder& sceneHolder)
{
  sceneHolder.KeyEventGeneratedSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnKeyEvent);
}

void KeyInputFocusManager::SetFocus(Ui::Control control)
{
}

void KeyInputFocusManager::SetFocus(Ui::View view)
{
  if(!view)
  {
    // No-op
    return;
  }

  if(view == mCurrentFocusView)
  {
    // View already has focus
    return;
  }

  view.OffSceneSignal().Connect(mSlotDelegate, &KeyInputFocusManager::OnFocusViewSceneDisconnection);

  Dali::Ui::View previousFocusView = GetCurrentFocusView();

  // Set view to currentFocusView
  mCurrentFocusView = view;
  mCurrentWindowId  = static_cast<uint32_t>(Dali::Integration::SceneHolder::Get(view).GetNativeId());

  if(previousFocusView)
  {
    // Notify the view that it has lost key input focus
    Integration::GetImpl(previousFocusView).OnKeyInputFocusLost();
  }

  // Tell the new actor that it has gained focus.
  Integration::GetImpl(view).OnKeyInputFocusGained();

  // Emit the signal to inform focus change to the application.
  if(!mKeyInputFocusChangedSignal.Empty())
  {
    mKeyInputFocusChangedSignal.Emit(view, previousFocusView);
  }
}

void KeyInputFocusManager::RemoveFocus(Ui::Control control)
{
}

void KeyInputFocusManager::RemoveFocus(Ui::View view)
{
  if(view && view == mCurrentFocusView)
  {
    DALI_LOG_RELEASE_INFO("RemoveFocus id:(%d)\n", view.GetProperty<int32_t>(Dali::Actor::Property::ID));
    view.OffSceneSignal().Disconnect(mSlotDelegate, &KeyInputFocusManager::OnFocusViewSceneDisconnection);

    mCurrentFocusView.Reset();
    mCurrentWindowId = 0;

    // Notify the view that it has lost key input focus
    Integration::GetImpl(view).OnKeyInputFocusLost();
  }
}

Ui::Control KeyInputFocusManager::GetCurrentFocusControl() const
{
  return Ui::Control();
}

Ui::View KeyInputFocusManager::GetCurrentFocusView() const
{
  return mCurrentFocusView;
}

uint32_t KeyInputFocusManager::GetCurrentWindowId() const
{
  return mCurrentWindowId;
}

Ui::KeyInputFocusManager::KeyInputFocusChangedSignalType& KeyInputFocusManager::KeyInputFocusChangedSignal()
{
  return mKeyInputFocusChangedSignal;
}

bool KeyInputFocusManager::OnKeyEvent(const KeyEvent& event)
{
  bool consumed = false;

  Ui::View view = GetCurrentFocusView();
  if(view)
  {
    // Key events that occur in windows other than the currently focused view are skipped.
    uint32_t eventWindowId = event.GetWindowId();
    if(eventWindowId > 0 && GetCurrentWindowId() != eventWindowId)
    {
      DALI_LOG_RELEASE_INFO("Current view window id %d, window ID where key event occurred %d : key event skip\n",
                            GetCurrentWindowId(), eventWindowId);
      return consumed;
    }

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
    consumed = EmitKeyEventSignal(view, event);
  }

  return consumed;
}

bool KeyInputFocusManager::EmitKeyEventSignal(Ui::Control control, const KeyEvent& event)
{
  return false;
}

bool KeyInputFocusManager::EmitKeyEventSignal(Ui::View view, const KeyEvent& event)
{
  bool consumed = false;

  if(view)
  {
    consumed = Integration::GetImpl(view).EmitKeyEventSignal(event);

    // if view doesn't consume KeyEvent, give KeyEvent to its parent.
    if(!consumed)
    {
      Ui::View parent = Ui::View::DownCast(view.GetParent());

      if(parent)
      {
        consumed = EmitKeyEventSignal(parent, event);
      }
    }
  }

  return consumed;
}

void KeyInputFocusManager::OnFocusControlSceneDisconnection(Dali::Actor actor)
{
}

void KeyInputFocusManager::OnFocusViewSceneDisconnection(Dali::Actor actor)
{
  RemoveFocus(Dali::Ui::View::DownCast(actor));
}

bool KeyInputFocusManager::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker,
                                           const Dali::String& signalName, FunctorDelegate* functor)
{
  bool                  connected(true);
  KeyInputFocusManager* manager = dynamic_cast<KeyInputFocusManager*>(object);

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

} // namespace Dali
