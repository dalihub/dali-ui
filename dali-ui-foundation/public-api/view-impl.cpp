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
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/common/stage.h>
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

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/color-visual-properties-devel.h>
#include <dali-ui-foundation/devel-api/visuals/visual-actions-devel.h>
#include <dali-ui-foundation/integration-api/layouts/layout-impl.h>
#include <dali-ui-foundation/integration-api/reserved-trait-id.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-callbacks-trait.h>
#include <dali-ui-foundation/internal/layouts/layout-params-impl.h>
#include <dali-ui-foundation/internal/render-effects/render-effect-impl.h>
#include <dali-ui-foundation/internal/ui-color-manager-impl.h>
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
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
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
      [](Dali::Actor actor) -> std::pair<std::shared_ptr<Accessibility::Accessible>, bool>
    {
      auto view = Ui::View::DownCast(actor);
      if(!view)
      {
        return {nullptr, true};
      }

      auto& viewImpl = GetImpl(view);
      if(Dali::Ui::Internal::ViewDataImpl::Get(viewImpl).IsCreateAccessibleEnabled())
      {
        return {std::shared_ptr<ViewAccessible>(viewImpl.CreateAccessibleObject()), true};
      }
      return {nullptr, false};
    });
  }
}

Internal::LayoutCallbacksTraitImpl* GetLayoutCallbacksTrait(ViewImpl* self)
{
  Dali::BaseHandle handle = Internal::ViewDataImpl::Get(*self).GetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS);
  if(handle)
  {
    return static_cast<Internal::LayoutCallbacksTraitImpl*>(&handle.GetBaseObject());
  }
  return nullptr;
}

Internal::LayoutCallbacksTraitImpl* EnsureLayoutCallbacksTrait(ViewImpl* self)
{
  auto* impl = GetLayoutCallbacksTrait(self);
  if(!impl)
  {
    impl                                  = new Internal::LayoutCallbacksTraitImpl();
    Internal::LayoutCallbacksTrait handle = Internal::LayoutCallbacksTrait::New(impl);
    Internal::ViewDataImpl::Get(*self).SetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS, handle);
  }
  return impl;
}

// Arranges a standalone MATCH_PARENT child within its parent.
// Standalone children ignore parent padding. MATCH_PARENT axes are
// expanded to the full parent size minus the child's own margin, then
// re-measured with the final size before arranging.
void ArrangeStandaloneChild(ViewImpl& childImpl,
                            float parentFullWidth, float parentFullHeight)
{
  Extents      margin   = childImpl.GetMargin();
  float        marginW  = static_cast<float>(margin.start + margin.end);
  float        marginH  = static_cast<float>(margin.top + margin.bottom);
  MeasuredSize measured = childImpl.GetMeasuredSize();
  float        childW   = measured.width;
  float        childH   = measured.height;

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

  LayoutRect bounds(childImpl.GetRequestedPositionX() + static_cast<float>(margin.start),
                    childImpl.GetRequestedPositionY() + static_cast<float>(margin.top),
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
  auto manager = UiColorManager::Get();
  if(manager)
  {
    GetImpl(manager).ClearBindings(GetOwner());
  }

  mImpl->NotifyTraitsViewDestroying();

  LayoutController::UnregisterFromAll(this);

  ClearRenderEffect();

  delete mImpl;
}

void ViewImpl::OnInitialize()
{
  Self().SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  Self().SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  Self().SetProperty(Actor::Property::POSITION_USES_PIVOT, false);

  DevelActor::ChildOrderChangedSignal(Self()).Connect(this, &ViewImpl::OnChildOrderChanged);
}

void ViewImpl::OnSceneConnection(int depth)
{
  mImpl->OnSceneConnection();
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
  Ui::InteractiveTrait existing = IntegrationView::GetTrait<Ui::InteractiveTrait>(*this, Integration::ReservedTraitId::INTERACTION_TRAIT);

  if(!existing)
  {
    Ui::InteractiveTrait interaction = Ui::InteractiveTrait::New();
    IntegrationView::SetTrait(*this, Integration::ReservedTraitId::INTERACTION_TRAIT, interaction);

    // Apply interaction effect only if the user has not already set one explicitly.
    if(!IntegrationView::GetTrait(*this, Integration::ReservedTraitId::INTERACTION_EFFECT))
    {
      Trait defaultEffect = Integration::UiConfigManager::Get().GetConfig().GetDefaultInteractionEffect();
      if(defaultEffect)
      {
        IntegrationView::SetTrait(*this, Integration::ReservedTraitId::INTERACTION_EFFECT, defaultEffect);
      }
    }

    return interaction;
  }

  return existing;
}

void ViewImpl::SetInteractionEffect(Trait effect)
{
  if(effect)
  {
    IntegrationView::SetTrait(*this, Integration::ReservedTraitId::INTERACTION_EFFECT, effect);
  }
  else
  {
    IntegrationView::RemoveTrait(*this, Integration::ReservedTraitId::INTERACTION_EFFECT);
  }
}

bool ViewImpl::IsInteractive() const
{
  return !!IntegrationView::GetTrait(*this, Integration::ReservedTraitId::INTERACTION_TRAIT);
}

Ui::SelectableTrait ViewImpl::EnsureSelectableTrait()
{
  Ui::SelectableTrait existing = IntegrationView::GetTrait<Ui::SelectableTrait>(*this, Integration::ReservedTraitId::SELECTABLE_TRAIT);

  if(!existing)
  {
    Ui::SelectableTrait selectable = Ui::SelectableTrait::New();
    IntegrationView::SetTrait(*this, Integration::ReservedTraitId::SELECTABLE_TRAIT, selectable);
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
  if(IntegrationView::IsLayout(*this) || GetParentLayout() || GetParentView())
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

MeasuredSize ViewImpl::Measure(float widthConstraint, float heightConstraint)
{
  // Ensure constraints respect this view's min/max bounds so that
  // OnMeasure (and therefore child measurements) see the effective
  // available space. Without this, children would be measured against
  // the original (smaller) constraint, then ApplyConstraints would
  // enlarge the result — leaving children incorrectly sized.
  float effectiveWidth  = std::min(std::max(widthConstraint, mImpl->mMinimumWidth), mImpl->mMaximumWidth);
  float effectiveHeight = std::min(std::max(heightConstraint, mImpl->mMinimumHeight), mImpl->mMaximumHeight);

  if(mImpl->mLastMeasuredConstraint.width >= 0.0f && FloatEqual(mImpl->mLastMeasuredConstraint.width, effectiveWidth) &&
     FloatEqual(mImpl->mLastMeasuredConstraint.height, effectiveHeight))
  {
    return mImpl->mMeasuredSize;
  }

  MeasuredSize measured                 = OnMeasure(effectiveWidth, effectiveHeight);
  measured                              = ApplyConstraints(measured);
  mImpl->mMeasuredSize                  = measured;
  mImpl->mLastMeasuredConstraint.width  = effectiveWidth;
  mImpl->mLastMeasuredConstraint.height = effectiveHeight;

  // Ensure standalone children are measured even when OnMeasure (e.g. in
  // leaf views like Label) does not iterate children. The measure cache
  // prevents redundant work when OnMeasure already measured them.
  MeasureStandaloneChildren(effectiveWidth, effectiveHeight);

  return mImpl->mMeasuredSize;
}

MeasuredSize ViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  float pw = static_cast<float>(mImpl->mPadding.start + mImpl->mPadding.end);
  float ph = static_cast<float>(mImpl->mPadding.top + mImpl->mPadding.bottom);

  float effectiveWidth  = (mImpl->mRequestedWidth >= 0) ? mImpl->mRequestedWidth : widthConstraint;
  float effectiveHeight = (mImpl->mRequestedHeight >= 0) ? mImpl->mRequestedHeight : heightConstraint;

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

      Extents      margin                = childImpl.GetMargin();
      float        marginW               = static_cast<float>(margin.start + margin.end);
      float        marginH               = static_cast<float>(margin.top + margin.bottom);
      float        childWidthConstraint  = std::max(0.0f, contentWidth - marginW);
      float        childHeightConstraint = std::max(0.0f, contentHeight - marginH);
      MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);

      float childX = childImpl.GetRequestedPositionX();
      float childY = childImpl.GetRequestedPositionY();
      maxRight     = std::max(maxRight, childX + marginW + childSize.width);
      maxBottom    = std::max(maxBottom, childY + marginH + childSize.height);
    }

    MeasuredSize size;
    if(mImpl->mRequestedWidth >= 0)
    {
      size.width = mImpl->mRequestedWidth;
    }
    else if(mImpl->mRequestedWidth == MATCH_PARENT)
    {
      size.width = mImpl->mMinimumWidth;
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
      size.height = mImpl->mMinimumHeight;
    }
    else
    {
      size.height = maxBottom + ph;
    }
    return size;
  }

  MeasuredSize size;
  if(mImpl->mRequestedWidth >= 0)
  {
    size.width = mImpl->mRequestedWidth;
  }
  else if(mImpl->mRequestedWidth == MATCH_PARENT)
  {
    size.width = mImpl->mMinimumWidth;
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
    size.height = mImpl->mMinimumHeight;
  }
  else
  {
    Vector3 naturalSize = Self().GetNaturalSize();
    size.height         = ((naturalSize.height > 0) ? naturalSize.height : 0.0f) + ph;
  }
  return size;
}

