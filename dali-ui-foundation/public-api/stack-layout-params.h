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
#include <dali-ui-foundation/public-api/layout-params.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
namespace Internal
{
class StackLayoutParamsImpl;
}

/**
 * @brief StackLayoutParams stores per-child layout parameters for StackLayout.
 *
 * This handle provides direct access to the layout weight attached to a child view.
 * Use View::SetLayoutParams() to attach parameters to a child view.
 *
 * @code
 * view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
 * @endcode
 */
class DALI_UI_API StackLayoutParams : public LayoutParams
{
public:
  /**
   * @brief Creates an uninitialized handle.
   */
  StackLayoutParams();

  /**
   * @brief Creates a new StackLayoutParams with default values.
   *
   * @return A handle to a newly allocated StackLayoutParams
   */
  static StackLayoutParams New();

  /**
   * @brief Creates a new StackLayoutParams by copying values from an existing one.
   *
   * @param[in] other The params to copy from
   * @return A handle to a newly allocated StackLayoutParams with copied values
   */
  static StackLayoutParams New(const StackLayoutParams& other);

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  StackLayoutParams(const StackLayoutParams& handle);

  /**
   * @brief Destructor.
   */
  ~StackLayoutParams();

  /**
   * @brief Downcasts a handle to StackLayoutParams.
   *
   * @param[in] handle Handle to an object
   * @return A valid handle or an uninitialized handle
   */
  static StackLayoutParams DownCast(BaseHandle handle);

  /**
   * @brief Returns the layout params type identifier.
   *
   * @return LayoutParamsType::STACK
   */
  static LayoutParamsType GetLayoutParamsType();

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

public: // Not intended for application developers
  /// @cond internal
  explicit StackLayoutParams(Internal::StackLayoutParamsImpl* implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
