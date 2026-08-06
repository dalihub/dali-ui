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
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/view-animation-spec-impl.autogen.h>

namespace Dali
{
namespace Ui
{

ViewAnimationBridge::ViewAnimationBridge(Animation animation, View view)
: mAnimation(animation),
  mView(view)
{
}

ViewAnimationBridge& ViewAnimationBridge::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyBackgroundColorTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target.GetRgba(), duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyBackgroundColorBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative.GetRgba(), duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BackgroundGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyBackgroundGradientStartOffsetTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BackgroundGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyBackgroundGradientStartOffsetBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyShadowBlurRadiusTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyShadowBlurRadiusBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyShadowOpacityTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyShadowOpacityBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplySizeTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplySizeBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplySizeWidthTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplySizeWidthBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplySizeHeightTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplySizeHeightBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyPositionTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyPositionBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyPositionXTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyPositionXBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyPositionYTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyPositionYBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyScaleTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyScaleBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyScaleXTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyScaleXBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyScaleYTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyScaleYBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyColorTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyColorBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyOpacityTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyOpacityBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, View::Property::CORNER_RADIUS), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, View::Property::CORNER_RADIUS), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, View::Property::CORNER_SQUARENESS), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, View::Property::CORNER_SQUARENESS), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, View::Property::BORDERLINE_WIDTH), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, View::Property::BORDERLINE_WIDTH), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, View::Property::BORDERLINE_COLOR), target.GetRgba(), alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, View::Property::BORDERLINE_COLOR), relative.GetRgba(), alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, View::Property::BORDERLINE_OFFSET), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, View::Property::BORDERLINE_OFFSET), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

} // namespace Ui
} // namespace Dali
