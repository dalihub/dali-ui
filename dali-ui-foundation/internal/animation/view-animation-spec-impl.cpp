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
#include <dali-ui-foundation/internal/animation/view-animation-spec-impl.autogen.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace IntegrationView = Dali::Ui::Integration::View;

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
Dali::TypeRegistration VIEW_ANIMATION_SPEC_TYPE(typeid(Dali::Ui::ViewAnimationSpec), typeid(Dali::Ui::Internal::ViewAnimationSpecImpl), typeid(Dali::BaseHandle), nullptr);

Dali::Property GetShadowVisualProperty(View view, Property::Key visualPropertyKey, const char* propertyName)
{
  Dali::Property visualProperty = IntegrationView::GetVisualProperty(view, View::Property::SHADOW, visualPropertyKey);
  if(visualProperty.propertyIndex == Property::INVALID_INDEX)
  {
    DALI_LOG_DEBUG_INFO("Skip shadow %s animation: primary shadow visual is not set.\n", propertyName);
  }
  return visualProperty;
}
} // namespace

ViewAnimationSpecImpl::ViewAnimationSpecImpl() = default;

ViewAnimationSpecImpl::~ViewAnimationSpecImpl() = default;

ViewAnimationSpecImplPtr ViewAnimationSpecImpl::New()
{
  return ViewAnimationSpecImplPtr(new ViewAnimationSpecImpl());
}

void ViewAnimationSpecImpl::AddAnimateToEntry(Property::Index index, Property::Value value,
                                              Duration duration, AlphaFunction alpha, Duration delay)
{
  mEntries.push_back({index, std::move(value), duration, alpha, delay, &ApplyAnimateTo});
}

void ViewAnimationSpecImpl::AddAnimateToEntry(Property::Index index, Property::Value value,
                                              Duration duration, AlphaFunction alpha, Duration delay,
                                              Entry::ApplyFunction apply)
{
  mEntries.push_back({index, std::move(value), duration, alpha, delay, apply});
}

void ViewAnimationSpecImpl::AddAnimateByEntry(Property::Index index, Property::Value value,
                                              Duration duration, AlphaFunction alpha, Duration delay)
{
  mEntries.push_back({index, std::move(value), duration, alpha, delay, &ApplyAnimateBy});
}

void ViewAnimationSpecImpl::AddAnimateByEntry(Property::Index index, Property::Value value,
                                              Duration duration, AlphaFunction alpha, Duration delay,
                                              Entry::ApplyFunction apply)
{
  mEntries.push_back({index, std::move(value), duration, alpha, delay, apply});
}

void ViewAnimationSpecImpl::ApplyEntries(Animation animation, View view) const
{
  for(const auto& entry : mEntries)
  {
    float required = entry.delay.InSeconds() + entry.duration.InSeconds();
    if(required > animation.GetDuration())
    {
      animation.SetDuration(required);
    }
    entry.apply(animation, view, entry);
  }
}

bool ViewAnimationSpecImpl::ContainsReverseAlpha() const
{
  for(const auto& entry : mEntries)
  {
    if(entry.alpha.GetMode() == AlphaFunction::BUILTIN_FUNCTION &&
       entry.alpha.GetBuiltinFunction() == AlphaFunction::REVERSE)
    {
      return true;
    }
  }
  return false;
}

bool ViewAnimationSpecImpl::ContainsLayoutBoundsProperty() const
{
  for(const auto& entry : mEntries)
  {
    if(entry.apply == &ApplyPositionTo || entry.apply == &ApplyPositionBy ||
       entry.apply == &ApplyPositionXTo || entry.apply == &ApplyPositionXBy ||
       entry.apply == &ApplyPositionYTo || entry.apply == &ApplyPositionYBy ||
       entry.apply == &ApplySizeTo || entry.apply == &ApplySizeBy ||
       entry.apply == &ApplySizeWidthTo || entry.apply == &ApplySizeWidthBy ||
       entry.apply == &ApplySizeHeightTo || entry.apply == &ApplySizeHeightBy)
    {
      return true;
    }
  }
  return false;
}

void ViewAnimationSpecImpl::ApplyAnimateTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, entry.index), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyAnimateBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, entry.index), entry.value, entry.alpha, period);
}

///////////////////////////////////////////////////////////////////////////////
// Manual apply functions
///////////////////////////////////////////////////////////////////////////////

void ViewAnimationSpecImpl::ApplyBackgroundColorTo(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = IntegrationView::GetVisualProperty(view, View::Property::BACKGROUND, Ui::VisualBasePropertyIndex::MIX_COLOR);
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyBackgroundColorBy(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = IntegrationView::GetVisualProperty(view, View::Property::BACKGROUND, Ui::VisualBasePropertyIndex::MIX_COLOR);
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyBackgroundGradientStartOffsetTo(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = IntegrationView::GetVisualProperty(view, View::Property::BACKGROUND, Ui::GradientVisualPropertyIndex::START_OFFSET);
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyBackgroundGradientStartOffsetBy(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = IntegrationView::GetVisualProperty(view, View::Property::BACKGROUND, Ui::GradientVisualPropertyIndex::START_OFFSET);
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyShadowBlurRadiusTo(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = GetShadowVisualProperty(view, Ui::ColorVisualPropertyIndex::BLUR_RADIUS, "blur radius");
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyShadowBlurRadiusBy(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = GetShadowVisualProperty(view, Ui::ColorVisualPropertyIndex::BLUR_RADIUS, "blur radius");
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyShadowOpacityTo(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = GetShadowVisualProperty(view, Ui::VisualBasePropertyIndex::OPACITY, "opacity");
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplyShadowOpacityBy(Animation& animation, View view, const Entry& entry)
{
  Dali::Property visualProperty = GetShadowVisualProperty(view, Ui::VisualBasePropertyIndex::OPACITY, "opacity");
  if(visualProperty.propertyIndex != Property::INVALID_INDEX)
  {
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(visualProperty, entry.value, entry.alpha, period);
  }
}

void ViewAnimationSpecImpl::ApplySizeTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::SIZE), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplySizeBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::SIZE), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplySizeWidthTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::SIZE_WIDTH), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplySizeWidthBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::SIZE_WIDTH), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplySizeHeightTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::SIZE_HEIGHT), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplySizeHeightBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::SIZE_HEIGHT), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyPositionTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::POSITION), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyPositionBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::POSITION), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyPositionXTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::POSITION_X), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyPositionXBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::POSITION_X), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyPositionYTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::POSITION_Y), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyPositionYBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::POSITION_Y), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyScaleTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::SCALE), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyScaleBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::SCALE), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyScaleXTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::SCALE_X), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyScaleXBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::SCALE_X), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyScaleYTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::SCALE_Y), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyScaleYBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::SCALE_Y), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyColorTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::COLOR_MULTIPLIER), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyColorBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::COLOR_MULTIPLIER), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyOpacityTo(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(view, Actor::Property::OPACITY), entry.value, entry.alpha, period);
}

void ViewAnimationSpecImpl::ApplyOpacityBy(Animation& animation, View view, const Entry& entry)
{
  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(view, Actor::Property::OPACITY), entry.value, entry.alpha, period);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
