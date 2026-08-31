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
#include <dali-ui-components/internal/switch-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/configuration/ui-theme-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{

BaseHandle Create()
{
  return BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(SwitchImpl, Extension::SelectableViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

constexpr float POSITION_EPSILON            = 0.001f;
constexpr float SELECTION_ANIMATION_SECONDS = 0.3f;
const Vector2   SELECTION_ANIMATION_CONTROL_POINT_1(0.22f, 0.25f);
const Vector2   SELECTION_ANIMATION_CONTROL_POINT_2(0.0f, 1.0f);

float Clamp(float value, float minimum, float maximum)
{
  return std::max(minimum, std::min(maximum, value));
}

} // namespace

Ui::Switch SwitchImpl::New(SwitchStyle style)
{
  DALI_ASSERT_ALWAYS(style && "SwitchStyle must be initialized");

  IntrusivePtr<SwitchImpl> impl(new SwitchImpl());
  Ui::Switch               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

void SwitchImpl::SetSelectionAnimationMode(SelectionAnimationMode mode)
{
  mSelectionAnimationMode = mode;
}

SelectionAnimationMode SwitchImpl::GetSelectionAnimationMode() const
{
  return mSelectionAnimationMode;
}

void SwitchImpl::OnInitialize()
{
  Ui::Extension::SelectableViewImpl::OnInitialize();

  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityRole(Accessibility::Role::SWITCH);
  Self().SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_TO_BOUNDING_BOX);

  SelectionChangedSignal().Connect(this, &SwitchImpl::OnSelectionChanged);
  self.StateChangedSignal().Connect(this, &SwitchImpl::OnViewStateChanged);
  self.InterceptTouchEventSignal().Connect(this, &SwitchImpl::OnInterceptTouch);
  self.TouchEventSignal().Connect(this, &SwitchImpl::OnTouch);
  UiThemeManager::Get().ThemeChangedSignal().Connect(this, &SwitchImpl::OnThemeChanged);
}

void SwitchImpl::ApplyInitialStyle(SwitchStyle style)
{
  mStyle              = style;
  mTrackColor         = style.GetTrackColor();
  mSelectedTrackColor = style.GetSelectedTrackColor();
  mThumbColor         = style.GetThumbColor();
  mThumbPosition      = IsSelected() ? GetTravelDistance() : 0.0f;

  Ui::View self = Ui::View::DownCast(Self());
  self.SetMinimumWidth(style.GetMinimumWidth());
  self.SetMinimumHeight(style.GetMinimumHeight());
  self.SetPadding(style.GetPadding());
  self.SetStateEffect(style.GetOverlayEffect());

  mTrack = Ui::View::New();
  mTrack.SetBackgroundColor(IsSelected() ? mSelectedTrackColor : mTrackColor);
  mTrack.SetCornerRadius(0.5f);
  mTrack.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  mTrack.SetAccessibilityHidden(true);

  // The 32x32 container is the primary overlay target. Its visible child is the
  // secondary recoil target, matching OneUI's Switch touch-effect structure.
  mThumbEffectTarget = Ui::View::New();
  mThumbEffectTarget.SetAccessibilityHidden(true);

  mThumb = Ui::View::New();
  mThumb.SetBackgroundColor(mThumbColor);
  mThumb.SetCornerRadius(0.5f);
  mThumb.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  mThumb.SetAccessibilityHidden(true);

  mThumbEffectTarget.Add(mThumb);
  self.Add(mTrack);
  self.Add(mThumbEffectTarget);
  self.SetStateEffectTarget(mThumbEffectTarget);

  InvalidateMeasure();
}

MeasuredSize SwitchImpl::OnMeasure(float /*widthConstraint*/, float /*heightConstraint*/)
{
  const float  scale   = GetEffectiveScale();
  const Insets padding = GetPadding();

  const float naturalWidth  = (mStyle.GetTrackWidth() + padding.start + padding.end) * scale;
  const float naturalHeight = (mStyle.GetTrackHeight() + padding.top + padding.bottom) * scale;

  const float requestedWidth  = GetRequestedWidth();
  const float requestedHeight = GetRequestedHeight();

  const float measuredWidth  = requestedWidth >= 0.0f ? requestedWidth * scale : std::max(naturalWidth, GetMinimumWidth() * scale);
  const float measuredHeight = requestedHeight >= 0.0f ? requestedHeight * scale : std::max(naturalHeight, GetMinimumHeight() * scale);
  return MeasuredSize(measuredWidth, measuredHeight);
}

LayoutRect SwitchImpl::OnArrange(const LayoutRect& bounds)
{
  bool restartTransition = false;
  bool transitionRebased = false;
  if(mTransition && mThumbEffectTargetWidth > POSITION_EPSILON)
  {
    const float targetPosition  = IsSelected() ? GetTravelDistance() : 0.0f;
    const float currentPosition = GetCurrentLogicalPosition();
    CancelTransition();

    mThumbPosition    = Clamp(currentPosition, 0.0f, GetTravelDistance());
    restartTransition = std::fabs(mThumbPosition - targetPosition) > POSITION_EPSILON;
    if(!restartTransition)
    {
      mThumbPosition = targetPosition;
    }
    transitionRebased = true;
  }

  const float  scale        = GetEffectiveScale();
  const Insets padding      = GetPadding();
  const Insets thumbPadding = mStyle.GetThumbPadding();

  const float contentX = padding.start * scale;
  const float contentY = padding.top * scale;
  const float contentW = std::max(0.0f, bounds.width - (padding.start + padding.end) * scale);
  const float contentH = std::max(0.0f, bounds.height - (padding.top + padding.bottom) * scale);

  mArrangedWidth = bounds.width;

  LayoutRect track;
  track.width  = mStyle.GetTrackWidth() * scale;
  track.height = mStyle.GetTrackHeight() * scale;
  track.x      = contentX + (contentW - track.width) * 0.5f;
  track.y      = contentY + (contentH - track.height) * 0.5f;
  GetImpl(mTrack).Measure(track.width, track.height);
  GetImpl(mTrack).Arrange(track);

  LayoutRect effectTarget;
  effectTarget.x          = track.x + mThumbPosition * scale;
  effectTarget.y          = track.y;
  effectTarget.width      = mStyle.GetTrackHeight() * scale;
  effectTarget.height     = mStyle.GetTrackHeight() * scale;
  mThumbEffectTargetBaseX = track.x;
  mThumbEffectTargetWidth = effectTarget.width;
  GetImpl(mThumbEffectTarget).Measure(effectTarget.width, effectTarget.height);
  GetImpl(mThumbEffectTarget).Arrange(effectTarget);

  LayoutRect thumb;
  thumb.x      = thumbPadding.start * scale;
  thumb.y      = thumbPadding.top * scale;
  thumb.width  = std::max(0.0f, (mStyle.GetTrackHeight() - thumbPadding.start - thumbPadding.end) * scale);
  thumb.height = std::max(0.0f, (mStyle.GetTrackHeight() - thumbPadding.top - thumbPadding.bottom) * scale);
  GetImpl(mThumb).Measure(thumb.width, thumb.height);
  GetImpl(mThumb).Arrange(thumb);

  if(transitionRebased)
  {
    ApplyTrackColorForPosition(mThumbPosition);
    if(restartTransition)
    {
      AnimateToSelection(IsSelected());
    }
    else
    {
      mTrack.SetBackgroundColor(IsSelected() ? mSelectedTrackColor : mTrackColor);
    }
  }

  return bounds;
}

void SwitchImpl::OnSelectionChanged(Ui::View view, bool selected, Ui::InputEvent event)
{
  if(selected)
  {
    view.AddAccessibilityState(Accessibility::State::CHECKED);
  }
  else
  {
    view.RemoveAccessibilityState(Accessibility::State::CHECKED);
  }

  if(IsSelectionAnimationRequired(event))
  {
    AnimateToSelection(selected);
  }
  else
  {
    SnapToSelection(selected);
  }
}

void SwitchImpl::OnViewStateChanged(Ui::View view, StateEvent event)
{
  if(event.Added(ViewState::DISABLED))
  {
    view.RemoveAccessibilityState(Accessibility::State::ENABLED);
    mTouchActive = false;
    mDragging    = false;
    SnapToSelection(IsSelected());
  }
  else if(event.Removed(ViewState::DISABLED))
  {
    view.AddAccessibilityState(Accessibility::State::ENABLED);
  }
}

void SwitchImpl::OnThemeChanged()
{
  mThumb.SetBackgroundColor(mThumbColor);
  if(mTransition)
  {
    return;
  }

  if(mDragging)
  {
    ApplyTrackColorForPosition(mThumbPosition);
  }
  else
  {
    mTrack.SetBackgroundColor(IsSelected() ? mSelectedTrackColor : mTrackColor);
  }
}

void SwitchImpl::OnTransitionFinished(Animation animation)
{
  if(!mTransition || animation.GetAnimationId() != mTransition.GetAnimationId())
  {
    return;
  }

  mTrack.SetBackgroundColor(IsSelected() ? mSelectedTrackColor : mTrackColor);
  mThumb.SetBackgroundColor(mThumbColor);
  mTransition.FinishedSignal().Disconnect(this, &SwitchImpl::OnTransitionFinished);
  mTransition.Reset();
}

bool SwitchImpl::OnInterceptTouch(Actor /*actor*/, TouchEvent event)
{
  if(!CanUserToggle())
  {
    if(mTouchActive)
    {
      mTouchActive = false;
      mDragging    = false;
      SnapToSelection(IsSelected());
    }
    return false;
  }

  if(!mTouchActive)
  {
    return false;
  }

  // Once a horizontal drag is recognized, keep the remainder of the sequence
  // on the Switch even when it lives inside a horizontal scroll container.
  if(!mDragging && event.GetPointCount() > 0u && event.GetState(0u) == PointState::MOTION)
  {
    const float scale     = std::max(POSITION_EPSILON, GetEffectiveScale());
    const float x         = event.GetLocalPosition(0u).x;
    const float direction = IsRightToLeft() ? -1.0f : 1.0f;
    const float candidate = Clamp(mThumbPosition + direction * (x - mLastTouchX) / scale, 0.0f, GetTravelDistance());
    if(std::fabs(candidate - mInitialThumbPosition) > POSITION_EPSILON && std::fabs(x - mInitialTouchX) > mStyle.GetDragThreshold() * scale)
    {
      mDragging = true;
    }
  }
  return mDragging;
}

bool SwitchImpl::OnTouch(Actor /*actor*/, TouchEvent event)
{
  if(!CanUserToggle())
  {
    if(mTouchActive)
    {
      mTouchActive = false;
      mDragging    = false;
      SnapToSelection(IsSelected());
    }
    return false;
  }

  if(event.GetPointCount() == 0u)
  {
    return false;
  }

  const PointState::Type state = event.GetState(0u);
  const float            x     = event.GetLocalPosition(0u).x;

  if(state == PointState::DOWN)
  {
    StopTransitionAtCurrentPosition();
    mInitialTouchX        = x;
    mLastTouchX           = x;
    mInitialThumbPosition = mThumbPosition;
    mTouchActive          = true;
    mDragging             = false;
    return false;
  }

  if(state == PointState::MOTION)
  {
    if(!mTouchActive)
    {
      return false;
    }

    const float scale     = std::max(POSITION_EPSILON, GetEffectiveScale());
    const float direction = IsRightToLeft() ? -1.0f : 1.0f;
    const float candidate = Clamp(mThumbPosition + direction * (x - mLastTouchX) / scale, 0.0f, GetTravelDistance());
    mLastTouchX           = x;

    if(!mDragging && std::fabs(candidate - mInitialThumbPosition) > POSITION_EPSILON && std::fabs(x - mInitialTouchX) > mStyle.GetDragThreshold() * scale)
    {
      mDragging = true;
    }

    SetDragPosition(candidate);
    return mDragging;
  }

  if(state == PointState::UP || state == PointState::FINISHED || state == PointState::INTERRUPTED || state == PointState::LEAVE)
  {
    if(!mTouchActive)
    {
      return false;
    }

    const bool wasDragging = mDragging;
    mTouchActive           = false;
    if(wasDragging)
    {
      const bool interrupted    = state == PointState::INTERRUPTED || state == PointState::LEAVE;
      const bool targetSelected = interrupted ? IsSelected() : mThumbPosition >= GetTravelDistance() * 0.5f;

      if(interrupted)
      {
        // OneUI cancels an interrupted gesture immediately rather than
        // treating it as another user selection transition.
        mDragging = false;
        SnapToSelection(IsSelected());
        return true;
      }

      // Keep mDragging true until SetSelected() has emitted SelectionChanged so
      // AUTO mode can distinguish this drag commit from an application call.
      // SetSelected() short-circuits when unchanged, therefore explicitly
      // animate that visual-only return path.
      if(targetSelected == IsSelected())
      {
        if(mSelectionAnimationMode == SelectionAnimationMode::DISABLED)
        {
          SnapToSelection(targetSelected);
        }
        else
        {
          AnimateToSelection(targetSelected);
        }
      }
      else
      {
        SetSelected(targetSelected);
      }
      mDragging = false;
    }
    else if(std::fabs(mThumbPosition - (IsSelected() ? GetTravelDistance() : 0.0f)) > POSITION_EPSILON)
    {
      // A sub-threshold move may not produce a TapGesture. Restore the resting
      // endpoint so no release path can leave the thumb between states. Keep
      // this animated: if the inherited TapGesture toggles immediately after
      // touch dispatch, that selection continues from the current position.
      if(mSelectionAnimationMode == SelectionAnimationMode::DISABLED)
      {
        SnapToSelection(IsSelected());
      }
      else
      {
        AnimateToSelection(IsSelected());
      }
    }
    return wasDragging;
  }

  return mDragging;
}

bool SwitchImpl::CanUserToggle() const
{
  return Self().IsEnabled() && IsClickable() && IsToggleByClickEnabled();
}

bool SwitchImpl::IsSelectionAnimationRequired(const InputEvent& event) const
{
  if(mSelectionAnimationMode == SelectionAnimationMode::DISABLED)
  {
    return false;
  }
  if(!(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE) && Self().IsVisible()))
  {
    return false;
  }
  if(mThumbEffectTargetWidth <= POSITION_EPSILON)
  {
    return false;
  }
  if(mSelectionAnimationMode == SelectionAnimationMode::ENABLED)
  {
    return true;
  }
  return mDragging || !event.IsProgrammatic();
}

