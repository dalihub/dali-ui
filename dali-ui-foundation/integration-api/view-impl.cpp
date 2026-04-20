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
#include <dali-ui-foundation/integration-api/layouts/layout-manager.h>
#include <dali-ui-foundation/integration-api/trait-id.h>
#include <dali-ui-foundation/integration-api/trait-impl.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/integration-api/view-impl.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>
#include <dali-ui-foundation/internal/focus-manager/keyinput-focus-manager.h>
#include <dali-ui-foundation/internal/layouts/layout-callbacks-impl.h>
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
#include <dali-ui-foundation/public-api/ui-color-manager.h>
#include <dali-ui-foundation/public-api/ui-color.h>
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

namespace Integration
{

namespace
{

/// Scoped guard that lets View::OnChildAdd accept a non-View Actor as a child.
/// Used by Integration::AddActorChild. Thread-local because Actor::Add invokes
/// OnChildAdd synchronously on the same (event) thread.
thread_local bool gAllowNonViewChild = false;

/// RAII guard for ViewImpl::mSkipChildrenUpdate. Saves and restores the
/// previous value, so nested scopes (e.g. a signal handler re-entering Insert
/// while an outer PRESERVE Raise is active) cannot prematurely unguard the
/// outer scope.
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
DALI_TYPE_REGISTRATION_BEGIN(Ui::Integration::ViewImpl, Ui::View, Create)
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

