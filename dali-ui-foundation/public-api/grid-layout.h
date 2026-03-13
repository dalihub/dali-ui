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
#include <vector> // Used for row/column definitions; ABI considerations may apply across toolchain versions

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
class GridLayoutImpl;
}

/**
 * @brief GridLayout arranges its children in a grid of rows and columns.
 *
 * It supports:
 * - Row and column definitions with absolute, star (*), and auto sizing
 * - Row and column spacing
 * - Child positioning using attached properties (Row, Column, RowSpan, ColumnSpan)
 */
class DALI_UI_API GridLayout : public Layout
{
public:
  /**
   * @brief Creates an uninitialized GridLayout handle.
   */
  GridLayout();

  /**
   * @brief Creates a new GridLayout.
   *
   * @return A handle to a newly allocated GridLayout
   */
  static GridLayout New();

  /**
   * @brief Copy constructor.
   */
  GridLayout(const GridLayout& layout);

  /**
   * @brief Move constructor.
   */
  GridLayout(GridLayout&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~GridLayout();

  /**
   * @brief Copy assignment operator.
   */
  GridLayout& operator=(const GridLayout& handle);

  /**
   * @brief Move assignment operator.
   */
  GridLayout& operator=(GridLayout&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to GridLayout handle.
   */
  static GridLayout DownCast(BaseHandle handle);

public: // Row/Column Definition API
  /**
   * @brief Adds a row definition.
   *
   * @param[in] height The row height specification
   */
  void AddRowDefinition(GridLength height);

  /**
   * @brief Adds a column definition.
   *
   * @param[in] width The column width specification
   */
  void AddColumnDefinition(GridLength width);

  /**
   * @brief Sets all row definitions at once.
   *
   * @param[in] rows Vector of row height specifications
   */
  void SetRowDefinitions(const std::vector<GridLength>& rows);

  /**
   * @brief Sets all column definitions at once.
   *
   * @param[in] columns Vector of column width specifications
   */
  void SetColumnDefinitions(const std::vector<GridLength>& columns);

  /**
   * @brief Gets the row definitions.
   *
   * @return Vector of row height specifications
   */
  std::vector<GridLength> GetRowDefinitions() const;

  /**
   * @brief Gets the column definitions.
   *
   * @return Vector of column width specifications
   */
  std::vector<GridLength> GetColumnDefinitions() const;

  /**
   * @brief Gets the number of rows.
   *
   * @return The row count
   */
  uint32_t GetRowCount() const;

  /**
   * @brief Gets the number of columns.
   *
   * @return The column count
   */
  uint32_t GetColumnCount() const;

  /**
   * @brief Clears all row definitions.
   */
  void ClearRowDefinitions();

  /**
   * @brief Clears all column definitions.
   */
  void ClearColumnDefinitions();

public: // Spacing API
  /**
   * @brief Sets the row spacing.
   *
   * @param[in] spacing The spacing between rows
   */
  void SetRowSpacing(float spacing);

  /**
   * @brief Gets the row spacing.
   *
   * @return The row spacing
   */
  float GetRowSpacing() const;

  /**
   * @brief Sets the column spacing.
   *
   * @param[in] spacing The spacing between columns
   */
  void SetColumnSpacing(float spacing);

  /**
   * @brief Gets the column spacing.
   *
   * @return The column spacing
   */
  float GetColumnSpacing() const;

public: // Chaining methods
  DALI_UI_CHAIN_LAYOUT_METHODS(GridLayout)

  GridLayout& RowSpacing(float spacing)
  {
    SetRowSpacing(spacing);
    return *this;
  }

  GridLayout& ColumnSpacing(float spacing)
  {
    SetColumnSpacing(spacing);
    return *this;
  }

  GridLayout& Rows(const std::vector<GridLength>& rows)
  {
    SetRowDefinitions(rows);
    return *this;
  }

  GridLayout& Columns(const std::vector<GridLength>& columns)
  {
    SetColumnDefinitions(columns);
    return *this;
  }

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL          GridLayout(Integration::GridLayoutImpl& implementation);
  explicit DALI_INTERNAL GridLayout(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali