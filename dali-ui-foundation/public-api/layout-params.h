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
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class LayoutParamsImpl;
}

/**
 * @brief LayoutParams is the base handle class for all layout parameter types.
 *
 * It provides a common base for AbsoluteLayoutParams, FlexLayoutParams,
 * GridLayoutParams, and StackLayoutParams. Use with
 * View::SetLayoutParams() and View::GetLayoutParams().
 */
class DALI_UI_API LayoutParams : public Trait
{
public:
  /**
   * @brief Creates an uninitialized LayoutParams handle.
   */
  LayoutParams();

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  LayoutParams(const LayoutParams& handle);

  /**
   * @brief Destructor.
   */
  ~LayoutParams();

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  LayoutParams& operator=(const LayoutParams& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  LayoutParams(LayoutParams&& rhs) noexcept = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  LayoutParams& operator=(LayoutParams&& rhs) noexcept = default;

  /**
   * @brief Downcasts a handle to LayoutParams.
   *
   * @param[in] handle Handle to an object
   * @return A valid handle or an uninitialized handle
   */
  static LayoutParams DownCast(BaseHandle handle);

public: // Not intended for application developers
  /// @cond internal
  explicit LayoutParams(Internal::LayoutParamsImpl* implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
