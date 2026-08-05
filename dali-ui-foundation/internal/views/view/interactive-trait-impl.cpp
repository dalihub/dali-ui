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
#include <dali-ui-foundation/internal/views/view/interactive-trait-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/view.h>

#include <dali-ui-foundation/internal/interactive-trait/pending-press-manager.h>
#include <dali-ui-foundation/internal/scroll-state-observer.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace ExtensionView = Dali::Ui::Extension::View;

namespace Dali::Ui::Internal
{

InteractiveTraitImpl::InteractiveTraitImpl()
: mTapGestureDetector(TapGestureDetector::New()),
  mPressedChangedSignal(),
  mPseudoDisabledChangedSignal(),
  mKeyClickPolicy(UiConfig::GetCurrent().GetKeyClickPolicy()),
  mPendingKeyInputEvent(),
  mPendingTouchEvent(),
  mPressedExecutionKey(),
  mPressedExecutionKeyCount(0),
  mHoveringDeviceId(-1),
  mPendingKeyAction(PendingKeyAction::NONE),
  mPseudoDisabled(false),
  mClickable(true),
  mClickBlockedByTouch(false),
  mClickBlockedByKey(false)
{
  mTapGestureDetector.DetectedSignal().Connect(this, &InteractiveTraitImpl::OnTapInternal);
}

InteractiveTraitImpl::~InteractiveTraitImpl()
{
  Internal::PendingPressManager::Get().ClearAll(*this);
}

void InteractiveTraitImpl::EnableLongPressDetection()
{
  if(!mLongPressGestureDetector)
  {
    mLongPressGestureDetector = LongPressGestureDetector::New();
    mLongPressGestureDetector.DetectedSignal().Connect(this, &InteractiveTraitImpl::OnLongPressedInternal);
  }
}

Signal<void(View, bool, InputEvent)>& InteractiveTraitImpl::PressedChangedSignal()
{
  return mPressedChangedSignal;
}

Signal<void(View, bool)>& InteractiveTraitImpl::PseudoDisabledChangedSignal()
{
  return mPseudoDisabledChangedSignal;
}

Signal<void(View, InputEvent)>& InteractiveTraitImpl::ClickedSignal()
{
  return mClickedSignal;
}

Signal<bool(View, InputEvent)>& InteractiveTraitImpl::LongPressedSignal()
{
  EnableLongPressDetection();
  return mLongPressedSignal;
}

Signal<void(View, bool, InputEvent)>& InteractiveTraitImpl::HoveredChangedSignal()
{
  return mHoveredChangedSignal;
}

bool InteractiveTraitImpl::IsPressed() const
{
  View owner = mOwner.GetHandle();
  return owner && owner.GetState().Contains(ViewState::PRESSED);
}

bool InteractiveTraitImpl::IsPseudoDisabled() const
{
  return mPseudoDisabled;
}

bool InteractiveTraitImpl::IsHovered() const
{
  View owner = mOwner.GetHandle();
  return owner && owner.GetState().Contains(ViewState::HOVERED);
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
    ExtensionView::SetState(GetImpl(owner), ViewState::PSEUDO_DISABLED, pseudoDisabled);
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
    SetPressedInternal(false, InputEvent::Programmatic().WithCancellation());
  }
}

void InteractiveTraitImpl::OnEnabledChanged(View view, bool enabled)
{
  if(!enabled)
  {
    Internal::PendingPressManager::Get().Cancel(*this);
  }
}

void InteractiveTraitImpl::OnPressedClearedByViewState(View view, InputEvent event)
{
  OnPressedChanged(view, event);
}

void InteractiveTraitImpl::OnHoveredClearedByViewState(View view, InputEvent event)
{
  mHoveringDeviceId = -1;
  mHoveredChangedSignal.Emit(view, IsHovered(), event);
}

bool InteractiveTraitImpl::OnAccessibilityActivate(View view, InputEvent event)
{
  if(!view.IsEnabled() || !mClickable)
  {
    return false;
  }

  OnClicked(view, event);
  return true;
}

void InteractiveTraitImpl::OnSceneConnection(View)
{
}

void InteractiveTraitImpl::OnSceneDisconnection(View)
{
  Internal::PendingPressManager::Get().Cancel(*this);
  ClearKeyPressedHistory();
  mPendingTouchEvent.Reset();
  mHoveringDeviceId    = -1;
  mClickBlockedByTouch = false;
  mClickBlockedByKey   = false;
}

