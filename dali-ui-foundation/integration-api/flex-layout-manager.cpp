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
#include <dali-ui-foundation/integration-api/flex-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <functional>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{
namespace UI
{
namespace Integration
{

namespace
{

struct FlexLine
{
  std::vector<uint32_t> childIndices;
  float mainSize{0.0f};
  float crossSize{0.0f};
  float totalFlexGrow{0.0f};
  float totalFlexShrink{0.0f};
};

std::vector<FlexLine> BuildFlexLinesForArrange(const ViewImpl::ChildContainer& children, float availableMain,
                                               bool isMainAxisHorizontal, FlexWrap wrap,
                                               const std::function<ViewImpl&(UI::View)>& getImpl)
{
  std::vector<FlexLine> lines;
  FlexLine currentLine;
  for (uint32_t i = 0; i < children.size(); ++i)
  {
    const auto& childData = children[i];
    ViewImpl& childImpl = getImpl(childData.view);
    Extents margin = childImpl.GetViewMargin();
    float childMainSize = isMainAxisHorizontal ? childData.measuredSize.width + margin.start + margin.end
                                               : childData.measuredSize.height + margin.top + margin.bottom;
    bool shouldWrap = (wrap != FlexWrap::NoWrap) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);
    if (shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }
    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    float childCrossSize = isMainAxisHorizontal ? childData.measuredSize.height + margin.top + margin.bottom
                                                : childData.measuredSize.width + margin.start + margin.end;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }
  if (!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }
  return lines;
}

struct FlexJustifyOffsets
{
  float mainOffset{0.0f};
  float spacing{0.0f};
};

FlexJustifyOffsets GetFlexJustifyOffsets(float freeSpace, FlexJustify justify, size_t lineChildCount)
{
  FlexJustifyOffsets out;
  switch (justify)
  {
    case FlexJustify::FlexStart:
      break;
    case FlexJustify::FlexEnd:
      out.mainOffset = freeSpace;
      break;
    case FlexJustify::Center:
      out.mainOffset = freeSpace / 2.0f;
      break;
    case FlexJustify::SpaceBetween:
      if (lineChildCount > 1)
      {
        out.spacing = freeSpace / (lineChildCount - 1);
      }
      break;
    case FlexJustify::SpaceAround:
      out.spacing = freeSpace / lineChildCount;
      out.mainOffset = out.spacing / 2.0f;
      break;
    case FlexJustify::SpaceEvenly:
      out.spacing = freeSpace / (lineChildCount + 1);
      out.mainOffset = out.spacing;
      break;
  }
  return out;
}

void ArrangeOneFlexLine(FlexLine& line, ViewImpl::ChildContainer& children, const LayoutRect& bounds,
                        float contentWidth, float contentHeight, float& crossOffsetInOut, float& mainOffsetInOut,
                        float spacing, FlexAlign alignItems, bool isMainAxisHorizontal, bool isMainAxisReversed,
                        const std::function<ViewImpl&(UI::View)>& getImpl)
{
  for (uint32_t idx : line.childIndices)
  {
    auto& childData = children[idx];
    ViewImpl& childImpl = getImpl(childData.view);
    Extents margin = childImpl.GetViewMargin();

    float childMainSize = isMainAxisHorizontal ? childData.measuredSize.width : childData.measuredSize.height;
    float childCrossSize = isMainAxisHorizontal ? childData.measuredSize.height : childData.measuredSize.width;
    float marginMain = isMainAxisHorizontal ? static_cast<float>(margin.start + margin.end)
                                            : static_cast<float>(margin.top + margin.bottom);
    float marginCross = isMainAxisHorizontal ? static_cast<float>(margin.top + margin.bottom)
                                             : static_cast<float>(margin.start + margin.end);

    float childCrossOffset = crossOffsetInOut;
    float crossSpace = line.crossSize - childCrossSize - marginCross;
    switch (alignItems)
    {
      case FlexAlign::FlexStart:
      case FlexAlign::Auto:
        break;
      case FlexAlign::FlexEnd:
        childCrossOffset += crossSpace;
        break;
      case FlexAlign::Center:
        childCrossOffset += crossSpace / 2.0f;
        break;
      case FlexAlign::Stretch:
        childCrossSize = line.crossSize - marginCross;
        break;
      case FlexAlign::Baseline:
        break;
    }

    float allocMain = childMainSize + marginMain;
    float allocCross = childCrossSize + marginCross;
    LayoutRect childBounds;
    if (isMainAxisHorizontal)
    {
      if (isMainAxisReversed)
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
      childBounds.y = bounds.y + childCrossOffset;
      childBounds.width = allocMain;
      childBounds.height = allocCross;
    }
    else
    {
      if (isMainAxisReversed)
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
      childBounds.x = bounds.x + childCrossOffset;
      childBounds.width = allocCross;
      childBounds.height = allocMain;
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

void FlexLayoutManager::SetWrap(FlexWrap wrap)
{
  mWrap = wrap;
}

void FlexLayoutManager::SetJustifyContent(FlexJustify justify)
{
  mJustifyContent = justify;
}

void FlexLayoutManager::SetAlignItems(FlexAlign align)
{
  mAlignItems = align;
}

void FlexLayoutManager::SetAlignContent(FlexAlign align)
{
  mAlignContent = align;
}

bool FlexLayoutManager::IsMainAxisHorizontal() const
{
  return mDirection == FlexDirection::Row || mDirection == FlexDirection::RowReverse;
}

bool FlexLayoutManager::IsMainAxisReversed() const
{
  return mDirection == FlexDirection::RowReverse || mDirection == FlexDirection::ColumnReverse;
}

MeasuredSize FlexLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if (!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);
  Extents padding = view->GetViewPadding();
  float paddingMain = IsMainAxisHorizontal() ? static_cast<float>(padding.start + padding.end)
                                             : static_cast<float>(padding.top + padding.bottom);
  float paddingCross = IsMainAxisHorizontal() ? static_cast<float>(padding.top + padding.bottom)
                                              : static_cast<float>(padding.start + padding.end);

  float availableMain = (IsMainAxisHorizontal() ? widthConstraint : heightConstraint) - paddingMain;
  float availableCross = (IsMainAxisHorizontal() ? heightConstraint : widthConstraint) - paddingCross;

  std::vector<FlexLine> lines;
  FlexLine currentLine;

  for (uint32_t i = 0; i < children.size(); ++i)
  {
    auto& childData = children[i];
    ViewImpl& childImpl = GetImpl(childData.view);
    float childWidthConstraint = IsMainAxisHorizontal() ? availableMain : availableCross;
    float childHeightConstraint = IsMainAxisHorizontal() ? availableCross : availableMain;
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    childData.measuredSize = childSize;

    Extents margin = childImpl.GetViewMargin();
    float childMainSize = IsMainAxisHorizontal() ? childSize.width + margin.start + margin.end
                                                 : childSize.height + margin.top + margin.bottom;
    float childCrossSize = IsMainAxisHorizontal() ? childSize.height + margin.top + margin.bottom
                                                  : childSize.width + margin.start + margin.end;

    bool shouldWrap = (mWrap != FlexWrap::NoWrap) && !currentLine.childIndices.empty() &&
                      (currentLine.mainSize + childMainSize > availableMain);

    if (shouldWrap)
    {
      lines.push_back(currentLine);
      currentLine = FlexLine();
    }

    currentLine.childIndices.push_back(i);
    currentLine.mainSize += childMainSize;
    currentLine.crossSize = std::max(currentLine.crossSize, childCrossSize);
  }

  if (!currentLine.childIndices.empty())
  {
    lines.push_back(currentLine);
  }

  float totalMainSize = 0.0f;
  float totalCrossSize = 0.0f;

  for (const auto& line : lines)
  {
    totalMainSize = std::max(totalMainSize, line.mainSize);
    totalCrossSize += line.crossSize;
  }

  MeasuredSize result;
  if (IsMainAxisHorizontal())
  {
    result.width = totalMainSize;
    result.height = totalCrossSize;
  }
  else
  {
    result.width = totalCrossSize;
    result.height = totalMainSize;
  }

  return result;
}

MeasuredSize FlexLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if (!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);
  float contentWidth = bounds.width;
  float contentHeight = bounds.height;
  float availableMain = IsMainAxisHorizontal() ? contentWidth : contentHeight;
  auto getImpl = [this](UI::View v) -> ViewImpl& { return GetImpl(v); };

  std::vector<FlexLine> lines =
      BuildFlexLinesForArrange(children, availableMain, IsMainAxisHorizontal(), mWrap, getImpl);

  if (mWrap == FlexWrap::WrapReverse)
  {
    std::reverse(lines.begin(), lines.end());
  }

  float crossOffset = 0.0f;
  for (auto& line : lines)
  {
    float freeSpace = availableMain - line.mainSize;
    FlexJustifyOffsets justify = GetFlexJustifyOffsets(freeSpace, mJustifyContent, line.childIndices.size());
    float mainOffset = justify.mainOffset;
    if (IsMainAxisReversed())
    {
      mainOffset = (IsMainAxisHorizontal() ? contentWidth : contentHeight);
    }
    ArrangeOneFlexLine(line, children, bounds, contentWidth, contentHeight, crossOffset, mainOffset, justify.spacing,
                       mAlignItems, IsMainAxisHorizontal(), IsMainAxisReversed(), getImpl);
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace UI
} // namespace Dali