MeasuredSize ViewImpl::Arrange(const LayoutRect& bounds)
{
  MeasuredSize arrangedSize = OnArrange(bounds);
  mImpl->mArrangedBounds    = bounds;
  mImpl->mArrangeDirty      = false;

  // Ensure standalone children are arranged even when OnArrange (e.g. in
  // leaf views like Label) does not iterate children.
  ArrangeStandaloneChildren(bounds);

  // Mirror direct children when the effective layout direction resolves to
  // RIGHT_TO_LEFT. Runs once per Arrange after every OnArrange variant
  // (LayoutManager / ArrangeCallback / default), keeping layout managers
  // direction-agnostic.
  ApplyLayoutDirection(bounds.width);

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
    float padLeft   = static_cast<float>(mImpl->mPadding.start);
    float padRight  = static_cast<float>(mImpl->mPadding.end);
    float padTop    = static_cast<float>(mImpl->mPadding.top);
    float padBottom = static_cast<float>(mImpl->mPadding.bottom);

    for(auto& childView : mImpl->mChildren)
    {
      ViewImpl& childImpl = GetImpl(childView);

      // Standalone children are handled by ArrangeStandaloneChildren()
      // in ViewImpl::Arrange() after OnArrange returns.
      if(IntegrationView::IsLayoutModeStandalone(childImpl))
      {
        continue;
      }

      Extents margin  = childImpl.GetMargin();
      float   marginW = static_cast<float>(margin.start + margin.end);
      float   marginH = static_cast<float>(margin.top + margin.bottom);
      // Read measured size directly from the child (set during OnMeasure).
      MeasuredSize childMeasured = childImpl.GetMeasuredSize();
      float        childW        = childMeasured.width;
      float        childH        = childMeasured.height;

      // MATCH_PARENT: fills parent content area minus own margin.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childW = std::max(0.0f, width - padLeft - padRight - marginW);
      }
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childH = std::max(0.0f, height - padTop - padBottom - marginH);
      }
      float childX = padLeft + static_cast<float>(margin.start) + childImpl.GetRequestedPositionX();
      float childY = padTop + static_cast<float>(margin.top) + childImpl.GetRequestedPositionY();

      // MATCH_PARENT children reported minSize during Measure, but their
      // subtree was measured with the original constraint. Re-measure with
      // the actual final size so internal state (e.g. text layout, nested
      // children) reflects the real available space.
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

