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

#include "flux-view-data.h"

namespace
{
constexpr uint32_t SIGNAL_DELAY_MS = 3000u;
}

FluxViewData::FluxViewData(Dali::Ui::View targetView)
: mTargetView(targetView)
{
}

Dali::Ui::View FluxViewData::GetTargetView() const
{
  return mTargetView.GetHandle();
}

void FluxViewData::SetFluxPolicy(FluxPolicy policy)
{
  mFluxPolicy = policy;

  // NOTE Signal Poc를 위해 FluxPolicy를 설정하면 3초후에 signal을 emit하게 구현
  if(!mFluxPolicyTimer)
  {
    mFluxPolicyTimer = Dali::Timer::New(SIGNAL_DELAY_MS);
    mFluxPolicyTimer.TickSignal().Connect(this, &FluxViewData::OnFluxPolicyTimerTick);
  }

  mFluxPolicyTimer.Start();
}

FluxPolicy FluxViewData::GetFluxPolicy() const
{
  return mFluxPolicy;
}

FluxViewData::FluxPolicyAppliedSignalType& FluxViewData::FluxPolicyAppliedSignal()
{
  return mFluxPolicyAppliedSignal;
}

bool FluxViewData::OnFluxPolicyTimerTick()
{
  Dali::Ui::View targetView = GetTargetView();
  if(targetView)
  {
    mFluxPolicyAppliedSignal.Emit(targetView);
  }

  return false;
}
