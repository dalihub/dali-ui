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
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-renderers.h>
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/traits/interactive-trait.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

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
  View        handle(*impl);

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

MeasuredSize View::Measure(float widthConstraint, float heightConstraint)
{
  return GetImpl(*this).Measure(widthConstraint, heightConstraint);
}

LayoutRect View::Arrange(const LayoutRect& bounds)
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

void View::SetLayoutTransition(LayoutTransition transition)
{
  GetImpl(*this).SetLayoutTransition(transition);
}

LayoutTransition View::GetLayoutTransition() const
{
  return GetImpl(*this).GetLayoutTransition();
}

void View::AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager)
{
  GetImpl(*this).AttachLayoutManager(std::move(manager));
}

// =============================================================================
// Properties
// =============================================================================

void View::SetUiScalePolicy(UiScalePolicy policy)
{
  GetImpl(*this).SetUiScalePolicy(policy);
}

UiScalePolicy View::GetUiScalePolicy() const
{
  return GetImpl(*this).GetUiScalePolicy();
}

void View::SetRequestedX(float x)
{
  GetImpl(*this).SetRequestedX(x);
}

float View::GetRequestedX() const
{
  return GetImpl(*this).GetRequestedX();
}

void View::SetRequestedY(float y)
{
  GetImpl(*this).SetRequestedY(y);
}

float View::GetRequestedY() const
{
  return GetImpl(*this).GetRequestedY();
}

void View::SetRequestedWidth(float width)
{
  GetImpl(*this).SetRequestedWidth(width);
}

float View::GetRequestedWidth() const
{
  return GetImpl(*this).GetRequestedWidth();
}

void View::SetRequestedHeight(float height)
{
  GetImpl(*this).SetRequestedHeight(height);
}

float View::GetRequestedHeight() const
{
  return GetImpl(*this).GetRequestedHeight();
}

void View::SetMinimumWidth(float width)
{
  GetImpl(*this).SetMinimumWidth(width);
}

float View::GetMinimumWidth() const
{
  return GetImpl(*this).GetMinimumWidth();
}

void View::SetMinimumHeight(float height)
{
  GetImpl(*this).SetMinimumHeight(height);
}

float View::GetMinimumHeight() const
{
  return GetImpl(*this).GetMinimumHeight();
}

void View::SetMaximumWidth(float width)
{
  GetImpl(*this).SetMaximumWidth(width);
}

float View::GetMaximumWidth() const
{
  return GetImpl(*this).GetMaximumWidth();
}

void View::SetMaximumHeight(float height)
{
  GetImpl(*this).SetMaximumHeight(height);
}

float View::GetMaximumHeight() const
{
  return GetImpl(*this).GetMaximumHeight();
}

void View::SetMargin(const Insets& margin)
{
  GetImpl(*this).SetMargin(margin);
}

void View::SetMargin(float start, float end, float top, float bottom)
{
  SetMargin(Insets(start, end, top, bottom));
}

void View::SetMargin(float horizontal, float vertical)
{
  SetMargin(Insets(horizontal, horizontal, vertical, vertical));
}

void View::SetMargin(float uniform)
{
  SetMargin(Insets(uniform, uniform, uniform, uniform));
}

void View::SetStartMargin(float margin)
{
  Insets insets = GetMargin();
  insets.start  = margin;
  SetMargin(insets);
}

void View::SetEndMargin(float margin)
{
  Insets insets = GetMargin();
  insets.end    = margin;
  SetMargin(insets);
}

void View::SetTopMargin(float margin)
{
  Insets insets = GetMargin();
  insets.top    = margin;
  SetMargin(insets);
}

void View::SetBottomMargin(float margin)
{
  Insets insets = GetMargin();
  insets.bottom = margin;
  SetMargin(insets);
}

Insets View::GetMargin() const
{
  return GetImpl(*this).GetMargin();
}

void View::SetPadding(const Insets& padding)
{
  GetImpl(*this).SetPadding(padding);
}

void View::SetPadding(float start, float end, float top, float bottom)
{
  SetPadding(Insets(start, end, top, bottom));
}

void View::SetPadding(float horizontal, float vertical)
{
  SetPadding(Insets(horizontal, horizontal, vertical, vertical));
}

