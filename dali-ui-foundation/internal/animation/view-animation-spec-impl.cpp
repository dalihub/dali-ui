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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

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
  // TODO: Implement background color AnimateTo
}

void ViewAnimationSpecImpl::ApplyBackgroundColorBy(Animation& animation, View view, const Entry& entry)
{
  // TODO: Implement background color AnimateBy
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

} // namespace Internal
} // namespace Ui
} // namespace Dali
