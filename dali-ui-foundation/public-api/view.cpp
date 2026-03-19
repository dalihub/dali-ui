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
#include <dali-ui-foundation/public-api/view.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/control-accessible.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>
#include <dali-ui-foundation/public-api/clickable-trait.h>
#include <dali-ui-foundation/public-api/layout.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{

namespace Ui
{

View::View()
{
}

View View::New()
{
  Integration::ViewImplPtr impl = Integration::ViewImpl::New();

  // Pass ownership to handle
  View handle(*impl);

  impl->Initialize();

  return handle;
}

View::View(const View& view) = default;

View::View(View&& rhs) noexcept = default;

View::~View()
{
}

View View::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<View, Integration::ViewImpl>(handle);
}

View::View(Integration::ViewImpl& implementation)
: CustomActor(implementation)
{
}

View::View(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<Integration::ViewImpl>(internal);
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize View::Measure(float widthConstraint, float heightConstraint)
{
  return Integration::GetImpl(*this).Measure(widthConstraint, heightConstraint);
}

MeasuredSize View::Arrange(const LayoutRect& bounds)
{
  return Integration::GetImpl(*this).Arrange(bounds);
}

void View::InvalidateMeasure()
{
  Integration::GetImpl(*this).InvalidateMeasure();
}

void View::InvalidateArrange()
{
  Integration::GetImpl(*this).InvalidateArrange();
}

MeasuredSize View::GetDesiredSize() const
{
  return Integration::GetImpl(*this).GetDesiredSize();
}

bool View::IsMeasureValid() const
{
  return Integration::GetImpl(*this).IsMeasureValid();
}

bool View::IsArrangeValid() const
{
  return Integration::GetImpl(*this).IsArrangeValid();
}

// =============================================================================
// Properties
// =============================================================================

View& View::SetSizeWidth(float width)
{
  Integration::GetImpl(*this).SetSizeWidth(width);
  return *this;
}

float View::GetSizeWidth() const
{
  return Integration::GetImpl(*this).GetSizeWidth();
}

View& View::SetSizeHeight(float height)
{
  Integration::GetImpl(*this).SetSizeHeight(height);
  return *this;
}

float View::GetSizeHeight() const
{
  return Integration::GetImpl(*this).GetSizeHeight();
}

View& View::SetPositionX(float x)
{
  Integration::GetImpl(*this).SetPositionX(x);
  return *this;
}

float View::GetPositionX() const
{
  return Integration::GetImpl(*this).GetPositionX();
}

View& View::SetPositionY(float y)
{
  Integration::GetImpl(*this).SetPositionY(y);
  return *this;
}

float View::GetPositionY() const
{
  return Integration::GetImpl(*this).GetPositionY();
}

View& View::SetParentOrigin(const Vector3& point)
{
  Integration::GetImpl(*this).SetParentOrigin(point);
  return *this;
}

Vector3 View::GetParentOrigin() const
{
  return Integration::GetImpl(*this).GetParentOrigin();
}

View& View::SetPivotPoint(const Vector3& point)
{
  Integration::GetImpl(*this).SetPivotPoint(point);
  return *this;
}

Vector3 View::GetPivotPoint() const
{
  return Integration::GetImpl(*this).GetPivotPoint();
}

View& View::SetLayoutWidth(float width)
{
  Integration::GetImpl(*this).SetLayoutWidth(width);
  return *this;
}

float View::GetLayoutWidth() const
{
  return Integration::GetImpl(*this).GetLayoutWidth();
}

View& View::SetLayoutHeight(float height)
{
  Integration::GetImpl(*this).SetLayoutHeight(height);
  return *this;
}

float View::GetLayoutHeight() const
{
  return Integration::GetImpl(*this).GetLayoutHeight();
}

View& View::SetMinimumWidth(float width)
{
  Integration::GetImpl(*this).SetMinimumWidth(width);
  return *this;
}

float View::GetMinimumWidth() const
{
  return Integration::GetImpl(*this).GetMinimumWidth();
}

View& View::SetMinimumHeight(float height)
{
  Integration::GetImpl(*this).SetMinimumHeight(height);
  return *this;
}

float View::GetMinimumHeight() const
{
  return Integration::GetImpl(*this).GetMinimumHeight();
}

View& View::SetMaximumWidth(float width)
{
  Integration::GetImpl(*this).SetMaximumWidth(width);
  return *this;
}

float View::GetMaximumWidth() const
{
  return Integration::GetImpl(*this).GetMaximumWidth();
}

View& View::SetMaximumHeight(float height)
{
  Integration::GetImpl(*this).SetMaximumHeight(height);
  return *this;
}

float View::GetMaximumHeight() const
{
  return Integration::GetImpl(*this).GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

View& View::SetViewMargin(const Extents& margin)
{
  Integration::GetImpl(*this).SetViewMargin(margin);
  return *this;
}

Extents View::GetViewMargin() const
{
  return Integration::GetImpl(*this).GetViewMargin();
}

View& View::SetViewPadding(const Extents& padding)
{
  Integration::GetImpl(*this).SetViewPadding(padding);
  return *this;
}

Extents View::GetViewPadding() const
{
  return Integration::GetImpl(*this).GetViewPadding();
}

View& View::SetHorizontalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetHorizontalAlignment(alignment);
  return *this;
}

LayoutAlignment View::GetHorizontalAlignment() const
{
  return Integration::GetImpl(*this).GetHorizontalAlignment();
}

View& View::SetVerticalAlignment(LayoutAlignment alignment)
{
  Integration::GetImpl(*this).SetVerticalAlignment(alignment);
  return *this;
}

LayoutAlignment View::GetVerticalAlignment() const
{
  return Integration::GetImpl(*this).GetVerticalAlignment();
}

bool View::IsFocusable() const
{
  return Integration::GetImpl(*this).IsFocusable();
}

View& View::SetFocusable(bool focusable)
{
  Integration::GetImpl(*this).SetFocusable(focusable);
  return *this;
}

bool View::IsTouchFocusable() const
{
  return Integration::GetImpl(*this).IsTouchFocusable();
}

View& View::SetTouchFocusable(bool touchFocusable)
{
  Integration::GetImpl(*this).SetTouchFocusable(touchFocusable);
  return *this;
}

UiColor View::GetBackgroundColor()
{
  return Integration::GetImpl(*this).GetBackgroundColor();
}

View& View::SetBackgroundColor(const UiColor& color)
{
  Integration::GetImpl(*this).SetBackgroundColor(color);
  return *this;
}

ClickableTrait View::GetOrAttachClickableTrait()
{
  auto&                      impl = Integration::GetImpl(*this);
  const Integration::TraitId interactionTraitId(Integration::ReservedTraitId::INTERACTION_TRAIT);
  Trait                      existing = impl.GetTrait(interactionTraitId);

  if(!existing)
  {
    ClickableTrait clickable = ClickableTrait::New();
    impl.SetTrait(interactionTraitId, clickable);
    return clickable;
  }

  ClickableTrait clickable = ClickableTrait::DownCast(existing);
  if(!clickable)
  {
    DALI_ASSERT_ALWAYS(false && "View already has a different interaction trait; cannot attach ClickableTrait");
    return ClickableTrait();
  }

  return clickable;
}

ClickableTrait View::GetClickableTrait() const
{
  const auto& impl  = Integration::GetImpl(*this);
  Trait       trait = impl.GetTrait(Integration::TraitId(Integration::ReservedTraitId::INTERACTION_TRAIT));
  return ClickableTrait::DownCast(trait);
}

BaseHandle View::GetLayoutParamsTrait(LayoutParamsType type) const
{
  return Integration::GetImpl(*this).GetLayoutParamsTrait(type);
}

View& View::SetLayoutParams(LayoutParams params)
{
  Integration::GetImpl(*this).SetLayoutParams(params);
  return *this;
}

// From control.cpp

void View::ClearBackground()
{
  Integration::GetImpl(*this).ClearBackground();
}

void View::SetRenderEffect(Ui::RenderEffect effect)
{
  Integration::GetImpl(*this).SetRenderEffect(effect);
}

Ui::RenderEffect View::GetRenderEffect() const
{
  return Integration::GetImpl(*this).GetRenderEffect();
}

void View::ClearRenderEffect()
{
  Integration::GetImpl(*this).ClearRenderEffect();
}

bool View::IsResourceReady() const
{
  return Integration::GetImpl(*this).IsResourceReady();
}

View::ResourceReadySignalType& View::ResourceReadySignal()
{
  Integration::ViewImpl&  viewImpl     = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);

  return viewDataImpl.mResourceReadySignal;
}

View::OffScreenRenderingFinishedSignalType& View::OffScreenRenderingFinishedSignal()
{
  Integration::ViewImpl&  viewImpl     = Ui::Integration::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);

  return viewDataImpl.mOffScreenRenderingFinishedSignal;
}

} // namespace Ui

} // namespace Dali
