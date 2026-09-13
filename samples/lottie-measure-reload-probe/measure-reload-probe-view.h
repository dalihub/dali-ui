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

#include <cstdint>

using namespace Dali;
using namespace Dali::Ui;

namespace LottieProbeSample
{

class MeasureReloadProbeViewImpl;

/**
 * @brief Diagnostic View that reloads its Lottie child from inside its own measure pass.
 *
 * The view builds a LottieAnimationView child in its construction path and installs a
 * MeasureCallback through View::SetMeasureCallback(). From inside that callback it calls
 * one of two things on the child before measuring it: SetResourceUrl() with the URL the
 * child already has, which is a NO-OP, or Reload(), which is the explicit reload and
 * rebuilds the child's visual. SetExplicitReload() selects between them.
 *
 * This is a reproducer for a contract violation, not a pattern to copy. A measure
 * implementation must be a pure function of its inputs.
 */
class MeasureReloadProbeView : public View
{
public:

  static MeasureReloadProbeView New();
  static MeasureReloadProbeView DownCast(BaseHandle handle);

  MeasureReloadProbeView();
  MeasureReloadProbeView(const MeasureReloadProbeView& probeView);
  MeasureReloadProbeView(MeasureReloadProbeView&& rhs) noexcept;
  DALI_INTERNAL MeasureReloadProbeView(MeasureReloadProbeViewImpl& impl);
  DALI_INTERNAL MeasureReloadProbeView(Dali::Internal::CustomActor* customActor);

  ~MeasureReloadProbeView();

  MeasureReloadProbeView& operator=(const MeasureReloadProbeView& handle) = default;
  MeasureReloadProbeView& operator=(MeasureReloadProbeView&& rhs) noexcept = default;

  /**
   * @brief Enables or disables the in-measure call under test.
   *
   * Enabling issues one event-time InvalidateMeasure() so the producer restarts after it
   * has been switched off and the layout has settled.
   */
  void SetReloadInMeasure(bool enabled);
  bool IsReloadInMeasure() const;

  /**
   * @brief Selects which call the measure callback issues.
   *
   * Off by default: the callback calls SetResourceUrl() with the URL the child already
   * has, which is a no-op, so the layout settles. Switching it on makes the callback call
   * Reload() instead, which rebuilds the child's visual on every pass.
   */
  void SetExplicitReload(bool enabled);
  bool IsExplicitReload() const;

  /**
   * @brief Enables or disables the one-line-per-pass console trace.
   *
   * The trace serialises the loop, so switch it off before reading CPU usage.
   */
  void SetPerPassLogging(bool enabled);
  bool IsPerPassLogging() const;

  /**
   * @brief Starts or pauses the Lottie animation.
   *
   * Off by default: a playing vector animation raises the CPU baseline on its own and
   * would be hard to tell apart from the main loop behaviour under test.
   */
  void SetPlaying(bool playing);
  bool IsPlaying() const;

  uint32_t GetMeasureCount() const;
  uint32_t GetReloadCount() const;

  DALI_UI_VIEW_WITH(MeasureReloadProbeView)

  // [IMPORTANT] No data members in the handle class.
};

} // namespace LottieProbeSample
