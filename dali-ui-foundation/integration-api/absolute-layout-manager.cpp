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
#include <dali-ui-foundation/integration-api/absolute-layout-manager.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/property.h>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

LayoutRect GetChildBounds(Ui::View view)
{
  LayoutRect bounds(0.0f, 0.0f, -1.0f, -1.0f);

  if (view)
  {
    Property::Index xIdx = view.GetPropertyIndex("absoluteLayoutX");
    Property::Index yIdx = view.GetPropertyIndex("absoluteLayoutY");
    Property::Index wIdx = view.GetPropertyIndex("absoluteLayoutWidth");
    Property::Index hIdx = view.GetPropertyIndex("absoluteLayoutHeight");

    if (xIdx != Property::INVALID_INDEX)
    {
      bounds.x = view.GetProperty<float>(xIdx);
    }
    if (yIdx != Property::INVALID_INDEX)
    {
      bounds.y = view.GetProperty<float>(yIdx);
    }
    if (wIdx != Property::INVALID_INDEX)
    {
      bounds.width = view.GetProperty<float>(wIdx);
    }
    if (hIdx != Property::INVALID_INDEX)
    {
      bounds.height = view.GetProperty<float>(hIdx);
    }
  }

  return bounds;
}

AbsoluteLayoutFlags GetChildFlags(Ui::View view)
{
  if (view)
  {
    Property::Index index = view.GetPropertyIndex("absoluteLayoutFlags");
    if (index != Property::INVALID_INDEX)
    {
      return static_cast<AbsoluteLayoutFlags>(view.GetProperty<int>(index));
    }
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
  if (!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);
  Extents padding = view->GetViewPadding();
  float contentWidth = widthConstraint - static_cast<float>(padding.start + padding.end);
  float contentHeight = heightConstraint - static_cast<float>(padding.top + padding.bottom);
  contentWidth = std::max(0.0f, contentWidth);
  contentHeight = std::max(0.0f, contentHeight);

  float maxRight = 0.0f;
  float maxBottom = 0.0f;

  for (auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);
    LayoutRect bounds = GetChildBounds(childData.view);
    AbsoluteLayoutFlags flags = GetChildFlags(childData.view);

    float x = bounds.x;
    float y = bounds.y;
    float w = bounds.width;
    float h = bounds.height;

    bool positionProportional =
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL)) != 0;
    bool sizeProportional =
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL)) != 0;

    if (sizeProportional)
    {
      w *= contentWidth;
      h *= contentHeight;
    }

    if (w < 0 || h < 0)
    {
      Extents margin = childImpl.GetViewMargin();
      float marginW = static_cast<float>(margin.start + margin.end);
      float marginH = static_cast<float>(margin.top + margin.bottom);
      float measureW = w >= 0.0f ? w : std::max(0.0f, contentWidth - marginW);
      float measureH = h >= 0.0f ? h : std::max(0.0f, contentHeight - marginH);
      MeasuredSize childSize = childImpl.Measure(measureW, measureH);

      if (w < 0)
      {
        w = childSize.width;
      }
      if (h < 0)
      {
        h = childSize.height;
      }
    }
    else if (childImpl.HasLayoutManager())
    {
      // Nested layout containers need Measure even with explicit size,
      // so their own children get measured.
      childImpl.Measure(w, h);
    }

    childData.measuredSize = MeasuredSize(w, h);

    // Proportional position: x = (available - childWidth) * proportion
    if (positionProportional)
    {
      x = (contentWidth - w) * bounds.x;
      y = (contentHeight - h) * bounds.y;
    }

    Extents margin = childImpl.GetViewMargin();
    maxRight = std::max(maxRight, x + w + margin.start + margin.end);
    maxBottom = std::max(maxBottom, y + h + margin.top + margin.bottom);
  }

  return MeasuredSize(maxRight, maxBottom);
}

MeasuredSize AbsoluteLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if (!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);

  float availableWidth = bounds.width;
  float availableHeight = bounds.height;

  for (auto& childData : children)
  {
    ViewImpl& childImpl = GetImpl(childData.view);
    LayoutRect childBoundsSpec = GetChildBounds(childData.view);
    AbsoluteLayoutFlags flags = GetChildFlags(childData.view);

    float x = childBoundsSpec.x;
    float y = childBoundsSpec.y;
    float w = childBoundsSpec.width;
    float h = childBoundsSpec.height;

    bool positionProportional =
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL)) != 0;
    bool sizeProportional =
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL)) != 0;

    if (sizeProportional)
    {
      w *= availableWidth;
      h *= availableHeight;
    }

    if (w < 0)
    {
      w = childData.measuredSize.width;
    }
    if (h < 0)
    {
      h = childData.measuredSize.height;
    }

    // Proportional position: x = (available - childWidth) * proportion
    if (positionProportional)
    {
      x = (availableWidth - w) * childBoundsSpec.x;
      y = (availableHeight - h) * childBoundsSpec.y;
    }

    Extents margin = childImpl.GetViewMargin();

    LayoutRect childBounds;
    childBounds.x = bounds.x + x + static_cast<float>(margin.start);
    childBounds.y = bounds.y + y + static_cast<float>(margin.top);
    childBounds.width = w;
    childBounds.height = h;

    // Sync desired size so OnArrange uses allocated bounds when the
    // child has no measured content (WrapContent with zero natural size).
    MeasuredSize desiredSize;
    desiredSize.width = (childData.measuredSize.width > 0.0f) ? childData.measuredSize.width : childBounds.width;
    desiredSize.height = (childData.measuredSize.height > 0.0f) ? childData.measuredSize.height : childBounds.height;
    childImpl.SetDesiredSize(desiredSize);

    childImpl.Arrange(childBounds);
    childData.arrangedBounds = childBounds;
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
