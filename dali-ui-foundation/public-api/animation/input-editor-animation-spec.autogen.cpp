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
#include <dali-ui-foundation/public-api/animation/input-editor-animation-spec.autogen.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/input-editor-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>

namespace Dali
{
namespace Ui
{
namespace
{

void ApplyInputEditorTextGradientStartOffsetTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputEditor child = InputEditor::DownCast(view);
  if(child)
  {
    Internal::InputEditorAnimationSpecImpl::ApplyTextGradientStartOffsetTo(animation, child, entry);
  }
}

void ApplyInputEditorTextGradientStartOffsetBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputEditor child = InputEditor::DownCast(view);
  if(child)
  {
    Internal::InputEditorAnimationSpecImpl::ApplyTextGradientStartOffsetBy(animation, child, entry);
  }
}

void ApplyInputEditorPlaceholderTextGradientStartOffsetTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputEditor child = InputEditor::DownCast(view);
  if(child)
  {
    Internal::InputEditorAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetTo(animation, child, entry);
  }
}

void ApplyInputEditorPlaceholderTextGradientStartOffsetBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  InputEditor child = InputEditor::DownCast(view);
  if(child)
  {
    Internal::InputEditorAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetBy(animation, child, entry);
  }
}

} // namespace

InputEditorAnimationSpec::InputEditorAnimationSpec() = default;

InputEditorAnimationSpec InputEditorAnimationSpec::New()
{
  Internal::InputEditorAnimationSpecImplPtr p = Internal::InputEditorAnimationSpecImpl::New();
  return InputEditorAnimationSpec(p.Get());
}

InputEditorAnimationSpec InputEditorAnimationSpec::DownCast(BaseHandle handle)
{
  return InputEditorAnimationSpec(dynamic_cast<Internal::InputEditorAnimationSpecImpl*>(handle.GetObjectPtr()));
}

InputEditorAnimationSpec::InputEditorAnimationSpec(Internal::InputEditorAnimationSpecImpl* impl)
: ViewAnimationSpec(impl)
{
}

///////////////////////////////////////////////////////////////////////////////
// Property methods (AddEntry calls)
///////////////////////////////////////////////////////////////////////////////

InputEditorAnimationSpec& InputEditorAnimationSpec::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundColor(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundColorBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowBlurRadius(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowBlurRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowOpacity(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowOpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Size(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeWidth(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeHeight(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeHeightBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Position(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionX(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionXBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionY(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionYBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Scale(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleX(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleXBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleY(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleYBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Color(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ColorBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Opacity(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::OpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerRadius(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerSquareness(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerSquarenessBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineWidth(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineColor(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineColorBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineOffset(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineOffsetBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyInputEditorTextGradientStartOffsetTo);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyInputEditorTextGradientStartOffsetBy);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PlaceholderTextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyInputEditorPlaceholderTextGradientStartOffsetTo);
  return *this;
}

InputEditorAnimationSpec& InputEditorAnimationSpec::PlaceholderTextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyInputEditorPlaceholderTextGradientStartOffsetBy);
  return *this;
}

} // namespace Ui
} // namespace Dali
