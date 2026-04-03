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

// CLASS HEADER
#include <dali-ui-foundation/integration-api/layouts/grid-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/layouts/grid-layout-params-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

uint32_t GetChildRow(ViewImpl& childImpl)
{
  auto* params = Internal::GridLayoutParamsImpl::Get(childImpl);
  return params ? params->GetRow() : 0;
}

uint32_t GetChildColumn(ViewImpl& childImpl)
{
  auto* params = Internal::GridLayoutParamsImpl::Get(childImpl);
  return params ? params->GetColumn() : 0;
}

uint32_t GetChildRowSpan(ViewImpl& childImpl)
{
  auto* params = Internal::GridLayoutParamsImpl::Get(childImpl);
  return params ? params->GetRowSpan() : 1;
}

uint32_t GetChildColumnSpan(ViewImpl& childImpl)
{
  auto* params = Internal::GridLayoutParamsImpl::Get(childImpl);
  return params ? params->GetColumnSpan() : 1;
}

LayoutAlignment GetChildHorizontalAlignment(ViewImpl& childImpl)
{
  auto* params = Internal::GridLayoutParamsImpl::Get(childImpl);
  return params ? params->GetHorizontalAlignment() : LayoutAlignment::FILL;
}

LayoutAlignment GetChildVerticalAlignment(ViewImpl& childImpl)
{
  auto* params = Internal::GridLayoutParamsImpl::Get(childImpl);
  return params ? params->GetVerticalAlignment() : LayoutAlignment::FILL;
}