void SwitchImpl::CancelTransition()
{
  if(!mTransition)
  {
    return;
  }

  mTransition.FinishedSignal().Disconnect(this, &SwitchImpl::OnTransitionFinished);
  mTransition.Stop();
  mTransition.Reset();
}

void SwitchImpl::StopTransitionAtCurrentPosition()
{
  if(!mTransition)
  {
    return;
  }

  const float currentX       = mThumbEffectTarget.GetCurrentProperty<float>(Actor::Property::POSITION_X);
  const float currentLogical = GetCurrentLogicalPosition();
  CancelTransition();

  mThumbPosition = currentLogical;
  mThumbEffectTarget.SetProperty(Actor::Property::POSITION_X, currentX);
  ApplyTrackColorForPosition(mThumbPosition);
}

void SwitchImpl::AnimateToSelection(bool selected)
{
  const float targetPosition = selected ? GetTravelDistance() : 0.0f;
  const float sourcePosition = mTransition ? GetCurrentLogicalPosition() : mThumbPosition;

  if(mTransition)
  {
    const float currentX = mThumbEffectTarget.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    CancelTransition();
    mThumbEffectTarget.SetProperty(Actor::Property::POSITION_X, currentX);
  }

  const float travelDistance = GetTravelDistance();
  const float distanceRatio  = travelDistance > POSITION_EPSILON ? std::fabs(targetPosition - sourcePosition) / travelDistance : 0.0f;
  const float duration       = SELECTION_ANIMATION_SECONDS * distanceRatio;

  mThumbPosition = targetPosition;
  if(duration <= POSITION_EPSILON)
  {
    SnapToSelection(selected);
    return;
  }

  const AlphaFunction easing(SELECTION_ANIMATION_CONTROL_POINT_1, SELECTION_ANIMATION_CONTROL_POINT_2);
  const Duration      timing(duration);

  mTransition = Animation::New(duration);
  mThumbEffectTarget.Animate(mTransition).PositionX(LogicalPositionToActorX(targetPosition), timing, easing);
  mTrack.Animate(mTransition).BackgroundColor(selected ? mSelectedTrackColor : mTrackColor, timing, easing);
  mTransition.FinishedSignal().Connect(this, &SwitchImpl::OnTransitionFinished);
  mTransition.Play();
}

void SwitchImpl::SnapToSelection(bool selected)
{
  if(mTransition)
  {
    CancelTransition();
  }

  mThumbPosition = selected ? GetTravelDistance() : 0.0f;
  mTrack.SetBackgroundColor(selected ? mSelectedTrackColor : mTrackColor);
  mThumb.SetBackgroundColor(mThumbColor);
  InvalidateArrange();
}

void SwitchImpl::SetDragPosition(float position)
{
  if(mTransition)
  {
    CancelTransition();
  }

  mThumbPosition = Clamp(position, 0.0f, GetTravelDistance());
  mThumbEffectTarget.SetProperty(Actor::Property::POSITION_X, LogicalPositionToActorX(mThumbPosition));
  ApplyTrackColorForPosition(mThumbPosition);
}

void SwitchImpl::ApplyTrackColorForPosition(float position)
{
  const float   travel = GetTravelDistance();
  const float   ratio  = travel > POSITION_EPSILON ? Clamp(position / travel, 0.0f, 1.0f) : (IsSelected() ? 1.0f : 0.0f);
  const Vector4 from   = mTrackColor.GetRgba();
  const Vector4 to     = mSelectedTrackColor.GetRgba();
  mTrack.SetBackgroundColor(UiColor(from + (to - from) * ratio));
}

float SwitchImpl::GetTravelDistance() const
{
  return std::max(0.0f, mStyle.GetTrackWidth() - mStyle.GetTrackHeight());
}

float SwitchImpl::GetCurrentLogicalPosition() const
{
  const float scale = GetEffectiveScale();
  if(!mThumbEffectTarget || scale <= POSITION_EPSILON)
  {
    return mThumbPosition;
  }
  const float currentX = mThumbEffectTarget.GetCurrentProperty<float>(Actor::Property::POSITION_X);
  const float logicalX = IsRightToLeft() ? mArrangedWidth - currentX - mThumbEffectTargetWidth : currentX;
  return Clamp((logicalX - mThumbEffectTargetBaseX) / scale, 0.0f, GetTravelDistance());
}

float SwitchImpl::LogicalPositionToActorX(float position) const
{
  const float logicalX = mThumbEffectTargetBaseX + position * GetEffectiveScale();
  return IsRightToLeft() ? mArrangedWidth - logicalX - mThumbEffectTargetWidth : logicalX;
}

bool SwitchImpl::IsRightToLeft() const
{
  return Self().GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT;
}

SwitchImpl::SwitchImpl() = default;

SwitchImpl::~SwitchImpl()
{
  CancelTransition();
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