      auto& viewImpl = Integration::GetImpl(view);
      if(Dali::Ui::Internal::ViewDataImpl::Get(viewImpl).IsCreateAccessibleEnabled())
      {
        return {std::shared_ptr<ViewAccessible>(viewImpl.CreateAccessibleObject()), true};
      }
      return {nullptr, false};
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
  mInteractiveTrait(nullptr),
  mRequestedPositionX(0.0f),
  mRequestedPositionY(0.0f),
  mMeasuredSize{0.0f, 0.0f},
  mLastMeasuredConstraint{-1.0f, -1.0f},
  mArrangedBounds{0.0f, 0.0f, 0.0f, 0.0f},
  mArrangeValid(false),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->mFlags = static_cast<Ui::Integration::ViewImpl::ViewBehaviour>(
    static_cast<int>(VIEW_BEHAVIOUR_DEFAULT) |
    static_cast<int>(Dali::CustomActorImpl::DISABLE_SIZE_NEGOTIATION));
}

ViewImpl::~ViewImpl()
{
  auto manager = UiColorManager::Get();
  GetImpl(manager).ClearBindings(GetOwner());

  for(auto& iter : mTraits)
  {
    GetImpl(iter.second).OnViewDestroying(this);
  }

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

  if(!GetParentView())
  {
    RegisterWithLayoutController();
  }
}

bool ViewImpl::OnKeyEvent(const Dali::KeyEvent& event)
{
  if(mInteractiveTrait)
  {
    return mInteractiveTrait->OnKeyEvent(View::DownCast(Self()), event);
  }
  return false;
}

void ViewImpl::OnFocusGained()
{
  OnFocusChanged(true, GetImpl(Ui::FocusManager::Get()).FocusChangedContext().inputEvent);
}

void ViewImpl::OnFocusLost()
{
  OnFocusChanged(false, GetImpl(Ui::FocusManager::Get()).FocusChangedContext().inputEvent);
}

// =============================================================================
// State API
// =============================================================================

const ViewState& ViewImpl::GetState() const
{
  return mState;
}

bool ViewImpl::IsEnabled() const
{
  return Self().GetProperty<bool>(DevelActor::Property::USER_INTERACTION_ENABLED);
}

void ViewImpl::SetEnabled(bool enabled)
{
  Self().SetProperty(DevelActor::Property::USER_INTERACTION_ENABLED, enabled);
  OnEnableChanged(enabled);
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
  return mStateChangedSignal;
}

Ui::InteractiveTrait ViewImpl::EnsureInteractiveTrait()
{
  Trait existing = GetTrait(ReservedTraitId::INTERACTION_TRAIT);

  if(!existing)
  {
    Ui::InteractiveTrait interaction = Ui::InteractiveTrait::New();
    SetTrait(ReservedTraitId::INTERACTION_TRAIT, interaction);

    // Apply interaction effect only if the user has not already set one explicitly.
    if(!GetTrait(ReservedTraitId::INTERACTION_EFFECT))
    {
      Trait defaultEffect = UiConfigManager::Get().GetConfig().GetDefaultInteractionEffect();
      if(defaultEffect)
      {
        SetTrait(ReservedTraitId::INTERACTION_EFFECT, defaultEffect);
      }
    }

    return interaction;
  }

  Ui::InteractiveTrait interaction = Ui::InteractiveTrait::DownCast(existing);
  DALI_ASSERT_ALWAYS(interaction && "View already has a different interaction trait; cannot attach InteractiveTrait");
  return interaction;
}

void ViewImpl::SetInteractionEffect(Trait effect)
{
  if(effect)
  {
    SetTrait(ReservedTraitId::INTERACTION_EFFECT, effect);
  }
  else
  {
    RemoveTrait(ReservedTraitId::INTERACTION_EFFECT);
  }
}

bool ViewImpl::IsInteractive() const
{
  return !!GetTrait(ReservedTraitId::INTERACTION_TRAIT);
}

Ui::SelectableTrait ViewImpl::EnsureSelectableTrait()
{
  Trait existing = GetTrait(ReservedTraitId::SELECTABLE_TRAIT);

  if(!existing)
  {
    Ui::SelectableTrait selectable = Ui::SelectableTrait::New();
    SetTrait(ReservedTraitId::SELECTABLE_TRAIT, selectable);
    return selectable;
  }

  Ui::SelectableTrait selectable = Ui::SelectableTrait::DownCast(existing);
  DALI_ASSERT_ALWAYS(selectable && "View already has a different selectable trait; cannot attach SelectableTrait");
  return selectable;
}

bool ViewImpl::IsSelectable() const
{
  return !!GetTrait(ReservedTraitId::SELECTABLE_TRAIT);
}

void ViewImpl::SetNamedStateHandler(const Dali::String& id, Dali::ConnectionTrackerInterface* tracker, CallbackBase* callback)
{
  Trait existing = GetTrait(ReservedTraitId::STATE_HANDLER_TRAIT);

  if(!existing)
  {
    Internal::StateHandlerTrait stateHandlerTrait = Internal::StateHandlerTrait::New();
    SetTrait(ReservedTraitId::STATE_HANDLER_TRAIT, stateHandlerTrait);
    existing = stateHandlerTrait;
  }

  static_cast<Internal::StateHandlerTrait&>(existing).GetImpl().Set(id.CStr(), tracker, callback);
}

bool ViewImpl::UnsetStateHandler(const Dali::String& id)
{
  Trait existing = GetTrait(ReservedTraitId::STATE_HANDLER_TRAIT);
  if(!existing)
  {
    return false;
  }

  return static_cast<Internal::StateHandlerTrait&>(existing).GetImpl().Unset(id.CStr());
}

bool ViewImpl::UnsetStateHandlerWhenNotProcessing(const Dali::String& id)
{
  Trait existing = GetTrait(ReservedTraitId::STATE_HANDLER_TRAIT);
  if(!existing)
  {
    return false;
  }

  return static_cast<Internal::StateHandlerTrait&>(existing).GetImpl().UnsetWhenNotProcessing(id.CStr());
}

void ViewImpl::SetViewState(ViewState state, bool on, InputEvent cause)
{
  // NOTE Orthogonal state constraint: Disabled is mutually exclusive with Focused and Pressed.
  // Clear them immediately rather than waiting for potentially late system events.

  // NOTE that when the view is focused and user sets `view.SetEnabled(false)`,
  // the event squence will be: "Focused out" -> "Enabled changed".

  ViewState prev = mState;
  if(on)
  {
    mState = mState + state;

    // NOTE Handle orthogonal state constraint
    // When DISABLED added,
    // - PRESSED needs to be cleaned immediately
    // - FOCUSED should have gone already (ASSERT(!mState.Contains(FOCUSED)))
    // When PSUEDO_DISABLED added,
    // - PRESSED needs to be cleaned immediately
    // - FOCUSED can exist
    if(state.IsAnyDisabled())
    {
      mState = mState - ViewState::PRESSED;
    }
  }
  else
  {
    mState = mState - state;

    // NOTE Handle orthogonal state constraint
    // This is the case that the focus has gone because it turned disabled.
    // (but disabled state hasn't dispatched yet)
    // -> Immediately update states at once.
    if(state == ViewState::FOCUSED && !IsEnabled())
    {
      mState = mState - ViewState::PRESSED + ViewState::DISABLED;
    }
  }

  if(mState != prev)
  {
    Internal::ViewStateManager::Get().NotifyStateChanged(View::DownCast(Self()), prev, mState, cause);
  }
}

void ViewImpl::OnFocusChanged(bool focused, InputEvent cause)
{
  SetViewState(ViewState::FOCUSED, focused, cause);

  if(mInteractiveTrait)
  {
    mInteractiveTrait->OnFocusedChanged(View::DownCast(Self()), focused);
  }

  EmitFocusChangedSignal(focused);
}

void ViewImpl::OnEnableChanged(bool enabled)
{
  SetViewState(ViewState::DISABLED, !enabled);

  if(mInteractiveTrait)
  {
    mInteractiveTrait->OnEnabledChanged(View::DownCast(Self()), enabled);
  }
}

void ViewImpl::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  if(IsLayout() || GetParentLayout() || GetParentView())
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

