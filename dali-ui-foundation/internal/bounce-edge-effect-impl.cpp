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
#include <dali-ui-foundation/internal/bounce-edge-effect-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/time-period.h>

#include <algorithm>
#include <cmath>

namespace Dali
{

namespace Ui
{

namespace
{
constexpr float MAX_PULL_PIXELS    = 100.0f;
constexpr float ABSORB_FACTOR      = 0.05f;
constexpr float MAX_ABSORB_PX      = 80.0f;
constexpr float ABSORB_PHASE_RATIO = 0.3f;
} // namespace

BounceEdgeEffectImplPtr BounceEdgeEffectImpl::New(ScrollDirection axis)
{
  return BounceEdgeEffectImplPtr(new BounceEdgeEffectImpl(axis));
}

BounceEdgeEffectImpl::BounceEdgeEffectImpl(ScrollDirection axis)
: mAxis(axis)
{
}

BounceEdgeEffectImpl::~BounceEdgeEffectImpl()
{
  CancelAnimation();
}

ScrollDirection BounceEdgeEffectImpl::GetAxis() const
{
  return mAxis;
}

float BounceEdgeEffectImpl::GetPullResistance() const
{
  return mPullResistance;
}
void BounceEdgeEffectImpl::SetPullResistance(float resistance)
{
  mPullResistance = resistance;
}

float BounceEdgeEffectImpl::GetBounceDuration() const
{
  return mBounceDuration;
}
void BounceEdgeEffectImpl::SetBounceDuration(float durationSec)
{
  mBounceDuration = durationSec;
}

void BounceEdgeEffectImpl::CaptureBasePosition()
{
  if(mBasePositionCaptured || !mSource)
  {
    return;
  }
  mBasePosition         = mSource.GetProperty<Vector3>(Actor::Property::POSITION);
  mBasePositionCaptured = true;
}

void BounceEdgeEffectImpl::ApplyOffset(float offset)
{
  if(!mSource)
  {
    return;
  }
  Vector3 pos = mBasePosition;
  if(mAxis == ScrollDirection::Horizontal)
    pos.x += offset;
  else
    pos.y += offset;
  mSource.SetProperty(Actor::Property::POSITION, pos);
}

void BounceEdgeEffectImpl::CancelAnimation()
{
  if(mAnimation)
  {
    mAnimation.Stop();
    mAnimation.Reset();
  }
}

void BounceEdgeEffectImpl::OnPullImpl(State /*prevState*/, float deltaDistance, float /*displacement*/)
{
  if(!mSource)
  {
    return;
  }
  CaptureBasePosition();
  CancelAnimation();

  mCurrentOffset += deltaDistance * mPullResistance;
  mCurrentOffset = std::max(-MAX_PULL_PIXELS, std::min(MAX_PULL_PIXELS, mCurrentOffset));
  ApplyOffset(mCurrentOffset);
}

void BounceEdgeEffectImpl::OnReleaseImpl()
{
  if(!mSource || !mBasePositionCaptured)
  {
    return;
  }
  StartSpringBack();
}

void BounceEdgeEffectImpl::OnAbsorbImpl(float velocity)
{
  if(!mSource)
  {
    return;
  }
  CaptureBasePosition();
  CancelAnimation();

  float extent = std::min(MAX_ABSORB_PX, std::abs(velocity) * ABSORB_FACTOR);
  float dir    = (velocity >= 0.0f) ? 1.0f : -1.0f;

  bool  isHoriz    = (mAxis == ScrollDirection::Horizontal);
  auto  axisProp   = isHoriz ? Actor::Property::POSITION_X : Actor::Property::POSITION_Y;
  float baseCoord  = isHoriz ? mBasePosition.x : mBasePosition.y;
  float pushTarget = baseCoord + dir * extent;
  float restTarget = baseCoord;
  float phase1Dur  = mBounceDuration * ABSORB_PHASE_RATIO;
  float phase2Dur  = mBounceDuration - phase1Dur;

  mAnimation = Animation::New(mBounceDuration);
  mAnimation.AnimateTo(Property(mSource, axisProp),
                       pushTarget,
                       AlphaFunction::EASE_OUT,
                       TimePeriod(0.0f, phase1Dur));
  mAnimation.AnimateTo(Property(mSource, axisProp),
                       restTarget,
                       AlphaFunction(AlphaFunction::EASE_OUT_BACK),
                       TimePeriod(phase1Dur, phase2Dur));
  mAnimation.FinishedSignal().Connect(this, &BounceEdgeEffectImpl::OnSpringBackFinished);
  mAnimation.Play();

  mCurrentOffset = dir * extent;
  TransitionState(State::RECEDE);
}

void BounceEdgeEffectImpl::StartSpringBack()
{
  CancelAnimation();
  if(!mSource)
  {
    return;
  }

  bool  isHoriz    = (mAxis == ScrollDirection::Horizontal);
  auto  axisProp   = isHoriz ? Actor::Property::POSITION_X : Actor::Property::POSITION_Y;
  float restTarget = isHoriz ? mBasePosition.x : mBasePosition.y;

  mAnimation = Animation::New(mBounceDuration);
  mAnimation.AnimateTo(Property(mSource, axisProp),
                       restTarget,
                       AlphaFunction(AlphaFunction::EASE_OUT_BACK));
  mAnimation.FinishedSignal().Connect(this, &BounceEdgeEffectImpl::OnSpringBackFinished);
  mAnimation.Play();
}

void BounceEdgeEffectImpl::OnSpringBackFinished(Dali::Animation /*animation*/)
{
  mAnimation.Reset();
  mCurrentOffset        = 0.0f;
  mBasePositionCaptured = false;
  Finish();
}

void BounceEdgeEffectImpl::OnFinishImpl()
{
  CancelAnimation();
  if(mSource && mBasePositionCaptured)
  {
    mSource.SetProperty(Actor::Property::POSITION, mBasePosition);
  }
  mCurrentOffset        = 0.0f;
  mBasePositionCaptured = false;
}

} // namespace Ui

} // namespace Dali
