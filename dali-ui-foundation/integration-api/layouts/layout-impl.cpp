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
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/actors/actor.h>

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

void LayoutImpl::SetLayoutManager(LayoutManager* layoutManager)
{
  DALI_ASSERT_ALWAYS(!HasLayoutManager() && "LayoutManager already set. Cannot replace an existing LayoutManager.");
  Internal::LayoutManagerTrait trait = Internal::LayoutManagerTrait::New(layoutManager);
  SetTrait(ReservedTraitId::LAYOUT_MANAGER, trait);
}

Internal::LayoutManagerTraitImpl* LayoutImpl::GetLayoutManagerTrait() const
{
  Trait trait = const_cast<LayoutImpl*>(this)->GetTrait(ReservedTraitId::LAYOUT_MANAGER);
  if(trait)
  {
    return static_cast<Internal::LayoutManagerTraitImpl*>(&Ui::GetImpl(trait));
  }
  return nullptr;
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

bool LayoutImpl::IsLayout() const
{
  return true;
}

MeasuredSize LayoutImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // Callback takes priority over LayoutManager
  auto* measureCallback = GetMeasureCallback();
  if(measureCallback)
  {
    Ui::View view = Ui::View::DownCast(Self());
    return measureCallback->Invoke(view, widthConstraint, heightConstraint);
  }

  LayoutManager* layoutManager = GetLayoutManager();
  if(!layoutManager)
  {
    return ViewImpl::OnMeasure(widthConstraint, heightConstraint);
  }

  Extents padding = GetPadding();
  float   pw      = static_cast<float>(padding.start + padding.end);
  float   ph      = static_cast<float>(padding.top + padding.bottom);

  float requestedWidth  = GetRequestedWidth();
  float requestedHeight = GetRequestedHeight();

  float effectiveWidth  = (requestedWidth >= 0) ? requestedWidth : widthConstraint;
  float effectiveHeight = (requestedHeight >= 0) ? requestedHeight : heightConstraint;

  float contentWidth  = std::max(0.0f, effectiveWidth - pw);
  float contentHeight = std::max(0.0f, effectiveHeight - ph);

  MeasuredSize content = layoutManager->Measure(this, contentWidth, contentHeight);

  float resultWidth;
  if(requestedWidth >= 0)
  {
    resultWidth = requestedWidth;
  }
  else if(requestedWidth == MATCH_PARENT)
  {
    // MATCH_PARENT: report minimum desired size; actual size is determined
    // by the parent during the Arrange phase.
    resultWidth = GetMinimumWidth();
  }
  else
  {
    resultWidth = content.width + pw;
  }

  float resultHeight;
  if(requestedHeight >= 0)
  {
    resultHeight = requestedHeight;
  }
  else if(requestedHeight == MATCH_PARENT)
  {
    resultHeight = GetMinimumHeight();
  }
  else
  {
    resultHeight = content.height + ph;
  }

  return MeasuredSize(resultWidth, resultHeight);
}

MeasuredSize LayoutImpl::OnArrange(const LayoutRect& bounds)
{
  // Callback takes priority over LayoutManager
  auto* arrangeCallback = GetArrangeCallback();
  if(arrangeCallback)
  {
    // Set self position/size before invoking callback
    Actor self = Self();
    self.SetProperty(Actor::Property::POSITION_X, bounds.x);
    self.SetProperty(Actor::Property::POSITION_Y, bounds.y);
    self.SetProperty(Actor::Property::SIZE_WIDTH, bounds.width);
    self.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);

    Ui::View view = Ui::View::DownCast(self);
    return arrangeCallback->Invoke(view, bounds);
  }

  LayoutManager* layoutManager = GetLayoutManager();
  if(!layoutManager)
  {
    return ViewImpl::OnArrange(bounds);
  }

  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, bounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, bounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, bounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);

  Extents    padding = GetPadding();
  LayoutRect contentBounds;
  contentBounds.x      = static_cast<float>(padding.start);
  contentBounds.y      = static_cast<float>(padding.top);
  contentBounds.width  = bounds.width - static_cast<float>(padding.start + padding.end);
  contentBounds.height = bounds.height - static_cast<float>(padding.top + padding.bottom);

  layoutManager->ArrangeChildren(this, contentBounds);

  return {bounds.width, bounds.height};
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
