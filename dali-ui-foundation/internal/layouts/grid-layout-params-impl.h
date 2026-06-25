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
#include <dali/public-api/object/base-handle.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/layouts/layout-params-impl.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/trait-id.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores GridLayout child parameters.
 */
class GridLayoutParamsImpl : public LayoutParamsImpl
{
public:
  /**
   * @brief Constructs with default grid position (row=0, column=0) and span (1x1).
   */
  GridLayoutParamsImpl()
  : LayoutParamsImpl(),
    mRow(0),
    mColumn(0),
    mRowSpan(1),
    mColumnSpan(1),
    mHorizontalAlignment(LayoutAlignment::FILL),
    mVerticalAlignment(LayoutAlignment::FILL)
  {
  }

  GridLayoutParamsImpl(const GridLayoutParamsImpl& other)
  : LayoutParamsImpl(),
    mRow(other.mRow),
    mColumn(other.mColumn),
    mRowSpan(other.mRowSpan),
    mColumnSpan(other.mColumnSpan),
    mHorizontalAlignment(other.mHorizontalAlignment),
    mVerticalAlignment(other.mVerticalAlignment)
  {
  }

  TraitId GetTraitId() const override
  {
    return Integration::ReservedTraitId::GRID_LAYOUT_PARAMS;
  }

  /**
   * @brief Sets the row index for this child in the grid.
   * @param[in] row Zero-based row index.
   */
  void SetRow(uint32_t row)
  {
    mRow = row;
  }

  /**
   * @brief Gets the row index for this child in the grid.
   * @return The row index.
   */
  uint32_t GetRow() const
  {
    return mRow;
  }

  /**
   * @brief Sets the column index for this child in the grid.
   * @param[in] column Zero-based column index.
   */
  void SetColumn(uint32_t column)
  {
    mColumn = column;
  }

  /**
   * @brief Gets the column index for this child in the grid.
   * @return The column index.
   */
  uint32_t GetColumn() const
  {
    return mColumn;
  }

  /**
   * @brief Sets how many rows this child spans.
   * @param[in] span Number of rows to span (minimum 1).
   */
  void SetRowSpan(uint32_t span)
  {
    mRowSpan = std::max(1u, span);
  }

  /**
   * @brief Gets how many rows this child spans.
   * @return The row span count.
   */
  uint32_t GetRowSpan() const
  {
    return mRowSpan;
  }

  /**
   * @brief Sets how many columns this child spans.
   * @param[in] span Number of columns to span (minimum 1).
   */
  void SetColumnSpan(uint32_t span)
  {
    mColumnSpan = std::max(1u, span);
  }

  /**
   * @brief Gets how many columns this child spans.
   * @return The column span count.
   */
  uint32_t GetColumnSpan() const
  {
    return mColumnSpan;
  }

  void SetHorizontalAlignment(LayoutAlignment alignment)
  {
    mHorizontalAlignment = alignment;
  }

  LayoutAlignment GetHorizontalAlignment() const
  {
    return mHorizontalAlignment;
  }

  void SetVerticalAlignment(LayoutAlignment alignment)
  {
    mVerticalAlignment = alignment;
  }

  LayoutAlignment GetVerticalAlignment() const
  {
    return mVerticalAlignment;
  }

  /**
   * @brief Retrieves the GridLayoutParams trait attached to a view, if any.
   * @param[in] viewImpl The view implementation to query.
   * @return Pointer to the params, or nullptr if not attached.
   */
  static GridLayoutParamsImpl* Get(ViewImpl& viewImpl)
  {
    IntrusivePtr<TraitObject> object = Integration::View::GetTrait(viewImpl, Integration::ReservedTraitId::GRID_LAYOUT_PARAMS);
    DALI_ASSERT_DEBUG(!object || (dynamic_cast<GridLayoutParamsImpl*>(object.Get()) && "GRID_LAYOUT_PARAMS trait must be a GridLayoutParamsImpl"));
    return object ? static_cast<GridLayoutParamsImpl*>(object.Get()) : nullptr;
  }

  /**
   * @brief Retrieves or creates the GridLayoutParams trait on a view.
   * @param[in] viewImpl The view implementation.
   * @return Reference to the params (newly created if not already attached).
   */
  static GridLayoutParamsImpl& GetOrCreate(ViewImpl& viewImpl)
  {
    GridLayoutParamsImpl* existing = Get(viewImpl);
    if(existing)
    {
      return *existing;
    }
    GridLayoutParams params = GridLayoutParams::New();
    Integration::View::SetTrait(viewImpl, Integration::ReservedTraitId::GRID_LAYOUT_PARAMS, IntrusivePtr<TraitObject>(&static_cast<TraitObject&>(params.GetBaseObject())));
    return static_cast<GridLayoutParamsImpl&>(params.GetBaseObject());
  }

protected:
  ~GridLayoutParamsImpl() override = default;

private:
  uint32_t        mRow;
  uint32_t        mColumn;
  uint32_t        mRowSpan;
  uint32_t        mColumnSpan;
  LayoutAlignment mHorizontalAlignment;
  LayoutAlignment mVerticalAlignment;
};

} // namespace Internal

inline Internal::GridLayoutParamsImpl& GetImpl(GridLayoutParams& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::GridLayoutParamsImpl&>(handle);
}

inline const Internal::GridLayoutParamsImpl& GetImpl(const GridLayoutParams& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::GridLayoutParamsImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
