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
#include <dali-ui-foundation/public-api/animation/label-animation-spec.autogen.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/label-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

namespace Dali
{
namespace Ui
{
namespace
{

void ApplyLabelTextGradientStartOffsetTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyTextGradientStartOffsetTo(animation, child, entry);
  }
}

void ApplyLabelTextGradientStartOffsetBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyTextGradientStartOffsetBy(animation, child, entry);
  }
}

void ApplyLabelTextGradientOverlayStartOffsetTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyTextGradientOverlayStartOffsetTo(animation, child, entry);
  }
}

void ApplyLabelTextGradientOverlayStartOffsetBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyTextGradientOverlayStartOffsetBy(animation, child, entry);
  }
}

void ApplyLabelTextRevealProgressTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyTextRevealProgressTo(animation, child, entry);
  }
}

void ApplyLabelTextRevealProgressBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyTextRevealProgressBy(animation, child, entry);
  }
}

void ApplyLabelPixelSnapFactorTo(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyPixelSnapFactorTo(animation, child, entry);
  }
}

void ApplyLabelPixelSnapFactorBy(Animation& animation, View view, const Internal::ViewAnimationSpecImpl::Entry& entry)
{
  Label child = Label::DownCast(view);
  if(child)
  {
    Internal::LabelAnimationSpecImpl::ApplyPixelSnapFactorBy(animation, child, entry);
  }
}

} // namespace

LabelAnimationSpec::LabelAnimationSpec() = default;

LabelAnimationSpec LabelAnimationSpec::New()
{
  Internal::LabelAnimationSpecImplPtr p = Internal::LabelAnimationSpecImpl::New();
  return LabelAnimationSpec(p.Get());
}

LabelAnimationSpec LabelAnimationSpec::DownCast(BaseHandle handle)
{
  return LabelAnimationSpec(dynamic_cast<Internal::LabelAnimationSpecImpl*>(handle.GetObjectPtr()));
}

LabelAnimationSpec::LabelAnimationSpec(Internal::LabelAnimationSpecImpl* impl)
: ViewAnimationSpec(impl)
{
}

///////////////////////////////////////////////////////////////////////////////
// Property methods (AddEntry calls)
///////////////////////////////////////////////////////////////////////////////

LabelAnimationSpec& LabelAnimationSpec::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundColor(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundColorBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BackgroundGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundGradientStartOffset(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BackgroundGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BackgroundGradientStartOffsetBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowBlurRadius(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowBlurRadiusBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ShadowOpacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowOpacity(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ShadowOpacityBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::Size(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Size(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeWidth(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeWidthBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeHeight(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::SizeHeightBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::Position(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Position(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionX(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionXBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionY(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::PositionYBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::Scale(const Vector3& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Scale(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleX(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleXBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleY(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ScaleYBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::Color(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Color(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::ColorBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::Opacity(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::OpacityBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerRadius(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerRadiusBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerSquareness(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::CornerSquarenessBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineWidth(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineWidthBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineColor(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineColorBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineOffset(target, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  ViewAnimationSpec::BorderlineOffsetBy(relative, duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Label::Property::TEXT_COLOR, target.GetRgba(), duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Label::Property::TEXT_COLOR, relative.GetRgba(), duration, alpha, delay);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyLabelTextGradientStartOffsetTo);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyLabelTextGradientStartOffsetBy);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextGradientOverlayStartOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyLabelTextGradientOverlayStartOffsetTo);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextGradientOverlayStartOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyLabelTextGradientOverlayStartOffsetBy);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextRevealProgress(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyLabelTextRevealProgressTo);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::TextRevealProgressBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyLabelTextRevealProgressBy);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PixelSnapFactor(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &ApplyLabelPixelSnapFactorTo);
  return *this;
}

LabelAnimationSpec& LabelAnimationSpec::PixelSnapFactorBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &ApplyLabelPixelSnapFactorBy);
  return *this;
}

} // namespace Ui
} // namespace Dali
