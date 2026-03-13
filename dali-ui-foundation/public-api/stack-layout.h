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
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace Ui
{

// Forward declarations
namespace Integration
{
class StackLayoutImpl;
}

/**
 * @brief StackLayout arranges its children in a single row or column.
 *
 * StackLayout is a simple layout container that stacks its children either
 * horizontally or vertically with optional spacing between them.
 */
class DALI_UI_API StackLayout : public Layout
{
public:
  /**
   * @brief Creates an uninitialized StackLayout handle.
   */
  StackLayout();

  /**
   * @brief Creates a new StackLayout with the specified orientation.
   *
   * @param[in] orientation The orientation of the stack (default: Vertical)
   * @return A handle to a newly allocated StackLayout
   */
  static StackLayout New(StackOrientation orientation = StackOrientation::VERTICAL);

  /**
   * @brief Copy constructor.
   *
   * @param[in] layout Handle to copy
   */
  StackLayout(const StackLayout& layout);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  StackLayout(StackLayout&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~StackLayout();

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  StackLayout& operator=(const StackLayout& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  StackLayout& operator=(StackLayout&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to StackLayout handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a StackLayout or an uninitialized handle
   */
  static StackLayout DownCast(BaseHandle handle);

public: // API

  /**
   * @brief Sets the orientation of the stack layout.
   *
   * @param[in] orientation The orientation to set
   */
  void SetOrientation(StackOrientation orientation);

  /**
   * @brief Gets the orientation of the stack layout.
   *
   * @return The current orientation
   */
  StackOrientation GetOrientation() const;

  /**
   * @brief Sets the spacing between children in the stack.
   *
   * @param[in] spacing The spacing to set (in pixels)
   */
  void SetSpacing(float spacing);

  /**
   * @brief Gets the spacing between children in the stack.
   *
   * @return The current spacing
   */
  float GetSpacing() const;

public: // Static methods for attached properties (StackLayout children only)

  /**
   * @brief Sets the layout weight for a child view.
   *
   * Remaining space on the main axis is distributed among children with
   * weight > 0 in proportion to their weight. Only valid when the view is
   * a child of a StackLayout; ignored otherwise.
   *
   * @param[in] view The child view
   * @param[in] weight The weight (default 0; use 0 to not participate in weight distribution)
   */
  static void SetLayoutWeight(View view, float weight);

  /**
   * @brief Gets the layout weight for a child view.
   *
   * @param[in] view The child view
   * @return The weight (0 if not set)
   */
  static float GetLayoutWeight(View view);

public: // Chaining methods
  DALI_UI_CHAIN_LAYOUT_METHODS(StackLayout)

  /**
   * @brief Sets the orientation (chaining).
   */
  StackLayout& Orientation(StackOrientation orientation)
  {
    SetOrientation(orientation);
    return *this;
  }

  /**
   * @brief Sets the spacing (chaining).
   */
  StackLayout& Spacing(float spacing)
  {
    SetSpacing(spacing);
    return *this;
  }

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The Control implementation
   */
  DALI_INTERNAL StackLayout(Integration::StackLayoutImpl& implementation);

  /**
   * @brief Allows the creation of this Control from an Integration::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL StackLayout(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali