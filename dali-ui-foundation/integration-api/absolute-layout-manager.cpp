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
namespace UI
{
namespace Integration
{

namespace
{

LayoutRect GetChildBounds(UI::View view)
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

AbsoluteLayoutFlags GetChildFlags(UI::View view)
{
  if (view)
  {
    Property::Index index = view.GetPropertyIndex("absoluteLayoutFlags");
    if (index != Property::INVALID_INDEX)
    {
      return static_cast<AbsoluteLayoutFlags>(view.GetProperty<int>(index));
    }
  }
  return AbsoluteLayoutFlags::None;
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
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::PositionProportional)) != 0;
    bool sizeProportional =
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::SizeProportional)) != 0;

    if (positionProportional)
    {
      x *= widthConstraint;
      y *= heightConstraint;
    }

    if (sizeProportional)
    {
      w *= widthConstraint;
      h *= heightConstraint;
    }

    if (w < 0 || h < 0)
    {
      MeasuredSize childSize = childImpl.Measure(w >= 0 ? w : widthConstraint, h >= 0 ? h : heightConstraint);

      if (w < 0)
      {
        w = childSize.width;
      }
      if (h < 0)
      {
        h = childSize.height;
      }

      childData.measuredSize = MeasuredSize(w, h);
    }
    else
    {
      childData.measuredSize = MeasuredSize(w, h);
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
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::PositionProportional)) != 0;
    bool sizeProportional =
        (static_cast<uint8_t>(flags) & static_cast<uint8_t>(AbsoluteLayoutFlags::SizeProportional)) != 0;

    if (positionProportional)
    {
      x *= availableWidth;
      y *= availableHeight;
    }

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

    Extents margin = childImpl.GetViewMargin();
    float marginW = static_cast<float>(margin.start + margin.end);
    float marginH = static_cast<float>(margin.top + margin.bottom);

    LayoutRect childBounds;
    childBounds.x = bounds.x + x;
    childBounds.y = bounds.y + y;
    childBounds.width = w + marginW;
    childBounds.height = h + marginH;

    childImpl.Arrange(childBounds);
    childData.arrangedBounds = childBounds;
  }

  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace UI
} // namespace Dali