void View::SetPadding(float uniform)
{
  SetPadding(Insets(uniform, uniform, uniform, uniform));
}

void View::SetStartPadding(float padding)
{
  Insets insets = GetPadding();
  insets.start  = padding;
  SetPadding(insets);
}

void View::SetEndPadding(float padding)
{
  Insets insets = GetPadding();
  insets.end    = padding;
  SetPadding(insets);
}

void View::SetTopPadding(float padding)
{
  Insets insets = GetPadding();
  insets.top    = padding;
  SetPadding(insets);
}

void View::SetBottomPadding(float padding)
{
  Insets insets = GetPadding();
  insets.bottom = padding;
  SetPadding(insets);
}

Insets View::GetPadding() const
{
  return GetImpl(*this).GetPadding();
}

void View::SetLayoutMode(LayoutMode mode)
{
  GetImpl(*this).SetLayoutMode(mode);
}

LayoutMode View::GetLayoutMode() const
{
  return GetImpl(*this).GetLayoutMode();
}

void View::SetLeftFocusableView(View view)
{
  SetProperty(Property::LEFT_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetRightFocusableView(View view)
{
  SetProperty(Property::RIGHT_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetUpFocusableView(View view)
{
  SetProperty(Property::UP_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetDownFocusableView(View view)
{
  SetProperty(Property::DOWN_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetClockwiseFocusableView(View view)
{
  SetProperty(Property::CLOCKWISE_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetCounterClockwiseFocusableView(View view)
{
  SetProperty(Property::COUNTER_CLOCKWISE_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetForwardFocusableView(View view)
{
  SetProperty(Property::FORWARD_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetBackwardFocusableView(View view)
{
  SetProperty(Property::BACKWARD_FOCUSABLE_VIEW_ID, view.GetProperty<int>(Actor::Property::ID));
}

void View::SetFocusNavigationCallback(FocusNavigationCallback callback)
{
  GetImpl(*this).SetFocusNavigationCallback(std::move(callback));
}

void View::SetBackgroundColor(const UiColor& color)
{
  GetImpl(*this).SetBackgroundColor(color);
}

UiColor View::GetBackgroundColor()
{
  return GetImpl(*this).GetBackgroundColor();
}

void View::SetBackgroundImage(const Dali::String& url)
{
  GetImpl(*this).SetBackgroundImage(url);
}

void View::SetBackgroundGradient(const Gradient::Base& gradient)
{
  GetImpl(*this).SetBackgroundGradient(gradient);
}

void View::SetShadow(const Shadow& shadow)
{
  GetImpl(*this).SetShadow(shadow);
}

void View::SetShadow(const ShadowStack& shadowStack)
{
  GetImpl(*this).SetShadow(shadowStack);
}

void View::SetInnerShadow(const InnerShadow& innerShadow)
{
  GetImpl(*this).SetInnerShadow(innerShadow);
}

UiColor View::GetColor() const
{
  return GetImpl(*this).GetColor();
}

void View::SetColor(const UiColor& color)
{
  GetImpl(*this).SetColor(color);
}

UiColor View::GetCurrentColor() const
{
  return GetImpl(*this).GetCurrentColor();
}

bool View::IsEffectivelyFocused() const
{
  return GetImpl(*this).IsEffectivelyFocused();
}

const ViewState& View::GetState() const
{
  return GetImpl(*this).GetState();
}

View::StateChangedSignalType& View::StateChangedSignal()
{
  return GetImpl(*this).StateChangedSignal();
}

View::LayoutFinishedSignalType& View::LayoutFinishedSignal()
{
  return GetImpl(*this).LayoutFinishedSignal();
}

Vector4 View::GetCornerRadius() const
{
  return GetImpl(*this).GetCornerRadius();
}

void View::SetCornerRadius(float radius)
{
  GetImpl(*this).SetCornerRadius(Vector4(radius, radius, radius, radius));
}

void View::SetCornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  GetImpl(*this).SetCornerRadius(Vector4(topLeft, topRight, bottomRight, bottomLeft));
}

void View::SetCornerRadius(const Vector4& radius)
{
  GetImpl(*this).SetCornerRadius(radius);
}

CornerRadiusPolicy View::GetCornerRadiusPolicy() const
{
  return GetImpl(*this).GetCornerRadiusPolicy();
}

void View::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  GetImpl(*this).SetCornerRadiusPolicy(policy);
}

void View::SetCornerRadiusPolicyRelative()
{
  GetImpl(*this).SetCornerRadiusPolicy(CornerRadiusPolicy::RELATIVE);
}

bool View::IsCornerRadiusPolicyRelative() const
{
  return GetImpl(*this).GetCornerRadiusPolicy() == CornerRadiusPolicy::RELATIVE;
}

Vector4 View::GetCornerSquareness() const
{
  return GetImpl(*this).GetCornerSquareness();
}

void View::SetCornerSquareness(float squareness)
{
  GetImpl(*this).SetCornerSquareness(Vector4(squareness, squareness, squareness, squareness));
}

void View::SetCornerSquareness(float topLeft, float topRight, float bottomRight, float bottomLeft)
{
  GetImpl(*this).SetCornerSquareness(Vector4(topLeft, topRight, bottomRight, bottomLeft));
}

void View::SetCornerSquareness(const Vector4& squareness)
{
  GetImpl(*this).SetCornerSquareness(squareness);
}

float View::GetBorderlineWidth() const
{
  return GetImpl(*this).GetBorderlineWidth();
}

void View::SetBorderlineWidth(float width)
{
  GetImpl(*this).SetBorderlineWidth(width);
}

UiColor View::GetBorderlineColor()
{
  return GetImpl(*this).GetBorderlineColor();
}

void View::SetBorderlineColor(const UiColor& color)
{
  GetImpl(*this).SetBorderlineColor(color);
}

float View::GetBorderlineOffset() const
{
  return GetImpl(*this).GetBorderlineOffset();
}

void View::SetBorderlineOffset(float offset)
{
  GetImpl(*this).SetBorderlineOffset(offset);
}

InteractiveTrait View::AsInteractive()
{
  return GetImpl(*this).EnsureInteractiveTrait();
}

bool View::IsInteractive() const
{
  return GetImpl(*this).IsInteractive();
}

SelectableTrait View::AsSelectable()
{
  return GetImpl(*this).EnsureSelectableTrait();
}

bool View::IsSelectable() const
{
  return GetImpl(*this).IsSelectable();
}

GroupSelectableTrait View::AsGroupSelectable()
{
  return GetImpl(*this).EnsureGroupSelectableTrait();
}

bool View::IsGroupSelectable() const
{
  return GetImpl(*this).IsGroupSelectable();
}

void View::SetStateEffect(StateEffect effect)
{
  GetImpl(*this).SetStateEffect(effect);
}

void View::SetStateEffectTarget(View target)
{
  GetImpl(*this).SetStateEffectTarget(target);
}

View View::GetStateEffectTarget() const
{
  return GetImpl(*this).GetStateEffectTarget();
}

void View::SetLayoutParams(const AbsoluteLayoutParams& params)
{
  GetImpl(*this).SetLayoutParams(params);
}

void View::SetLayoutParams(const FlexLayoutParams& params)
{
  GetImpl(*this).SetLayoutParams(params);
}

void View::SetLayoutParams(const GridLayoutParams& params)
{
  GetImpl(*this).SetLayoutParams(params);
}

void View::SetLayoutParams(const StackLayoutParams& params)
{
  GetImpl(*this).SetLayoutParams(params);
}

bool View::TryGetLayoutParams(AbsoluteLayoutParams& params) const
{
  return GetImpl(*this).TryGetLayoutParams(params);
}

bool View::TryGetLayoutParams(FlexLayoutParams& params) const
{
  return GetImpl(*this).TryGetLayoutParams(params);
}

bool View::TryGetLayoutParams(GridLayoutParams& params) const
{
  return GetImpl(*this).TryGetLayoutParams(params);
}

bool View::TryGetLayoutParams(StackLayoutParams& params) const
{
  return GetImpl(*this).TryGetLayoutParams(params);
}

uint32_t View::GetChildViewCount() const
{
  return GetImpl(*this).GetChildViewCount();
}

View View::GetChildViewAt(uint32_t index) const
{
  return GetImpl(*this).GetChildViewAt(index);
}

int32_t View::IndexOfChildView(View childView) const
{
  return GetImpl(*this).IndexOfChildView(childView);
}

void View::Remove(View child, RemovePolicy policy)
{
  GetImpl(*this).Remove(child, policy);
}

void View::RemoveAll(RemovePolicy policy)
{
  GetImpl(*this).RemoveAll(policy);
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

bool View::AddVisual(Dali::Ui::VisualBase visualBase, Dali::Ui::Visual::ContainerRangeType containerRangeType)
{
  return GetImpl(*this).AddVisual(visualBase, containerRangeType);
}

void View::RemoveVisual(Dali::Ui::VisualBase visualBase)
{
  GetImpl(*this).RemoveVisual(visualBase);
}

uint32_t View::GetVisualCount(Dali::Ui::Visual::ContainerRangeType containerRangeType) const
{
  return GetImpl(*this).GetVisualCount(containerRangeType);
}

Dali::Ui::VisualBase View::GetVisualAt(Dali::Ui::Visual::ContainerRangeType containerRangeType, uint32_t siblingOrder) const
{
  return GetImpl(*this).GetVisualAt(containerRangeType, siblingOrder);
}

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

void View::SetAccessibilityName(StringView name)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityName(name);
}

Dali::String View::GetAccessibilityName() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetAccessibilityName();
}

void View::SetAccessibilityDescription(StringView description)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityDescription(description);
}

Dali::String View::GetAccessibilityDescription() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetAccessibilityDescription();
}

void View::SetAccessibilityValue(StringView value)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityValue(value);
}

Dali::String View::GetAccessibilityValue() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetAccessibilityValue();
}

void View::SetAccessibilityRole(Accessibility::Role role)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityRole(role);
}

Accessibility::Role View::GetAccessibilityRole() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetAccessibilityRole();
}

void View::SetAccessibilityHidden(bool hidden)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityHidden(hidden);
}

bool View::IsAccessibilityHidden() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).IsAccessibilityHidden();
}

void View::SetAccessibilityHighlightable(bool highlightable)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityHighlightable(highlightable);
}

void View::ResetAccessibilityHighlightable()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ResetAccessibilityHighlightable();
}

