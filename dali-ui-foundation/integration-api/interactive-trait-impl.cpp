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
#include <dali-ui-foundation/integration-api/interactive-trait-impl.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/input-options.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali::Ui::Integration
{

InteractiveTraitImpl::InteractiveTraitImpl()
: TraitImpl(),
  mTapGestureDetector(TapGestureDetector::New()),
  mPressedChangedSignal(),
  mPseudoDisabledChangedSignal(),
  mKeyClickPolicy(UiConfigManager::Get().GetConfig().GetKeyClickPolicy()),
  mPressedExecutionKey(),
  mPressedExecutionKeyCount(0),
  mPseudoDisabled(false),
  mPressed(false),
  mClickable(true),
  mClickBlockedByTouch(false),
  mClickBlockedByKey(false)
{
  Dali::Integration::SetTapRecognizerTime(UiConfigManager::Get().GetConfig().GetTapRecognizerTime());

  mTapGestureDetector.DetectedSignal().Connect(this, &InteractiveTraitImpl::OnTapInternal);
}

InteractiveTraitImpl::~InteractiveTraitImpl()
{
}

void InteractiveTraitImpl::EnableLongPressDetection()
{
  if(!mLongPressGestureDetector)
  {
    mLongPressGestureDetector = LongPressGestureDetector::New();
    mLongPressGestureDetector.DetectedSignal().Connect(this, &InteractiveTraitImpl::OnLongPressedInternal);
    View owner = mOwner.GetHandle();
    if(owner)
    {
      mLongPressGestureDetector.Attach(owner);
    }
  }
}

Signal<void(View, bool, const InputEvent&)>& InteractiveTraitImpl::PressedChangedSignal()
{
  return mPressedChangedSignal;
}

Signal<void(View, bool)>& InteractiveTraitImpl::PseudoDisabledChangedSignal()
{
  return mPseudoDisabledChangedSignal;
}

Signal<void(View, const InputEvent&)>& InteractiveTraitImpl::ClickedSignal()
{
  return mClickedSignal;
}

Signal<bool(View, const InputEvent&)>& InteractiveTraitImpl::LongPressedSignal()
{
  EnableLongPressDetection();
  return mLongPressedSignal;
}

bool InteractiveTraitImpl::IsPressed() const
{
  return mPressed;
}

bool InteractiveTraitImpl::IsPseudoDisabled() const
{
  return mPseudoDisabled;
}

void InteractiveTraitImpl::SetPseudoDisabled(bool pseudoDisabled)
{
  if(mPseudoDisabled == pseudoDisabled)
  {
    return;
  }
  mPseudoDisabled = pseudoDisabled;

  View owner = mOwner.GetHandle();
  if(owner)
  {
    GetImpl(owner).SetState(ViewState::PSEUDO_DISABLED, pseudoDisabled);
  }

  if(mPseudoDisabled && mPressed)
  {
    SetPressedInternal(false, InputEvent::None());
  }

  mPseudoDisabledChangedSignal.Emit(owner, mPseudoDisabled);
}

bool InteractiveTraitImpl::IsClickable() const
{
  return mClickable;
}

void InteractiveTraitImpl::SetClickable(bool clickable)
{
  mClickable = clickable;
}

KeyClickPolicy InteractiveTraitImpl::GetKeyClickPolicy() const
{
  return mKeyClickPolicy;
}

void InteractiveTraitImpl::SetKeyClickPolicy(KeyClickPolicy policy)
{
  mKeyClickPolicy = policy;
}

void InteractiveTraitImpl::OnFocusedChanged(View view, bool focused)
{
  mClickBlockedByTouch = false;

  if(!focused)
  {
    ClearKeyPressedHistory();
    mClickBlockedByKey = false;

    // NOTE This is for the case that,
    // when holding key pressed and the focus moved to the other object before release,
    // the key release event never come to this view.
    if(mPressed)
    {
      SetPressedInternal(false, InputEvent::None());
    }
  }
}

void InteractiveTraitImpl::OnEnabledChanged(View view, bool enabled)
{
  if(!enabled && mPressed)
  {
    SetPressedInternal(false, InputEvent::None());
  }
}

bool InteractiveTraitImpl::OnKeyEvent(View view, const KeyEvent& event)
{
  InputEvent inputEvent = InputEvent::New(event);

  switch(event.GetState())
  {
    case KeyEvent::State::DOWN:
    {
      return HandleKeyPressed(view, inputEvent);
    }
    case KeyEvent::State::UP:
    {
      return HandleKeyReleased(view, inputEvent);
    }
    default:
      break;
  }
  return false;
}

bool InteractiveTraitImpl::HandleKeyPressed(View view, const InputEvent& event)
{
  const Dali::String& keyName = event.GetKeyEvent().GetKeyName();
  if(IsExecutionKey(keyName))
  {
    RecordPressedExecutionKey(keyName);
    SetPressedInternal(true, event);

    if(ShouldKeyPressTriggerClicked())
    {
      ClearKeyPressedHistory();
      OnClicked(view, event);
    }
    else if(ShouldKeyPressTriggerLongPressed())
    {
      mClickBlockedByKey = OnLongPressed(view, event);
    }
    return true;
  }
  return false;
}

bool InteractiveTraitImpl::HandleKeyReleased(View view, const InputEvent& event)
{
  const Dali::String& keyName = event.GetKeyEvent().GetKeyName();
  if(mPressedExecutionKey && *mPressedExecutionKey == keyName)
  {
    ClearKeyPressedHistory();
    SetPressedInternal(false, event);

    if(ShouldKeyReleaseTriggerClicked())
    {
      OnClicked(view, event);
    }

    mClickBlockedByKey = false;
    return true;
  }
  return false;
}

View InteractiveTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void InteractiveTraitImpl::OnBeforeAttached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner = view;
}

