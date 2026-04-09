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
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/view-animation-spec-impl.autogen.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

ViewAnimationSpec::ViewAnimationSpec() = default;

ViewAnimationSpec::ViewAnimationSpec(const ViewAnimationSpec& other) = default;

ViewAnimationSpec::ViewAnimationSpec(ViewAnimationSpec&& rhs) noexcept = default;

ViewAnimationSpec::~ViewAnimationSpec() = default;

ViewAnimationSpec& ViewAnimationSpec::operator=(const ViewAnimationSpec& other) = default;

ViewAnimationSpec& ViewAnimationSpec::operator=(ViewAnimationSpec&& rhs) noexcept = default;

ViewAnimationSpec ViewAnimationSpec::New()
{
  Internal::ViewAnimationSpecImplPtr p = Internal::ViewAnimationSpecImpl::New();
  return ViewAnimationSpec(p.Get());
}

ViewAnimationSpec::ViewAnimationSpec(Internal::ViewAnimationSpecImpl* impl)
: BaseHandle(impl)
{
}

void ViewAnimationSpec::ApplyTo(Animation animation, View view) const
{
  Internal::GetImpl(*this).ApplyEntries(animation, view);
}

///////////////////////////////////////////////////////////////////////////////
// Property methods (AddEntry calls)
///////////////////////////////////////////////////////////////////////////////

ViewAnimationSpec& ViewAnimationSpec::BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target.Resolve(), duration, alpha, delay, &Internal::ViewAnimationSpecImpl::ApplyBackgroundColorTo);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative.Resolve(), duration, alpha, delay, &Internal::ViewAnimationSpecImpl::ApplyBackgroundColorBy);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::SizeWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &Internal::ViewAnimationSpecImpl::ApplySizeWidthTo);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::SizeWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &Internal::ViewAnimationSpecImpl::ApplySizeWidthBy);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::SizeHeight(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Dali::Property::INVALID_INDEX, target, duration, alpha, delay, &Internal::ViewAnimationSpecImpl::ApplySizeHeightTo);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::SizeHeightBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Dali::Property::INVALID_INDEX, relative, duration, alpha, delay, &Internal::ViewAnimationSpecImpl::ApplySizeHeightBy);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::ScaleX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Actor::Property::SCALE_X, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::ScaleXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Actor::Property::SCALE_X, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::ScaleY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Actor::Property::SCALE_Y, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::ScaleYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Actor::Property::SCALE_Y, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::Opacity(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Actor::Property::OPACITY, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::OpacityBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Actor::Property::OPACITY, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::PositionX(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Actor::Property::POSITION_X, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::PositionXBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Actor::Property::POSITION_X, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::PositionY(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(Actor::Property::POSITION_Y, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::PositionYBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(Actor::Property::POSITION_Y, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(View::Property::CORNER_RADIUS, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(View::Property::CORNER_RADIUS, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(View::Property::CORNER_SQUARENESS, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(View::Property::CORNER_SQUARENESS, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BorderlineWidth(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(View::Property::BORDERLINE_WIDTH, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(View::Property::BORDERLINE_WIDTH, relative, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(View::Property::BORDERLINE_COLOR, target.Resolve(), duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(View::Property::BORDERLINE_COLOR, relative.Resolve(), duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BorderlineOffset(float target, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateToEntry(View::Property::BORDERLINE_OFFSET, target, duration, alpha, delay);
  return *this;
}

ViewAnimationSpec& ViewAnimationSpec::BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha, Duration delay)
{
  Internal::GetImpl(*this).AddAnimateByEntry(View::Property::BORDERLINE_OFFSET, relative, duration, alpha, delay);
  return *this;
}

} // namespace Ui
} // namespace Dali
