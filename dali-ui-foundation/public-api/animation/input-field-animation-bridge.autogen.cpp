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
#include <dali-ui-foundation/public-api/animation/input-field-animation-bridge.autogen.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/input-field-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>

namespace Dali
{
namespace Ui
{

InputFieldAnimationBridge::InputFieldAnimationBridge(Animation animation, InputField view)
: ViewAnimationBridge(animation, view)
{
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundColor(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundColorBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowBlurRadius(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowBlurRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowOpacity(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowOpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Size(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeWidth(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeHeight(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeHeightBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Position(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionX(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionXBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionY(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionYBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Scale(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleX(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleXBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleY(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleYBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Color(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ColorBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Opacity(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::OpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerRadius(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerSquareness(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerSquarenessBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineWidth(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineColor(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineColorBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineOffset(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineOffsetBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputFieldAnimationSpecImpl::ApplyTextGradientStartOffsetTo(mAnimation, InputField::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputFieldAnimationSpecImpl::ApplyTextGradientStartOffsetBy(mAnimation, InputField::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PlaceholderTextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputFieldAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetTo(mAnimation, InputField::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

InputFieldAnimationBridge& InputFieldAnimationBridge::PlaceholderTextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputFieldAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetBy(mAnimation, InputField::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

} // namespace Ui
} // namespace Dali
