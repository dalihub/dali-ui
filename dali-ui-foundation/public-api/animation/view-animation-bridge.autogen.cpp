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
  Internal::ViewAnimationSpecImpl::ApplyBackgroundColorTo(mAnimation, mView, {Dali::Property::INVALID_INDEX, target.Resolve(), duration, alpha, delay, nullptr});
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::ViewAnimationSpecImpl::ApplyBackgroundColorBy(mAnimation, mView, {Dali::Property::INVALID_INDEX, relative.Resolve(), duration, alpha, delay, nullptr});
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

ViewAnimationBridge& ViewAnimationBridge::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, Actor::Property::SCALE_X), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, Actor::Property::SCALE_X), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, Actor::Property::SCALE_Y), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, Actor::Property::SCALE_Y), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, Actor::Property::OPACITY), target, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, Actor::Property::OPACITY), relative, alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
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
  mAnimation.AnimateTo(Property(mView, View::Property::BORDERLINE_COLOR), target.Resolve(), alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

ViewAnimationBridge& ViewAnimationBridge::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, View::Property::BORDERLINE_COLOR), relative.Resolve(), alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
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
