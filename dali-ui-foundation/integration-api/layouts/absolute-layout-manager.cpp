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
#include <dali-ui-foundation/integration-api/layouts/absolute-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/layouts/absolute-layout-params-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

LayoutRect GetChildBounds(ViewImpl& childImpl)
{
  auto* params = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
  if(params)
  {
    return params->GetBounds();
  }
  return LayoutRect(0.0f, 0.0f, -1.0f, -1.0f);
}

AbsoluteLayoutFlags GetChildFlags(ViewImpl& childImpl)
{
  auto* params = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
  if(params)
  {
    return params->GetFlags();
  }
  return AbsoluteLayoutFlags::NONE;
}

} // namespace

AbsoluteLayoutManager::AbsoluteLayoutManager()
{
}

AbsoluteLayoutManager::~AbsoluteLayoutManager()
{
}

MeasuredSize AbsoluteLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children      = GetChildren(view);
  float contentWidth  = widthConstraint;
  float contentHeight = heightConstraint;

  float maxRight  = 0.0f;
  float maxBottom = 0.0f;

  for(auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);

    // Standalone children are measured/arranged by ViewImpl::Measure/Arrange
    // at the base level; skip them in the layout manager.
    if(childImpl.IsLayoutModeStandalone())
    {
      continue;
    }

    LayoutRect          bounds = GetChildBounds(childImpl);
    AbsoluteLayoutFlags flags  = GetChildFlags(childImpl);

    float x = bounds.x;
    float y = bounds.y;
    float w = bounds.width;
    float h = bounds.height;

    bool positionProportional =
      (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL)) != 0;
    bool sizeProportional =
      (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL)) != 0;

    if(sizeProportional)
    {
      w *= contentWidth;
      h *= contentHeight;
    }

    if(w < 0 || h < 0)
    {
      Extents      margin    = childImpl.GetViewMargin();
      float        marginW   = static_cast<float>(margin.start + margin.end);
      float        marginH   = static_cast<float>(margin.top + margin.bottom);
      float        measureW  = w >= 0.0f ? w : std::max(0.0f, contentWidth - marginW);
      float        measureH  = h >= 0.0f ? h : std::max(0.0f, contentHeight - marginH);
      MeasuredSize childSize = childImpl.Measure(measureW, measureH);

      if(w < 0)
      {
        w = childSize.width;
      }
      if(h < 0)
      {
        h = childSize.height;
      }
    }
    else if(childImpl.IsLayout())
    {
      // Nested layout containers need Measure even with explicit size,
      // so their own children get measured.
      childImpl.Measure(w, h);
    }

    childData.measuredSize = MeasuredSize(w, h);

    // Proportional position: x = (available - childWidth) * proportion
    if(positionProportional)
    {
      x = (contentWidth - w) * bounds.x;
      y = (contentHeight - h) * bounds.y;
    }

    Extents margin = childImpl.GetViewMargin();
    maxRight       = std::max(maxRight, x + w + margin.start + margin.end);
    maxBottom      = std::max(maxBottom, y + h + margin.top + margin.bottom);
  }

  return MeasuredSize(maxRight, maxBottom);
}

MeasuredSize AbsoluteLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);

  float availableWidth  = bounds.width;
  float availableHeight = bounds.height;

  for(auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);

    // Standalone children are measured/arranged by ViewImpl::Measure/Arrange
    // at the base level; skip them in the layout manager.
    if(childImpl.IsLayoutModeStandalone())
    {
      continue;
    }

    LayoutRect          childBoundsSpec = GetChildBounds(childImpl);
    AbsoluteLayoutFlags flags           = GetChildFlags(childImpl);

    float x = childBoundsSpec.x;
    float y = childBoundsSpec.y;
    float w = childBoundsSpec.width;
    float h = childBoundsSpec.height;

    bool positionProportional =
      (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL)) != 0;
    bool sizeProportional =
      (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL)) != 0;

    if(sizeProportional)
    {
      w *= availableWidth;
      h *= availableHeight;
    }

    if(w < 0)
    {
      // MATCH_PARENT: fill the available width minus own margin.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        Extents margin = childImpl.GetViewMargin();
        w              = std::max(0.0f, availableWidth - static_cast<float>(margin.start + margin.end));
      }
      else
      {
        w = childData.measuredSize.width;
      }
    }
    if(h < 0)
    {
      // MATCH_PARENT: fill the available height minus own margin.
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        Extents margin = childImpl.GetViewMargin();
        h              = std::max(0.0f, availableHeight - static_cast<float>(margin.top + margin.bottom));
      }
      else
      {
        h = childData.measuredSize.height;
      }
    }

    // Proportional position: x = (available - childWidth) * proportion
    if(positionProportional)
    {
      x = (availableWidth - w) * childBoundsSpec.x;
      y = (availableHeight - h) * childBoundsSpec.y;
    }

    Extents margin = childImpl.GetViewMargin();

    LayoutRect childBounds;
    childBounds.x      = bounds.x + x + static_cast<float>(margin.start);
    childBounds.y      = bounds.y + y + static_cast<float>(margin.top);
    childBounds.width  = w;
    childBounds.height = h;

    // Re-measure MATCH_PARENT children with their final size.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childImpl.Measure(w, h);
    }
    childImpl.Arrange(childBounds);
    childData.arrangedBounds = childBounds;
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
