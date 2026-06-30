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
#include <dali-ui-foundation/public-api/views/scroll/edge-effect-impl.h>

namespace Dali
{

namespace Ui
{

EdgeEffectImplPtr EdgeEffectImpl::New()
{
  return EdgeEffectImplPtr(new EdgeEffectImpl());
}

EdgeEffectImpl::EdgeEffectImpl()  = default;
EdgeEffectImpl::~EdgeEffectImpl() = default;

void EdgeEffectImpl::SetSource(Ui::View source)
{
  mSource = source;
}

Ui::View EdgeEffectImpl::GetSource() const
{
  return mSource;
}

EdgeEffectImpl::State EdgeEffectImpl::GetState() const
{
  return mState;
}

void EdgeEffectImpl::OnPull(float deltaDistance, float displacement)
{
  State prev = mState;
  mState     = State::PULL;
  OnPullImpl(prev, deltaDistance, displacement);
  mPullSignal.Emit(deltaDistance, displacement);
}

void EdgeEffectImpl::OnRelease()
{
  if(mState != State::PULL && mState != State::PULLDECAY)
  {
    return;
  }
  mState = State::RECEDE;
  OnReleaseImpl();
  mReleaseSignal.Emit();
}

void EdgeEffectImpl::OnAbsorb(float velocity)
{
  switch(mState)
  {
    case State::IDLE:
    case State::RECEDE:
    case State::PULL:
      break;
    default:
      return;
  }
  mState = State::ABSORB;
  OnAbsorbImpl(velocity);
  mAbsorbSignal.Emit(velocity);
}

void EdgeEffectImpl::Finish()
{
  mState = State::IDLE;
  OnFinishImpl();
  mFinishedSignal.Emit();
}

void EdgeEffectImpl::TransitionState(State next)
{
  mState = next;
}

void EdgeEffectImpl::OnPullImpl(State, float, float)
{
}
void EdgeEffectImpl::OnReleaseImpl()
{
}
void EdgeEffectImpl::OnAbsorbImpl(float)
{
}
void EdgeEffectImpl::OnFinishImpl()
{
}

EdgeEffectImpl::PullSignalType& EdgeEffectImpl::PullSignal()
{
  return mPullSignal;
}
EdgeEffectImpl::ReleaseSignalType& EdgeEffectImpl::ReleaseSignal()
{
  return mReleaseSignal;
}
EdgeEffectImpl::AbsorbSignalType& EdgeEffectImpl::AbsorbSignal()
{
  return mAbsorbSignal;
}
EdgeEffectImpl::FinishedSignalType& EdgeEffectImpl::FinishedSignal()
{
  return mFinishedSignal;
}

} // namespace Ui

} // namespace Dali
