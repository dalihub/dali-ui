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

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layout.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
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
 */
class DALI_UI_API AbsoluteLayout : public Layout
{
public:
  /**
   * @brief Creates an uninitialized AbsoluteLayout handle.
   */
  AbsoluteLayout();

  /**
   * @brief Creates a new AbsoluteLayout.
   *
   * @return A handle to a newly allocated AbsoluteLayout
   */
  static AbsoluteLayout New();

  /**
   * @brief Copy constructor.
   */
  AbsoluteLayout(const AbsoluteLayout& layout);

  /**
   * @brief Move constructor.
   */
  AbsoluteLayout(AbsoluteLayout&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~AbsoluteLayout();

  /**
   * @brief Copy assignment operator.
   */
  AbsoluteLayout& operator=(const AbsoluteLayout& handle);

  /**
   * @brief Move assignment operator.
   */
  AbsoluteLayout& operator=(AbsoluteLayout&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to AbsoluteLayout handle.
   */
  static AbsoluteLayout DownCast(BaseHandle handle);

public: // Chaining methods
  DALI_UI_CHAIN_LAYOUT_METHODS(AbsoluteLayout)

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL AbsoluteLayout(Integration::AbsoluteLayoutImpl& implementation);
  explicit DALI_INTERNAL AbsoluteLayout(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali