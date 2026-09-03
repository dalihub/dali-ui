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

#include <dali-ui-components/internal/progress-bar-impl.h>

#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/math/math-utils.h>

#include <algorithm>
#include <cmath>
#include <utility>

#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
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

DALI_TYPE_REGISTRATION_BEGIN(ProgressBarImpl, Extension::ProgressViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

constexpr float DIVIDER_WIDTH                 = 1.0f;
constexpr float INDETERMINATE_REFERENCE_WIDTH = 1024.0f;
constexpr float INDETERMINATE_DURATION        = 1.916f;

const Vector2 INDETERMINATE_ALPHA_1_CONTROL_POINT_1(0.33f, 0.0f);
const Vector2 INDETERMINATE_ALPHA_1_CONTROL_POINT_2(0.1f, 1.0f);
const Vector2 INDETERMINATE_ALPHA_2_CONTROL_POINT_1(0.33f, 0.0f);
const Vector2 INDETERMINATE_ALPHA_2_CONTROL_POINT_2(0.2f, 1.0f);

bool PositionsEqual(const Vector2& lhs, const Vector2& rhs)
{
  return Dali::Equals(lhs.x, rhs.x) && Dali::Equals(lhs.y, rhs.y);
}

void SetRenderedGeometry(View view, float x, float y, float width, float height)
{
  Dali::Ui::Extension::View::SetPositionX(view, x);
  Dali::Ui::Extension::View::SetPositionY(view, y);
  Dali::Ui::Extension::View::SetSizeWidth(view, width);
  Dali::Ui::Extension::View::SetSizeHeight(view, height);
}

float ResolveMeasuredDimension(float requested,
                               float natural,
                               float minimum,
                               float scale)
{
  if(requested >= 0.0f)
  {
    return requested * scale;
  }
  if(requested == MATCH_PARENT)
  {
    return minimum * scale;
  }
  return std::max(natural, minimum * scale);
}

} // namespace

Ui::ProgressBar ProgressBarImpl::New(float minimumValue, float maximumValue, ProgressBarStyle style)
{
  DALI_ASSERT_ALWAYS(style && "ProgressBarStyle must be initialized");

  IntrusivePtr<ProgressBarImpl> impl(new ProgressBarImpl(minimumValue, maximumValue));
  Ui::ProgressBar               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

ProgressBarImpl::ProgressBarImpl(float minimumValue, float maximumValue)
: Dali::Ui::Extension::ProgressViewImpl(minimumValue, maximumValue)
{
}

ProgressBarImpl::~ProgressBarImpl() = default;

void ProgressBarImpl::OnInitialize()
{
  Dali::Ui::Extension::ProgressViewImpl::OnInitialize();

  Ui::View self = Ui::View::DownCast(Self());
  self.SetRequestedWidth(MATCH_PARENT);
  self.SetRequestedHeight(WRAP_CONTENT);

  mTrack = Ui::View::New();
  mTrack.SetCornerRadius(0.5f);
  mTrack.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  mTrack.SetAccessibilityHidden(true);
  mTrack.SetMeasureCallback(MeasureCallback::New(&ProgressBarImpl::MeasureTrackSurface));
  mTrack.SetArrangeCallback(ArrangeCallback::New(&ProgressBarImpl::ArrangeTrackSurface));

  mTrail = Ui::View::New();
  mTrail.SetCornerRadius(0.5f);
  mTrail.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  mTrail.SetAccessibilityHidden(true);

  mTrack.Add(mTrail);
  for(View& bar : mIndeterminateBars)
  {
    bar = View::New();
    bar.SetCornerRadius(0.5f);
    bar.SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
    bar.SetAccessibilityHidden(true);
    bar.SetVisible(false);
    mTrack.Add(bar);
  }
  self.Add(mTrack);
}

void ProgressBarImpl::OnDestroy()
{
  if(mGeometryIdleCallback)
  {
    if(Adaptor::IsAvailable())
    {
      Adaptor::Get().RemoveIdle(mGeometryIdleCallback);
    }
    mGeometryIdleCallback = nullptr;
  }
  mTrailGeometryChangedPending = false;
  mDividersChangedPending      = false;
  ClearIndeterminateAnimation();
  Dali::Ui::Extension::ProgressViewImpl::OnDestroy();
}

void ProgressBarImpl::OnSceneConnection(int depth)
{
  Dali::Ui::Extension::ProgressViewImpl::OnSceneConnection(depth);
  mOnScene = true;
  if(!IsDeterminate())
  {
    StartIndeterminateAnimation();
  }
}

void ProgressBarImpl::OnSceneDisconnection()
{
  mOnScene = false;
  ClearIndeterminateAnimation();
  Dali::Ui::Extension::ProgressViewImpl::OnSceneDisconnection();
}

MeasuredSize ProgressBarImpl::MeasureTrackSurface(View, float widthConstraint, float heightConstraint)
{
  return MeasuredSize(std::max(0.0f, widthConstraint), std::max(0.0f, heightConstraint));
}

LayoutRect ProgressBarImpl::ArrangeTrackSurface(View, const LayoutRect& bounds)
{
  return bounds;
}

void ProgressBarImpl::ApplyInitialStyle(ProgressBarStyle style)
{
  mTrackThickness = style.GetTrackThickness();
  mTrackColor     = style.GetTrackColor();
  mTrailColor     = style.GetTrailColor();
  mDividerColor   = style.GetDividerColor();

  Ui::View self = Ui::View::DownCast(Self());
  self.SetPadding(style.GetPadding());
  mTrack.SetBackgroundColor(mTrackColor);
  mTrail.SetBackgroundColor(mTrailColor);
  for(View bar : mIndeterminateBars)
  {
    bar.SetBackgroundColor(mTrailColor);
  }

  const float  scale   = GetEffectiveScale();
  const Insets padding = GetPadding();
  mTrackBounds         = LayoutRect(padding.start * scale, padding.top * scale, 0.0f, 0.0f);
  UpdateLocalGeometry();
  InvalidateMeasure();
}

void ProgressBarImpl::SetTrackThickness(float thickness)
{
  DALI_ASSERT_ALWAYS(std::isfinite(thickness) && thickness > 0.0f &&
                     "ProgressBar track thickness must be finite and positive");
  if(Dali::Equals(thickness, mTrackThickness))
  {
    return;
  }

  mTrackThickness = thickness;
  ClearIndeterminateAnimation();
  InvalidateMeasure();
}

float ProgressBarImpl::GetTrackThickness() const
{
  return mTrackThickness;
}

void ProgressBarImpl::SetTrackColor(const UiColor& color)
{
  if(color == mTrackColor)
  {
    return;
  }
  mTrackColor = color;
  mTrack.SetBackgroundColor(color);
}

UiColor ProgressBarImpl::GetTrackColor() const
{
  return mTrackColor;
}

void ProgressBarImpl::SetTrailColor(const UiColor& color)
{
  if(color == mTrailColor)
  {
    return;
  }
  mTrailColor = color;
  mTrail.SetBackgroundColor(color);
  for(View bar : mIndeterminateBars)
  {
    if(bar)
    {
      bar.SetBackgroundColor(color);
    }
  }
}

UiColor ProgressBarImpl::GetTrailColor() const
{
  return mTrailColor;
}

void ProgressBarImpl::SetDividerStepCount(int32_t count)
{
  DALI_ASSERT_ALWAYS(count >= 0 && "ProgressBar divider step count must be non-negative");
  if(count == mDividerStepCount)
  {
    return;
  }

  mDividerStepCount           = count;
  const uint32_t dividerCount = count > 1 ? static_cast<uint32_t>(count - 1) : 0u;
  EnsureDividerCount(dividerCount);
  UpdateLocalGeometry();
}

int32_t ProgressBarImpl::GetDividerStepCount() const
{
  return mDividerStepCount;
}

void ProgressBarImpl::SetReversed(bool reversed)
{
  if(reversed == mReversed)
  {
    return;
  }
  mReversed = reversed;
  UpdateLocalGeometry();
}

bool ProgressBarImpl::IsReversed() const
{
  return mReversed;
}

ProgressBar::TrailEndPositionChangedSignalType& ProgressBarImpl::TrailEndPositionChangedSignal()
{
  return mTrailEndPositionChangedSignal;
}

ProgressBar::DividersChangedSignalType& ProgressBarImpl::DividersChangedSignal()
{
  return mDividersChangedSignal;
}

Vector2 ProgressBarImpl::GetTrailEndPosition() const
{
  return mTrailEndPosition;
}

uint32_t ProgressBarImpl::GetDividerCount() const
{
  return static_cast<uint32_t>(mDividerPositions.size());
}

Vector2 ProgressBarImpl::GetDividerPosition(uint32_t index) const
{
  DALI_ASSERT_ALWAYS(index < mDividerPositions.size() && "ProgressBar divider index is out of range");
  return mDividerPositions[index];
}

MeasuredSize ProgressBarImpl::OnMeasure(float, float)
{
  const float  scale          = GetEffectiveScale();
  const Insets padding        = GetPadding();
  const float  naturalWidth   = (padding.start + padding.end) * scale;
  const float  naturalHeight  = (padding.top + padding.bottom + mTrackThickness) * scale;
  const float  measuredWidth  = ResolveMeasuredDimension(GetRequestedWidth(), naturalWidth, GetMinimumWidth(), scale);
  const float  measuredHeight = ResolveMeasuredDimension(GetRequestedHeight(), naturalHeight, GetMinimumHeight(), scale);
  return MeasuredSize(measuredWidth, measuredHeight);
}

LayoutRect ProgressBarImpl::OnArrange(const LayoutRect& bounds)
{
  const float  scale                        = GetEffectiveScale();
  const Insets padding                      = GetPadding();
  const float  contentX                     = padding.start * scale;
  const float  contentY                     = padding.top * scale;
  const float  contentWidth                 = std::max(0.0f, bounds.width - (padding.start + padding.end) * scale);
  const float  contentHeight                = std::max(0.0f, bounds.height - (padding.top + padding.bottom) * scale);
  const float  trackHeight                  = std::min(mTrackThickness * scale, contentHeight);
  const bool   indeterminateGeometryChanged = !Dali::Equals(contentWidth, mTrackBounds.width) ||
                                            !Dali::Equals(trackHeight, mTrackBounds.height);

  mArrangedWidth = bounds.width;
  mTrackBounds   = LayoutRect(contentX,
                              contentY + (contentHeight - trackHeight) * 0.5f,
                              contentWidth,
                              trackHeight);

  Ui::GetImpl(mTrack).Measure(mTrackBounds.width, mTrackBounds.height);
  Ui::GetImpl(mTrack).Arrange(mTrackBounds);
  UpdateLocalGeometry();

  if(!IsDeterminate() && (!mIndeterminateAnimation || indeterminateGeometryChanged))
  {
    StartIndeterminateAnimation();
  }
  return bounds;
}

void ProgressBarImpl::OnValueRatioChanged(float)
{
  UpdateLocalGeometry();
}

void ProgressBarImpl::OnDeterminateChanged(bool determinate)
{
  if(determinate)
  {
    ClearIndeterminateAnimation();
    SetIndeterminateOffScreenRendering(false);
    mTrail.SetVisible(true);
    for(View divider : mDividers)
    {
      divider.SetVisible(true);
    }
    UpdateLocalGeometry();
  }
  else
  {
    mTrail.SetVisible(false);
    for(View divider : mDividers)
    {
      divider.SetVisible(false);
    }
    SetIndeterminateOffScreenRendering(true);
    StartIndeterminateAnimation();
  }
}

void ProgressBarImpl::EnsureDividerCount(uint32_t count)
{
  while(mDividers.size() > count)
  {
    mTrack.Remove(mDividers.back());
    mDividers.pop_back();
  }

  while(mDividers.size() < count)
  {
    View divider = View::New();
    divider.SetBackgroundColor(mDividerColor);
    divider.SetAccessibilityHidden(true);
    divider.SetVisible(IsDeterminate());
    mTrack.Add(divider);
    mDividers.push_back(divider);
  }
}

void ProgressBarImpl::UpdateLocalGeometry()
{
  if(!mTrack || !mTrail)
  {
    return;
  }

  const float trackWidth        = std::max(0.0f, mTrackBounds.width);
  const float trackHeight       = std::max(0.0f, mTrackBounds.height);
  const float ratio             = std::max(0.0f, std::min(1.0f, GetValueRatio()));
  const float mathematicalWidth = trackWidth * ratio;
  const float visualWidth       = Dali::Equals(mathematicalWidth, 0.0f)
                                    ? 0.0f
                                    : std::max(trackHeight, mathematicalWidth);
  const bool  rtl               = IsRightToLeft();
  const bool  fillFromRight     = rtl != mReversed;
  const float trailX            = fillFromRight ? trackWidth - visualWidth : 0.0f;

  SetRenderedGeometry(mTrail, trailX, 0.0f, visualWidth, trackHeight);

  const float visualTrackX = rtl
                               ? mArrangedWidth - mTrackBounds.x - trackWidth
                               : mTrackBounds.x;
  const float endpointX    = fillFromRight
                               ? visualTrackX + trackWidth - mathematicalWidth
                               : visualTrackX + mathematicalWidth;
  CommitTrailSnapshot(Vector2(endpointX, mTrackBounds.y + trackHeight * 0.5f));

  std::vector<Vector2> dividerPositions;
  dividerPositions.reserve(mDividers.size());
  if(mDividerStepCount > 1)
  {
    const float stepWidth    = trackWidth / static_cast<float>(mDividerStepCount);
    const float dividerWidth = DIVIDER_WIDTH * GetEffectiveScale();
    for(uint32_t index = 0u; index < mDividers.size(); ++index)
    {
      const float logicalX = static_cast<float>(index + 1u) * stepWidth;
      const float visualX  = rtl ? trackWidth - logicalX : logicalX;
      SetRenderedGeometry(mDividers[index], visualX, 0.0f, dividerWidth, trackHeight);
      dividerPositions.emplace_back(visualTrackX + visualX, mTrackBounds.y + trackHeight * 0.5f);
    }
  }
  CommitDividerSnapshot(std::move(dividerPositions));
}

void ProgressBarImpl::CommitTrailSnapshot(const Vector2& position)
{
  if(PositionsEqual(position, mTrailEndPosition))
  {
    return;
  }
  mTrailEndPosition            = position;
  mTrailGeometryChangedPending = true;
  QueueGeometryNotification();
}

void ProgressBarImpl::CommitDividerSnapshot(std::vector<Vector2> positions)
{
  bool changed = positions.size() != mDividerPositions.size();
  if(!changed)
  {
    for(uint32_t index = 0u; index < positions.size(); ++index)
    {
      if(!PositionsEqual(positions[index], mDividerPositions[index]))
      {
        changed = true;
        break;
      }
    }
  }

  mDividerPositions = std::move(positions);
  if(changed)
  {
    mDividersChangedPending = true;
    QueueGeometryNotification();
  }
}

void ProgressBarImpl::QueueGeometryNotification()
{
  if(mGeometryIdleCallback)
  {
    return;
  }
  if(!Adaptor::IsAvailable())
  {
    mTrailGeometryChangedPending = false;
    mDividersChangedPending      = false;
    return;
  }

  mGeometryIdleCallback = MakeCallback(this, &ProgressBarImpl::EmitGeometryNotifications);
  if(!Adaptor::Get().AddIdle(mGeometryIdleCallback, false))
  {
    mGeometryIdleCallback        = nullptr;
    mTrailGeometryChangedPending = false;
    mDividersChangedPending      = false;
  }
}

void ProgressBarImpl::EmitGeometryNotifications()
{
  mGeometryIdleCallback        = nullptr;
  const bool emitTrail         = mTrailGeometryChangedPending;
  const bool emitDividers      = mDividersChangedPending;
  mTrailGeometryChangedPending = false;
  mDividersChangedPending      = false;

  Ui::ProgressBar self = Ui::ProgressBar::DownCast(Self());
  if(emitTrail && !mTrailEndPositionChangedSignal.Empty())
  {
    mTrailEndPositionChangedSignal.Emit(self);
  }
  if(emitDividers && !mDividersChangedSignal.Empty())
  {
    mDividersChangedSignal.Emit(self);
  }
}

void ProgressBarImpl::SetIndeterminateOffScreenRendering(bool enabled)
{
  // TODO: View's public offscreen-rendering API must keep user and system values
  // separately. Replace this temporary direct property write with a system-value
  // request so mode changes preserve the application-provided user value.
  mTrack.SetProperty(Ui::View::Property::OFFSCREEN_RENDERING,
                     enabled ? Ui::View::OffScreenRenderingType::REFRESH_ALWAYS
                             : Ui::View::OffScreenRenderingType::NONE);
}

void ProgressBarImpl::StartIndeterminateAnimation()
{
  ClearIndeterminateAnimation();
  if(!mOnScene || IsDeterminate() || mTrackBounds.width <= 0.0f || mTrackBounds.height <= 0.0f)
  {
    return;
  }

  const float                 trackWidth  = mTrackBounds.width;
  const float                 trackHeight = mTrackBounds.height;
  const float                 widthRatio  = trackWidth / INDETERMINATE_REFERENCE_WIDTH;
  const std::array<float, 4u> initialWidths{
    672.0f * widthRatio,
    397.0f * widthRatio,
    249.0f * widthRatio,
    trackHeight};

  for(uint32_t index = 0u; index < mIndeterminateBars.size(); ++index)
  {
    View bar = mIndeterminateBars[index];
    SetRenderedGeometry(bar, -initialWidths[index], 0.0f, initialWidths[index], trackHeight);
    bar.SetVisible(true);
  }

  const AlphaFunction alpha1(INDETERMINATE_ALPHA_1_CONTROL_POINT_1,
                             INDETERMINATE_ALPHA_1_CONTROL_POINT_2);
  const AlphaFunction alpha2(INDETERMINATE_ALPHA_2_CONTROL_POINT_1,
                             INDETERMINATE_ALPHA_2_CONTROL_POINT_2);

  mIndeterminateAnimation = Animation::New(INDETERMINATE_DURATION);
  mIndeterminateAnimation.SetLoopCount(Animation::INFINITE_LOOP);

  mIndeterminateBars[0].Animate(mIndeterminateAnimation).SizeWidth(std::max(99.0f * widthRatio, trackHeight), Duration(0.980f), alpha2, Duration(0.350f));
  mIndeterminateBars[0].Animate(mIndeterminateAnimation).PositionX(trackWidth, Duration(1.280f), alpha1);

  mIndeterminateBars[1].Animate(mIndeterminateAnimation).SizeWidth(std::max(52.0f * widthRatio, trackHeight), Duration(0.980f), alpha1, Duration(0.566f));
  mIndeterminateBars[1].Animate(mIndeterminateAnimation).PositionX(trackWidth, Duration(1.200f), alpha1, Duration(0.350f));

  mIndeterminateBars[2].Animate(mIndeterminateAnimation).SizeWidth(std::max(16.0f * widthRatio, trackHeight), Duration(1.250f), alpha1, Duration(0.500f));
  mIndeterminateBars[2].Animate(mIndeterminateAnimation).PositionX(trackWidth, Duration(1.250f), alpha1, Duration(0.500f));

  mIndeterminateBars[3].Animate(mIndeterminateAnimation).PositionX(trackWidth, Duration(1.250f), alpha1, Duration(0.666f));

  mIndeterminateAnimation.Play();
}

void ProgressBarImpl::ClearIndeterminateAnimation()
{
  if(mIndeterminateAnimation)
  {
    mIndeterminateAnimation.Stop();
    mIndeterminateAnimation.Reset();
  }

  for(View& bar : mIndeterminateBars)
  {
    if(bar)
    {
      bar.SetVisible(false);
    }
  }
}

bool ProgressBarImpl::IsRightToLeft() const
{
  return Self().GetEffectiveLayoutDirection() == LayoutDirection::RIGHT_TO_LEFT;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
