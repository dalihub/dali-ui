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
#include <dali-ui-foundation/public-api/layouts/flex-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/flex-layout-params-impl.h>
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

float GetFlexGrow(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetFlexGrow() : 0.0f;
}

float GetFlexShrink(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetFlexShrink() : 1.0f;
}

FlexAlign GetAlignSelf(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetAlignSelf() : FlexAlign::AUTO;
}

float GetFlexBasis(ViewImpl& childImpl)
{
  auto* params = Internal::FlexLayoutParamsImpl::Get(childImpl);
  return params ? params->GetFlexBasis() : WRAP_CONTENT;
}

bool IsChildStandalone(ViewImpl& childImpl)
{
  return childImpl.GetLayoutMode() == LayoutMode::STANDALONE;
}

struct FlexLine
{
  std::vector<uint32_t> childIndices;
  float                 mainSize{0.0f};
  float                 crossSize{0.0f};
  float                 totalFlexGrow{0.0f};
  float                 totalFlexShrink{0.0f};
  bool                  hasAssigned{false}; ///< Arrange of a mode-aware flex: the line holds a child whose kept size its flex assigned
};

using Internal::MeasureMode;
using Internal::MeasureModeCheck;

// The measure cache's own tolerance (ViewDataImpl's FloatEqual).
bool FlexFloatEqual(float a, float b)
{
  return std::abs(a - b) < 0.001f;
}

// What a mode-aware single-line flex remembers about one child between its measure and its arrange
// (visual units, without margins). The child pointer guards against a changed child list.
struct FlexAssignedMain
{
  const ViewImpl* child{nullptr};
  float           extent{-1.0f};  ///< The main slot size the flex assigned the child; negative = not assigned
  float           natural{-1.0f}; ///< The child's line input before grow/shrink (its at-most main report or basis)
  bool            kept{false};    ///< This arrange keeps the assigned size (the line's grow/shrink skip the child)
};

bool IsKept(const std::vector<FlexAssignedMain>* entries, uint32_t index)
{
  return entries != nullptr && (*entries)[index].kept;
}

/**
 * The measure-mode facts of one flex pass, read once from the flex's own view.
 *
 * modeAware is true only for a single-line flex whose producer is exactly this manager. An exact
 * manager with wrapping lines (its Arrange re-breaks the lines from the measured sizes) measures
 * every child AT_MOST, as before; any other view running this manager (a subclass, a layout
 * callback) passes its children the custom-parent modes of its view in one step
 * (legacyDerivation).
 */
struct FlexModeContext
{
  bool                          modeAware{false};
  bool                          legacyDerivation{false};
  bool                          mainDetermined{true};
  bool                          crossDetermined{true};
  const Internal::ViewDataImpl* data{nullptr}; ///< The flex's own view data
};

FlexModeContext MakeFlexModeContext(const Internal::ViewDataImpl& viewData, bool isMainAxisHorizontal)
{
  FlexModeContext                   context;
  const Internal::MeasureModeParent kind = viewData.GetMeasureModeParent();
  context.data                           = &viewData;
  context.modeAware                      = (kind == Internal::MeasureModeParent::FLEX);
  context.legacyDerivation               = (kind == Internal::MeasureModeParent::NONE);
  if(context.modeAware)
  {
    context.mainDetermined  = isMainAxisHorizontal ? viewData.IsWidthDetermined() : viewData.IsHeightDetermined();
    context.crossDetermined = isMainAxisHorizontal ? viewData.IsHeightDetermined() : viewData.IsWidthDetermined();
  }
  return context;
}

// The modes a flex that is not mode-aware passes a child (what the child's own derivation gives):
// the custom-parent rule of its view, or AT_MOST on every axis for a wrapping flex.
Internal::MeasureModes LegacyFlexChildModes(const FlexModeContext& context, ViewImpl& childImpl)
{
  return context.legacyDerivation ? context.data->DeriveChildModesAsCustomParent(childImpl.GetRequestedWidth(), childImpl.GetRequestedHeight())
                                  : Internal::MeasureModes{};
}

FlexAlign GetEffectiveAlign(ViewImpl& childImpl, FlexAlign alignItems)
{
  FlexAlign align = GetAlignSelf(childImpl);
  return (align == FlexAlign::AUTO) ? alignItems : align;
}

/**
 * One child of a mode-aware single-line flex measure pass.
 */
struct FlexMeasureRecord
{
  ViewImpl*    impl{nullptr};
  float        widthConstraint{0.0f}; ///< The step-1 numbers (today's)
  float        heightConstraint{0.0f};
  MeasureMode  crossMode{MeasureMode::AT_MOST};
  MeasuredSize size;    ///< The child's latest report in this pass
  MeasuredSize working; ///< The step-1 report with the basis override (the line's input)
  float        marginW{0.0f};
  float        marginH{0.0f};
  float        grow{0.0f};
  float        basis{WRAP_CONTENT};
  float        assignedMain{0.0f}; ///< The main extent the measure assigned (valid when assigned)
  bool         fixedMain{false};
  bool         matchMain{false};
  bool         matchCross{false};
  bool         stretchCross{false}; ///< WRAP_CONTENT cross axis with an effective STRETCH alignment
  bool         assigned{false};
};

