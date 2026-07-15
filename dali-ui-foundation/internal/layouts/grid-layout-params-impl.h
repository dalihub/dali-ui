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
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/traits/trait-id.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Trait implementation that stores GridLayout child parameters.
 */
class GridLayoutParamsImpl : public TraitObject
{
public:
  explicit GridLayoutParamsImpl(const GridLayoutParams& params)
  : mRow(params.GetRow()),
    mColumn(params.GetColumn()),
    mRowSpan(params.GetRowSpan()),
    mColumnSpan(params.GetColumnSpan()),
    mHorizontalAlignment(params.GetHorizontalAlignment()),
    mVerticalAlignment(params.GetVerticalAlignment())
  {
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
   * @brief Gets the column index for this child in the grid.
   * @return The column index.
   */
  uint32_t GetColumn() const
  {
    return mColumn;
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
   * @brief Gets how many columns this child spans.
   * @return The column span count.
   */
  uint32_t GetColumnSpan() const
  {
    return mColumnSpan;
  }

  LayoutAlignment GetHorizontalAlignment() const
  {
    return mHorizontalAlignment;
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

} // namespace Ui
} // namespace Dali
