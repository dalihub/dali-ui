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
 * @brief StackLayoutParams stores per-child layout parameters for StackLayout.
 *
 * This value type provides layout weight and alignment for a child view.
 * Use View::SetLayoutParams() to attach parameters to a child view.
 *
 * @code
 * view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
 * @endcode
 */
class DALI_UI_API StackLayoutParams
{
public:
  /**
   * @brief Creates parameters with default values.
   */
  StackLayoutParams();

  /**
   * @brief Creates a new StackLayoutParams with default values.
   *
   * @return A value initialized with default parameters
   */
  static StackLayoutParams New();

  /**
   * @brief Copy constructor.
   *
   * @param[in] other Value to copy
   */
  StackLayoutParams(const StackLayoutParams& other);

  /**
   * @brief Creates a value by moving another value.
   *
   * @note After the move the source is in a moved-from state; assign it a valid
   * value before use. Calling a getter or setter on, or copying from, a
   * moved-from value throws.
   */
  StackLayoutParams(StackLayoutParams&& other) noexcept;

  /**
   * @brief Copies another value.
   */
  StackLayoutParams& operator=(const StackLayoutParams& other);

  /**
   * @brief Moves another value.
   *
   * @note After the move the source is in a moved-from state; assign it a valid
   * value before use. Calling a getter or setter on, or copying from, a
   * moved-from value throws.
   */
  StackLayoutParams& operator=(StackLayoutParams&& other) noexcept;

  /**
   * @brief Destructor.
   */
  ~StackLayoutParams();

  /**
   * @brief Sets the weight for proportional space distribution along the stack axis.
   *
   * When 0, the child is measured normally using its LayoutWidth/LayoutHeight.
   * When > 0, the child's main-axis size is determined entirely by its weight
   * proportion of the remaining space (the main-axis LayoutWidth/LayoutHeight
   * is ignored).
   *
   * @param[in] weight The layout weight
   * @return Reference to this for chaining
   */
  StackLayoutParams& SetWeight(float weight);

  /**
   * @brief Gets the layout weight.
   *
   * @return The weight value
   */
  float GetWeight() const;

  /**
   * @brief Sets the cross-axis alignment for this child within the stack.
   *
   * In a vertical stack this controls horizontal placement;
   * in a horizontal stack this controls vertical placement.
   *
   * @param[in] alignment FILL, START (default), CENTER, or END
   * @return Reference to this for chaining
   */
  StackLayoutParams& SetAlignment(LayoutAlignment alignment);

  /**
   * @brief Gets the cross-axis alignment.
   *
   * @return The cross-axis alignment
   */
  LayoutAlignment GetAlignment() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Ui
} // namespace Dali
