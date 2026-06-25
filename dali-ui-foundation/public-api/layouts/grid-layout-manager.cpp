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
#include <dali-ui-foundation/public-api/layouts/grid-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/grid-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
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

bool IsChildStandalone(ViewImpl& childImpl)
{
  return childImpl.GetLayoutMode() == LayoutMode::STANDALONE;
}

// Reports whether the track at the given index behaves as auto-sized. An axis
// with no definitions falls back to a single implicit track, which behaves
// like an auto-sized track here, capturing the child content size as a floor;
// ApplyGridDefinitions then lets it grow to fill a definite grid (so
// MATCH_PARENT children also work), matching a single star cell.
bool IsAutoLikeTrack(const Dali::Vector<GridLength>& defs, uint32_t index)
{
  return defs.Size() == 0u || (index < defs.Size() && defs[index].GetType() == GridLengthType::AUTO);
}

void MeasureGridChildrenAndFillAuto(std::vector<View>& children, float availableWidth, float availableHeight,
                                    uint32_t rowCount, uint32_t colCount,
                                    const Dali::Vector<GridLength>& rowDefs,
                                    const Dali::Vector<GridLength>& colDefs,
                                    std::vector<float>&             rowHeights,
                                    std::vector<float>&             colWidths,
                                    float rowSpacing, float colSpacing, float scale)
{
  // First pass: capture the content size of every non-spanning child as the
  // floor of its auto-sized track (auto track intrinsic sizing).
  for(auto& childView : children)
  {
    ViewImpl& childImpl = GetImpl(childView);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    uint32_t row     = GetChildRow(childImpl);
    uint32_t col     = GetChildColumn(childImpl);
    uint32_t rowSpan = GetChildRowSpan(childImpl);
    uint32_t colSpan = GetChildColumnSpan(childImpl);
    row              = std::min(row, rowCount - 1);
    col              = std::min(col, colCount - 1);
    rowSpan          = std::min(rowSpan, rowCount - row);
    colSpan          = std::min(colSpan, colCount - col);

    float        childScale            = childImpl.GetEffectiveScale();
    Extents      margin                = childImpl.GetMargin();
    float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
    float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
    float        childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
    float        childHeightConstraint = std::max(0.0f, availableHeight - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

    bool rowAutoLike = IsAutoLikeTrack(rowDefs, row);
    bool colAutoLike = IsAutoLikeTrack(colDefs, col);
    if(rowSpan == 1 && rowAutoLike)
    {
      rowHeights[row] = std::max(rowHeights[row], childSize.height + marginH);
    }
    if(colSpan == 1 && colAutoLike)
    {
      colWidths[col] = std::max(colWidths[col], childSize.width + marginW);
    }
  }

  // Between the passes, seed each ABSOLUTE track with its resolved base size
  // (def.GetValue() * scale, identical to ApplyGridDefinitions). Without this
  // the span pass below sees ABSOLUTE tracks as 0, overstates the deficit, and
  // dumps the surplus onto the spanned AUTO tracks. STAR tracks are left at
  // their intrinsic 0 here: their final size depends on the remaining space
  // (which in turn depends on the AUTO sizes computed below), so it is not yet
  // known. The minimum guarantee this restores is that AUTO is not over-grown
  // by the ABSOLUTE portion of a span.
  for(uint32_t i = 0; i < colCount; ++i)
  {
    if(i < colDefs.Size() && colDefs[i].GetType() == GridLengthType::ABSOLUTE)
    {
      colWidths[i] = colDefs[i].GetValue() * scale;
    }
  }
  for(uint32_t i = 0; i < rowCount; ++i)
  {
    if(i < rowDefs.Size() && rowDefs[i].GetType() == GridLengthType::ABSOLUTE)
    {
      rowHeights[i] = rowDefs[i].GetValue() * scale;
    }
  }

  // Second pass: distribute extra space for spanning children (span > 1) across
  // the auto-sized tracks they cross. After the first pass the auto tracks hold
  // their single-cell content floor;
  // a spanning child that needs more than the sum of the tracks (plus the gaps
  // between them) it currently spans grows the auto tracks so it is not
  // clipped. The deficit is shared equally across only the spanned auto tracks;
  // non-auto tracks (absolute/star) are left untouched here.
  for(auto& childView : children)
  {
    ViewImpl& childImpl = GetImpl(childView);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    uint32_t row     = GetChildRow(childImpl);
    uint32_t col     = GetChildColumn(childImpl);
    uint32_t rowSpan = GetChildRowSpan(childImpl);
    uint32_t colSpan = GetChildColumnSpan(childImpl);
    row              = std::min(row, rowCount - 1);
    col              = std::min(col, colCount - 1);
    rowSpan          = std::min(rowSpan, rowCount - row);
    colSpan          = std::min(colSpan, colCount - col);

    if(rowSpan <= 1 && colSpan <= 1)
    {
      continue; // Non-spanning children are already handled by the first pass.
    }

    float        childScale            = childImpl.GetEffectiveScale();
    Extents      margin                = childImpl.GetMargin();
    float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
    float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
    float        childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
    float        childHeightConstraint = std::max(0.0f, availableHeight - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

    // Column axis: grow spanned auto columns to fit the child's width.
    if(colSpan > 1)
    {
      uint32_t spannedAutoCount = 0u;
      float    currentSpanSize  = colSpacing * static_cast<float>(colSpan - 1); // Gaps between spanned tracks.
      for(uint32_t i = 0; i < colSpan; ++i)
      {
        currentSpanSize += colWidths[col + i];
        if(IsAutoLikeTrack(colDefs, col + i))
        {
          ++spannedAutoCount;
        }
      }
      if(spannedAutoCount > 0u)
      {
        float deficit = std::max(0.0f, (childSize.width + marginW) - currentSpanSize);
        if(deficit > 0.0f)
        {
          float share = deficit / static_cast<float>(spannedAutoCount);
          for(uint32_t i = 0; i < colSpan; ++i)
          {
            if(IsAutoLikeTrack(colDefs, col + i))
            {
              colWidths[col + i] += share;
            }
          }
        }
      }
    }

    // Row axis: grow spanned auto rows to fit the child's height.
    if(rowSpan > 1)
    {
      uint32_t spannedAutoCount = 0u;
      float    currentSpanSize  = rowSpacing * static_cast<float>(rowSpan - 1); // Gaps between spanned tracks.
      for(uint32_t i = 0; i < rowSpan; ++i)
      {
        currentSpanSize += rowHeights[row + i];
        if(IsAutoLikeTrack(rowDefs, row + i))
        {
          ++spannedAutoCount;
        }
      }
      if(spannedAutoCount > 0u)
      {
        float deficit = std::max(0.0f, (childSize.height + marginH) - currentSpanSize);
        if(deficit > 0.0f)
        {
          float share = deficit / static_cast<float>(spannedAutoCount);
          for(uint32_t i = 0; i < rowSpan; ++i)
          {
            if(IsAutoLikeTrack(rowDefs, row + i))
            {
              rowHeights[row + i] += share;
            }
          }
        }
      }
    }
  }
}

void ApplyGridDefinitions(std::vector<float>& rowHeights, std::vector<float>& colWidths,
                          const Dali::Vector<GridLength>& rowDefs, const Dali::Vector<GridLength>& colDefs,
                          float availableWidth, float availableHeight, float rowSpacing, float colSpacing,
                          uint32_t rowCount, uint32_t colCount, float& totalWidthOut, float& totalHeightOut,
                          float scale)
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
        colWidths[i] = def.GetValue() * scale;
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
    else
    {
      // Implicit track (axis has no definitions): treat as Star(1) so it fills
      // a definite grid. Its auto content floor is preserved by the star pass.
      totalStarWidth += 1.0f;
    }
  }
  for(uint32_t i = 0; i < rowCount; ++i)
  {
    if(i < rowDefs.Size())
    {
      const auto& def = rowDefs[i];
      if(def.GetType() == GridLengthType::ABSOLUTE)
      {
        rowHeights[i] = def.GetValue() * scale;
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
    else
    {
      // Implicit track (axis has no definitions): treat as Star(1).
      totalStarHeight += 1.0f;
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
    else if(i >= colDefs.Size())
    {
      // Implicit Star(1) track: fill the available space but never shrink below
      // the child content floor captured during the auto pass.
      float share  = (totalStarWidth > 0) ? (1.0f / totalStarWidth) * remainingWidth : 0.0f;
      colWidths[i] = std::max(colWidths[i], share);
    }
  }
  for(uint32_t i = 0; i < rowCount; ++i)
  {
    if(i < rowDefs.Size() && rowDefs[i].GetType() == GridLengthType::STAR)
    {
      float starValue = rowDefs[i].GetValue();
      rowHeights[i]   = (totalStarHeight > 0) ? (starValue / totalStarHeight) * remainingHeight : 0.0f;
    }
    else if(i >= rowDefs.Size())
    {
      // Implicit Star(1) track: fill the available space but never shrink below
      // the child content floor captured during the auto pass.
      float share   = (totalStarHeight > 0) ? (1.0f / totalStarHeight) * remainingHeight : 0.0f;
      rowHeights[i] = std::max(rowHeights[i], share);
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

void ArrangeGridChildrenToCells(std::vector<View>& children, const std::vector<float>& rowPositions,
                                const std::vector<float>& colPositions, uint32_t rowCount, uint32_t colCount,
                                float rowSpacing, float colSpacing)
{
  for(auto& childView : children)
  {
    ViewImpl& childImpl = GetImpl(childView);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    uint32_t row     = std::min(GetChildRow(childImpl), rowCount - 1);
    uint32_t col     = std::min(GetChildColumn(childImpl), colCount - 1);
    uint32_t rowSpan = std::min(GetChildRowSpan(childImpl), rowCount - row);
    uint32_t colSpan = std::min(GetChildColumnSpan(childImpl), colCount - col);

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

    float   childScale = childImpl.GetEffectiveScale();
    Extents margin     = childImpl.GetMargin();
    childBounds.x += static_cast<float>(margin.start) * childScale;
    childBounds.y += static_cast<float>(margin.top) * childScale;
    childBounds.width  = std::max(0.0f, childBounds.width - static_cast<float>(margin.start + margin.end) * childScale);
    childBounds.height = std::max(0.0f, childBounds.height - static_cast<float>(margin.top + margin.bottom) * childScale);

    float        cellWidth     = childBounds.width;
    float        cellHeight    = childBounds.height;
    MeasuredSize childMeasured = childImpl.GetMeasuredSize();
    float        childWidth    = childMeasured.width;
    float        childHeight   = childMeasured.height;

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

    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childImpl.Measure(childBounds.width, childBounds.height);
    }
    childImpl.Arrange(childBounds);
  }
}

} // namespace

class GridLayoutManager::Impl : public LayoutManager::Impl
{
public:
  Impl(const Dali::Vector<GridLength>& rows, const Dali::Vector<GridLength>& columns, float rowSpacing,
       float columnSpacing)
  : mRowDefinitions(rows),
    mColumnDefinitions(columns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing)
  {
  }

  Dali::Vector<GridLength> mRowDefinitions;
  Dali::Vector<GridLength> mColumnDefinitions;
  float                    mRowSpacing;
  float                    mColumnSpacing;
};

GridLayoutManager::GridLayoutManager(const Dali::Vector<GridLength>& rows, const Dali::Vector<GridLength>& columns,
                                     float rowSpacing, float columnSpacing)
: LayoutManager(new Impl(rows, columns, rowSpacing, columnSpacing))
{
}

GridLayoutManager::~GridLayoutManager()
{
}

void GridLayoutManager::SetRowDefinitions(const Dali::Vector<GridLength>& rows)
{
  GetImplAs<Impl>()->mRowDefinitions = rows;
}

const Dali::Vector<GridLength>& GridLayoutManager::GetRowDefinitions() const
{
  return GetImplAs<Impl>()->mRowDefinitions;
}

void GridLayoutManager::SetColumnDefinitions(const Dali::Vector<GridLength>& columns)
{
  GetImplAs<Impl>()->mColumnDefinitions = columns;
}

const Dali::Vector<GridLength>& GridLayoutManager::GetColumnDefinitions() const
{
  return GetImplAs<Impl>()->mColumnDefinitions;
}

void GridLayoutManager::SetRowSpacing(float spacing)
{
  GetImplAs<Impl>()->mRowSpacing = spacing;
}

float GridLayoutManager::GetRowSpacing() const
{
  return GetImplAs<Impl>()->mRowSpacing;
}

void GridLayoutManager::SetColumnSpacing(float spacing)
{
  GetImplAs<Impl>()->mColumnSpacing = spacing;
}

float GridLayoutManager::GetColumnSpacing() const
{
  return GetImplAs<Impl>()->mColumnSpacing;
}

MeasuredSize GridLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  float s               = view->GetEffectiveScale();
  float visRowSpacing   = impl->mRowSpacing * s;
  float visColSpacing   = impl->mColumnSpacing * s;
  float availableWidth  = widthConstraint;
  float availableHeight = heightConstraint;

  // Collect children once for reuse across the measure phase helpers.
  const uint32_t    childCount = GetChildCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildAt(view, i));
  }

  uint32_t           rowCount = std::max(1u, static_cast<uint32_t>(impl->mRowDefinitions.Size()));
  uint32_t           colCount = std::max(1u, static_cast<uint32_t>(impl->mColumnDefinitions.Size()));
  std::vector<float> rowHeights(rowCount, 0.0f);
  std::vector<float> colWidths(colCount, 0.0f);

  MeasureGridChildrenAndFillAuto(children, availableWidth, availableHeight, rowCount,
                                 colCount, impl->mRowDefinitions, impl->mColumnDefinitions, rowHeights, colWidths,
                                 visRowSpacing, visColSpacing, s);

  Extents parentPadding   = view->GetPadding();
  float   requestedWidth  = view->GetRequestedWidth();
  float   requestedHeight = view->GetRequestedHeight();

  float starAvailableWidth  = availableWidth;
  float starAvailableHeight = availableHeight;

  if(requestedWidth != MATCH_PARENT && requestedWidth < 0.0f)
  {
    float nonStarWidth = 0.0f;
    for(uint32_t i = 0; i < colCount; ++i)
    {
      if(i >= impl->mColumnDefinitions.Size() || impl->mColumnDefinitions[i].GetType() != GridLengthType::STAR)
      {
        nonStarWidth += colWidths[i];
      }
    }
    nonStarWidth += visColSpacing * (colCount > 0 ? colCount - 1 : 0);
    float pw              = static_cast<float>(parentPadding.start + parentPadding.end) * s;
    float minWidthContent = std::max(0.0f, view->GetMinimumWidth() * s - pw);
    starAvailableWidth    = std::max(nonStarWidth, minWidthContent);
  }

  if(requestedHeight != MATCH_PARENT && requestedHeight < 0.0f)
  {
    float nonStarHeight = 0.0f;
    for(uint32_t i = 0; i < rowCount; ++i)
    {
      if(i >= impl->mRowDefinitions.Size() || impl->mRowDefinitions[i].GetType() != GridLengthType::STAR)
      {
        nonStarHeight += rowHeights[i];
      }
    }
    nonStarHeight += visRowSpacing * (rowCount > 0 ? rowCount - 1 : 0);
    float ph               = static_cast<float>(parentPadding.top + parentPadding.bottom) * s;
    float minHeightContent = std::max(0.0f, view->GetMinimumHeight() * s - ph);
    starAvailableHeight    = std::max(nonStarHeight, minHeightContent);
  }

  float totalWidth  = 0.0f;
  float totalHeight = 0.0f;
  ApplyGridDefinitions(rowHeights, colWidths, impl->mRowDefinitions, impl->mColumnDefinitions, starAvailableWidth, starAvailableHeight,
                       visRowSpacing, visColSpacing, rowCount, colCount, totalWidth, totalHeight, s);

  return MeasuredSize(totalWidth, totalHeight);
}

MeasuredSize GridLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  float s               = view->GetEffectiveScale();
  float visRowSpacing   = impl->mRowSpacing * s;
  float visColSpacing   = impl->mColumnSpacing * s;
  float availableWidth  = bounds.width;
  float availableHeight = bounds.height;

  const uint32_t    childCount = GetChildCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildAt(view, i));
  }

  uint32_t           rowCount = std::max(1u, static_cast<uint32_t>(impl->mRowDefinitions.Size()));
  uint32_t           colCount = std::max(1u, static_cast<uint32_t>(impl->mColumnDefinitions.Size()));
  std::vector<float> rowHeights(rowCount, 0.0f);
  std::vector<float> colWidths(colCount, 0.0f);

  // Re-measure children to get fresh auto row/column sizes using actual arrange bounds
  MeasureGridChildrenAndFillAuto(children, availableWidth, availableHeight, rowCount,
                                 colCount, impl->mRowDefinitions, impl->mColumnDefinitions, rowHeights, colWidths,
                                 visRowSpacing, visColSpacing, s);

  float totalWidth  = 0.0f;
  float totalHeight = 0.0f;
  ApplyGridDefinitions(rowHeights, colWidths, impl->mRowDefinitions, impl->mColumnDefinitions, availableWidth, availableHeight,
                       visRowSpacing, visColSpacing, rowCount, colCount, totalWidth, totalHeight, s);

  std::vector<float> rowPositions;
  std::vector<float> colPositions;
  ComputeGridPositions(rowHeights, colWidths, bounds, visRowSpacing, visColSpacing, rowCount, colCount, rowPositions,
                       colPositions);

  ArrangeGridChildrenToCells(children, rowPositions, colPositions, rowCount, colCount, visRowSpacing, visColSpacing);

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Ui
} // namespace Dali