void MeasureGridChildrenAndFillAuto(ViewImpl::ChildContainer& children, float availableWidth, float availableHeight,
                                    uint32_t rowCount, uint32_t colCount, const Dali::Vector<GridLength>& rowDefs,
                                    const Dali::Vector<GridLength>&           colDefs,
                                    const std::function<ViewImpl&(Ui::View)>& getImpl, std::vector<float>& rowHeights,
                                    std::vector<float>& colWidths)
{
  for(auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    uint32_t  row       = GetChildRow(childImpl);
    uint32_t  col       = GetChildColumn(childImpl);
    uint32_t  rowSpan   = GetChildRowSpan(childImpl);
    uint32_t  colSpan   = GetChildColumnSpan(childImpl);
    row                 = std::min(row, rowCount - 1);
    col                 = std::min(col, colCount - 1);
    rowSpan             = std::min(rowSpan, rowCount - row);
    colSpan             = std::min(colSpan, colCount - col);

    Extents      margin                = childImpl.GetViewMargin();
    float        marginW               = static_cast<float>(margin.start + margin.end);
    float        marginH               = static_cast<float>(margin.top + margin.bottom);
    float        childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
    float        childHeightConstraint = std::max(0.0f, availableHeight - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    childData.measuredSize             = childSize;

    if(rowSpan == 1 && row < rowDefs.Size() && rowDefs[row].GetType() == GridLengthType::AUTO)
    {
      rowHeights[row] = std::max(rowHeights[row], childSize.height + margin.top + margin.bottom);
    }
    if(colSpan == 1 && col < colDefs.Size() && colDefs[col].GetType() == GridLengthType::AUTO)
    {
      colWidths[col] = std::max(colWidths[col], childSize.width + margin.start + margin.end);
    }
  }
}

void ApplyGridDefinitions(std::vector<float>& rowHeights, std::vector<float>& colWidths,
                          const Dali::Vector<GridLength>& rowDefs, const Dali::Vector<GridLength>& colDefs,
                          float availableWidth, float availableHeight, float rowSpacing, float colSpacing,
                          uint32_t rowCount, uint32_t colCount, float& totalWidthOut, float& totalHeightOut)
{
  float totalAbsoluteWidth  = 0.0f;
  float totalAbsoluteHeight = 0.0f;
  float totalStarWidth      = 0.0f;
  float totalStarHeight     = 0.0f;

  for(uint32_t i = 0; i < colCount; ++i)
  {
    if(i < colDefs.Size())
    {
      const auto& def = colDefs[i];
      if(def.GetType() == GridLengthType::ABSOLUTE)
      {
        colWidths[i] = def.GetValue();
        totalAbsoluteWidth += colWidths[i];
      }
      else if(def.GetType() == GridLengthType::STAR)
      {
        totalStarWidth += def.GetValue();
      }
      else
      {
        totalAbsoluteWidth += colWidths[i];
      }
    }
  }
  for(uint32_t i = 0; i < rowCount; ++i)
  {
    if(i < rowDefs.Size())
    {
      const auto& def = rowDefs[i];
      if(def.GetType() == GridLengthType::ABSOLUTE)
      {
        rowHeights[i] = def.GetValue();
        totalAbsoluteHeight += rowHeights[i];
      }
      else if(def.GetType() == GridLengthType::STAR)
      {
        totalStarHeight += def.GetValue();
      }
      else
      {
        totalAbsoluteHeight += rowHeights[i];
      }
    }
  }

  float totalSpacingWidth  = colSpacing * (colCount > 0 ? colCount - 1 : 0);
  float totalSpacingHeight = rowSpacing * (rowCount > 0 ? rowCount - 1 : 0);
  float remainingWidth     = std::max(0.0f, availableWidth - totalAbsoluteWidth - totalSpacingWidth);
  float remainingHeight    = std::max(0.0f, availableHeight - totalAbsoluteHeight - totalSpacingHeight);

  for(uint32_t i = 0; i < colCount; ++i)
  {
    if(i < colDefs.Size() && colDefs[i].GetType() == GridLengthType::STAR)
    {
      float starValue = colDefs[i].GetValue();
      colWidths[i]    = (totalStarWidth > 0) ? (starValue / totalStarWidth) * remainingWidth : 0.0f;
    }
  }
  for(uint32_t i = 0; i < rowCount; ++i)
  {
    if(i < rowDefs.Size() && rowDefs[i].GetType() == GridLengthType::STAR)
    {
      float starValue = rowDefs[i].GetValue();
      rowHeights[i]   = (totalStarHeight > 0) ? (starValue / totalStarHeight) * remainingHeight : 0.0f;
    }
  }

  totalWidthOut  = std::accumulate(colWidths.begin(), colWidths.end(), 0.0f) + totalSpacingWidth;
  totalHeightOut = std::accumulate(rowHeights.begin(), rowHeights.end(), 0.0f) + totalSpacingHeight;
}

void ComputeGridPositions(const std::vector<float>& rowHeights, const std::vector<float>& colWidths,
                          const LayoutRect& bounds, float rowSpacing, float colSpacing, uint32_t rowCount,
                          uint32_t colCount, std::vector<float>& rowPositionsOut, std::vector<float>& colPositionsOut)
{
  rowPositionsOut.resize(rowCount + 1);
  colPositionsOut.resize(colCount + 1);
  rowPositionsOut[0] = bounds.y;
  for(uint32_t i = 0; i < rowCount; ++i)
  {
    rowPositionsOut[i + 1] = rowPositionsOut[i] + rowHeights[i] + (i < rowCount - 1 ? rowSpacing : 0.0f);
  }
  colPositionsOut[0] = bounds.x;
  for(uint32_t i = 0; i < colCount; ++i)
  {
    colPositionsOut[i + 1] = colPositionsOut[i] + colWidths[i] + (i < colCount - 1 ? colSpacing : 0.0f);
  }
}

void ArrangeGridChildrenToCells(ViewImpl::ChildContainer& children, const std::vector<float>& rowPositions,
                                const std::vector<float>& colPositions, uint32_t rowCount, uint32_t colCount,
                                float rowSpacing, float colSpacing, const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  for(auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    uint32_t  row       = std::min(GetChildRow(childImpl), rowCount - 1);
    uint32_t  col       = std::min(GetChildColumn(childImpl), colCount - 1);
    uint32_t  rowSpan   = std::min(GetChildRowSpan(childImpl), rowCount - row);
    uint32_t  colSpan   = std::min(GetChildColumnSpan(childImpl), colCount - col);

    float      cellX = colPositions[col];
    float      cellY = rowPositions[row];
    LayoutRect childBounds;
    childBounds.x      = cellX;
    childBounds.y      = cellY;
    childBounds.width  = colPositions[col + colSpan] - cellX;
    childBounds.height = rowPositions[row + rowSpan] - cellY;

    if(col + colSpan < colCount)
    {
      childBounds.width -= colSpacing;
    }
    if(row + rowSpan < rowCount)
    {
      childBounds.height -= rowSpacing;
    }

    Extents margin = childImpl.GetViewMargin();
    childBounds.x += static_cast<float>(margin.start);
    childBounds.y += static_cast<float>(margin.top);
    childBounds.width  = std::max(0.0f, childBounds.width - static_cast<float>(margin.start + margin.end));
    childBounds.height = std::max(0.0f, childBounds.height - static_cast<float>(margin.top + margin.bottom));

    // Apply child alignment within the cell (from GridLayoutParams)
    float cellWidth   = childBounds.width;
    float cellHeight  = childBounds.height;
    float childWidth  = childData.measuredSize.width;
    float childHeight = childData.measuredSize.height;

    LayoutAlignment hAlign = GetChildHorizontalAlignment(childImpl);
    if(childWidth > 0.0f && childWidth < cellWidth)
    {
      switch(hAlign)
      {
        case LayoutAlignment::CENTER:
          childBounds.x += (cellWidth - childWidth) * 0.5f;
          childBounds.width = childWidth;
          break;
        case LayoutAlignment::END:
          childBounds.x += cellWidth - childWidth;
          childBounds.width = childWidth;
          break;
        case LayoutAlignment::START:
          childBounds.width = childWidth;
          break;
        case LayoutAlignment::FILL:
        default:
          break;
      }
    }

    LayoutAlignment vAlign = GetChildVerticalAlignment(childImpl);
    if(childHeight > 0.0f && childHeight < cellHeight)
    {
      switch(vAlign)
      {
        case LayoutAlignment::CENTER:
          childBounds.y += (cellHeight - childHeight) * 0.5f;
          childBounds.height = childHeight;
          break;
        case LayoutAlignment::END:
          childBounds.y += cellHeight - childHeight;
          childBounds.height = childHeight;
          break;
        case LayoutAlignment::START:
          childBounds.height = childHeight;
          break;
        case LayoutAlignment::FILL:
        default:
          break;
      }
    }

    childImpl.Arrange(childBounds);
    childData.arrangedBounds = childBounds;
  }
}

} // namespace

