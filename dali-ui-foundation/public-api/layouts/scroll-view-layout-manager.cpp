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
#include <dali-ui-foundation/public-api/layouts/scroll-view-layout-manager.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-dependency-scope.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-bar.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{

class ScrollViewLayoutManager::Impl : public LayoutManager::Impl
{
};

ScrollViewLayoutManager::ScrollViewLayoutManager()
: LayoutManager(new Impl())
{
  SetArrangePolicy(ArrangePolicy::ALWAYS);

  // This producer must execute on every arrange pass.
  //
  // Arrange() below takes the scrolled child's CURRENT ACTOR POSITION as that child's
  // arrange input (childBounds.x = child.GetPositionX() * s, and the same for y). That
  // read is exactly how a scrolled content view survives a layout pass: ScrollView
  // drives scrolling through Ui::Extension::SetPositionX/Y on its content
  // (ScrollViewImpl::ApplyScrollPosition), which writes the actor property WITHOUT
  // invalidating layout, and this manager reads it back on the next pass.
  //
  // ArrangePolicy::IF_CHANGED would let a settled ScrollView serve its children from the
  // arrange cache and re-apply the bounds published BEFORE the scroll, so the content
  // would snap back and scrolling would visibly freeze.
  // UtcDaliScrollViewScrolledContentSurvivesSettledLayoutPassP pins this.
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

  const uint32_t count = GetChildViewCount(view);

  float maxWidth  = 0.0f;
  float maxHeight = 0.0f;

  for(uint32_t i = 0; i < count; ++i)
  {
    View child = GetChildViewAt(view, i);
    if(!child || !child.GetObjectPtr())
    {
      continue;
    }

    ViewImpl& childImpl = GetImpl(child);

    if(IsStandalone(&childImpl))
    {
      continue;
    }

    bool widthIsMatchParent  = (childImpl.GetRequestedWidth() == MATCH_PARENT);
    bool heightIsMatchParent = (childImpl.GetRequestedHeight() == MATCH_PARENT);

    float childWidthConstraint  = widthIsMatchParent ? widthConstraint : std::numeric_limits<float>::max();
    float childHeightConstraint = heightIsMatchParent ? heightConstraint : std::numeric_limits<float>::max();

    MeasuredSize childSize = childImpl.Measure(childWidthConstraint, childHeightConstraint);

    float effectiveWidth  = widthIsMatchParent ? widthConstraint : childSize.width;
    float effectiveHeight = heightIsMatchParent ? heightConstraint : childSize.height;
    maxWidth              = std::max(maxWidth, effectiveWidth);
    maxHeight             = std::max(maxHeight, effectiveHeight);
  }

  return MeasuredSize(maxWidth, maxHeight);
}

void ScrollViewLayoutManager::Arrange(ViewImpl* view, const LayoutRect& bounds)
{
  if(!view)
  {
    return;
  }

  Integration::ScrollViewImpl* scrollImpl = dynamic_cast<Integration::ScrollViewImpl*>(view);
  const uint32_t               count      = GetChildViewCount(view);
  float                        s          = view->GetEffectiveScale();

  for(uint32_t i = 0; i < count; ++i)
  {
    View child = GetChildViewAt(view, i);
    if(!child || !child.GetObjectPtr())
    {
      continue;
    }

    ViewImpl& childImpl = GetImpl(child);

    if(IsStandalone(&childImpl))
    {
      continue;
    }

    MeasuredSize childMeasured = childImpl.GetMeasuredSize();
    LayoutRect   childBounds;
    childBounds.x      = child.GetPositionX() * s;
    childBounds.y      = child.GetPositionY() * s;
    childBounds.width  = childMeasured.width;
    childBounds.height = childMeasured.height;

    if(childImpl.GetRequestedWidth() == MATCH_PARENT)
    {
      childBounds.width = bounds.width;
    }
    if(childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      childBounds.height = bounds.height;
    }

    if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
    {
      Internal::LayoutDependency::ArrangeOwnedMeasureScope ownerScope(view);
      childImpl.Measure(childBounds.width, childBounds.height);
    }
    childImpl.Arrange(childBounds);

    if(scrollImpl != nullptr && child == scrollImpl->GetContent())
    {
      scrollImpl->SetScrollableWidth(childBounds.width);
      scrollImpl->SetScrollableHeight(childBounds.height);
    }
  }
}

} // namespace Ui
} // namespace Dali
