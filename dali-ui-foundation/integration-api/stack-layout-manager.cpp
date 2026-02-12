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
#include <dali-ui-foundation/integration-api/stack-layout-manager.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <algorithm>
#include <functional>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace UI
{
namespace Integration
{

namespace
{

float GetChildWeight(UI::View view)
{
  if (!view)
  {
    return 0.0f;
  }
  Property::Index index = view.GetPropertyIndex("stackLayoutWeight");
  if (index == Property::INVALID_INDEX)
  {
    return 0.0f;
  }
  return view.GetProperty<float>(index);
}

struct StackMeasureFirstPassResult
{
  float mainAxisNonWeight{0.0f};
  float maxCrossAxis{0.0f};
  float totalWeight{0.0f};
  uint32_t visibleChildCount{0};
};

StackMeasureFirstPassResult MeasureStackNonWeightChildren(ViewImpl::ChildContainer& children, float contentWidth,
                                                          float contentHeight, StackOrientation orientation,
                                                          const std::function<ViewImpl&(UI::View)>& getImpl)
{
  StackMeasureFirstPassResult result;
  for (auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    result.visibleChildCount++;
    float weight = GetChildWeight(childData.view);
    if (weight > 0.0f)
    {
      result.totalWeight += weight;
      continue;
    }
    float childWidthConstraint = contentWidth;
    float childHeightConstraint = contentHeight;
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    childData.measuredSize = childSize;
    Extents margin = childImpl.GetViewMargin();
    float marginW = static_cast<float>(margin.start + margin.end);
    float marginH = static_cast<float>(margin.top + margin.bottom);
    if (orientation == StackOrientation::Vertical)
    {
      result.mainAxisNonWeight += childSize.height + marginH;
      result.maxCrossAxis = std::max(result.maxCrossAxis, childSize.width + marginW);
    }
    else
    {
      result.mainAxisNonWeight += childSize.width + marginW;
      result.maxCrossAxis = std::max(result.maxCrossAxis, childSize.height + marginH);
    }
  }
  return result;
}

void MeasureStackWeightChildren(ViewImpl::ChildContainer& children, float contentMain, float contentWidth,
                                float contentHeight, float mainAxisNonWeight, float totalWeight,
                                uint32_t visibleChildCount, float spacing, StackOrientation orientation,
                                float& maxCrossAxisInOut, const std::function<ViewImpl&(UI::View)>& getImpl)
{
  float spacingTotal = (visibleChildCount > 1) ? spacing * (visibleChildCount - 1) : 0.0f;
  float remainingMain = contentMain - mainAxisNonWeight - spacingTotal;
  remainingMain = std::max(0.0f, remainingMain);
  for (auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    float weight = GetChildWeight(childData.view);
    if (weight <= 0.0f)
    {
      continue;
    }
    float share = (weight / totalWeight) * remainingMain;
    Extents margin = childImpl.GetViewMargin();
    float marginH = static_cast<float>(margin.top + margin.bottom);
    float marginW = static_cast<float>(margin.start + margin.end);
    float mainAxisConstraint = std::max(0.0f, share - (orientation == StackOrientation::Vertical ? marginH : marginW));
    float childWidthConstraint = contentWidth;
    float childHeightConstraint = contentHeight;
    if (orientation == StackOrientation::Vertical)
    {
      childHeightConstraint = mainAxisConstraint;
    }
    else
    {
      childWidthConstraint = mainAxisConstraint;
    }
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    if (orientation == StackOrientation::Vertical)
    {
      float crossSize = std::max(childSize.width, std::max(0.0f, contentWidth - marginW));
      childData.measuredSize.width = crossSize;
      childData.measuredSize.height = std::max(0.0f, share - marginH);
      maxCrossAxisInOut = std::max(maxCrossAxisInOut, crossSize + marginW);
    }
    else
    {
      float crossSize = std::max(childSize.height, std::max(0.0f, contentHeight - marginH));
      childData.measuredSize.width = std::max(0.0f, share - marginW);
      childData.measuredSize.height = crossSize;
      maxCrossAxisInOut = std::max(maxCrossAxisInOut, crossSize + marginH);
    }
  }
}

/**
 * @brief Returns the cross-axis offset for alignment (Start/Center/End).
 * Fill uses Start behavior on cross axis (size from layout dimension).
 */
float GetCrossAxisOffset(float available, float childSize, LayoutAlignment alignment)
{
  switch (alignment)
  {
    case LayoutAlignment::Center:
      return std::max(0.0f, (available - childSize) * 0.5f);
    case LayoutAlignment::End:
      return std::max(0.0f, available - childSize);
    case LayoutAlignment::Start:
    case LayoutAlignment::Fill:
    default:
      return 0.0f;
  }
}

} // namespace

StackLayoutManager::StackLayoutManager(StackOrientation orientation, float spacing)
  : LayoutManager(),
    mOrientation(orientation),
    mSpacing(spacing)
{
}

StackLayoutManager::~StackLayoutManager()
{
}

void StackLayoutManager::SetOrientation(StackOrientation orientation)
{
  mOrientation = orientation;
}

void StackLayoutManager::SetSpacing(float spacing)
{
  mSpacing = spacing;
}

MeasuredSize StackLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if (!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);
  Extents parentPadding = view->GetViewPadding();
  float contentWidth = widthConstraint - static_cast<float>(parentPadding.start + parentPadding.end);
  float contentHeight = heightConstraint - static_cast<float>(parentPadding.top + parentPadding.bottom);
  contentWidth = std::max(0.0f, contentWidth);
  contentHeight = std::max(0.0f, contentHeight);
  float contentMain = (mOrientation == StackOrientation::Vertical) ? contentHeight : contentWidth;

  auto getImpl = [this](UI::View v) -> ViewImpl& { return GetImpl(v); };
  StackMeasureFirstPassResult first =
      MeasureStackNonWeightChildren(children, contentWidth, contentHeight, mOrientation, getImpl);

  float maxCrossAxis = first.maxCrossAxis;
  float mainAxisTotal;

  if (first.totalWeight > 0.0f && first.visibleChildCount > 0)
  {
    MeasureStackWeightChildren(children, contentMain, contentWidth, contentHeight, first.mainAxisNonWeight,
                               first.totalWeight, first.visibleChildCount, mSpacing, mOrientation, maxCrossAxis,
                               getImpl);
    mainAxisTotal = contentMain;
  }
  else
  {
    mainAxisTotal = first.mainAxisNonWeight;
    if (first.visibleChildCount > 1)
    {
      mainAxisTotal += mSpacing * (first.visibleChildCount - 1);
    }
  }

  MeasuredSize totalSize(0.0f, 0.0f);
  if (mOrientation == StackOrientation::Vertical)
  {
    totalSize.width = maxCrossAxis;
    totalSize.height = mainAxisTotal;
  }
  else
  {
    totalSize.width = mainAxisTotal;
    totalSize.height = maxCrossAxis;
  }
  return totalSize;
}

MeasuredSize StackLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if (!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);

  float availableWidth = bounds.width;
  float availableHeight = bounds.height;
  float currentX = bounds.x;
  float currentY = bounds.y;
  float remainingHeight = availableHeight;
  float remainingWidth = availableWidth;

  for (auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);
    Extents margin = childImpl.GetViewMargin();
    float marginW = static_cast<float>(margin.start + margin.end);
    float marginH = static_cast<float>(margin.top + margin.bottom);
    LayoutRect childBounds;

    if (mOrientation == StackOrientation::Vertical)
    {
      // Cross axis = horizontal: apply HorizontalAlignment.
      // Fill is treated as Start: cross-axis size comes from layout dimension (MatchParent to fill).
      const float childWidth = (childImpl.GetLayoutWidth() == LayoutDimension::MatchParent)
                                   ? availableWidth
                                   : (childData.measuredSize.width + marginW);
      const LayoutAlignment crossAlign = childImpl.GetHorizontalAlignment();
      const LayoutAlignment effectiveAlign =
          (crossAlign == LayoutAlignment::Fill) ? LayoutAlignment::Start : crossAlign;
      childBounds.width = childWidth;
      childBounds.x = currentX + GetCrossAxisOffset(availableWidth, childWidth, effectiveAlign);
      childBounds.y = currentY;
      childBounds.height = (childImpl.GetLayoutHeight() == LayoutDimension::MatchParent)
                               ? remainingHeight
                               : (childData.measuredSize.height + marginH);

      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentY += childBounds.height + mSpacing;
      remainingHeight -= childBounds.height + mSpacing;
    }
    else
    {
      // Cross axis = vertical: apply VerticalAlignment.
      // Fill is treated as Start: cross-axis size comes from layout dimension (MatchParent to fill).
      childBounds.x = currentX;
      const float childHeight = (childImpl.GetLayoutHeight() == LayoutDimension::MatchParent)
                                    ? availableHeight
                                    : (childData.measuredSize.height + marginH);
      const LayoutAlignment crossAlign = childImpl.GetVerticalAlignment();
      const LayoutAlignment effectiveAlign =
          (crossAlign == LayoutAlignment::Fill) ? LayoutAlignment::Start : crossAlign;
      childBounds.height = childHeight;
      childBounds.y = currentY + GetCrossAxisOffset(availableHeight, childHeight, effectiveAlign);
      childBounds.width = (childImpl.GetLayoutWidth() == LayoutDimension::MatchParent)
                              ? remainingWidth
                              : (childData.measuredSize.width + marginW);

      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentX += childBounds.width + mSpacing;
      remainingWidth -= childBounds.width + mSpacing;
    }
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace UI
} // namespace Dali
