/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

#pragma once

#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/dali-signal.h>

enum class FluxPolicy
{
  NONE,
  CALM,
  BRIGHT,
  BOLD,
  SHARP,
};

class FluxViewData : public Dali::ConnectionTracker
{
public:
  using FluxPolicyAppliedSignalType = Dali::Signal<void(Dali::Ui::View)>;

  explicit FluxViewData(Dali::Ui::View targetView);
  virtual ~FluxViewData() = default;

  Dali::Ui::View GetTargetView() const;

  void SetFluxPolicy(FluxPolicy policy);
  FluxPolicy GetFluxPolicy() const;

  FluxPolicyAppliedSignalType& FluxPolicyAppliedSignal();

private:
  bool OnFluxPolicyTimerTick();

private:
  Dali::WeakHandle<Dali::Ui::View> mTargetView;
  FluxPolicy                       mFluxPolicy{FluxPolicy::NONE};
  Dali::Timer                      mFluxPolicyTimer;
  FluxPolicyAppliedSignalType      mFluxPolicyAppliedSignal;
};
