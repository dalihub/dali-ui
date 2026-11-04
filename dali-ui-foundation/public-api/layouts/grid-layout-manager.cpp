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
#include <typeinfo>
#include <vector>

// INTERNAL INCLUDES
// label-impl.h stays in a block of its own, ahead of every other internal header: it is read for typeid only
// (ReportsMatchFollowerSize) and does not compile once view-integ.h has declared Integration::View, which
// view-data-impl.h includes.
#include <dali-ui-foundation/integration-api/label-impl.h>

#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/internal/layouts/grid-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/measure-mode.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace DALI_NAMESPACE
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
                                    float rowSpacing, float colSpacing, float scale,
                                    const Internal::ViewDataImpl& viewData)
{
  // A grid that is not mode-aware (a subclass, a view with a layout callback) measures every
  // child with the custom-parent modes of its view, which is also what the child's own
  // derivation gives (ViewDataImpl::DeriveChildModesAsCustomParent).
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

    float                        childScale            = childImpl.GetEffectiveScale();
    Insets                       margin                = childImpl.GetMargin();
    float                        marginW               = (margin.start + margin.end) * childScale;
    float                        marginH               = (margin.top + margin.bottom) * childScale;
    float                        childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
    float                        childHeightConstraint = std::max(0.0f, availableHeight - marginH);
    const Internal::MeasureModes modes                 = viewData.DeriveChildModesAsCustomParent(childImpl.GetRequestedWidth(), childImpl.GetRequestedHeight());
    MeasuredSize                 childSize             = Internal::ViewDataImpl::Get(childImpl).Measure(childWidthConstraint, childHeightConstraint, modes.width, modes.height, Internal::MeasureModeCheck::MATCHES_DERIVATION);

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

    float                        childScale            = childImpl.GetEffectiveScale();
    Insets                       margin                = childImpl.GetMargin();
    float                        marginW               = (margin.start + margin.end) * childScale;
    float                        marginH               = (margin.top + margin.bottom) * childScale;
    float                        childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
    float                        childHeightConstraint = std::max(0.0f, availableHeight - marginH);
    const Internal::MeasureModes modes                 = viewData.DeriveChildModesAsCustomParent(childImpl.GetRequestedWidth(), childImpl.GetRequestedHeight());
    MeasuredSize                 childSize             = Internal::ViewDataImpl::Get(childImpl).Measure(childWidthConstraint, childHeightConstraint, modes.width, modes.height, Internal::MeasureModeCheck::MATCHES_DERIVATION);

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

// One axis of ApplyGridDefinitions, with the same float operations in the same order (the two
// axes of ApplyGridDefinitions never read each other's values, so resolving them separately
// gives bit-identical tracks and totals).
void ResolveGridTracks(std::vector<float>& sizes, const Dali::Vector<GridLength>& defs, float available, float spacing,
                       uint32_t count, float& totalOut, float scale)
{
  float totalAbsolute = 0.0f;
  float totalStar     = 0.0f;

  for(uint32_t i = 0; i < count; ++i)
  {
    if(i < defs.Size())
    {
      const auto& def = defs[i];
      if(def.GetType() == GridLengthType::ABSOLUTE)
      {
        sizes[i] = def.GetValue() * scale;
        totalAbsolute += sizes[i];
      }
      else if(def.GetType() == GridLengthType::STAR)
      {
        totalStar += def.GetValue();
      }
      else
      {
        totalAbsolute += sizes[i];
      }
    }
    else
    {
      totalStar += 1.0f;
    }
  }

  float totalSpacing = spacing * (count > 0 ? count - 1 : 0);
  float remaining    = std::max(0.0f, available - totalAbsolute - totalSpacing);

  for(uint32_t i = 0; i < count; ++i)
  {
    if(i < defs.Size() && defs[i].GetType() == GridLengthType::STAR)
    {
      float starValue = defs[i].GetValue();
      sizes[i]        = (totalStar > 0) ? (starValue / totalStar) * remaining : 0.0f;
    }
    else if(i >= defs.Size())
    {
      float share = (totalStar > 0) ? (1.0f / totalStar) * remaining : 0.0f;
      sizes[i]    = std::max(sizes[i], share);
    }
  }

  totalOut = std::accumulate(sizes.begin(), sizes.end(), 0.0f) + totalSpacing;
}

// The WRAP_CONTENT extent the star tracks of one axis share in the measure pass (the expression
// GridLayoutManager::Measure uses for a WRAP_CONTENT axis): the non-star tracks plus spacing,
// at least the grid's own minimum content extent.
float GridWrapStarAvailable(const std::vector<float>& sizes, const Dali::Vector<GridLength>& defs, float spacing,
                            uint32_t count, float minimumContent)
{
  float nonStar = 0.0f;
  for(uint32_t i = 0; i < count; ++i)
  {
    if(i >= defs.Size() || defs[i].GetType() != GridLengthType::STAR)
    {
      nonStar += sizes[i];
    }
  }
  nonStar += spacing * (count > 0 ? count - 1 : 0);
  return std::max(nonStar, minimumContent);
}

using Internal::MeasureMode;
using Internal::MeasureModeCheck;

// The measure cache's own tolerance (ViewDataImpl's FloatEqual).
bool GridFloatEqual(float a, float b)
{
  return std::abs(a - b) < 0.001f;
}

enum class GridTrackKind
{
  ABSOLUTE, ///< Every spanned track is ABSOLUTE: the cell is determined before any child is measured
  STAR,     ///< No spanned track is AUTO-like and at least one is STAR
  AUTO      ///< At least one spanned track is AUTO-like (sized from the children)
};

// The same classification as ViewDataImpl::DeriveMeasureModes uses (file-local copy).
GridTrackKind ClassifyGridSpan(const Dali::Vector<GridLength>& defs, uint32_t start, uint32_t span)
{
  bool allAbsolute = true;
  bool anyAutoLike = false;
  bool anyStar     = false;
  for(uint32_t i = start; i < start + span; ++i)
  {
    const bool defined = i < defs.Size();
    if(IsAutoLikeTrack(defs, i))
    {
      anyAutoLike = true;
    }
    if(!defined || defs[i].GetType() != GridLengthType::ABSOLUTE)
    {
      allAbsolute = false;
    }
    if(defined && defs[i].GetType() == GridLengthType::STAR)
    {
      anyStar = true;
    }
  }
  if(allAbsolute && span > 0u)
  {
    return GridTrackKind::ABSOLUTE;
  }
  if(!anyAutoLike && anyStar)
  {
    return GridTrackKind::STAR;
  }
  return GridTrackKind::AUTO;
}

// Whether a MATCH_PARENT width measured AT_MOST reports exactly the minimum size whatever the
// constraint (ViewDataImpl::GetMatchFollowerSize()): the LayoutManager dispatch substitutes it,
// the default measure (a plain View, a Layout without a manager) and Label report it. A measure
// callback outranks all of them and may return anything.
bool ReportsMatchFollowerSize(ViewImpl& childImpl)
{
  const Internal::ViewDataImpl& childData = Internal::ViewDataImpl::Get(childImpl);
  if(childData.HasMeasureCallback())
  {
    return false;
  }
  if(childData.GetLayoutManager() != nullptr)
  {
    return true;
  }
  const std::type_info& type = typeid(childImpl);
  return type == typeid(ViewImpl) || type == typeid(Integration::LayoutImpl) || type == typeid(Integration::LabelImpl);
}

/**
 * One child of a mode-aware grid pass.
 */
struct GridChildEntry
{
  ViewImpl*       impl{nullptr};
  uint32_t        row{0u};
  uint32_t        col{0u};
  uint32_t        rowSpan{1u};
  uint32_t        colSpan{1u};
  float           marginW{0.0f};
  float           marginH{0.0f};
  GridTrackKind   colKind{GridTrackKind::AUTO};
  GridTrackKind   rowKind{GridTrackKind::AUTO};
  LayoutAlignment hAlign{LayoutAlignment::FILL};
  LayoutAlignment vAlign{LayoutAlignment::FILL};
  bool            matchW{false};
  bool            matchH{false};
  bool            wrapW{false};
  bool            wrapH{false};
  float           height{0.0f}; ///< The height number of every call (item 2)
  MeasureMode     heightMode{MeasureMode::AT_MOST};
  bool            deferred{false}; ///< MATCH_PARENT width in an AUTO column: the final call is phase B
  MeasuredSize    sizeA;           ///< Phase-A result (width: the follower size when synthesized)
  MeasuredSize    finalSize;
};

// A MATCH_PARENT axis measured at anything but EXACTLY is an intermediate call (a probe, or a
// cell the measure pass cannot know yet); every other mode a grid passes is its derivation.
MeasureModeCheck GridCallCheck(const GridChildEntry& entry, MeasureMode widthMode, MeasureMode heightMode)
{
  return ((entry.matchW && widthMode != MeasureMode::EXACTLY) || (entry.matchH && heightMode != MeasureMode::EXACTLY))
           ? MeasureModeCheck::NONE
           : MeasureModeCheck::MATCHES_DERIVATION;
}

/**
 * The mode-aware replacement of MeasureGridChildrenAndFillAuto + ApplyGridDefinitions, used by
 * both Measure and Arrange of a grid whose producer is exactly GridLayoutManager.
 *
 * Columns first:
 *  - ABSOLUTE tracks are seeded up front (they never take a content floor, so seeding them
 *    before the AUTO pass changes nothing).
 *  - Phase A sizes the AUTO columns exactly as before (today's numbers, order and floors) from
 *    the children in AUTO columns and, while the grid's width is not determined, from the children in
 *    STAR columns too. A MATCH_PARENT child in an AUTO column contributes the size it reports as
 *    a follower -- synthesized without a call when its producer follows the contract exactly
 *    (bit-identical, and no second key to alternate on in Arrange), a probe otherwise -- and is
 *    measured exactly in phase B.
 *  - The columns are resolved, then phase B measures every child whose cell width is now determined
 *    -- ABSOLUTE columns, STAR columns of a grid with a determined width, and the deferred
 *    MATCH_PARENT children -- at that cell: MATCH_PARENT and FILL WRAP_CONTENT EXACTLY,
 *    otherwise AT_MOST.
 * Rows: every call's height is the cell for an ABSOLUTE row (EXACTLY for MATCH_PARENT / FILL),
 * otherwise today's budget AT_MOST; the AUTO row floors and spans come from each child's final
 * size, then the rows are resolved.
 *
 * @param[in] arrangePhase True in Arrange: the star tracks share the arranged extent
 */
void MeasureGridChildrenPhased(std::vector<View>& children, float availableWidth, float availableHeight,
                               uint32_t rowCount, uint32_t colCount,
                               const Dali::Vector<GridLength>& rowDefs, const Dali::Vector<GridLength>& colDefs,
                               std::vector<float>& rowHeights, std::vector<float>& colWidths,
                               float rowSpacing, float colSpacing, float scale, bool widthDetermined, bool heightDetermined,
                               bool arrangePhase, ViewImpl& view, float& totalWidthOut, float& totalHeightOut)
{
  // 1. ABSOLUTE tracks up front.
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

  // 2. Per child: its cell, kinds, and the height of every call.
  std::vector<GridChildEntry> entries;
  entries.reserve(children.size());
  for(auto& childView : children)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    GridChildEntry entry;
    entry.impl    = &childImpl;
    entry.row     = std::min(GetChildRow(childImpl), rowCount - 1);
    entry.col     = std::min(GetChildColumn(childImpl), colCount - 1);
    entry.rowSpan = std::min(GetChildRowSpan(childImpl), rowCount - entry.row);
    entry.colSpan = std::min(GetChildColumnSpan(childImpl), colCount - entry.col);

    float  childScale = childImpl.GetEffectiveScale();
    Insets margin     = childImpl.GetMargin();
    entry.marginW     = (margin.start + margin.end) * childScale;
    entry.marginH     = (margin.top + margin.bottom) * childScale;
    entry.colKind     = ClassifyGridSpan(colDefs, entry.col, entry.colSpan);
    entry.rowKind     = ClassifyGridSpan(rowDefs, entry.row, entry.rowSpan);
    entry.hAlign      = GetChildHorizontalAlignment(childImpl);
    entry.vAlign      = GetChildVerticalAlignment(childImpl);
    entry.matchW      = childImpl.GetRequestedWidth() == MATCH_PARENT;
    entry.matchH      = childImpl.GetRequestedHeight() == MATCH_PARENT;
    entry.wrapW       = childImpl.GetRequestedWidth() == WRAP_CONTENT;
    entry.wrapH       = childImpl.GetRequestedHeight() == WRAP_CONTENT;

    if(entry.rowKind == GridTrackKind::ABSOLUTE)
    {
      float cell = rowSpacing * static_cast<float>(entry.rowSpan - 1);
      for(uint32_t i = 0; i < entry.rowSpan; ++i)
      {
        cell += rowHeights[entry.row + i];
      }
      entry.height     = std::max(0.0f, cell - entry.marginH);
      entry.heightMode = (entry.matchH || (entry.wrapH && entry.vAlign == LayoutAlignment::FILL)) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
    }
    else
    {
      entry.height     = std::max(0.0f, availableHeight - entry.marginH);
      entry.heightMode = MeasureMode::AT_MOST;
    }
    entries.push_back(entry);
  }

  // 3. Phase A: AUTO column sizing, today's numbers and order. ownColWidths follows colWidths over
  // the children that size themselves (deferred MATCH_PARENT children excluded): resolved like the
  // columns, it gives the width a deferred child's cell has of its own.
  std::vector<float> ownColWidths = colWidths;
  bool               anyDeferred  = false;
  for(auto& entry : entries)
  {
    const bool inAutoColumn       = (entry.colKind == GridTrackKind::AUTO);
    const bool inUndeterminedStar = (entry.colKind == GridTrackKind::STAR && !widthDetermined);
    if(!inAutoColumn && !inUndeterminedStar)
    {
      continue;
    }

    const float budgetWidth = std::max(0.0f, availableWidth - entry.marginW);
    if(inAutoColumn && entry.matchW)
    {
      entry.deferred = true;
      anyDeferred    = true;
      if(ReportsMatchFollowerSize(*entry.impl))
      {
        entry.sizeA = Internal::ViewDataImpl::Get(*entry.impl).GetMatchFollowerSize();
      }
      else
      {
        entry.sizeA = Internal::ViewDataImpl::Get(*entry.impl).Measure(budgetWidth, entry.height, MeasureMode::AT_MOST, entry.heightMode, MeasureModeCheck::NONE);
      }
    }
    else if(arrangePhase && inUndeterminedStar &&
            Internal::ViewDataImpl::Get(*entry.impl).CanSkipArrangeRemeasure(budgetWidth, entry.height, MeasureMode::AT_MOST, entry.heightMode))
    {
      // An arrange-phase repeat of a WRAP_CONTENT or explicitly sized child in a STAR column of a
      // grid whose width is not determined: such a child floors no column (STAR tracks take no
      // floor, and a STAR span holds no AUTO-like track to grow), and its row contribution is the
      // same whether it holds this budget measurement or the cell measurement the previous arrange
      // made (that one is AT_MOST the very row it is laid out in, and not lower than its budget
      // height). Reusing a valid, clean measurement keeps the child from being re-keyed to the
      // budget -- which made the cell re-measure below run again on every arrange.
      entry.sizeA     = entry.impl->GetMeasuredSize();
      entry.finalSize = entry.sizeA;
    }
    else
    {
      entry.sizeA     = Internal::ViewDataImpl::Get(*entry.impl).Measure(budgetWidth, entry.height, MeasureMode::AT_MOST, entry.heightMode, GridCallCheck(entry, MeasureMode::AT_MOST, entry.heightMode));
      entry.finalSize = entry.sizeA;
    }

    if(entry.colSpan == 1 && IsAutoLikeTrack(colDefs, entry.col))
    {
      colWidths[entry.col] = std::max(colWidths[entry.col], entry.sizeA.width + entry.marginW);
      if(!entry.deferred)
      {
        ownColWidths[entry.col] = std::max(ownColWidths[entry.col], entry.sizeA.width + entry.marginW);
      }
    }
  }

  // Spanning children: grow the spanned AUTO columns by the deficit (the column half of the
  // span pass of MeasureGridChildrenAndFillAuto, from the phase-A sizes).
  for(const auto& entry : entries)
  {
    if(entry.colSpan <= 1 || !(entry.colKind == GridTrackKind::AUTO || (entry.colKind == GridTrackKind::STAR && !widthDetermined)))
    {
      continue;
    }
    uint32_t spannedAutoCount = 0u;
    float    currentSpanSize  = colSpacing * static_cast<float>(entry.colSpan - 1);
    for(uint32_t i = 0; i < entry.colSpan; ++i)
    {
      currentSpanSize += colWidths[entry.col + i];
      if(IsAutoLikeTrack(colDefs, entry.col + i))
      {
        ++spannedAutoCount;
      }
    }
    if(spannedAutoCount > 0u)
    {
      float deficit = std::max(0.0f, (entry.sizeA.width + entry.marginW) - currentSpanSize);
      if(deficit > 0.0f)
      {
        float share = deficit / static_cast<float>(spannedAutoCount);
        for(uint32_t i = 0; i < entry.colSpan; ++i)
        {
          if(IsAutoLikeTrack(colDefs, entry.col + i))
          {
            colWidths[entry.col + i] += share;
          }
        }
      }
      if(!entry.deferred)
      {
        // The same deficit over the own widths.
        float ownSpanSize = colSpacing * static_cast<float>(entry.colSpan - 1);
        for(uint32_t i = 0; i < entry.colSpan; ++i)
        {
          ownSpanSize += ownColWidths[entry.col + i];
        }
        const float ownDeficit = std::max(0.0f, (entry.sizeA.width + entry.marginW) - ownSpanSize);
        if(ownDeficit > 0.0f)
        {
          const float ownShare = ownDeficit / static_cast<float>(spannedAutoCount);
          for(uint32_t i = 0; i < entry.colSpan; ++i)
          {
            if(IsAutoLikeTrack(colDefs, entry.col + i))
            {
              ownColWidths[entry.col + i] += ownShare;
            }
          }
        }
      }
    }
  }

  // 4. Resolve the columns. The own widths are resolved the same way, so a STAR track or one without a
  // definition has its share of the grid's width as its own -- while that width is not determined,
  // its share of the non-STAR tracks' own widths or of the grid's own minimum.
  const Insets padding           = view.GetPadding();
  float        starAvailableWide = availableWidth;
  float        ownStarAvailable  = availableWidth;
  if(!arrangePhase && !widthDetermined)
  {
    const float pw         = (padding.start + padding.end) * scale;
    const float minContent = std::max(0.0f, view.GetMinimumWidth() * scale - pw);
    starAvailableWide      = GridWrapStarAvailable(colWidths, colDefs, colSpacing, colCount, minContent);
    if(anyDeferred)
    {
      ownStarAvailable = GridWrapStarAvailable(ownColWidths, colDefs, colSpacing, colCount, minContent);
    }
  }
  ResolveGridTracks(colWidths, colDefs, starAvailableWide, colSpacing, colCount, totalWidthOut, scale);
  if(anyDeferred)
  {
    float ownTotalWidth = 0.0f;
    ResolveGridTracks(ownColWidths, colDefs, ownStarAvailable, colSpacing, colCount, ownTotalWidth, scale);
  }

  // 5. Phase B: every child whose cell width is now determined, at that cell.
  for(auto& entry : entries)
  {
    const bool cellDetermined = (entry.colKind == GridTrackKind::ABSOLUTE) || (entry.colKind == GridTrackKind::STAR && widthDetermined);
    if(!cellDetermined && !entry.deferred)
    {
      continue;
    }
    float cell = colSpacing * static_cast<float>(entry.colSpan - 1);
    for(uint32_t i = 0; i < entry.colSpan; ++i)
    {
      cell += colWidths[entry.col + i];
    }
    // The cell a deferred entry's columns have of their own (ownColWidths).
    bool ownCell = true;
    if(entry.deferred)
    {
      float own = colSpacing * static_cast<float>(entry.colSpan - 1);
      for(uint32_t i = 0; i < entry.colSpan; ++i)
      {
        own += ownColWidths[entry.col + i];
      }
      ownCell = Internal::SnapEmptySlot(own) > 0.0f;
    }
    const float width = entry.deferred ? (ownCell ? Internal::SnapEmptySlot(cell - entry.marginW) : 0.0f) : std::max(0.0f, cell - entry.marginW);
    // An AUTO column only MATCH_PARENT children span has no width of its own: such a child keeps
    // today's budget measurement (its phase-A probe, or this call when phase A synthesized its
    // follower size, whose height was never measured), so its rows keep the height they had before
    // modes existed.
    if(entry.deferred && width <= 0.0f)
    {
      entry.finalSize = ReportsMatchFollowerSize(*entry.impl)
                          ? Internal::ViewDataImpl::Get(*entry.impl).Measure(std::max(0.0f, availableWidth - entry.marginW), entry.height, MeasureMode::AT_MOST, entry.heightMode, MeasureModeCheck::NONE)
                          : entry.sizeA;
      continue;
    }
    const MeasureMode widthMode = (entry.matchW || (entry.wrapW && entry.hAlign == LayoutAlignment::FILL && cellDetermined)) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
    entry.finalSize             = Internal::ViewDataImpl::Get(*entry.impl).Measure(width, entry.height, widthMode, entry.heightMode, GridCallCheck(entry, widthMode, entry.heightMode));
  }

  // 6. Rows from the final sizes: span-1 AUTO floors, then the spanning deficits.
  for(const auto& entry : entries)
  {
    if(entry.rowSpan == 1 && IsAutoLikeTrack(rowDefs, entry.row))
    {
      rowHeights[entry.row] = std::max(rowHeights[entry.row], entry.finalSize.height + entry.marginH);
    }
  }
  for(const auto& entry : entries)
  {
    if(entry.rowSpan <= 1)
    {
      continue;
    }
    uint32_t spannedAutoCount = 0u;
    float    currentSpanSize  = rowSpacing * static_cast<float>(entry.rowSpan - 1);
    for(uint32_t i = 0; i < entry.rowSpan; ++i)
    {
      currentSpanSize += rowHeights[entry.row + i];
      if(IsAutoLikeTrack(rowDefs, entry.row + i))
      {
        ++spannedAutoCount;
      }
    }
    if(spannedAutoCount > 0u)
    {
      float deficit = std::max(0.0f, (entry.finalSize.height + entry.marginH) - currentSpanSize);
      if(deficit > 0.0f)
      {
        float share = deficit / static_cast<float>(spannedAutoCount);
        for(uint32_t i = 0; i < entry.rowSpan; ++i)
        {
          if(IsAutoLikeTrack(rowDefs, entry.row + i))
          {
            rowHeights[entry.row + i] += share;
          }
        }
      }
    }
  }

  float starAvailableHigh = availableHeight;
  if(!arrangePhase && !heightDetermined)
  {
    const float ph         = (padding.top + padding.bottom) * scale;
    const float minContent = std::max(0.0f, view.GetMinimumHeight() * scale - ph);
    starAvailableHigh      = GridWrapStarAvailable(rowHeights, rowDefs, rowSpacing, rowCount, minContent);
  }
  ResolveGridTracks(rowHeights, rowDefs, starAvailableHigh, rowSpacing, rowCount, totalHeightOut, scale);
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

// @param[in] owner The arranging View, threaded through so the final MATCH_PARENT
//                  re-measure below can be attributed to it. nullptr makes the scope inert.
// @param[in] modeAware       True when the grid's producer is exactly GridLayoutManager
// @param[in] widthDetermined The grid's width is determined (its STAR columns are determined cells)
void ArrangeGridChildrenToCells(ViewImpl* owner, std::vector<View>& children, const std::vector<float>& rowPositions,
                                const std::vector<float>& colPositions, uint32_t rowCount, uint32_t colCount,
                                float rowSpacing, float colSpacing, bool modeAware, bool widthDetermined,
                                const Dali::Vector<GridLength>& rowDefs, const Dali::Vector<GridLength>& colDefs,
                                const Internal::ViewDataImpl& viewData)
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

    float  childScale = childImpl.GetEffectiveScale();
    Insets margin     = childImpl.GetMargin();
    childBounds.x += margin.start * childScale;
    childBounds.y += margin.top * childScale;
    childBounds.width  = std::max(0.0f, childBounds.width - (margin.start + margin.end) * childScale);
    childBounds.height = std::max(0.0f, childBounds.height - (margin.top + margin.bottom) * childScale);

    float                   cellWidth     = childBounds.width;
    float                   cellHeight    = childBounds.height;
    MeasuredSize            childMeasured = childImpl.GetMeasuredSize();
    LayoutAlignment         hAlign        = GetChildHorizontalAlignment(childImpl);
    LayoutAlignment         vAlign        = GetChildVerticalAlignment(childImpl);
    const float             requestedW    = childImpl.GetRequestedWidth();
    const float             requestedH    = childImpl.GetRequestedHeight();
    const bool              isMatch       = (requestedW == MATCH_PARENT || requestedH == MATCH_PARENT);
    Internal::ViewDataImpl& childData     = Internal::ViewDataImpl::Get(childImpl);

    if(modeAware)
    {
      // MATCH_PARENT is assigned its cell; a FILL WRAP_CONTENT axis only when its cell is
      // determined before the children are measured (ABSOLUTE tracks, STAR columns of a grid with
      // a determined width).
      const GridTrackKind colKind    = ClassifyGridSpan(colDefs, col, colSpan);
      const bool          colCell    = (colKind == GridTrackKind::ABSOLUTE) || (widthDetermined && colKind == GridTrackKind::STAR);
      const bool          rowCell    = (ClassifyGridSpan(rowDefs, row, rowSpan) == GridTrackKind::ABSOLUTE);
      const MeasureMode   widthMode  = (requestedW == MATCH_PARENT || (requestedW == WRAP_CONTENT && hAlign == LayoutAlignment::FILL && colCell)) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
      const MeasureMode   heightMode = (requestedH == MATCH_PARENT || (requestedH == WRAP_CONTENT && vAlign == LayoutAlignment::FILL && rowCell)) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;

      // A WRAP_CONTENT child wider (or taller) than its cell -- measured at the grid's budget
      // because its cell was not determined then (a STAR column of a grid whose width was not
      // determined, a STAR row), or a grid laid out smaller than it measured -- is re-measured at
      // the cell, so its measured size is the size it is laid out at. The trigger is explicit
      // because CanSkipArrangeRemeasure() does not compare a non-assigned WRAP_CONTENT axis. An
      // explicit size keeps its request whatever the constraint and is clipped to the cell, as
      // before. A MATCH_PARENT child is re-measured at its cell unless it already holds a
      // measurement for exactly this cell (the phase-B call made at the same resolved tracks).
      const bool wideW = (requestedW == WRAP_CONTENT) && childMeasured.width > cellWidth && !GridFloatEqual(childMeasured.width, cellWidth);
      const bool tallH = (requestedH == WRAP_CONTENT) && childMeasured.height > cellHeight && !GridFloatEqual(childMeasured.height, cellHeight);
      if(wideW || tallH || (isMatch && !childData.CanSkipArrangeRemeasure(cellWidth, cellHeight, widthMode, heightMode)))
      {
        Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
        childMeasured = childData.Measure(cellWidth, cellHeight, widthMode, heightMode, MeasureModeCheck::MATCHES_DERIVATION);
      }
    }

    float childWidth  = childMeasured.width;
    float childHeight = childMeasured.height;

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

    if(!modeAware && isMatch)
    {
      // A grid that is not mode-aware re-measures a MATCH_PARENT child at its placement with the
      // custom-parent modes of its view, never skipped.
      const Internal::MeasureModes                         legacy = viewData.DeriveChildModesAsCustomParent(requestedW, requestedH);
      Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
      childData.Measure(childBounds.width, childBounds.height, legacy.width, legacy.height, MeasureModeCheck::MATCHES_DERIVATION);
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

  // Row definitions are read by both Measure() and Arrange(), and neither cache key
  // can see them, so the owner has to be told. Unguarded, unlike the scalar setters
  // below: GridLength has no equality operator, and comparing element-wise to save an
  // invalidation would cost more than the invalidation on any realistic grid.
  InvalidateOwnerMeasure();
}

const Dali::Vector<GridLength>& GridLayoutManager::GetRowDefinitions() const
{
  return GetImplAs<Impl>()->mRowDefinitions;
}

void GridLayoutManager::SetColumnDefinitions(const Dali::Vector<GridLength>& columns)
{
  GetImplAs<Impl>()->mColumnDefinitions = columns;

  // See SetRowDefinitions: same reasoning, same unguarded invalidation.
  InvalidateOwnerMeasure();
}

const Dali::Vector<GridLength>& GridLayoutManager::GetColumnDefinitions() const
{
  return GetImplAs<Impl>()->mColumnDefinitions;
}

void GridLayoutManager::SetRowSpacing(float spacing)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mRowSpacing == spacing)
  {
    return;
  }
  impl->mRowSpacing = spacing;
  InvalidateOwnerMeasure();
}

