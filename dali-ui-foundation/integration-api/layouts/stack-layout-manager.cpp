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
#include <dali-ui-foundation/integration-api/layouts/stack-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <functional>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/layouts/stack-layout-params-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

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

LayoutAlignment GetChildAlignment(ViewImpl& childImpl)
{
  auto* params = Internal::StackLayoutParamsImpl::Get(childImpl);
  return params ? params->GetAlignment() : LayoutAlignment::START;
}

struct StackMeasureFirstPassResult
{
  float    mainAxisNonWeight{0.0f};
  float    maxCrossAxis{0.0f};
  float    totalWeight{0.0f};
  uint32_t visibleChildCount{0};
};

StackMeasureFirstPassResult MeasureStackNonWeightChildren(ViewImpl::ChildContainer& children, float contentWidth,
                                                          float contentHeight, float parentWidth,
                                                          float parentHeight, StackOrientation orientation,
                                                          const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  StackMeasureFirstPassResult result;
  for(auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);

    // Standalone children: still measured (so MATCH_PARENT / WRAP_CONTENT resolve),
    // but excluded from this layout's main-axis accumulation, cross-axis maximum,
    // visible-child count and weight distribution. They also ignore the parent's
    // padding entirely, so they are sized against the parent's full inner size.
    if(childImpl.IsLayoutModeStandalone())
    {
      Extents margin         = childImpl.GetViewMargin();
      float   marginW        = static_cast<float>(margin.start + margin.end);
      float   marginH        = static_cast<float>(margin.top + margin.bottom);
      float   childW         = std::max(0.0f, parentWidth - marginW);
      float   childH         = std::max(0.0f, parentHeight - marginH);
      childData.measuredSize = childImpl.Measure(childW, childH);
      continue;
    }

    result.visibleChildCount++;
    float weight = GetChildWeight(childImpl);
    if(weight > 0.0f)
    {
      result.totalWeight += weight;
      continue;
    }
    Extents      margin                = childImpl.GetViewMargin();
    float        marginW               = static_cast<float>(margin.start + margin.end);
    float        marginH               = static_cast<float>(margin.top + margin.bottom);
    float        childWidthConstraint  = std::max(0.0f, contentWidth - marginW);
    float        childHeightConstraint = std::max(0.0f, contentHeight - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    childData.measuredSize             = childSize;
    if(orientation == StackOrientation::VERTICAL)
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
  float spacingTotal  = (visibleChildCount > 1) ? spacing * (visibleChildCount - 1) : 0.0f;
  float remainingMain = contentMain - mainAxisNonWeight - spacingTotal;
  remainingMain       = std::max(0.0f, remainingMain);

  for(auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);
    if(childImpl.IsLayoutModeStandalone())
    {
      continue;
    }
    float weight = GetChildWeight(childImpl);
    if(weight <= 0.0f)
    {
      continue;
    }

    float   share              = (weight / totalWeight) * remainingMain;
    Extents margin             = childImpl.GetViewMargin();
    float   marginH            = static_cast<float>(margin.top + margin.bottom);
    float   marginW            = static_cast<float>(margin.start + margin.end);
    float   mainAxisConstraint = std::max(0.0f, share - (orientation == StackOrientation::VERTICAL ? marginH : marginW));
    float   childWidthConstraint =
      (orientation == StackOrientation::VERTICAL) ? std::max(0.0f, contentWidth - marginW) : mainAxisConstraint;
    float childHeightConstraint =
      (orientation == StackOrientation::VERTICAL) ? mainAxisConstraint : std::max(0.0f, contentHeight - marginH);
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    if(orientation == StackOrientation::VERTICAL)
    {
      childData.measuredSize.width  = childSize.width;
      childData.measuredSize.height = std::max(0.0f, share - marginH);
      maxCrossAxisInOut             = std::max(maxCrossAxisInOut, childSize.width + marginW);
    }
    else
    {
      childData.measuredSize.width  = std::max(0.0f, share - marginW);
      childData.measuredSize.height = childSize.height;
      maxCrossAxisInOut             = std::max(maxCrossAxisInOut, childSize.height + marginH);
    }
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

StackOrientation StackLayoutManager::GetOrientation() const
{
  return mOrientation;
}

void StackLayoutManager::SetSpacing(float spacing)
{
  mSpacing = spacing;
}

float StackLayoutManager::GetSpacing() const
{
  return mSpacing;
}

MeasuredSize StackLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children    = GetChildren(view);
  float contentMain = (mOrientation == StackOrientation::VERTICAL) ? heightConstraint : widthConstraint;

  auto getImpl = [this](Ui::View v) -> ViewImpl&
  { return GetImpl(v); };

  Extents parentPadding = view->GetViewPadding();
  float   parentWidth   = widthConstraint + static_cast<float>(parentPadding.start + parentPadding.end);
  float   parentHeight  = heightConstraint + static_cast<float>(parentPadding.top + parentPadding.bottom);

  StackMeasureFirstPassResult first = MeasureStackNonWeightChildren(
    children, widthConstraint, heightConstraint, parentWidth, parentHeight, mOrientation, getImpl);

  float maxCrossAxis = first.maxCrossAxis;
  float mainAxisTotal;

  if(first.totalWeight > 0.0f && first.visibleChildCount > 0)
  {
    MeasureStackWeightChildren(children, contentMain, widthConstraint, heightConstraint, first.mainAxisNonWeight,
                               first.totalWeight, first.visibleChildCount, mSpacing, mOrientation, maxCrossAxis,
                               getImpl);
    mainAxisTotal = contentMain;
  }
  else
  {
    mainAxisTotal = first.mainAxisNonWeight;
    if(first.visibleChildCount > 1)
    {
      mainAxisTotal += mSpacing * (first.visibleChildCount - 1);
    }
  }

  MeasuredSize totalSize(0.0f, 0.0f);
  if(mOrientation == StackOrientation::VERTICAL)
  {
    totalSize.width  = maxCrossAxis;
    totalSize.height = mainAxisTotal;
  }
  else
  {
    totalSize.width  = mainAxisTotal;
    totalSize.height = maxCrossAxis;
  }
  return totalSize;
}

MeasuredSize StackLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);

  float availableWidth  = bounds.width;
  float availableHeight = bounds.height;
  float currentX        = bounds.x;
  float currentY        = bounds.y;

  for(auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);

    // Standalone children: place at their RequestedPositionX/Y in the parent's
    // coordinate space (ignoring parent padding) plus the child's own margin,
    // and use the size resolved during Measure. They do not advance the stack
    // cursor and are excluded from spacing.
    if(childImpl.IsLayoutModeStandalone())
    {
      Extents    standaloneMargin = childImpl.GetViewMargin();
      LayoutRect standaloneBounds(childImpl.GetPositionX() + static_cast<float>(standaloneMargin.start),
                                  childImpl.GetPositionY() + static_cast<float>(standaloneMargin.top),
                                  childData.measuredSize.width,
                                  childData.measuredSize.height);
      childImpl.Arrange(standaloneBounds);
      childData.arrangedBounds = standaloneBounds;
      continue;
    }

    Extents    margin  = childImpl.GetViewMargin();
    float      marginW = static_cast<float>(margin.start + margin.end);
    float      marginH = static_cast<float>(margin.top + margin.bottom);
    LayoutRect childBounds;

    if(mOrientation == StackOrientation::VERTICAL)
    {
      const float crossAvailable = std::max(0.0f, availableWidth - marginW);
      const float childHeight    = childData.measuredSize.height;
      const float slotHeight     = childHeight + marginH;
      const float childWidth     = childData.measuredSize.width;

      // Cross-axis (horizontal) alignment from StackLayoutParams
      LayoutAlignment crossAlign = GetChildAlignment(childImpl);
      float           crossX     = currentX + static_cast<float>(margin.start);
      float           finalWidth = childWidth;
      switch(crossAlign)
      {
        case LayoutAlignment::FILL:
          if(childImpl.GetRequestedWidth() <= 0.0f)
          {
            finalWidth = crossAvailable;
          }
          break;
        case LayoutAlignment::CENTER:
          crossX += (crossAvailable - childWidth) * 0.5f;
          break;
        case LayoutAlignment::END:
          crossX += crossAvailable - childWidth;
          break;
        case LayoutAlignment::START:
        default:
          break;
      }

      childBounds.width  = finalWidth;
      childBounds.height = childHeight;
      childBounds.x      = crossX;
      childBounds.y      = currentY + static_cast<float>(margin.top);

      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentY += slotHeight + mSpacing;
    }
    else
    {
      const float crossAvailable = std::max(0.0f, availableHeight - marginH);
      const float childWidth     = childData.measuredSize.width;
      const float slotWidth      = childWidth + marginW;
      const float childHeight    = childData.measuredSize.height;

      // Cross-axis (vertical) alignment from StackLayoutParams
      LayoutAlignment crossAlign  = GetChildAlignment(childImpl);
      float           crossY      = currentY + static_cast<float>(margin.top);
      float           finalHeight = childHeight;
      switch(crossAlign)
      {
        case LayoutAlignment::FILL:
          if(childImpl.GetRequestedHeight() <= 0.0f)
          {
            finalHeight = crossAvailable;
          }
          break;
        case LayoutAlignment::CENTER:
          crossY += (crossAvailable - childHeight) * 0.5f;
          break;
        case LayoutAlignment::END:
          crossY += crossAvailable - childHeight;
          break;
        case LayoutAlignment::START:
        default:
          break;
      }

      childBounds.width  = childWidth;
      childBounds.height = finalHeight;
      childBounds.x      = currentX + static_cast<float>(margin.start);
      childBounds.y      = crossY;

      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentX += slotWidth + mSpacing;
    }
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
