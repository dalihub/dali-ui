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
#include <dali-ui-foundation/internal/animation/label-animation-spec-impl.autogen.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/text-pixel-snap-data.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <algorithm>
#include <cmath>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace
{
Dali::TypeRegistration LABEL_ANIMATION_SPEC_TYPE(typeid(Dali::Ui::LabelAnimationSpec), typeid(Dali::Ui::Internal::LabelAnimationSpecImpl), typeid(Dali::Ui::ViewAnimationSpec), nullptr);

Dali::Ui::Integration::LabelImpl* GetLabelImpl(Label label)
{
  if(!label)
  {
    return nullptr;
  }

  Dali::RefObject& handle = label.GetImplementation();
  return &static_cast<Dali::Ui::Integration::LabelImpl&>(handle);
}

float ClampTextRevealProgress(float value)
{
  // Keep public normalized state deterministic for every IEEE-754 input.
  // Infinities saturate naturally; unordered NaN resolves to the safe hidden
  // endpoint instead of entering the scene graph.
  return std::isnan(value) ? 0.0f : std::max(0.0f, std::min(1.0f, value));
}
} // namespace

LabelAnimationSpecImpl::LabelAnimationSpecImpl() = default;

LabelAnimationSpecImpl::~LabelAnimationSpecImpl() = default;

LabelAnimationSpecImplPtr LabelAnimationSpecImpl::New()
{
  return LabelAnimationSpecImplPtr(new LabelAnimationSpecImpl());
}

void LabelAnimationSpecImpl::ApplyPixelSnapFactorTo(Animation& animation, Label label, const Entry& entry)
{
  const Property::Index index = EnsureTextPixelSnapFactorProperty(label);
  if(index == Property::INVALID_INDEX)
  {
    return;
  }

  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateTo(Property(label, index), entry.value, entry.alpha, period);
}

void LabelAnimationSpecImpl::ApplyPixelSnapFactorBy(Animation& animation, Label label, const Entry& entry)
{
  const Property::Index index = EnsureTextPixelSnapFactorProperty(label);
  if(index == Property::INVALID_INDEX)
  {
    return;
  }

  TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
  animation.AnimateBy(Property(label, index), entry.value, entry.alpha, period);
}

void LabelAnimationSpecImpl::ApplyTextGradientStartOffsetTo(Animation& animation, Label label, const Entry& entry)
{
  if(auto* labelImpl = GetLabelImpl(label))
  {
    const Property::Index index = labelImpl->EnsureGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(label, index), entry.value, entry.alpha, period);
  }
}

void LabelAnimationSpecImpl::ApplyTextGradientStartOffsetBy(Animation& animation, Label label, const Entry& entry)
{
  if(auto* labelImpl = GetLabelImpl(label))
  {
    const Property::Index index = labelImpl->EnsureGradientAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(label, index), entry.value, entry.alpha, period);
  }
}

void LabelAnimationSpecImpl::ApplyTextGradientOverlayStartOffsetTo(Animation& animation, Label label, const Entry& entry)
{
  if(auto* labelImpl = GetLabelImpl(label))
  {
    const Property::Index index = labelImpl->EnsureGradientOverlayAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(label, index), entry.value, entry.alpha, period);
  }
}

void LabelAnimationSpecImpl::ApplyTextGradientOverlayStartOffsetBy(Animation& animation, Label label, const Entry& entry)
{
  if(auto* labelImpl = GetLabelImpl(label))
  {
    const Property::Index index = labelImpl->EnsureGradientOverlayAnimOffset();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(label, index), entry.value, entry.alpha, period);
  }
}

void LabelAnimationSpecImpl::ApplyTextRevealProgressTo(Animation& animation, Label label, const Entry& entry)
{
  if(auto* labelImpl = GetLabelImpl(label))
  {
    const Property::Index index = labelImpl->EnsureTextRevealProgress();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    const float target = ClampTextRevealProgress(entry.value.Get<float>());
    TimePeriod  period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateTo(Property(label, index), target, entry.alpha, period);
  }
}

void LabelAnimationSpecImpl::ApplyTextRevealProgressBy(Animation& animation, Label label, const Entry& entry)
{
  if(auto* labelImpl = GetLabelImpl(label))
  {
    const Property::Index index = labelImpl->EnsureTextRevealProgress();
    if(index == Property::INVALID_INDEX)
    {
      return;
    }

    // Preserve DALi AnimateBy semantics: the relative value is applied to the
    // scene property at the entry's actual start, not when this bridge is
    // configured. The public getter and the shader clamp the presentation to
    // [0, 1], matching other bounded animatable properties such as opacity.
    TimePeriod period(entry.delay.InSeconds(), entry.duration.InSeconds());
    animation.AnimateBy(Property(label, index), entry.value, entry.alpha, period);
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
