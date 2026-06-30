#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/dali-signal.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{

class EdgeEffectImpl;
using EdgeEffectImplPtr = IntrusivePtr<EdgeEffectImpl>;

/**
 * @brief Internal implementation class for EdgeEffect.
 *
 * Manages a 5-state machine (IDLE, PULL, ABSORB, RECEDE, PULLDECAY) for scroll
 * edge visual effects. Concrete behaviours (e.g. BounceEdgeEffect) subclass this
 * and override the protected virtual hooks. This header lives in public-api so
 * that subclasses implemented in a separate library can extend it without
 * requiring access to integration-api.
 */
class DALI_UI_API EdgeEffectImpl : public Dali::BaseObject, public Dali::ConnectionTracker
{
public:
  enum class State
  {
    IDLE,
    PULL,
    ABSORB,
    RECEDE,
    PULLDECAY
  };

  using PullSignalType     = Dali::Signal<void(float, float)>; ///< (deltaDistance, displacement)
  using ReleaseSignalType  = Dali::Signal<void()>;
  using AbsorbSignalType   = Dali::Signal<void(float)>; ///< (velocity)
  using FinishedSignalType = Dali::Signal<void()>;

public:
  static EdgeEffectImplPtr New();

  virtual ~EdgeEffectImpl();

  void     SetSource(Ui::View source);
  Ui::View GetSource() const;

  State GetState() const;

  void OnPull(float deltaDistance, float displacement);
  void OnRelease();
  void OnAbsorb(float velocity);
  void Finish();

  PullSignalType&     PullSignal();
  ReleaseSignalType&  ReleaseSignal();
  AbsorbSignalType&   AbsorbSignal();
  FinishedSignalType& FinishedSignal();

protected:
  EdgeEffectImpl();

  // Virtual hooks for subclasses — never reorder, add, or remove
  virtual void OnPullImpl(State prevState, float deltaDistance, float displacement);
  virtual void OnReleaseImpl();
  virtual void OnAbsorbImpl(float velocity);
  virtual void OnFinishImpl();

  void TransitionState(State next);

  Ui::View mSource; ///< The source view whose position this effect manipulates

private:
  State              mState{State::IDLE};
  PullSignalType     mPullSignal;
  ReleaseSignalType  mReleaseSignal;
  AbsorbSignalType   mAbsorbSignal;
  FinishedSignalType mFinishedSignal;
};

} // namespace Ui

} // namespace Dali
