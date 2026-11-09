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

#include <dali-ui-foundation/public-api/views/view.h>

#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace Extension
{
class ProgressViewImpl;
}

/**
 * @brief Base handle for non-interactive views that display progress through a value range.
 *
 * ProgressView owns the common range, value, step and determinate-state contract used by
 * rendered progress components. It cannot be created directly because it has no visual
 * representation of its own. Create a concrete component such as ProgressBar instead.
 * @SINCE_3_0.0
 */
class DALI_UI_API ProgressView : public View
{
public:
  using ValueChangedSignalType       = Signal<void(ProgressView, float)>;
  using DeterminateChangedSignalType = Signal<void(ProgressView, bool)>;

  /**
   * @brief Creates an uninitialized ProgressView handle.
   * @SINCE_3_0.0
   */
  ProgressView();

  /**
   * @brief Downcasts a handle to ProgressView.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to downcast
   * @return An initialized ProgressView on success, otherwise an empty handle
   */
  static ProgressView DownCast(BaseHandle handle);

  ProgressView(const ProgressView& progressView);
  ProgressView(ProgressView&& rhs) noexcept;

  /**
   * @brief Destroys the handle.
   *
   * This destructor is non-virtual because DALi handle subclasses must not add virtual methods.
   * @SINCE_3_0.0
   */
  ~ProgressView();

  ProgressView& operator=(const ProgressView& handle)  = default;
  ProgressView& operator=(ProgressView&& rhs) noexcept = default;

  DALI_UI_VIEW_WITH(ProgressView)

  /**
   * @brief Returns the signal emitted after the effective value and renderer state are committed.
   *
   * The second argument is the value after clamping and optional step snapping. Repeating an
   * input whose effective value is unchanged does not emit the signal.
   * @SINCE_3_0.0
   */
  ValueChangedSignalType& ValueChangedSignal();

  /**
   * @brief Returns the signal emitted after the determinate state and renderer are committed.
   *
   * The second argument is the new determinate state. Repeating the current state is a no-op.
   * @SINCE_3_0.0
   */
  DeterminateChangedSignalType& DeterminateChangedSignal();

  /**
   * @brief Sets the current value.
   *
   * Finite values are clamped to the current range and, when ValueStepCount is positive,
   * snapped to the nearest valid step.
   *
   * @SINCE_3_0.0
   * @param[in] value The finite value to set
   */
  void SetValue(float value);

  /**
   * @brief Returns the committed effective value.
   * @SINCE_3_0.0
   */
  float GetValue() const;

  /**
   * @brief Atomically sets the valid value range.
   *
   * Both endpoints must be finite and @p maximumValue must be greater than @p minimumValue.
   * The current value is re-coerced against the new range and current step count.
   * @SINCE_3_0.0
   */
  void SetRange(float minimumValue, float maximumValue);

  /**
   * @brief Returns both endpoints from one committed range snapshot.
   * @SINCE_3_0.0
   */
  void GetRange(float& minimumValue, float& maximumValue) const;

  /**
   * @brief Sets the number of equal intervals in the range.
   *
   * Zero selects continuous operation. A positive value N creates N equal intervals and
   * N + 1 valid values including both endpoints. Negative values are rejected.
   * @SINCE_3_0.0
   */
  void SetValueStepCount(int32_t count);

  /**
   * @brief Returns the number of equal value intervals, or zero in continuous mode.
   * @SINCE_3_0.0
   */
  int32_t GetValueStepCount() const;

  /**
   * @brief Sets whether the component displays determinate progress.
   * @SINCE_3_0.0
   */
  void SetDeterminate(bool determinate);

  /**
   * @brief Returns whether the component displays determinate progress.
   * @SINCE_3_0.0
   */
  bool IsDeterminate() const;

public: // Not intended for application developers
  /// @cond internal
  explicit ProgressView(Extension::ProgressViewImpl& implementation);
  explicit ProgressView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE
