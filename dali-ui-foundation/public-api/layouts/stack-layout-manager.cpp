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
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <limits>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/stack-layout-params-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
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

bool IsChildStandalone(ViewImpl& childImpl)
{
  return childImpl.GetLayoutMode() == LayoutMode::STANDALONE;
}

struct StackMeasureFirstPassResult
{
  float    mainAxisNonWeight{0.0f};
  float    maxCrossAxis{0.0f};
  float    totalWeight{0.0f};
  uint32_t visibleChildCount{0};
};

StackMeasureFirstPassResult MeasureStackNonWeightChildren(std::vector<View>&         children,
                                                          std::vector<MeasuredSize>& workingSizes,
                                                          float                      contentWidth,
                                                          float                      contentHeight,
                                                          StackOrientation           orientation)
{
  StackMeasureFirstPassResult result;
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);

    if(IsChildStandalone(childImpl))
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
    float        childScale            = childImpl.GetEffectiveScale();
    Extents      margin                = childImpl.GetMargin();
    float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
    float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
    float        childWidthConstraint  = std::max(0.0f, contentWidth - marginW);
    float        childHeightConstraint = std::max(0.0f, contentHeight - marginH);
    MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    workingSizes[i]                    = childSize;
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

void MeasureStackWeightChildren(std::vector<View>& children, std::vector<MeasuredSize>& workingSizes,
                                float contentMain, float contentWidth,
                                float contentHeight, float mainAxisNonWeight, float totalWeight,
                                uint32_t visibleChildCount, float spacing, StackOrientation orientation,
                                float& maxCrossAxisInOut)
{
  float spacingTotal  = (visibleChildCount > 1) ? spacing * (visibleChildCount - 1) : 0.0f;
  float remainingMain = contentMain - mainAxisNonWeight - spacingTotal;
  remainingMain       = std::max(0.0f, remainingMain);

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);
    if(IsChildStandalone(childImpl))
    {
      continue;
    }
    float weight = GetChildWeight(childImpl);
    if(weight <= 0.0f)
    {
      continue;
    }

    float   share              = (weight / totalWeight) * remainingMain;
    float   childScale         = childImpl.GetEffectiveScale();
    Extents margin             = childImpl.GetMargin();
    float   marginH            = static_cast<float>(margin.top + margin.bottom) * childScale;
    float   marginW            = static_cast<float>(margin.start + margin.end) * childScale;
    float   mainAxisConstraint = std::max(0.0f, share - (orientation == StackOrientation::VERTICAL ? marginH : marginW));
    float   childWidthConstraint =
      (orientation == StackOrientation::VERTICAL) ? std::max(0.0f, contentWidth - marginW) : mainAxisConstraint;
    float childHeightConstraint =
      (orientation == StackOrientation::VERTICAL) ? mainAxisConstraint : std::max(0.0f, contentHeight - marginH);
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);
    if(orientation == StackOrientation::VERTICAL)
    {
      workingSizes[i].width  = childSize.width;
      workingSizes[i].height = std::max(0.0f, share - marginH);
      maxCrossAxisInOut      = std::max(maxCrossAxisInOut, childSize.width + marginW);
    }
    else
    {
      workingSizes[i].width  = std::max(0.0f, share - marginW);
      workingSizes[i].height = childSize.height;
      maxCrossAxisInOut      = std::max(maxCrossAxisInOut, childSize.height + marginH);
    }
  }
}

} // namespace

class StackLayoutManager::Impl : public LayoutManager::Impl
{
public:
  Impl(StackOrientation orientation, float spacing)
  : mOrientation(orientation),
    mSpacing(spacing)
  {
  }

  StackOrientation mOrientation;
  float            mSpacing;
};

StackLayoutManager::StackLayoutManager(StackOrientation orientation, float spacing)
: LayoutManager(new Impl(orientation, spacing))
{
}

StackLayoutManager::~StackLayoutManager()
{
}

void StackLayoutManager::SetOrientation(StackOrientation orientation)
{
  GetImplAs<Impl>()->mOrientation = orientation;
}

StackOrientation StackLayoutManager::GetOrientation() const
{
  return GetImplAs<Impl>()->mOrientation;
}

void StackLayoutManager::SetSpacing(float spacing)
{
  GetImplAs<Impl>()->mSpacing = spacing;
}

float StackLayoutManager::GetSpacing() const
{
  return GetImplAs<Impl>()->mSpacing;
}