void InteractiveTraitImpl::OnAttached(TraitId id, View& view)
{
  view.TouchedSignal().Connect(this, &InteractiveTraitImpl::OnTouchInternal);
  mTapGestureDetector.Attach(view);
  view.SetFocusable(true);
  view.SetTouchFocusable(true);

  if(mLongPressGestureDetector)
  {
    mLongPressGestureDetector.Attach(view);
  }
}

void InteractiveTraitImpl::OnDetached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(false && "The trait can not be detached once it attached to a view");
}

void InteractiveTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
}

bool InteractiveTraitImpl::OnTouch(View view, const TouchEvent& touchEvent)
{
  if(!view.IsEnabled())
  {
    return false;
  }

  switch(touchEvent.GetState(0))
  {
    case PointState::STARTED:
    {
      SetPressedInternal(true, InputEvent::New(touchEvent));
      return true;
    }
    case PointState::FINISHED:
    case PointState::INTERRUPTED:
    {
      SetPressedInternal(false, InputEvent::New(touchEvent));
      return true;
    }
    default:
    {
      break;
    }
  }
  return false;
}

void InteractiveTraitImpl::OnTap(View view, const TapGesture& tap)
{
  // NOTE Using TapGestureDetector.HandleEvent() in OnTouch handler can detect tap gesture,
  // but Clicked event should be triggered after the all registered touch event handlers are called.
  // So, we need to detect tap gesture without using OnTouch + HandleEvent.
  if(ShouldTapTriggerClicked())
  {
    InputEvent inputEvent = InputEvent::New(tap);
    OnClicked(view, inputEvent);
  }
  mClickBlockedByTouch = false;
}

void InteractiveTraitImpl::OnPressedChanged(View view, const InputEvent& inputEvent)
{
  if(auto* receiver = dynamic_cast<InteractiveEventReceiverInterface*>(&GetImpl(view)))
  {
    receiver->OnPressedChanged(view, mPressed, inputEvent);
  }
  mPressedChangedSignal.Emit(view, mPressed, inputEvent);
}

void InteractiveTraitImpl::OnClicked(View view, const InputEvent& inputEvent)
{
  if(auto* receiver = dynamic_cast<InteractiveEventReceiverInterface*>(&GetImpl(view)))
  {
    receiver->OnClicked(view, inputEvent);
  }
  mClickedSignal.Emit(view, inputEvent);
}

bool InteractiveTraitImpl::OnLongPressed(View view, const InputEvent& inputEvent)
{
  bool consumed = false;
  if(auto* receiver = dynamic_cast<InteractiveEventReceiverInterface*>(&GetImpl(view)))
  {
    consumed = receiver->OnLongPressed(view, inputEvent);
  }
  bool signalResult = mLongPressedSignal.Emit(view, inputEvent);
  return consumed || signalResult;
}

bool InteractiveTraitImpl::IsExecutionKey(const Dali::String& keyName) const
{
  return UiConfigManager::Get().GetConfig().GetExecutionKeyPredicate()(keyName);
}

bool InteractiveTraitImpl::OnTouchInternal(Actor actor, const TouchEvent& touchEvent)
{
  return OnTouch(View::DownCast(actor), touchEvent);
}

void InteractiveTraitImpl::OnTapInternal(Actor actor, const TapGesture& event)
{
  OnTap(View::DownCast(actor), event);
}

void InteractiveTraitImpl::OnLongPressedInternal(Actor actor, const LongPressGesture& event)
{
  // NOTE OnLongPressedInternal will invoke this method twice: once for Start and once for Finished.
  if(event.GetState() == GestureState::STARTED)
  {
    InputEvent inputEvent = InputEvent::New(event);
    mClickBlockedByTouch  = OnLongPressed(View::DownCast(actor), inputEvent);
  }
}

void InteractiveTraitImpl::RecordPressedExecutionKey(const Dali::String& keyName)
{
  if(!mPressedExecutionKey || *mPressedExecutionKey == keyName)
  {
    if(!mPressedExecutionKey)
    {
      mPressedExecutionKey = MakeUnique<Dali::String>(keyName);
    }
    else
    {
      *mPressedExecutionKey = keyName;
    }
    mPressedExecutionKeyCount++;
  }
}

void InteractiveTraitImpl::ClearKeyPressedHistory()
{
  mPressedExecutionKey.Reset();
  mPressedExecutionKeyCount = 0;
}

void InteractiveTraitImpl::SetPressedInternal(bool value, const InputEvent& event)
{
  if(value == mPressed)
  {
    return;
  }

  mPressed = value;

  View owner = mOwner.GetHandle();
  if(owner)
  {
    GetImpl(owner).SetState(ViewState::PRESSED, value, event);
  }

  OnPressedChanged(owner, event);
}

bool InteractiveTraitImpl::ShouldTapTriggerClicked() const
{
  return mClickable && !mClickBlockedByTouch;
}

bool InteractiveTraitImpl::ShouldKeyReleaseTriggerClicked() const
{
  return mClickable && mKeyClickPolicy == KeyClickPolicy::ON_RELEASE && !mClickBlockedByKey;
}

bool InteractiveTraitImpl::ShouldKeyPressTriggerClicked() const
{
  return mClickable && mKeyClickPolicy == KeyClickPolicy::ON_PRESS && (mPressedExecutionKeyCount == 1);
}

bool InteractiveTraitImpl::ShouldKeyPressTriggerLongPressed() const
{
  return mClickable && mKeyClickPolicy == KeyClickPolicy::ON_RELEASE &&
         (mPressedExecutionKeyCount >= UiConfigManager::Get().GetConfig().GetKeyLongPressThreshold());
}

} // namespace Dali::Ui::Integration
