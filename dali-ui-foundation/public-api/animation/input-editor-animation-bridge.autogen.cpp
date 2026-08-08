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
#include <dali-ui-foundation/public-api/animation/input-editor-animation-bridge.autogen.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/input-editor-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/input-editor.h>

namespace Dali
{
namespace Ui
{

InputEditorAnimationBridge::InputEditorAnimationBridge(Animation animation, InputEditor view)
: ViewAnimationBridge(animation, view)
{
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundColor(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundColorBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BackgroundGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundGradientStartOffset(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BackgroundGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BackgroundGradientStartOffsetBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowBlurRadius(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowBlurRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowOpacity(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ShadowOpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Size(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeWidth(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeHeight(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::SizeHeightBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Position(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionX(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionXBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionY(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::PositionYBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Scale(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleX(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleXBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleY(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ScaleYBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Color(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::ColorBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::Opacity(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::OpacityBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerRadius(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerRadiusBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerSquareness(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::CornerSquarenessBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineWidth(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineWidthBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineColor(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineColorBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineOffset(target, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationBridge::BorderlineOffsetBy(relative, duration, alpha, delay);
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputEditorAnimationSpecImpl::ApplyTextGradientStartOffsetTo(mAnimation, InputEditor::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputEditorAnimationSpecImpl::ApplyTextGradientStartOffsetBy(mAnimation, InputEditor::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PlaceholderTextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputEditorAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetTo(mAnimation, InputEditor::DownCast(mView), {Dali::Property::INVALID_INDEX, target, duration, alpha, delay, nullptr});
  return *this;
}

InputEditorAnimationBridge& InputEditorAnimationBridge::PlaceholderTextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ExtendIfNeeded(delay, duration);
  Internal::InputEditorAnimationSpecImpl::ApplyPlaceholderTextGradientStartOffsetBy(mAnimation, InputEditor::DownCast(mView), {Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, nullptr});
  return *this;
}

} // namespace Ui
} // namespace Dali
