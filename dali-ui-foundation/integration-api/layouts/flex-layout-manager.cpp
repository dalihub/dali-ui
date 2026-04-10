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
#include <dali-ui-foundation/integration-api/layouts/flex-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <functional>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/layouts/flex-layout-params-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
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

struct FlexLine
{
  std::vector<uint32_t> childIndices;
  float                 mainSize{0.0f};
  float                 crossSize{0.0f};
  float                 totalFlexGrow{0.0f};
  float                 totalFlexShrink{0.0f};
};

std::vector<FlexLine> BuildFlexLinesForArrange(ViewImpl::ChildContainer& children, float availableMain,
                                               bool isMainAxisHorizontal, FlexWrap wrap,
                                               const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  std::vector<FlexLine> lines;
  FlexLine              currentLine;
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    auto&     childData = children[i];
    ViewImpl& childImpl = getImpl(childData.view);
    if(childImpl.IsLayoutModeStandalone())
    {
      // Standalone children are excluded from flex line building.
      continue;
    }
    Extents margin = childImpl.GetViewMargin();

    // Apply flex-basis: override the main-axis measured size when flex-basis is set
    float basis = GetFlexBasis(childImpl);
    if(basis > 0.0f)
    {
      if(isMainAxisHorizontal)
      {
        childData.measuredSize.width = basis;
      }
      else
      {
        childData.measuredSize.height = basis;
      }
    }

    float childMainSize = isMainAxisHorizontal ? childData.measuredSize.width + margin.start + margin.end
                                               : childData.measuredSize.height + margin.top + margin.bottom;
    bool  shouldWrap    = (wrap != FlexWrap::NO_WRAP) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);
    if(shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }
    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    currentLine.totalFlexGrow += GetFlexGrow(childImpl);
    currentLine.totalFlexShrink += GetFlexShrink(childImpl);
    float childCrossSize  = isMainAxisHorizontal ? childData.measuredSize.height + margin.top + margin.bottom
                                                 : childData.measuredSize.width + margin.start + margin.end;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }
  if(!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }
  return lines;
}

/**
 * @brief Applies flex-grow/flex-shrink to adjust child main-axis sizes within a line.
 *
 * When there is free space and totalFlexGrow > 0, distribute extra space.
 * When there is overflow and totalFlexShrink > 0, shrink children proportionally.
 */
void ApplyFlexGrowShrink(FlexLine& line, ViewImpl::ChildContainer& children, float availableMain,
                         bool isMainAxisHorizontal, const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  float freeSpace = availableMain - line.mainSize;

  if(freeSpace > 0.0f && line.totalFlexGrow > 0.0f)
  {
    // Distribute extra space proportional to flex-grow
    for(uint32_t idx : line.childIndices)
    {
      auto&     childData = children[idx];
      ViewImpl& childImpl = getImpl(childData.view);
      float     grow      = GetFlexGrow(childImpl);
      if(grow > 0.0f)
      {
        float extra = (grow / line.totalFlexGrow) * freeSpace;
        if(isMainAxisHorizontal)
        {
          childData.measuredSize.width += extra;
        }
        else
        {
          childData.measuredSize.height += extra;
        }
      }
    }
    line.mainSize = availableMain;
  }
  else if(freeSpace < 0.0f && line.totalFlexShrink > 0.0f)
  {
    // Shrink proportional to flex-shrink * child base size
    float totalWeightedShrink = 0.0f;
    for(uint32_t idx : line.childIndices)
    {
      auto&     childData     = children[idx];
      ViewImpl& childImpl     = getImpl(childData.view);
      float     childMainSize = isMainAxisHorizontal ? childData.measuredSize.width : childData.measuredSize.height;
      float     shrink        = GetFlexShrink(childImpl);
      totalWeightedShrink += shrink * childMainSize;
    }
    if(totalWeightedShrink > 0.0f)
    {
      float overflow = -freeSpace;
      for(uint32_t idx : line.childIndices)
      {
        auto&     childData     = children[idx];
        ViewImpl& childImpl     = getImpl(childData.view);
        float     childMainSize = isMainAxisHorizontal ? childData.measuredSize.width : childData.measuredSize.height;
        float     shrink        = GetFlexShrink(childImpl);
        float     reduction     = (shrink * childMainSize / totalWeightedShrink) * overflow;
        if(isMainAxisHorizontal)
        {
          childData.measuredSize.width = std::max(0.0f, childData.measuredSize.width - reduction);
        }
        else
        {
          childData.measuredSize.height = std::max(0.0f, childData.measuredSize.height - reduction);
        }
      }
      line.mainSize = availableMain;
    }
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
    case FlexJustify::SPACE_BETWEEN:
      if(lineChildCount > 1)
      {
        out.spacing = freeSpace / (lineChildCount - 1);
      }
      break;
    case FlexJustify::SPACE_AROUND:
      out.spacing    = freeSpace / lineChildCount;
      out.mainOffset = out.spacing / 2.0f;
      break;
    case FlexJustify::SPACE_EVENLY:
      out.spacing    = freeSpace / (lineChildCount + 1);
      out.mainOffset = out.spacing;
      break;
  }
  return out;
}