float GridLayoutManager::GetRowSpacing() const
{
  return GetImplAs<Impl>()->mRowSpacing;
}

void GridLayoutManager::SetColumnSpacing(float spacing)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mColumnSpacing == spacing)
  {
    return;
  }
  impl->mColumnSpacing = spacing;
  InvalidateOwnerMeasure();
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
  const uint32_t    childCount = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  uint32_t           rowCount = std::max(1u, static_cast<uint32_t>(impl->mRowDefinitions.Size()));
  uint32_t           colCount = std::max(1u, static_cast<uint32_t>(impl->mColumnDefinitions.Size()));
  std::vector<float> rowHeights(rowCount, 0.0f);
  std::vector<float> colWidths(colCount, 0.0f);

  // Measure modes: only a view whose producer is exactly this manager is mode-aware; anything
  // else (a subclass, a layout callback) measures every child below with the custom-parent modes
  // of its view, in one step.
  const Internal::ViewDataImpl& viewData = Internal::ViewDataImpl::Get(*view);
  if(viewData.GetMeasureModeParent() == Internal::MeasureModeParent::GRID)
  {
    float phasedWidth  = 0.0f;
    float phasedHeight = 0.0f;
    MeasureGridChildrenPhased(children, availableWidth, availableHeight, rowCount, colCount, impl->mRowDefinitions,
                              impl->mColumnDefinitions, rowHeights, colWidths, visRowSpacing, visColSpacing, s,
                              viewData.IsWidthDetermined(), viewData.IsHeightDetermined(), false, *view, phasedWidth, phasedHeight);
    return MeasuredSize(phasedWidth, phasedHeight);
  }

  MeasureGridChildrenAndFillAuto(children, availableWidth, availableHeight, rowCount,
                                 colCount, impl->mRowDefinitions, impl->mColumnDefinitions, rowHeights, colWidths,
                                 visRowSpacing, visColSpacing, s, viewData);

  Insets parentPadding   = view->GetPadding();
  float  requestedWidth  = view->GetRequestedWidth();
  float  requestedHeight = view->GetRequestedHeight();

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
    float pw              = (parentPadding.start + parentPadding.end) * s;
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
    float ph               = (parentPadding.top + parentPadding.bottom) * s;
    float minHeightContent = std::max(0.0f, view->GetMinimumHeight() * s - ph);
    starAvailableHeight    = std::max(nonStarHeight, minHeightContent);
  }

  float totalWidth  = 0.0f;
  float totalHeight = 0.0f;
  ApplyGridDefinitions(rowHeights, colWidths, impl->mRowDefinitions, impl->mColumnDefinitions, starAvailableWidth, starAvailableHeight,
                       visRowSpacing, visColSpacing, rowCount, colCount, totalWidth, totalHeight, s);

  return MeasuredSize(totalWidth, totalHeight);
}

void GridLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return;
  }

  auto* impl = GetImplAs<Impl>();

  float s               = view->GetEffectiveScale();
  float visRowSpacing   = impl->mRowSpacing * s;
  float visColSpacing   = impl->mColumnSpacing * s;
  float availableWidth  = bounds.width;
  float availableHeight = bounds.height;

  const uint32_t    childCount = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  uint32_t           rowCount = std::max(1u, static_cast<uint32_t>(impl->mRowDefinitions.Size()));
  uint32_t           colCount = std::max(1u, static_cast<uint32_t>(impl->mColumnDefinitions.Size()));
  std::vector<float> rowHeights(rowCount, 0.0f);
  std::vector<float> colWidths(colCount, 0.0f);

  // Re-measure children to get fresh auto row/column sizes using actual arrange bounds.
  // The helper only measures (it never arranges), so one scope around the whole call has
  // exactly the narrow extent required and needs no threaded owner parameter. A mode-aware grid
  // runs the same phased helper as its measure (with the stored extent knowledge of its latest
  // measurement), so a grid arranged at its measured extent repeats those calls as cache hits.
  const Internal::ViewDataImpl& viewData        = Internal::ViewDataImpl::Get(*view);
  const bool                    modeAware       = (viewData.GetMeasureModeParent() == Internal::MeasureModeParent::GRID);
  const bool                    widthDetermined = modeAware && viewData.IsWidthDetermined();
  float                         totalWidth      = 0.0f;
  float                         totalHeight     = 0.0f;
  if(modeAware)
  {
    Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(view);
    MeasureGridChildrenPhased(children, availableWidth, availableHeight, rowCount, colCount, impl->mRowDefinitions,
                              impl->mColumnDefinitions, rowHeights, colWidths, visRowSpacing, visColSpacing, s,
                              widthDetermined, viewData.IsHeightDetermined(), true, *view, totalWidth, totalHeight);
  }
  else
  {
    {
      Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(view);
      MeasureGridChildrenAndFillAuto(children, availableWidth, availableHeight, rowCount,
                                     colCount, impl->mRowDefinitions, impl->mColumnDefinitions, rowHeights, colWidths,
                                     visRowSpacing, visColSpacing, s, viewData);
    }

    ApplyGridDefinitions(rowHeights, colWidths, impl->mRowDefinitions, impl->mColumnDefinitions, availableWidth, availableHeight,
                         visRowSpacing, visColSpacing, rowCount, colCount, totalWidth, totalHeight, s);
  }

  std::vector<float> rowPositions;
  std::vector<float> colPositions;
  ComputeGridPositions(rowHeights, colWidths, bounds, visRowSpacing, visColSpacing, rowCount, colCount, rowPositions,
                       colPositions);

  ArrangeGridChildrenToCells(view, children, rowPositions, colPositions, rowCount, colCount, visRowSpacing, visColSpacing,
                             modeAware, widthDetermined, impl->mRowDefinitions, impl->mColumnDefinitions, viewData);
}

} // namespace Ui
} //namespace DALI_NAMESPACE
