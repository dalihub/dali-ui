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
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-callbacks-impl.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>
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

MeasuredSize View::GetMeasuredSize() const
{
  return Integration::GetImpl(*this).GetMeasuredSize();
}

bool View::IsMeasureValid() const
{
  return Integration::GetImpl(*this).IsMeasureValid();
}

bool View::IsArrangeValid() const
{
  return Integration::GetImpl(*this).IsArrangeValid();
}

void View::SetMeasureCallback(::Dali::UniquePtr<LayoutMeasureCallback> callback)
{
  auto&                          impl      = Integration::GetImpl(*this);
  Internal::LayoutCallbacksImpl* callbacks = impl.EnsureLayoutCallbacks();
  callbacks->SetMeasureCallback(callback->GetCallback());
}

void View::SetArrangeCallback(::Dali::UniquePtr<LayoutArrangeCallback> callback)
{
  auto&                          impl      = Integration::GetImpl(*this);
  Internal::LayoutCallbacksImpl* callbacks = impl.EnsureLayoutCallbacks();
  callbacks->SetArrangeCallback(callback->GetCallback());
}

// =============================================================================
// Properties
// =============================================================================

float View::GetScaleX() const
{
  return Integration::GetImpl(*this).GetScaleX();
}

View& View::SetScaleX(float scaleX)
{
  Integration::GetImpl(*this).SetScaleX(scaleX);
  return *this;
}

float View::GetScaleY() const
{
  return Integration::GetImpl(*this).GetScaleY();
}

View& View::SetScaleY(float scaleY)
{
  Integration::GetImpl(*this).SetScaleY(scaleY);
  return *this;
}

View& View::SetScale(float scaleX, float scaleY)
{
  SetScaleX(scaleX);
  SetScaleY(scaleY);
  return *this;
}

bool View::IsVisible() const
{
  return Integration::GetImpl(*this).IsVisible();
}

View& View::SetVisibility(bool visibility)
{
  Integration::GetImpl(*this).SetVisibility(visibility);
  return *this;
}

float View::GetOpacity() const
{
  return Integration::GetImpl(*this).GetOpacity();
}

View& View::SetOpacity(float opacity)
{
  Integration::GetImpl(*this).SetOpacity(opacity);
  return *this;
}

MeasuredSize View::GetSize() const
{
  return Integration::GetImpl(*this).GetSize();
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

View& View::SetPivot(const Vector3& point)
{
  Integration::GetImpl(*this).SetPivot(point);
  return *this;
}

Vector3 View::GetPivot() const
{
  return Integration::GetImpl(*this).GetPivot();
}

View& View::SetRequestedWidth(float width)
{
  Integration::GetImpl(*this).SetRequestedWidth(width);
  return *this;
}

float View::GetRequestedWidth() const
{
  return Integration::GetImpl(*this).GetRequestedWidth();
}

View& View::SetRequestedHeight(float height)
{
  Integration::GetImpl(*this).SetRequestedHeight(height);
  return *this;
}

float View::GetRequestedHeight() const
{
  return Integration::GetImpl(*this).GetRequestedHeight();
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

View& View::SetMargin(const Extents& margin)
{
  Integration::GetImpl(*this).SetMargin(margin);
  return *this;
}

Extents View::GetMargin() const
{
  return Integration::GetImpl(*this).GetMargin();
}

View& View::SetPadding(const Extents& padding)
{
  Integration::GetImpl(*this).SetPadding(padding);
  return *this;
}

Extents View::GetPadding() const
{
  return Integration::GetImpl(*this).GetPadding();
}

View& View::SetLayoutMode(LayoutMode mode)
{
  Integration::GetImpl(*this).SetLayoutMode(mode);
  return *this;
}

LayoutMode View::GetLayoutMode() const
{
  return Integration::GetImpl(*this).GetLayoutMode();
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

bool View::IsEnabled() const
{
  return Integration::GetImpl(*this).IsEnabled();
}

View& View::SetEnabled(bool enabled)
{
  Integration::GetImpl(*this).SetEnabled(enabled);
  return *this;
}

bool View::IsEffectivelyEnabled() const
{
  return Integration::GetImpl(*this).IsEffectivelyEnabled();
}

bool View::IsEffectivelyFocused() const
{
  return Integration::GetImpl(*this).IsEffectivelyFocused();
}

// =============================================================================
// State API
// =============================================================================

const UiState& View::GetState() const
{
  return Integration::GetImpl(*this).GetState();
}

View::StateChangedSignalType& View::StateChangedSignal()
{
  return Integration::GetImpl(*this).StateChangedSignal();
}

View& View::SetBackgroundColor(const UiColor& color)
{
  Integration::GetImpl(*this).SetBackgroundColor(color);
  return *this;
}

Vector4 View::GetCornerRadius() const
{
  return Integration::GetImpl(*this).GetCornerRadius();
}

View& View::SetCornerRadius(float radius)
{
  Integration::GetImpl(*this).SetCornerRadius(Vector4(radius, radius, radius, radius));
  return *this;
}

View& View::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  Integration::GetImpl(*this).SetCornerRadius(Vector4(topLeft, topRight, bottomRight, bottomLeft));
  return *this;
}

View& View::SetCornerRadius(const Vector4& radius)
{
  Integration::GetImpl(*this).SetCornerRadius(radius);
  return *this;
}

CornerRadiusPolicy View::GetCornerRadiusPolicy() const
{
  return Integration::GetImpl(*this).GetCornerRadiusPolicy();
}

View& View::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  Integration::GetImpl(*this).SetCornerRadiusPolicy(policy);
  return *this;
}

View& View::SetCornerRadiusPolicyRelative()
{
  Integration::GetImpl(*this).SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  return *this;
}

bool View::IsCornerRadiusPolicyRelative() const
{
  return Integration::GetImpl(*this).GetCornerRadiusPolicy() == CornerRadiusPolicy::RELATIVE;
}

Vector4 View::GetCornerSquareness() const
{
  return Integration::GetImpl(*this).GetCornerSquareness();
}

View& View::SetCornerSquareness(float squareness)
{
  Integration::GetImpl(*this).SetCornerSquareness(Vector4(squareness, squareness, squareness, squareness));
  return *this;
}

View& View::SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  Integration::GetImpl(*this).SetCornerSquareness(Vector4(topLeft, topRight, bottomRight, bottomLeft));
  return *this;
}

View& View::SetCornerSquareness(const Vector4& squareness)
{
  Integration::GetImpl(*this).SetCornerSquareness(squareness);
  return *this;
}

float View::GetBorderlineWidth() const
{
  return Integration::GetImpl(*this).GetBorderlineWidth();
}

View& View::SetBorderlineWidth(float width)
{
  Integration::GetImpl(*this).SetBorderlineWidth(width);
  return *this;
}

UiColor View::GetBorderlineColor()
{
  return Integration::GetImpl(*this).GetBorderlineColor();
}

View& View::SetBorderlineColor(const UiColor& color)
{
  Integration::GetImpl(*this).SetBorderlineColor(color);
  return *this;
}

float View::GetBorderlineOffset() const
{
  return Integration::GetImpl(*this).GetBorderlineOffset();
}

View& View::SetBorderlineOffset(float offset)
{
  Integration::GetImpl(*this).SetBorderlineOffset(offset);
  return *this;
}

Dali::String View::GetName() const
{
  return Integration::GetImpl(*this).GetName();
}

View& View::SetName(const Dali::String& name)
{
  Integration::GetImpl(*this).SetName(name);
  return *this;
}

InteractiveTrait View::EnsureInteractiveTrait()
{
  return Integration::GetImpl(*this).EnsureInteractiveTrait();
}

bool View::IsInteractive() const
{
  return Integration::GetImpl(*this).IsInteractive();
}

SelectableTrait View::EnsureSelectableTrait()
{
  return Integration::GetImpl(*this).EnsureSelectableTrait();
}

bool View::IsSelectable() const
{
  return Integration::GetImpl(*this).IsSelectable();
}

View& View::SetInteractionEffect(Trait effect)
{
  Integration::GetImpl(*this).SetInteractionEffect(effect);
  return *this;
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

// =============================================================================
// Child Management API
// =============================================================================

void View::Insert(uint32_t index, View child)
{
  Integration::GetImpl(*this).Insert(index, child);
}

void View::RemoveAllChildren()
{
  Integration::GetImpl(*this).RemoveAllChildren();
}

uint32_t View::GetChildCount() const
{
  return Integration::GetImpl(*this).GetChildCount();
}

View View::GetChildAt(uint32_t index) const
{
  return Integration::GetImpl(*this).GetChildAt(index);
}

int32_t View::IndexOfChild(View view) const
{
  return Integration::GetImpl(*this).IndexOfChild(view);
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

bool View::IsOnScene() const
{
  return Integration::GetImpl(*this).IsOnScene();
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

View::KeyEventSignalType& View::KeyEventSignal()
{
  return Integration::GetImpl(*this).KeyEventSignal();
}

View::FocusChangedSignalType& View::FocusChangedSignal()
{
  return Integration::GetImpl(*this).FocusChangedSignal();
}

} // namespace Ui

} // namespace Dali