void ArrangeOneFlexLine(FlexLine& line, ViewImpl::ChildContainer& children, const LayoutRect& bounds,
                        float contentWidth, float contentHeight, float& crossOffsetInOut, float& mainOffsetInOut,
                        float spacing, FlexAlign alignItems, bool isMainAxisHorizontal, bool isMainAxisReversed,
                        const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  for(uint32_t idx : line.childIndices)
  {
    auto&     childData = children[idx];
    ViewImpl& childImpl = getImpl(childData.view);
    Extents   margin    = childImpl.GetViewMargin();

    float childMainSize  = isMainAxisHorizontal ? childData.measuredSize.width : childData.measuredSize.height;
    float childCrossSize = isMainAxisHorizontal ? childData.measuredSize.height : childData.measuredSize.width;
    float marginMain     = isMainAxisHorizontal ? static_cast<float>(margin.start + margin.end)
                                                : static_cast<float>(margin.top + margin.bottom);
    float marginCross    = isMainAxisHorizontal ? static_cast<float>(margin.top + margin.bottom)
                                                : static_cast<float>(margin.start + margin.end);

    // MATCH_PARENT on cross axis: fill the line's cross-axis space.
    bool crossIsMatchParent = isMainAxisHorizontal ? (childImpl.GetRequestedHeight() == MATCH_PARENT)
                                                   : (childImpl.GetRequestedWidth() == MATCH_PARENT);
    if(crossIsMatchParent)
    {
      childCrossSize = line.crossSize - marginCross;
    }

    // MATCH_PARENT on main axis: fill the available main-axis space.
    bool mainIsMatchParent = isMainAxisHorizontal ? (childImpl.GetRequestedWidth() == MATCH_PARENT)
                                                  : (childImpl.GetRequestedHeight() == MATCH_PARENT);
    if(mainIsMatchParent)
    {
      float availMain = isMainAxisHorizontal ? contentWidth : contentHeight;
      childMainSize   = std::max(0.0f, availMain - marginMain);
    }

    // Use align-self if set, otherwise fall back to align-items
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
    childBounds.x += static_cast<float>(margin.start);
    childBounds.y += static_cast<float>(margin.top);
    childBounds.width  = std::max(0.0f, childBounds.width - static_cast<float>(margin.start + margin.end));
    childBounds.height = std::max(0.0f, childBounds.height - static_cast<float>(margin.top + margin.bottom));

    // Re-measure MATCH_PARENT children with their final size.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childImpl.Measure(childBounds.width, childBounds.height);
    }
    childImpl.Arrange(childBounds);
    childData.arrangedBounds = childBounds;
  }
  crossOffsetInOut += line.crossSize;
}

} // namespace

FlexLayoutManager::FlexLayoutManager(FlexDirection direction, FlexWrap wrap, FlexJustify justify, FlexAlign alignItems,
                                     FlexAlign alignContent)
