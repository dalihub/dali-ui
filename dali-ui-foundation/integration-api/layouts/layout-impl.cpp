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
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-integ.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/layouts/layout-manager.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-trait-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{

BaseHandle Create()
{
  return Layout::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::LayoutImpl, Ui::ViewImpl, Create)
DALI_TYPE_REGISTRATION_END()

} // namespace

LayoutImplPtr LayoutImpl::New()
{
  return LayoutImplPtr(new LayoutImpl());
}

LayoutImpl::LayoutImpl()
: ViewImpl()
{
}

LayoutImpl::~LayoutImpl()
{
}

View LayoutImpl::OnFocusRequested()
{
  Ui::View self = Ui::View::DownCast(Self());

  // If descendant focus is blocked, skip children and try self only
  if(self.IsDescendantFocusBlocked())
  {
    return ViewImpl::OnFocusRequested();
  }

  // Try children first (FOCUS_AFTER_DESCENDANTS behavior)
  const auto childCount = self.GetChildCount();
  for(auto i = 0u; i < childCount; ++i)
  {
    View child = self.GetChildAt(i);
    if(child && child.IsVisible())
    {
      View resolved = GetImpl(child).RequestFocus();
      if(resolved)
      {
        return resolved;
      }
    }
  }

  // No child accepted focus, try self
  return ViewImpl::OnFocusRequested();
}

void LayoutImpl::SetLayoutManager(LayoutManager* layoutManager)
{
  DALI_ASSERT_ALWAYS(!HasLayoutManager() && "LayoutManager already set. Cannot replace an existing LayoutManager.");
  Internal::LayoutManagerTrait trait = Internal::LayoutManagerTrait::New(layoutManager);
  IntegrationView::SetTrait(*this, ReservedTraitId::LAYOUT_MANAGER, trait);
}

Internal::LayoutManagerTraitImpl* LayoutImpl::GetLayoutManagerTrait() const
{
  Internal::LayoutManagerTrait trait = IntegrationView::GetTrait<Internal::LayoutManagerTrait>(const_cast<LayoutImpl&>(*this), ReservedTraitId::LAYOUT_MANAGER);
  return trait ? &trait.GetImpl() : nullptr;
}

LayoutManager* LayoutImpl::GetLayoutManager() const
{
  Internal::LayoutManagerTraitImpl* managerTrait = GetLayoutManagerTrait();
  return managerTrait ? managerTrait->GetLayoutManager() : nullptr;
}

bool LayoutImpl::HasLayoutManager() const
{
  return GetLayoutManager() != nullptr;
}

MeasuredSize LayoutImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // widthConstraint/heightConstraint are visual (scale-applied) sizes,
  // consistent with OnArrange. Callback and return value are also visual.

  // Callback takes priority over LayoutManager
  auto* measureCallback = GetMeasureCallback();
  if(measureCallback)
  {
    Ui::View view = Ui::View::DownCast(Self());
    return measureCallback->Invoke(view, widthConstraint, heightConstraint);
  }

  LayoutManager* layoutManager = GetLayoutManager();
  if(!layoutManager)
    return ViewImpl::OnMeasure(widthConstraint, heightConstraint);

  float s = GetEffectiveScale();

  Extents padding = GetPadding();
  float   visPadW = static_cast<float>(padding.start + padding.end) * s;
  float   visPadH = static_cast<float>(padding.top + padding.bottom) * s;

  float requestedWidth  = GetRequestedWidth();
  float requestedHeight = GetRequestedHeight();

  float requestedVisW = (requestedWidth >= 0.f) ? requestedWidth * s : requestedWidth;
  float requestedVisH = (requestedHeight >= 0.f) ? requestedHeight * s : requestedHeight;
  float effectiveVisW = (requestedVisW >= 0.f) ? requestedVisW : widthConstraint;
  float effectiveVisH = (requestedVisH >= 0.f) ? requestedVisH : heightConstraint;
  float contentVisW   = std::max(0.0f, effectiveVisW - visPadW);
  float contentVisH   = std::max(0.0f, effectiveVisH - visPadH);

  MeasuredSize visContent = layoutManager->Measure(this, contentVisW, contentVisH);

  float resultVisW;
  if(requestedVisW >= 0.f)
    resultVisW = requestedVisW;
  else if(requestedWidth == MATCH_PARENT)
    resultVisW = GetMinimumWidth() * s;
  else
    resultVisW = visContent.width + visPadW;

  float resultVisH;
  if(requestedVisH >= 0.f)
    resultVisH = requestedVisH;
  else if(requestedHeight == MATCH_PARENT)
    resultVisH = GetMinimumHeight() * s;
  else
    resultVisH = visContent.height + visPadH;

  return MeasuredSize(resultVisW, resultVisH);
}

MeasuredSize LayoutImpl::OnArrange(const LayoutRect& visualBounds)
{
  // Callback takes priority over LayoutManager
  auto* arrangeCallback = GetArrangeCallback();
  if(arrangeCallback)
  {
    Actor self = Self();
    self.SetProperty(Actor::Property::POSITION_X, visualBounds.x);
    self.SetProperty(Actor::Property::POSITION_Y, visualBounds.y);
    self.SetProperty(Actor::Property::SIZE_WIDTH, visualBounds.width);
    self.SetProperty(Actor::Property::SIZE_HEIGHT, visualBounds.height);
    Ui::View view = Ui::View::DownCast(self);
    return arrangeCallback->Invoke(view, visualBounds);
  }

  LayoutManager* layoutManager = GetLayoutManager();
  if(!layoutManager)
    return ViewImpl::OnArrange(visualBounds);

  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, visualBounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, visualBounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, visualBounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, visualBounds.height);

  float   s       = GetEffectiveScale();
  Extents padding = GetPadding();

  LayoutRect visContentBounds;
  visContentBounds.x      = static_cast<float>(padding.start) * s;
  visContentBounds.y      = static_cast<float>(padding.top) * s;
  visContentBounds.width  = visualBounds.width - static_cast<float>(padding.start + padding.end) * s;
  visContentBounds.height = visualBounds.height - static_cast<float>(padding.top + padding.bottom) * s;

  layoutManager->ArrangeChildren(this, visContentBounds);

  return {visualBounds.width, visualBounds.height};
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
