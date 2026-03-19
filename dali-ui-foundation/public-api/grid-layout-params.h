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
#include <cstdint>

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
class GridLayoutParamsImpl;
}

/**
 * @brief GridLayoutParams stores per-child layout parameters for GridLayout.
 *
 * This handle provides direct access to row, column, rowSpan, and columnSpan
 * attached to a child view.
 * Use View::SetLayoutParams() to attach parameters to a child view.
 *
 * @code
 * view.SetLayoutParams(GridLayoutParams::New()
 *   .SetRow(1)
 *   .SetColumn(2)
 *   .SetColumnSpan(3));
 * @endcode
 */
class DALI_UI_API GridLayoutParams : public LayoutParams
{
public:
  /**
   * @brief Creates an uninitialized handle.
   */
  GridLayoutParams();

  /**
   * @brief Creates a new GridLayoutParams with default values.
   *
   * @return A handle to a newly allocated GridLayoutParams
   */
  static GridLayoutParams New();

  /**
   * @brief Creates a new GridLayoutParams by copying values from an existing one.
   *
   * @param[in] other The params to copy from
   * @return A handle to a newly allocated GridLayoutParams with copied values
   */
  static GridLayoutParams New(const GridLayoutParams& other);

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  GridLayoutParams(const GridLayoutParams& handle);

  /**
   * @brief Destructor.
   */
  ~GridLayoutParams();

  /**
   * @brief Downcasts a handle to GridLayoutParams.
   *
   * @param[in] handle Handle to an object
   * @return A valid handle or an uninitialized handle
   */
  static GridLayoutParams DownCast(BaseHandle handle);

  /**
   * @brief Returns the layout params type identifier.
   *
   * @return LayoutParamsType::GRID
   */
  static LayoutParamsType GetLayoutParamsType();

  /**
   * @brief Sets the row index for this child in the grid.
   *
   * @param[in] row Zero-based row index
   * @return Reference to this for chaining
   */
  GridLayoutParams& SetRow(uint32_t row);

  /**
   * @brief Gets the row index for this child in the grid.
   *
   * @return The row index
   */
  uint32_t GetRow() const;

  /**
   * @brief Sets the column index for this child in the grid.
   *
   * @param[in] column Zero-based column index
   * @return Reference to this for chaining
   */
  GridLayoutParams& SetColumn(uint32_t column);

  /**
   * @brief Gets the column index for this child in the grid.
   *
   * @return The column index
   */
  uint32_t GetColumn() const;

  /**
   * @brief Sets how many rows this child spans.
   *
   * @param[in] span Number of rows to span (minimum 1)
   * @return Reference to this for chaining
   */
  GridLayoutParams& SetRowSpan(uint32_t span);

  /**
   * @brief Gets how many rows this child spans.
   *
   * @return The row span count
   */
  uint32_t GetRowSpan() const;

  /**
   * @brief Sets how many columns this child spans.
   *
   * @param[in] span Number of columns to span (minimum 1)
   * @return Reference to this for chaining
   */
  GridLayoutParams& SetColumnSpan(uint32_t span);

  /**
   * @brief Gets how many columns this child spans.
   *
   * @return The column span count
   */
  uint32_t GetColumnSpan() const;

  /**
   * @brief Sets the horizontal alignment within the grid cell.
   *
   * @param[in] alignment FILL, START, CENTER, or END
   * @return Reference to this for chaining
   */
  GridLayoutParams& SetHorizontalAlignment(LayoutAlignment alignment);

  /**
   * @brief Gets the horizontal alignment.
   *
   * @return The horizontal alignment
   */
  LayoutAlignment GetHorizontalAlignment() const;

  /**
   * @brief Sets the vertical alignment within the grid cell.
   *
   * @param[in] alignment FILL, START, CENTER, or END
   * @return Reference to this for chaining
   */
  GridLayoutParams& SetVerticalAlignment(LayoutAlignment alignment);

  /**
   * @brief Gets the vertical alignment.
   *
   * @return The vertical alignment
   */
  LayoutAlignment GetVerticalAlignment() const;

public: // Not intended for application developers
  /// @cond internal
  explicit GridLayoutParams(Internal::GridLayoutParamsImpl* implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
