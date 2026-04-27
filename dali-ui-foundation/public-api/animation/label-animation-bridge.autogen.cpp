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
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES

#include <dali-ui-foundation/public-api/label.h>

namespace Dali
{
namespace Ui
{

LabelAnimationBridge::LabelAnimationBridge(Animation animation, Label view)
: ViewAnimationBridge(animation, view)
{
}

LabelAnimationBridge& LabelAnimationBridge::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundColor(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundColorBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeWidth(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeWidthBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeHeight(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeHeightBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionX(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionXBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionY(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionYBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleX(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleXBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleY(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleYBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Opacity(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::OpacityBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerRadius(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerRadiusBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerSquareness(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerSquarenessBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineWidth(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineWidthBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineColor(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineColorBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineOffset(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineOffsetBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateTo(Property(mView, Label::Property::TEXT_COLOR), target.GetRgba(), alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  mAnimation.AnimateBy(Property(mView, Label::Property::TEXT_COLOR), relative.GetRgba(), alpha, TimePeriod(delay.InSeconds(), duration.InSeconds()));
  return *this;
}

} // namespace Ui
} // namespace Dali