: mDirection(direction),
  mWrap(wrap),
  mJustifyContent(justify),
  mAlignItems(alignItems),
  mAlignContent(alignContent)
{
}

FlexLayoutManager::~FlexLayoutManager()
{
}

void FlexLayoutManager::SetDirection(FlexDirection direction)
{
  mDirection = direction;
}

FlexDirection FlexLayoutManager::GetDirection() const
{
  return mDirection;
}

void FlexLayoutManager::SetWrap(FlexWrap wrap)
{
  mWrap = wrap;
}

FlexWrap FlexLayoutManager::GetWrap() const
{
  return mWrap;
}

void FlexLayoutManager::SetJustifyContent(FlexJustify justify)
{
  mJustifyContent = justify;
}

FlexJustify FlexLayoutManager::GetJustifyContent() const
{
  return mJustifyContent;
}

void FlexLayoutManager::SetAlignItems(FlexAlign align)
{
  mAlignItems = align;
}

FlexAlign FlexLayoutManager::GetAlignItems() const
{
  return mAlignItems;
}

void FlexLayoutManager::SetAlignContent(FlexAlign align)
{
  mAlignContent = align;
}

FlexAlign FlexLayoutManager::GetAlignContent() const
{
  return mAlignContent;
}

bool FlexLayoutManager::IsMainAxisHorizontal() const
{
  return mDirection == FlexDirection::ROW || mDirection == FlexDirection::ROW_REVERSE;
}

bool FlexLayoutManager::IsMainAxisReversed() const
{
  return mDirection == FlexDirection::ROW_REVERSE || mDirection == FlexDirection::COLUMN_REVERSE;
}

