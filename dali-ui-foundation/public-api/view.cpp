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
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/interactive-trait.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/view-impl.h>

namespace Dali
{

namespace Ui
{

View::View()
{
}

View View::New()
{
  ViewImplPtr impl = ViewImpl::New();

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
  return Ui::View::DownCast<View, ViewImpl>(handle);
}

View::View(ViewImpl& implementation)
: CustomActor(implementation)
{
}

View::View(Dali::Internal::CustomActor* internal)
: CustomActor(internal)
{
  VerifyCustomActorPointer<ViewImpl>(internal);
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize View::Measure(float widthConstraint, float heightConstraint)
{
  return GetImpl(*this).Measure(widthConstraint, heightConstraint);
}

MeasuredSize View::Arrange(const LayoutRect& bounds)
{
  return GetImpl(*this).Arrange(bounds);
}

void View::InvalidateMeasure()
{
  GetImpl(*this).InvalidateMeasure();
}

void View::InvalidateArrange()
{
  GetImpl(*this).InvalidateArrange();
}

MeasuredSize View::GetMeasuredSize() const
{
  return GetImpl(*this).GetMeasuredSize();
}

void View::SetMeasureCallback(MeasureCallback callback)
{
  GetImpl(*this).SetMeasureCallback(std::move(callback));
}

void View::SetArrangeCallback(ArrangeCallback callback)
{
  GetImpl(*this).SetArrangeCallback(std::move(callback));
}

// =============================================================================
// Properties
// =============================================================================

float View::GetScaleX() const
{
  return GetImpl(*this).GetScaleX();
}

View& View::SetScaleX(float scaleX)
{
  GetImpl(*this).SetScaleX(scaleX);
  return *this;
}

float View::GetScaleY() const
{
  return GetImpl(*this).GetScaleY();
}

View& View::SetScaleY(float scaleY)
{
  GetImpl(*this).SetScaleY(scaleY);
  return *this;
}

View& View::SetScale(float scaleX, float scaleY)
{
  SetScaleX(scaleX);
  SetScaleY(scaleY);
  return *this;
}

View& View::SetLayoutDirection(Dali::LayoutDirection::Type direction)
{
  GetImpl(*this).SetLayoutDirection(direction);
  return *this;
}

void View::ClearLayoutDirection()
{
  GetImpl(*this).ClearLayoutDirection();
}

bool View::IsLayoutDirectionInherited() const
{
  return GetImpl(*this).IsLayoutDirectionInherited();
}

Dali::LayoutDirection::Type View::GetEffectiveLayoutDirection() const
{
  return GetImpl(*this).GetEffectiveLayoutDirection();
}

bool View::IsVisible() const
{
  return GetImpl(*this).IsVisible();
}

View& View::SetVisibility(bool visibility)
{
  GetImpl(*this).SetVisibility(visibility);
  return *this;
}

float View::GetOpacity() const
{
  return GetImpl(*this).GetOpacity();
}

View& View::SetOpacity(float opacity)
{
  GetImpl(*this).SetOpacity(opacity);
  return *this;
}

MeasuredSize View::GetSize() const
{
  return GetImpl(*this).GetSize();
}

View& View::SetRequestedPositionX(float x)
{
  GetImpl(*this).SetRequestedPositionX(x);
  return *this;
}

float View::GetRequestedPositionX() const
{
  return GetImpl(*this).GetRequestedPositionX();
}

View& View::SetRequestedPositionY(float y)
{
  GetImpl(*this).SetRequestedPositionY(y);
  return *this;
}

float View::GetRequestedPositionY() const
{
  return GetImpl(*this).GetRequestedPositionY();
}

float View::GetPositionX() const
{
  return GetImpl(*this).GetPositionX();
}

float View::GetPositionY() const
{
  return GetImpl(*this).GetPositionY();
}

View& View::SetParentOrigin(const Vector3& point)
{
  GetImpl(*this).SetParentOrigin(point);
  return *this;
}

Vector3 View::GetParentOrigin() const
{
  return GetImpl(*this).GetParentOrigin();
}

View& View::SetPivot(const Vector3& point)
{
  GetImpl(*this).SetPivot(point);
  return *this;
}

Vector3 View::GetPivot() const
{
  return GetImpl(*this).GetPivot();
}

View& View::SetRequestedWidth(float width)
{
  GetImpl(*this).SetRequestedWidth(width);
  return *this;
}

float View::GetRequestedWidth() const
{
  return GetImpl(*this).GetRequestedWidth();
}

View& View::SetRequestedHeight(float height)
{
  GetImpl(*this).SetRequestedHeight(height);
  return *this;
}

float View::GetRequestedHeight() const
{
  return GetImpl(*this).GetRequestedHeight();
}

View& View::SetMinimumWidth(float width)
{
  GetImpl(*this).SetMinimumWidth(width);
  return *this;
}

float View::GetMinimumWidth() const
{
  return GetImpl(*this).GetMinimumWidth();
}

View& View::SetMinimumHeight(float height)
{
  GetImpl(*this).SetMinimumHeight(height);
  return *this;
}

float View::GetMinimumHeight() const
{
  return GetImpl(*this).GetMinimumHeight();
}

View& View::SetMaximumWidth(float width)
{
  GetImpl(*this).SetMaximumWidth(width);
  return *this;
}

float View::GetMaximumWidth() const
{
  return GetImpl(*this).GetMaximumWidth();
}

View& View::SetMaximumHeight(float height)
{
  GetImpl(*this).SetMaximumHeight(height);
  return *this;
}

float View::GetMaximumHeight() const
{
  return GetImpl(*this).GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

View& View::SetMargin(const Extents& margin)
{
  GetImpl(*this).SetMargin(margin);
  return *this;
}

Extents View::GetMargin() const
{
  return GetImpl(*this).GetMargin();
}

View& View::SetPadding(const Extents& padding)
{
  GetImpl(*this).SetPadding(padding);
  return *this;
}

Extents View::GetPadding() const
{
  return GetImpl(*this).GetPadding();
}

View& View::SetLayoutMode(LayoutMode mode)
{
  GetImpl(*this).SetLayoutMode(mode);
  return *this;
}

LayoutMode View::GetLayoutMode() const
{
  return GetImpl(*this).GetLayoutMode();
}

bool View::IsFocusable() const
{
  return GetImpl(*this).IsFocusable();
}

View& View::SetFocusable(bool focusable)
{
  GetImpl(*this).SetFocusable(focusable);
  return *this;
}

bool View::IsTouchFocusable() const
{
  return GetImpl(*this).IsTouchFocusable();
}

View& View::SetTouchFocusable(bool touchFocusable)
{
  GetImpl(*this).SetTouchFocusable(touchFocusable);
  return *this;
}

View& View::SetLeftFocusableView(View view)
{
  SetProperty(Property::LEFT_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
  return *this;
}

View& View::SetRightFocusableView(View view)
{
  SetProperty(Property::RIGHT_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
  return *this;
}

View& View::SetUpFocusableView(View view)
{
  SetProperty(Property::UP_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
  return *this;
}

View& View::SetDownFocusableView(View view)
{
  SetProperty(Property::DOWN_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
  return *this;
}

View& View::SetClockwiseFocusableView(View view)
{
  SetProperty(Property::CLOCKWISE_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
  return *this;
}

View& View::SetCounterClockwiseFocusableView(View view)
{
  SetProperty(Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
  return *this;
}

View& View::SetKeyNavigationSupport(bool isSupported)
{
  GetImpl(*this).SetKeyNavigationSupport(isSupported);
  return *this;
}

bool View::IsKeyNavigationSupported() const
{
  return GetImpl(*this).IsKeyNavigationSupported();
}

UiColor View::GetBackgroundColor()
{
  return GetImpl(*this).GetBackgroundColor();
}

bool View::IsEnabled() const
{
  return GetImpl(*this).IsEnabled();
}

View& View::SetEnabled(bool enabled)
{
  GetImpl(*this).SetEnabled(enabled);
  return *this;
}

bool View::IsEffectivelyEnabled() const
{
  return GetImpl(*this).IsEffectivelyEnabled();
}

bool View::IsEffectivelyFocused() const
{
  return GetImpl(*this).IsEffectivelyFocused();
}

// =============================================================================
// State API
// =============================================================================

const ViewState& View::GetState() const
{
  return GetImpl(*this).GetState();
}

View::StateChangedSignalType& View::StateChangedSignal()
{
  return GetImpl(*this).StateChangedSignal();
}

View& View::SetBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetBackgroundColor(color);
  return *this;
}

Vector4 View::GetCornerRadius() const
{
  return GetImpl(*this).GetCornerRadius();
}

View& View::SetCornerRadius(float radius)
{
  GetImpl(*this).SetCornerRadius(Vector4(radius, radius, radius, radius));
  return *this;
}

View& View::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  GetImpl(*this).SetCornerRadius(Vector4(topLeft, topRight, bottomRight, bottomLeft));
  return *this;
}

View& View::SetCornerRadius(const Vector4& radius)
{
  GetImpl(*this).SetCornerRadius(radius);
  return *this;
}

CornerRadiusPolicy View::GetCornerRadiusPolicy() const
{
  return GetImpl(*this).GetCornerRadiusPolicy();
}

View& View::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  GetImpl(*this).SetCornerRadiusPolicy(policy);
  return *this;
}

View& View::SetCornerRadiusPolicyRelative()
{
  GetImpl(*this).SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
  return *this;
}

bool View::IsCornerRadiusPolicyRelative() const
{
  return GetImpl(*this).GetCornerRadiusPolicy() == CornerRadiusPolicy::RELATIVE;
}

Vector4 View::GetCornerSquareness() const
{
  return GetImpl(*this).GetCornerSquareness();
}

View& View::SetCornerSquareness(float squareness)
{
  GetImpl(*this).SetCornerSquareness(Vector4(squareness, squareness, squareness, squareness));
  return *this;
}

View& View::SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  GetImpl(*this).SetCornerSquareness(Vector4(topLeft, topRight, bottomRight, bottomLeft));
  return *this;
}

View& View::SetCornerSquareness(const Vector4& squareness)
{
  GetImpl(*this).SetCornerSquareness(squareness);
  return *this;
}

float View::GetBorderlineWidth() const
{
  return GetImpl(*this).GetBorderlineWidth();
}

View& View::SetBorderlineWidth(float width)
{
  GetImpl(*this).SetBorderlineWidth(width);
  return *this;
}

UiColor View::GetBorderlineColor()
{
  return GetImpl(*this).GetBorderlineColor();
}

View& View::SetBorderlineColor(const UiColor& color)
{
  GetImpl(*this).SetBorderlineColor(color);
  return *this;
}

float View::GetBorderlineOffset() const
{
  return GetImpl(*this).GetBorderlineOffset();
}

View& View::SetBorderlineOffset(float offset)
{
  GetImpl(*this).SetBorderlineOffset(offset);
  return *this;
}

Dali::String View::GetName() const
{
  return GetImpl(*this).GetName();
}

View& View::SetName(const Dali::String& name)
{
  GetImpl(*this).SetName(name);
  return *this;
}

InteractiveTrait View::EnsureInteractiveTrait()
{
  return GetImpl(*this).EnsureInteractiveTrait();
}

bool View::IsInteractive() const
{
  return GetImpl(*this).IsInteractive();
}

SelectableTrait View::EnsureSelectableTrait()
{
  return GetImpl(*this).EnsureSelectableTrait();
}

bool View::IsSelectable() const
{
  return GetImpl(*this).IsSelectable();
}

View& View::SetInteractionEffect(Trait effect)
{
  GetImpl(*this).SetInteractionEffect(effect);
  return *this;
}

BaseHandle View::GetLayoutParamsInternal(LayoutParamsType type) const
{
  return GetImpl(*this).GetLayoutParams(type);
}

View& View::SetLayoutParams(LayoutParams params)
{
  GetImpl(*this).SetLayoutParams(params);
  return *this;
}

// =============================================================================
// Child Management API
// =============================================================================

void View::Insert(uint32_t index, View child)
{
  GetImpl(*this).Insert(index, child);
}

void View::RemoveAllChildren()
{
  GetImpl(*this).RemoveAllChildren();
}

uint32_t View::GetChildCount() const
{
  return GetImpl(*this).GetChildCount();
}

View View::GetChildAt(uint32_t index) const
{
  return GetImpl(*this).GetChildAt(index);
}

int32_t View::IndexOfChild(View view) const
{
  return GetImpl(*this).IndexOfChild(view);
}

void View::Raise(LayoutOrderPolicy policy)
{
  GetImpl(*this).Raise(policy);
}

void View::Lower(LayoutOrderPolicy policy)
{
  GetImpl(*this).Lower(policy);
}

void View::RaiseToTop(LayoutOrderPolicy policy)
{
  GetImpl(*this).RaiseToTop(policy);
}

void View::LowerToBottom(LayoutOrderPolicy policy)
{
  GetImpl(*this).LowerToBottom(policy);
}

void View::RaiseAbove(View target, LayoutOrderPolicy policy)
{
  GetImpl(*this).RaiseAbove(target, policy);
}

void View::LowerBelow(View target, LayoutOrderPolicy policy)
{
  GetImpl(*this).LowerBelow(target, policy);
}

// From control.cpp

void View::ClearBackground()
{
  GetImpl(*this).ClearBackground();
}

void View::SetRenderEffect(Ui::RenderEffect effect)
{
  GetImpl(*this).SetRenderEffect(effect);
}

Ui::RenderEffect View::GetRenderEffect() const
{
  return GetImpl(*this).GetRenderEffect();
}

void View::ClearRenderEffect()
{
  GetImpl(*this).ClearRenderEffect();
}

bool View::IsResourceReady() const
{
  return GetImpl(*this).IsResourceReady();
}

bool View::IsOnScene() const
{
  return GetImpl(*this).IsOnScene();
}

View::ResourceReadySignalType& View::ResourceReadySignal()
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);

  return viewDataImpl.mResourceReadySignal;
}

View::OffScreenRenderingFinishedSignalType& View::OffScreenRenderingFinishedSignal()
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);

  return viewDataImpl.mOffScreenRenderingFinishedSignal;
}

View::KeyEventSignalType& View::KeyEventSignal()
{
  return GetImpl(*this).KeyEventSignal();
}

View::FocusChangedSignalType& View::FocusChangedSignal()
{
  return GetImpl(*this).FocusChangedSignal();
}

ViewAnimationBridge View::Animate(Animation animation)
{
  return ViewAnimationBridge(animation, *this);
}

ViewAnimationSpec View::NewAnimationSpec()
{
  return ViewAnimationSpec::New();
}

} // namespace Ui

} // namespace Dali