bool View::IsAccessibilityHighlightable() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).IsAccessibilityHighlightable();
}

void View::SetAccessibilityScrollable(bool scrollable)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityScrollable(scrollable);
}

bool View::IsAccessibilityScrollable() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).IsAccessibilityScrollable();
}

void View::SetAccessibilityModal(bool modal)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityModal(modal);
}

bool View::IsAccessibilityModal() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).IsAccessibilityModal();
}

void View::SetAutomationId(StringView automationId)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAutomationId(automationId);
}

Dali::String View::GetAutomationId() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetAutomationId();
}

void View::SetTranslatableAccessibilityName(StringView resourceId)
{
  SetTranslatableAccessibilityName(resourceId, StringView());
}

void View::SetTranslatableAccessibilityName(StringView resourceId, StringView domain)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetTranslatableAccessibilityName(resourceId, domain);
}

Dali::String View::GetTranslatableAccessibilityName() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetTranslatableAccessibilityName();
}

void View::ClearTranslatableAccessibilityName()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearTranslatableAccessibilityName();
}

void View::SetTranslatableAccessibilityDescription(StringView resourceId)
{
  SetTranslatableAccessibilityDescription(resourceId, StringView());
}

void View::SetTranslatableAccessibilityDescription(StringView resourceId, StringView domain)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetTranslatableAccessibilityDescription(resourceId, domain);
}

Dali::String View::GetTranslatableAccessibilityDescription() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetTranslatableAccessibilityDescription();
}

void View::ClearTranslatableAccessibilityDescription()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearTranslatableAccessibilityDescription();
}

void View::AddAccessibilityRelation(Accessibility::RelationType type, View target)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AddAccessibilityRelation(type, target);
}

void View::RemoveAccessibilityRelation(Accessibility::RelationType type, View target)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).RemoveAccessibilityRelation(type, target);
}

void View::ClearAccessibilityRelations()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearAccessibilityRelations();
}

bool View::HasAccessibilityRelation(Accessibility::RelationType type, View target) const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).HasAccessibilityRelation(type, target);
}

void View::AddAccessibilityReadingInfo(Accessibility::ReadingInfo info)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AddAccessibilityReadingInfo(info);
}

void View::RemoveAccessibilityReadingInfo(Accessibility::ReadingInfo info)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).RemoveAccessibilityReadingInfo(info);
}

void View::ClearAccessibilityReadingInfo()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearAccessibilityReadingInfo();
}

bool View::HasAccessibilityReadingInfo(Accessibility::ReadingInfo info) const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).HasAccessibilityReadingInfo(info);
}

bool View::AddAccessibilityNameLanguageSpan(uint32_t start, uint32_t length, StringView locale)
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AddAccessibilityNameLanguageSpan(start, length, locale);
}

