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
#include <dali-ui-foundation/public-api/layouts/absolute-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/absolute-layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{
namespace Ui
{

namespace
{

LayoutRect GetChildBounds(Internal::AbsoluteLayoutParamsImpl* params)
{
  if(params)
  {
    return params->GetBounds();
  }
  return LayoutRect(0.0f, 0.0f, -1.0f, -1.0f);
}

AbsoluteLayoutFlags GetChildFlags(Internal::AbsoluteLayoutParamsImpl* params)
{
  if(params)
  {
    return params->GetFlags();
  }
  return AbsoluteLayoutFlags::NONE;
}

} // namespace

class AbsoluteLayoutManager::Impl : public LayoutManager::Impl
{
};

AbsoluteLayoutManager::AbsoluteLayoutManager()
: LayoutManager(new Impl())
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

  const uint32_t count         = GetChildCount(view);
  float          contentWidth  = widthConstraint;
  float          contentHeight = heightConstraint;

  float maxRight  = 0.0f;
  float maxBottom = 0.0f;

  for(uint32_t i = 0; i < count; ++i)
  {
    View      child     = GetChildAt(view, i);
    ViewImpl& childImpl = GetImpl(child);

    if(IsStandalone(&childImpl))
    {
      continue;
    }

    float               childScale = childImpl.GetEffectiveScale();
    auto*               params     = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
    LayoutRect          bounds     = GetChildBounds(params);
    AbsoluteLayoutFlags flags      = GetChildFlags(params);

    float x = bounds.x;
    float y = bounds.y;
    float w = bounds.width;
    float h = bounds.height;

    const uint8_t rawFlags = static_cast<uint8_t>(flags);
    const bool    xProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::X_PROPORTIONAL)) != 0;
    const bool yProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::Y_PROPORTIONAL)) != 0;
    const bool widthProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL)) != 0;
    const bool heightProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL)) != 0;

    if(widthProportional)
    {
      w *= contentWidth;
    }
    else if(w > 0.0f)
    {
      w *= childScale;
    }
    if(heightProportional)
    {
      h *= contentHeight;
    }
    else if(h > 0.0f)
    {
      h *= childScale;
    }

    Extents margin  = childImpl.GetMargin();
    float   marginW = static_cast<float>(margin.start + margin.end) * childScale;
    float   marginH = static_cast<float>(margin.top + margin.bottom) * childScale;

    // Always measure children. For negative axes (WRAP_CONTENT / MATCH_PARENT)
    // the constraint is the available content space; for explicit positive
    // bounds the constraint is the bounds value itself. Calling Measure
    // unconditionally keeps the child's measure cache and dirty state coherent
    // with the layout pass — required so a later InvalidateMeasure can
    // propagate. The child's own cache no-ops when constraints repeat.
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

    // Proportional position: axis = (available - childExtent) * proportion
    if(xProportional)
    {
      x = (contentWidth - w) * bounds.x;
    }
    else
    {
      x *= childScale;
    }
    if(yProportional)
    {
      y = (contentHeight - h) * bounds.y;
    }
    else
    {
      y *= childScale;
    }

    maxRight  = std::max(maxRight, x + w + marginW);
    maxBottom = std::max(maxBottom, y + h + marginH);
  }

  return MeasuredSize(maxRight, maxBottom);
}

MeasuredSize AbsoluteLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  const uint32_t count           = GetChildCount(view);
  float          availableWidth  = bounds.width;
  float          availableHeight = bounds.height;

  for(uint32_t i = 0; i < count; ++i)
  {
    View      child     = GetChildAt(view, i);
    ViewImpl& childImpl = GetImpl(child);

    if(IsStandalone(&childImpl))
    {
      continue;
    }

    float               childScale      = childImpl.GetEffectiveScale();
    auto*               params          = Internal::AbsoluteLayoutParamsImpl::Get(childImpl);
    LayoutRect          childBoundsSpec = GetChildBounds(params);
    AbsoluteLayoutFlags flags           = GetChildFlags(params);

    float x = childBoundsSpec.x;
    float y = childBoundsSpec.y;
    float w = childBoundsSpec.width;
    float h = childBoundsSpec.height;

    const uint8_t rawFlags = static_cast<uint8_t>(flags);
    const bool    xProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::X_PROPORTIONAL)) != 0;
    const bool yProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::Y_PROPORTIONAL)) != 0;
    const bool widthProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL)) != 0;
    const bool heightProportional =
      (rawFlags & static_cast<uint8_t>(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL)) != 0;

    if(widthProportional)
    {
      w *= availableWidth;
    }
    else if(w > 0.0f)
    {
      w *= childScale;
    }
    if(heightProportional)
    {
      h *= availableHeight;
    }
    else if(h > 0.0f)
    {
      h *= childScale;
    }

    if(w < 0 || h < 0)
    {
      // For MATCH_PARENT axes we recompute from available space here; for
      // WRAP_CONTENT axes we reuse the natural size already recorded on the
      // child by Measure().
      MeasuredSize childMeasured = childImpl.GetMeasuredSize();
      if(w < 0)
      {
        if(childImpl.GetRequestedWidth() == MATCH_PARENT)
        {
          Extents margin = childImpl.GetMargin();
          w              = std::max(0.0f, availableWidth - static_cast<float>(margin.start + margin.end) * childScale);
        }
        else
        {
          w = childMeasured.width;
        }
      }
      if(h < 0)
      {
        if(childImpl.GetRequestedHeight() == MATCH_PARENT)
        {
          Extents margin = childImpl.GetMargin();
          h              = std::max(0.0f, availableHeight - static_cast<float>(margin.top + margin.bottom) * childScale);
        }
        else
        {
          h = childMeasured.height;
        }
      }
    }

    // Proportional position: axis = (available - childExtent) * proportion
    if(xProportional)
    {
      x = (availableWidth - w) * childBoundsSpec.x;
    }
    else
    {
      x *= childScale;
    }
    if(yProportional)
    {
      y = (availableHeight - h) * childBoundsSpec.y;
    }
    else
    {
      y *= childScale;
    }

    Extents margin = childImpl.GetMargin();

    LayoutRect childBounds;
    childBounds.x      = bounds.x + x + static_cast<float>(margin.start) * childScale;
    childBounds.y      = bounds.y + y + static_cast<float>(margin.top) * childScale;
    childBounds.width  = w;
    childBounds.height = h;

    // Re-measure MATCH_PARENT children with their final (scaled) size.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childImpl.Measure(w, h);
    }
    childImpl.Arrange(childBounds);
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Ui
} // namespace Dali
