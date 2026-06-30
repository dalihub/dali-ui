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
#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Ui
{

BounceEdgeEffectImpl& BounceEdgeEffect::GetBounceImpl()
{
  return static_cast<BounceEdgeEffectImpl&>(GetImpl());
}

const BounceEdgeEffectImpl& BounceEdgeEffect::GetBounceImpl() const
{
  return static_cast<const BounceEdgeEffectImpl&>(GetImpl());
}

BounceEdgeEffect BounceEdgeEffect::New(ScrollDirection axis)
{
  return BounceEdgeEffect(BounceEdgeEffectImpl::New(axis).Get());
}

BounceEdgeEffect::BounceEdgeEffect(BounceEdgeEffectImpl* impl)
: EdgeEffect(impl)
{
}

BounceEdgeEffect BounceEdgeEffect::DownCast(EdgeEffect effect)
{
  if(effect)
  {
    auto* p = dynamic_cast<BounceEdgeEffectImpl*>(&effect.GetImpl());
    if(p)
    {
      return BounceEdgeEffect(p);
    }
  }
  return BounceEdgeEffect();
}

ScrollDirection BounceEdgeEffect::GetAxis() const
{
  return GetBounceImpl().GetAxis();
}

BounceEdgeEffect& BounceEdgeEffect::SetPullResistance(float resistance)
{
  GetBounceImpl().SetPullResistance(resistance);
  return *this;
}

float BounceEdgeEffect::GetPullResistance() const
{
  return GetBounceImpl().GetPullResistance();
}

BounceEdgeEffect& BounceEdgeEffect::SetBounceDuration(float durationSec)
{
  GetBounceImpl().SetBounceDuration(durationSec);
  return *this;
}

float BounceEdgeEffect::GetBounceDuration() const
{
  return GetBounceImpl().GetBounceDuration();
}

} // namespace Ui

} // namespace Dali