void View::ClearAccessibilityNameLanguageSpans()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearAccessibilityNameLanguageSpans();
}

bool View::AddAccessibilityDescriptionLanguageSpan(uint32_t start, uint32_t length, StringView locale)
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AddAccessibilityDescriptionLanguageSpan(start, length, locale);
}

void View::ClearAccessibilityDescriptionLanguageSpans()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearAccessibilityDescriptionLanguageSpans();
}

void View::SetRequestInitialAccessibilityHighlight(bool request)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetRequestInitialAccessibilityHighlight(request);
}

bool View::IsInitialAccessibilityHighlightRequested() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).IsInitialAccessibilityHighlightRequested();
}

void View::SetAccessibilityCollectionContainer(bool container)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityCollectionContainer(container);
}

bool View::IsAccessibilityCollectionContainer() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).IsAccessibilityCollectionContainer();
}

void View::SetAccessibilityCollectionIndex(int32_t index)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).SetAccessibilityCollectionIndex(index);
}

int32_t View::GetAccessibilityCollectionIndex() const
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).GetAccessibilityCollectionIndex();
}

void View::ClearAccessibilityCollectionIndex()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearAccessibilityCollectionIndex();
}

void View::AppendAccessibilityAttribute(StringView key, StringView value)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AppendAccessibilityAttribute(Dali::String(key), Dali::String(value));
}

void View::RemoveAccessibilityAttribute(StringView key)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).RemoveAccessibilityAttribute(Dali::String(key));
}

void View::ClearAccessibilityAttributes()
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).ClearAccessibilityAttributes();
}

void View::AddAccessibilityState(Accessibility::State state)
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
  viewDataImpl.AddAccessibilityState(state);
}

void View::RemoveAccessibilityState(Accessibility::State state)
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
  viewDataImpl.RemoveAccessibilityState(state);
}

void View::ClearAccessibilityStates()
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
  viewDataImpl.ClearAccessibilityStates();
}

void View::NotifyAccessibilityActiveDescendantChanged(View descendant)
{
  Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).NotifyAccessibilityActiveDescendantChanged(descendant);
}

bool View::HasAccessibilityState(Accessibility::State state) const
{
  const ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  const Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
  return viewDataImpl.HasAccessibilityState(state);
}

View::AccessibilityReadingStatusChangedSignalType& View::AccessibilityReadingStatusChangedSignal()
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AccessibilityReadingStatusChangedSignal();
}

View::AccessibilityHighlightedSignalType& View::AccessibilityHighlightedSignal()
{
  return Internal::ViewDataImpl::Get(Ui::GetImpl(*this)).AccessibilityHighlightedSignal();
}

View::ResourceReadySignalType& View::ResourceReadySignal()
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);

  return viewDataImpl.ResourceReadySignal();
}

View::OffScreenRenderingFinishedSignalType& View::OffScreenRenderingFinishedSignal()
{
  ViewImpl&               viewImpl     = Ui::GetImpl(*this);
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);

  return viewDataImpl.OffScreenRenderingFinishedSignal();
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

void View::SetAttachment(AttachmentId id, UniqueAny attachment)
{
  GetImpl(*this).SetAttachment(id, Dali::Move(attachment));
}

bool View::RemoveAttachment(AttachmentId id)
{
  return GetImpl(*this).RemoveAttachment(id);
}

UniqueAny View::DetachAttachmentInternal(AttachmentId id)
{
  return GetImpl(*this).DetachAttachment(id);
}

UniqueAny* View::GetAttachmentInternal(AttachmentId id)
{
  return GetImpl(*this).GetAttachment(id);
}

const UniqueAny* View::GetAttachmentInternal(AttachmentId id) const
{
  return GetImpl(*this).GetAttachment(id);
}

} // namespace Ui

} // namespace Dali
