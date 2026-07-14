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
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h> // LCOV_EXCL_LINE
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/animation/constraint.h>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
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

/// Scoped guard that lets View::OnChildAdd accept a non-View Actor as a child.
/// Used by Integration::View::AddActorChild. Thread-local because Actor::Add invokes
/// OnChildAdd synchronously on the same (event) thread.
thread_local bool gAllowNonViewChild = false;

// RAII guard: restores the previous flag value on any exit path (including
// exceptions from view.Add()), and stays correct under nesting.
struct AllowNonViewChildScope
{
  const bool previous;
  AllowNonViewChildScope()
  : previous(gAllowNonViewChild)
  {
    gAllowNonViewChild = true;
  }
  ~AllowNonViewChildScope()
  {
    gAllowNonViewChild = previous;
  }

  AllowNonViewChildScope(const AllowNonViewChildScope&)                = delete;
  AllowNonViewChildScope& operator=(const AllowNonViewChildScope&)     = delete;
  AllowNonViewChildScope(AllowNonViewChildScope&&) noexcept            = default;
  AllowNonViewChildScope& operator=(AllowNonViewChildScope&&) noexcept = delete;
};

thread_local std::vector<AllowNonViewChildScope> gAllowScopeList;

} // namespace

namespace
{
BaseHandle Create()
{
  return View::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::ViewImpl, Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

void RegisterViewAccessibleGetter()
{
  static bool onceFlag = false;
  if(DALI_UNLIKELY(!onceFlag))
  {
    onceFlag = true;
    Dali::Accessibility::Accessible::RegisterExternalAccessibleGetter(
      [](Dali::Actor actor) -> std::pair<SharedPtr<Dali::Accessibility::Accessible>, bool>
    {
      auto view = Ui::View::DownCast(actor);
      if(!view)
      {
        return {SharedPtr<ViewAccessible>(), true};
      }

      auto& viewImpl = GetImpl(view);
      if(Dali::Ui::Internal::ViewDataImpl::Get(viewImpl).IsCreateAccessibleEnabled())
      {
        return {SharedPtr<ViewAccessible>(viewImpl.CreateAccessibleObject()), true};
      }
      return {SharedPtr<ViewAccessible>(), false};
    });
  }
}

} // namespace

ViewImplPtr ViewImpl::New()
{
  IntrusivePtr<ViewImpl> viewImpl = new ViewImpl();
  return ViewImplPtr(viewImpl);
}

ViewImpl::ViewImpl()
: CustomActorImpl(static_cast<ActorFlags>(
    static_cast<int>(VIEW_BEHAVIOUR_DEFAULT) |
    static_cast<int>(Dali::CustomActorImpl::DISABLE_SIZE_NEGOTIATION))),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->SetBehaviourFlags(static_cast<Ui::ViewImpl::ViewBehaviour>(
    static_cast<int>(VIEW_BEHAVIOUR_DEFAULT) |
    static_cast<int>(Dali::CustomActorImpl::DISABLE_SIZE_NEGOTIATION)));
}

ViewImpl::~ViewImpl()
{
  delete mImpl;
}

void ViewImpl::OnInitialize()
{
  DevelActor::ChildOrderChangedSignal(Self()).Connect(mImpl, &Internal::ViewDataImpl::OnChildOrderChanged);
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

// =============================================================================
// State API
// =============================================================================

const ViewState& ViewImpl::GetState() const
{
  return mImpl->GetState();
}

bool ViewImpl::IsEffectivelyEnabled() const
{
  return mImpl->IsEffectivelyEnabled();
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

void ViewImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  mImpl->RelayoutDefault(size, container);
}

Dali::LayoutDirection::Type ViewImpl::GetEffectiveLayoutDirection() const
{
  return mImpl->GetEffectiveLayoutDirection();
}

void ViewImpl::SetRequestedPositionX(float x)
{
  mImpl->SetRequestedPositionX(x);
}

void ViewImpl::SetRequestedPositionY(float y)
{
  mImpl->SetRequestedPositionY(y);
}

float ViewImpl::GetRequestedPositionX() const
{
  return mImpl->GetRequestedPositionX();
}

float ViewImpl::GetRequestedPositionY() const
{
  return mImpl->GetRequestedPositionY();
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

UiColor ViewImpl::GetColor() const
{
  return mImpl->GetColor();
}

void ViewImpl::SetColor(const UiColor& color)
{
  mImpl->SetColor(color);
}

UiColor ViewImpl::GetCurrentColor() const
{
  return mImpl->GetCurrentColor();
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

MeasuredSize ViewImpl::Arrange(const LayoutRect& bounds)
{
  return mImpl->Arrange(bounds);
}

MeasuredSize ViewImpl::OnArrange(const LayoutRect& bounds)
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
  mImpl->InvalidateMeasure();
}

void ViewImpl::InvalidateArrange()
{
  mImpl->InvalidateArrange();
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

void ViewImpl::AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager)
{
  mImpl->AttachLayoutManager(std::move(manager));
}

LayoutManager* ViewImpl::GetLayoutManager() const
{
  return mImpl->GetLayoutManager();
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

void ViewImpl::Insert(uint32_t index, Ui::View child)
{
  mImpl->Insert(index, child);
}

void ViewImpl::RemoveAllChildren()
{
  // No-argument form mirrors the inherited one-argument Actor::Remove:
  // unparent every child immediately, running no EXIT transition.
  RemoveAllChildren(Ui::RemovePolicy::IMMEDIATE);
}

void ViewImpl::RemoveAllChildren(Ui::RemovePolicy policy)
{
  mImpl->RemoveAllChildren(policy);
}

void ViewImpl::Remove(Ui::View child, Ui::RemovePolicy policy)
{
  mImpl->Remove(child, policy);
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

BaseHandle ViewImpl::GetLayoutParams(LayoutParamsType type) const
{
  return mImpl->GetLayoutParams(type);
}

void ViewImpl::SetLayoutParams(Ui::LayoutParams params)
{
  mImpl->SetLayoutParams(params);
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
: CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->SetBehaviourFlags(static_cast<Ui::ViewImpl::ViewBehaviour>(behaviourFlags));
}

void ViewImpl::Initialize()
{
  if(mImpl->AreVisualsEnabled())
  {
    mImpl->InitializeVisualData();
  }

  RegisterViewAccessibleGetter();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if(UiConfig::HasCurrent())
  {
    UiConfig::GetCurrent().GetViewInitializer()(View::DownCast(Self()));
  }
  else
  {
    UiConfig::DefaultViewInitializer(View::DownCast(Self()));
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

void ViewImpl::SetFocusNavigationCallback(Callback<View(View, FocusDirection)> callback)
{
  mImpl->SetFocusNavigationCallback(std::move(callback));
}

View ViewImpl::OnFocusRequested()
{
  return mImpl->ResolveDefaultFocusRequest();
}

bool ViewImpl::OnAccessibilityActivated()
{
  return mImpl->ActivateAccessibilityDefault();
}

bool ViewImpl::OnAccessibilityPan(PanGesture gesture)
{
  return false; // Accessibility pan gesture is not handled by default
}

bool ViewImpl::OnAccessibilityValueChange(bool isIncrease)
{
  return false; // Accessibility value change action is not handled by default
}

bool ViewImpl::OnAccessibilityZoom()
{
  return false; // Accessibility zoom action is not handled by default
}

ViewAccessible* ViewImpl::CreateAccessibleObject()
{
  return mImpl->CreateDefaultAccessibleObject();
}

View ViewImpl::OnFocusNavigationRequested(View currentFocusedView, FocusDirection direction)
{
  return View();
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
  mImpl->OnChildAdded(child, gAllowNonViewChild);
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

void ViewImpl::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 ViewImpl::GetNaturalSize()
{
  return mImpl->GetNaturalSize();
}

float ViewImpl::CalculateChildSize(const Dali::Actor& child, Dimension::Type dimension)
{
  return CalculateChildSizeBase(child, dimension);
}

float ViewImpl::GetHeightForWidth(float width)
{
  return GetHeightForWidthBase(width);
}

float ViewImpl::GetWidthForHeight(float height)
{
  return GetWidthForHeightBase(height);
}

bool ViewImpl::RelayoutDependentOnChildren(Dimension::Type dimension)
{
  return RelayoutDependentOnChildrenBase(dimension);
}

void ViewImpl::OnCalculateRelayoutSize(Dimension::Type dimension)
{
}

void ViewImpl::OnLayoutNegotiated(float size, Dimension::Type dimension)
{
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
  gAllowScopeList.emplace_back();
}

void AllowToAddActorToChildEnd(Ui::View view)
{
  gAllowScopeList.pop_back();
}

} // namespace View
} // namespace Integration
} // namespace Ui
} // namespace Dali