// @param[in] entries nullptr for a flex that is not mode-aware; otherwise, per child, whether this
//                    arrange keeps the size its flex assigned (kept): the size the flex stored for
//                    this very main size, already in @p workingSizes -- the child reports it clamped
//                    by its own minimum/maximum, so its report is not read back. A kept child takes no
//                    basis override and no share of grow/shrink, which then distribute the residual
//                    among the others -- the same values the full distribution gives (the kept sizes
//                    are its result).
std::vector<FlexLine> BuildFlexLinesForArrange(std::vector<View>&                   children,
                                               std::vector<MeasuredSize>&           workingSizes,
                                               float                                availableMain,
                                               bool                                 isMainAxisHorizontal,
                                               FlexWrap                             wrap,
                                               const std::vector<FlexAssignedMain>* entries)
{
  std::vector<FlexLine> lines;
  FlexLine              currentLine;
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);
    if(IsChildStandalone(childImpl))
    {
      continue;
    }
    float  childScale = childImpl.GetEffectiveScale();
    Insets margin     = childImpl.GetMargin();

    const bool isKept = IsKept(entries, i);
    float      basis  = GetFlexBasis(childImpl);
    if(basis >= 0.0f && !isKept)
    {
      if(isMainAxisHorizontal)
      {
        workingSizes[i].width = basis * childScale;
      }
      else
      {
        workingSizes[i].height = basis * childScale;
      }
    }

    float visMarginMainH = (margin.top + margin.bottom) * childScale;
    float visMarginMainW = (margin.start + margin.end) * childScale;
    float childMainSize  = isMainAxisHorizontal ? workingSizes[i].width + visMarginMainW
                                                : workingSizes[i].height + visMarginMainH;
    bool  shouldWrap     = (wrap != FlexWrap::NO_WRAP) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);
    if(shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }
    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    if(isKept)
    {
      currentLine.hasAssigned = true;
    }
    else
    {
      currentLine.totalFlexGrow += GetFlexGrow(childImpl);
      currentLine.totalFlexShrink += GetFlexShrink(childImpl);
    }
    float childCrossSize  = isMainAxisHorizontal ? workingSizes[i].height + visMarginMainH
                                                 : workingSizes[i].width + visMarginMainW;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }
  if(!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }
  return lines;
}

// @param[in] entries See BuildFlexLinesForArrange; nullptr distributes over every child.
void ApplyFlexGrowShrink(FlexLine& line, std::vector<View>& children,
                         std::vector<MeasuredSize>& workingSizes, float availableMain,
                         bool isMainAxisHorizontal, const std::vector<FlexAssignedMain>* entries)
{
  float freeSpace = availableMain - line.mainSize;

  if(freeSpace > 0.0f && line.totalFlexGrow > 0.0f)
  {
    for(uint32_t idx : line.childIndices)
    {
      if(IsKept(entries, idx))
      {
        continue;
      }
      ViewImpl& childImpl = GetImpl(children[idx]);
      float     grow      = GetFlexGrow(childImpl);
      if(grow > 0.0f)
      {
        float extra = (grow / line.totalFlexGrow) * freeSpace;
        if(isMainAxisHorizontal)
        {
          workingSizes[idx].width += extra;
        }
        else
        {
          workingSizes[idx].height += extra;
        }
      }
    }
    line.mainSize = availableMain;
  }
  else if(freeSpace < 0.0f && line.totalFlexShrink > 0.0f)
  {
    float totalWeightedShrink = 0.0f;
    for(uint32_t idx : line.childIndices)
    {
      if(IsKept(entries, idx))
      {
        continue;
      }
      ViewImpl& childImpl     = GetImpl(children[idx]);
      float     childMainSize = isMainAxisHorizontal ? workingSizes[idx].width : workingSizes[idx].height;
      float     shrink        = GetFlexShrink(childImpl);
      totalWeightedShrink += shrink * childMainSize;
    }
    if(totalWeightedShrink > 0.0f)
    {
      float overflow = -freeSpace;
      for(uint32_t idx : line.childIndices)
      {
        if(IsKept(entries, idx))
        {
          continue;
        }
        ViewImpl& childImpl     = GetImpl(children[idx]);
        float     childMainSize = isMainAxisHorizontal ? workingSizes[idx].width : workingSizes[idx].height;
        float     shrink        = GetFlexShrink(childImpl);
        float     reduction     = (shrink * childMainSize / totalWeightedShrink) * overflow;
        if(isMainAxisHorizontal)
        {
          workingSizes[idx].width = std::max(0.0f, workingSizes[idx].width - reduction);
        }
        else
        {
          workingSizes[idx].height = std::max(0.0f, workingSizes[idx].height - reduction);
        }
      }
      line.mainSize = availableMain;
    }
  }

  // The kept sizes are the full distribution of this same line at this main size, so with them in
  // place the residual is (up to float rounding) zero and the line already fills the main
  // extent: snap that rounding, exactly as a distribution that ran here would have set it. A line
  // that genuinely does not fill (no grow/shrink distribution happened in the measure either, e.g.
  // a basis narrower than the content) keeps its own extent, so justification is unchanged.
  if(line.hasAssigned && FlexFloatEqual(line.mainSize, availableMain))
  {
    line.mainSize = availableMain;
  }
}

struct FlexJustifyOffsets
{
  float mainOffset{0.0f};
  float spacing{0.0f};
};

FlexJustifyOffsets GetFlexJustifyOffsets(float freeSpace, FlexJustify justify, size_t lineChildCount)
{
  FlexJustifyOffsets out;
  switch(justify)
  {
    case FlexJustify::FLEX_START:
      break;
    case FlexJustify::FLEX_END:
      out.mainOffset = freeSpace;
      break;
    case FlexJustify::CENTER:
      out.mainOffset = freeSpace / 2.0f;
      break;
    // On overflow (freeSpace < 0) the distributed modes must not produce
    // negative gaps that overlap children: fall back to start packing
    // (SPACE_BETWEEN) or centring the overflow (SPACE_AROUND/EVENLY).
    case FlexJustify::SPACE_BETWEEN:
      if(freeSpace > 0.0f && lineChildCount > 1)
      {
        out.spacing = freeSpace / (lineChildCount - 1);
      }
      break;
    case FlexJustify::SPACE_AROUND:
      if(freeSpace > 0.0f)
      {
        out.spacing    = freeSpace / lineChildCount;
        out.mainOffset = out.spacing / 2.0f;
      }
      else
      {
        out.mainOffset = freeSpace / 2.0f;
      }
      break;
    case FlexJustify::SPACE_EVENLY:
      if(freeSpace > 0.0f)
      {
        out.spacing    = freeSpace / (lineChildCount + 1);
        out.mainOffset = out.spacing;
      }
      else
      {
        out.mainOffset = freeSpace / 2.0f;
      }
      break;
  }
  return out;
}