GridLayoutManager::GridLayoutManager(const Dali::Vector<GridLength>& rows, const Dali::Vector<GridLength>& columns,
                                     float rowSpacing, float columnSpacing)
: LayoutManager(),
  mRowDefinitions(rows),
  mColumnDefinitions(columns),
  mRowSpacing(rowSpacing),
  mColumnSpacing(columnSpacing)
{
}

GridLayoutManager::~GridLayoutManager()
{
}

void GridLayoutManager::SetRowDefinitions(const Dali::Vector<GridLength>& rows)
{
  mRowDefinitions = rows;
}

const Dali::Vector<GridLength>& GridLayoutManager::GetRowDefinitions() const
{
  return mRowDefinitions;
}

void GridLayoutManager::SetColumnDefinitions(const Dali::Vector<GridLength>& columns)
{
  mColumnDefinitions = columns;
}

const Dali::Vector<GridLength>& GridLayoutManager::GetColumnDefinitions() const
{
  return mColumnDefinitions;
}

void GridLayoutManager::SetRowSpacing(float spacing)
{
  mRowSpacing = spacing;
}

float GridLayoutManager::GetRowSpacing() const
{
  return mRowSpacing;
}

void GridLayoutManager::SetColumnSpacing(float spacing)
{
  mColumnSpacing = spacing;
}

float GridLayoutManager::GetColumnSpacing() const
{
  return mColumnSpacing;
}

MeasuredSize GridLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto&   children        = GetChildren(view);
  Extents padding         = view->GetViewPadding();
  float   availableWidth  = widthConstraint - static_cast<float>(padding.start + padding.end);
  float   availableHeight = heightConstraint - static_cast<float>(padding.top + padding.bottom);
  availableWidth          = std::max(0.0f, availableWidth);
  availableHeight         = std::max(0.0f, availableHeight);

  uint32_t           rowCount = std::max(1u, static_cast<uint32_t>(mRowDefinitions.Size()));
  uint32_t           colCount = std::max(1u, static_cast<uint32_t>(mColumnDefinitions.Size()));
  std::vector<float> rowHeights(rowCount, 0.0f);
  std::vector<float> colWidths(colCount, 0.0f);

  auto getImpl = [this](Ui::View v) -> ViewImpl&
  { return GetImpl(v); };
  MeasureGridChildrenAndFillAuto(children, availableWidth, availableHeight, rowCount, colCount, mRowDefinitions,
                                 mColumnDefinitions, getImpl, rowHeights, colWidths);

  float totalWidth  = 0.0f;
  float totalHeight = 0.0f;
  ApplyGridDefinitions(rowHeights, colWidths, mRowDefinitions, mColumnDefinitions, availableWidth, availableHeight,
                       mRowSpacing, mColumnSpacing, rowCount, colCount, totalWidth, totalHeight);

  return MeasuredSize(totalWidth, totalHeight);
}

MeasuredSize GridLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto&              children        = GetChildren(view);
  float              availableWidth  = bounds.width;
  float              availableHeight = bounds.height;
  uint32_t           rowCount        = std::max(1u, static_cast<uint32_t>(mRowDefinitions.Size()));
  uint32_t           colCount        = std::max(1u, static_cast<uint32_t>(mColumnDefinitions.Size()));
  std::vector<float> rowHeights(rowCount, 0.0f);
  std::vector<float> colWidths(colCount, 0.0f);

  auto getImpl = [this](Ui::View v) -> ViewImpl&
  { return GetImpl(v); };
  // Re-measure children to get fresh auto row/column sizes using actual arrange bounds
  MeasureGridChildrenAndFillAuto(children, availableWidth, availableHeight, rowCount, colCount, mRowDefinitions,
                                 mColumnDefinitions, getImpl, rowHeights, colWidths);

  float totalWidth  = 0.0f;
  float totalHeight = 0.0f;
  ApplyGridDefinitions(rowHeights, colWidths, mRowDefinitions, mColumnDefinitions, availableWidth, availableHeight,
                       mRowSpacing, mColumnSpacing, rowCount, colCount, totalWidth, totalHeight);

  std::vector<float> rowPositions;
  std::vector<float> colPositions;
  ComputeGridPositions(rowHeights, colWidths, bounds, mRowSpacing, mColumnSpacing, rowCount, colCount, rowPositions,
                       colPositions);

  ArrangeGridChildrenToCells(children, rowPositions, colPositions, rowCount, colCount, mRowSpacing, mColumnSpacing,
                             getImpl);

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
