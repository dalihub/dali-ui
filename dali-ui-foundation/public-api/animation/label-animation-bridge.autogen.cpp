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
#include <dali-ui-foundation/internal/animation/label-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

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

LabelAnimationBridge& LabelAnimationBridge::BackgroundGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundGradientStartOffset(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::BackgroundGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundGradientStartOffsetBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowBlurRadius(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowBlurRadiusBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowOpacity(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowOpacityBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Size(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeBy(relative, duration, alpha, delay);
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

LabelAnimationBridge& LabelAnimationBridge::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Position(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionBy(relative, duration, alpha, delay);
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

LabelAnimationBridge& LabelAnimationBridge::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Scale(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleBy(relative, duration, alpha, delay);
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

LabelAnimationBridge& LabelAnimationBridge::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Color(target, duration, alpha, delay);
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ColorBy(relative, duration, alpha, delay);
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

LabelAnimationBridge& LabelAnimationBridge::TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyTextGradientStartOffsetTo(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyTextGradientStartOffsetBy(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextGradientOverlayStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyTextGradientOverlayStartOffsetTo(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextGradientOverlayStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyTextGradientOverlayStartOffsetBy(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextRevealProgress(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyTextRevealProgressTo(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::TextRevealProgressBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyTextRevealProgressBy(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PixelSnapFactor(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyPixelSnapFactorTo(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

LabelAnimationBridge& LabelAnimationBridge::PixelSnapFactorBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::LabelAnimationSpecImpl::ApplyPixelSnapFactorBy(mAnimation, Label::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

} // namespace Ui
} // namespace Dali
