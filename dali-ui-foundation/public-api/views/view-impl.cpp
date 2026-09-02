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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h> // LCOV_EXCL_LINE
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/animation/constraint.h>
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-accessibility.h>
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/render-effects/render-effect.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

// Verify CornerRadiusPolicy values stay in sync with Ui::Integration::Visual::Policy::Type.
static_assert(static_cast<int>(Dali::Ui::CornerRadiusPolicy::RELATIVE) == Dali::Ui::Visual::Transform::Policy::RELATIVE);
static_assert(static_cast<int>(Dali::Ui::CornerRadiusPolicy::ABSOLUTE) == Dali::Ui::Visual::Transform::Policy::ABSOLUTE);

namespace Dali
{

namespace Ui
{

namespace
{

/// Nesting depth of active "allow non-View child" scopes. When > 0, View::OnChildAdd
/// accepts a non-View Actor as a child. Used by Integration::View::AddActorChild.
/// Thread-local because Actor::Add invokes OnChildAdd synchronously on the same
/// (event) thread. POD with a constant initializer: no dynamic init, no heap allocation.
thread_local unsigned int gAllowNonViewChildDepth = 0u;

} // namespace

ViewImplPtr ViewImpl::New()
{
  ViewImplPtr viewImpl = new ViewImpl();

  // Return the local itself, not a copy of it. `return ViewImplPtr(viewImpl)` copy-
  // constructs from an lvalue, which is a Reference/Unreference pair of atomics
  // (RefObject, dali-core) on every View::New. Naming the local in the return
  // statement is NRVO-eligible, and IntrusivePtr's move constructor detaches without
  // touching the count at all.
  return viewImpl;
}

ViewImpl::ViewImpl()
: CustomActorImpl(),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->SetBehaviourFlags(static_cast<Ui::ViewImpl::ViewBehaviour>(VIEW_BEHAVIOUR_DEFAULT));
}

ViewImpl::~ViewImpl()
{
  delete mImpl;
}

void ViewImpl::OnInitialize()
{
  // Intentionally empty. The child-order-changed connection this used to make now
  // lives in the non-virtual Initialize(); see the comment there.
}

void ViewImpl::OnDestroy()
{
  mImpl->Destroy();

  RefObject::OnDestroy();
}

void ViewImpl::OnSceneConnection(int depth)
{
  mImpl->OnViewSceneConnection();
}

bool ViewImpl::FilterKeyEvent(const Dali::KeyEvent&)
{
  return false;
}

bool ViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  return mImpl->HandleKeyEventDefault(event);
}

void ViewImpl::OnFinalizeKeyEventDispatch(const Dali::KeyEvent&)
{
  mImpl->FinalizeKeyEventDispatchDefault();
}

bool ViewImpl::HasIntrinsicHoverHandling() const
{
  return mImpl->HasIntrinsicHoverHandlingDefault();
}

bool ViewImpl::OnHoverEvent(const Dali::HoverEvent& event)
{
  return mImpl->HandleHoverEventDefault(event);
}

bool ViewImpl::HasIntrinsicTouchHandling() const
{
  return mImpl->HasIntrinsicTouchHandlingDefault();
}

bool ViewImpl::OnTouchEvent(const Dali::TouchEvent& event)
{
  return mImpl->HandleTouchEventDefault(event);
}

void ViewImpl::OnFinalizeTouchEventDispatch(const Dali::TouchEvent& event)
{
  mImpl->FinalizeTouchEventDispatchDefault(event);
}

// =============================================================================
// State API
// =============================================================================

const ViewState& ViewImpl::GetState() const
{
  return mImpl->GetState();
}

bool ViewImpl::IsEffectivelyFocused() const
{
  return mImpl->IsEffectivelyFocused();
}

View::LayoutFinishedSignalType& ViewImpl::LayoutFinishedSignal()
{
  return mImpl->LayoutFinishedSignal();
}

View::StateChangedSignalType& ViewImpl::StateChangedSignal()
{
  return mImpl->StateChangedSignal();
}

Ui::InteractiveTrait ViewImpl::EnsureInteractiveTrait()
{
  return mImpl->EnsureInteractiveTrait();
}

void ViewImpl::SetStateEffect(StateEffect effect)
{
  mImpl->SetStateEffect(effect);
}

void ViewImpl::SetStateEffectTarget(View target)
{
  mImpl->SetStateEffectTarget(target);
}

View ViewImpl::GetStateEffectTarget() const
{
  return mImpl->GetStateEffectTarget();
}

bool ViewImpl::IsInteractive() const
{
  return mImpl->IsInteractive();
}

void ViewImpl::SetAttachment(AttachmentId id, UniqueAny attachment)
{
  mImpl->SetAttachment(id, Dali::Move(attachment));
}

bool ViewImpl::RemoveAttachment(AttachmentId id)
{
  return mImpl->RemoveAttachment(id);
}

UniqueAny ViewImpl::DetachAttachment(AttachmentId id)
{
  return mImpl->DetachAttachment(id);
}

UniqueAny* ViewImpl::GetAttachment(AttachmentId id)
{
  return mImpl->GetAttachment(id);
}

const UniqueAny* ViewImpl::GetAttachment(AttachmentId id) const
{
  return mImpl->GetAttachment(id);
}

Ui::SelectableTrait ViewImpl::EnsureSelectableTrait()
{
  return mImpl->EnsureSelectableTrait();
}

bool ViewImpl::IsSelectable() const
{
  return mImpl->IsSelectable();
}

Ui::GroupSelectableTrait ViewImpl::EnsureGroupSelectableTrait()
{
  return mImpl->EnsureGroupSelectableTrait();
}

bool ViewImpl::IsGroupSelectable() const
{
  return mImpl->IsGroupSelectable();
}

void ViewImpl::NotifyFocusChanged(bool focused)
{
  OnFocusChanged(focused);
}

void ViewImpl::OnFocusChanged(bool focused)
{
  mImpl->HandleFocusChangedDefault(focused);
}

void ViewImpl::SetRequestedX(float x)
{
  mImpl->SetRequestedX(x);
}

void ViewImpl::SetRequestedY(float y)
{
  mImpl->SetRequestedY(y);
}

float ViewImpl::GetRequestedX() const
{
  return mImpl->GetRequestedX();
}

float ViewImpl::GetRequestedY() const
{
  return mImpl->GetRequestedY();
}

UiColor ViewImpl::GetBackgroundColor()
{
  return mImpl->GetBackgroundColor();
}

void ViewImpl::SetBackgroundColor(const UiColor& color)
{
  mImpl->SetBackgroundColor(color);
}

void ViewImpl::SetBackgroundImage(const Dali::String& url)
{
  mImpl->SetBackgroundImage(url);
}

void ViewImpl::SetBackgroundGradient(const Gradient::Base& gradient)
{
  mImpl->SetBackgroundGradient(gradient);
}

UiColor ViewImpl::GetColorMultiplier() const
{
  return mImpl->GetColor();
}

UiColor ViewImpl::GetColor() const
{
  return GetColorMultiplier();
}

void ViewImpl::SetColorMultiplier(const UiColor& multiplier)
{
  mImpl->SetColor(multiplier);
}

void ViewImpl::SetColor(const UiColor& color)
{
  SetColorMultiplier(color);
}

UiColor ViewImpl::GetCurrentColorMultiplier() const
{
  return mImpl->GetCurrentColor();
}

UiColor ViewImpl::GetCurrentColor() const
{
  return GetCurrentColorMultiplier();
}

Vector4 ViewImpl::GetCornerRadius() const
{
  return mImpl->GetCornerRadius();
}

void ViewImpl::SetCornerRadius(const Vector4& radius)
{
  mImpl->SetCornerRadius(radius);
}

CornerRadiusPolicy ViewImpl::GetCornerRadiusPolicy() const
{
  return mImpl->GetCornerRadiusPolicy();
}

void ViewImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  mImpl->SetCornerRadiusPolicy(policy);
}

Vector4 ViewImpl::GetCornerSquareness() const
{
  return mImpl->GetCornerSquareness();
}

void ViewImpl::SetCornerSquareness(const Vector4& squareness)
{
  mImpl->SetCornerSquareness(squareness);
}

float ViewImpl::GetBorderlineWidth() const
{
  return mImpl->GetBorderlineWidth();
}

void ViewImpl::SetBorderlineWidth(float width)
{
  mImpl->SetBorderlineWidth(width);
}

UiColor ViewImpl::GetBorderlineColor()
{
  return mImpl->GetBorderlineColor();
}

void ViewImpl::SetBorderlineColor(const UiColor& color)
{
  mImpl->SetBorderlineColor(color);
}

float ViewImpl::GetBorderlineOffset() const
{
  return mImpl->GetBorderlineOffset();
}

void ViewImpl::SetBorderlineOffset(float offset)
{
  mImpl->SetBorderlineOffset(offset);
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize ViewImpl::Measure(float visualW, float visualH)
{
  return mImpl->Measure(visualW, visualH);
}

MeasuredSize ViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  return mImpl->MeasureDefault(widthConstraint, heightConstraint);
}

LayoutRect ViewImpl::Arrange(const LayoutRect& bounds)
{
  return mImpl->Arrange(bounds);
}

LayoutRect ViewImpl::OnArrange(const LayoutRect& bounds)
{
  return mImpl->ArrangeDefault(bounds);
}

// =============================================================================
// UiScale API
// =============================================================================

void ViewImpl::SetUiScalePolicy(UiScalePolicy policy)
{
  mImpl->SetUiScalePolicy(policy);
}

UiScalePolicy ViewImpl::GetUiScalePolicy() const
{
  return mImpl->GetUiScalePolicy();
}

float ViewImpl::GetEffectiveScale() const
{
  return mImpl->GetEffectiveScale();
}

bool ViewImpl::UpdateColorBindingInternal(StringView bindingId, const UiColor& color)
{
  return mImpl->UpdateColorBindingInternal(bindingId, color);
}

void ViewImpl::SetColorBindingInternal(StringView bindingId, const UiColor& color, ColorCallback callback)
{
  mImpl->SetColorBindingInternal(bindingId, color, std::move(callback));
}

bool ViewImpl::UpdateColorBindingInternal(StringView bindingId, const Gradient::Base& gradient)
{
  return mImpl->UpdateColorBindingInternal(bindingId, gradient);
}

void ViewImpl::SetColorBindingInternal(StringView bindingId, const Gradient::Base& gradient, Callback<void(const Gradient::Base&)> callback)
{
  mImpl->SetColorBindingInternal(bindingId, gradient, std::move(callback));
}

void ViewImpl::ClearGradientColorBinding(StringView bindingId)
{
  mImpl->ClearGradientColorBinding(bindingId);
}

void ViewImpl::InvalidateMeasure()
{
  // The public wrapper adds the once-per-View diagnostic for invalidation from
  // layout callbacks. It still delegates to the same full invalidation
  // transaction as framework paths; the LayoutController centrally decides
  // whether the resulting pending root may arm an idle wake.
  mImpl->InvalidateMeasureFromPublicApi();
}

void ViewImpl::InvalidateArrange()
{
  mImpl->InvalidateArrangeFromPublicApi();
}

MeasuredSize ViewImpl::GetMeasuredSize() const
{
  return mImpl->GetMeasuredSize();
}

// =============================================================================
// Requested size API
// =============================================================================

void ViewImpl::SetRequestedWidth(float width)
{
  mImpl->SetRequestedWidth(width);
}

float ViewImpl::GetRequestedWidth() const
{
  return mImpl->GetRequestedWidth();
}

void ViewImpl::SetRequestedHeight(float height)
{
  mImpl->SetRequestedHeight(height);
}

float ViewImpl::GetRequestedHeight() const
{
  return mImpl->GetRequestedHeight();
}

void ViewImpl::SetMinimumWidth(float width)
{
  mImpl->SetMinimumWidth(width);
}

float ViewImpl::GetMinimumWidth() const
{
  return mImpl->GetMinimumWidth();
}

void ViewImpl::SetMinimumHeight(float height)
{
  mImpl->SetMinimumHeight(height);
}

float ViewImpl::GetMinimumHeight() const
{
  return mImpl->GetMinimumHeight();
}

void ViewImpl::SetMaximumWidth(float width)
{
  mImpl->SetMaximumWidth(width);
}

float ViewImpl::GetMaximumWidth() const
{
  return mImpl->GetMaximumWidth();
}

void ViewImpl::SetMaximumHeight(float height)
{
  mImpl->SetMaximumHeight(height);
}

float ViewImpl::GetMaximumHeight() const
{
  return mImpl->GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

void ViewImpl::SetMargin(const Insets& margin)
{
  mImpl->SetMargin(margin);
}

Insets ViewImpl::GetMargin() const
{
  return mImpl->GetMargin();
}

void ViewImpl::SetPadding(const Insets& padding)
{
  mImpl->SetPadding(padding);
}

Insets ViewImpl::GetPadding() const
{
  return mImpl->GetPadding();
}

void ViewImpl::SetLayoutMode(Ui::LayoutMode mode)
{
  mImpl->SetLayoutMode(mode);
}

Ui::LayoutMode ViewImpl::GetLayoutMode() const
{
  return mImpl->GetLayoutMode();
}

// =============================================================================
// Parent Layout API
// =============================================================================

void ViewImpl::SetMeasureCallback(MeasureCallback callback)
{
  mImpl->SetMeasureCallback(std::move(callback));
}

void ViewImpl::SetArrangeCallback(ArrangeCallback callback)
{
  mImpl->SetArrangeCallback(std::move(callback));
}

void ViewImpl::SetArrangeCallback(ArrangeCallback callback, ArrangePolicy policy)
{
  mImpl->SetArrangeCallback(std::move(callback), policy);
}

void ViewImpl::AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager)
{
  mImpl->AttachLayoutManager(std::move(manager));
}

LayoutManager* ViewImpl::GetLayoutManager() const
{
  return mImpl->GetLayoutManager();
}

void ViewImpl::SetArrangePolicy(ArrangePolicy policy)
{
  mImpl->SetArrangePolicy(policy);
}

ArrangePolicy ViewImpl::GetArrangePolicy() const
{
  return mImpl->GetArrangePolicy();
}

void ViewImpl::SetLayoutTransition(LayoutTransition transition)
{
  mImpl->SetLayoutTransition(transition);
}

LayoutTransition ViewImpl::GetLayoutTransition() const
{
  return mImpl->GetLayoutTransition();
}

LayoutRect ViewImpl::GetArrangedBounds() const
{
  return mImpl->GetArrangedBounds();
}

// =============================================================================
// Child Management API
// =============================================================================

void ViewImpl::Remove(Ui::View child, Ui::RemovePolicy policy)
{
  mImpl->Remove(child, policy);
}

void ViewImpl::RemoveAll(Ui::RemovePolicy policy)
{
  mImpl->RemoveAll(policy);
}

uint32_t ViewImpl::GetChildViewCount() const
{
  return mImpl->GetChildViewCount();
}

Ui::View ViewImpl::GetChildViewAt(uint32_t index) const
{
  return mImpl->GetChildViewAt(index);
}

int32_t ViewImpl::IndexOfChildView(Ui::View view) const
{
  return mImpl->IndexOfChildView(view);
}

void ViewImpl::Raise(Ui::LayoutOrderPolicy policy)
{
  mImpl->Raise(policy);
}

void ViewImpl::Lower(Ui::LayoutOrderPolicy policy)
{
  mImpl->Lower(policy);
}

void ViewImpl::RaiseToTop(Ui::LayoutOrderPolicy policy)
{
  mImpl->RaiseToTop(policy);
}

void ViewImpl::LowerToBottom(Ui::LayoutOrderPolicy policy)
{
  mImpl->LowerToBottom(policy);
}

void ViewImpl::RaiseAbove(Ui::View target, Ui::LayoutOrderPolicy policy)
{
  mImpl->RaiseAbove(target, policy);
}

void ViewImpl::LowerBelow(Ui::View target, Ui::LayoutOrderPolicy policy)
{
  mImpl->LowerBelow(target, policy);
}

void ViewImpl::SetLayoutParams(const AbsoluteLayoutParams& params)
{
  mImpl->SetLayoutParams(params);
}

void ViewImpl::SetLayoutParams(const FlexLayoutParams& params)
{
  mImpl->SetLayoutParams(params);
}

void ViewImpl::SetLayoutParams(const GridLayoutParams& params)
{
  mImpl->SetLayoutParams(params);
}

void ViewImpl::SetLayoutParams(const StackLayoutParams& params)
{
  mImpl->SetLayoutParams(params);
}

bool ViewImpl::TryGetLayoutParams(AbsoluteLayoutParams& params) const
{
  return mImpl->TryGetLayoutParams(params);
}

bool ViewImpl::TryGetLayoutParams(FlexLayoutParams& params) const
{
  return mImpl->TryGetLayoutParams(params);
}

bool ViewImpl::TryGetLayoutParams(GridLayoutParams& params) const
{
  return mImpl->TryGetLayoutParams(params);
}

bool ViewImpl::TryGetLayoutParams(StackLayoutParams& params) const
{
  return mImpl->TryGetLayoutParams(params);
}

// =============================================================================
// VisualBase API
// =============================================================================

bool ViewImpl::AddVisual(Dali::Ui::VisualBase visualBase, Dali::Ui::Visual::ContainerRangeType containerRangeType)
{
  return mImpl->AddVisualObject(visualBase, static_cast<Dali::Ui::Integration::Visual::InternalContainerRangeType>(containerRangeType));
}

void ViewImpl::RemoveVisual(Dali::Ui::VisualBase visualBase)
{
  mImpl->RemoveVisualObject(visualBase);
}

uint32_t ViewImpl::GetVisualCount(Dali::Ui::Visual::ContainerRangeType containerRangeType) const
{
  return mImpl->GetVisualObjectCount(static_cast<Dali::Ui::Integration::Visual::InternalContainerRangeType>(containerRangeType));
}

Dali::Ui::VisualBase ViewImpl::GetVisualAt(Dali::Ui::Visual::ContainerRangeType containerRangeType, uint32_t siblingOrder) const
{
  return mImpl->GetVisualObjectAt(static_cast<Dali::Ui::Integration::Visual::InternalContainerRangeType>(containerRangeType), siblingOrder);
}

// =============================================================================
// From control-impl.cpp
// =============================================================================

ViewImpl::ViewImpl(ViewBehaviour behaviourFlags)
: CustomActorImpl(),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->SetBehaviourFlags(static_cast<Ui::ViewImpl::ViewBehaviour>(behaviourFlags));
}

void ViewImpl::Initialize()
{
  // Disable relayout for base View (derived classes can re-enable if needed)
  DevelActor::SetRelayoutEnabled(Self(), false);
  // Layout direction is owned by dali-core (Actor::SetLayoutDirection, plus the
  // inheritance walk that resolves it for a whole subtree), and the arranged x of
  // every non-standalone child is a function of it. Nothing in the layout pipeline
  // reports it on its own -- the RelayoutRequest core issues alongside the change
  // drives legacy size negotiation, not the dali-ui pass -- so a direction change
  // on a settled tree has to be turned into a layout invalidation explicitly.
  //
  // NO per-View signal connection is made here. That used to be the mechanism, and
  // it cost every View a callback object, the heap BaseSignal the signal member
  // lazily allocates on its first connect, and that signal's first connection-pool
  // block, whether or not the application ever changed a direction. Coverage comes
  // from four legs instead -- two mechanisms, one structural fact and one backstop:
  //
  //  (a) A change landing AT or ABOVE a layout root is observed by a LAZY actor
  //      signal hook the root makes, once, in
  //      Internal::ViewDataImpl::RegisterWithLayoutController() -- and only when it
  //      registers there with a LIVE WINDOW, so the cost falls on on-scene layout
  //      roots and on-scene standalone boundaries alone. Its handler walks the
  //      subtree itself, so one connection per on-scene layout root covers every
  //      descendant -- including the case no property hook of ours can see, a
  //      direction set on a non-View ancestor (an intermediate Layer, or the
  //      window's root layer).
  //  (b) A direction property WRITE on any View is intercepted by
  //      Internal::ViewDataImpl::OnPropertySet, which raises the same subtree walk.
  //      LAYOUT_DIRECTION DOES reach that hook: Object::SetProperty invokes the
  //      OnPropertySet virtual for core DEFAULT properties too, not only for
  //      registered ones, and Actor::SetLayoutDirection routes through
  //      SetProperty. This is what covers a mid-tree View that is not a layout
  //      root and therefore holds no hook of its own. It is window-independent, so
  //      it covers an off-scene write as well.
  //  (c) A direction moved while a subtree is OFF-SCENE needs no hook at all: no
  //      layout pass can run without a window, and the reconnection path
  //      (Internal::ViewDataImpl::OnViewSceneConnection) drops the whole subtree's
  //      cached measure and arrange results -- ResetSubtreeScaleAndLayoutCaches()
  //      -- before it registers the reconnecting root, so nothing stale can be
  //      served across the reconnection.
  //  (d) The arrange cache's recorded-direction KEY term (mLastArrangeDirection)
  //      remains the correctness backstop under all of it: the direction lives in
  //      dali-core and can be moved through actors dali-ui does not own, so a
  //      missed hook degrades to a cache MISS -- slower -- and never to an
  //      arrangement mirrored the wrong way round.
  //
  // The signal ORDER this changes is immaterial. The only other subscribers to the
  // actor's direction signal inside the library are three text controls, whose
  // handlers raise a controller state flag (Controller::ChangedLayoutDirection),
  // and ScrollViewImpl, whose handler forwards the direction to its scroll bar --
  // a property write that leg (b) intercepts -- so nothing depends on whether a
  // layout root's handler ran before or after them.
  //
  // One NARROW residual is accepted: core flips an added child's resolved
  // direction at Add() time AFTER OnChildAdded has already dropped the child's
  // caches and raised its dirty bits. A nested synchronous layout pass driven
  // from a child-added handler can therefore publish under the OLD direction with
  // nothing left to re-schedule. The recorded-direction key (leg (d)) then forces
  // a MISS on the next scheduled pass, so geometry is never wrong -- the update is
  // merely deferred to that pass.
  //
  // While the direction hook did live here, it was here rather than in
  // OnInitialize() because OnInitialize is virtual: a third-party subclass that
  // overrode it without up-calling would silently lose the hook.

  // Child order is the other actor-owned input to layout that only dali-core can
  // report. OnChildOrderChanged() rebuilds mChildren in actor order, tags the
  // reorder for transitions and invalidates measure; without it a RaiseToTop() or
  // LowerBelow() leaves mChildren in the old order AND leaves the measure/arrange
  // caches valid, so the settled subtree is replayed at the stale order instead of
  // being re-laid-out.
  //
  // That connection is NOT made here. It is made lazily, in
  // Internal::ViewDataImpl::OnChildAdded, at the moment this view gains its FIRST
  // tracked (View) child -- see the reasoning there. Connecting for every View cost a
  // leaf that never gains a child a heap callback, the signal's first connection-pool
  // block and a ConnectionTracker entry, for an event that could never concern it, and
  // that leaf is the common case in a large tree. The new site does not reintroduce the
  // virtual-OnInitialize hazard that kept the direction hook out of OnInitialize()
  // either. It is reached through ViewImpl::OnChildAdd, which IS a virtual and is not
  // final, so a subclass can displace it -- but a subclass that overrides OnChildAdd
  // without up-calling already forfeits mChildren synchronization itself, today, with
  // or without a connection here. The lazy connection therefore adds no exposure that
  // was not there before. It also still runs before any reorder of a tracked child is
  // possible, including for children a subclass adds from its own OnInitialize().

  // No VisualData here. The context is allocated lazily, by the first visual mutation
  // that actually needs it (EnsureVisualData), because every default View was paying a
  // heap allocation plus the context's own construction for a facility most views never
  // touch -- no background, no shadow, no borderline, no corner radius -- and in a large
  // tree those views are the bulk of the tree.
  //
  // DISABLE_VISUALS still means never-allocated, exactly as before; what widened is that
  // an ENABLED view is now null too until it first asks for a visual. Every read path
  // already answers a null context and an empty one identically (no visual found, count
  // 0, resources ready), so the widening is invisible to them -- the single place where
  // the two would have parted, GetVisualResourceStatus's fallback, now branches on
  // AreVisualsEnabled() to keep each case's original answer.

  Integration::ViewAccessibility::Register();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  // View(Internal::CustomActor*) rather than View::DownCast(Self()): the owner is
  // already live here (View::New constructs the handle before calling Initialize), and
  // the DownCast form paid two dynamic_casts and two extra handle constructions to
  // re-derive a type this function already knows. The constructor keeps the debug-build
  // check via VerifyCustomActorPointer<ViewImpl>.
  View view(GetOwner());
  view.SetLeaveRequired(true);
  // NOTE: UI layout coordinates are normally based on the parent's top-left,
  // while scale/rotation transform origins are normally centered. Keep
  // ParentOrigin as TOP_LEFT for placement and leave PIVOT unset here so the
  // DALi default CENTER pivot remains the View transform origin.
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION_USES_PIVOT, false);

  if(UiConfig::HasCurrent())
  {
    UiConfig::GetCurrent().GetViewInitializer()(view);
  }
  else
  {
    UiConfig::DefaultViewInitializer(view);
  }
}

