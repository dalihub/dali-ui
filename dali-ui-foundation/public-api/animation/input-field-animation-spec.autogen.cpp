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
#include <dali-ui-foundation/public-api/animation/input-field-animation-spec.autogen.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/input-field-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-field.h>

namespace Dali
{
namespace Ui
{
namespace
{

void ApplyInputFieldTextGradientStartOffsetTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputField child = InputField::DownCast(view);
  if(child)
  {
    Internal::InputFieldAnimationSpecImpl::ApplyTextGradientStartOffsetTo(animation, child, entry);
  }
}

void ApplyInputFieldTextGradientStartOffsetBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputField child = InputField::DownCast(view);
  if(child)
  {
    Internal::InputFieldAnimationSpecImpl::ApplyTextGradientStartOffsetBy(animation, child, entry);
  }
}

void ApplyInputFieldPlaceholderTextGradientStartOffsetTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputField child = InputField::DownCast(view);
  if(child)
  {
    Internal::InputFieldAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetTo(animation, child, entry);
  }
}

void ApplyInputFieldPlaceholderTextGradientStartOffsetBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputField child = InputField::DownCast(view);
  if(child)
  {
    Internal::InputFieldAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetBy(animation, child, entry);
  }
}

} // namespace

InputFieldAnimationSpec::InputFieldAnimationSpec() = default;

InputFieldAnimationSpec InputFieldAnimationSpec::New()
{
  Internal::InputFieldAnimationSpecImplPtr p = Internal::InputFieldAnimationSpecImpl::New();
  return InputFieldAnimationSpec(p.Get());
}

InputFieldAnimationSpec InputFieldAnimationSpec::DownCast(BaseHandle handle)
{
  return InputFieldAnimationSpec(dynamic_cast<Internal::InputFieldAnimationSpecImpl*>(handle.GetObjectPtr()));
}

InputFieldAnimationSpec::InputFieldAnimationSpec(Internal::InputFieldAnimationSpecImpl* impl)
: ViewAnimationSpec(impl)
{
}

///////////////////////////////////////////////////////////////////////////////
// Property methods (AddEntry calls)
///////////////////////////////////////////////////////////////////////////////

InputFieldAnimationSpec& InputFieldAnimationSpec::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundColor(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundColorBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowBlurRadius(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowBlurRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowOpacity(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowOpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Size(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeWidth(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeHeight(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeHeightBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Position(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionX(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionXBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionY(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionYBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Scale(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleX(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleXBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleY(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleYBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Color(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ColorBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Opacity(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::OpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerRadius(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerSquareness(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerSquarenessBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineWidth(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineColor(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineColorBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineOffset(target, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineOffsetBy(relative, duration, alpha, delay);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyInputFieldTextGradientStartOffsetTo);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyInputFieldTextGradientStartOffsetBy);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PlaceholderTextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyInputFieldPlaceholderTextGradientStartOffsetTo);
  return *this;
}

InputFieldAnimationSpec& InputFieldAnimationSpec::PlaceholderTextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyInputFieldPlaceholderTextGradientStartOffsetBy);
  return *this;
}

} // namespace Ui
} // namespace Dali
