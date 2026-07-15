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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief FlexLayoutParams stores per-child layout parameters for FlexLayout.
 *
 * This value type provides flex grow, shrink, basis, and align-self for a child view.
 * Use View::SetLayoutParams() to attach parameters to a child view.
 *
 * @code
 * view.SetLayoutParams(FlexLayoutParams::New()
 *   .SetFlexGrow(1.0f)
 *   .SetFlexShrink(0.0f)
 *   .SetAlignSelf(FlexAlign::CENTER));
 * @endcode
 */
class DALI_UI_API FlexLayoutParams
{
public:
  /**
   * @brief Creates parameters with default values.
   */
  FlexLayoutParams();

  /**
   * @brief Creates a new FlexLayoutParams with default values.
   *
   * @return A value initialized with default parameters
   */
  static FlexLayoutParams New();

  /**
   * @brief Copy constructor.
   *
   * @param[in] other Value to copy
   */
  FlexLayoutParams(const FlexLayoutParams& other);

  /**
   * @brief Creates a value by moving another value.
   *
   * @note After the move the source is in a moved-from state; assign it a valid
   * value before use. Calling a getter or setter on, or copying from, a
   * moved-from value throws.
   */
  FlexLayoutParams(FlexLayoutParams&& other) noexcept;

  /**
   * @brief Copies another value.
   */
  FlexLayoutParams& operator=(const FlexLayoutParams& other);

  /**
   * @brief Moves another value.
   *
   * @note After the move the source is in a moved-from state; assign it a valid
   * value before use. Calling a getter or setter on, or copying from, a
   * moved-from value throws.
   */
  FlexLayoutParams& operator=(FlexLayoutParams&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~FlexLayoutParams();

  /**
   * @brief Sets the flex grow factor for distributing remaining space.
   *
   * @param[in] grow The grow factor (0 means no growing)
   * @return Reference to this for chaining
   */
  FlexLayoutParams& SetFlexGrow(float grow);

  /**
   * @brief Gets the flex grow factor.
   *
   * @return The grow factor
   */
  float GetFlexGrow() const;

  /**
   * @brief Sets the flex shrink factor for distributing negative space.
   *
   * @param[in] shrink The shrink factor (1 means proportional shrinking)
   * @return Reference to this for chaining
   */
  FlexLayoutParams& SetFlexShrink(float shrink);

  /**
   * @brief Gets the flex shrink factor.
   *
   * @return The shrink factor
   */
  float GetFlexShrink() const;

  /**
   * @brief Sets the initial main size of the flex item before grow/shrink.
   *
   * @param[in] basis The flex basis value (use WRAP_CONTENT for auto)
   * @return Reference to this for chaining
   */
  FlexLayoutParams& SetFlexBasis(float basis);

  /**
   * @brief Gets the flex basis value.
   *
   * @return The flex basis
   */
  float GetFlexBasis() const;

  /**
   * @brief Sets the cross-axis alignment override for this child.
   *
   * @param[in] align The alignment (AUTO defers to parent's alignItems)
   * @return Reference to this for chaining
   */
  FlexLayoutParams& SetAlignSelf(FlexAlign align);

  /**
   * @brief Gets the cross-axis alignment override for this child.
   *
   * @return The align-self value
   */
  FlexAlign GetAlignSelf() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Ui
} // namespace Dali
