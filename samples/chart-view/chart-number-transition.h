#ifndef DALI_CHART_SAMPLE_CHART_NUMBER_TRANSITION_H
#define DALI_CHART_SAMPLE_CHART_NUMBER_TRANSITION_H

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
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include "chart-reference-cases.h"

namespace ChartStandard
{
/**
 * @brief Sample-local digit rolling, counting and cross fading driven by elapsed time.
 */
class NumberTransition
{
public:
  /**
   * @brief Creates number labels in a clipped sample-local container.
   *
   * @param[in] parent The parent layout
   * @param[in] x The left offset in pixels
   * @param[in] y The top offset in pixels
   * @param[in] width The available width in pixels
   * @param[in] color The fixed RGBA text color
   */
  NumberTransition(Dali::Ui::AbsoluteLayout parent, float x, float y, float width, const Dali::Vector4& color);
  ~NumberTransition()                                  = default;
  NumberTransition(const NumberTransition&)            = delete;
  NumberTransition& operator=(const NumberTransition&) = delete;
  NumberTransition(NumberTransition&&)                 = delete;
  NumberTransition& operator=(NumberTransition&&)      = delete;

  /**
   * @brief Immediately displays a value and stops any running transition.
   *
   * @param[in] value The displayed number
   * @param[in] digits The requested number of decimal places
   */
  void SetValue(double value, int digits);

  /**
   * @brief Starts a rolling, counting or cross-fading transition from the current value.
   *
   * @param[in] target The target number
   * @param[in] mode The transition mode
   * @param[in] digits The requested number of decimal places
   * @param[in] duration The duration in seconds
   */
  void Start(double target, NumberMode mode, int digits, float duration);

  /**
   * @brief Advances the transition using elapsed time.
   *
   * @param[in] seconds The elapsed time in seconds
   */
  void Advance(float seconds);

  /**
   * @brief Returns the currently displayed interpolated number.
   *
   * @return The displayed number
   */
  double GetValue() const;

  /**
   * @brief Tests whether a transition is still active.
   *
   * @return True while a transition is running
   */
  bool IsRunning() const;

  /**
   * @brief Returns the clipped number container.
   *
   * @return The root view
   */
  Dali::Ui::View GetView() const;

private:
  /**
   * @brief Rebuilds old and new digit columns for the rolling transition.
   */
  void                         RebuildDigits();
  std::vector<Dali::Ui::Label> mOldDigits;
  std::vector<Dali::Ui::Label> mNewDigits;
  Dali::Vector4                mColor;
  Dali::Ui::AbsoluteLayout     mRoot;
  Dali::Ui::Label              mCurrent;
  Dali::Ui::Label              mNext;
  double                       mFrom{0.0};
  double                       mTarget{0.0};
  double                       mValue{0.0};
  float                        mElapsed{0.0f};
  float                        mDuration{0.5f};
  int                          mDigits{0};
  NumberMode                   mMode{NumberMode::NONE};
  float                        mWidth;
};
} //namespace ChartStandard

#endif // DALI_CHART_SAMPLE_CHART_NUMBER_TRANSITION_H
