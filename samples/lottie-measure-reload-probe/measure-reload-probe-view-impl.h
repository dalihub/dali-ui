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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include "measure-reload-probe-view.h"

#include <chrono>
#include <cstdint>

using namespace Dali;
using namespace Dali::Ui;

namespace LottieProbeSample
{

class MeasureReloadProbeViewImpl : public ViewImpl
{
public:
  static IntrusivePtr<MeasureReloadProbeViewImpl> New();

  MeasureReloadProbeViewImpl();
  ~MeasureReloadProbeViewImpl() override = default;

  void SetReloadInMeasure(bool enabled);
  bool IsReloadInMeasure() const;

  void SetExplicitReload(bool enabled);
  bool IsExplicitReload() const;

  void SetPerPassLogging(bool enabled);
  bool IsPerPassLogging() const;

  void SetPlaying(bool playing);
  bool IsPlaying() const;

  uint32_t GetMeasureCount() const;
  uint32_t GetReloadCount() const;

protected:
  void OnInitialize() override;

private:
  /**
   * @brief This view's measure implementation, installed with View::SetMeasureCallback().
   *
   * It REPLACES the default measurement, so it measures the Lottie child itself.
   */
  MeasuredSize OnMeasurePass(View self, float widthConstraint, float heightConstraint);

  /**
   * @brief Console instrumentation. Touches no View and starts no Timer, by design.
   */
  void ReportPass();

private:
  using Clock = std::chrono::steady_clock;

  LottieAnimationView mLottie;
  Clock::time_point   mLastReportTime;
  uint32_t            mMeasureCount;
  uint32_t            mReloadCount;
  bool                mReloadInMeasure;
  bool                mExplicitReload;
  bool                mPerPassLogging;
  bool                mPlaying;
};

} // namespace LottieProbeSample