// @param[in] owner The arranging View, threaded through so the final re-measures below can be
//                  attributed to it. nullptr makes the scope inert.
// @param[in] context  The flex's measure-mode facts
// @param[in,out] assignedMain nullptr for a flex that is not mode-aware; otherwise the flex's
//                     stored assigned main sizes, updated for every child this arrange measures
//                     EXACTLY on its main axis
void ArrangeOneFlexLine(ViewImpl* owner, FlexLine& line, std::vector<View>& children,
                        std::vector<MeasuredSize>& workingSizes, const LayoutRect& bounds,
                        float contentWidth, float contentHeight, float& crossOffsetInOut, float& mainOffsetInOut,
                        float spacing, FlexAlign alignItems, bool isMainAxisHorizontal, bool isMainAxisReversed,
                        const FlexModeContext& context, std::vector<FlexAssignedMain>* assignedMain)
{
  for(uint32_t idx : line.childIndices)
  {
    ViewImpl& childImpl  = GetImpl(children[idx]);
    float     childScale = childImpl.GetEffectiveScale();
    Insets    margin     = childImpl.GetMargin();

    float childMainSize  = isMainAxisHorizontal ? workingSizes[idx].width : workingSizes[idx].height;
    float childCrossSize = isMainAxisHorizontal ? workingSizes[idx].height : workingSizes[idx].width;
    float marginMain     = isMainAxisHorizontal ? (margin.start + margin.end) * childScale
                                                : (margin.top + margin.bottom) * childScale;
    float marginCross    = isMainAxisHorizontal ? (margin.top + margin.bottom) * childScale
                                                : (margin.start + margin.end) * childScale;

    bool crossIsMatchParent = isMainAxisHorizontal ? (childImpl.GetRequestedHeight() == MATCH_PARENT)
                                                   : (childImpl.GetRequestedWidth() == MATCH_PARENT);
    if(crossIsMatchParent)
    {
      childCrossSize = line.crossSize - marginCross;
    }

    bool mainIsMatchParent = isMainAxisHorizontal ? (childImpl.GetRequestedWidth() == MATCH_PARENT)
                                                  : (childImpl.GetRequestedHeight() == MATCH_PARENT);
    // A main-axis MATCH_PARENT child only fills the whole line when it does not
    // participate in flex-grow distribution. When flex-grow is set, the grow
    // distribution already produced the child's share in workingSizes; trusting
    // that value keeps siblings from being overlapped by a full-line overwrite.
    if(mainIsMatchParent && GetFlexGrow(childImpl) == 0.0f)
    {
      float availMain = isMainAxisHorizontal ? contentWidth : contentHeight;
      childMainSize   = std::max(0.0f, availMain - marginMain);
    }

    FlexAlign effectiveAlign = GetAlignSelf(childImpl);
    if(effectiveAlign == FlexAlign::AUTO)
    {
      effectiveAlign = alignItems;
    }

    float childCrossOffset = crossOffsetInOut;
    float crossSpace       = line.crossSize - childCrossSize - marginCross;
    switch(effectiveAlign)
    {
      case FlexAlign::FLEX_START:
      case FlexAlign::AUTO:
        break;
      case FlexAlign::FLEX_END:
        childCrossOffset += crossSpace;
        break;
      case FlexAlign::CENTER:
        childCrossOffset += crossSpace / 2.0f;
        break;
      case FlexAlign::STRETCH:
        childCrossSize = line.crossSize - marginCross;
        break;
      case FlexAlign::BASELINE:
        break;
    }

    float      allocMain  = childMainSize + marginMain;
    float      allocCross = childCrossSize + marginCross;
    LayoutRect childBounds;
    if(isMainAxisHorizontal)
    {
      if(isMainAxisReversed)
      {
        mainOffsetInOut -= allocMain;
        childBounds.x = bounds.x + mainOffsetInOut;
        mainOffsetInOut -= spacing;
      }
      else
      {
        childBounds.x = bounds.x + mainOffsetInOut;
        mainOffsetInOut += allocMain + spacing;
      }
      childBounds.y      = bounds.y + childCrossOffset;
      childBounds.width  = allocMain;
      childBounds.height = allocCross;
    }
    else
    {
      if(isMainAxisReversed)
      {
        mainOffsetInOut -= allocMain;
        childBounds.y = bounds.y + mainOffsetInOut;
        mainOffsetInOut -= spacing;
      }
      else
      {
        childBounds.y = bounds.y + mainOffsetInOut;
        mainOffsetInOut += allocMain + spacing;
      }
      childBounds.x      = bounds.x + childCrossOffset;
      childBounds.width  = allocCross;
      childBounds.height = allocMain;
    }
    childBounds.x += margin.start * childScale;
    childBounds.y += margin.top * childScale;
    childBounds.width  = std::max(0.0f, childBounds.width - (margin.start + margin.end) * childScale);
    childBounds.height = std::max(0.0f, childBounds.height - (margin.top + margin.bottom) * childScale);

    Internal::ViewDataImpl& childData = Internal::ViewDataImpl::Get(childImpl);
    if(context.modeAware)
    {
      const float                  requestedCross   = isMainAxisHorizontal ? childImpl.GetRequestedHeight() : childImpl.GetRequestedWidth();
      const Internal::MeasureModes stored           = childData.GetMeasureModes();
      const bool                   storedMainExact  = ((isMainAxisHorizontal ? stored.width : stored.height) == MeasureMode::EXACTLY);
      const bool                   storedCrossExact = ((isMainAxisHorizontal ? stored.height : stored.width) == MeasureMode::EXACTLY);
      // A stretched WRAP_CONTENT child that its flex measured EXACTLY on the cross axis (a determined
      // cross size, or a container with size-dependent content on an undetermined one) is laid out at
      // the line's cross size, which a sibling's own minimum or explicit size can make larger than the
      // size it was measured at.
      const bool stretchedExact = (requestedCross == WRAP_CONTENT) && (effectiveAlign == FlexAlign::STRETCH) && storedCrossExact;
      if(mainIsMatchParent || crossIsMatchParent || storedMainExact || stretchedExact)
      {
        // Main: a MATCH_PARENT child in the grow distribution and a child whose main size its flex
        // assigned are given their (possibly redistributed) share; a MATCH_PARENT main axis without
        // grow stays AT_MOST (the line sums the main sizes). Skipped when the child already holds a
        // measurement for exactly this slot.
        const MeasureMode mainMode   = mainIsMatchParent ? ((GetFlexGrow(childImpl) > 0.0f) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST)
                                                         : (storedMainExact ? MeasureMode::EXACTLY : MeasureMode::AT_MOST);
        const MeasureMode crossMode  = (crossIsMatchParent || stretchedExact) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
        const MeasureMode widthMode  = isMainAxisHorizontal ? mainMode : crossMode;
        const MeasureMode heightMode = isMainAxisHorizontal ? crossMode : mainMode;
        // The child's main mode is (or stays) EXACTLY at this slot: the next arrange at this main size
        // must read the slot, not the child's report (the slot clamped by its own minimum/maximum).
        // Stored whether or not the re-measure below is skipped.
        if(mainMode == MeasureMode::EXACTLY && assignedMain != nullptr && idx < assignedMain->size())
        {
          (*assignedMain)[idx].child  = &childImpl;
          (*assignedMain)[idx].extent = childMainSize;
        }
        if(!childData.CanSkipArrangeRemeasure(childBounds.width, childBounds.height, widthMode, heightMode))
        {
          // An assigned WRAP_CONTENT main axis, and a stretched cross axis of an undetermined cross
          // size, are EXACTLY here while the child's derivation says AT_MOST.
          const MeasureModeCheck                               check = ((!mainIsMatchParent && mainMode == MeasureMode::EXACTLY) || (stretchedExact && !context.crossDetermined))
                                                                         ? MeasureModeCheck::NONE
                                                                         : MeasureModeCheck::MATCHES_DERIVATION;
          Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
          childData.Measure(childBounds.width, childBounds.height, widthMode, heightMode, check);
        }
      }
    }
    else if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      const Internal::MeasureModes                         legacy = LegacyFlexChildModes(context, childImpl);
      Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(owner);
      childData.Measure(childBounds.width, childBounds.height, legacy.width, legacy.height, MeasureModeCheck::MATCHES_DERIVATION);
    }
    childImpl.Arrange(childBounds);
  }
  crossOffsetInOut += line.crossSize;
}

/**
 * The two-step part of a mode-aware single-line flex measure, run after step 1 (today's loop)
 * has built the line.
 *
 * Main step (only when the main extent is determined): the line's grow/shrink/basis distribution is
 * computed here, in the measure, on a copy of the step-1 sizes; a child whose main extent it
 * decides is re-measured EXACTLY at that extent -- a MATCH_PARENT child in the grow distribution,
 * and a WRAP_CONTENT child the distribution narrows (or widens, when its content follows its main
 * size: HasSizeDependentContent). Their new cross extents form the line's cross extent. Arrange
 * keeps these extents when it is laid out at this main size, and distributes the rest (see
 * BuildFlexLinesForArrange).
 *
 * Cross step (only when the cross extent is not determined): the line's cross extent is resolved, so
 * a MATCH_PARENT-cross child (measured as a probe in step 1) and a narrower STRETCH WRAP_CONTENT
 * child whose content follows its cross size are re-measured EXACTLY at the cross slot Arrange
 * gives them. The cross aggregate is final; their new main extents are added to the main sum.
 */
void ResolveFlexLineModes(FlexLine& line, std::vector<View>& children, std::vector<FlexMeasureRecord>& records,
                          const FlexModeContext& context, float availableMain, bool isMainAxisHorizontal, ViewImpl& view)
{
  const bool horizontal = isMainAxisHorizontal;

  if(context.mainDetermined)
  {
    std::vector<MeasuredSize> distributed(children.size());
    for(uint32_t idx : line.childIndices)
    {
      distributed[idx] = records[idx].working;
    }
    FlexLine distributedLine = line;
    ApplyFlexGrowShrink(distributedLine, children, distributed, availableMain, horizontal, nullptr);

    bool resized = false;
    for(uint32_t idx : line.childIndices)
    {
      FlexMeasureRecord& record = records[idx];
      if(record.fixedMain)
      {
        continue;
      }
      const float assignedMain = horizontal ? distributed[idx].width : distributed[idx].height;
      const float measuredMain = horizontal ? record.size.width : record.size.height;
      bool        assign       = false;
      if(record.matchMain)
      {
        // A MATCH_PARENT main axis without grow fills the whole line in Arrange; with grow it
        // takes its share.
        assign = record.grow > 0.0f;
      }
      else if(!FlexFloatEqual(assignedMain, measuredMain))
      {
        // Narrowed (shrink, or a basis below the content): always. Widened (grow, or a basis
        // above the content): only a child whose content follows its main size -- widening a
        // child whose content ignores it changes nothing but its own report.
        assign = (assignedMain < measuredMain) || Internal::ViewDataImpl::Get(*record.impl).HasSizeDependentContent(horizontal);
      }
      if(!assign)
      {
        continue;
      }
      record.assigned     = true;
      record.assignedMain = std::max(0.0f, assignedMain);

      // A MATCH_PARENT cross axis that is not determined yet is resolved below; the child is
      // re-measured once, there.
      if(record.matchCross && !context.crossDetermined)
      {
        continue;
      }

      const float width  = horizontal ? record.assignedMain : record.widthConstraint;
      const float height = horizontal ? record.heightConstraint : record.assignedMain;
      // An assigned WRAP_CONTENT main axis is EXACTLY here while its derivation says AT_MOST.
      const MeasureModeCheck check = record.matchMain ? MeasureModeCheck::MATCHES_DERIVATION : MeasureModeCheck::NONE;
      record.size                  = Internal::ViewDataImpl::Get(*record.impl).Measure(width, height, horizontal ? MeasureMode::EXACTLY : record.crossMode, horizontal ? record.crossMode : MeasureMode::EXACTLY, check);
      resized                      = true;
    }

    if(resized)
    {
      float crossSize = 0.0f;
      for(uint32_t idx : line.childIndices)
      {
        const FlexMeasureRecord& record = records[idx];
        const float              cross  = horizontal ? record.size.height + record.marginH : record.size.width + record.marginW;
        crossSize                       = std::max(crossSize, cross);
      }
      line.crossSize = crossSize;
    }
  }

  if(!context.crossDetermined)
  {
    // The cross content extent Arrange gives the line when the flex is placed at its measured
    // size: the line cross extent, or the flex's own min/max-clamped extent if that is larger
    // (Arrange takes max(line cross, available cross) for a single line).
    const Insets padding  = view.GetPadding();
    const float  s        = view.GetEffectiveScale();
    const float  padCross = horizontal ? static_cast<float>(padding.top + padding.bottom) * s : static_cast<float>(padding.start + padding.end) * s;
    const float  minCross = (horizontal ? view.GetMinimumHeight() : view.GetMinimumWidth()) * s;
    const float  maxCross = (horizontal ? view.GetMaximumHeight() : view.GetMaximumWidth()) * s;

    // Only a line with a cross size of its own -- what its children that are not MATCH_PARENT on the
    // cross axis and the flex's own minimum give it, not its MATCH_PARENT children's margins or
    // reports -- hands out a slot: otherwise every MATCH_PARENT-cross child keeps its probe,
    // whatever its margin.
    float ownLineCross = 0.0f;
    for(uint32_t idx : line.childIndices)
    {
      const FlexMeasureRecord& record = records[idx];
      if(!record.matchCross)
      {
        ownLineCross = std::max(ownLineCross, horizontal ? record.size.height + record.marginH : record.size.width + record.marginW);
      }
    }
    const bool  hasOwnCross = Internal::SnapEmptySlot(std::min(std::max(ownLineCross + padCross, minCross), maxCross) - padCross) > 0.0f;
    const float slotCross   = hasOwnCross ? std::max(line.crossSize, std::max(0.0f, std::min(std::max(line.crossSize + padCross, minCross), maxCross) - padCross)) : 0.0f;

    float mainDelta = 0.0f;
    for(uint32_t idx : line.childIndices)
    {
      FlexMeasureRecord& record      = records[idx];
      const float        marginCross = horizontal ? record.marginH : record.marginW;
      const float        crossSlot   = Internal::SnapEmptySlot(slotCross - marginCross);
      const float        cross       = horizontal ? record.size.height : record.size.width;
      if(!record.matchCross &&
         !(record.stretchCross && Internal::IsNarrowerThanSlot(cross, crossSlot) && Internal::ViewDataImpl::Get(*record.impl).HasSizeDependentContent(!horizontal)))
      {
        continue;
      }

      // A MATCH_PARENT cross axis is never measured exactly at an empty slot: nothing gives the line a
      // cross size of its own, so the step-1 probe (the child's at-most report) stands -- unless the
      // main step assigned the child's main size and deferred its re-measure to here: then it is
      // measured once at that main size, with the cross at its step-1 number and mode.
      if(record.matchCross && crossSlot <= 0.0f)
      {
        if(record.assigned)
        {
          const float crossNumber = horizontal ? record.heightConstraint : record.widthConstraint;
          record.size             = Internal::ViewDataImpl::Get(*record.impl).Measure(horizontal ? record.assignedMain : crossNumber, horizontal ? crossNumber : record.assignedMain, horizontal ? MeasureMode::EXACTLY : MeasureMode::AT_MOST, horizontal ? MeasureMode::AT_MOST : MeasureMode::EXACTLY, MeasureModeCheck::NONE);
        }
        continue;
      }

      const float       mainNumber = record.assigned ? record.assignedMain : (horizontal ? record.widthConstraint : record.heightConstraint);
      const MeasureMode mainMode   = record.assigned ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
      // Agrees with the derivation for a MATCH_PARENT-cross child whose main mode does too.
      const bool             mainMatches = record.assigned ? record.matchMain : !(record.matchMain && record.grow > 0.0f);
      const MeasureModeCheck check       = (record.matchCross && mainMatches) ? MeasureModeCheck::MATCHES_DERIVATION : MeasureModeCheck::NONE;
      const MeasuredSize     previous    = record.size;
      record.size                        = Internal::ViewDataImpl::Get(*record.impl).Measure(horizontal ? mainNumber : crossSlot, horizontal ? crossSlot : mainNumber, horizontal ? mainMode : MeasureMode::EXACTLY, horizontal ? MeasureMode::EXACTLY : mainMode, check);

      // The main sum counts the child's own main extent unless a basis replaces it or the
      // measure assigned it.
      if(!record.assigned && record.basis < 0.0f)
      {
        mainDelta += horizontal ? (record.size.width - previous.width) : (record.size.height - previous.height);
      }
    }
    if(mainDelta != 0.0f)
    {
      line.mainSize += mainDelta;
    }
  }
}

} // namespace

