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
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/property-helper-devel.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/size-negotiation/relayout-container.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stack>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/integration-api/interactive-trait-impl.h>
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-callbacks-object.h>
#include <dali-ui-foundation/internal/layouts/layout-manager-object.h>
#include <dali-ui-foundation/internal/layouts/layout-params-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-reflow-resolver.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali-ui-foundation/internal/ui-color-manager-impl.h>
#include <dali-ui-foundation/internal/ui-localization-manager-impl.h>
#include <dali-ui-foundation/internal/views/state-handler-trait.h>
#include <dali-ui-foundation/internal/views/view-state-manager.h>
#include <dali-ui-foundation/internal/views/view/view-accessibility-data.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/views/view/view-visual-data.h>
#include <dali-ui-foundation/internal/visuals/color/color-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-string-constants.h>
#include <dali-ui-foundation/public-api/align-enumerations.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/image-view.h>
#include <dali-ui-foundation/public-api/layouts/layout-params.h>
#include <dali-ui-foundation/public-api/render-effects/render-effect.h>
#include <dali-ui-foundation/public-api/trait-id.h>
#include <dali-ui-foundation/public-api/trait-object.h>
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

// Verify CornerRadiusPolicy values stay in sync with Ui::Visual::Policy::Type.
static_assert(static_cast<int>(Dali::Ui::CornerRadiusPolicy::RELATIVE) == Dali::Ui::Visual::Transform::Policy::RELATIVE);
static_assert(static_cast<int>(Dali::Ui::CornerRadiusPolicy::ABSOLUTE) == Dali::Ui::Visual::Transform::Policy::ABSOLUTE);

namespace Dali
{

namespace Ui
{

namespace
{

/// Scoped guard that lets View::OnChildAdd accept a non-View Actor as a child.
/// Used by IntegrationView::AddActorChild. Thread-local because Actor::Add invokes
/// OnChildAdd synchronously on the same (event) thread.
thread_local bool gAllowNonViewChild = false;

IntrusivePtr<TraitObject> ToTraitObject(BaseHandle handle)
{
  if(!handle)
  {
    return nullptr;
  }

  auto* traitObject = dynamic_cast<TraitObject*>(handle.GetObjectPtr());
  DALI_ASSERT_ALWAYS(traitObject && "Handle used as a View trait must wrap a TraitObject");
  return traitObject ? IntrusivePtr<TraitObject>(traitObject) : nullptr;
}

// mLastMeasuredConstraint encodes three states:
//   NaN                        : initial state before any measure
//   MEASURE_CACHE_DIRTY (-1.0f): was measured, then invalidated
//   positive values            : last effective constraint (cache hit check)
//
// InvalidateMeasure's early-exit guard skips propagation when the view
// is already DIRTY — the ancestor chain has already been invalidated
// and the layout root already registered. A view in the initial state
// must still propagate on its first invalidation; this works because
// Dali::Equals returns false for any NaN comparison (IEEE 754), so
// the guard naturally bypasses it without a special-case check.
constexpr float MEASURE_CACHE_DIRTY = -1.0f;

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

/// RAII guard for ViewImpl::mSkipChildrenUpdate. Saves and restores the
/// previous value, so nested scopes (e.g. a signal handler re-entering a
/// child-list-managing call while an outer PRESERVE Raise is active) cannot
/// prematurely unguard the outer scope.
class ScopedSkipChildrenUpdate
{
public:
  explicit ScopedSkipChildrenUpdate(bool& flag)
  : mFlag(flag),
    mPrev(flag)
  {
    mFlag = true;
  }
  ~ScopedSkipChildrenUpdate()
  {
    mFlag = mPrev;
  }
  ScopedSkipChildrenUpdate(const ScopedSkipChildrenUpdate&)            = delete;
  ScopedSkipChildrenUpdate& operator=(const ScopedSkipChildrenUpdate&) = delete;

private:
  bool& mFlag;
  bool  mPrev;
};

BaseHandle Create()
{
  return View::New();
}

// Type Registration
DALI_TYPE_REGISTRATION_BEGIN(Ui::ViewImpl, Ui::View, Create)
DALI_TYPE_REGISTRATION_END()

inline bool FloatEqual(float a, float b, float epsilon = 0.001f)
{
  return std::abs(a - b) < epsilon;
}

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

void CreateClippingRenderer(ViewImpl& viewImpl)
{
  Actor self(viewImpl.Self());
  int   clippingMode = ClippingMode::DISABLED;
  if(self.GetProperty(Actor::Property::CLIPPING_MODE).Get(clippingMode))
  {
    Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(viewImpl);
    if(clippingMode == ClippingMode::CLIP_CHILDREN &&
       (DALI_UNLIKELY(!viewDataImpl.mVisualData) || viewDataImpl.mVisualData->mVisuals.Empty()) &&
       self.GetRendererCount() == 0u)
    {
      viewImpl.SetBackgroundColor(Color::TRANSPARENT);
    }
  }
}

void RegisterViewAccessibleGetter()
{
  static bool onceFlag = false;
  if(DALI_UNLIKELY(!onceFlag))
  {
    onceFlag = true;
    Accessibility::Accessible::RegisterExternalAccessibleGetter(
      [](Dali::Actor actor) -> std::pair<SharedPtr<Accessibility::Accessible>, bool>
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

Internal::LayoutCallbacksObject* GetLayoutCallbacksObject(ViewImpl* self)
{
  IntrusivePtr<TraitObject> object = Internal::ViewDataImpl::Get(*self).GetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS);
  return dynamic_cast<Internal::LayoutCallbacksObject*>(object.Get());
}

Internal::LayoutCallbacksObject* EnsureLayoutCallbacksObject(ViewImpl* self)
{
  auto* object = GetLayoutCallbacksObject(self);
  if(!object)
  {
    IntrusivePtr<TraitObject> newObject(new Internal::LayoutCallbacksObject());
    object = static_cast<Internal::LayoutCallbacksObject*>(newObject.Get());
    Internal::ViewDataImpl::Get(*self).SetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS, newObject);
  }
  return object;
}

template<typename HandleType>
HandleType GetTraitHandle(const ViewImpl& viewImpl, TraitId id)
{
  IntrusivePtr<TraitObject> object     = IntegrationView::GetTrait(viewImpl, id);
  auto*                     baseObject = dynamic_cast<BaseObject*>(object.Get());
  return baseObject ? HandleType::DownCast(BaseHandle(baseObject)) : HandleType();
}

// Arranges a standalone MATCH_PARENT child within its parent.
// Standalone children ignore parent padding. MATCH_PARENT axes are
// expanded to the full parent size minus the child's own margin, then
// re-measured with the final size before arranging.
void ArrangeStandaloneChild(ViewImpl& childImpl,
                            float parentFullWidth, float parentFullHeight)
{
  float        childScale = childImpl.GetEffectiveScale();
  Extents      margin     = childImpl.GetMargin();
  float        marginW    = static_cast<float>(margin.start + margin.end) * childScale;
  float        marginH    = static_cast<float>(margin.top + margin.bottom) * childScale;
  MeasuredSize measured   = childImpl.GetMeasuredSize();
  float        childW     = measured.width;
  float        childH     = measured.height;

  if(childImpl.GetRequestedWidth() == MATCH_PARENT)
  {
    childW = std::max(0.0f, parentFullWidth - marginW);
  }
  if(childImpl.GetRequestedHeight() == MATCH_PARENT)
  {
    childH = std::max(0.0f, parentFullHeight - marginH);
  }
  if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
  {
    childImpl.Measure(childW, childH);
  }

  LayoutRect bounds(childImpl.GetRequestedPositionX() * childScale + static_cast<float>(margin.start) * childScale,
                    childImpl.GetRequestedPositionY() * childScale + static_cast<float>(margin.top) * childScale,
                    childW, childH);
  childImpl.Arrange(bounds);
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
  mImpl->mFlags = static_cast<Ui::ViewImpl::ViewBehaviour>(
    static_cast<int>(VIEW_BEHAVIOUR_DEFAULT) |
    static_cast<int>(Dali::CustomActorImpl::DISABLE_SIZE_NEGOTIATION));
}

ViewImpl::~ViewImpl()
{
  delete mImpl;
}

void ViewImpl::OnInitialize()
{
  DevelActor::ChildOrderChangedSignal(Self()).Connect(this, &ViewImpl::OnChildOrderChanged);
}

void ViewImpl::OnDestroy()
{
  auto colorManager = UiColorManager::Get();
  if(colorManager)
  {
    GetImpl(colorManager).ClearBindings(GetOwner());
  }

  auto localizationManager = UiLocalizationManager::Get();
  if(localizationManager)
  {
    GetImpl(localizationManager).ClearBindings(GetOwner());
  }

  mImpl->NotifyTraitsViewDestroying();

  LayoutController::UnregisterFromAll(this);

  ClearRenderEffect();

  RefObject::OnDestroy();
}

void ViewImpl::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection();

  if(auto* interactiveTrait = mImpl->GetInteractiveTrait())
  {
    interactiveTrait->OnSceneConnection(View::DownCast(Self()));
  }

  CreateClippingRenderer(*this);

  // Register as a layout root if this view is:
  //   (a) the top of the view tree (no parent view), OR
  //   (b) a standalone (boundary) view whose invalidation does not propagate
  //       to its parent. Such a view must self-register so its pending layout
  //       work is processed by the LayoutController on the current window.
  //
  // The boundary case matters when a view becomes dirty while off-scene
  // (RegisterWithLayoutController silently no-ops without a window). Once
  // connected to a scene here, it must register so the pending state is
  // picked up in the new window's controller.
  const bool isDirty = Dali::Equals(mImpl->mLastMeasuredConstraint.width, MEASURE_CACHE_DIRTY) ||
                       mImpl->mArrangeDirty;
  if(!GetParentView() || (IntegrationView::IsLayoutModeStandalone(*this) && isDirty))
  {
    RegisterWithLayoutController();
  }
  else if(IntegrationView::IsLayoutModeStandalone(*this))
  {
    // Standalone but clean: no layout pass needed, but must be tracked by
    // UiScaleManager so future scale changes reach this view. This covers the
    // case where a standalone view was unregistered on scene-disconnection and
    // reconnects without any pending dirty work.
    Window window = DevelWindow::Get(Self());
    if(window)
    {
      UiScaleManager::Get().RegisterLayoutRoot(Ui::View::DownCast(Self()));
    }
  }
}

bool ViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if(auto* interactiveTrait = mImpl->GetInteractiveTrait())
  {
    return interactiveTrait->OnKeyEvent(View::DownCast(Self()), event);
  }
  return false;
}

// =============================================================================
// State API
// =============================================================================

const ViewState& ViewImpl::GetState() const
{
  return mImpl->mState;
}

bool ViewImpl::IsEnabled() const
{
  return Self().GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED);
}

void ViewImpl::SetEnabled(bool enabled)
{
  Self().SetProperty(DevelActor::Property::USER_INTERACTION_ENABLED, enabled);
}

bool ViewImpl::IsEffectivelyEnabled() const
{
  return Internal::ViewStateManager::Get().IsEffectivelyEnabled(*this);
}

bool ViewImpl::IsEffectivelyFocused() const
{
  return Internal::ViewStateManager::Get().IsEffectivelyFocused(*this);
}

ViewImpl::StateChangedSignalType& ViewImpl::StateChangedSignal()
{
  return mImpl->mStateChangedSignal;
}

Ui::InteractiveTrait ViewImpl::EnsureInteractiveTrait()
{
  Ui::InteractiveTrait existing = GetTraitHandle<Ui::InteractiveTrait>(*this, Integration::ReservedTraitId::INTERACTION_TRAIT);

  if(!existing)
  {
    Ui::InteractiveTrait interaction = Ui::InteractiveTrait::New();
    IntegrationView::SetTrait(*this, Integration::ReservedTraitId::INTERACTION_TRAIT, ToTraitObject(interaction));
    return interaction;
  }

  return existing;
}

bool ViewImpl::IsInteractive() const
{
  return !!IntegrationView::GetTrait(*this, Integration::ReservedTraitId::INTERACTION_TRAIT);
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
  Ui::SelectableTrait existing = GetTraitHandle<Ui::SelectableTrait>(*this, Integration::ReservedTraitId::SELECTABLE_TRAIT);

  if(!existing)
  {
    Ui::SelectableTrait selectable = Ui::SelectableTrait::New();
    IntegrationView::SetTrait(*this, Integration::ReservedTraitId::SELECTABLE_TRAIT, ToTraitObject(selectable));
    return selectable;
  }

  return existing;
}

bool ViewImpl::IsSelectable() const
{
  return !!IntegrationView::GetTrait(*this, Integration::ReservedTraitId::SELECTABLE_TRAIT);
}

void ViewImpl::NotifyFocusChanged(bool focused)
{
  OnFocusChanged(focused);
}

void ViewImpl::OnFocusChanged(bool focused)
{
  InputEvent cause;
  auto       focusManager = Ui::FocusManager::Get();
  if(focusManager)
  {
    cause = GetImpl(focusManager).FocusChangedContext().inputEvent;
  }
  IntegrationView::SetState(*this, ViewState::FOCUSED, focused, cause);

  if(auto* interactiveTrait = mImpl->GetInteractiveTrait())
  {
    interactiveTrait->OnFocusedChanged(View::DownCast(Self()), focused);
  }

  EmitFocusChangedSignal(focused);
}

void ViewImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(IntegrationView::HasLayoutCapability(*this) || GetParentLayout() || GetParentView())
  {
    return;
  }

  if((mImpl->mPadding.start != 0) || (mImpl->mPadding.end != 0) || (mImpl->mPadding.top != 0) ||
     (mImpl->mPadding.bottom != 0) || (mImpl->mMargin.start != 0) || (mImpl->mMargin.end != 0) ||
     (mImpl->mMargin.top != 0) || (mImpl->mMargin.bottom != 0))
  {
    for(unsigned int i = 0, numChildren = Self().GetChildCount(); i < numChildren; ++i)
    {
      Actor   child = Self().GetChildAt(i);
      Vector2 newChildSize(size);

      Extents padding = mImpl->mPadding;

      Dali::LayoutDirection::Type layoutDirection = GetEffectiveLayoutDirection();

      if(Dali::LayoutDirection::RIGHT_TO_LEFT == layoutDirection)
      {
        std::swap(padding.start, padding.end);
      }

      newChildSize.width  = size.width - (padding.start + padding.end);
      newChildSize.height = size.height - (padding.top + padding.bottom);

      Vector2 childOffset(0.f, 0.f);
      childOffset.x += (mImpl->mMargin.start + padding.start);
      childOffset.y += (mImpl->mMargin.top + padding.top);

      child.SetProperty(Actor::Property::POSITION, Vector2(childOffset.x, childOffset.y));

      container.Add(child, newChildSize);
    }
  }

  if(Accessibility::IsUp())
  {
    auto accessible = GetViewDataImpl().GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      auto highlightFrame = accessible->GetHighlightActor();
      if(accessible->GetCurrentlyHighlightedActor() == this->Self() &&
         highlightFrame.GetProperty<Vector3>(Dali::Actor::Property::SIZE).GetVectorXY() != size)
      {
        highlightFrame.SetProperty(Actor::Property::SIZE, size);
        container.Add(highlightFrame, size);
      }
    }
  }

  mImpl->ApplyFittingMode(size);
}

// =============================================================================
// API (size, position, parent origin, pivot)
// =============================================================================

float ViewImpl::GetScaleX() const
{
  return Self().GetProperty<float>(Actor::Property::SCALE_X);
}

float ViewImpl::GetCurrentScaleX() const
{
  return Self().GetCurrentProperty<float>(Actor::Property::SCALE_X);
}

void ViewImpl::SetScaleX(float scaleX)
{
  Self().SetProperty(Actor::Property::SCALE_X, scaleX);
}

float ViewImpl::GetScaleY() const
{
  return Self().GetProperty<float>(Actor::Property::SCALE_Y);
}

float ViewImpl::GetCurrentScaleY() const
{
  return Self().GetCurrentProperty<float>(Actor::Property::SCALE_Y);
}

void ViewImpl::SetScaleY(float scaleY)
{
  Self().SetProperty(Actor::Property::SCALE_Y, scaleY);
}

void ViewImpl::SetLayoutDirection(Dali::LayoutDirection::Type direction)
{
  Self().SetProperty(Actor::Property::LAYOUT_DIRECTION, direction);
  // Direction only affects child placement, not measured sizes.
  InvalidateArrange();
}

void ViewImpl::ClearLayoutDirection()
{
  Self().SetProperty(Actor::Property::INHERIT_LAYOUT_DIRECTION, true);
  InvalidateArrange();
}

bool ViewImpl::IsLayoutDirectionInherited() const
{
  return Self().GetProperty<bool>(Actor::Property::INHERIT_LAYOUT_DIRECTION);
}

Dali::LayoutDirection::Type ViewImpl::GetEffectiveLayoutDirection() const
{
  return static_cast<Dali::LayoutDirection::Type>(Self().GetProperty<int>(Actor::Property::LAYOUT_DIRECTION));
}

bool ViewImpl::IsVisible() const
{
  return Self().GetProperty<float>(Actor::Property::VISIBLE);
}

void ViewImpl::SetVisibility(bool visibility)
{
  Self().SetProperty(Actor::Property::VISIBLE, visibility);
}

float ViewImpl::GetOpacity() const
{
  return Self().GetProperty<float>(Actor::Property::OPACITY);
}

void ViewImpl::SetOpacity(float opacity)
{
  Self().SetProperty(Actor::Property::OPACITY, opacity);
}

MeasuredSize ViewImpl::GetSize() const
{
  return MeasuredSize(Self().GetProperty<float>(Actor::Property::SIZE_WIDTH),
                      Self().GetProperty<float>(Actor::Property::SIZE_HEIGHT));
}

MeasuredSize ViewImpl::GetCurrentSize() const
{
  return MeasuredSize(Self().GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH),
                      Self().GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT));
}

float ViewImpl::GetPositionX() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_X);
}

float ViewImpl::GetCurrentPositionX() const
{
  return Self().GetCurrentProperty<float>(Actor::Property::POSITION_X);
}

float ViewImpl::GetPositionY() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_Y);
}

float ViewImpl::GetCurrentPositionY() const
{
  return Self().GetCurrentProperty<float>(Actor::Property::POSITION_Y);
}

void ViewImpl::SetRequestedPositionX(float x)
{
  if(!Dali::Equals(mImpl->mRequestedPositionX, x))
  {
    mImpl->mRequestedPositionX = x;
    // InvalidateMeasure (not InvalidateArrange): a WRAP_CONTENT parent's
    // OnMeasure reads the child's RequestedPosition into maxRight/maxBottom,
    // so a position change can affect the parent's measured size. Measure
    // invalidation also marks the chain dirty for Arrange.
    InvalidateMeasure();
  }
}

void ViewImpl::SetRequestedPositionY(float y)
{
  if(!Dali::Equals(mImpl->mRequestedPositionY, y))
  {
    mImpl->mRequestedPositionY = y;
    InvalidateMeasure();
  }
}

float ViewImpl::GetRequestedPositionX() const
{
  return mImpl->mRequestedPositionX;
}

float ViewImpl::GetRequestedPositionY() const
{
  return mImpl->mRequestedPositionY;
}

Vector3 ViewImpl::GetParentOrigin() const
{
  return Self().GetProperty<Vector3>(Actor::Property::PARENT_ORIGIN);
}

void ViewImpl::SetParentOrigin(const Vector3& point)
{
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, point);
}

Vector3 ViewImpl::GetPivot() const
{
  return Self().GetProperty<Vector3>(Actor::Property::PIVOT);
}

void ViewImpl::SetPivot(const Vector3& point)
{
  Self().SetProperty(Actor::Property::PIVOT, point);
}

UiColor ViewImpl::GetBackgroundColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "BackgroundColor", outColor))
  {
    return outColor;
  }
  return mImpl->mBackgroundColor;
}

void ViewImpl::SetBackgroundColor(const UiColor& color)
{
  SetColorBinding("BackgroundColor", color, this, &ViewImpl::SetBackgroundColorInternal);
}

UiColor ViewImpl::GetColor() const
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "Color", outColor))
  {
    return outColor;
  }
  return UiColor(Self().GetProperty<Vector4>(Actor::Property::COLOR));
}

void ViewImpl::SetColor(const UiColor& color)
{
  SetColorBinding("Color", color, this, &ViewImpl::SetColorInternal);
}

UiColor ViewImpl::GetCurrentColor() const
{
  return UiColor(Self().GetCurrentProperty<Vector4>(Actor::Property::COLOR));
}

void ViewImpl::SetColorInternal(const Vector4& color)
{
  Self().SetProperty(Actor::Property::COLOR, color);
}

Vector4 ViewImpl::GetCornerRadius() const
{
  return Self().GetProperty<Vector4>(Ui::View::Property::CORNER_RADIUS);
}

void ViewImpl::SetCornerRadius(const Vector4& radius)
{
  Self().SetProperty(Ui::View::Property::CORNER_RADIUS, radius);
}

CornerRadiusPolicy ViewImpl::GetCornerRadiusPolicy() const
{
  return static_cast<CornerRadiusPolicy>(Self().GetProperty<int>(Ui::View::Property::CORNER_RADIUS_POLICY));
}

void ViewImpl::SetCornerRadiusPolicy(CornerRadiusPolicy policy)
{
  Self().SetProperty(Ui::View::Property::CORNER_RADIUS_POLICY, static_cast<int>(policy));
}

Vector4 ViewImpl::GetCornerSquareness() const
{
  return Self().GetProperty<Vector4>(Ui::View::Property::CORNER_SQUARENESS);
}

void ViewImpl::SetCornerSquareness(const Vector4& squareness)
{
  Self().SetProperty(Ui::View::Property::CORNER_SQUARENESS, squareness);
}

float ViewImpl::GetBorderlineWidth() const
{
  return Self().GetProperty<float>(Ui::View::Property::BORDERLINE_WIDTH);
}

void ViewImpl::SetBorderlineWidth(float width)
{
  Self().SetProperty(Ui::View::Property::BORDERLINE_WIDTH, width);
}

UiColor ViewImpl::GetBorderlineColor()
{
  UiColor outColor;
  if(UiColorManager::Get().GetBindingColor(Self(), "BorderlineColor", outColor))
  {
    return outColor;
  }
  return Self().GetProperty<Vector4>(Ui::View::Property::BORDERLINE_COLOR);
}

void ViewImpl::SetBorderlineColor(const UiColor& color)
{
  SetColorBinding("BorderlineColor", color, this, &ViewImpl::SetBorderlineColorInternal);
}

void ViewImpl::SetBorderlineColorInternal(const Vector4& color)
{
  Self().SetProperty(Ui::View::Property::BORDERLINE_COLOR, color);
}

float ViewImpl::GetBorderlineOffset() const
{
  return Self().GetProperty<float>(Ui::View::Property::BORDERLINE_OFFSET);
}

void ViewImpl::SetBorderlineOffset(float offset)
{
  Self().SetProperty(Ui::View::Property::BORDERLINE_OFFSET, offset);
}