void ViewImpl::MeasureStandaloneChildren(float effectiveWidth, float effectiveHeight)
{
  for(auto& childView : mImpl->mChildren)
  {
    ViewImpl& childImpl = GetImpl(childView);
    if(!IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      continue;
    }
    Extents margin                = childImpl.GetMargin();
    float   marginW               = static_cast<float>(margin.start + margin.end);
    float   marginH               = static_cast<float>(margin.top + margin.bottom);
    float   childWidthConstraint  = std::max(0.0f, effectiveWidth - marginW);
    float   childHeightConstraint = std::max(0.0f, effectiveHeight - marginH);
    childImpl.Measure(childWidthConstraint, childHeightConstraint);
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

  mImpl->mLastMeasuredConstraint.width  = MEASURE_CACHE_DIRTY;
  mImpl->mLastMeasuredConstraint.height = MEASURE_CACHE_DIRTY;
  mImpl->mArrangeDirty                  = true;

  // Layout boundary: a standalone view is excluded from its parent's
  // OnMeasure/OnArrange accumulation, so its measure result cannot change
  // the parent's measured size. Stop propagation here and register this view
  // as its own layout root.
  if(IntegrationView::IsLayoutModeStandalone(*this))
  {
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
  }
}

MeasuredSize ViewImpl::GetMeasuredSize() const
{
  return mImpl->mMeasuredSize;
}

MeasuredSize ViewImpl::ApplyConstraints(const MeasuredSize& size) const
{
  MeasuredSize constrained = size;
  constrained.width        = std::max(constrained.width, mImpl->mMinimumWidth);
  constrained.height       = std::max(constrained.height, mImpl->mMinimumHeight);
  constrained.width        = std::min(constrained.width, mImpl->mMaximumWidth);
  constrained.height       = std::min(constrained.height, mImpl->mMaximumHeight);
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
  return mImpl->mMinimumWidth;
}

void ViewImpl::SetMinimumHeight(float height)
{
  Self().SetProperty(Ui::View::Property::MINIMUM_HEIGHT, height);
}

float ViewImpl::GetMinimumHeight() const
{
  return mImpl->mMinimumHeight;
}

void ViewImpl::SetMaximumWidth(float width)
{
  Self().SetProperty(Ui::View::Property::MAXIMUM_WIDTH, width);
}

float ViewImpl::GetMaximumWidth() const
{
  return mImpl->mMaximumWidth;
}

void ViewImpl::SetMaximumHeight(float height)
{
  Self().SetProperty(Ui::View::Property::MAXIMUM_HEIGHT, height);
}

float ViewImpl::GetMaximumHeight() const
{
  return mImpl->mMaximumHeight;
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
  EnsureLayoutCallbacksTrait(this)->SetMeasureCallback(std::move(callback));
}

void ViewImpl::SetArrangeCallback(ArrangeCallback callback)
{
  EnsureLayoutCallbacksTrait(this)->SetArrangeCallback(std::move(callback));
}

MeasureCallback* ViewImpl::GetMeasureCallback()
{
  auto* impl = GetLayoutCallbacksTrait(this);
  return impl ? impl->GetMeasureCallback() : nullptr;
}

ArrangeCallback* ViewImpl::GetArrangeCallback()
{
  auto* impl = GetLayoutCallbacksTrait(this);
  return impl ? impl->GetArrangeCallback() : nullptr;
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
  {
    ScopedSkipChildrenUpdate guard(mImpl->mSkipChildrenUpdate);
    for(auto& childView : mImpl->mChildren)
    {
      // Invalidate each child's measure cache so that re-parented children
      // are re-measured under the new parent's constraints.
      GetImpl(childView).InvalidateMeasure();
      Self().Remove(childView);
    }
  }

  mImpl->mChildren.Clear();
  InvalidateMeasure();
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
  return mImpl->GetTrait(ToTraitId(type));
}

void ViewImpl::SetLayoutParams(Ui::LayoutParams params)
{
  auto& paramsImpl = static_cast<Internal::LayoutParamsImpl&>(params.GetBaseObject());
  mImpl->SetTrait(paramsImpl.GetTraitId(), params);
  InvalidateMeasure();
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

  if(mImpl->mFlags & Ui::ViewImpl::ViewBehaviour::REQUIRES_KEY_NAVIGATION_SUPPORT)
  {
    SetKeyNavigationSupport(true);
  }
}

void ViewImpl::SetBackgroundColorInternal(const Vector4& color)
{
  mImpl->mBackgroundColor = color;

  Property::Map map;
  map.Insert(Ui::Visual::Property::TYPE, Ui::Visual::COLOR);
  map.Insert(Ui::ColorVisual::Property::MIX_COLOR, color);

  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr && visualImplPtr->GetType() == Ui::Visual::COLOR)
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

void ViewImpl::SetKeyNavigationSupport(bool isSupported)
{
  Self().SetProperty(Ui::View::Property::KEY_NAVIGATION_SUPPORT, isSupported);
}

bool ViewImpl::IsKeyNavigationSupported() const
{
  return mImpl->mIsKeyNavigationSupported;
}

void ViewImpl::SetAsFocusGroup(bool isFocusGroup)
{
  Self().SetProperty(Ui::View::Property::FOCUS_GROUP, isFocusGroup);
}

bool ViewImpl::IsFocusGroup()
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

Ui::View ViewImpl::GetNextFocusableView(Ui::View currentFocusedView, Ui::FocusDirection direction,
                                        bool loopEnabled)
{
  return Ui::View();
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
        parent->EmitActiveDescendantChanged(accessible.get());
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

  mImpl->OnSceneDisconnection();
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

    // Standalone children do not contribute to this view's OnMeasure/OnArrange
    // accumulation, so adding one does not invalidate this view's cached
    // measured size or arranged bounds. Skip self-invalidation in that case.
    const bool childAffectsSelf = !IntegrationView::IsLayoutModeStandalone(childImpl);

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

  Ui::Internal::Visual::Base* visualImplPtr = mImpl->GetVisualImplPtr(Ui::View::Property::BACKGROUND);
  if(visualImplPtr)
  {
    visualImplPtr->SetControlSize(size); // Send an empty map as we do not want to modify the visual's set transform
  }

  // Apply FittingMode here
  mImpl->mSize = Vector2(targetSize);
  mImpl->RegisterProcessorOnce();

  // Refresh render effects
  if(mImpl->mRenderEffect)
  {
    mImpl->mRenderEffect->Refresh();
  }

  if(mImpl->mOffScreenRenderingImpl)
  {
    mImpl->mOffScreenRenderingImpl->Refresh();
  }
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

    AllowNonViewChildScope(const AllowNonViewChildScope&)            = delete;
    AllowNonViewChildScope& operator=(const AllowNonViewChildScope&) = delete;
  } scope;

  view.Add(actor);
}

} // namespace IntegrationView
} // namespace Ui
} // namespace Dali