class FlexLayoutManager::Impl : public LayoutManager::Impl
{
public:
  Impl(FlexDirection direction, FlexWrap wrap, FlexJustify justify, FlexAlign alignItems, FlexAlign alignContent)
  : mDirection(direction),
    mWrap(wrap),
    mJustifyContent(justify),
    mAlignItems(alignItems),
    mAlignContent(alignContent)
  {
  }

  FlexDirection                 mDirection;
  FlexWrap                      mWrap;
  FlexJustify                   mJustifyContent;
  FlexAlign                     mAlignItems;
  FlexAlign                     mAlignContent;
  std::vector<FlexAssignedMain> mAssignedMain;                 ///< See FlexAssignedMain; written by the mode-aware Measure (main step) and Arrange (an EXACTLY main re-measure), read by the mode-aware Arrange
  float                         mAssignedAvailableMain{-1.0f}; ///< The main content size the stored sizes were distributed for
};

FlexLayoutManager::FlexLayoutManager(FlexDirection direction, FlexWrap wrap, FlexJustify justify, FlexAlign alignItems,
                                     FlexAlign alignContent)
: LayoutManager(new Impl(direction, wrap, justify, alignItems, alignContent))
{
}

FlexLayoutManager::~FlexLayoutManager()
{
}

void FlexLayoutManager::SetDirection(FlexDirection direction)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mDirection == direction)
  {
    return;
  }
  impl->mDirection = direction;

  // A MEASURE input: the direction maps which constraint is the main axis, so
  // line breaking and the accumulated extents both change with it. Neither cache
  // key can see this manager's own state, so the owner has to be told. See
  // LayoutManager::InvalidateOwnerMeasure.
  InvalidateOwnerMeasure();
}