Dali::String ViewImpl::GetName() const
{
  return Self().GetProperty<Dali::String>(Dali::Actor::Property::NAME);
}

void ViewImpl::SetName(const Dali::String& name)
{
  Self().SetProperty(Dali::Actor::Property::NAME, name);
}

bool ViewImpl::IsFocusable() const
{
  return Self().GetProperty<bool>(Actor::Property::KEYBOARD_FOCUSABLE);
}

void ViewImpl::SetFocusable(bool focusable)
{
  Self().SetProperty(Actor::Property::KEYBOARD_FOCUSABLE, focusable);
}

bool ViewImpl::IsTouchFocusable() const
{
  return Self().GetProperty<bool>(DevelActor::Property::TOUCH_FOCUSABLE);
}

void ViewImpl::SetTouchFocusable(bool touchFocusable)
{
  Self().SetProperty(DevelActor::Property::TOUCH_FOCUSABLE, touchFocusable);
}

// =============================================================================
// Measure / Arrange API
// =============================================================================

MeasuredSize ViewImpl::Measure(float visualW, float visualH)
{
  float s = GetEffectiveScale();

  // Push effective scale to the actor animatable property so that decoration
  // constraints (corner radius, borderline width) can read it as a scale input.
  // Read back the current actor property value to skip redundant scene-graph writes.
  // This also naturally corrects any value set externally on EFFECTIVE_SCALE.
  if(!Dali::Equals(s, Self().GetProperty<float>(Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX)))
  {
    // SetProperty triggers ViewDataImpl::SetProperty(VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), which:
    //   - updates the actor animatable so decoration constraints re-evaluate, and
    //   - calls UpdateCornerRadius() for active RenderEffect / OffScreenRendering.
    Self().SetProperty(Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX, s);
  }

  float natW = (visualW > 0.f) ? visualW / s : visualW;
  float natH = (visualH > 0.f) ? visualH / s : visualH;

  // Ensure constraints respect this view's min/max bounds so that
  // OnMeasure (and therefore child measurements) see the effective
  // available space. Without this, children would be measured against
  // the original (smaller) constraint, then ApplyConstraints would
  // enlarge the result — leaving children incorrectly sized.
  float effNatW = std::min(std::max(natW, mImpl->GetMinimumWidth()), mImpl->GetMaximumWidth());
  float effNatH = std::min(std::max(natH, mImpl->GetMinimumHeight()), mImpl->GetMaximumHeight());

  if(mImpl->mLastMeasuredConstraint.width >= 0.0f && FloatEqual(mImpl->mLastMeasuredConstraint.width, effNatW) &&
     FloatEqual(mImpl->mLastMeasuredConstraint.height, effNatH))
  {
    return mImpl->mMeasuredSize;
  }

  // OnMeasure receives and returns visual (scale-applied) sizes, consistent with OnArrange.
  float        effVisW = (effNatW >= 0.f) ? effNatW * s : effNatW;
  float        effVisH = (effNatH >= 0.f) ? effNatH * s : effNatH;
  MeasuredSize visual;
  if(auto* callback = GetMeasureCallback())
  {
    Ui::View view = Ui::View::DownCast(Self());
    visual        = callback->Invoke(view, effVisW, effVisH);
  }
  else if(auto* manager = GetLayoutManager())
  {
    visual = DispatchMeasureWithLayoutManager(manager, effVisW, effVisH);
  }
  else
  {
    visual = OnMeasure(effVisW, effVisH);
  }
  visual                                = ApplyConstraints(visual);
  mImpl->mMeasuredSize.width            = visual.width;
  mImpl->mMeasuredSize.height           = visual.height;
  mImpl->mLastMeasuredConstraint.width  = effNatW;
  mImpl->mLastMeasuredConstraint.height = effNatH;

  // Ensure standalone children are measured even when OnMeasure (e.g. in
  // leaf views like Label) does not iterate children. The measure cache
  // prevents redundant work when OnMeasure already measured them.
  MeasureStandaloneChildren(effVisW, effVisH);

  return mImpl->mMeasuredSize;
}

MeasuredSize ViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  // widthConstraint/heightConstraint are visual (scale-applied) sizes.
  // Convert to natural for internal computation; return visual at the end.
  float s    = GetEffectiveScale();
  float natW = (widthConstraint >= 0.f && s > 0.f) ? widthConstraint / s : widthConstraint;
  float natH = (heightConstraint >= 0.f && s > 0.f) ? heightConstraint / s : heightConstraint;

  float pw = static_cast<float>(mImpl->mPadding.start + mImpl->mPadding.end);
  float ph = static_cast<float>(mImpl->mPadding.top + mImpl->mPadding.bottom);

  float effectiveWidth  = (mImpl->mRequestedWidth >= 0) ? mImpl->mRequestedWidth : natW;
  float effectiveHeight = (mImpl->mRequestedHeight >= 0) ? mImpl->mRequestedHeight : natH;

  float contentWidth  = std::max(0.0f, effectiveWidth - pw);
  float contentHeight = std::max(0.0f, effectiveHeight - ph);

  if(!mImpl->mChildren.Empty())
  {
    float maxRight  = 0.0f;
    float maxBottom = 0.0f;
    for(auto& childView : mImpl->mChildren)
    {
      ViewImpl& childImpl = GetImpl(childView);

      // Standalone children are measured by MeasureStandaloneChildren()
      // in ViewImpl::Measure() after OnMeasure returns.
      if(IntegrationView::IsLayoutModeStandalone(childImpl))
      {
        continue;
      }

      float        childScale            = childImpl.GetEffectiveScale();
      Extents      margin                = childImpl.GetMargin();
      float        marginW               = static_cast<float>(margin.start + margin.end) * childScale;
      float        marginH               = static_cast<float>(margin.top + margin.bottom) * childScale;
      float        childWidthConstraint  = std::max(0.0f, contentWidth * s - marginW);
      float        childHeightConstraint = std::max(0.0f, contentHeight * s - marginH);
      MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

      float childNatW  = (s > 0.0f) ? childSize.width / s : childSize.width;
      float childNatH  = (s > 0.0f) ? childSize.height / s : childSize.height;
      float childX     = childImpl.GetRequestedPositionX();
      float childY     = childImpl.GetRequestedPositionY();
      float natMarginW = (s > 0.0f) ? marginW / s : marginW;
      float natMarginH = (s > 0.0f) ? marginH / s : marginH;
      maxRight         = std::max(maxRight, childX + natMarginW + childNatW);
      maxBottom        = std::max(maxBottom, childY + natMarginH + childNatH);
    }

    MeasuredSize size;
    if(mImpl->mRequestedWidth >= 0)
    {
      size.width = mImpl->mRequestedWidth;
    }
    else if(mImpl->mRequestedWidth == MATCH_PARENT)
    {
      size.width = mImpl->GetMinimumWidth();
    }
    else
    {
      size.width = maxRight + pw;
    }
    if(mImpl->mRequestedHeight >= 0)
    {
      size.height = mImpl->mRequestedHeight;
    }
    else if(mImpl->mRequestedHeight == MATCH_PARENT)
    {
      size.height = mImpl->GetMinimumHeight();
    }
    else
    {
      size.height = maxBottom + ph;
    }
    return {size.width * s, size.height * s};
  }

  MeasuredSize size;
  if(mImpl->mRequestedWidth >= 0)
  {
    size.width = mImpl->mRequestedWidth;
  }
  else if(mImpl->mRequestedWidth == MATCH_PARENT)
  {
    size.width = mImpl->GetMinimumWidth();
  }
  else
  {
    Vector3 naturalSize = Self().GetNaturalSize();
    size.width          = ((naturalSize.width > 0) ? naturalSize.width : 0.0f) + pw;
  }
  if(mImpl->mRequestedHeight >= 0)
  {
    size.height = mImpl->mRequestedHeight;
  }
  else if(mImpl->mRequestedHeight == MATCH_PARENT)
  {
    size.height = mImpl->GetMinimumHeight();
  }
  else
  {
    Vector3 naturalSize = Self().GetNaturalSize();
    size.height         = ((naturalSize.height > 0) ? naturalSize.height : 0.0f) + ph;
  }
  return {size.width * s, size.height * s};
}

MeasuredSize ViewImpl::Arrange(const LayoutRect& bounds)
{
  MeasuredSize arrangedSize;
  if(auto* callback = GetArrangeCallback())
  {
    arrangedSize = DispatchArrangeWithCallback(callback, bounds);
  }
  else if(auto* manager = GetLayoutManager())
  {
    arrangedSize = DispatchArrangeWithLayoutManager(manager, bounds);
  }
  else
  {
    arrangedSize = OnArrange(bounds);
  }
  mImpl->mArrangedBounds = bounds;
  mImpl->mArrangeDirty   = false;

  // Ensure standalone children are arranged even when OnArrange (e.g. in
  // leaf views like Label) does not iterate children.
  ArrangeStandaloneChildren(bounds);

  // Mirror direct children when the effective layout direction resolves to
  // RIGHT_TO_LEFT. Runs once per Arrange after every OnArrange variant
  // (LayoutManager / ArrangeCallback / default), keeping layout managers
  // direction-agnostic.
  ApplyLayoutDirection(bounds.width);

  // Mark this view as having completed an arrange pass. Read by the layout
  // transition dispatcher to suppress ENTER on initial mount: the dispatcher
  // records views that were captured before this flag became true and
  // settles their declarative ENTER specs to final values without firing
  // OnStart / OnFinished.
  mImpl->mInitialLayoutDone = true;
  return arrangedSize;
}

MeasuredSize ViewImpl::OnArrange(const LayoutRect& bounds)
{
  float x      = bounds.x;
  float y      = bounds.y;
  float width  = bounds.width;
  float height = bounds.height;

  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, x);
  self.SetProperty(Actor::Property::POSITION_Y, y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, height);

  if(!mImpl->mChildren.Empty())
  {
    float s            = GetEffectiveScale();
    float visPadLeft   = static_cast<float>(mImpl->mPadding.start) * s;
    float visPadRight  = static_cast<float>(mImpl->mPadding.end) * s;
    float visPadTop    = static_cast<float>(mImpl->mPadding.top) * s;
    float visPadBottom = static_cast<float>(mImpl->mPadding.bottom) * s;

    for(auto& childView : mImpl->mChildren)
    {
      ViewImpl& childImpl = GetImpl(childView);

      // Standalone children are handled by ArrangeStandaloneChildren()
      // in ViewImpl::Arrange() after OnArrange returns.
      if(IntegrationView::IsLayoutModeStandalone(childImpl))
      {
        continue;
      }

      float   childScale      = childImpl.GetEffectiveScale();
      Extents margin          = childImpl.GetMargin();
      float   visMarginStart  = static_cast<float>(margin.start) * childScale;
      float   visMarginEnd    = static_cast<float>(margin.end) * childScale;
      float   visMarginTop    = static_cast<float>(margin.top) * childScale;
      float   visMarginBottom = static_cast<float>(margin.bottom) * childScale;
      float   visMarginW      = visMarginStart + visMarginEnd;
      float   visMarginH      = visMarginTop + visMarginBottom;
      // Read measured size directly from the child (set during OnMeasure).
      MeasuredSize childMeasured = childImpl.GetMeasuredSize();
      float        childW        = childMeasured.width;
      float        childH        = childMeasured.height;

      // MATCH_PARENT: fills parent content area minus own margin (in visual units).
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childW = std::max(0.0f, width - visPadLeft - visPadRight - visMarginW);
      }
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childH = std::max(0.0f, height - visPadTop - visPadBottom - visMarginH);
      }
      float childX = visPadLeft + visMarginStart + childImpl.GetRequestedPositionX() * s;
      float childY = visPadTop + visMarginTop + childImpl.GetRequestedPositionY() * s;

      // MATCH_PARENT children: re-measure with the actual final visual size.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childImpl.Measure(childW, childH);
      }

      LayoutRect childBounds(childX, childY, childW, childH);
      childImpl.Arrange(childBounds);
    }
  }

  return {width, height};
}