bool InteractiveTraitImpl::OnKeyEvent(View view, const KeyEvent& event)
{
  DALI_ASSERT_DEBUG(mPendingKeyAction == PendingKeyAction::NONE && "Nested key dispatch is unsupported");

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

bool InteractiveTraitImpl::OnHoverEvent(View view, const HoverEvent& event)
{
  InputEvent inputEvent = InputEvent::New(event);

  switch(event.GetState(0u))
  {
    case PointState::STARTED:
    case PointState::MOTION:
    {
      mHoveringDeviceId = event.GetDeviceId(0u);
      SetHoveredInternal(true, inputEvent);
      return true;
    }
    case PointState::FINISHED:
    case PointState::LEAVE:
    case PointState::INTERRUPTED:
    {
      mHoveringDeviceId = -1;
      SetHoveredInternal(false, inputEvent);
      return true;
    }
    default:
    {
      break;
    }
  }

  return false;
}

void InteractiveTraitImpl::FinalizeKeyEventDispatch()
{
  const PendingKeyAction pendingAction = mPendingKeyAction;
  InputEvent             pendingEvent  = mPendingKeyInputEvent;

  mPendingKeyAction     = PendingKeyAction::NONE;
  mPendingKeyInputEvent = InputEvent();

  View view = mOwner.GetHandle();
  if(pendingAction == PendingKeyAction::CLICKED)
  {
    OnClicked(view, pendingEvent);
  }
  else if(pendingAction == PendingKeyAction::LONG_PRESSED)
  {
    mClickBlockedByKey = OnLongPressed(view, pendingEvent);
  }
}

void InteractiveTraitImpl::CancelKeyEventDispatch()
{
  mPendingKeyAction     = PendingKeyAction::NONE;
  mPendingKeyInputEvent = InputEvent();
}

bool InteractiveTraitImpl::HandleKeyPressed(View view, InputEvent event)
{
  const Dali::String& keyName = event.GetKeyEvent().GetKeyName();
  if(IsExecutionKey(keyName))
  {
    RecordPressedExecutionKey(keyName);
    SetPressedInternal(true, event);

    if(ShouldKeyPressTriggerClicked())
    {
      SetPendingKeyAction(event, false);
    }
    else if(ShouldKeyPressTriggerLongPressed())
    {
      SetPendingKeyAction(event, true);
    }
    return true;
  }
  return false;
}

bool InteractiveTraitImpl::HandleKeyReleased(View view, InputEvent event)
{
  const Dali::String& keyName = event.GetKeyEvent().GetKeyName();
  if(mPressedExecutionKey && *mPressedExecutionKey == keyName)
  {
    ClearKeyPressedHistory();
    SetPressedInternal(false, event);

    if(ShouldKeyReleaseTriggerClicked())
    {
      SetPendingKeyAction(event, false);
    }

    mClickBlockedByKey = false;
    return true;
  }
  return false;
}

void InteractiveTraitImpl::SetPendingKeyAction(InputEvent event, bool longPressed)
{
  DALI_ASSERT_DEBUG(mPendingKeyAction == PendingKeyAction::NONE && "Multiple semantic key actions in one dispatch are unsupported");
  mPendingKeyAction     = longPressed ? PendingKeyAction::LONG_PRESSED : PendingKeyAction::CLICKED;
  mPendingKeyInputEvent = event;
}

View InteractiveTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void InteractiveTraitImpl::OnAttached(View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner = view;

  view.SetFocusable(true);
  view.SetFocusOnTouchEnabled(true);
}

void InteractiveTraitImpl::OnDetaching(View& view)
{
  Internal::PendingPressManager::Get().Cancel(*this);
  DALI_ASSERT_ALWAYS(false && "The trait can not be detached once it attached to a view");
}

void InteractiveTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
  Internal::PendingPressManager::Get().Cancel(*this);
}