FlexDirection FlexLayoutManager::GetDirection() const
{
  return GetImplAs<Impl>()->mDirection;
}

void FlexLayoutManager::SetWrap(FlexWrap wrap)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mWrap == wrap)
  {
    return;
  }
  impl->mWrap = wrap;

  // A MEASURE input, like the direction: wrapping decides how many lines the
  // children break into, which is what the measured extents accumulate.
  InvalidateOwnerMeasure();
}

FlexWrap FlexLayoutManager::GetWrap() const
{
  return GetImplAs<Impl>()->mWrap;
}

void FlexLayoutManager::SetJustifyContent(FlexJustify justify)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mJustifyContent == justify)
  {
    return;
  }
  impl->mJustifyContent = justify;

  // PLACEMENT-only: Measure() reads the direction and the wrap but never this
  // value -- justification distributes the free space of an already-measured
  // line, so no measured size can change. Invalidating the ARRANGE axis alone
  // keeps every measure cache in the subtree warm; the pass this schedules
  // re-runs only Arrange (and its arrange-owned re-measures, which hit their
  // measure caches at the unchanged constraints).
  InvalidateOwnerArrange();
}

FlexJustify FlexLayoutManager::GetJustifyContent() const
{
  return GetImplAs<Impl>()->mJustifyContent;
}

void FlexLayoutManager::SetAlignItems(FlexAlign align)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mAlignItems == align)
  {
    return;
  }
  impl->mAlignItems = align;

  // A MEASURE input: stretching decides which children a single-line flex measures EXACTLY
  // on the cross axis (a STRETCH WRAP_CONTENT child is measured at the cross extent once that
  // is determined), so a measured extent can change with it. Justification and line alignment stay
  // placement-only (see SetJustifyContent / SetAlignContent).
  InvalidateOwnerMeasure();
}

FlexAlign FlexLayoutManager::GetAlignItems() const
{
  return GetImplAs<Impl>()->mAlignItems;
}

void FlexLayoutManager::SetAlignContent(FlexAlign align)
{
  Impl* impl = GetImplAs<Impl>();
  if(impl->mAlignContent == align)
  {
    return;
  }
  impl->mAlignContent = align;

  // PLACEMENT-only: line distribution along the cross axis within the bounds
  // handed to Arrange(). See SetJustifyContent.
  InvalidateOwnerArrange();
}

FlexAlign FlexLayoutManager::GetAlignContent() const
{
  return GetImplAs<Impl>()->mAlignContent;
}

bool FlexLayoutManager::IsMainAxisHorizontal() const
{
  const FlexDirection direction = GetImplAs<Impl>()->mDirection;
  return direction == FlexDirection::ROW || direction == FlexDirection::ROW_REVERSE;
}

bool FlexLayoutManager::IsMainAxisReversed() const
{
  const FlexDirection direction = GetImplAs<Impl>()->mDirection;
  return direction == FlexDirection::ROW_REVERSE || direction == FlexDirection::COLUMN_REVERSE;
}

