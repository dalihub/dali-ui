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

#include <cstdint>
#include <dali/integration-api/input-options.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/integration-api/input-event-impl.h>

// CLASS HEADER
#include <dali-ui-foundation/integration-api/clickable-trait-impl.h>

namespace Dali::UI::Integration
{

ClickableTraitImpl::ClickableTraitImpl()
  : TraitImpl(),
    mTapGestureDetector(TapGestureDetector::New()),
    mLongPressGestureDetector(LongPressGestureDetector::New()),
    mPressedChangedSignal(),
    mPseudoDisabledChangedSignal(),
    mKeyClickPolicy(KeyClickPolicy::ON_RELEASE), // TODO Read initial value from config
    mPressedExecutionKey(),
    mPressedExecutionKeyCount(0),
    mPseudoDisabled(false),
    mPressed(false),
    mClickable(true),
    mClickBlockedByTouch(false),
    mClickBlockedByKey(false)
{
  // NOTE Move tap settings to the config
  Dali::Integration::SetTapRecognizerTime(UINT32_MAX);

  mTapGestureDetector.DetectedSignal().Connect(this, &ClickableTraitImpl::OnTapInternal);
  mLongPressGestureDetector.DetectedSignal().Connect(this, &ClickableTraitImpl::OnLongPressedInternal);
}

ClickableTraitImpl::~ClickableTraitImpl()
{
}

Signal<bool(View, const InputEvent&)>& ClickableTraitImpl::GetPressedChangedSignal()
{
  return mPressedChangedSignal;
}

Signal<void(View)>& ClickableTraitImpl::GetPseudoDisabledChangedSignal()
{
  return mPseudoDisabledChangedSignal;
}

Signal<bool(View, const InputEvent&)>& ClickableTraitImpl::GetClickedSignal()
{
  return mClickedSignal;
}

Signal<bool(View, const InputEvent&)>& ClickableTraitImpl::GetLongPressedSignal()
{
  return mLongPressedSignal;
}

bool ClickableTraitImpl::IsPressed() const
{
  return mPressed;
}

bool ClickableTraitImpl::IsPseudoDisabled() const
{
  return mPseudoDisabled;
}

void ClickableTraitImpl::SetPseudoDisabled(bool pseudoDisabled)
{
  if (mPseudoDisabled == pseudoDisabled)
  {
    return;
  }
  mPseudoDisabled = pseudoDisabled;
  mPseudoDisabledChangedSignal.Emit(mOwner.GetHandle());
}

bool ClickableTraitImpl::IsClickable() const
{
  return mClickable;
}

void ClickableTraitImpl::SetClickable(bool clickable)
{
  mClickable = clickable;
}

KeyClickPolicy ClickableTraitImpl::GetKeyClickPolicy() const
{
  return mKeyClickPolicy;
}

void ClickableTraitImpl::SetKeyClickPolicy(KeyClickPolicy policy)
{
  mKeyClickPolicy = policy;
}

void ClickableTraitImpl::OnFocusedChanged(View view, bool focused)
{
  mClickBlockedByTouch = false;

  if (!focused)
  {
    ClearKeyPressedHistory();
    mClickBlockedByKey = false;
  }
#if false
  //FIXME: This is a workaround for the issue that the component is not released
  //       when the component is pressed and the focus is changed.
  if (mPressed)
  {
      mPressed = false;
      OnReleased(/* */);
  }
#endif
}

bool ClickableTraitImpl::OnKeyEvent(View view, const KeyEvent& event)
{
  InputEvent inputEvent = InputEventImpl::New(event);

  switch (event.GetState())
  {
    case KeyEvent::State::DOWN:
    {
      return HandleKeyPressed(view, inputEvent) | HandleKeyPressedForClick(view, inputEvent);
    }
    case KeyEvent::State::UP:
    {
      return HandleKeyReleased(view, inputEvent) | HandleKeyReleasedForClick(view, inputEvent);
    }
    default:
      break;
  }
  return false;
}

bool ClickableTraitImpl::HandleKeyPressed(View view, const InputEvent& event)
{
  const std::string& keyName = event.GetKeyEvent().GetKeyName();
  if (IsExecutionKey(keyName))
  {
    RecordPressedExecutionKey(keyName);
    return SetPressedInternal(true, event);
  }
  return false;
}

bool ClickableTraitImpl::HandleKeyPressedForClick(View view, const InputEvent& event)
{
  bool handled = false;

  if (ShouldKeyPressTriggerClicked())
  {
    handled |= OnClicked(view, event);
  }
  else if (ShouldKeyPressTriggerLongPressed())
  {
    bool consumed = OnLongPressed(view, event);
    mClickBlockedByKey = consumed;
    handled |= consumed;
  }

  return handled;
}

bool ClickableTraitImpl::HandleKeyReleased(View view, const InputEvent& event)
{
  const std::string& keyName = event.GetKeyEvent().GetKeyName();
  if (mPressedExecutionKey == keyName)
  {
    ClearKeyPressedHistory();
    return SetPressedInternal(false, event);
  }
  return false;
}

bool ClickableTraitImpl::HandleKeyReleasedForClick(View view, const InputEvent& event)
{
  if (ShouldKeyReleaseTriggerClicked())
  {
    OnClicked(view, event);
  }
  mClickBlockedByKey = false;

  return mClickable;
}

View ClickableTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void ClickableTraitImpl::OnBeforeAttached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner = view;
}

