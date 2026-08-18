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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/math/vector3.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/edge-effect-impl.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

class BounceEdgeEffectImpl;
using BounceEdgeEffectImplPtr = IntrusivePtr<BounceEdgeEffectImpl>;

/**
 * @brief Bounce edge effect implementation.
 *
 * When the user drags past the scroll boundary the source content is displaced
 * against a configurable resistance, then springs back with an EASE_OUT_BACK
 * curve on release. A fling that reaches the edge runs the same spring-back
 * animation after a brief push-out proportional to the fling velocity.
 */
class DALI_UI_API BounceEdgeEffectImpl : public Integration::EdgeEffectImpl
{
public:
  static BounceEdgeEffectImplPtr New(ScrollDirection axis);

  ~BounceEdgeEffectImpl() override;

  ScrollDirection GetAxis() const;

  float GetPullResistance() const;
  void  SetPullResistance(float resistance);

  float GetBounceDuration() const;
  void  SetBounceDuration(float durationSec);

protected:
  explicit BounceEdgeEffectImpl(ScrollDirection axis);

  void OnPullImpl(State prevState, float deltaDistance, float displacement) override;
  void OnReleaseImpl() override;
  void OnAbsorbImpl(float velocity) override;
  void OnFinishImpl() override;

private:
  void CaptureBasePosition();
  void ApplyOffset(float offset);
  void CancelAnimation();
  void StartSpringBack();
  void OnSpringBackFinished(Dali::Animation animation);

  ScrollDirection mAxis;
  float           mPullResistance{0.4f};
  float           mBounceDuration{0.35f};
  float           mCurrentOffset{0.0f};
  bool            mBasePositionCaptured{false};
  Vector3         mBasePosition{Vector3::ZERO};
  Dali::Animation mAnimation;
};

} // namespace Internal

} // namespace Ui

} // namespace Dali
