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
#include <dali-ui-foundation/internal/layouts/stack-layout-params-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/view-impl.h>

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
                                                          float contentHeight, StackOrientation orientation,
                                                          const std::function<ViewImpl&(Ui::View)>& getImpl)
{
  StackMeasureFirstPassResult result;
  for(auto& childData : children)
  {
    ViewImpl& childImpl = getImpl(childData.view);

    // Standalone children are measured/arranged by ViewImpl::Measure/Arrange
    // at the base level; skip them in the layout manager.
    if(childImpl.IsLayoutModeStandalone())
    {
      continue;
    }

    result.visibleChildCount++;
    float weight = GetChildWeight(childImpl);
    if(weight > 0.0f)
    {
      result.totalWeight += weight;
      continue;
    }
    Extents      margin                = childImpl.GetMargin();
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
    Extents margin             = childImpl.GetMargin();
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

  StackMeasureFirstPassResult first = MeasureStackNonWeightChildren(
    children, widthConstraint, heightConstraint, mOrientation, getImpl);

  float maxCrossAxis = first.maxCrossAxis;
  float mainAxisTotal;

  if(first.totalWeight > 0.0f && first.visibleChildCount > 0)
  {
    float requestedMain  = (mOrientation == StackOrientation::VERTICAL) ? view->GetRequestedHeight() : view->GetRequestedWidth();
    bool  isMainAxisWrap = (requestedMain != MATCH_PARENT && requestedMain < 0.0f);

    float spacingTotal = (first.visibleChildCount > 1) ? mSpacing * (first.visibleChildCount - 1) : 0.0f;
    float wrappedMain  = first.mainAxisNonWeight + spacingTotal;

    // Determine the target main-axis size for weight distribution.
    float targetMain;
    if(isMainAxisWrap)
    {
      // WRAP_CONTENT: the target is the larger of the wrapped content and
      // the view's minimum size (converted to content area by subtracting
      // padding). The full constraint is NOT used — only the minimum size
      // can create extra space for weight children.
      Extents padding        = view->GetPadding();
      float   paddingMain    = (mOrientation == StackOrientation::VERTICAL)
                                 ? static_cast<float>(padding.top + padding.bottom)
                                 : static_cast<float>(padding.start + padding.end);
      float   minMain        = (mOrientation == StackOrientation::VERTICAL) ? view->GetMinimumHeight() : view->GetMinimumWidth();
      float   minMainContent = std::max(0.0f, minMain - paddingMain);
      targetMain             = std::max(wrappedMain, minMainContent);
    }
    else
    {
      // Fixed or MATCH_PARENT: use the full constraint.
      targetMain = contentMain;
    }

    if(targetMain > wrappedMain)
    {
      // Extra space available — distribute it among weight children.
      MeasureStackWeightChildren(children, targetMain, widthConstraint, heightConstraint, first.mainAxisNonWeight,
                                 first.totalWeight, first.visibleChildCount, mSpacing, mOrientation, maxCrossAxis,
                                 getImpl);
      mainAxisTotal = targetMain;
    }
    else
    {
      // No extra space: measure weight children at their natural size
      // and accumulate normally, just like non-weight children.
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

        Extents      margin                = childImpl.GetMargin();
        float        marginW               = static_cast<float>(margin.start + margin.end);
        float        marginH               = static_cast<float>(margin.top + margin.bottom);
        float        childWidthConstraint  = std::max(0.0f, widthConstraint - marginW);
        float        childHeightConstraint = std::max(0.0f, heightConstraint - marginH);
        MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
        childData.measuredSize             = childSize;

        if(mOrientation == StackOrientation::VERTICAL)
        {
          first.mainAxisNonWeight += childSize.height + marginH;
          maxCrossAxis = std::max(maxCrossAxis, childSize.width + marginW);
        }
        else
        {
          first.mainAxisNonWeight += childSize.width + marginW;
          maxCrossAxis = std::max(maxCrossAxis, childSize.height + marginH);
        }
      }
      mainAxisTotal = first.mainAxisNonWeight + spacingTotal;
    }
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

  // Re-distribute weight among weight children using actual arrange bounds.
  // During Measure, weight distribution may have been skipped (e.g. WRAP_CONTENT
  // parent with no minSize) or computed against a different constraint. The
  // arrange bounds represent the real allocated space, so weight children must
  // be resized accordingly.
  {
    float    totalWeight       = 0.0f;
    float    nonWeightMain     = 0.0f;
    uint32_t visibleChildCount = 0;

    for(auto& childData : children)
    {
      ViewImpl& childImpl = GetImpl(childData.view);
      if(childImpl.IsLayoutModeStandalone())
      {
        continue;
      }
      visibleChildCount++;
      float weight = GetChildWeight(childImpl);
      if(weight > 0.0f)
      {
        totalWeight += weight;
      }
      else
      {
        Extents margin   = childImpl.GetMargin();
        float   marginM  = (mOrientation == StackOrientation::VERTICAL) ? static_cast<float>(margin.top + margin.bottom)
                                                                        : static_cast<float>(margin.start + margin.end);
        float   mainSize = (mOrientation == StackOrientation::VERTICAL) ? childData.measuredSize.height : childData.measuredSize.width;
        nonWeightMain += mainSize + marginM;
      }
    }

    if(totalWeight > 0.0f)
    {
      float availableMain = (mOrientation == StackOrientation::VERTICAL) ? availableHeight : availableWidth;
      float spacingTotal  = (visibleChildCount > 1) ? mSpacing * (visibleChildCount - 1) : 0.0f;
      float remainingMain = std::max(0.0f, availableMain - nonWeightMain - spacingTotal);

      for(auto& childData : children)
      {
        ViewImpl& childImpl = GetImpl(childData.view);
        if(childImpl.IsLayoutModeStandalone())
        {
          continue;
        }
        float weight = GetChildWeight(childImpl);
        if(weight <= 0.0f)
        {
          continue;
        }

        float   share   = (weight / totalWeight) * remainingMain;
        Extents margin  = childImpl.GetMargin();
        float   marginW = static_cast<float>(margin.start + margin.end);
        float   marginH = static_cast<float>(margin.top + margin.bottom);

        float childWidthConstraint;
        float childHeightConstraint;
        if(mOrientation == StackOrientation::VERTICAL)
        {
          childWidthConstraint  = std::max(0.0f, availableWidth - marginW);
          childHeightConstraint = std::max(0.0f, share - marginH);
        }
        else
        {
          childWidthConstraint  = std::max(0.0f, share - marginW);
          childHeightConstraint = std::max(0.0f, availableHeight - marginH);
        }

        MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);

        if(mOrientation == StackOrientation::VERTICAL)
        {
          childData.measuredSize.width  = childSize.width;
          childData.measuredSize.height = std::max(0.0f, share - marginH);
        }
        else
        {
          childData.measuredSize.width  = std::max(0.0f, share - marginW);
          childData.measuredSize.height = childSize.height;
        }
      }
    }
  }

  for(auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);

    // Standalone children are measured/arranged by ViewImpl::Measure/Arrange
    // at the base level; skip them in the layout manager.
    if(childImpl.IsLayoutModeStandalone())
    {
      continue;
    }

    Extents    margin  = childImpl.GetMargin();
    float      marginW = static_cast<float>(margin.start + margin.end);
    float      marginH = static_cast<float>(margin.top + margin.bottom);
    LayoutRect childBounds;

    if(mOrientation == StackOrientation::VERTICAL)
    {
      const float crossAvailable = std::max(0.0f, availableWidth - marginW);
      // MATCH_PARENT on main axis: fill the available main-axis space.
      float childHeight = childData.measuredSize.height;
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childHeight = std::max(0.0f, availableHeight - marginH);
      }
      const float slotHeight = childHeight + marginH;
      float       childWidth = childData.measuredSize.width;
      // MATCH_PARENT on cross axis: fill the available cross-axis space.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childWidth = crossAvailable;
      }

      // Cross-axis (horizontal) alignment from StackLayoutParams
      LayoutAlignment crossAlign = GetChildAlignment(childImpl);
      float           crossX     = currentX + static_cast<float>(margin.start);
      float           finalWidth = childWidth;
      switch(crossAlign)
      {
        case LayoutAlignment::FILL:
          if(childImpl.GetRequestedWidth() == WRAP_CONTENT)
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

      // Re-measure MATCH_PARENT children with their final size.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childImpl.Measure(childBounds.width, childBounds.height);
      }
      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;

      currentY += slotHeight + mSpacing;
    }
    else
    {
      const float crossAvailable = std::max(0.0f, availableHeight - marginH);
      // MATCH_PARENT on main axis: fill the available main-axis space.
      float childWidth = childData.measuredSize.width;
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childWidth = std::max(0.0f, availableWidth - marginW);
      }
      const float slotWidth   = childWidth + marginW;
      float       childHeight = childData.measuredSize.height;
      // MATCH_PARENT on cross axis: fill the available cross-axis space.
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childHeight = crossAvailable;
      }

      // Cross-axis (vertical) alignment from StackLayoutParams
      LayoutAlignment crossAlign  = GetChildAlignment(childImpl);
      float           crossY      = currentY + static_cast<float>(margin.top);
      float           finalHeight = childHeight;
      switch(crossAlign)
      {
        case LayoutAlignment::FILL:
          if(childImpl.GetRequestedHeight() == WRAP_CONTENT)
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

      // Re-measure MATCH_PARENT children with their final size.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childImpl.Measure(childBounds.width, childBounds.height);
      }
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