void ClickableTraitImpl::OnAttached(TraitId id, View& view)
{
  view.TouchedSignal().Connect(this, &ClickableTraitImpl::OnTouchInternal);
  mTapGestureDetector.Attach(view);
  mLongPressGestureDetector.Attach(view);
  view.SetFocusable(true);
  view.SetTouchFocusable(true);
}

void ClickableTraitImpl::OnDetached(TraitId id, View& view)
{
  DALI_ASSERT_ALWAYS(false && "The trait can not be detached once it attached to a view");
}

void ClickableTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
}

bool ClickableTraitImpl::OnTouch(View view, const TouchEvent& touchEvent)
{
  switch (touchEvent.GetState(0))
  {
    case PointState::STARTED:
    {
      return SetPressedInternal(true, InputEventImpl::New(touchEvent));
    }
    case PointState::FINISHED:
    case PointState::INTERRUPTED:
    {
      return SetPressedInternal(false, InputEventImpl::New(touchEvent));
    }
    default:
    {
      break;
    }
  }
  return false;
}

void ClickableTraitImpl::OnTap(View view, const TapGesture& tap)
{
  // NOTE Using TapGestureDetector.HandleEvent() in OnTouch handler can detect tap gesture,
  // but Clicked event should be triggered after the all registered touch event handlers are called.
  // So, we need to detect tap gesture without using OnTouch + HandleEvent.
  if (ShouldTapTriggerClicked())
  {
    InputEvent inputEvent = InputEventImpl::New(tap);
    OnClicked(view, inputEvent);
  }
  mClickBlockedByTouch = false;
}

bool ClickableTraitImpl::OnPressedChanged(View view, const InputEvent& inputEvent)
{
  return mClickable | mPressedChangedSignal.Emit(view, inputEvent);
}

bool ClickableTraitImpl::OnClicked(View view, const InputEvent& inputEvent)
{
  return mClickedSignal.Emit(view, inputEvent);
}

bool ClickableTraitImpl::OnLongPressed(View view, const InputEvent& inputEvent)
{
  return mLongPressedSignal.Emit(view, inputEvent);
}

bool ClickableTraitImpl::IsExecutionKey(const std::string& keyName) const
{
  // TODO Read from config
  return keyName == "Return";
}

bool ClickableTraitImpl::OnTouchInternal(Actor actor, const TouchEvent& touchEvent)
{
  return OnTouch(View::DownCast(actor), touchEvent);
}

void ClickableTraitImpl::OnTapInternal(Actor actor, const TapGesture& event)
{
  OnTap(View::DownCast(actor), event);
}

void ClickableTraitImpl::OnLongPressedInternal(Actor actor, const LongPressGesture& event)
{
  // NOTE OnLongPressedInternal will invoke this method twice: once for Start and once for Finished.
  if (event.GetState() == GestureState::STARTED)
  {
    InputEvent inputEvent = InputEventImpl::New(event);
    mClickBlockedByTouch = OnLongPressed(View::DownCast(actor), inputEvent);
  }
}

void ClickableTraitImpl::RecordPressedExecutionKey(const std::string& keyName)
{
  if (mPressedExecutionKey.empty() || mPressedExecutionKey == keyName)
  {
    mPressedExecutionKey = keyName;
    mPressedExecutionKeyCount++;
  }
}

void ClickableTraitImpl::ClearKeyPressedHistory()
{
  mPressedExecutionKey.clear();
  mPressedExecutionKeyCount = 0;
}

bool ClickableTraitImpl::SetPressedInternal(bool value, const InputEvent& event)
{
  if (value == mPressed)
  {
    return false;
  }

  mPressed = value;

  return OnPressedChanged(mOwner.GetHandle(), event);
}

bool ClickableTraitImpl::ShouldTapTriggerClicked() const
{
  return mClickable && !mClickBlockedByTouch;
}

bool ClickableTraitImpl::ShouldKeyReleaseTriggerClicked() const
{
  return mClickable && mKeyClickPolicy == KeyClickPolicy::ON_RELEASE && !mClickBlockedByKey;
}

bool ClickableTraitImpl::ShouldKeyPressTriggerClicked() const
{
  return mClickable && mKeyClickPolicy == KeyClickPolicy::ON_PRESS && (mPressedExecutionKeyCount == 1);
}

bool ClickableTraitImpl::ShouldKeyPressTriggerLongPressed() const
{
  const uint32_t cMinLongPressKeyCount = 3; // TODO read from the config
  return mClickable && mKeyClickPolicy == KeyClickPolicy::ON_RELEASE &&
         (mPressedExecutionKeyCount >= cMinLongPressKeyCount);
}

} // namespace Dali::UI::Integration