void ViewImpl::MeasureStandaloneChildren(float visEffW, float visEffH)
{
  for(auto& childView : mImpl->mChildren)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(!IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }
    float   childScale = childImpl.GetEffectiveScale();
    Extents margin     = childImpl.GetMargin();
    float   visMarginW = static_cast<float>(margin.start + margin.end) * childScale;
    float   visMarginH = static_cast<float>(margin.top + margin.bottom) * childScale;
    float   childVisW  = std::max(0.0f, visEffW - visMarginW);
    float   childVisH  = std::max(0.0f, visEffH - visMarginH);
    childImpl.Measure(childVisW, childVisH);
  }
}

void ViewImpl::ArrangeStandaloneChildren(const LayoutRect& bounds)
{
  for(auto& childView : mImpl->mChildren)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(!IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }
    ArrangeStandaloneChild(childImpl, bounds.width, bounds.height);
  }
}

void ViewImpl::ApplyLayoutDirection(float parentWidth)
{
  if(GetEffectiveLayoutDirection() != Dali::LayoutDirection::RIGHT_TO_LEFT)
  {
    return;
  }

  for(auto& childView : mImpl->mChildren)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }

    Actor child  = childImpl.Self();
    float oldX   = child.GetProperty<float>(Actor::Property::POSITION_X);
    float childW = child.GetProperty<float>(Actor::Property::SIZE_WIDTH);
    child.SetProperty(Actor::Property::POSITION_X, parentWidth - oldX - childW);
  }
}

// =============================================================================
// UiScale API
// =============================================================================

void ViewImpl::SetUiScalePolicy(UiScalePolicy policy)
{
  if(mScalePolicy != policy)
  {
    mScalePolicy = policy;
    ResetEffectiveScaleRecursive();
    InvalidateMeasure();
  }
}

UiScalePolicy ViewImpl::GetUiScalePolicy() const
{
  return mScalePolicy;
}

float ViewImpl::GetEffectiveScale() const
{
  if(mEffectiveScale < 0.0f)
  {
    mEffectiveScale = ComputeEffectiveScale();
  }
  return mEffectiveScale;
}

float ViewImpl::ComputeEffectiveScale() const
{
  if(mScalePolicy == UiScalePolicy::DISABLED)
  {
    return 1.0f;
  }
  if(mScalePolicy == UiScalePolicy::ENABLED)
  {
    return UiScaleManager::Get().GetScale();
  }

  // INHERIT: walk up the parent chain (Layout first, consistent with InvalidateMeasure)
  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    return GetImpl(parentLayout).GetEffectiveScale();
  }

  Ui::View parentView = GetParentView();
  if(parentView)
  {
    return GetImpl(parentView).GetEffectiveScale();
  }

  // Root: inherit from UiScaleManager
  return UiScaleManager::Get().GetScale();
}

void ViewImpl::ResetEffectiveScaleRecursive()
{
  mEffectiveScale = -1.0f;

  // Reset measure cache to the NaN "never measured" initial state (not to
  // MEASURE_CACHE_DIRTY = -1.0f). This forces a cache miss in Measure() so
  // every node re-measures with the new scale.
  //
  // NaN is required here (not DIRTY) because the caller follows this with
  // InvalidateMeasure() on the root. DIRTY would trigger InvalidateMeasure's
  // early-exit guard on the root and skip RegisterWithLayoutController().
  // NaN != DIRTY, so the guard does not fire; Dali::Equals(NaN, DIRTY) is
  // false per IEEE 754.
  mImpl->mLastMeasuredConstraint.width  = std::numeric_limits<float>::quiet_NaN();
  mImpl->mLastMeasuredConstraint.height = std::numeric_limits<float>::quiet_NaN();

  for(auto& childView : mImpl->mChildren)
  {
    GetImpl(childView).ResetEffectiveScaleRecursive();
  }
}

void ViewImpl::InvalidateMeasure()
{
  // Early-exit guard: if already dirty, the ancestor chain has already been
  // invalidated and the layout root has been registered, so re-propagation is
  // redundant. "Never measured" state (NaN) still requires propagation because
  // the chain has never been walked from this view.
  //
  // NOTE: Callers that change the view's ancestor-chain structure
  // (e.g. reparenting) must not rely on this view's InvalidateMeasure to reach
  // the new parent chain — the guard may skip propagation. Instead, the
  // reparenting site should invalidate the new parent directly. See
  // OnChildAdd / OnChildRemove where this is done.
  if(Dali::Equals(mImpl->mLastMeasuredConstraint.width, MEASURE_CACHE_DIRTY))
  {
    return;
  }

  mEffectiveScale                       = -1.0f;
  mImpl->mLastMeasuredConstraint.width  = MEASURE_CACHE_DIRTY;
  mImpl->mLastMeasuredConstraint.height = MEASURE_CACHE_DIRTY;
  mImpl->mArrangeDirty                  = true;

  // Layout boundary: a standalone view is excluded from its parent's
  // OnMeasure/OnArrange accumulation, so its measure result cannot change
  // the parent's measured size. Stop propagation here and register this view
  // as its own layout root.
  //
  // When the parent has a LayoutTransition attached, also invalidate the
  // parent so its CaptureBeforeLayout / StartTransitionsAfterLayout pass
  // runs in the same layout batch. Combined with the controller's
  // depth-sorted iteration, this lets the parent capture pre-change
  // bounds before the standalone child's own arrange updates them, so
  // a standalone child's RequestedWidth / RequestedHeight change
  // surfaces as the parent's CHANGE slot. Mirrors the existing
  // OnChildAdd guard for the standalone+transition combination.
  if(IntegrationView::IsLayoutModeStandalone(*this))
  {
    Ui::View parentView = GetParentView();
    if(parentView && GetImpl(parentView).GetLayoutTransition())
    {
      GetImpl(parentView).InvalidateMeasure();
    }
    RegisterWithLayoutController();
    return;
  }

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    GetImpl(parentLayout).InvalidateMeasure();
    return;
  }

  Ui::View parentView = GetParentView();
  if(parentView)
  {
    GetImpl(parentView).InvalidateMeasure();
    return;
  }

  RegisterWithLayoutController();
}

void ViewImpl::InvalidateArrange()
{
  // Early-exit guard mirrors InvalidateMeasure: if already dirty, the
  // ancestor chain has already been invalidated. Only the Dirty state
  // short-circuits; NeverArranged (mArrangeDirty=false, but never arranged)
  // still propagates on its first invalidation.
  if(mImpl->mArrangeDirty)
  {
    return;
  }

  mImpl->mArrangeDirty = true;

  // Layout boundary: standalone child's arrange result does not feed back
  // into the parent's arrangement — stop here and self-register.
  if(IntegrationView::IsLayoutModeStandalone(*this))
  {
    RegisterWithLayoutController();
    return;
  }

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    GetImpl(parentLayout).InvalidateArrange();
    return;
  }

  // Propagate to parent View (no LayoutManager)
  Ui::View parentView = GetParentView();
  if(parentView)
  {
    GetImpl(parentView).InvalidateArrange();
    return;
  }

  // Reached top of View tree → register with LayoutController
  RegisterWithLayoutController();
}

void ViewImpl::RegisterWithLayoutController()
{
  Actor  self   = Self();
  Window window = DevelWindow::Get(self);

  DALI_LOG_DEBUG_INFO("[ViewImpl] RegisterWithLayoutController: hasWindow=%d\n", window ? 1 : 0);

  if(window)
  {
    LayoutController& controller = LayoutController::Get(window);
    controller.RequestLayout(this);

    // Register as a layout root in UiScaleManager so it gets invalidated when
    // the system scale changes. Duplicate registration is silently ignored.
    UiScaleManager::Get().RegisterLayoutRoot(Ui::View::DownCast(self));
  }
}

MeasuredSize ViewImpl::GetMeasuredSize() const
{
  return mImpl->mMeasuredSize;
}

MeasuredSize ViewImpl::ApplyConstraints(const MeasuredSize& size) const
{
  // size is in visual (scale-applied) units; scale min/max (natural) accordingly.
  float        s           = GetEffectiveScale();
  MeasuredSize constrained = size;
  constrained.width        = std::max(constrained.width, mImpl->GetMinimumWidth() * s);
  constrained.height       = std::max(constrained.height, mImpl->GetMinimumHeight() * s);
  constrained.width        = std::min(constrained.width, mImpl->GetMaximumWidth() * s);
  constrained.height       = std::min(constrained.height, mImpl->GetMaximumHeight() * s);
  return constrained;
}

// =============================================================================
// Requested size API
// =============================================================================

void ViewImpl::SetRequestedWidth(float width)
{
  Self().SetProperty(Ui::View::Property::REQUESTED_WIDTH, width);
}

float ViewImpl::GetRequestedWidth() const
{
  return mImpl->mRequestedWidth;
}

void ViewImpl::SetRequestedHeight(float height)
{
  Self().SetProperty(Ui::View::Property::REQUESTED_HEIGHT, height);
}

float ViewImpl::GetRequestedHeight() const
{
  return mImpl->mRequestedHeight;
}

void ViewImpl::SetMinimumWidth(float width)
{
  Self().SetProperty(Ui::View::Property::MINIMUM_WIDTH, width);
}

float ViewImpl::GetMinimumWidth() const
{
  return mImpl->GetMinimumWidth();
}

void ViewImpl::SetMinimumHeight(float height)
{
  Self().SetProperty(Ui::View::Property::MINIMUM_HEIGHT, height);
}

float ViewImpl::GetMinimumHeight() const
{
  return mImpl->GetMinimumHeight();
}

void ViewImpl::SetMaximumWidth(float width)
{
  Self().SetProperty(Ui::View::Property::MAXIMUM_WIDTH, width);
}

float ViewImpl::GetMaximumWidth() const
{
  return mImpl->GetMaximumWidth();
}

void ViewImpl::SetMaximumHeight(float height)
{
  Self().SetProperty(Ui::View::Property::MAXIMUM_HEIGHT, height);
}

float ViewImpl::GetMaximumHeight() const
{
  return mImpl->GetMaximumHeight();
}

// =============================================================================
// Layout Properties API
// =============================================================================

void ViewImpl::SetMargin(const Extents& margin)
{
  Self().SetProperty(Ui::View::Property::MARGIN, margin);
}

Extents ViewImpl::GetMargin() const
{
  return mImpl->mMargin;
}

void ViewImpl::SetPadding(const Extents& padding)
{
  Self().SetProperty(Ui::View::Property::PADDING, padding);
}

Extents ViewImpl::GetPadding() const
{
  return mImpl->mPadding;
}

void ViewImpl::SetLayoutMode(Ui::LayoutMode mode)
{
  Self().SetProperty(Ui::View::Property::LAYOUT_MODE, static_cast<int>(mode));
}

Ui::LayoutMode ViewImpl::GetLayoutMode() const
{
  return mImpl->mLayoutMode;
}

// =============================================================================
// Parent Layout API
// =============================================================================

Ui::Layout ViewImpl::GetParentLayout() const
{
  Actor parent = Self().GetParent();
  if(parent)
  {
    return Ui::Layout::DownCast(parent);
  }
  return Ui::Layout();
}