MeasuredSize FlexLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children      = GetChildren(view);
  float availableMain = IsMainAxisHorizontal() ? widthConstraint : heightConstraint;

  Extents parentPadding = view->GetViewPadding();
  float   parentWidth   = widthConstraint + static_cast<float>(parentPadding.start + parentPadding.end);
  float   parentHeight  = heightConstraint + static_cast<float>(parentPadding.top + parentPadding.bottom);

  std::vector<FlexLine> lines;
  FlexLine              currentLine;

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    auto&     childData = children[i];
    ViewImpl& childImpl = GetImpl(childData.view);
    Extents   margin    = childImpl.GetViewMargin();

    // Standalone children ignore parent padding entirely; they are sized
    // against the parent's full inner size minus their own margin.
    float        baseWidth             = childImpl.IsLayoutModeStandalone() ? parentWidth : widthConstraint;
    float        baseHeight            = childImpl.IsLayoutModeStandalone() ? parentHeight : heightConstraint;
    float        marginW               = static_cast<float>(margin.start + margin.end);
    float        marginH               = static_cast<float>(margin.top + margin.bottom);
    float        childWidthConstraint  = std::max(0.0f, baseWidth - marginW);
    float        childHeightConstraint = std::max(0.0f, baseHeight - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    childData.measuredSize             = childSize;

    // Standalone children are still measured (so MATCH_PARENT / WRAP_CONTENT
    // resolve), but excluded from this layout's line building, flex-grow/shrink,
    // wrap and accumulation.
    if(childImpl.IsLayoutModeStandalone())
    {
      continue;
    }

    // Apply flex-basis: override the main-axis measured size when flex-basis is set
    float basis = GetFlexBasis(childImpl);
    if(basis > 0.0f)
    {
      if(IsMainAxisHorizontal())
      {
        childData.measuredSize.width = basis;
      }
      else
      {
        childData.measuredSize.height = basis;
      }
    }

    float childMainSize  = IsMainAxisHorizontal() ? childData.measuredSize.width + margin.start + margin.end
                                                  : childData.measuredSize.height + margin.top + margin.bottom;
    float childCrossSize = IsMainAxisHorizontal() ? childData.measuredSize.height + margin.top + margin.bottom
                                                  : childData.measuredSize.width + margin.start + margin.end;

    bool shouldWrap = (mWrap != FlexWrap::NO_WRAP) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);

    if(shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }

    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }

  if(!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
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

MeasuredSize FlexLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children      = GetChildren(view);
  float contentWidth  = bounds.width;
  float contentHeight = bounds.height;
  float availableMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;
  auto  getImpl       = [this](Ui::View v) -> ViewImpl&
  { return GetImpl(v); };

  // Save original measured sizes: BuildFlexLinesForArrange and ApplyFlexGrowShrink
  // modify childData.measuredSize in-place (flex-basis, grow, shrink). Without
  // restoring afterwards, repeated layout passes accumulate these adjustments.
  std::vector<MeasuredSize> savedMeasuredSizes(children.size());
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    savedMeasuredSizes[i] = children[i].measuredSize;
  }

  std::vector<FlexLine> lines =
    BuildFlexLinesForArrange(children, availableMain, IsMainAxisHorizontal(), mWrap, getImpl);

  // Apply flex-grow/flex-shrink to adjust child sizes within each line
  for(auto& line : lines)
  {
    ApplyFlexGrowShrink(line, children, availableMain, IsMainAxisHorizontal(), getImpl);
  }

  if(mWrap == FlexWrap::WRAP_REVERSE)
  {
    std::reverse(lines.begin(), lines.end());
  }

  float availableCross = IsMainAxisHorizontal() ? contentHeight : contentWidth;

  // For NO_WRAP, the single line fills the entire available cross-axis space.
  // This matches CSS flexbox: a single flex line stretches to the container's cross size.
  if(mWrap == FlexWrap::NO_WRAP && lines.size() == 1)
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

  // Apply align-content when wrapping is enabled
  if(mWrap != FlexWrap::NO_WRAP && !lines.empty())
  {
    switch(mAlignContent)
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
    FlexJustifyOffsets justify    = GetFlexJustifyOffsets(freeSpace, mJustifyContent, line.childIndices.size());
    float              mainOffset = justify.mainOffset;
    if(IsMainAxisReversed())
    {
      float contentMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;
      mainOffset        = contentMain - justify.mainOffset;
    }
    ArrangeOneFlexLine(line, children, bounds, contentWidth, contentHeight, crossOffset, mainOffset, justify.spacing,
                       mAlignItems, IsMainAxisHorizontal(), IsMainAxisReversed(), getImpl);
  }

  // Restore original measured sizes so repeated layout passes start clean.
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    children[i].measuredSize = savedMeasuredSizes[i];
  }

  // Arrange standalone children: place at RequestedPositionX/Y plus the
  // child's own margin in the parent's coordinate space (ignoring parent
  // padding). MATCH_PARENT fills the full parent size minus own margin.
  for(auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);
    if(!childImpl.IsLayoutModeStandalone())
    {
      continue;
    }
    Extents standaloneMargin = childImpl.GetViewMargin();
    float   marginW          = static_cast<float>(standaloneMargin.start + standaloneMargin.end);
    float   marginH          = static_cast<float>(standaloneMargin.top + standaloneMargin.bottom);
    Extents parentPadding    = view->GetViewPadding();
    float   parentWidth      = contentWidth + static_cast<float>(parentPadding.start + parentPadding.end);
    float   parentHeight     = contentHeight + static_cast<float>(parentPadding.top + parentPadding.bottom);
    float   standaloneW      = childData.measuredSize.width;
    float   standaloneH      = childData.measuredSize.height;
    if(childImpl.GetRequestedWidth() == MATCH_PARENT)
    {
      standaloneW = std::max(0.0f, parentWidth - marginW);
    }
    if(childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      standaloneH = std::max(0.0f, parentHeight - marginH);
    }
    // Re-measure MATCH_PARENT standalone children with their final size.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childImpl.Measure(standaloneW, standaloneH);
    }
    LayoutRect standaloneBounds(childImpl.GetPositionX() + static_cast<float>(standaloneMargin.start),
                                childImpl.GetPositionY() + static_cast<float>(standaloneMargin.top),
                                standaloneW,
                                standaloneH);
    childImpl.Arrange(standaloneBounds);
    childData.arrangedBounds = standaloneBounds;
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
