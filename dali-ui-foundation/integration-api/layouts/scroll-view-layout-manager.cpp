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
#include <dali-ui-foundation/integration-api/layouts/scroll-view-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

ScrollViewLayoutManager::ScrollViewLayoutManager()
: LayoutManager()
{
}

ScrollViewLayoutManager::~ScrollViewLayoutManager()
{
}

MeasuredSize ScrollViewLayoutManager::Measure(ViewImpl* view, float widthConstraint, float heightConstraint)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  auto& children = GetChildren(view);

  // For ScrollView, we want to allow content to have its natural size
  // without being constrained by the parent size
  float maxWidth  = 0.0f;
  float maxHeight = 0.0f;

  auto getImpl = [](Ui::View v) -> ViewImpl&
  {
    return GetImpl(v);
  };

  for(auto& childData : children)
  {
    if(!childData)
    {
      continue;
    }

    // Check if the view handle is valid
    if(!childData.GetObjectPtr())
    {
      continue;
    }

    ViewImpl& childImpl = getImpl(childData);

    // Check if child is using MatchParent for width or height
    bool widthIsMatchParent  = (childImpl.GetRequestedWidth() == MATCH_PARENT);
    bool heightIsMatchParent = (childImpl.GetRequestedHeight() == MATCH_PARENT);

    // For ScrollView, we allow children to have their natural size
    // unless they explicitly use MatchParent
    float childWidthConstraint  = widthIsMatchParent ? widthConstraint : std::numeric_limits<float>::max();
    float childHeightConstraint = heightIsMatchParent ? heightConstraint : std::numeric_limits<float>::max();

    // Measure the child with appropriate constraints
    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);

    // For ScrollView, the measured size represents the content size which can be larger than viewport.
    // MATCH_PARENT children fill the viewport, so use the constraint as their contribution.
    float effectiveWidth  = widthIsMatchParent ? widthConstraint : childSize.width;
    float effectiveHeight = heightIsMatchParent ? heightConstraint : childSize.height;
    maxWidth              = std::max(maxWidth, effectiveWidth);
    maxHeight             = std::max(maxHeight, effectiveHeight);
  }

  return MeasuredSize(maxWidth, maxHeight);
}

MeasuredSize ScrollViewLayoutManager::ArrangeChildren(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return MeasuredSize(0.0f, 0.0f);
  }

  ScrollViewImpl* scrollImpl = dynamic_cast<ScrollViewImpl*>(view);
  auto&           children   = GetChildren(view);

  // In ScrollView, children are arranged at position (0,0) with their measured size
  // The ScrollView will handle the scrolling/positioning of the content
  for(auto& childData : children)
  {
    if(!childData)
    {
      continue;
    }
    ViewImpl& childImpl = GetImpl(childData);

    LayoutRect childBounds;

    // content인 경우만
    // Read measured size directly from the child (set during MeasureChildren).
    MeasuredSize childMeasured = childImpl.GetMeasuredSize();
    childBounds.x              = childData.GetPositionX();
    childBounds.y              = childData.GetPositionY();
    childBounds.width          = childMeasured.width;
    childBounds.height         = childMeasured.height;

    // MATCH_PARENT: fill the viewport.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT)
    {
      childBounds.width = bounds.width;
    }
    if(childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childBounds.height = bounds.height;
    }

    // Re-measure MATCH_PARENT children with their final size.
    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childImpl.Measure(childBounds.width, childBounds.height);
    }
    // Arrange the child
    childImpl.Arrange(childBounds);

    if(scrollImpl != nullptr)
    {
      scrollImpl->SetScrollableWidth(childBounds.width);
      scrollImpl->SetScrollableHeight(childBounds.height);
    }
  }

  // Return the bounds size, which represents the viewport size
  return MeasuredSize(bounds.width, bounds.height);
}

} // namespace Integration
} // namespace Ui
} // namespace Dali