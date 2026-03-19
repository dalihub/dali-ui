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
#include <dali-ui-foundation/public-api/layout-types.h>
#include <dali-ui-foundation/public-api/layout.h>

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
 *
 * @section stack_common_props Common Property Behavior
 *
 * For a vertical StackLayout (horizontal is analogous with axes swapped):
 *
 * - @b RequestedHeight (main-axis): A positive value sets a fixed height.
 *   WRAP_CONTENT (-1) uses the child's natural size.
 *   MATCH_PARENT (-2) fills the parent.
 * - @b RequestedWidth (cross-axis): A positive value sets a fixed width.
 *   WRAP_CONTENT (-1) uses the child's natural size.
 *   MATCH_PARENT (-2) fills the cross-axis.
 * - @b StackLayoutParams alignment (cross-axis): START, CENTER, and END
 *   position the child within the cross-axis.
 * - @b Margin: Applied on all sides, reducing the space available for the child.
 * - @b Weight (via StackLayoutParams): Children with weight > 0 share the
 *   remaining main-axis space proportionally. The child's main-axis
 *   RequestedWidth/RequestedHeight is ignored — the size is determined entirely
 *   by the weight ratio. Only children with weight 0 are measured normally
 *   using their RequestedWidth/RequestedHeight.
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