Ui::View ViewImpl::GetParentView() const
{
  Actor parent = Self().GetParent();
  if(parent)
  {
    return Ui::View::DownCast(parent);
  }
  return Ui::View();
}

void ViewImpl::SetMeasureCallback(MeasureCallback callback)
{
  EnsureLayoutCallbacksObject(this)->SetMeasureCallback(std::move(callback));
  InvalidateMeasure();
}

void ViewImpl::SetArrangeCallback(ArrangeCallback callback)
{
  EnsureLayoutCallbacksObject(this)->SetArrangeCallback(std::move(callback));
  InvalidateArrange();
}

MeasureCallback* ViewImpl::GetMeasureCallback()
{
  auto* object = GetLayoutCallbacksObject(this);
  return object ? object->GetMeasureCallback() : nullptr;
}

ArrangeCallback* ViewImpl::GetArrangeCallback()
{
  auto* object = GetLayoutCallbacksObject(this);
  return object ? object->GetArrangeCallback() : nullptr;
}

void ViewImpl::AttachLayoutManager(Dali::UniquePtr<LayoutManager> manager)
{
  DALI_ASSERT_ALWAYS(manager && "AttachLayoutManager requires a non-null LayoutManager.");
  DALI_ASSERT_ALWAYS(!HasLayoutManager() && "LayoutManager already set. Cannot replace an existing LayoutManager.");

  IntrusivePtr<TraitObject> object(new Internal::LayoutManagerObject(std::move(manager)));
  IntegrationView::SetTrait(*this, Integration::ReservedTraitId::LAYOUT_MANAGER, object);
  InvalidateMeasure();
}

LayoutManager* ViewImpl::GetLayoutManager() const
{
  IntrusivePtr<TraitObject> object        = IntegrationView::GetTrait(*this, Integration::ReservedTraitId::LAYOUT_MANAGER);
  auto*                     managerObject = object ? static_cast<Internal::LayoutManagerObject*>(object.Get()) : nullptr;
  return managerObject ? managerObject->GetLayoutManager() : nullptr;
}

bool ViewImpl::HasLayoutManager() const
{
  return GetLayoutManager() != nullptr;
}

bool ViewImpl::HasLayoutCallback() const
{
  auto* object = GetLayoutCallbacksObject(const_cast<ViewImpl*>(this));
  return object && (object->GetMeasureCallback() || object->GetArrangeCallback());
}

void ViewImpl::SetLayoutTransition(LayoutTransition transition)
{
  mImpl->mLayoutTransition = transition;
  // Detach: drop any pending ENTER / REORDER / REMOVE markers. Records
  // are only produced while a transition is attached (see OnChildAdd /
  // Insert / OnChildOrderChanged / Remove); a previously attached
  // transition could have left entries that we want to discard now so
  // a later re-attach does not surface them as a stale cause on the
  // next pass. In particular mPendingChildRemovalForLayoutTransition is
  // consumed only when StartTransitionsForView runs (which requires an
  // attached transition), so without this clear the marker would
  // survive a detach -> layout pass -> reattach cycle and tag the next
  // unrelated CHANGE as SIBLING_REMOVED.
  if(!transition)
  {
    mImpl->mPendingEnterChildren.clear();
    mImpl->mPendingReorderedChildren.clear();
    mImpl->mPendingChildRemovalForLayoutTransition = false;
    // Symmetric with the direct markers above: drop any inherited-ENTER
    // candidates this view owns in the dispatcher, so a detach -> reattach
    // cycle does not surface a stale ENTER for a grand-child added under the
    // old transition. The records live in the per-window dispatcher; an
    // off-window view's records were already dropped by its scene-disconnect
    // cleanup (OnViewDestroyed).
    Window window = DevelWindow::Get(Self());
    if(window)
    {
      LayoutController::Get(window).ClearPendingInheritedEnters(this);
    }
    return;
  }

  // Attach: seed any pre-existing children as pending ENTER candidates,
  // but only when this view has not yet completed its initial layout
  // pass. OnChildAdd only inserts into mPendingEnterChildren when a
  // transition is already attached, so the order
  //   parent.Add(child); parent.SetLayoutTransition(transition);
  // would otherwise leave the child out of the pending set entirely.
  // Without this seed, the dispatcher's first pass would neither
  // dispatch ENTER nor settle a declarative ENTER spec onto the child,
  // leaving e.g. a child pre-set to opacity = 0 permanently invisible.
  //
  // Restricted to !mInitialLayoutDone so re-attaching a transition on a
  // view that has already been on screen does not retroactively classify
  // its already-visible children as initial-mount candidates.
  if(!mImpl->mInitialLayoutDone)
  {
    for(auto& childView : mImpl->mChildren)
    {
      mImpl->mPendingEnterChildren.insert(&GetImpl(childView));
    }
  }
}

LayoutTransition ViewImpl::GetLayoutTransition() const
{
  return mImpl->mLayoutTransition;
}

LayoutRect ViewImpl::GetArrangedBounds() const
{
  return mImpl->mArrangedBounds;
}

std::unordered_set<ViewImpl*> ViewImpl::TakePendingEnterChildren()
{
  std::unordered_set<ViewImpl*> result;
  std::swap(result, mImpl->mPendingEnterChildren);
  return result;
}

std::unordered_set<ViewImpl*> ViewImpl::TakePendingReorderedChildren()
{
  std::unordered_set<ViewImpl*> result;
  std::swap(result, mImpl->mPendingReorderedChildren);
  return result;
}

bool ViewImpl::TakePendingChildRemovalForLayoutTransition()
{
  const bool result                              = mImpl->mPendingChildRemovalForLayoutTransition;
  mImpl->mPendingChildRemovalForLayoutTransition = false;
  return result;
}

bool ViewImpl::IsInitialLayoutDone() const
{
  return mImpl->mInitialLayoutDone;
}

// =============================================================================
// Child Management API
// =============================================================================

void ViewImpl::Insert(uint32_t index, Ui::View child)
{
  if(!child)
  {
    return;
  }

  // Adding to the Actor tree triggers OnChildAdd on this ViewImpl, which is
  // the single source of truth for registering the child in mChildren and
  // for invalidating the new parent chain. Insert only takes additional
  // responsibility for positioning the child at the requested index.
  Self().Add(child);

  if(index >= mImpl->mChildren.Count())
  {
    // OnChildAdd push_back'd the child at the end; target index is end.
    return;
  }

  // Fast path: when this was a fresh add, OnChildAdd push_back'd the child,
  // so it is at the tail of mChildren. Avoid an O(N) scan in that case.
  IntegrationView::ChildContainer::Iterator it;
  if(mImpl->mChildren.Count() > 0 && *(mImpl->mChildren.End() - 1) == child)
  {
    it = mImpl->mChildren.End() - 1;
  }
  else
  {
    it = std::find(mImpl->mChildren.Begin(), mImpl->mChildren.End(), child);
    if(it == mImpl->mChildren.End())
    {
      // OnChildAdd did not register this child (e.g. non-View actor). Nothing
      // to reorder.
      return;
    }
  }

  const size_t currentIdx = static_cast<size_t>(std::distance(mImpl->mChildren.Begin(), it));
  if(currentIdx == index)
  {
    return;
  }

  Ui::View moved = std::move(*it);
  mImpl->mChildren.Erase(it);
  mImpl->mChildren.Insert(mImpl->mChildren.Begin() + index, std::move(moved));

  // Tag every logical child so the layout transition dispatcher reports
  // CHANGE cause as LayoutChangeCause::REORDERED for both the moved child and the
  // siblings whose indices shifted as a result. dali-core's
  // OnChildOrderChanged fires only on actor-tree sibling order changes;
  // Insert() touches the logical (mChildren) order alone, so this is the
  // only place that records the reorder for the CHANGE classifier.
  // Matches OnChildOrderChanged's full-list tagging so a logical reorder
  // and an actor-tree reorder produce the same cause classification.
  // Skip the record when no transition is attached — the dispatcher
  // would never consume it, and stale raw pointers could outlive the
  // child without any global cleanup.
  if(mImpl->mLayoutTransition)
  {
    for(auto& childView : mImpl->mChildren)
    {
      mImpl->mPendingReorderedChildren.insert(&GetImpl(childView));
    }
  }

  // mChildren order affects layout output (e.g. LinearLayout visual order,
  // GridLayout cell assignment). When the child was already under this view
  // (Self().Add is a no-op in that case), OnChildAdd does not fire, so this
  // is the only invalidation point for the reorder. When the child was a
  // fresh add, self is already dirty from OnChildAdd and the guard makes
  // this a no-op.
  InvalidateMeasure();
}

void ViewImpl::RemoveAllChildren()
{
  // If an EXIT effect governs these children — either this view's own EXIT
  // slot (direct EXIT) or, when this view has no EXIT slot, the closest
  // ancestor SUBTREE owner with an EXIT effect (inherited EXIT) — defer each
  // child to the dispatcher so the EXIT animation plays. Without this,
  // RemoveAllChildren would silently bypass EXIT and the bulk remove would
  // feel jarring.
  //
  // All direct children of this view share the same EXIT owner: it depends
  // only on this view and the ancestor chain above it, not on the individual
  // child, so a single resolution covers the whole bulk remove.
  Ui::LayoutTransition transition = mImpl->mLayoutTransition;
  Dali::Window         window     = DevelWindow::Get(Self());
  ViewImpl*            exitOwner  = nullptr;
  if(window)
  {
    const bool selfHasExitFx = transition && Internal::GetImpl(transition).HasExitFx();
    exitOwner                = selfHasExitFx
                                 ? this
                                 : Internal::FindGoverningSubtreeOwner(this, Internal::ReflowSlot::EXIT);
  }

  if(exitOwner)
  {
    // Snapshot first because ScheduleLayoutExit does not touch mChildren,
    // but we want subsequent layout passes to see an empty logical list so
    // remaining siblings reflow immediately. Clear pending-enter so a
    // child that was add+remove'd in the same frame does not leak into
    // the dispatcher's enter set after the bulk remove.
    std::vector<Ui::View> snapshot;
    snapshot.reserve(mImpl->mChildren.Count());
    for(auto& childView : mImpl->mChildren)
    {
      GetImpl(childView).InvalidateMeasure();
      snapshot.push_back(childView);
    }
    mImpl->mChildren.Clear();
    mImpl->mPendingEnterChildren.clear();
    // Same rationale as the per-child OnChildRemove erase: a stale raw
    // ViewImpl* in the reorder set could outlive its child and cause a
    // future address-reused child to be misclassified as REORDERED.
    mImpl->mPendingReorderedChildren.clear();
    // Bulk remove via deferred path — the dispatcher fires EXIT on every
    // child so no siblings remain, but the marker keeps semantics consistent
    // with the per-child path. Set it only when THIS view owns a transition to
    // consume it: for an inherited (ancestor-owned) EXIT this view may have no
    // transition, and the marker — consumed only by a transition-bearing view's
    // layout pass — would never be cleared and would mis-tag a future CHANGE.
    if(transition && !snapshot.empty())
    {
      mImpl->mPendingChildRemovalForLayoutTransition = true;
    }
    InvalidateMeasure();

    auto& controller = LayoutController::Get(window);
    for(auto& child : snapshot)
    {
      controller.ScheduleLayoutExit(this, child, exitOwner);
    }
    return;
  }

  // No EXIT slot configured — original immediate-remove path. The guard
  // suppresses OnChildRemove (which would re-enter mChildren) so the loop
  // can iterate safely while removing.
  const bool hadChildren = mImpl->mChildren.Count() > 0;
  {
    ScopedSkipChildrenUpdate guard(mImpl->mSkipChildrenUpdate);
    for(auto& childView : mImpl->mChildren)
    {
      GetImpl(childView).InvalidateMeasure();
      Self().Remove(childView);
    }
  }

  mImpl->mChildren.Clear();
  // The OnChildRemove guard above suppresses the per-child pending-set
  // erase that the deferred-remove path performs explicitly. Drop both
  // pending sets here so the immediate-remove path leaves the same clean
  // state that the deferred path produces.
  mImpl->mPendingEnterChildren.clear();
  mImpl->mPendingReorderedChildren.clear();
  // Mark sibling removal only when a transition is attached so the next
  // CHANGE pass on remaining children (e.g. animator-only EXIT path) is
  // tagged correctly. Skip when no transition is attached.
  if(transition && hadChildren)
  {
    mImpl->mPendingChildRemovalForLayoutTransition = true;
  }
  InvalidateMeasure();
}

void ViewImpl::Remove(Ui::View child, Ui::RemovePolicy policy)
{
  if(!child)
  {
    return;
  }

  const bool animateExit = (policy == Ui::RemovePolicy::ANIMATE_EXIT);

  // ANIMATE_EXIT only: if a LayoutTransition with an EXIT slot (spec OR
  // animator) is attached, hand the child off to the layout transition
  // dispatcher so the EXIT animation can play. IMMEDIATE (and the
  // no-EXIT-slot case) falls through to the immediate unparent below.
  Ui::LayoutTransition transition = mImpl->mLayoutTransition;
  bool                 deferred   = false;
  if(animateExit && transition)
  {
    auto&      impl      = Internal::GetImpl(transition);
    const bool hasExitFx = static_cast<bool>(impl.GetExitVisualSpec()) || impl.HasExitAnimator() || impl.HasActiveExitBoundsEffect();
    if(hasExitFx)
    {
      Actor  self   = Self();
      Window window = DevelWindow::Get(self);
      if(window)
      {
        // Remove the child from this view's layout-tracking list and
        // invalidate so siblings flow into the freed slot during the next
        // layout pass. The child's Actor stays under this Actor so the
        // dispatcher can animate it before unparenting.
        ViewImpl& childImpl = GetImpl(child);
        auto      it        = std::find(mImpl->mChildren.begin(), mImpl->mChildren.end(), child);
        if(it != mImpl->mChildren.end())
        {
          mImpl->mChildren.Erase(it);
          mImpl->mPendingEnterChildren.erase(&childImpl);
          // Same rationale as the immediate-remove path's OnChildRemove:
          // a stale raw ViewImpl* in the reorder set could outlive its
          // child after deferred-remove EXIT and cause a future heap-
          // reused address to be misclassified as REORDERED. Erase
          // per-child here (not full clear) so the cause of any
          // siblings still pending reorder is preserved.
          mImpl->mPendingReorderedChildren.erase(&childImpl);
          // Mark sibling removal so the dispatcher tags this pass's CHANGE
          // dispatches on the remaining siblings as SIBLING_REMOVED. Set
          // only when a transition is attached to avoid leaving stale
          // marker state on views without transitions.
          mImpl->mPendingChildRemovalForLayoutTransition = true;
          InvalidateMeasure();

          // Only schedule the EXIT transition when @p child was actually a
          // tracked child. Calling Remove on a non-child must not fire
          // any DALi layout-transition lifecycle / animation; without this
          // guard a misuse would leave a ghost animation that fires
          // OnStart / OnFinished and races with
          // the actor's real parent.
          LayoutController::Get(window).ScheduleLayoutExit(this, child);
          deferred = true;
        }
      }
    }
  }

  if(!deferred)
  {
    // Guard against re-removing a child that is currently an EXIT ghost
    // under this view. Ghost detection: actor parent is still Self() (the
    // deferred-remove keeps the actor attached) AND the child has already
    // been removed from the logical children list (mChildren). Without
    // this guard, the second Remove bypasses the dispatcher
    // duplicate-EXIT guard and synchronously unparents the ghost, which
    // triggers OnSceneDisconnection → CancelPendingExit/CancelActiveAnimator
    // and silently cancels the in-flight EXIT (no OnFinished, no fade).
    // The same applies when the parent's LayoutTransition has been replaced
    // or cleared between the first and second Remove — the second
    // call cannot enter the deferred branch but the ghost is still in
    // flight under its original transition.
    if(child.GetParent() == Self() &&
       std::find(mImpl->mChildren.begin(), mImpl->mChildren.end(), child) == mImpl->mChildren.end())
    {
      return;
    }

    Actor      selfActor      = Self();
    Window     window         = DevelWindow::Get(selfActor);
    auto       it             = std::find(mImpl->mChildren.begin(), mImpl->mChildren.end(), child);
    const bool isCurrentChild = (it != mImpl->mChildren.end());

    // Inherited (SUBTREE-scope) EXIT: this view does not handle EXIT through
    // its own transition (otherwise the deferred branch above would have run).
    // Walk up to the closest ancestor SUBTREE owner that carries an EXIT
    // effect; if found, defer the child to that owner. The actor stays under
    // this view — the ghost's direct/visual parent — while the owner's
    // transition drives the EXIT effect (INV-GHOST-UNDER-DIRECT-PARENT). The
    // closest-owner / standalone-boundary rules are enforced inside the
    // resolver, so a child claimed by a closer (non-SUBTREE or non-EXIT)
    // transition is not stolen by an ancestor.
    // ANIMATE_EXIT only: inherited (SUBTREE-scope) EXIT defer. IMMEDIATE skips
    // this and unparents synchronously below.
    if(animateExit && window && isCurrentChild)
    {
      ViewImpl* owner = Internal::FindGoverningSubtreeOwner(this, Internal::ReflowSlot::EXIT);
      if(owner)
      {
        ViewImpl& childImpl = GetImpl(child);
        mImpl->mChildren.Erase(it);
        mImpl->mPendingEnterChildren.erase(&childImpl);
        mImpl->mPendingReorderedChildren.erase(&childImpl);
        // Remaining siblings under THIS direct parent reflow into the freed
        // slot; tag their CHANGE as SIBLING_REMOVED on the next pass — but only
        // when THIS view owns a transition to consume the marker. For an
        // inherited EXIT this view may have no transition, and the marker —
        // consumed only by a transition-bearing view's layout pass — would
        // never be cleared and would mis-tag a future CHANGE if it later gains
        // one.
        if(transition)
        {
          mImpl->mPendingChildRemovalForLayoutTransition = true;
        }
        InvalidateMeasure();
        LayoutController::Get(window).ScheduleLayoutExit(this, child, owner);
        return;
      }
    }

    // Mark sibling removal for the next CHANGE pass when a transition is
    // attached (without an EXIT slot) AND we have a window. The
    // remaining children may reflow and should be tagged with
    // SIBLING_REMOVED. Skip the marker when no transition is attached,
    // or when no window is available — without a window the marker
    // cannot be consumed by the dispatcher in this pass (no layout
    // pass runs), so it would leak across a later add-to-window event
    // and mis-tag the first layout pass's CHANGE as SIBLING_REMOVED.
    if(transition && window && isCurrentChild)
    {
      mImpl->mPendingChildRemovalForLayoutTransition = true;
    }
    selfActor.Remove(child);
  }
}

uint32_t ViewImpl::GetChildCount() const
{
  return static_cast<uint32_t>(mImpl->mChildren.Count());
}

Ui::View ViewImpl::GetChildAt(uint32_t index) const
{
  if(index < mImpl->mChildren.Count())
  {
    return mImpl->mChildren[index];
  }
  return Ui::View();
}

int32_t ViewImpl::IndexOfChild(Ui::View view) const
{
  if(!view)
  {
    return -1;
  }
  for(size_t i = 0; i < mImpl->mChildren.Count(); ++i)
  {
    if(mImpl->mChildren[i] == view)
    {
      return static_cast<int32_t>(i);
    }
  }
  return -1;
}

void ViewImpl::Raise(Ui::LayoutOrderPolicy policy)
{
  Actor self = Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
      self.Raise();
      return;
    }
  }
  self.Raise();
}

void ViewImpl::Lower(Ui::LayoutOrderPolicy policy)
{
  Actor self = Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
      self.Lower();
      return;
    }
  }
  self.Lower();
}

void ViewImpl::RaiseToTop(Ui::LayoutOrderPolicy policy)
{
  Actor self = Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
      self.RaiseToTop();
      return;
    }
  }
  self.RaiseToTop();
}

void ViewImpl::LowerToBottom(Ui::LayoutOrderPolicy policy)
{
  Actor self = Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
      self.LowerToBottom();
      return;
    }
  }
  self.LowerToBottom();
}

void ViewImpl::RaiseAbove(Ui::View target, Ui::LayoutOrderPolicy policy)
{
  if(!target)
  {
    return;
  }
  Actor self = Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
      self.RaiseAbove(target);
      return;
    }
  }
  self.RaiseAbove(target);
}

void ViewImpl::LowerBelow(Ui::View target, Ui::LayoutOrderPolicy policy)
{
  if(!target)
  {
    return;
  }
  Actor self = Self();
  if(policy == Ui::LayoutOrderPolicy::PRESERVE)
  {
    Ui::View parent = Ui::View::DownCast(self.GetParent());
    if(parent)
    {
      ScopedSkipChildrenUpdate guard(GetImpl(parent).mImpl->mSkipChildrenUpdate);
      self.LowerBelow(target);
      return;
    }
  }
  self.LowerBelow(target);
}

namespace
{

TraitId ToTraitId(LayoutParamsType type)
{
  switch(type)
  {
    case LayoutParamsType::ABSOLUTE:
      return Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
    case LayoutParamsType::STACK:
      return Integration::ReservedTraitId::STACK_LAYOUT_PARAMS;
    case LayoutParamsType::GRID:
      return Integration::ReservedTraitId::GRID_LAYOUT_PARAMS;
    case LayoutParamsType::FLEX:
      return Integration::ReservedTraitId::FLEX_LAYOUT_PARAMS;
  }
  DALI_ASSERT_ALWAYS(false && "Unknown LayoutParamsType");
  return Integration::ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
}

} // unnamed namespace

BaseHandle ViewImpl::GetLayoutParams(LayoutParamsType type) const
{
  IntrusivePtr<TraitObject> object     = mImpl->GetTrait(ToTraitId(type));
  auto*                     baseObject = dynamic_cast<BaseObject*>(object.Get());
  return baseObject ? BaseHandle(baseObject) : BaseHandle();
}

void ViewImpl::SetLayoutParams(Ui::LayoutParams params)
{
  auto& paramsImpl = static_cast<Internal::LayoutParamsImpl&>(params.GetBaseObject());
  mImpl->SetTrait(paramsImpl.GetTraitId(), ToTraitObject(params));
  InvalidateMeasure();
}

// =============================================================================
// VisualBase API
// =============================================================================

bool ViewImpl::AddVisual(Dali::Ui::VisualBase visualBase, Dali::Ui::Visual::ContainerRangeType containerRangeType)
{
  return mImpl->AddVisualObject(visualBase, static_cast<Dali::Ui::DevelVisual::InternalContainerRangeType>(containerRangeType));
}

void ViewImpl::RemoveVisual(Dali::Ui::VisualBase visualBase)
{
  mImpl->RemoveVisualObject(visualBase);
}

uint32_t ViewImpl::GetVisualCount(Dali::Ui::Visual::ContainerRangeType containerRangeType) const
{
  return mImpl->GetVisualObjectCount(static_cast<Dali::Ui::DevelVisual::InternalContainerRangeType>(containerRangeType));
}

Dali::Ui::VisualBase ViewImpl::GetVisualAt(Dali::Ui::Visual::ContainerRangeType containerRangeType, uint32_t siblingOrder) const
{
  return mImpl->GetVisualObjectAt(static_cast<Dali::Ui::DevelVisual::InternalContainerRangeType>(containerRangeType), siblingOrder);
}

// =============================================================================
// From control-impl.cpp
// =============================================================================

ViewImpl::ViewImpl(ViewBehaviour behaviourFlags)
: CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->mFlags = static_cast<Ui::ViewImpl::ViewBehaviour>(behaviourFlags);
}

void ViewImpl::Initialize()
{
  if(!(mImpl->mFlags & Ui::ViewImpl::ViewBehaviour::DISABLE_VISUALS))
  {
    mImpl->InitializeVisualData();
  }

  RegisterViewAccessibleGetter();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  auto uiConfigManager = Integration::UiConfigManager::Get();
  if(uiConfigManager.IsInitialized())
  {
    uiConfigManager.GetConfig().GetViewInitializer()(View::DownCast(Self()));
  }
  else
  {
    UiConfig::DefaultViewInitializer(View::DownCast(Self()));
  }
}

void ViewImpl::SetBackgroundColorInternal(const Vector4& color)
{
  mImpl->mBackgroundColor = color;

  Property::Map map;
  map.Insert(Ui::VisualBasePropertyIndex::TYPE, Ui::InternalVisualType::COLOR);
  map.Insert(Ui::VisualBasePropertyIndex::MIX_COLOR, color);

  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr && visualImplPtr->GetType() == Ui::InternalVisualType::COLOR)
  {
    // Update background color only
    visualImplPtr->DoAction(DevelVisual::Action::UPDATE_PROPERTY, map);
    return;
  }

  SetBackground(map);
}

void ViewImpl::SetBackground(const Property::Map& map)
{
  Self().SetProperty(Ui::View::Property::BACKGROUND, map);
}

void ViewImpl::ClearBackground()
{
  mImpl->UnregisterVisual(Ui::View::Property::BACKGROUND);
  mImpl->mBackgroundColor = Color::TRANSPARENT;

  UiColorManager::Get().ClearBinding(Self(), "BackgroundColor");

  // Trigger a size negotiation request that may be needed when unregistering a visual.
  RelayoutRequest();
}

void ViewImpl::SetRenderEffect(Ui::RenderEffect effect)
{
  ClearRenderEffect();

  if(effect)
  {
    Internal::RenderEffectImpl* object = dynamic_cast<Internal::RenderEffectImpl*>(effect.GetObjectPtr());
    DALI_ASSERT_ALWAYS(object && "Given render effect is not valid.");

    Dali::Ui::View ownerView(GetOwner());
    object->SetOwnerView(ownerView);

    mImpl->mRenderEffect = object;
  }
  else
  {
    mImpl->mRenderEffect.Reset();
  }
}

RenderEffect ViewImpl::GetRenderEffect() const
{
  return RenderEffect(mImpl->mRenderEffect.Get());
}

void ViewImpl::ClearRenderEffect()
{
  if(mImpl->mRenderEffect)
  {
    Dali::Ui::Internal::RenderEffectImplPtr effectImpl = std::move(mImpl->mRenderEffect);

    // Reset handle first to avoid circular reference
    mImpl->mRenderEffect.Reset();

    effectImpl->ClearOwnerView();
  }
}

void ViewImpl::SetResourceReady()
{
  Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(*this);
  viewDataImpl.ResourceReady();
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
  mImpl->mFocusNavigationCallback = std::move(callback);
}

View ViewImpl::RequestFocusNavigation(View currentFocusedView, FocusDirection direction)
{
  if(mImpl->mFocusNavigationCallback)
  {
    return mImpl->mFocusNavigationCallback.Invoke(currentFocusedView, direction);
  }
  return OnFocusNavigationRequested(currentFocusedView, direction);
}

void ViewImpl::SetDescendantFocusBlocked(bool blocked)
{
  Self().SetProperty(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN, !blocked);
}

bool ViewImpl::IsDescendantFocusBlocked() const
{
  return !Self().GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN);
}

bool ViewImpl::HasAncestorBlockingFocus() const
{
  Dali::Actor parent = Self().GetParent();
  while(parent)
  {
    if(!parent.GetProperty<bool>(DevelActor::Property::KEYBOARD_FOCUSABLE_CHILDREN))
    {
      return true;
    }
    parent = parent.GetParent();
  }
  return false;
}

View ViewImpl::RequestFocus()
{
  if(HasAncestorBlockingFocus())
  {
    return View();
  }
  return OnFocusRequested();
}

View ViewImpl::DefaultOnFocusRequested()
{
  Ui::View self = Ui::View::DownCast(Self());
  if(self.IsFocusable() && self.IsEnabled() && self.IsVisible())
  {
    return self;
  }
  return View();
}

View ViewImpl::RequestChildFirstFocus()
{
  Ui::View self = Ui::View::DownCast(Self());

  if(self.IsDescendantFocusBlocked())
  {
    return DefaultOnFocusRequested();
  }

  const uint32_t childCount = self.GetChildCount();
  for(uint32_t i = 0; i < childCount; ++i)
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
  return DefaultOnFocusRequested();
}

View ViewImpl::OnFocusRequested()
{
  if(IntegrationView::HasLayoutCapability(*this))
  {
    return RequestChildFirstFocus();
  }
  return DefaultOnFocusRequested();
}

MeasuredSize ViewImpl::DispatchMeasureWithLayoutManager(LayoutManager* manager, float widthConstraint, float heightConstraint)
{
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

  MeasuredSize visContent = manager->Measure(this, contentVisW, contentVisH);

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

MeasuredSize ViewImpl::DispatchArrangeWithLayoutManager(LayoutManager* manager, const LayoutRect& visualBounds)
{
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
  visContentBounds.width  = std::max(0.0f, visualBounds.width - static_cast<float>(padding.start + padding.end) * s);
  visContentBounds.height = std::max(0.0f, visualBounds.height - static_cast<float>(padding.top + padding.bottom) * s);

  manager->Arrange(this, visContentBounds);

  return {visualBounds.width, visualBounds.height};
}

MeasuredSize ViewImpl::DispatchArrangeWithCallback(ArrangeCallback* callback, const LayoutRect& visualBounds)
{
  Actor self = Self();
  self.SetProperty(Actor::Property::POSITION_X, visualBounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, visualBounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH, visualBounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, visualBounds.height);
  Ui::View view = Ui::View::DownCast(self);
  return callback->Invoke(view, visualBounds);
}

void ViewImpl::SetAsFocusGroup(bool isFocusGroup)
{
  Self().SetProperty(Ui::View::Property::FOCUS_GROUP, isFocusGroup);
}

bool ViewImpl::IsFocusGroup() const
{
  return mImpl->mIsFocusGroup;
}

bool ViewImpl::OnAccessibilityActivated()
{
  return Ui::FocusManager::Get().SetCurrentFocusView(Ui::View::DownCast(Self()));
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
  return new ViewAccessible(Self());
}

View ViewImpl::OnFocusNavigationRequested(View currentFocusedView, FocusDirection direction)
{
  return View();
}

void ViewImpl::OnFocusChangeCommitted(Ui::View committedFocusableView)
{
}

void ViewImpl::NotifyFocusChangeCommitted(Ui::View committedFocusableView)
{
  OnFocusChangeCommitted(committedFocusableView);
}

Ui::View::KeyEventSignalType& ViewImpl::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Ui::View::FocusChangedSignalType& ViewImpl::FocusChangedSignal()
{
  return mImpl->mFocusChangedSignal;
}

bool ViewImpl::NotifyKeyEvent(const KeyEvent& event)
{
  // Guard against destruction during signal emission
  Dali::Ui::View handle(GetOwner());

  bool consumed = false;

  consumed = mImpl->FilterKeyEvent(event);

  // signals are allocated dynamically when someone connects
  if(!consumed && !mImpl->mKeyEventSignal.Empty())
  {
    consumed = mImpl->mKeyEventSignal.Emit(handle, event);
  }

  if(!consumed)
  {
    // Notification for derived classes
    consumed = OnKeyEvent(event);
  }

  return consumed;
}

Dali::Texture ViewImpl::GetOffScreenRenderingOutput() const
{
  if(mImpl->mOffScreenRenderingType != Ui::View::OffScreenRenderingType::REFRESH_ONCE)
  {
    DALI_LOG_ERROR(
      "Precondition unsatisfied: Set property OFFSCREEN_RENDERING to OffScreenRenderingType::REFRESH_ONCE\n");
    return Dali::Texture();
  }
  return mImpl->mOffScreenRenderingImpl->GetTexture();
}

void ViewImpl::EmitFocusChangedSignal(bool focusGained)
{
  Dali::Ui::View handle(GetOwner());

  if(Accessibility::IsUp())
  {
    auto accessible = GetViewDataImpl().GetAccessibleObject();
    if(DALI_LIKELY(accessible))
    {
      accessible->EmitFocused(focusGained);
      auto parent = dynamic_cast<Dali::Accessibility::ActorAccessible*>(accessible->GetParent());
      if(parent && !accessible->GetStates()[Dali::Accessibility::State::MANAGES_DESCENDANTS])
      {
        parent->EmitActiveDescendantChanged(accessible.Get());
      }
    }
  }

  // signals are allocated dynamically when someone connects
  if(!mImpl->mFocusChangedSignal.Empty())
  {
    mImpl->mFocusChangedSignal.Emit(handle, focusGained);
  }
}

void ViewImpl::OnSceneDisconnection()
{
  // When a view leaves the scene (including being removed from a parent or
  // when its window is destroyed), remove any pending LayoutController
  // registration. Otherwise the controller would carry a stale entry whose
  // parent-chain is no longer in this window, and the next layout pass
  // would process a view that is effectively orphaned. The view's destructor
  // already calls UnregisterFromAll as a last resort, but doing it here
  // avoids stale pending work between disconnect and destruction.
  LayoutController::UnregisterFromAll(this);

  if(auto* interactiveTrait = mImpl->GetInteractiveTrait())
  {
    interactiveTrait->OnSceneDisconnection(View::DownCast(Self()));
  }

  mImpl->OnSceneDisconnection();

  // Remove from UiScaleManager if this view was registered as a layout root.
  // Two cases match the registration paths in RegisterWithLayoutController:
  //   (a) tree root: no parent view and no parent layout
  //   (b) standalone: boundary views self-register regardless of their parent
  if((!GetParentView() && !GetParentLayout()) || IntegrationView::IsLayoutModeStandalone(*this))
  {
    UiScaleManager::Get().UnregisterLayoutRoot(Ui::View::DownCast(Self()));
  }
}

void ViewImpl::OnChildAdd(Actor& child)
{
  if(mImpl->mSkipChildrenUpdate)
  {
    return;
  }

  Ui::View view = Ui::View::DownCast(child);
  if(view)
  {
    mImpl->mChildren.PushBack(view);

    ViewImpl& childImpl = GetImpl(view);

    // If this child still has an in-flight transition under an old parent
    // (reparent during EXIT), cancel it before we mark the child for
    // ENTER under this view. Otherwise the orphan callback / animation
    // would keep driving the actor against the old parent's coord system.
    {
      Actor  self   = Self();
      Window window = DevelWindow::Get(self);
      if(window)
      {
        auto& controller = LayoutController::Get(window);
        controller.NotifyChildReparented(&childImpl);

        // Inherited (SUBTREE-scope) ENTER: when THIS view has no transition of
        // its own, a child added here is not recorded for direct ENTER (the
        // gate below requires this view's transition). Notify the dispatcher so
        // it can walk up to the closest ancestor SUBTREE owner with an ENTER
        // effect and register an inherited-ENTER candidate. When this view HAS
        // a transition it is the closest owner and the direct path below claims
        // the child, so the inherited walk is skipped here.
        if(!mImpl->mLayoutTransition)
        {
          controller.NotifyChildAdded(this, view);
        }
      }
    }

    // Mark this child for ENTER-slot dispatch only when this view has a
    // LayoutTransition attached at the time of the add. Recording
    // unconditionally would (a) accumulate stale entries while no
    // transition is attached, ready to mis-fire as ENTER once a future
    // SetLayoutTransition + unrelated layout pass runs, and (b) keep
    // raw ViewImpl* pointers alive in the set after the child is
    // destroyed (no global cleanup hook today). Recording at the event
    // time matches the semantic that ENTER is for "child added under a
    // transition-bearing parent".
    if(mImpl->mLayoutTransition)
    {
      mImpl->mPendingEnterChildren.insert(&childImpl);
    }

    // Standalone children do not contribute to this view's OnMeasure/OnArrange
    // accumulation, so adding one does not invalidate this view's cached
    // measured size or arranged bounds. Skip self-invalidation in that case.
    const bool childAffectsSelf = !IntegrationView::IsLayoutModeStandalone(childImpl);

    // Reset the effective scale cache for the entire subtree being added.
    // The child may be arriving from a parent with a different UiScalePolicy
    // context (e.g. reparented from DISABLED to INHERIT), so every descendant's
    // cached mEffectiveScale is potentially stale and must be recomputed.
    //
    // InvalidateMeasure() alone is not sufficient: it only resets
    // mEffectiveScale on the direct child and propagates *upward*. Descendants
    // retain their old cached values and use them in the next Measure() call,
    // producing incorrect font sizes, paddings, and decorations even though
    // the layout container size updates correctly.
    //
    // ResetEffectiveScaleRecursive() sets mEffectiveScale = -1.0f and
    // mLastMeasuredConstraint = NaN for every node in the subtree, guaranteeing:
    //   (a) scale is recomputed from the new parent chain on next GetEffectiveScale(), and
    //   (b) the NaN constraint forces a cache miss in Measure() so all nodes
    //       fully re-measure with the new scale.
    //
    // After this, InvalidateMeasure() transitions the direct child from NaN to
    // DIRTY (the guard passes because NaN != DIRTY per IEEE 754) and propagates
    // up to the new layout root.
    childImpl.ResetEffectiveScaleRecursive();

    // Invalidate the child's measure cache — its previous cache was computed
    // under a different parent's constraints and is no longer reliable.
    childImpl.InvalidateMeasure();

    if(childAffectsSelf)
    {
      // Also invalidate this view's chain directly. The child's
      // InvalidateMeasure may early-exit via the dirty guard if the child was
      // already invalidated under its previous parent (reparenting of a dirty
      // child), in which case the new parent chain would not be reached.
      // Calling InvalidateMeasure on self guarantees the new ancestor chain
      // is marked and the new layout root is registered. If self is already
      // dirty, the guard makes this a no-op.
      //
      // For standalone (boundary) children, this fallback is unnecessary: the
      // child's own InvalidateMeasure registers it as a layout root (Phase 2
      // boundary rule), and OnSceneConnection re-registers dirty boundaries
      // that were already dirty when reparented.
      InvalidateMeasure();
    }
    else if(mImpl->mLayoutTransition)
    {
      // Standalone child + transition-attached parent: the standalone
      // path above does not dirty self, so this view would not be
      // reached by ProcessLayouts and the dispatcher would never run
      // its CaptureBeforeLayout / StartTransitionsAfterLayout pass for
      // this parent — meaning the ENTER (and any subsequent CHANGE)
      // would never be dispatched, while the pending-enter set
      // accumulates entries that fire late on the next unrelated
      // dirty event. Force the parent dirty so its dispatcher pass
      // runs in the same layout batch as the standalone child's.
      InvalidateMeasure();
    }
  }
  else
  {
    if(gAllowNonViewChild)
    {
      // Permitted via IntegrationView::AddActorChild: skip the View-only check
      // and do not record this child in mImpl->mChildren (it is excluded from layout).
      return;
    }
    DALI_ASSERT_ALWAYS(false && "View could only have child as View class!");
  }
}

void ViewImpl::OnChildRemove(Actor& child)
{
  if(mImpl->mSkipChildrenUpdate)
  {
    return;
  }

  Ui::View view = Ui::View::DownCast(child);
  if(view)
  {
    auto it = std::find(mImpl->mChildren.begin(), mImpl->mChildren.end(), view);
    if(it != mImpl->mChildren.end())
    {
      ViewImpl& childImpl = GetImpl(view);

      // Standalone children are excluded from this view's OnMeasure/OnArrange
      // accumulation, so removing one does not change this view's measured
      // size or arranged bounds. Skip self-invalidation in that case to avoid
      // an unnecessary parent chain walk.
      const bool childWasAffectingSelf = !IntegrationView::IsLayoutModeStandalone(childImpl);

      // If the child was added and removed within the same frame (before
      // any layout pass consumed the pending-enter set), drop the ENTER
      // marker so the dispatcher does not fire on a no-longer-present view.
      // Same for the reorder marker: Insert() / OnChildOrderChanged keep
      // raw ViewImpl* pointers in mPendingReorderedChildren which must
      // not survive the child's removal — otherwise a heap-reused address
      // could mis-classify a future child as REORDERED.
      mImpl->mPendingEnterChildren.erase(&childImpl);
      mImpl->mPendingReorderedChildren.erase(&childImpl);

      // Record sibling removal so the next CHANGE pass tags remaining
      // siblings as SIBLING_REMOVED. This covers paths that reach
      // OnChildRemove without going through View::Remove's marker-
      // setting branch (e.g. inherited Actor::Remove called directly on
      // the view actor). Same window guard as View::Remove — without
      // a window the marker cannot be consumed in this pass and would
      // leak across a later add-to-window event. Setting the marker
      // here is idempotent with View::Remove's own setter.
      if(mImpl->mLayoutTransition && DevelWindow::Get(Self()))
      {
        mImpl->mPendingChildRemovalForLayoutTransition = true;
      }

      // Invalidate the removed child's measure cache so that it gets
      // re-measured when re-parented to a different container.
      // Note: Actor parent-child relationship is already severed at this
      // point, so child's InvalidateMeasure cannot propagate to us.
      childImpl.InvalidateMeasure();
      mImpl->mChildren.Erase(it);

      if(childWasAffectingSelf)
      {
        InvalidateMeasure();
      }
    }
  }
}

void ViewImpl::OnChildOrderChanged(Actor orderChangedChild)
{
  if(mImpl->mSkipChildrenUpdate)
  {
    return;
  }

  Actor                           self            = Self();
  uint32_t                        actorChildCount = self.GetChildCount();
  IntegrationView::ChildContainer newChildren;
  newChildren.Reserve(actorChildCount);

  for(uint32_t i = 0; i < actorChildCount; ++i)
  {
    Ui::View view = Ui::View::DownCast(self.GetChildAt(i));
    if(view)
    {
      auto it = std::find(mImpl->mChildren.begin(), mImpl->mChildren.end(), view);
      if(it != mImpl->mChildren.end())
      {
        newChildren.PushBack(std::move(*it));
      }
    }
  }

  mImpl->mChildren = std::move(newChildren);

  // Tag every child as reordered so the layout transition dispatcher can
  // tag CHANGE-slot dispatches with @c LayoutChangeCause::REORDERED. The
  // dispatcher consumes this set once per layout pass. Skip when no
  // transition is attached so stale records cannot leak across an
  // unrelated SetLayoutTransition + layout pass later, and so raw
  // ViewImpl* pointers do not outlive their owning views without a
  // central cleanup hook.
  if(mImpl->mLayoutTransition)
  {
    for(auto& childView : mImpl->mChildren)
    {
      mImpl->mPendingReorderedChildren.insert(&GetImpl(childView));
    }
  }

  InvalidateArrange();
}

void ViewImpl::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  // If the clipping mode has been set, we may need to create a renderer.
  // Only do this if we are already on-stage as the OnSceneConnection will handle the off-stage clipping controls.
  switch(index)
  {
    case Actor::Property::CLIPPING_MODE:
    {
      if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
      {
        // Note: This method will handle whether creation of the renderer is required.
        CreateClippingRenderer(*this);
      }
      break;
    }
    case DevelActor::Property::USER_INTERACTION_ENABLED:
    {
      const bool enabled = propertyValue.Get<bool>();
      if(!enabled && Self() == Dali::Ui::FocusManager::Get().GetCurrentFocusView())
      {
        Dali::Ui::FocusManager::Get().ClearFocus();
      }

      IntegrationView::SetState(*this, ViewState::DISABLED, !enabled);

      if(auto* interactiveTrait = mImpl->GetInteractiveTrait())
      {
        interactiveTrait->OnEnabledChanged(View::DownCast(Self()), enabled);
      }
      break;
    }
  }
}

void ViewImpl::OnSizeSet(const Vector3& targetSize)
{
  Vector2 size(targetSize);

  mImpl->mSize = Vector2(targetSize);

  // Notify that size or UiScale changed
  mImpl->SizeOrUiScaleChanged();
}

void ViewImpl::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  // @todo size negotiate background to new size, animate as well?

  // TODO : Could we clear animation constraint when size animation stopped?
  mImpl->CreateAnimationConstraints(animation.GetBaseObject(), Dali::Actor::Property::SIZE);
}

void ViewImpl::OnAnimateAnimatableProperty(Animation& animation, Property::Index index, Animation::State state)
{
  if(state == Animation::State::PLAYING)
  {
    mImpl->CreateAnimationConstraints(animation.GetBaseObject(), index);
  }
  else if(state == Animation::State::STOPPED)
  {
    mImpl->ClearAnimationConstraints(animation.GetBaseObject(), index);
  }
}

void ViewImpl::OnConstraintAnimatableProperty(Constraint& constraint, Property::Index index, bool applied)
{
  if(applied)
  {
    mImpl->CreateAnimationConstraints(constraint.GetBaseObject(), index);
  }
  else
  {
    mImpl->ClearAnimationConstraints(constraint.GetBaseObject(), index);
  }
}

void ViewImpl::GetOffScreenRenderTasks(Dali::Vector<Dali::RenderTask>& tasks, bool isForward)
{
  if(mImpl->mRenderEffect)
  {
    mImpl->mRenderEffect->GetOffScreenRenderTasks(tasks, isForward);
  }
  if(mImpl->mOffScreenRenderingImpl)
  {
    mImpl->mOffScreenRenderingImpl->GetOffScreenRenderTasks(tasks, isForward);
  }
}

bool ViewImpl::IsResourceReady() const
{
  const Internal::ViewDataImpl& viewDataImpl = Internal::ViewDataImpl::Get(*this);
  return viewDataImpl.IsResourceReady();
}

bool ViewImpl::IsOnScene() const
{
  return Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE);
}

void ViewImpl::OnSetResizePolicy(ResizePolicy::Type policy, Dimension::Type dimension)
{
}

Vector3 ViewImpl::GetNaturalSize()
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "ViewImpl::GetNaturalSize for %s\n",
                Self().GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr());
  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr)
  {
    Vector2 naturalSize;
    visualImplPtr->GetNaturalSize(naturalSize);
    naturalSize.width += (mImpl->mPadding.start + mImpl->mPadding.end);
    naturalSize.height += (mImpl->mPadding.top + mImpl->mPadding.bottom);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
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

namespace IntegrationView
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

} // namespace IntegrationView
} // namespace Ui
} // namespace Dali