void ViewImpl::ClearBackground()
{
  mImpl->ClearBackground();
}

void ViewImpl::SetShadow(const Shadow& shadow)
{
  mImpl->SetShadow(shadow);
}

void ViewImpl::SetShadow(const ShadowStack& shadowStack)
{
  mImpl->SetShadow(shadowStack);
}

void ViewImpl::SetInnerShadow(const InnerShadow& innerShadow)
{
  mImpl->SetInnerShadow(innerShadow);
}

void ViewImpl::ClearShadow()
{
  mImpl->ClearShadow();
}

void ViewImpl::SetRenderEffect(Ui::RenderEffect effect)
{
  mImpl->SetRenderEffect(effect);
}

RenderEffect ViewImpl::GetRenderEffect() const
{
  return mImpl->GetRenderEffect();
}

void ViewImpl::ClearRenderEffect()
{
  mImpl->ClearRenderEffect();
}

Internal::ViewDataImpl& ViewImpl::GetViewDataImpl() const
{
  return *mImpl;
}

Dali::Actor ViewImpl::GetOffScreenRenderableSourceActor()
{
  // Need to override this in FORWARD OffScreenRenderable
  return Dali::Actor();
}

bool ViewImpl::IsOffScreenRenderTaskExclusive()
{
  return false;
}

void ViewImpl::SetFocusNavigationCallback(FocusNavigationCallback callback)
{
  mImpl->SetFocusNavigationCallback(std::move(callback));
}

View ViewImpl::OnFocusRequested()
{
  return mImpl->ResolveDefaultFocusRequest();
}

bool ViewImpl::OnAccessibilityActivate()
{
  return mImpl->ActivateAccessibilityDefault();
}

bool ViewImpl::OnAccessibilityEscape()
{
  return false;
}

bool ViewImpl::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool ViewImpl::OnAccessibilityValueChange(bool isIncreased)
{
  return false; // Accessibility value change action is not handled by default
}

bool ViewImpl::OnAccessibilityScrollToChild(View child)
{
  return false;
}

bool ViewImpl::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

bool ViewImpl::OnAccessibilityRequestName(Dali::String& value)
{
  return false;
}

bool ViewImpl::OnAccessibilityRequestDefaultName(Dali::String& value)
{
  return false;
}

bool ViewImpl::OnAccessibilityRequestDescription(Dali::String& value)
{
  return false;
}

bool ViewImpl::OnAccessibilityRequestDefaultDescription(Dali::String& value)
{
  return false;
}

bool ViewImpl::OnAccessibilityRequestValue(Dali::String& value)
{
  return false;
}

FocusNavigationResult ViewImpl::OnFocusNavigationRequested(View currentFocusedView, FocusNavigationContext context)
{
  return FocusNavigationResult::NotHandled();
}

Ui::View::KeyEventSignalType& ViewImpl::KeyEventSignal()
{
  return mImpl->KeyEventSignal();
}

Ui::View::FocusChangedSignalType& ViewImpl::FocusChangedSignal()
{
  return mImpl->FocusChangedSignal();
}

Dali::Texture ViewImpl::GetOffScreenRenderingOutput() const
{
  return mImpl->GetOffScreenRenderingOutput();
}

void ViewImpl::OnSceneDisconnection()
{
  mImpl->OnViewSceneDisconnection();
}

void ViewImpl::OnChildAdd(Actor& child)
{
  mImpl->OnChildAdded(child, gAllowNonViewChildDepth > 0u);
}

void ViewImpl::OnChildRemove(Actor& child)
{
  mImpl->OnChildRemoved(child);
}

void ViewImpl::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  mImpl->OnPropertySet(index, propertyValue);
}

void ViewImpl::OnSizeSet(const Vector3& targetSize)
{
  mImpl->OnSizeSet(targetSize);
}

void ViewImpl::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  mImpl->OnSizeAnimation(animation);
}

void ViewImpl::OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Animation::State state)
{
  mImpl->OnAnimateAnimatableProperty(animation, index, state);
}

void ViewImpl::OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied)
{
  mImpl->OnConstraintAnimatableProperty(constraint, index, applied);
}

void ViewImpl::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  mImpl->GetOffScreenRenderTasks(tasks, isForward);
}

bool ViewImpl::IsResourceReady() const
{
  const Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(*this);
  return viewDataImpl.IsResourceReady();
}

void ViewImpl::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalConnected(slotObserver, callback);
}

void ViewImpl::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalDisconnected(slotObserver, callback);
}

namespace Integration
{
namespace View
{

void AddActorChild(Ui::View view, Dali::Actor actor)
{
  if(!view || !actor)
  {
    return;
  }

  AllowToAddActorToChildBegin(view);
  view.Add(actor);
  AllowToAddActorToChildEnd(view);
}

void AllowToAddActorToChildBegin(Ui::View view)
{
  ++gAllowNonViewChildDepth;
}

void AllowToAddActorToChildEnd(Ui::View view)
{
  if(gAllowNonViewChildDepth > 0u)
  {
    --gAllowNonViewChildDepth;
  }
}

} // namespace View
} // namespace Integration
} // namespace Ui
} // namespace Dali