MeasuredSize StackLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  float s           = view->GetEffectiveScale();
  float visSpacing  = impl->mSpacing * s;
  float contentMain = (impl->mOrientation == StackOrientation::VERTICAL) ? heightConstraint : widthConstraint;

  // Collect children once and pass to helpers (helpers run twice).
  const uint32_t    count = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  // Local working buffer (seed later from child measurements): weight
  // distribution writes allocation values here without persisting on the
  // child, so repeated layout passes do not accumulate.
  std::vector<MeasuredSize> workingSizes(children.size());

  StackMeasureFirstPassResult first = MeasureStackNonWeightChildren(
    children, workingSizes, widthConstraint, heightConstraint, impl->mOrientation);

  float maxCrossAxis = first.maxCrossAxis;
  float mainAxisTotal;

  if(first.totalWeight > 0.0f && first.visibleChildCount > 0)
  {
    float requestedMain  = (impl->mOrientation == StackOrientation::VERTICAL) ? view->GetRequestedHeight() : view->GetRequestedWidth();
    bool  isMainAxisWrap = (requestedMain != MATCH_PARENT && requestedMain < 0.0f);

    float spacingTotal = (first.visibleChildCount > 1) ? visSpacing * (first.visibleChildCount - 1) : 0.0f;
    float wrappedMain  = first.mainAxisNonWeight + spacingTotal;

    // Determine the target main-axis size for weight distribution.
    float targetMain;
    if(isMainAxisWrap)
    {
      Extents padding        = view->GetPadding();
      float   paddingMain    = (impl->mOrientation == StackOrientation::VERTICAL)
                                 ? static_cast<float>(padding.top + padding.bottom) * s
                                 : static_cast<float>(padding.start + padding.end) * s;
      float   minMain        = (impl->mOrientation == StackOrientation::VERTICAL) ? view->GetMinimumHeight() : view->GetMinimumWidth();
      float   minMainContent = std::max(0.0f, minMain * s - paddingMain);
      targetMain             = std::max(wrappedMain, minMainContent);
    }
    else
    {
      targetMain = contentMain;
    }

    if(targetMain > wrappedMain)
    {
      MeasureStackWeightChildren(children, workingSizes, targetMain, widthConstraint, heightConstraint,
                                 first.mainAxisNonWeight, first.totalWeight, first.visibleChildCount, visSpacing,
                                 impl->mOrientation, maxCrossAxis);
      mainAxisTotal = targetMain;
    }
    else
    {
      for(uint32_t i = 0; i < children.size(); ++i)
      {
        ViewImpl& childImpl = GetImpl(children[i]);
        if(IsChildStandalone(childImpl))
        {
          continue;
        }
        float weight = GetChildWeight(childImpl);
        if(weight <= 0.0f)
        {
          continue;
        }

        float        childScale            = childImpl.GetEffectiveScale();
        Extents      margin                = childImpl.GetMargin();
        float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
        float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
        float        childWidthConstraint  = std::max(0.0f, widthConstraint - marginW);
        float        childHeightConstraint = std::max(0.0f, heightConstraint - marginH);
        MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
        workingSizes[i]                    = childSize;

        if(impl->mOrientation == StackOrientation::VERTICAL)
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
      mainAxisTotal += visSpacing * (first.visibleChildCount - 1);
    }
  }

  MeasuredSize totalSize(0.0f, 0.0f);
  if(impl->mOrientation == StackOrientation::VERTICAL)
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

MeasuredSize StackLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto* impl = GetImplAs<Impl>();

  float s          = view->GetEffectiveScale();
  float visSpacing = impl->mSpacing * s;

  const uint32_t    count = GetChildViewCount(view);
  std::vector<View> children;
  children.reserve(count);
  for(uint32_t i = 0; i < count; ++i)
  {
    children.push_back(GetChildViewAt(view, i));
  }

  float availableWidth  = bounds.width;
  float availableHeight = bounds.height;
  float currentX        = bounds.x;
  float currentY        = bounds.y;

  // Local allocation buffer: seeded from each child's current measured size.
  std::vector<MeasuredSize> allocations(children.size());
  for(uint32_t i = 0; i < children.size(); ++i)
  {
    allocations[i] = GetImpl(children[i]).GetMeasuredSize();
  }

  // Re-distribute weight among weight children using actual arrange bounds.
  {
    float    totalWeight       = 0.0f;
    float    nonWeightMain     = 0.0f;
    uint32_t visibleChildCount = 0;

    for(uint32_t i = 0; i < children.size(); ++i)
    {
      ViewImpl& childImpl = GetImpl(children[i]);
      if(IsChildStandalone(childImpl))
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
        float   childScale = childImpl.GetEffectiveScale();
        Extents margin     = childImpl.GetMargin();
        float   marginM    = (impl->mOrientation == StackOrientation::VERTICAL) ? static_cast<float>(margin.top + margin.bottom) * childScale
                                                                                : static_cast<float>(margin.start + margin.end) * childScale;
        float   mainSize   = (impl->mOrientation == StackOrientation::VERTICAL) ? allocations[i].height : allocations[i].width;
        nonWeightMain += mainSize + marginM;
      }
    }

    if(totalWeight > 0.0f)
    {
      float availableMain = (impl->mOrientation == StackOrientation::VERTICAL) ? availableHeight : availableWidth;
      float spacingTotal  = (visibleChildCount > 1) ? visSpacing * (visibleChildCount - 1) : 0.0f;
      float remainingMain = std::max(0.0f, availableMain - nonWeightMain - spacingTotal);

      for(uint32_t i = 0; i < children.size(); ++i)
      {
        ViewImpl& childImpl = GetImpl(children[i]);
        if(IsChildStandalone(childImpl))
        {
          continue;
        }
        float weight = GetChildWeight(childImpl);
        if(weight <= 0.0f)
        {
          continue;
        }

        float   share      = (weight / totalWeight) * remainingMain;
        float   childScale = childImpl.GetEffectiveScale();
        Extents margin     = childImpl.GetMargin();
        float   marginW    = static_cast<float>(margin.start + margin.end) * childScale;
        float   marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;

        float childWidthConstraint;
        float childHeightConstraint;
        if(impl->mOrientation == StackOrientation::VERTICAL)
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

        if(impl->mOrientation == StackOrientation::VERTICAL)
        {
          allocations[i].width  = childSize.width;
          allocations[i].height = std::max(0.0f, share - marginH);
        }
        else
        {
          allocations[i].width  = std::max(0.0f, share - marginW);
          allocations[i].height = childSize.height;
        }
      }
    }
  }

  for(uint32_t i = 0; i < children.size(); ++i)
  {
    ViewImpl& childImpl = GetImpl(children[i]);

    if(IsChildStandalone(childImpl))
    {
      continue;
    }

    float      childScale = childImpl.GetEffectiveScale();
    Extents    margin     = childImpl.GetMargin();
    float      marginW    = static_cast<float>(margin.start + margin.end) * childScale;
    float      marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;
    LayoutRect childBounds;

    if(impl->mOrientation == StackOrientation::VERTICAL)
    {
      const float crossAvailable = std::max(0.0f, availableWidth - marginW);
      float       childHeight    = allocations[i].height;
      if(childImpl.GetRequestedHeight() == MATCH_PARENT && GetChildWeight(childImpl) <= 0.0f)
      {
        childHeight = std::max(0.0f, availableHeight - marginH);
      }
      const float slotHeight = childHeight + marginH;
      float       childWidth = allocations[i].width;
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childWidth = crossAvailable;
      }

      LayoutAlignment crossAlign = GetChildAlignment(childImpl);
      float           crossX     = currentX + static_cast<float>(margin.start) * childScale;
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
      childBounds.y      = currentY + static_cast<float>(margin.top) * childScale;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childImpl.Measure(childBounds.width, childBounds.height);
      }
      childImpl.Arrange(childBounds);

      currentY += slotHeight + visSpacing;
    }
    else
    {
      const float crossAvailable = std::max(0.0f, availableHeight - marginH);
      float       childWidth     = allocations[i].width;
      if(childImpl.GetRequestedWidth() == MATCH_PARENT && GetChildWeight(childImpl) <= 0.0f)
      {
        childWidth = std::max(0.0f, availableWidth - marginW);
      }
      const float slotWidth   = childWidth + marginW;
      float       childHeight = allocations[i].height;
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childHeight = crossAvailable;
      }

      LayoutAlignment crossAlign  = GetChildAlignment(childImpl);
      float           crossY      = currentY + static_cast<float>(margin.top) * childScale;
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
      childBounds.x      = currentX + static_cast<float>(margin.start) * childScale;
      childBounds.y      = crossY;

      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childImpl.Measure(childBounds.width, childBounds.height);
      }
      childImpl.Arrange(childBounds);

      currentX += slotWidth + visSpacing;
    }
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Ui
} // namespace Dali