      Dali::CustomActor           ownerActor(GetOwner());
      Dali::LayoutDirection::Type layoutDirection = static_cast<Dali::LayoutDirection::Type>(
        ownerActor.GetProperty(Dali::Actor::Property::LAYOUT_DIRECTION).Get<int>());

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
    auto accessible = GetAccessibleObject();
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

void ViewImpl::SetScaleX(float scaleX)
{
  Self().SetProperty(Actor::Property::SCALE_X, scaleX);
}

float ViewImpl::GetScaleY() const
{
  return Self().GetProperty<float>(Actor::Property::SCALE_Y);
}

void ViewImpl::SetScaleY(float scaleY)
{
  Self().SetProperty(Actor::Property::SCALE_Y, scaleY);
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

float ViewImpl::GetPositionX() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_X);
}

void ViewImpl::SetPositionX(float x)
{
  mRequestedPositionX = x;
  Self().SetProperty(Actor::Property::POSITION_X, x);
}

float ViewImpl::GetPositionY() const
{
  return Self().GetProperty<float>(Actor::Property::POSITION_Y);
}

void ViewImpl::SetPositionY(float y)
{
  mRequestedPositionY = y;
  Self().SetProperty(Actor::Property::POSITION_Y, y);
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

void ViewImpl::SetTrait(TraitId id, Trait& trait)
{
  View  self      = View::DownCast(Self());
  auto& traitImpl = GetImpl(trait);

  if(id == ReservedTraitId::INTERACTION_TRAIT)
  {
    if(mInteractiveTrait)
    {
      DALI_ASSERT_ALWAYS(false && "Interaction trait cannot be replaced once set");
      return;
    }
    InteractiveTraitInterface* interactiveTrait = dynamic_cast<InteractiveTraitInterface*>(&traitImpl);
    DALI_ASSERT_ALWAYS(interactiveTrait &&
                       "Trait for ReservedTraitId::INTERACTION_TRAIT must implement InteractiveTraitInterface");
    mInteractiveTrait = interactiveTrait;
  }

  for(auto& entry : mTraits)
  {
    if(entry.first == id)
    {
      auto& oldTrait = entry.second;
      if(oldTrait == trait)
      {
        return;
      }
      GetImpl(oldTrait).OnDetached(id, self);
      traitImpl.OnBeforeAttached(id, self);
      entry.second = trait;
      traitImpl.OnAttached(id, self);
      return;
    }
  }

  traitImpl.OnBeforeAttached(id, self);
  mTraits.emplace_back(id, trait);
  traitImpl.OnAttached(id, self);
}

Trait ViewImpl::GetTrait(TraitId id) const
{
  if(!mTraits.empty())
  {
    for(auto& entry : mTraits)
    {
      if(entry.first == id)
      {
        return entry.second;
      }
    }
  }
  return Trait();
}

bool ViewImpl::RemoveTrait(TraitId id)
{
  if(id == ReservedTraitId::INTERACTION_TRAIT)
  {
    DALI_ASSERT_ALWAYS(false && "Interaction trait cannot be removed once set");
    return false;
  }

  for(auto it = mTraits.begin(); it != mTraits.end(); ++it)
  {
    if(it->first == id)
    {
      View self = View::DownCast(Self());
      GetImpl(it->second).OnDetached(id, self);
      mTraits.erase(it);
      return true;
    }
  }
  return false;
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

  if(mLastMeasuredConstraint.width >= 0.0f && FloatEqual(mLastMeasuredConstraint.width, effectiveWidth) &&
     FloatEqual(mLastMeasuredConstraint.height, effectiveHeight))
  {
    return mMeasuredSize;
  }

  MeasuredSize measured          = OnMeasure(effectiveWidth, effectiveHeight);
  measured                       = ApplyConstraints(measured);
  mMeasuredSize                  = measured;
  mLastMeasuredConstraint.width  = effectiveWidth;
  mLastMeasuredConstraint.height = effectiveHeight;

  // Ensure standalone children are measured even when OnMeasure (e.g. in
  // leaf views like Label) does not iterate children. The measure cache
  // prevents redundant work when OnMeasure already measured them.
  MeasureStandaloneChildren(effectiveWidth, effectiveHeight);

  return mMeasuredSize;
}

MeasuredSize ViewImpl::OnMeasure(float widthConstraint, float heightConstraint)
{
  float pw = static_cast<float>(mImpl->mPadding.start + mImpl->mPadding.end);
  float ph = static_cast<float>(mImpl->mPadding.top + mImpl->mPadding.bottom);

  float effectiveWidth  = (mImpl->mRequestedWidth >= 0) ? mImpl->mRequestedWidth : widthConstraint;
  float effectiveHeight = (mImpl->mRequestedHeight >= 0) ? mImpl->mRequestedHeight : heightConstraint;

  float contentWidth  = std::max(0.0f, effectiveWidth - pw);
  float contentHeight = std::max(0.0f, effectiveHeight - ph);

  if(!mChildren.empty())
  {
    float maxRight  = 0.0f;
    float maxBottom = 0.0f;
    for(auto& childData : mChildren)
    {
      ViewImpl& childImpl = Integration::GetImpl(childData.view);

      // Standalone children are measured by MeasureStandaloneChildren()
      // in ViewImpl::Measure() after OnMeasure returns.
      if(childImpl.IsLayoutModeStandalone())
      {
        continue;
      }

      Extents      margin                = childImpl.GetMargin();
      float        marginW               = static_cast<float>(margin.start + margin.end);
      float        marginH               = static_cast<float>(margin.top + margin.bottom);
      float        childWidthConstraint  = std::max(0.0f, contentWidth - marginW);
      float        childHeightConstraint = std::max(0.0f, contentHeight - marginH);
      MeasuredSize childSize             = childImpl.Measure(childWidthConstraint, childHeightConstraint);
      childData.measuredSize             = childSize;

      float childX = childImpl.GetPositionX();
      float childY = childImpl.GetPositionY();
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
  mArrangedBounds           = bounds;
  mArrangeValid             = true;

  // Ensure standalone children are arranged even when OnArrange (e.g. in
  // leaf views like Label) does not iterate children. The arrange-valid
  // flag prevents redundant work when OnArrange already arranged them.
  ArrangeStandaloneChildren(bounds);

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

  if(!mChildren.empty())
  {
    float padLeft   = static_cast<float>(mImpl->mPadding.start);
    float padRight  = static_cast<float>(mImpl->mPadding.end);
    float padTop    = static_cast<float>(mImpl->mPadding.top);
    float padBottom = static_cast<float>(mImpl->mPadding.bottom);

    for(auto& childData : mChildren)
    {
      ViewImpl& childImpl = Integration::GetImpl(childData.view);

      // Standalone children are handled by ArrangeStandaloneChildren()
      // in ViewImpl::Arrange() after OnArrange returns.
      if(childImpl.IsLayoutModeStandalone())
      {
        continue;
      }

      Extents margin  = childImpl.GetMargin();
      float   marginW = static_cast<float>(margin.start + margin.end);
      float   marginH = static_cast<float>(margin.top + margin.bottom);
      float   childW  = childData.measuredSize.width;
      float   childH  = childData.measuredSize.height;

      // MATCH_PARENT: fills parent content area minus own margin.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT)
      {
        childW = std::max(0.0f, width - padLeft - padRight - marginW);
      }
      if(childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childH = std::max(0.0f, height - padTop - padBottom - marginH);
      }
      float childX = padLeft + static_cast<float>(margin.start) + childImpl.mRequestedPositionX;
      float childY = padTop + static_cast<float>(margin.top) + childImpl.mRequestedPositionY;

      // MATCH_PARENT children reported minSize during Measure, but their
      // subtree was measured with the original constraint. Re-measure with
      // the actual final size so internal state (e.g. text layout, nested
      // children) reflects the real available space.
      if(childImpl.GetRequestedWidth() == MATCH_PARENT || childImpl.GetRequestedHeight() == MATCH_PARENT)
      {
        childData.measuredSize = childImpl.Measure(childW, childH);
      }

      LayoutRect childBounds(childX, childY, childW, childH);
      childImpl.Arrange(childBounds);
      childData.arrangedBounds = childBounds;
    }
  }

  return {width, height};
}

void ViewImpl::MeasureStandaloneChildren(float effectiveWidth, float effectiveHeight)
{
  for(auto& childData : mChildren)
  {
    ViewImpl& childImpl = Integration::GetImpl(childData.view);
    if(!childImpl.IsLayoutModeStandalone())
    {
      continue;
    }
    Extents margin                = childImpl.GetMargin();
    float   marginW               = static_cast<float>(margin.start + margin.end);
    float   marginH               = static_cast<float>(margin.top + margin.bottom);
    float   childWidthConstraint  = std::max(0.0f, effectiveWidth - marginW);
    float   childHeightConstraint = std::max(0.0f, effectiveHeight - marginH);
    childData.measuredSize        = childImpl.Measure(childWidthConstraint, childHeightConstraint);
  }
}

void ViewImpl::ArrangeStandaloneChildren(const LayoutRect& bounds)
{
  for(auto& childData : mChildren)
  {
    ViewImpl& childImpl = Integration::GetImpl(childData.view);
    if(!childImpl.IsLayoutModeStandalone())
    {
      continue;
    }
    ArrangeStandaloneChild(childImpl, childData, bounds.width, bounds.height);
  }
}

void ViewImpl::InvalidateMeasure()
{
  mLastMeasuredConstraint.width  = -1.0f;
  mLastMeasuredConstraint.height = -1.0f;
  mArrangeValid                  = false;

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    Integration::GetImpl(parentLayout).InvalidateMeasure();
    return;
  }

  Ui::View parentView = GetParentView();
  if(parentView)
  {
    Integration::GetImpl(parentView).InvalidateMeasure();
    return;
  }

  RegisterWithLayoutController();
}

void ViewImpl::InvalidateArrange()
{
  mArrangeValid = false;

  Ui::Layout parentLayout = GetParentLayout();
  if(parentLayout)
  {
    Integration::GetImpl(parentLayout).InvalidateArrange();
    return;
  }

  // Propagate to parent View (no LayoutManager)
  Ui::View parentView = GetParentView();
  if(parentView)
  {
    Integration::GetImpl(parentView).InvalidateArrange();
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
  return mMeasuredSize;
}

bool ViewImpl::IsMeasureValid() const
{
  return mLastMeasuredConstraint.width >= 0.0f;
}

bool ViewImpl::IsArrangeValid() const
{
  return mArrangeValid;
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

bool ViewImpl::IsLayoutModeStandalone() const
{
  return mImpl->mLayoutMode == Ui::LayoutMode::STANDALONE;
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

bool ViewImpl::IsLayout() const
{
  return false;
}

Internal::LayoutCallbacksImpl* ViewImpl::GetLayoutCallbacks() const
{
  Trait trait = const_cast<ViewImpl*>(this)->GetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS);
  if(trait)
  {
    return static_cast<Internal::LayoutCallbacksImpl*>(&Ui::GetImpl(trait));
  }
  return nullptr;
}

Internal::LayoutCallbacksImpl* ViewImpl::EnsureLayoutCallbacks()
{
  Internal::LayoutCallbacksImpl* callbacks = GetLayoutCallbacks();
  if(!callbacks)
  {
    callbacks                             = new Internal::LayoutCallbacksImpl();
    Internal::LayoutCallbacksTrait handle = Internal::LayoutCallbacksTrait::New(callbacks);
    SetTrait(Integration::ReservedTraitId::LAYOUT_SIGNALS, handle);
  }
  return callbacks;
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
  if(index > mChildren.size())
  {
    index = static_cast<uint32_t>(mChildren.size());
  }
  ChildData childData;
  childData.view           = child;
  childData.measuredSize   = {0.0f, 0.0f};
  childData.arrangedBounds = {0.0f, 0.0f, 0.0f, 0.0f};
  mChildren.insert(mChildren.begin() + index, childData);

  {
    ScopedSkipChildrenUpdate guard(mSkipChildrenUpdate);
    Self().Add(child);
  }

  // Invalidate the child's measure cache — its previous cache was computed
  // under a different parent's constraints and is no longer reliable.
  Integration::GetImpl(child).InvalidateMeasure();
}

void ViewImpl::RemoveAllChildren()
{
  {
    ScopedSkipChildrenUpdate guard(mSkipChildrenUpdate);
    for(auto& childData : mChildren)
    {
      // Invalidate each child's measure cache so that re-parented children
      // are re-measured under the new parent's constraints.
      Integration::GetImpl(childData.view).InvalidateMeasure();
      Self().Remove(childData.view);
    }
  }

  mChildren.clear();
  InvalidateMeasure();
}

uint32_t ViewImpl::GetChildCount() const
{
  return static_cast<uint32_t>(mChildren.size());
}

Ui::View ViewImpl::GetChildAt(uint32_t index) const
{
  if(index < mChildren.size())
  {
    return mChildren[index].view;
  }
  return Ui::View();
}

int32_t ViewImpl::IndexOfChild(Ui::View view) const
{
  if(!view)
  {
    return -1;
  }
  for(size_t i = 0; i < mChildren.size(); ++i)
  {
    if(mChildren[i].view == view)
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
      ScopedSkipChildrenUpdate guard(Integration::GetImpl(parent).mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(Integration::GetImpl(parent).mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(Integration::GetImpl(parent).mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(Integration::GetImpl(parent).mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(Integration::GetImpl(parent).mSkipChildrenUpdate);
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
      ScopedSkipChildrenUpdate guard(Integration::GetImpl(parent).mSkipChildrenUpdate);
      self.LowerBelow(target);
      return;
    }
  }
  self.LowerBelow(target);
}

Integration::ViewImpl& ViewImpl::Contents(std::initializer_list<Ui::View> children)
{
  for(const auto& child : children)
  {
    Self().Add(child);
  }
  return *this;
}

ViewImpl::ChildContainer& ViewImpl::GetChildren()
{
  return mChildren;
}

const ViewImpl::ChildContainer& ViewImpl::GetChildren() const
{
  return mChildren;
}

namespace
{

TraitId ToTraitId(LayoutParamsType type)
{
  switch(type)
  {
    case LayoutParamsType::ABSOLUTE:
      return ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
    case LayoutParamsType::STACK:
      return ReservedTraitId::STACK_LAYOUT_PARAMS;
    case LayoutParamsType::GRID:
      return ReservedTraitId::GRID_LAYOUT_PARAMS;
    case LayoutParamsType::FLEX:
      return ReservedTraitId::FLEX_LAYOUT_PARAMS;
  }
  DALI_ASSERT_ALWAYS(false && "Unknown LayoutParamsType");
  return ReservedTraitId::ABSOLUTE_LAYOUT_PARAMS;
}

} // unnamed namespace

BaseHandle ViewImpl::GetLayoutParamsTrait(LayoutParamsType type) const
{
  return GetTrait(ToTraitId(type));
}

void ViewImpl::SetLayoutParams(Ui::LayoutParams params)
{
  auto& paramsImpl = static_cast<Internal::LayoutParamsImpl&>(Ui::GetImpl(params));
  SetTrait(paramsImpl.GetTraitId(), params);
  InvalidateMeasure();
}

// =============================================================================
// From control-impl.cpp
// =============================================================================

ViewImpl::ViewImpl(ViewBehaviour behaviourFlags)
: CustomActorImpl(static_cast<ActorFlags>(behaviourFlags)),
  mInteractiveTrait(nullptr),
  mRequestedPositionX(0.0f),
  mRequestedPositionY(0.0f),
  mMeasuredSize{0.0f, 0.0f},
  mLastMeasuredConstraint{-1.0f, -1.0f},
  mArrangedBounds{0.0f, 0.0f, 0.0f, 0.0f},
  mArrangeValid(false),
  mImpl(new Internal::ViewDataImpl(*this))
{
  mImpl->mFlags = static_cast<Ui::Integration::ViewImpl::ViewBehaviour>(behaviourFlags);
}

void ViewImpl::Initialize()
{
  if(!(mImpl->mFlags & Ui::Integration::ViewImpl::ViewBehaviour::DISABLE_VISUALS))
  {
    mImpl->InitializeVisualData();
  }

  RegisterViewAccessibleGetter();

  // Call deriving classes so initialised before styling is applied to them.
  OnInitialize();

  if(mImpl->mFlags & Ui::Integration::ViewImpl::ViewBehaviour::REQUIRES_KEYBOARD_NAVIGATION_SUPPORT)
  {
    SetKeyboardNavigationSupport(true);
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

std::shared_ptr<Ui::ViewAccessible> ViewImpl::GetAccessibleObject()
{
  return mImpl->GetAccessibleObject();
}

void ViewImpl::EnableGestureDetection(GestureType::Value type)
{
  if((type & GestureType::PINCH) && !mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector = PinchGestureDetector::New();
    mImpl->mPinchGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::PinchDetected);
    mImpl->mPinchGestureDetector.Attach(Self());
  }

  if((type & GestureType::PAN) && !mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector = PanGestureDetector::New();
    mImpl->mPanGestureDetector.SetMaximumTouchesRequired(2);
    mImpl->mPanGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::PanDetected);
    mImpl->mPanGestureDetector.Attach(Self());
  }

  if((type & GestureType::TAP) && !mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector = TapGestureDetector::New();
    mImpl->mTapGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::TapDetected);
    mImpl->mTapGestureDetector.Attach(Self());
  }

  if((type & GestureType::LONG_PRESS) && !mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector = LongPressGestureDetector::New();
    mImpl->mLongPressGestureDetector.DetectedSignal().Connect(mImpl, &Internal::ViewDataImpl::LongPressDetected);
    mImpl->mLongPressGestureDetector.Attach(Self());
  }
}

void ViewImpl::DisableGestureDetection(GestureType::Value type)
{
  if((type & GestureType::PINCH) && mImpl->mPinchGestureDetector)
  {
    mImpl->mPinchGestureDetector.Detach(Self());
    mImpl->mPinchGestureDetector.Reset();
  }

  if((type & GestureType::PAN) && mImpl->mPanGestureDetector)
  {
    mImpl->mPanGestureDetector.Detach(Self());
    mImpl->mPanGestureDetector.Reset();
  }

  if((type & GestureType::TAP) && mImpl->mTapGestureDetector)
  {
    mImpl->mTapGestureDetector.Detach(Self());
    mImpl->mTapGestureDetector.Reset();
  }

  if((type & GestureType::LONG_PRESS) && mImpl->mLongPressGestureDetector)
  {
    mImpl->mLongPressGestureDetector.Detach(Self());
    mImpl->mLongPressGestureDetector.Reset();
  }
}

PinchGestureDetector ViewImpl::GetPinchGestureDetector() const
{
  return mImpl->mPinchGestureDetector;
}

PanGestureDetector ViewImpl::GetPanGestureDetector() const
{
  return mImpl->mPanGestureDetector;
}

TapGestureDetector ViewImpl::GetTapGestureDetector() const
{
  return mImpl->mTapGestureDetector;
}

LongPressGestureDetector ViewImpl::GetLongPressGestureDetector() const
{
  return mImpl->mLongPressGestureDetector;
}

void ViewImpl::SetKeyboardNavigationSupport(bool isSupported)
{
  Self().SetProperty(Ui::View::Property::KEYBOARD_NAVIGATION_SUPPORT, isSupported);
}

bool ViewImpl::IsKeyboardNavigationSupported()
{
  return mImpl->mIsKeyboardNavigationSupported;
}

void ViewImpl::SetKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Internal::KeyInputFocusManager::Get().SetFocus(Ui::View::DownCast(Self()));
  }
}

bool ViewImpl::HasKeyInputFocus()
{
  bool result = false;
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Ui::View view = Internal::KeyInputFocusManager::Get().GetCurrentFocusView();
    if(Self() == view)
    {
      result = true;
    }
  }
  return result;
}

void ViewImpl::ClearKeyInputFocus()
{
  if(Self().GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Internal::KeyInputFocusManager::Get().RemoveFocus(Ui::View::DownCast(Self()));
  }
}

void ViewImpl::SetAsKeyboardFocusGroup(bool isFocusGroup)
{
  Self().SetProperty(Ui::View::Property::KEYBOARD_FOCUS_GROUP, isFocusGroup);
}

bool ViewImpl::IsKeyboardFocusGroup()
{
  return Ui::FocusManager::Get().IsFocusGroup(Self());
}

void ViewImpl::KeyboardEnter()
{
  // Inform deriving classes
  OnKeyboardEnter();
}

bool ViewImpl::OnAccessibilityActivated()
{
  if(Ui::FocusManager::Get().SetCurrentFocusActor(Self()))
  {
    return OnKeyboardEnter();
  }
  return false;
}

bool ViewImpl::OnKeyboardEnter()
{
  return false; // Keyboard enter is not handled by default
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

Actor ViewImpl::GetNextKeyboardFocusableActor(Actor currentFocusedActor, Ui::FocusDirection direction,
                                              bool loopEnabled)
{
  return Actor();
}

void ViewImpl::OnKeyboardFocusChangeCommitted(Actor committedFocusableActor)
{
}

Ui::View::KeyEventSignalType& ViewImpl::KeyEventSignal()
{
  return mImpl->mKeyEventSignal;
}

Ui::View::FocusChangedSignalType& ViewImpl::FocusChangedSignal()
{
  return mImpl->mFocusChangedSignal;
}

bool ViewImpl::EmitKeyEventSignal(const KeyEvent& event)
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
    auto accessible = GetAccessibleObject();
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
  mImpl->OnSceneDisconnection();
}

void ViewImpl::OnChildAdd(Actor& child)
{
  if(mSkipChildrenUpdate)
  {
    return;
  }

  Ui::View view = Ui::View::DownCast(child);
  if(view)
  {
    ChildData childData;
    childData.view           = view;
    childData.measuredSize   = {0.0f, 0.0f};
    childData.arrangedBounds = {0.0f, 0.0f, 0.0f, 0.0f};
    mChildren.push_back(childData);
    // Invalidate the child's measure cache — its previous cache was computed
    // under a different parent's constraints and is no longer reliable.
    // This also propagates to the parent (this) via InvalidateMeasure chain.
    Integration::GetImpl(view).InvalidateMeasure();
  }
  else
  {
    if(gAllowNonViewChild)
    {
      // Permitted via Integration::AddActorChild: skip the View-only check
      // and do not record this child in mChildren (it is excluded from layout).
      return;
    }
    DALI_ASSERT_ALWAYS(false && "View could only have child as View class!");
  }
}

void ViewImpl::OnChildRemove(Actor& child)
{
  if(mSkipChildrenUpdate)
  {
    return;
  }

  Ui::View view = Ui::View::DownCast(child);
  if(view)
  {
    auto it = std::find_if(mChildren.begin(), mChildren.end(), [&view](const ChildData& data)
    {
      return data.view == view;
    });
    if(it != mChildren.end())
    {
      // Invalidate the removed child's measure cache so that it gets
      // re-measured when re-parented to a different container.
      // Note: Actor parent-child relationship is already severed at this
      // point, so child's InvalidateMeasure cannot propagate to us.
      Integration::GetImpl(view).InvalidateMeasure();
      mChildren.erase(it);
      InvalidateMeasure();
    }
  }
}

void ViewImpl::OnChildOrderChanged(Actor orderChangedChild)
{
  if(mSkipChildrenUpdate)
  {
    return;
  }

  Actor          self            = Self();
  uint32_t       actorChildCount = self.GetChildCount();
  ChildContainer newChildren;
  newChildren.reserve(actorChildCount);

  for(uint32_t i = 0; i < actorChildCount; ++i)
  {
    Ui::View view = Ui::View::DownCast(self.GetChildAt(i));
    if(view)
    {
      auto it = std::find_if(mChildren.begin(), mChildren.end(), [&view](const ChildData& data)
      {
        return data.view == view;
      });
      if(it != mChildren.end())
      {
        newChildren.push_back(std::move(*it));
      }
    }
  }

  mChildren = std::move(newChildren);
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
      if(!enabled && Self() == Dali::Ui::FocusManager::Get().GetCurrentFocusActor())
      {
        Dali::Ui::FocusManager::Get().ClearFocus();
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

void ViewImpl::OnPinch(const PinchGesture& pinch)
{
  if(!(mImpl->mStartingPinchScale))
  {
    // lazy allocate
    mImpl->mStartingPinchScale = new Vector3;
  }

  if(pinch.GetState() == GestureState::STARTED)
  {
    *(mImpl->mStartingPinchScale) = Self().GetCurrentProperty<Vector3>(Actor::Property::SCALE);
  }
  Self().SetProperty(Actor::Property::SCALE, *(mImpl->mStartingPinchScale) * pinch.GetScale());
}

void ViewImpl::OnPan(const PanGesture& pan)
{
}

void ViewImpl::OnTap(const TapGesture& tap)
{
}

void ViewImpl::OnLongPress(const LongPressGesture& longPress)
{
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

void ViewImpl::RelayoutRequestToView()
{
  RelayoutRequest();
}

void ViewImpl::SignalConnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalConnected(slotObserver, callback);
}

void ViewImpl::SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback)
{
  mImpl->SignalDisconnected(slotObserver, callback);
}

// From view.cpp

std::vector<Accessibility::Relation> ViewImpl::GetAccessibilityRelations()
{
  std::vector<Accessibility::Relation> result;

  const auto* accessibilityData = mImpl->GetAccessibilityData();
  if(DALI_LIKELY(accessibilityData))
  {
    const auto& relations = accessibilityData->mAccessibilityProps.relations;
    for(const auto& relation : relations)
    {
      const auto& targets = relation.second;

      result.emplace_back(Accessibility::Relation{relation.first, {}});
      std::copy(targets.begin(), targets.end(), std::back_inserter(result.back().mTargets));
    }
  }

  return result;
}

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

} // namespace Integration
} // namespace Ui
} // namespace Dali
