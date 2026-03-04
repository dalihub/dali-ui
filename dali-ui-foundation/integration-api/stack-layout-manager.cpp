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
#include <algorithm>
#include <functional>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layout/stack-layout-params-impl.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/public-api/layout-types.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

float GetChildWeight(ViewImpl& childImpl)
{
  auto* params = Internal::StackLayoutParamsImpl::Get(childImpl);
  return params ? params->GetWeight() : 0.0f;
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
                                                          const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  StackMeasureFirstPassResult result;
  for (auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    result.visibleChildCount++;
    float weight = GetChildWeight(childImpl);
    if (weight > 0.0f)
    {
      result.totalWeight += weight;
      continue;
    }
    Extents margin = childImpl.GetViewMargin();
    float marginW = static_cast<float>(margin.start + margin.end);
    float marginH = static_cast<float>(margin.top + margin.bottom);
    float childWidthConstraint = std::max(0.0f, contentWidth - marginW);
    float childHeightConstraint = std::max(0.0f, contentHeight - marginH);
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    childData.measuredSize = childSize;
    if (orientation == StackOrientation::VERTICAL)
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
                                float& maxCrossAxisInOut, const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  float spacingTotal = (visibleChildCount > 1) ? spacing * (visibleChildCount - 1) : 0.0f;
  float remainingMain = contentMain - mainAxisNonWeight - spacingTotal;
  remainingMain = std::max(0.0f, remainingMain);
  for (auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    float weight = GetChildWeight(childImpl);
    if (weight <= 0.0f)
    {
      continue;
    }
    float share = (weight / totalWeight) * remainingMain;
    Extents margin = childImpl.GetViewMargin();
    float marginH = static_cast<float>(margin.top + margin.bottom);
    float marginW = static_cast<float>(margin.start + margin.end);
    float mainAxisConstraint = std::max(0.0f, share - (orientation == StackOrientation::VERTICAL ? marginH : marginW));
    float childWidthConstraint =
        (orientation == StackOrientation::VERTICAL) ? std::max(0.0f, contentWidth - marginW) : mainAxisConstraint;
    float childHeightConstraint =
        (orientation == StackOrientation::VERTICAL) ? mainAxisConstraint : std::max(0.0f, contentHeight - marginH);
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    if (orientation == StackOrientation::VERTICAL)
    {
      childData.measuredSize.width = childSize.width;
      childData.measuredSize.height = std::max(0.0f, share - marginH);
      maxCrossAxisInOut = std::max(maxCrossAxisInOut, childSize.width + marginW);
    }
    else
    {
      childData.measuredSize.width = std::max(0.0f, share - marginW);
      childData.measuredSize.height = childSize.height;
      maxCrossAxisInOut = std::max(maxCrossAxisInOut, childSize.height + marginH);
    }
    // Sync the child's internal desired size so that OnArrange uses the
    // weighted share rather than the child's natural (possibly zero) size.
    childImpl.SetDesiredSize(childData.measuredSize);
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
  float contentMain = (mOrientation == StackOrientation::VERTICAL) ? contentHeight : contentWidth;

  auto getImpl = [this](Ui::View v) -> ViewImpl& { return GetImpl(v); };
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
  if (mOrientation == StackOrientation::VERTICAL)
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

    if (mOrientation == StackOrientation::VERTICAL)
    {
      const float crossAvailable = std::max(0.0f, availableWidth - marginW);
      const float childHeight = (childImpl.GetLayoutHeight() == LayoutDimension::MatchParent)
                                    ? std::max(0.0f, remainingHeight - marginH)
                                    : childData.measuredSize.height;
      const float slotHeight = childHeight + marginH;

      // Cross-axis (horizontal) alignment
      float childWidth =
          (childImpl.GetLayoutWidth() == LayoutDimension::MatchParent) ? crossAvailable : childData.measuredSize.width;
      float crossX = currentX + static_cast<float>(margin.start);
      LayoutAlignment hAlign = childImpl.GetHorizontalAlignment();
      switch (hAlign)
      {
        case LayoutAlignment::CENTER:
          crossX += (crossAvailable - childWidth) * 0.5f;
          break;
        case LayoutAlignment::END:
          crossX += crossAvailable - childWidth;
          break;
        case LayoutAlignment::START:
        case LayoutAlignment::FILL:
        default:
          break;
      }

      childBounds.width = childWidth;
      childBounds.height = childHeight;
      childBounds.x = crossX;
      childBounds.y = currentY + static_cast<float>(margin.top);

      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentY += slotHeight + mSpacing;
      remainingHeight -= slotHeight + mSpacing;
    }
    else
    {
      const float crossAvailable = std::max(0.0f, availableHeight - marginH);
      const float childWidth = (childImpl.GetLayoutWidth() == LayoutDimension::MatchParent)
                                   ? std::max(0.0f, remainingWidth - marginW)
                                   : childData.measuredSize.width;
      const float slotWidth = childWidth + marginW;

      // Cross-axis (vertical) alignment
      float childHeight = (childImpl.GetLayoutHeight() == LayoutDimension::MatchParent) ? crossAvailable
                                                                                        : childData.measuredSize.height;
      float crossY = currentY + static_cast<float>(margin.top);
      LayoutAlignment vAlign = childImpl.GetVerticalAlignment();
      switch (vAlign)
      {
        case LayoutAlignment::CENTER:
          crossY += (crossAvailable - childHeight) * 0.5f;
          break;
        case LayoutAlignment::END:
          crossY += crossAvailable - childHeight;
          break;
        case LayoutAlignment::START:
        case LayoutAlignment::FILL:
        default:
          break;
      }

      childBounds.width = childWidth;
      childBounds.height = childHeight;
      childBounds.x = currentX + static_cast<float>(margin.start);
      childBounds.y = crossY;

      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentX += slotWidth + mSpacing;
      remainingWidth -= slotWidth + mSpacing;
    }
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