MeasuredSize FlexLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  const uint32_t    childCount = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float availableMain = IsMainAxisHorizontal() ? widthConstraint : heightConstraint;

  std::vector<FlexLine> lines;
  FlexLine              currentLine;

  // Measure modes: a single-line flex whose producer is exactly this manager is mode-aware; a
  // wrapping flex measures every child AT_MOST, as before, and a subclass or a view with a
  // layout callback passes the custom-parent modes of its view.
  const bool                     horizontal = IsMainAxisHorizontal();
  const FlexModeContext          context    = MakeFlexModeContext(Internal::ViewDataImpl::Get(*view), horizontal);
  std::vector<FlexMeasureRecord> records;
  if(context.modeAware)
  {
    records.resize(children.size());
  }

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    float  childScale            = childImpl.GetEffectiveScale();
    Insets margin                = childImpl.GetMargin();
    float  marginW               = (margin.start + margin.end) * childScale;
    float  marginH               = (margin.top + margin.bottom) * childScale;
    float  childWidthConstraint  = std::max(0.0f, widthConstraint - marginW);
    float  childHeightConstraint = std::max(0.0f, heightConstraint - marginH);

    // Step 1 is this loop: today's numbers and order. Main axis AT_MOST for every child (the
    // line sums the main extents; an assigned main extent is decided after the line is built).
    // Cross axis: assigned once the cross extent is determined -- MATCH_PARENT, or WRAP_CONTENT with
    // an effective STRETCH; before that a MATCH_PARENT cross axis is a probe.
    MeasureMode      widthMode  = MeasureMode::AT_MOST;
    MeasureMode      heightMode = MeasureMode::AT_MOST;
    MeasureModeCheck check      = MeasureModeCheck::NONE;
    if(context.modeAware)
    {
      FlexMeasureRecord& record   = records[i];
      const float        reqMain  = horizontal ? childImpl.GetRequestedWidth() : childImpl.GetRequestedHeight();
      const float        reqCross = horizontal ? childImpl.GetRequestedHeight() : childImpl.GetRequestedWidth();
      record.impl                 = &childImpl;
      record.widthConstraint      = childWidthConstraint;
      record.heightConstraint     = childHeightConstraint;
      record.marginW              = marginW;
      record.marginH              = marginH;
      record.grow                 = GetFlexGrow(childImpl);
      record.basis                = GetFlexBasis(childImpl);
      record.fixedMain            = reqMain >= 0.0f;
      record.matchMain            = reqMain == MATCH_PARENT;
      record.matchCross           = reqCross == MATCH_PARENT;
      record.stretchCross         = reqCross == WRAP_CONTENT && GetEffectiveAlign(childImpl, impl->mAlignItems) == FlexAlign::STRETCH;
      record.crossMode            = (context.crossDetermined && (record.matchCross || record.stretchCross)) ? MeasureMode::EXACTLY : MeasureMode::AT_MOST;
      widthMode                   = horizontal ? MeasureMode::AT_MOST : record.crossMode;
      heightMode                  = horizontal ? record.crossMode : MeasureMode::AT_MOST;
      // A MATCH_PARENT main axis in the grow distribution and a MATCH_PARENT cross axis that is
      // not determined yet are measured exactly later; this call is the probe.
      check = ((record.matchMain && record.grow > 0.0f) || (record.matchCross && !context.crossDetermined)) ? MeasureModeCheck::NONE : MeasureModeCheck::MATCHES_DERIVATION;
    }
    else
    {
      const Internal::MeasureModes legacy = LegacyFlexChildModes(context, childImpl);
      widthMode                           = legacy.width;
      heightMode                          = legacy.height;
      check                               = MeasureModeCheck::MATCHES_DERIVATION;
    }
    MeasuredSize childSize = Internal::ViewDataImpl::Get(childImpl).Measure(childWidthConstraint, childHeightConstraint, widthMode, heightMode, check);

    MeasuredSize workingSize = childSize;
    float        basis       = GetFlexBasis(childImpl);
    if(basis >= 0.0f)
    {
      if(IsMainAxisHorizontal())
      {
        workingSize.width = basis * childScale;
      }
      else
      {
        workingSize.height = basis * childScale;
      }
    }

    float childMainSize  = IsMainAxisHorizontal() ? workingSize.width + marginW
                                                  : workingSize.height + marginH;
    float childCrossSize = IsMainAxisHorizontal() ? workingSize.height + marginH
                                                  : workingSize.width + marginW;

    bool shouldWrap = (impl->mWrap != FlexWrap::NO_WRAP) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);

    if(shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }

    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);

    if(context.modeAware)
    {
      // The line's grow/shrink totals, as Arrange accumulates them: the main step distributes
      // the line in the measure. Not part of today's measured result.
      records[i].size    = childSize;
      records[i].working = workingSize;
      currentLine.totalFlexGrow += records[i].grow;
      currentLine.totalFlexShrink += GetFlexShrink(childImpl);
    }
  }

  if(!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }

  // A mode-aware flex never wraps, so it has at most one line.
  if(context.modeAware && lines.size() == 1u)
  {
    ResolveFlexLineModes(lines[0], children, records, context, availableMain, horizontal, *view);
  }

  // The main sizes this measure assigned, kept for Arrange together with the main size they were
  // distributed for and each assigned child's line input: an assigned child reports its size
  // clamped by its own minimum/maximum, so its report cannot carry the size back, and an arrange
  // at another main size distributes the line again from the line inputs.
  if(context.modeAware)
  {
    impl->mAssignedMain.assign(children.size(), FlexAssignedMain{});
    impl->mAssignedAvailableMain = availableMain;
    for(uint32_t i = 0; i < children.size(); ++i)
    {
      FlexAssignedMain& entry = impl->mAssignedMain[i];
      entry.child             = &GetImpl(children[i]);
      if(records[i].assigned)
      {
        entry.extent  = records[i].assignedMain;
        entry.natural = horizontal ? records[i].working.width : records[i].working.height;
      }
    }
  }
  else
  {
    impl->mAssignedMain.clear();
    impl->mAssignedAvailableMain = -1.0f;
  }

  float totalMainSize  = 0.0f;
  float totalCrossSize = 0.0f;

  for(const auto& line : lines)
  {
    totalMainSize = std::max(totalMainSize, line.mainSize);
    totalCrossSize += line.crossSize;
  }

  MeasuredSize result;
  if(IsMainAxisHorizontal())
  {
    result.width  = totalMainSize;
    result.height = totalCrossSize;
  }
  else
  {
    result.width  = totalCrossSize;
    result.height = totalMainSize;
  }

  return result;
}

void FlexLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return;
  }

  auto* impl = GetImplAs<Impl>();

  const uint32_t    childCount = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(childCount);
  for(uint32_t i = 0; i < childCount; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float contentWidth  = bounds.width;
  float contentHeight = bounds.height;
  float availableMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;

  // Local working buffer: seeded from each child's current measured size.
  std::vector<MeasuredSize> workingSizes(children.size());
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    workingSizes[i] = GetImpl(children[i]).GetMeasuredSize();
  }

  // A mode-aware flex keeps the main sizes it assigned (the child's stored main mode is EXACTLY;
  // the size is stored in mAssignedMain -- the child reports it clamped by its own minimum/maximum,
  // so its report is not read back) when they were distributed for this very main size, and
  // distributes the rest; see BuildFlexLinesForArrange. Anything else distributes over every child,
  // as before.
  const FlexModeContext                context    = MakeFlexModeContext(Internal::ViewDataImpl::Get(*view), IsMainAxisHorizontal());
  const std::vector<FlexAssignedMain>* entriesPtr = nullptr;
  if(context.modeAware)
  {
    // A slot per child. Resized, not rebuilt: an entry whose child is unchanged stays readable.
    if(impl->mAssignedMain.size() != children.size())
    {
      impl->mAssignedMain.resize(children.size());
    }
    // The stored sizes are this arrange's distribution only when they were distributed for this very
    // main size (the measure cache's tolerance). Otherwise the flex is laid out at another main size
    // -- a minimum or a stretch larger than its request, a cell, explicit bounds, an arrange-only pass
    // at a new size -- and the line is distributed again from the children's natural sizes, exactly
    // as before modes existed.
    const bool sameMain = FlexFloatEqual(impl->mAssignedAvailableMain, availableMain);
    for(uint32_t i = 0; i < children.size(); ++i)
    {
      const ViewImpl&   childImpl = GetImpl(children[i]);
      FlexAssignedMain& entry     = impl->mAssignedMain[i];
      if(entry.child != &childImpl)
      {
        entry = FlexAssignedMain{&childImpl}; // a changed child list: nothing is known about this child
      }
      entry.kept                               = false;
      const Internal::MeasureModes modes       = Internal::ViewDataImpl::Get(childImpl).GetMeasureModes();
      const bool                   exactMain   = ((IsMainAxisHorizontal() ? modes.width : modes.height) == MeasureMode::EXACTLY);
      float&                       workingMain = IsMainAxisHorizontal() ? workingSizes[i].width : workingSizes[i].height;
      if(exactMain && entry.extent >= 0.0f)
      {
        if(sameMain)
        {
          workingMain = entry.extent; // its slot, not its report (the slot clamped by its own minimum/maximum)
          entry.kept  = true;
        }
        else if(entry.natural >= 0.0f)
        {
          workingMain = entry.natural; // distributed again; ArrangeOneFlexLine re-measures it at its new share
        }
        else
        {
          entry.kept = true; // defensive: no natural size known (every writer records one), keep the report
        }
      }
      else if(exactMain)
      {
        entry.kept = true; // EXACTLY from outside this flex: its report is its slot (exact unless its own min/max binds)
      }
      else
      {
        entry.extent  = -1.0f;       // not assigned: its report is its natural size, kept in case this
        entry.natural = workingMain; // arrange assigns it (a MATCH_PARENT child taking a grow share)
      }
    }
    entriesPtr = &impl->mAssignedMain;
  }

  std::vector<FlexLine> lines =
    BuildFlexLinesForArrange(children, workingSizes, availableMain, IsMainAxisHorizontal(), impl->mWrap, entriesPtr);

  for(auto& line : lines)
  {
    ApplyFlexGrowShrink(line, children, workingSizes, availableMain, IsMainAxisHorizontal(), entriesPtr);
  }

  if(impl->mWrap == FlexWrap::WRAP_REVERSE)
  {
    std::reverse(lines.begin(), lines.end());
  }

  float availableCross = IsMainAxisHorizontal() ? contentHeight : contentWidth;

  if(impl->mWrap == FlexWrap::NO_WRAP && lines.size() == 1)
  {
    lines[0].crossSize = std::max(lines[0].crossSize, availableCross);
  }

  float totalLineCross = 0.0f;
  for(const auto& line : lines)
  {
    totalLineCross += line.crossSize;
  }
  float freeCross = std::max(0.0f, availableCross - totalLineCross);

  float crossOffset = 0.0f;

  if(impl->mWrap != FlexWrap::NO_WRAP && !lines.empty())
  {
    switch(impl->mAlignContent)
    {
      case FlexAlign::FLEX_END:
        crossOffset = freeCross;
        break;
      case FlexAlign::CENTER:
        crossOffset = freeCross / 2.0f;
        break;
      case FlexAlign::STRETCH:
      {
        float extraPerLine = freeCross / static_cast<float>(lines.size());
        for(auto& line : lines)
        {
          line.crossSize += extraPerLine;
        }
        break;
      }
      case FlexAlign::FLEX_START:
      case FlexAlign::AUTO:
      case FlexAlign::BASELINE:
      default:
        break;
    }
  }

  for(auto& line : lines)
  {
    float              freeSpace  = availableMain - line.mainSize;
    FlexJustifyOffsets justify    = GetFlexJustifyOffsets(freeSpace, impl->mJustifyContent, line.childIndices.size());
    float              mainOffset = justify.mainOffset;
    if(IsMainAxisReversed())
    {
      float contentMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;
      mainOffset        = contentMain - justify.mainOffset;
    }
    ArrangeOneFlexLine(view, line, children, workingSizes, bounds, contentWidth, contentHeight, crossOffset, mainOffset,
                       justify.spacing, impl->mAlignItems, IsMainAxisHorizontal(), IsMainAxisReversed(), context,
                       context.modeAware ? &impl->mAssignedMain : nullptr);
  }

  // Every stored size now belongs to this arrange's distribution: the kept ones, and the ones
  // ArrangeOneFlexLine recorded at their new share.
  if(context.modeAware)
  {
    impl->mAssignedAvailableMain = availableMain;
  }
}

} // namespace Ui
} //namespace DALI_NAMESPACE