bool InteractiveTraitImpl::OnTouch(View view, TouchEvent touchEvent)
{
  mPendingTouchEvent.Reset();

  if(!view.IsEnabled())
  {
    return false;
  }

  mPendingTouchEvent = touchEvent;

  switch(touchEvent.GetState(0))
  {
    case PointState::STARTED:
    {
      InputEvent inputEvent = InputEvent::New(touchEvent);
      if(Internal::PendingPressManager::Get().HasPendingPress())
      {
        Internal::PendingPressManager::Get().FlushPendingPress();
        SetPressedInternal(true, inputEvent);
        return true;
      }

      if(Internal::ScrollStateObserver::Get().IsGestureDisambiguating() &&
         UiConfig::GetCurrent().GetAmbiguousPressDelay() > 0u)
      {
        Internal::PendingPressManager::Get().AddPendingPress(*this, inputEvent, touchEvent.GetDeviceId(0));
        return true;
      }

      Internal::PendingPressManager::Get().ClearPendingPress(*this);
      SetPressedInternal(true, inputEvent);
      return true;
    }
    case PointState::FINISHED:
    {
      InputEvent releaseEvent = InputEvent::New(touchEvent);
      if(Internal::PendingPressManager::Get().FinishPendingPress(*this, touchEvent.GetDeviceId(0), releaseEvent))
      {
        return true;
      }
      if(Internal::PendingPressManager::Get().HasPendingRelease(*this))
      {
        return true;
      }

      Internal::PendingPressManager::Get().ClearPendingPress(*this);
      SetPressedInternal(false, releaseEvent);
      return true;
    }
    case PointState::INTERRUPTED:
    case PointState::LEAVE:
    {
      Internal::PendingPressManager::Get().Cancel(*this);
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

void InteractiveTraitImpl::FinalizeTouchEventDispatch(View view, const TouchEvent& event)
{
  TouchEvent pendingEvent = mPendingTouchEvent;
  mPendingTouchEvent.Reset();

  if(!pendingEvent || pendingEvent != event)
  {
    return;
  }

  mTapGestureDetector.HandleEvent(view, event);
  if(mLongPressGestureDetector)
  {
    mLongPressGestureDetector.HandleEvent(view, event);
  }
}

void InteractiveTraitImpl::OnTap(View view, TapGesture tap)
{
  if(ShouldTapTriggerClicked())
  {
    InputEvent inputEvent = InputEvent::New(tap);
    OnClicked(view, inputEvent);
  }
  mClickBlockedByTouch = false;
}

void InteractiveTraitImpl::OnPressedChanged(View view, InputEvent inputEvent)
{
  mPressedChangedSignal.Emit(view, IsPressed(), inputEvent);
}

void InteractiveTraitImpl::OnClicked(View view, InputEvent inputEvent)
{
  mClickedSignal.Emit(view, inputEvent);
}

bool InteractiveTraitImpl::OnLongPressed(View view, InputEvent inputEvent)
{
  return mLongPressedSignal.Emit(view, inputEvent);
}

bool InteractiveTraitImpl::IsExecutionKey(const Dali::String& keyName) const
{
  return UiConfig::GetCurrent().GetExecutionKeyPredicate()(keyName);
}

void InteractiveTraitImpl::OnTapInternal(Actor actor, TapGesture event)
{
  Internal::PendingPressManager::Get().CompletePendingPressWithDuration(*this, InputEvent::New(event));
  OnTap(View::DownCast(actor), event);
}

void InteractiveTraitImpl::OnLongPressedInternal(Actor actor, LongPressGesture event)
{
  // NOTE OnLongPressedInternal will invoke this method twice: once for Start and once for Finished.
  if(event.GetState() == GestureState::STARTED)
  {
    View view = View::DownCast(actor);
    Internal::PendingPressManager::Get().FlushPendingPress(*this);
    InputEvent inputEvent = InputEvent::New(event);
    mClickBlockedByTouch  = OnLongPressed(view, inputEvent);
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

void InteractiveTraitImpl::SetPressedInternal(bool value, InputEvent event)
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  const bool pressed = IsPressed();
  if(value == pressed)
  {
    return;
  }

  if(value && owner.GetState().IsAnyDisabled())
  {
    return;
  }

  if(value && IsPressedByHoveringDevice(event))
  {
    ExtensionView::SetState(GetImpl(owner), ViewState::HOVERED, ViewState::PRESSED, event);
    mHoveredChangedSignal.Emit(owner, false, event);
  }
  else
  {
    ExtensionView::SetState(GetImpl(owner), ViewState::PRESSED, value, event);
  }
  OnPressedChanged(owner, event);
}

bool InteractiveTraitImpl::IsPressedByHoveringDevice(const InputEvent& event) const
{
  if(!IsHovered() || mHoveringDeviceId < 0 || event.GetInputEventType() != InputEventType::TOUCH_EVENT)
  {
    return false;
  }

  const TouchEvent& touchEvent = event.GetTouchEvent();
  return touchEvent.GetPointCount() > 0u &&
         touchEvent.GetState(0u) == PointState::STARTED &&
         touchEvent.GetDeviceId(0u) == mHoveringDeviceId;
}

void InteractiveTraitImpl::SetHoveredInternal(bool value, InputEvent event)
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  const bool hovered = IsHovered();
  if(value == hovered)
  {
    return;
  }

  if(value && owner.GetState().IsAnyDisabled())
  {
    return;
  }

  ExtensionView::SetState(GetImpl(owner), ViewState::HOVERED, value, event);
  mHoveredChangedSignal.Emit(owner, IsHovered(), event);
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
         (mPressedExecutionKeyCount >= UiConfig::GetCurrent().GetLongPressKeyEventMinimumCount());
}

} // namespace Dali::Ui::Internal
