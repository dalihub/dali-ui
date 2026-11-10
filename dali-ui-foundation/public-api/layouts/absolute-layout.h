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
#include <dali-ui-foundation/public-api/layouts/layout.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
class AbsoluteLayoutImpl;
}

/**
 * @brief AbsoluteLayout positions its children at explicit coordinates.
 *
 * Children are positioned using attached properties that specify their
 * bounds (x, y, width, height).
 *
 * Supports:
 * - Absolute positioning with pixel values
 * - Proportional positioning (0.0 to 1.0 relative to parent)
 * - Proportional sizing (0.0 to 1.0 relative to parent)
 *
 * @section abs_common_props Common Property Behavior
 *
 * - @b RequestedWidth/RequestedHeight: Only used when the corresponding bounds
 *   dimension is negative (auto-size). When bounds width or height is >= 0,
 *   the bounds value takes precedence and RequestedWidth/RequestedHeight is
 *   ignored.
 * - @b Alignment: Not used. Position is determined solely by the bounds
 *   x/y values and margin.
 * - @b Margin: Applied as an offset to the bounds position.
 * @SINCE_3_0.0
 */
class DALI_UI_API AbsoluteLayout : public Layout
{
public:
  /**
   * @brief Creates an uninitialized AbsoluteLayout handle.
   * @SINCE_3_0.0
   */
  AbsoluteLayout();

  /**
   * @brief Creates a new AbsoluteLayout.
   *
   * @SINCE_3_0.0
   * @return A handle to a newly allocated AbsoluteLayout
   */
  static AbsoluteLayout New();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  AbsoluteLayout(const AbsoluteLayout& layout);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  AbsoluteLayout(AbsoluteLayout&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~AbsoluteLayout();

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  AbsoluteLayout& operator=(const AbsoluteLayout& handle);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  AbsoluteLayout& operator=(AbsoluteLayout&& rhs) noexcept;

  DALI_UI_VIEW_WITH(AbsoluteLayout)

  /**
   * @brief Downcasts a handle to AbsoluteLayout handle.
   * @SINCE_3_0.0
   */
  static AbsoluteLayout DownCast(BaseHandle handle);

public: // API
public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Layout implementation
   */
  explicit AbsoluteLayout(Integration::AbsoluteLayoutImpl& implementation);

  /**
   * @brief Allows the creation of this Layout from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit AbsoluteLayout(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui
} //namespace DALI_NAMESPACE
