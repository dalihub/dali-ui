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
#include <dali-ui-foundation/internal/layouts/layout-transition-dispatcher.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>
#include <algorithm>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-integ.h>

#include <dali-ui-foundation/internal/layouts/layout-reflow-resolver.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-validation.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace IntegrationView = Dali::Ui::Integration::View;

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{
constexpr float BOUNDS_EPSILON = 0.5f;

bool BoundsApproxEqual(const LayoutRect& a, const LayoutRect& b)
{
  return Equals(a.x, b.x, BOUNDS_EPSILON) &&
         Equals(a.y, b.y, BOUNDS_EPSILON) &&
         Equals(a.width, b.width, BOUNDS_EPSILON) &&
         Equals(a.height, b.height, BOUNDS_EPSILON);
}

bool ChildStillPresent(ViewImpl* root, ViewImpl* child)
{
  auto& children = IntegrationView::GetChildren(*root);
  for(auto& c : children)
  {
    if(&GetImpl(c) == child)
    {
      return true;
    }
  }
  return false;
}

float ApplyAlpha(const AlphaFunction& alpha, float t)
{
  switch(alpha.GetMode())
  {
    case AlphaFunction::CUSTOM_FUNCTION:
    {
      AlphaFunctionPrototype fn = alpha.GetCustomFunction();
      return fn ? fn(t) : t;
    }
    case AlphaFunction::BUILTIN_FUNCTION:
      break;
    default:
      // BEZIER / SPRING / CUSTOM_SPRING are not evaluated server-side in
      // this phase — fall back to linear so application progress is
      // monotonic rather than incorrect.
      return t;
  }

  switch(alpha.GetBuiltinFunction())
  {
    case AlphaFunction::DEFAULT:
    case AlphaFunction::LINEAR:
      return t;
    case AlphaFunction::REVERSE:
    {
      DALI_ABORT("AlphaFunction::REVERSE is not supported by LayoutTransition");
      return t; // Unreachable; satisfies the compiler.
    }
    case AlphaFunction::EASE_IN_SQUARE:
      return t * t;
    case AlphaFunction::EASE_OUT_SQUARE:
      return 1.0f - (1.0f - t) * (1.0f - t);
    case AlphaFunction::EASE_IN:
      return t * t * t;
    case AlphaFunction::EASE_OUT:
    {
      const float u = 1.0f - t;
      return 1.0f - u * u * u;
    }
    case AlphaFunction::EASE_IN_OUT:
    {
      if(t < 0.5f)
      {
        return 4.0f * t * t * t;
      }
      const float u = 1.0f - t;
      return 1.0f - 4.0f * u * u * u;
    }
    case AlphaFunction::EASE_IN_SINE:
      return 1.0f - std::cos(t * 0.5f * static_cast<float>(M_PI));
    case AlphaFunction::EASE_OUT_SINE:
      return std::sin(t * 0.5f * static_cast<float>(M_PI));
    case AlphaFunction::EASE_IN_OUT_SINE:
      return 0.5f * (1.0f - std::cos(static_cast<float>(M_PI) * t));
    default:
      // BOUNCE / SIN / EASE_OUT_BACK are not yet mapped — linear fallback.
      return t;
  }
}

void EmitLifecycle(LayoutLifecycleCallback* cb, Ui::View view, LayoutTransitionSlot slot)
{
  if(cb && *cb && view)
  {
    cb->Invoke(view, slot);
  }
}

LayoutRect ParentLocalBoundsOf(ViewImpl* parent)
{
  if(!parent)
  {
    return {};
  }
  const LayoutRect arranged = parent->GetArrangedBounds();
  return {0.0f, 0.0f, arranged.width, arranged.height};
}

float ResolveBoundsLength(const LayoutBoundsLength& length,
                          bool                      xAxis,
                          const LayoutRect&         childBase,
                          const LayoutRect&         parentLocalBounds)
{
  switch(length.unit)
  {
    case LayoutBoundsUnit::PIXEL:
      return length.value;
    case LayoutBoundsUnit::SELF_FRACTION:
      return length.value * (xAxis ? childBase.width : childBase.height);
    case LayoutBoundsUnit::PARENT_FRACTION:
      return length.value * (xAxis ? parentLocalBounds.width : parentLocalBounds.height);
  }
  return length.value;
}

LayoutRect ComputeBoundsEndpoint(const LayoutRect&         base,
                                 const LayoutRect&         parentLocalBounds,
                                 const LayoutBoundsEffect& effect)
{
  LayoutRect result = base;

  if(effect.hasSizeFactor)
  {
    const float width  = base.width * effect.sizeFactorX;
    const float height = base.height * effect.sizeFactorY;
    result.x           = base.x + (base.width - width) * effect.anchorX;
    result.y           = base.y + (base.height - height) * effect.anchorY;
    result.width       = width;
    result.height      = height;
  }

  if(effect.hasOffset)
  {
    result.x += ResolveBoundsLength(effect.offset.x, true, base, parentLocalBounds);
    result.y += ResolveBoundsLength(effect.offset.y, false, base, parentLocalBounds);
  }

  return result;
}

void ApplyBoundsEndpointAtBoundsAnimation(Animation&                animation,
                                          Dali::Actor               actor,
                                          const LayoutRect&         target,
                                          const LayoutBoundsEffect& effect)
{
  const float durationSec = effect.timing.duration.InSeconds();

  if(durationSec <= 0.0f)
  {
    // Bounds duration 0: settle target immediately. Visual spec's own
    // duration still drives the lifecycle animation.
    actor.SetProperty(Actor::Property::POSITION_X, target.x);
    actor.SetProperty(Actor::Property::POSITION_Y, target.y);
    actor.SetProperty(Actor::Property::SIZE_WIDTH, target.width);
    actor.SetProperty(Actor::Property::SIZE_HEIGHT, target.height);
    return;
  }

  const float delaySec         = effect.timing.delay.InSeconds();
  const float requiredDuration = delaySec + durationSec;
  if(requiredDuration > animation.GetDuration())
  {
    animation.SetDuration(requiredDuration);
  }

  TimePeriod period(delaySec, durationSec);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION_X), target.x, effect.timing.alpha, period);
  animation.AnimateTo(Property(actor, Actor::Property::POSITION_Y), target.y, effect.timing.alpha, period);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE_WIDTH), target.width, effect.timing.alpha, period);
  animation.AnimateTo(Property(actor, Actor::Property::SIZE_HEIGHT), target.height, effect.timing.alpha, period);
}

void SetActorBoundsImmediate(Dali::Actor actor, const LayoutRect& bounds)
{
  if(!actor)
  {
    return;
  }
  actor.SetProperty(Actor::Property::POSITION_X, bounds.x);
  actor.SetProperty(Actor::Property::POSITION_Y, bounds.y);
  actor.SetProperty(Actor::Property::SIZE_WIDTH, bounds.width);
  actor.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);
}

LayoutTransitionDispatcher::TransientActorState
ApplyTransientStateForBoundsEffect(Dali::Actor               actor,
                                   const LayoutBoundsEffect* effect)
{
  LayoutTransitionDispatcher::TransientActorState state;
  if(!actor || !effect)
  {
    return state;
  }

  const bool shouldClip =
    (effect->clipMode == LayoutBoundsClipMode::AUTO &&
     HasTimedSizeEffect(*effect)) ||
    (effect->clipMode == LayoutBoundsClipMode::CLIP_TO_BOUNDING_BOX &&
     IsTimedEffect(*effect));

  if(!shouldClip)
  {
    return state;
  }

  int current = static_cast<int>(ClippingMode::DISABLED);
  actor.GetProperty(Actor::Property::CLIPPING_MODE).Get(current);

  state.hasClippingMode = true;
  state.clippingMode    = current;

  actor.SetProperty(Actor::Property::CLIPPING_MODE,
                    static_cast<int>(ClippingMode::CLIP_TO_BOUNDING_BOX));

  return state;
}

void RestoreTransientActorState(Dali::Actor                                            actor,
                                const LayoutTransitionDispatcher::TransientActorState& state)
{
  if(actor && state.hasClippingMode)
  {
    actor.SetProperty(Actor::Property::CLIPPING_MODE, state.clippingMode);
  }
}

} // namespace

LayoutTransitionDispatcher::LayoutTransitionDispatcher()
: mLastTickTime(std::chrono::steady_clock::now())
{
}
LayoutTransitionDispatcher::~LayoutTransitionDispatcher() = default;

LayoutTransitionDispatcher::InteractionSnapshot
LayoutTransitionDispatcher::SaveAndDisableGhostInteraction(Dali::Actor actor)
{
  // Snapshot + property writes only. Focus clearing is intentionally NOT
  // performed here: ClearFocus emits FocusChangedSignal synchronously, and
  // an application listener that mutates the view tree (e.g. reparents the
  // ghost child to another parent) would re-enter the dispatcher before the
  // EXIT state has been registered, leaving CancelActiveAnimator /
  // CancelPendingExit with nothing to cancel. Callers must register the
  // EXIT state in mActiveAnimators / mPendingExits FIRST and then invoke
  // ClearGhostFocusIfHeld so the cancel paths can observe and clean up the
  // in-flight transition during re-entry.
  InteractionSnapshot snap{};
  if(!actor)
  {
    return snap;
  }
  snap.sensitive         = actor.GetProperty<bool>(Actor::Property::SENSITIVE);
  snap.keyboardFocusable = actor.GetProperty<bool>(Actor::Property::FOCUSABLE);
  snap.touchFocusable    = actor.GetProperty<bool>(Actor::Property::FOCUS_ON_TOUCH);
  actor.SetProperty(Actor::Property::SENSITIVE, false);
  actor.SetProperty(Actor::Property::FOCUSABLE, false);
  actor.SetProperty(Actor::Property::FOCUS_ON_TOUCH, false);
  return snap;
}

void LayoutTransitionDispatcher::ClearGhostFocusIfHeld(Dali::Actor actor)
{
  // Called AFTER the EXIT state has been registered. ClearFocus emits
  // FocusChangedSignal synchronously, so a listener that reparents the
  // ghost child re-enters NotifyChildReparented → OnChildReparented, which
  // can now find the registered state and cancel it cleanly. The caller
  // must re-check that its registration still exists before emitting
  // OnStart or arming timers.
  Ui::View view = Ui::View::DownCast(actor);
  if(!view)
  {
    return;
  }
  Ui::FocusManager fm = Ui::FocusManager::Get();
  if(fm && fm.GetCurrentFocusView() == view)
  {
    fm.ClearFocus();
  }
}

void LayoutTransitionDispatcher::RestoreGhostInteraction(Dali::Actor actor, const InteractionSnapshot& snap)
{
  if(!actor)
  {
    return;
  }
  actor.SetProperty(Actor::Property::SENSITIVE, snap.sensitive);
  actor.SetProperty(Actor::Property::FOCUSABLE, snap.keyboardFocusable);
  actor.SetProperty(Actor::Property::FOCUS_ON_TOUCH, snap.touchFocusable);
}

LayoutRect LayoutTransitionDispatcher::VisualBoundsOf(ViewImpl* parent, ViewImpl* child) const
{
  LayoutRect bounds = child->GetArrangedBounds();
  // Mirror only when the child participates in the parent's
  // ApplyLayoutDirection pass. Standalone children are explicitly skipped
  // there (see ViewImpl::ApplyLayoutDirection), so mirroring them here
  // would diverge from the actor's actual on-screen position and cause a
  // visible jump on the first transition under an RTL parent.
  if(parent &&
     !IntegrationView::IsLayoutModeStandalone(*child) &&
     parent->GetEffectiveLayoutDirection() == Dali::LayoutDirection::RIGHT_TO_LEFT)
  {
    const LayoutRect parentBounds = parent->GetArrangedBounds();
    bounds.x                      = parentBounds.width - bounds.x - bounds.width;
  }
  return bounds;
}

LayoutRect LayoutTransitionDispatcher::CurrentVisualBoundsForExit(ViewImpl*   parent,
                                                                  ViewImpl*   child,
                                                                  Dali::Actor actor) const
{
  LayoutRect bounds = VisualBoundsOf(parent, child);

  // Existing animator (any slot): framework's lastLerped is the
  // authoritative "intended" bounds at the last tick.
  auto animatorIt = mActiveAnimators.find(child);
  if(animatorIt != mActiveAnimators.end())
  {
    return animatorIt->second.lastLerped;
  }

  // Existing spec animation: sample currently rendered POSITION / SIZE so
  // EXIT's start reflects what is on screen, not the stale captured
  // arranged bounds.
  if(actor && mActiveAnimations.count(child) > 0)
  {
    bounds.x      = actor.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    bounds.y      = actor.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    bounds.width  = actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH);
    bounds.height = actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
  }

  return bounds;
}

void LayoutTransitionDispatcher::SettleChangeWithoutAnimation(ViewImpl*         child,
                                                              const LayoutRect& to)
{
  if(!child)
  {
    return;
  }
  Actor actor = child->Self();
  if(!actor)
  {
    return;
  }
  // Stop any in-flight CHANGE animation / animator on this child so the
  // actor properties we set below are not immediately overwritten by a
  // stale target. A deferred-remove EXIT on this child would also keep
  // driving its own animation, so cancel it too. Lifecycle is silent on
  // cancellation per the documented contract.
  CancelActiveAnimation(child, SpecCancelPolicy::SETTLE_ENTER_TO_FINAL);
  CancelActiveAnimator(child);
  CancelPendingExit(child);
  actor.SetProperty(Actor::Property::POSITION_X, to.x);
  actor.SetProperty(Actor::Property::POSITION_Y, to.y);
  actor.SetProperty(Actor::Property::SIZE_WIDTH, to.width);
  actor.SetProperty(Actor::Property::SIZE_HEIGHT, to.height);
}

void LayoutTransitionDispatcher::CollectTransitionViews(ViewImpl* node, std::vector<ViewImpl*>& out)
{
  if(!node)
  {
    return;
  }
  if(node->GetLayoutTransition())
  {
    out.push_back(node);
  }
  auto& children = IntegrationView::GetChildren(*node);
  for(auto& childView : children)
  {
    CollectTransitionViews(&GetImpl(childView), out);
  }
}

void LayoutTransitionDispatcher::CaptureBeforeLayout(ViewImpl* root)
{
  if(!root)
  {
    return;
  }
  std::vector<ViewImpl*> attached;
  CollectTransitionViews(root, attached);
  for(auto* view : attached)
  {
    CaptureSingleView(view);
  }
}

void LayoutTransitionDispatcher::CaptureGovernedChildren(ViewImpl*                    parent,
                                                         std::vector<CapturedBounds>& out,
                                                         bool                         recurse)
{
  auto& children = IntegrationView::GetChildren(*parent);
  out.reserve(out.size() + children.Count());
  for(auto& childView : children)
  {
    ViewImpl& childImpl = GetImpl(childView);
    out.push_back({&childImpl, parent, VisualBoundsOf(parent, &childImpl), !childImpl.IsInitialLayoutDone()});

    // Under SUBTREE scope, descend into descendants the owner governs: those
    // with no transition of their own (a child with its own transition
    // governs its own subtree, so the scope stops there) and that are not
    // standalone layout roots (those run as a separate layout pass).
    if(recurse &&
       !childImpl.GetLayoutTransition() &&
       !IntegrationView::IsLayoutModeStandalone(childImpl))
    {
      CaptureGovernedChildren(&childImpl, out, true);
    }
  }
}

void LayoutTransitionDispatcher::CaptureSingleView(ViewImpl* view)
{
  std::vector<CapturedBounds> captured;

  Ui::LayoutTransition transition = view->GetLayoutTransition();
  const bool           subtree =
    transition && GetImpl(transition).GetReflowScope() == LayoutReflowScope::SUBTREE;
  CaptureGovernedChildren(view, captured, subtree);

  mCaptured[view] = std::move(captured);

  // Record the view as "captured before first arrange" if it has not yet
  // completed any layout pass. The flag is set inside ViewImpl::Arrange
  // (called below by ProcessLayoutRoot), so checking before that runs
  // gives us a stable signal for StartTransitionsForView to consume.
  if(!view->IsInitialLayoutDone())
  {
    mInitialMountViews.insert(view);
  }
  else
  {
    // Defensive cleanup: a stale marker from a previous pass that was
    // interrupted before StartTransitionsForView consumed it, or address
    // reuse of a destroyed view's slot.
    mInitialMountViews.erase(view);
  }
}

void LayoutTransitionDispatcher::StartTransitionsAfterLayout(ViewImpl* root)
{
  if(!root)
  {
    return;
  }
  std::vector<ViewImpl*> attached;
  CollectTransitionViews(root, attached);
  for(auto* view : attached)
  {
    StartTransitionsForView(view);
  }
  // Per-pass flags are reset by EndLayoutPass after the entire batch
  // finishes. Resetting here would misclassify the second and subsequent
  // roots in a multi-root window.
}

void LayoutTransitionDispatcher::BeginLayoutPass()
{
  ++mLayoutPassDepth;
}

void LayoutTransitionDispatcher::EndLayoutPass()
{
  // Only reset per-pass flags when the outermost pass exits. A manual
  // LayoutController::ProcessLayouts invoked from a lifecycle callback
  // during a window-resize-driven pass would otherwise clear
  // mInWindowResize before the outer pass finishes classifying its
  // remaining roots.
  if(mLayoutPassDepth > 0)
  {
    --mLayoutPassDepth;
  }
  if(mLayoutPassDepth == 0)
  {
    mInWindowResize = false;
  }
}

void LayoutTransitionDispatcher::StartTransitionsForView(ViewImpl* root)
{
  auto capIt = mCaptured.find(root);
  if(capIt == mCaptured.end())
  {
    // No snapshot — nothing to compare against, but still consume any
    // pending sets so they do not leak into later passes.
    root->TakePendingEnterChildren();
    root->TakePendingReorderedChildren();
    root->TakePendingChildRemovalForLayoutTransition();
    mInitialMountViews.erase(root);
    return;
  }

  Ui::LayoutTransition transition = root->GetLayoutTransition();
  if(!transition)
  {
    mCaptured.erase(capIt);
    root->TakePendingEnterChildren();
    root->TakePendingReorderedChildren();
    root->TakePendingChildRemovalForLayoutTransition();
    mInitialMountViews.erase(root);
    return;
  }

  // Snapshot the captured-bounds vector to a local and erase the map entry
  // BEFORE iterating. OnStart lifecycle callbacks fire synchronously from
  // each StartXxxTransition below and are documented as safe to mutate the
  // view tree. Such a mutation can trigger OnSceneDisconnection →
  // LayoutController::UnregisterFromAll → OnViewDestroyed, which itself
  // erases mCaptured entries — including the one we are iterating. Iterating
  // a local snapshot keeps the loop well-defined even under reentrancy.
  std::vector<CapturedBounds> snapshot = std::move(capIt->second);
  mCaptured.erase(capIt);

  // Hold a strong handle to root for the duration of the loop so its impl
  // outlives any OnStart callback that drops every other handle (e.g. by
  // unparenting and not retaining the View handle elsewhere).
  Ui::View rootHandle = Ui::View::DownCast(root->Self());
  if(!rootHandle)
  {
    return;
  }

  // Pin every distinct inherited-descendant parent (SUBTREE scope) alive for
  // the loop. OnStart callbacks below fire synchronously and may mutate the
  // tree; without a strong handle an intermediate container dropped by an
  // earlier callback could be destroyed before a later entry dereferences it
  // via ChildStillPresent / VisualBoundsOf. The owner (root) is already
  // pinned by rootHandle, so only parents other than root need pinning.
  std::vector<Ui::View> pinnedParents;
  for(const CapturedBounds& cap : snapshot)
  {
    if(cap.parent && cap.parent != root)
    {
      Ui::View parentHandle = Ui::View::DownCast(cap.parent->Self());
      if(parentHandle)
      {
        pinnedParents.push_back(parentHandle);
      }
    }
  }

  std::unordered_set<ViewImpl*> enterChildren     = root->TakePendingEnterChildren();
  std::unordered_set<ViewImpl*> reorderedChildren = root->TakePendingReorderedChildren();
  // Capture sibling add/remove markers for CHANGE cause refinement. A
  // sibling add is detected as "this pass introduced at least one ENTER
  // child", a sibling remove via the dispatcher-facing marker set in
  // ViewImpl::Remove / RemoveAllChildren. Per-cause precedence is:
  // REORDERED > SIBLING_ADDED > SIBLING_REMOVED > WINDOW_RESIZED > OTHER.
  const bool hadSiblingAdd    = !enterChildren.empty();
  const bool hadSiblingRemove = root->TakePendingChildRemovalForLayoutTransition();

  // Per-pass window-resize flag (reset by StartTransitionsAfterLayout).
  // CHANGE causes are tagged WINDOW_RESIZED; the per-transition opt-out
  // (SetChangeOnWindowResize(false)) applies only to CHANGE entries whose
  // resolved cause is WINDOW_RESIZED. Sibling add/remove/reorder that
  // coincides with a resize pass keeps its higher-precedence cause and is
  // dispatched as usual. ENTER/EXIT also continue to dispatch normally.
  const bool isWindowResize = mInWindowResize;

  // Suppress ENTER for children present at the parent's initial mount.
  // The decision is captured in @c mInitialMountViews by
  // @c CaptureSingleView before @c Arrange sets @c mInitialLayoutDone, so
  // erasing-and-checking here gives a stable signal regardless of when
  // the flag transitions. Applications can opt back in to initial-mount
  // ENTER per-transition via @c SetEnterOnInitialMount(true).
  const bool wasInitialMount      = (mInitialMountViews.erase(root) > 0);
  const bool optInOnInitialMount  = GetImpl(transition).GetEnterOnInitialMount();
  const bool suppressInitialEnter = wasInitialMount && !optInOnInitialMount;

  // Children freshly added under a no-transition descendant since the last pass
  // are registered as inherited-ENTER candidates by NotifyChildAdded. They are
  // ALSO captured in this snapshot (with their pre-arrange bounds), but they
  // must be dispatched as ENTER by DispatchPendingInheritedEnters below — never
  // as a spurious CHANGE (their captured "from" predates the arrange) and never
  // as an initial-mount seed. Collect them so the inherited branch skips them.
  std::unordered_set<ViewImpl*> pendingInheritedEnterChildren;
  {
    auto pendIt = mPendingInheritedEnters.find(root);
    if(pendIt != mPendingInheritedEnters.end())
    {
      for(const PendingInheritedEnter& rec : pendIt->second)
      {
        Ui::View recChild = rec.child.GetHandle();
        if(recChild)
        {
          pendingInheritedEnterChildren.insert(&GetImpl(recChild));
        }
      }
    }
  }

  for(const CapturedBounds& cap : snapshot)
  {
    ViewImpl* child  = cap.child;
    ViewImpl* parent = cap.parent;
    if(!child || !ChildStillPresent(parent, child))
    {
      continue;
    }

    // SUBTREE-scope inherited descendant (not a direct child of the owner).
    // This per-pass branch handles the descendant's CHANGE and, on the owner's
    // first arrange with SetEnterOnInitialMount(true), seeds its ENTER. Runtime
    // ENTER (a child added later) is dispatched by DispatchPendingInheritedEnters
    // from records registered in NotifyChildAdded, and inherited EXIT is routed
    // at remove time by ViewImpl::Remove. For a direct child @c parent ==
    // @c root and this branch is skipped, so the existing per-child logic runs
    // unchanged.
    if(parent != root)
    {
      // A fresh inherited add (recorded by NotifyChildAdded) is dispatched as
      // ENTER by DispatchPendingInheritedEnters below; skip it here so it is
      // neither mis-dispatched as a CHANGE (its captured "from" predates the
      // arrange) nor double-fired by the initial-mount seed.
      if(pendingInheritedEnterChildren.count(child) > 0)
      {
        continue;
      }
      // Owner's first arrange (initial mount): the actor is already at its
      // final arranged bounds. Starting a CHANGE here would animate the
      // grand-child from its pre-arrange (zero) bounds while the surface is
      // typically still off screen. By default suppress (mirror direct-child
      // initial-mount suppression). With SetEnterOnInitialMount(true), seed an
      // inherited ENTER instead so the opt-in launch animation reaches deep
      // descendants present at the owner's first arrange — event-based ENTER
      // records only later adds, never pre-existing children, so this seed is
      // the only path that fires ENTER for them.
      if(wasInitialMount)
      {
        if(suppressInitialEnter)
        {
          // Suppress the launch ENTER on the owner's first arrange, but still
          // settle a declarative ENTER spec to its final values — same contract
          // as the direct-child path (SetEnterOnInitialMount docs). Without this
          // a grand-child pre-set to a fade-in start (e.g. opacity 0) stays
          // there forever. SettleInitialEnter skips animator mode.
          SettleInitialEnter(child, transition);
          continue;
        }
        if(GetImpl(transition).HasEnterFx() && mPendingExits.count(child) == 0)
        {
          auto       seedExitIt   = mActiveAnimators.find(child);
          const bool seedExitBusy = seedExitIt != mActiveAnimators.end() &&
                                    seedExitIt->second.slot == LayoutTransitionSlot::EXIT;
          if(!seedExitBusy)
          {
            if(GetImpl(transition).HasEnterAnimator())
            {
              StartAnimatorEnter(child, VisualBoundsOf(parent, child), transition);
            }
            else
            {
              StartEnterTransition(child, transition);
            }
          }
        }
        continue;
      }
      if(mPendingExits.count(child) > 0)
      {
        continue;
      }
      auto inheritedExitIt = mActiveAnimators.find(child);
      if(inheritedExitIt != mActiveAnimators.end() &&
         inheritedExitIt->second.slot == LayoutTransitionSlot::EXIT)
      {
        continue;
      }
      const LayoutRect inheritedFrom = cap.bounds;
      const LayoutRect inheritedTo   = VisualBoundsOf(parent, child);
      if(BoundsApproxEqual(inheritedFrom, inheritedTo))
      {
        continue;
      }
      // A freshly-added inherited grand-child (never arranged before this pass)
      // has a degenerate pre-arrange "from"; animating a CHANGE from there would
      // slide/grow it from nothing. This is reachable when the child's
      // inherited-ENTER candidate was dropped before this pass (e.g. the owner's
      // transition was detached then re-attached, emptying the skip-set), so the
      // child is no longer skipped above. Settle to the arranged bounds without
      // animation instead of firing a spurious CHANGE. (The owner's own first
      // arrange is already handled by the wasInitialMount branch above; this
      // also hardens any other drop path — reparent / governance change /
      // EXIT-in-flight — against the same zero-from artifact.)
      if(cap.freshChild)
      {
        // Snap geometry to the arranged bounds (no zero-from animation) AND
        // settle the declarative ENTER spec to its final values: the dropped
        // candidate was an inherited ENTER, and SettleChangeWithoutAnimation
        // writes only POSITION/SIZE, so without SettleInitialEnter a grand-child
        // pre-set to a fade-in start (e.g. opacity 0) would stay invisible
        // forever. SettleInitialEnter skips animator mode (the application owns
        // those property writes) and no-ops when the owner has no ENTER spec.
        SettleChangeWithoutAnimation(child, inheritedTo);
        SettleInitialEnter(child, transition);
        continue;
      }
      // WINDOW_RESIZED is a per-pass global flag and is honoured here
      // (including the resize opt-out). Sibling/reorder causes need
      // per-direct-parent markers and are deferred to a later phase; use
      // OTHER otherwise.
      const LayoutChangeCause inheritedCause =
        isWindowResize ? LayoutChangeCause::WINDOW_RESIZED : LayoutChangeCause::OTHER;
      if(inheritedCause == LayoutChangeCause::WINDOW_RESIZED &&
         !GetImpl(transition).GetChangeOnWindowResize())
      {
        SettleChangeWithoutAnimation(child, inheritedTo);
        continue;
      }
      if(GetImpl(transition).HasChangeAnimator())
      {
        StartAnimatorChange(child, inheritedFrom, inheritedTo, transition, inheritedCause);
      }
      else
      {
        StartChangeTransition(child, inheritedFrom, inheritedTo, transition, inheritedCause);
      }
      continue;
    }

    if(enterChildren.count(child) > 0)
    {
      if(suppressInitialEnter)
      {
        // Settle declarative ENTER specs to their target values so a
        // child that pre-set a fade-in start (e.g. opacity = 0) does not
        // remain at the start value forever. Animator-mode ENTER is
        // skipped without settling; the application owns property writes
        // for that callback. No OnStart / OnFinished is emitted.
        SettleInitialEnter(child, transition);
        continue;
      }
      // Newly added: ENTER slot. Animator mode wins over spec mode when both
      // are configured. Layout has already set bounds to the new position/
      // size; the spec/animator animates other properties (opacity, scale,
      // corner radius, ...) on top of that.
      if(GetImpl(transition).HasEnterAnimator())
      {
        StartAnimatorEnter(child, VisualBoundsOf(root, child), transition);
      }
      else
      {
        StartEnterTransition(child, transition);
      }
      continue;
    }

    // Skip CHANGE for children whose EXIT is in flight. Otherwise the
    // CHANGE branch would cancel the EXIT animator state (see
    // CancelActiveAnimator) and the ghost would never be unparented.
    if(mPendingExits.count(child) > 0)
    {
      continue;
    }
    auto activeIt = mActiveAnimators.find(child);
    if(activeIt != mActiveAnimators.end() && activeIt->second.slot == LayoutTransitionSlot::EXIT)
    {
      continue;
    }

    const LayoutRect from = cap.bounds;
    const LayoutRect to   = VisualBoundsOf(root, child);
    if(BoundsApproxEqual(from, to))
    {
      continue;
    }

    // Same per-child freshness guard as the inherited branch: a fresh direct
    // child (never arranged) reaches the CHANGE branch only when it is neither
    // in enterChildren nor seeded — e.g. added to an already-laid-out owner that
    // had no transition at add time, then SetLayoutTransition (the catch-up seed
    // is gated on !mInitialLayoutDone), or detach -> add -> reattach. Its "from"
    // is a degenerate pre-arrange zero, so animating a CHANGE would slide/grow it
    // from nothing. Owner-keyed wasInitialMount does NOT cover this child (it is
    // keyed on the owner's first arrange, not per-child). Settle geometry and the
    // declarative ENTER spec instead of firing a spurious CHANGE.
    if(cap.freshChild)
    {
      SettleChangeWithoutAnimation(child, to);
      SettleInitialEnter(child, transition);
      continue;
    }

    // Cause precedence (deterministic):
    //   REORDERED > SIBLING_ADDED > SIBLING_REMOVED > WINDOW_RESIZED > OTHER
    // hadSiblingAdd / hadSiblingRemove are per-parent flags; every CHANGE
    // child in this pass shares the marker because the layout shift was
    // driven by a sibling-set mutation.
    LayoutChangeCause cause;
    if(reorderedChildren.count(child) > 0)
    {
      cause = LayoutChangeCause::REORDERED;
    }
    else if(hadSiblingAdd)
    {
      cause = LayoutChangeCause::SIBLING_ADDED;
    }
    else if(hadSiblingRemove)
    {
      cause = LayoutChangeCause::SIBLING_REMOVED;
    }
    else if(isWindowResize)
    {
      cause = LayoutChangeCause::WINDOW_RESIZED;
    }
    else
    {
      cause = LayoutChangeCause::OTHER;
    }

    // Window-resize opt-out applies only when the resolved cause is
    // WINDOW_RESIZED. Sibling add/remove/reorder that happens in the
    // same pass as a resize keeps its higher-precedence cause and is
    // dispatched (or settled inline) as usual. Settle through the shared
    // helper so an in-flight CHANGE/animator/EXIT on this child does not
    // keep driving the actor toward a stale target after the resize.
    if(cause == LayoutChangeCause::WINDOW_RESIZED &&
       !GetImpl(transition).GetChangeOnWindowResize())
    {
      SettleChangeWithoutAnimation(child, to);
      continue;
    }

    if(GetImpl(transition).HasChangeAnimator())
    {
      StartAnimatorChange(child, from, to, transition, cause);
    }
    else
    {
      StartChangeTransition(child, from, to, transition, cause);
    }
  }

  // Inherited (SUBTREE-scope) ENTER: dispatch any candidates registered by
  // NotifyChildAdded for this owner since the last pass (re-validated inside).
  DispatchPendingInheritedEnters(root, suppressInitialEnter);
}

void LayoutTransitionDispatcher::NotifyChildAdded(ViewImpl* directParent, Ui::View child)
{
  if(!directParent || !child)
  {
    return;
  }
  // Walk up from the (no-transition) direct parent to the closest ancestor
  // SUBTREE owner with an ENTER effect. Returns nullptr when a closer
  // transition claims the child, a standalone boundary intervenes, or no such
  // owner exists — in which case there is nothing to record.
  ViewImpl* owner = FindGoverningSubtreeOwner(directParent, ReflowSlot::ENTER);
  if(!owner)
  {
    return;
  }
  Ui::View directParentView = Ui::View::DownCast(directParent->Self());
  if(!directParentView)
  {
    return;
  }
  PendingInheritedEnter record;
  record.directParent = WeakHandle<Ui::View>(directParentView);
  record.child        = WeakHandle<Ui::View>(child);
  mPendingInheritedEnters[owner].push_back(std::move(record));
}

void LayoutTransitionDispatcher::ClearPendingInheritedEnters(ViewImpl* owner)
{
  mPendingInheritedEnters.erase(owner);
}

void LayoutTransitionDispatcher::DispatchPendingInheritedEnters(ViewImpl* owner, bool suppressInitialEnter)
{
  auto it = mPendingInheritedEnters.find(owner);
  if(it == mPendingInheritedEnters.end())
  {
    return;
  }

  // Move the records out and erase the map slot before iterating: the
  // StartEnter* helpers fire OnStart synchronously, which may mutate the tree
  // (and this map) re-entrantly.
  std::vector<PendingInheritedEnter> records = std::move(it->second);
  mPendingInheritedEnters.erase(it);

  // Owner must still govern inherited ENTER (it may have detached / changed
  // scope / cleared the ENTER effect since the add). This runs BEFORE the
  // initial-mount handling because the suppress path still needs the owner's
  // transition to settle declarative specs.
  Ui::LayoutTransition ownerTransition = owner->GetLayoutTransition();
  if(!ownerTransition)
  {
    return; // detached since the add — drop the candidates
  }
  LayoutTransitionImpl& ownerImpl = GetImpl(ownerTransition);
  if(ownerImpl.GetReflowScope() != LayoutReflowScope::SUBTREE || !ownerImpl.HasEnterFx())
  {
    return; // owner no longer governs inherited ENTER — drop the candidates
  }
  const bool ownerHasEnterAnimator = ownerImpl.HasEnterAnimator();

  for(PendingInheritedEnter& record : records)
  {
    Ui::View childView  = record.child.GetHandle();
    Ui::View parentView = record.directParent.GetHandle();
    if(!childView || !parentView)
    {
      continue; // child or its direct parent destroyed since the add
    }
    ViewImpl* childImpl  = &GetImpl(childView);
    ViewImpl* parentImpl = &GetImpl(parentView);

    // Re-validate current parentage and governance: transition replace on the
    // owner is tolerated (owner state was re-checked above), but a reparent, a
    // remove, or an intervening transition added higher up since the add must
    // drop the candidate.
    if(!ChildStillPresent(parentImpl, childImpl))
    {
      continue;
    }
    if(FindGoverningSubtreeOwner(parentImpl, ReflowSlot::ENTER) != owner)
    {
      continue;
    }

    // EXIT precedence: never start OR settle ENTER on a child whose EXIT is in
    // flight (a blind settle here would corrupt the fading ghost's visual state).
    if(mPendingExits.count(childImpl) > 0)
    {
      continue;
    }
    auto activeIt = mActiveAnimators.find(childImpl);
    if(activeIt != mActiveAnimators.end() && activeIt->second.slot == LayoutTransitionSlot::EXIT)
    {
      continue;
    }

    if(suppressInitialEnter)
    {
      // Owner's first arrange: suppress the launch ENTER but still settle a
      // declarative ENTER spec to its final values (same contract as the
      // direct-child path). SettleInitialEnter skips animator mode, so an
      // animator-only owner correctly settles nothing. Re-validation above
      // ensures we never settle onto a since-reparented or removed child.
      SettleInitialEnter(childImpl, ownerTransition);
      continue;
    }

    // Dispatch using the child's REAL direct parent frame (INV-GEOMETRY): the
    // owner supplies only the spec / animator / timing. StartEnterTransition
    // self-derives the parent from the actor, and the animator path is given
    // VisualBoundsOf(directParent, child) — not the owner/root — so RTL
    // mirroring and parent-fraction effects anchor to the intermediate
    // container.
    if(ownerHasEnterAnimator)
    {
      StartAnimatorEnter(childImpl, VisualBoundsOf(parentImpl, childImpl), ownerTransition);
    }
    else
    {
      StartEnterTransition(childImpl, ownerTransition);
    }
  }
}

void LayoutTransitionDispatcher::StartChangeTransition(ViewImpl*             child,
                                                       const LayoutRect&     from,
                                                       const LayoutRect&     to,
                                                       Ui::LayoutTransition& transition,
                                                       LayoutChangeCause     cause)
{
  // CHANGE timing lookup: cause-specific override > default-if-enabled.
  // If neither is set the dispatcher snaps to the new bounds without
  // animation (matches the documented opt-out path).
  Ui::LayoutTransitionTiming spec;
  if(!GetImpl(transition).TryGetChangeTiming(cause, spec))
  {
    SettleChangeWithoutAnimation(child, to);
    return;
  }

  const float durationSec = spec.duration.InSeconds();
  const float delaySec    = spec.delay.InSeconds();

  Actor actor = child->Self();
  if(!actor)
  {
    return;
  }

  if(durationSec <= 0.0f)
  {
    // Treat duration == 0 as "no CHANGE animation, settle to layout
    // result". Routed through the shared helper so the no-timing,
    // duration-zero, and resize-opt-out branches all use the same
    // cancel-and-snap semantics. Lifecycle is intentionally silent here
    // (matches the cancellation contract on LayoutLifecycleCallback).
    SettleChangeWithoutAnimation(child, to);
    return;
  }

  // Decide effectiveFrom before cancelling in-flight state. The captured
  // "from" snapshot pre-dates any in-flight animation's progress; if a
  // CHANGE / ENTER spec animation is in flight, reading the actor's last
  // rendered position via GetCurrentProperty lets the new animation pick
  // up where the old one currently is on screen. If an animator-mode
  // transition is in flight (CHANGE / ENTER / EXIT) the lastLerped value
  // is more authoritative than the actor property, since the application
  // may not have applied the previous tick to POSITION/SIZE.
  LayoutRect effectiveFrom = from;
  auto       animatorIt    = mActiveAnimators.find(child);
  if(animatorIt != mActiveAnimators.end())
  {
    effectiveFrom = animatorIt->second.lastLerped;
  }
  else if(mActiveAnimations.count(child) > 0)
  {
    effectiveFrom.x      = actor.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    effectiveFrom.y      = actor.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    effectiveFrom.width  = actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH);
    effectiveFrom.height = actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
  }

  // Cancel any in-flight animation/animator for this child so the new
  // transition starts cleanly. Note: cancelling an animator-mode in-flight
  // does NOT fire its OnFinished callback (see caveat in LayoutTransition
  // docstring); future enhancements may add a finish-reason argument.
  CancelActiveAnimation(child, SpecCancelPolicy::SETTLE_ENTER_TO_FINAL);
  CancelActiveAnimator(child);
  // If a deferred-remove EXIT is still in flight when CHANGE fires, the
  // child has been re-added (or reparented) — cancel the orphan EXIT so
  // its eventual finish does not unparent the resurrected child.
  CancelPendingExit(child);

  // Revert actor properties to effectiveFrom so the animation starts from
  // the visually current state. Same-thread message order ensures these
  // SetProperty messages are processed before the AnimateTo target is
  // captured by the update thread.
  actor.SetProperty(Actor::Property::POSITION_X, effectiveFrom.x);
  actor.SetProperty(Actor::Property::POSITION_Y, effectiveFrom.y);
  actor.SetProperty(Actor::Property::SIZE_WIDTH, effectiveFrom.width);
  actor.SetProperty(Actor::Property::SIZE_HEIGHT, effectiveFrom.height);

  Animation  anim = Animation::New(durationSec + delaySec);
  TimePeriod period(delaySec, durationSec);
  anim.AnimateTo(Property(actor, Actor::Property::POSITION_X), to.x, spec.alpha, period);
  anim.AnimateTo(Property(actor, Actor::Property::POSITION_Y), to.y, spec.alpha, period);
  anim.AnimateTo(Property(actor, Actor::Property::SIZE_WIDTH), to.width, spec.alpha, period);
  anim.AnimateTo(Property(actor, Actor::Property::SIZE_HEIGHT), to.height, spec.alpha, period);

  anim.FinishedSignal().Connect(this, &LayoutTransitionDispatcher::OnAnimationFinished);
  anim.Play();

  ActiveSpecAnimation entry;
  entry.animation          = anim;
  entry.transition         = transition;
  entry.slot               = LayoutTransitionSlot::CHANGE;
  mActiveAnimations[child] = std::move(entry);

  EmitLifecycle(GetImpl(transition).GetOnStartCallback(),
                Ui::View::DownCast(child->Self()),
                LayoutTransitionSlot::CHANGE);
}

void LayoutTransitionDispatcher::StartEnterTransition(ViewImpl* child, Ui::LayoutTransition& transition)
{
  Ui::ViewAnimationSpec     spec        = GetImpl(transition).GetEnterVisualSpec();
  const bool                hasSpec     = static_cast<bool>(spec);
  const bool                isEffectSet = GetImpl(transition).IsEnterBoundsEffectSet();
  const LayoutBoundsEffect* effectPtr   = isEffectSet ? &GetImpl(transition).GetEnterBoundsEffect() : nullptr;
  // Derive hasEffect from effectPtr so the non-null guard is explicit in the
  // data flow: hasEffect is true only when effectPtr is non-null. Equivalent
  // to HasActiveEnterBoundsEffect() (mEnterBoundsEffectSet && !IsNoop).
  const bool hasEffect = (effectPtr != nullptr) && !IsNoopBoundsEffect(*effectPtr);

  if(!hasSpec && !hasEffect)
  {
    // Neither visual spec nor active bounds effect — leave the view at its
    // layout-applied state. Still cancel any orphan EXIT in case the same
    // child was just re-added.
    CancelPendingExit(child);
    return;
  }

  // Re-validate the spec at apply time so a spec mutated after registration
  // (entries added to the handle) is still rejected.
  if(hasSpec)
  {
    AbortIfSpecHasReverseAlpha(spec);
    AbortIfSpecHasLayoutBoundsProperty(spec);
  }
  // Validate the registered bounds effect even when no-op so REVERSE alpha
  // or other invalid values added by handle mutation are still caught.
  if(isEffectSet)
  {
    AbortIfInvalidBoundsEffect(*effectPtr);
  }

  // Cancel any in-flight animation/animator. Cancelled animators do NOT
  // fire their OnFinished callback (see LayoutTransition docstring caveat).
  CancelActiveAnimation(child);
  CancelActiveAnimator(child);
  // Re-add of a child whose EXIT is still in flight: cancel the orphan
  // EXIT so the resurrected child is not unparented when it finishes.
  CancelPendingExit(child);

  Ui::View childHandle = Ui::View::DownCast(child->Self());
  if(!childHandle)
  {
    return;
  }
  Actor childActor = child->Self();

  // ENTER base = final layout bounds. The dispatcher walks from the bounds
  // effect's endpoint toward this base. Sourcing from the actor's current
  // bounds would leak prior animation state into the new transition.
  ViewImpl*   parentImpl  = nullptr;
  Dali::Actor parentActor = childActor ? childActor.GetParent() : Dali::Actor();
  if(parentActor)
  {
    Ui::View parentView = Ui::View::DownCast(parentActor);
    if(parentView)
    {
      parentImpl = &GetImpl(parentView);
    }
  }
  const LayoutRect toBounds          = VisualBoundsOf(parentImpl, child);
  const LayoutRect parentLocalBounds = ParentLocalBoundsOf(parentImpl);
  const LayoutRect fromBounds        = hasEffect ? ComputeBoundsEndpoint(toBounds, parentLocalBounds, *effectPtr)
                                                 : toBounds;

  // Snap actor to the from-bounds so the animation begins from the effect
  // endpoint rather than the layout-applied bounds.
  if(hasEffect)
  {
    SetActorBoundsImmediate(childActor, fromBounds);
  }

  // Apply transient clipping for timed bounds effects per the slot's clip
  // mode policy. The state is captured and restored on finish/cancel.
  TransientActorState transientState =
    hasEffect ? ApplyTransientStateForBoundsEffect(childActor, effectPtr) : TransientActorState{};

  // ApplyTo captures each property's current value as the AnimateTo start,
  // so applications must pre-set fade-in starts (e.g. opacity = 0) before
  // adding the child.
  Animation anim = Animation::New(0.0f);
  if(hasEffect)
  {
    ApplyBoundsEndpointAtBoundsAnimation(anim, childActor, toBounds, *effectPtr);
  }
  if(hasSpec)
  {
    spec.ApplyTo(anim, childHandle);
  }

  if(anim.GetDuration() <= 0.0f)
  {
    // Composite duration 0 — bounds were settled inline and the visual
    // spec was empty / zero-duration. No animation to play; restore
    // transient state and leave actor at toBounds.
    RestoreTransientActorState(childActor, transientState);
    return;
  }

  anim.FinishedSignal().Connect(this, &LayoutTransitionDispatcher::OnAnimationFinished);
  anim.Play();

  ActiveSpecAnimation entry;
  entry.animation          = anim;
  entry.transition         = transition;
  entry.slot               = LayoutTransitionSlot::ENTER;
  entry.transientState     = transientState;
  mActiveAnimations[child] = std::move(entry);

  EmitLifecycle(GetImpl(transition).GetOnStartCallback(),
                childHandle,
                LayoutTransitionSlot::ENTER);
}

void LayoutTransitionDispatcher::SettleInitialEnter(ViewImpl* child, Ui::LayoutTransition& transition)
{
  if(!child || !transition)
  {
    return;
  }

  // Animator mode owns arbitrary application code. Suppressing initial-mount
  // ENTER must not invoke the animator callback or emit lifecycle hooks;
  // applications that want a launch animation should opt in via
  // @c LayoutTransition::SetEnterOnInitialMount(true) or add the child at
  // runtime after the first visible frame. Cancel any orphan EXIT in case
  // the same child was just re-added.
  CancelPendingExit(child);

  if(GetImpl(transition).HasEnterAnimator())
  {
    return;
  }

  Ui::ViewAnimationSpec spec        = GetImpl(transition).GetEnterVisualSpec();
  Ui::View              childHandle = Ui::View::DownCast(child->Self());
  if(!spec || !childHandle)
  {
    return;
  }

  // BAKE_FINAL semantics fire only when @c Stop transitions the animation
  // out of PLAYING / PAUSED (see dali-core Animation::InternalStateConverter).
  // The animation must therefore be Played before it can be stopped with
  // BAKE_FINAL — calling Stop on a CLEARED animation is a no-op and the
  // spec's target values would never be baked, leaving the actor at any
  // pre-set fade-in start value (e.g. opacity = 0).
  //
  // Order:
  //   Play()                  CLEARED -> PLAYING
  //   SetCurrentProgress(1)   jump to end (valid in PLAYING)
  //   Stop()                  PLAYING -> STOPPING, BAKE_FINAL fires,
  //                           NotifyObjects(USE_FINAL_VALUE) writes the
  //                           target values back into the actor's
  //                           event-side property cache
  // FinishedSignal does NOT fire on this path (we never connected to it),
  // so no lifecycle dispatch leaks for the suppressed initial mount.
  AbortIfSpecHasReverseAlpha(spec);
  AbortIfSpecHasLayoutBoundsProperty(spec);
  Animation anim = Animation::New(0.0f);
  spec.ApplyTo(anim, childHandle);
  anim.SetEndAction(Animation::BAKE_FINAL);
  anim.Play();
  anim.SetCurrentProgress(1.0f);
  anim.Stop();
}

void LayoutTransitionDispatcher::CancelActiveAnimation(ViewImpl* child, SpecCancelPolicy policy)
{
  auto it = mActiveAnimations.find(child);
  if(it == mActiveAnimations.end())
  {
    return;
  }
  if(it->second.animation)
  {
    // ENTER spec animations animate visual properties (opacity, scale,
    // color, ...) toward target values. Some successor paths want ENTER
    // visual properties settled to their final values (ENTER -> CHANGE),
    // while others must preserve the currently visible state (ENTER ->
    // EXIT). Make the caller choose explicitly so interruption semantics
    // stay local to the transition that is about to start.
    //
    // SETTLE_ENTER_TO_FINAL is safe only when the caller subsequently
    // writes the intended POSITION / SIZE start point back to the actor
    // (e.g. CHANGE effectiveFrom/to). EXIT paths use PRESERVE_CURRENT so
    // a child that is removed while entering fades/shrinks from the
    // on-screen state instead of jumping to ENTER's final endpoint first.
    //
    // CHANGE and EXIT slots keep default BAKE so their bounds
    // properties stay at the current lerped value, letting the
    // successor pick up from the on-screen state.
    if(policy == SpecCancelPolicy::SETTLE_ENTER_TO_FINAL &&
       it->second.slot == LayoutTransitionSlot::ENTER)
    {
      it->second.animation.SetEndAction(Animation::BAKE_FINAL);
      it->second.animation.SetCurrentProgress(1.0f);
    }
    it->second.animation.Stop();
  }
  // Restore transient properties (e.g. clipping) we mutated for the bounds
  // effect so the cancellation does not leak state across slot transitions.
  if(child)
  {
    RestoreTransientActorState(child->Self(), it->second.transientState);
  }
  mActiveAnimations.erase(it);
}

void LayoutTransitionDispatcher::CancelPendingExit(ViewImpl* child)
{
  auto it = mPendingExits.find(child);
  if(it == mPendingExits.end())
  {
    return;
  }
  if(it->second.animation)
  {
    it->second.animation.Stop();
  }
  // The ghost is being resurrected (re-add / reparent) — restore the actor
  // interaction state we disabled at EXIT entry.
  RestoreGhostInteraction(it->second.child, it->second.savedInteraction);
  RestoreTransientActorState(it->second.child, it->second.transientState);
  mPendingExits.erase(it);
}

void LayoutTransitionDispatcher::CancelActiveAnimator(ViewImpl* child)
{
  auto it = mActiveAnimators.find(child);
  if(it == mActiveAnimators.end())
  {
    return;
  }
  // EXIT animator: restore the actor interaction state we disabled at
  // EXIT entry. ENTER / CHANGE animators do not touch interaction.
  if(it->second.slot == LayoutTransitionSlot::EXIT)
  {
    RestoreGhostInteraction(it->second.childRef, it->second.savedInteraction);
  }
  mActiveAnimators.erase(it);
}

void LayoutTransitionDispatcher::StartAnimatorChange(ViewImpl* child, const LayoutRect& from, const LayoutRect& to, Ui::LayoutTransition& transition, LayoutChangeCause cause)
{
  // Acquire the actor BEFORE cancellation so the spec-mode continuity path
  // can sample its currently rendered properties via GetCurrentProperty.
  Actor actor = child->Self();
  if(!actor)
  {
    return;
  }

  // Decide effectiveFrom before cancelling in-flight state. Mirrors the
  // continuity logic in StartChangeTransition: the captured "from" snapshot
  // (cap.bounds == GetArrangedBounds, the layout system's tracked value)
  // pre-dates any in-flight animation's progress and can differ from the
  // actor's current visual POSITION/SIZE. Without this, the SetProperty
  // calls below would snap the actor back to cap.bounds, producing a
  // visible jump when an animator CHANGE supersedes an in-flight spec
  // CHANGE / ENTER (e.g. after SetLayoutTransition swap while a CHANGE is
  // mid-animation).
  LayoutRect effectiveFrom = from;
  auto       existingIt    = mActiveAnimators.find(child);
  if(existingIt != mActiveAnimators.end())
  {
    // Existing animator: framework's lastLerped is more authoritative than
    // GetCurrentProperty because the user callback may not have applied
    // the previous tick to POSITION/SIZE.
    effectiveFrom = existingIt->second.lastLerped;
  }
  else if(mActiveAnimations.count(child) > 0)
  {
    // Existing spec animation: dali-core's Animation drives POSITION/SIZE
    // directly, so GetCurrentProperty reflects the actor's current visual
    // state. Sample before CancelActiveAnimation runs Stop (default BAKE
    // end-action keeps the property at its current value), so the
    // subsequent SetProperty is a visual no-op and the animator picks up
    // where the spec animation left off on screen.
    effectiveFrom.x      = actor.GetCurrentProperty<float>(Actor::Property::POSITION_X);
    effectiveFrom.y      = actor.GetCurrentProperty<float>(Actor::Property::POSITION_Y);
    effectiveFrom.width  = actor.GetCurrentProperty<float>(Actor::Property::SIZE_WIDTH);
    effectiveFrom.height = actor.GetCurrentProperty<float>(Actor::Property::SIZE_HEIGHT);
  }

  CancelActiveAnimation(child, SpecCancelPolicy::SETTLE_ENTER_TO_FINAL);
  CancelActiveAnimator(child);
  // Re-add of a child whose EXIT spec is still in flight: cancel the orphan
  // EXIT so the resurrected child is not unparented when it finishes.
  CancelPendingExit(child);

  // Revert actor properties to effectiveFrom; the application callback will
  // overwrite per frame from this point.
  actor.SetProperty(Actor::Property::POSITION_X, effectiveFrom.x);
  actor.SetProperty(Actor::Property::POSITION_Y, effectiveFrom.y);
  actor.SetProperty(Actor::Property::SIZE_WIDTH, effectiveFrom.width);
  actor.SetProperty(Actor::Property::SIZE_HEIGHT, effectiveFrom.height);

  AnimatorState state;
  state.slot              = LayoutTransitionSlot::CHANGE;
  state.cause             = cause;
  state.timing            = GetImpl(transition).GetChangeAnimatorTiming();
  state.fromBounds        = effectiveFrom;
  state.toBounds          = to;
  state.lastLerped        = effectiveFrom;
  state.elapsed           = 0.0f;
  state.transition        = transition;
  state.finished          = false;
  state.freshlyCreated    = true;
  mActiveAnimators[child] = std::move(state);
  EnsureAnimatorTicking();
  EmitLifecycle(GetImpl(transition).GetOnStartCallback(),
                Ui::View::DownCast(child->Self()),
                mActiveAnimators[child].slot);
}

void LayoutTransitionDispatcher::StartAnimatorEnter(ViewImpl* child, const LayoutRect& bounds, Ui::LayoutTransition& transition)
{
  CancelActiveAnimation(child);
  CancelActiveAnimator(child);
  // Re-add of a child whose EXIT spec is still in flight.
  CancelPendingExit(child);

  Actor actor = child->Self();
  if(!actor)
  {
    return;
  }

  AnimatorState state;
  state.slot = LayoutTransitionSlot::ENTER;
  // ENTER slot's cause is implicit (child added); the changeCause field is
  // populated only for the CHANGE slot's animator callback. Choose OTHER as
  // an explicit placeholder for ENTER/EXIT (documented contract).
  state.cause             = LayoutChangeCause::OTHER;
  state.timing            = GetImpl(transition).GetEnterAnimatorTiming();
  state.fromBounds        = bounds;
  state.toBounds          = bounds;
  state.lastLerped        = bounds;
  state.elapsed           = 0.0f;
  state.transition        = transition;
  state.finished          = false;
  state.freshlyCreated    = true;
  mActiveAnimators[child] = std::move(state);
  EnsureAnimatorTicking();
  EmitLifecycle(GetImpl(transition).GetOnStartCallback(),
                Ui::View::DownCast(child->Self()),
                mActiveAnimators[child].slot);
}

void LayoutTransitionDispatcher::StartAnimatorExit(ViewImpl*             parent,
                                                   Ui::View              child,
                                                   Ui::LayoutTransition& transition)
{
  if(!parent || !child)
  {
    return;
  }
  ViewImpl* childImpl = &GetImpl(child);

  // Acquire the actor and decide the EXIT bounds BEFORE cancellation so
  // visual continuity is preserved when EXIT interrupts an in-flight
  // CHANGE / ENTER animation on the same child. Mirrors the continuity
  // logic in StartAnimatorChange / StartChangeTransition.
  //
  // Without this, the actor's visual position at the moment of removal
  // (mid-CHANGE) would not match `ctx.fromBounds` / `ctx.toBounds` (both
  // set to the arranged layout bounds), so an EXIT animator that drives
  // POSITION/SIZE from those values would jump on the first tick.
  Ui::View parentHandle = Ui::View::DownCast(parent->Self());
  Actor    actor        = child;
  if(!actor)
  {
    return;
  }

  // Sample current on-screen bounds so the EXIT animator picks up from
  // the lerped position when an in-flight CHANGE/ENTER is interrupted,
  // matching the spec/bounds-effect EXIT path.
  LayoutRect bounds = CurrentVisualBoundsForExit(parent, childImpl, actor);

  CancelActiveAnimation(childImpl, SpecCancelPolicy::PRESERVE_CURRENT);
  CancelActiveAnimator(childImpl);
  SetActorBoundsImmediate(actor, bounds);

  AnimatorState state;
  state.slot = LayoutTransitionSlot::EXIT;
  // EXIT slot's cause is implicit (child removed); see ENTER comment above.
  state.cause                 = LayoutChangeCause::OTHER;
  state.timing                = GetImpl(transition).GetExitAnimatorTiming();
  state.fromBounds            = bounds;
  state.toBounds              = bounds;
  state.lastLerped            = bounds;
  state.elapsed               = 0.0f;
  state.transition            = transition;
  state.childRef              = child;
  state.parentRef             = WeakHandle<Ui::View>(parentHandle);
  state.finished              = false;
  state.freshlyCreated        = true;
  state.savedInteraction      = SaveAndDisableGhostInteraction(child);
  mActiveAnimators[childImpl] = std::move(state);

  // Clear focus AFTER registration so the synchronous FocusChangedSignal
  // re-entry (reparent of the ghost child to a new parent) reaches
  // OnChildReparented → CancelActiveAnimator and finds the just-inserted
  // entry to remove. Without this ordering, a reparent during ClearFocus
  // would leave a stale EXIT animator driving the child under its new
  // parent and emit a spurious OnStart.
  ClearGhostFocusIfHeld(child);

  // Re-check registration: if FocusChangedSignal listener reparented the
  // child, OnChildReparented has already cancelled the entry. Skip OnStart
  // and ticking arm in that case so the lifecycle is silent on cancellation.
  if(mActiveAnimators.count(childImpl) > 0)
  {
    EnsureAnimatorTicking();
    EmitLifecycle(GetImpl(transition).GetOnStartCallback(),
                  child,
                  LayoutTransitionSlot::EXIT);
  }
}

namespace
{
constexpr float Lerp1D(float a, float b, float t)
{
  return a + (b - a) * t;
}
} // namespace

void LayoutTransitionDispatcher::DispatchOneTick(ViewImpl* child)
{
  auto it = mActiveAnimators.find(child);
  if(it == mActiveAnimators.end())
  {
    return;
  }
  AnimatorState& state = it->second;

  const float delaySec    = state.timing.delay.InSeconds();
  const float durationSec = state.timing.duration.InSeconds();

  float effectiveElapsed = state.elapsed - delaySec;
  if(effectiveElapsed < 0.0f)
  {
    effectiveElapsed = 0.0f;
  }
  float rawProgress = (durationSec > 0.0f) ? std::min(1.0f, effectiveElapsed / durationSec) : 1.0f;

  // Apply the timing's alpha function to the linear progress. Builtin
  // ease curves and custom function pointers are evaluated here; bezier
  // and spring modes fall back to linear (see ApplyAlpha).
  const float progress = ApplyAlpha(state.timing.alpha, rawProgress);

  state.lastLerped.x      = Lerp1D(state.fromBounds.x, state.toBounds.x, progress);
  state.lastLerped.y      = Lerp1D(state.fromBounds.y, state.toBounds.y, progress);
  state.lastLerped.width  = Lerp1D(state.fromBounds.width, state.toBounds.width, progress);
  state.lastLerped.height = Lerp1D(state.fromBounds.height, state.toBounds.height, progress);

  LayoutAnimatorContext ctx;
  ctx.view        = Ui::View::DownCast(child->Self());
  ctx.slot        = state.slot;
  ctx.changeCause = state.cause;
  ctx.progress    = progress;
  ctx.rawProgress = rawProgress;
  ctx.fromBounds  = state.fromBounds;
  ctx.toBounds    = state.toBounds;

  Internal::LayoutTransitionImpl& impl = GetImpl(state.transition);
  LayoutAnimatorCallback*         cb   = nullptr;
  switch(state.slot)
  {
    case LayoutTransitionSlot::ENTER:
      cb = impl.GetEnterAnimatorCallback();
      break;
    case LayoutTransitionSlot::EXIT:
      cb = impl.GetExitAnimatorCallback();
      break;
    case LayoutTransitionSlot::CHANGE:
      cb = impl.GetChangeAnimatorCallback();
      break;
  }
  if(cb && *cb)
  {
    // The user callback can mutate the view tree (docs forbid this for
    // animator callbacks, but applications may still do it). Once we
    // return from cb the @c state reference may dangle if a self-
    // targeted Remove erased the entry, so this function does not
    // touch state again. The end-of-animation marker is set in
    // TickAnimators step 2 instead, which iterates @c mActiveAnimators
    // fresh and skips entries that were just inserted by a misuse
    // callback (freshlyCreated == true).
    cb->Invoke(ctx);
  }
}

void LayoutTransitionDispatcher::FinalizeAnimator(ViewImpl* child)
{
  auto it = mActiveAnimators.find(child);
  if(it == mActiveAnimators.end())
  {
    return;
  }

  Ui::LayoutTransition transition = it->second.transition;
  LayoutTransitionSlot slot       = it->second.slot;
  Ui::View             viewArg;
  Ui::View             parentHandle;
  Ui::View             childHandle;
  InteractionSnapshot  savedInteraction{};

  // EXIT: capture refs and saved interaction before erase so they outlive
  // the entry; the erase must happen before parent.Remove because the
  // synchronous OnSceneDisconnection path re-enters OnViewDestroyed →
  // mActiveAnimators.erase(child), which would invalidate the iterator
  // we hold.
  if(slot == LayoutTransitionSlot::EXIT)
  {
    parentHandle     = it->second.parentRef.GetHandle();
    childHandle      = it->second.childRef;
    viewArg          = childHandle;
    savedInteraction = it->second.savedInteraction;
  }
  else
  {
    viewArg = Ui::View::DownCast(child->Self());
  }

  mActiveAnimators.erase(it);

  if(slot == LayoutTransitionSlot::EXIT)
  {
    // Restore the interaction state we disabled at EXIT entry BEFORE
    // unparenting. parentHandle.Remove fires synchronous signals
    // (DevelActor::ChildRemovedSignal) whose handlers may observe or
    // mutate SENSITIVE / FOCUSABLE / FOCUS_ON_TOUCH — those
    // must reflect the application's original values, not the disabled-
    // during-EXIT values, by the time any handler runs. Doing the
    // restore first also means handlers that explicitly set those
    // properties on the unparented child are not overwritten by a
    // late restore call.
    RestoreGhostInteraction(childHandle, savedInteraction);

    if(parentHandle && childHandle && childHandle.GetParent() == parentHandle)
    {
      parentHandle.Remove(childHandle);
    }
  }

  if(transition)
  {
    EmitLifecycle(GetImpl(transition).GetOnFinishedCallback(), viewArg, slot);
  }
}

void LayoutTransitionDispatcher::NotifyWindowResize()
{
  mInWindowResize = true;
}

void LayoutTransitionDispatcher::TickAnimators()
{
  // Compute deltaSec from the dispatcher's wall clock so the per-frame
  // ProcessEvents path and the periodic mTickTimer share a single time
  // source. The first tick after the active set drained reads no real
  // elapsed time (mLastTickTime is reset on the empty→non-empty edge).
  const auto now = std::chrono::steady_clock::now();
  float      deltaSec =
    std::chrono::duration<float>(now - mLastTickTime).count();
  mLastTickTime = now;

  // Cap deltaSec so an idle interval (during which neither
  // ProcessEvents nor the timer ran and mLastTickTime grew stale)
  // cannot snap an in-flight animator's progress to 1.0 in a single
  // tick. The cap is larger than a normal vsync delta (16ms @ 60Hz,
  // 8ms @ 120Hz) so steady-state ticks are unaffected; only post-idle
  // catch-up frames are clipped.
  constexpr float MAX_TICK_DELTA  = 0.1f; // 100 ms ≈ 6 frames @ 60Hz
  const float     clampedDeltaSec = std::min(deltaSec, MAX_TICK_DELTA);

  // Step 1: advance elapsed and dispatch a tick to every active animator.
  // Build a list of pointers because callbacks must not mutate the map.
  // Animators created within this turn have their freshlyCreated flag set;
  // we skip the elapsed bump for them on the first tick so the value the
  // application reads on its very first frame is exactly progress=0. The
  // cap above guards already-running animators against the same idle jump.
  std::vector<ViewImpl*> toDispatch;
  toDispatch.reserve(mActiveAnimators.size());
  for(auto& entry : mActiveAnimators)
  {
    if(entry.second.freshlyCreated)
    {
      entry.second.freshlyCreated = false;
    }
    else
    {
      entry.second.elapsed += clampedDeltaSec;
    }
    toDispatch.push_back(entry.first);
  }
  for(ViewImpl* child : toDispatch)
  {
    DispatchOneTick(child);
  }

  // Step 2: re-evaluate the @c finished flag against the current state of
  // each animator. Computing this here (rather than inside DispatchOneTick)
  // makes the framework safe against animator callbacks that violate the
  // "no view-tree mutation" contract: such a callback might erase the
  // entry we were dispatching and insert a new one (e.g. cancel CHANGE
  // and start EXIT via Remove on self). A new entry has
  // freshlyCreated == true and its own elapsed=0; skipping it here
  // ensures the new transition plays from frame 0 rather than being
  // immediately marked finished by a stale @c rawProgress carried over
  // from the cancelled animator.
  std::vector<ViewImpl*> toFinalize;
  for(auto& entry : mActiveAnimators)
  {
    if(entry.second.freshlyCreated)
    {
      // A state inserted during this turn (either by StartAnimator* in
      // a layout pass, or by a misuse callback) has not had its first
      // dispatch yet. Defer end-of-animation evaluation to the next tick.
      continue;
    }
    const float delaySec    = entry.second.timing.delay.InSeconds();
    const float durationSec = entry.second.timing.duration.InSeconds();
    const float effElapsed  = std::max(0.0f, entry.second.elapsed - delaySec);
    const bool  atEnd       = (durationSec > 0.0f) ? (effElapsed >= durationSec) : true;
    if(atEnd)
    {
      entry.second.finished = true;
      toFinalize.push_back(entry.first);
    }
  }
  for(ViewImpl* child : toFinalize)
  {
    FinalizeAnimator(child);
  }

  // Tick-driver lifecycle. We only touch @c mTickTimer here while the
  // dispatcher is alive (we are inside it, owned by an alive
  // LayoutControllerImpl). Doing the membership decision here — rather
  // than in OnTickTimer() after FlushUpdateMessages() — avoids any
  // post-flush member access on a possibly destroyed @c this.
  //
  //  - Active set non-empty: ensure the timer is armed (covers
  //    first-time arm and any path that bypasses StartAnimator*'s own
  //    EnsureAnimatorTicking).
  //  - Active set empty: drop the timer handle. The TimerEcore impl
  //    is held by a local guard in TimerEcore::Tick() until that call
  //    unwinds, so resetting our member here is safe; the underlying
  //    ecore_timer is removed by ~TimerEcore when the guard drops, and
  //    OnTickTimer's literal @c true return is ignored by the timer
  //    backend once the source has been deleted from inside its own
  //    callback.
  if(!mActiveAnimators.empty())
  {
    EnsureAnimatorTicking();
  }
  else if(mTickTimer)
  {
    mTickTimer.Reset();
  }
}

void LayoutTransitionDispatcher::EnsureAnimatorTicking()
{
  if(!mTickTimer)
  {
    // First-time creation: build the timer and connect the slot once.
    mTickTimer = Dali::Timer::New(16u);
    mTickTimer.TickSignal().Connect(this, &LayoutTransitionDispatcher::OnTickTimer);
  }
  if(!mTickTimer.IsRunning())
  {
    // Reset the wall clock at the empty→non-empty edge so the very first
    // tick does not see a stale delta from a long idle interval. The
    // restart path (timer was previously stopped because the active set
    // drained) shares the same logic — both reset elapsed measurement.
    mLastTickTime = std::chrono::steady_clock::now();
    mTickTimer.Start();
  }
}

bool LayoutTransitionDispatcher::OnTickTimer()
{
  // Synchronously run all registered Processors and flush queued update
  // messages. This routes through Core::RelayoutAndFlush, which calls
  // RunProcessors (transitively LayoutControllerImpl::Process →
  // TickAnimators(), advancing the animator and invoking the user
  // callback) and then FlushQueue (pushing the SetProperty messages
  // from the user callback to the update thread). Calling TickAnimators
  // directly here would advance state in-process but leave SetProperty
  // messages queued on the event thread, where they would only reach
  // the update thread when an unrelated user event next triggered
  // ProcessCoreEvents — visually freezing the sample between inputs.
  //
  // FlushUpdateMessages may destroy `this` mid-call: a window-invalid
  // LayoutControllerImpl::Process self-erases from gLayoutControllers,
  // which destroys this dispatcher (a controller member). To stay
  // safe we touch no members after the flush. The timer's lifecycle
  // is managed inside TickAnimators (which runs while the dispatcher
  // is alive) — when the active set drains, TickAnimators resets the
  // timer handle, and the underlying ecore_timer is removed by
  // ~TimerEcore when this Tick's local guard unwinds. So returning a
  // literal `true` here is correct in every case: it is ignored by the
  // backend when the timer source has been deleted from within its own
  // callback, and otherwise keeps the timer alive for the next active
  // animator.
  if(Dali::Adaptor::IsAvailable())
  {
    Dali::Adaptor::Get().FlushUpdateMessages();
  }
  return true;
}

bool LayoutTransitionDispatcher::HasActiveAnimators() const
{
  return !mActiveAnimators.empty();
}

void LayoutTransitionDispatcher::ScheduleExit(ViewImpl* parent, Ui::View child, ViewImpl* transitionOwner)
{
  if(!parent || !child)
  {
    return;
  }

  // @p parent is the child's direct (visual) parent — bounds frame, ghost
  // host, and unparent target. The EXIT effect is sourced from @c owner,
  // which is an ancestor for SUBTREE-scope inherited EXIT and equals @p parent
  // for a direct EXIT (transitionOwner == nullptr), so existing 2-arg call
  // sites behave identically.
  ViewImpl* owner = transitionOwner ? transitionOwner : parent;

  ViewImpl* childImpl = &GetImpl(child);

  // If the same child is already exiting (spec or animator mode), ignore
  // the second request and leave the in-flight EXIT animation alone.
  if(mPendingExits.count(childImpl) > 0)
  {
    return;
  }
  {
    auto activeIt = mActiveAnimators.find(childImpl);
    if(activeIt != mActiveAnimators.end() && activeIt->second.slot == LayoutTransitionSlot::EXIT)
    {
      return;
    }
  }

  Ui::LayoutTransition transition   = owner->GetLayoutTransition();
  Ui::View             parentHandle = Ui::View::DownCast(parent->Self());

  // Fallback: no transition or no EXIT spec → unparent immediately. Also
  // cancel any in-flight CHANGE / ENTER animation/animator so it does not
  // keep firing after the actor is unparented.
  if(!transition || !parentHandle)
  {
    CancelActiveAnimation(childImpl);
    CancelActiveAnimator(childImpl);
    if(parentHandle)
    {
      parentHandle.Remove(child);
    }
    return;
  }

  // Animator mode wins over spec mode for EXIT.
  if(GetImpl(transition).HasExitAnimator())
  {
    StartAnimatorExit(parent, child, transition);
    return;
  }

  Ui::ViewAnimationSpec     exitSpec    = GetImpl(transition).GetExitVisualSpec();
  const bool                hasSpec     = static_cast<bool>(exitSpec);
  const bool                isEffectSet = GetImpl(transition).IsExitBoundsEffectSet();
  const LayoutBoundsEffect* effectPtr   = isEffectSet ? &GetImpl(transition).GetExitBoundsEffect() : nullptr;
  // Derive hasEffect from effectPtr so the non-null guard is explicit in the
  // data flow: hasEffect is true only when effectPtr is non-null. Equivalent
  // to HasActiveExitBoundsEffect() (mExitBoundsEffectSet && !IsNoop).
  const bool hasEffect = (effectPtr != nullptr) && !IsNoopBoundsEffect(*effectPtr);

  if(!hasSpec && !hasEffect)
  {
    CancelActiveAnimation(childImpl);
    CancelActiveAnimator(childImpl);
    parentHandle.Remove(child);
    return;
  }

  // Re-validate at apply time so a spec / effect mutated after registration
  // is still rejected.
  if(hasSpec)
  {
    AbortIfSpecHasReverseAlpha(exitSpec);
    AbortIfSpecHasLayoutBoundsProperty(exitSpec);
  }
  if(isEffectSet)
  {
    AbortIfInvalidBoundsEffect(*effectPtr);
  }

  // EXIT base = current on-screen bounds. CurrentVisualBoundsForExit
  // mirrors the animator-EXIT continuity logic: lastLerped from an
  // in-flight animator, GetCurrentProperty from an in-flight spec
  // animation, otherwise the arranged bounds. With this the bounds
  // effect (when set) starts from the actor's last visible rectangle
  // and SetActorBoundsImmediate below does not introduce a jump. For
  // visual-spec-only EXIT (no bounds effect), the dispatcher still pins
  // currentBounds back after cancellation so ENTER -> EXIT does not jump
  // to ENTER's final bounds before fading out.
  Actor            childActor        = child;
  const LayoutRect currentBounds     = CurrentVisualBoundsForExit(parent, childImpl, childActor);
  const LayoutRect parentLocalBounds = ParentLocalBoundsOf(parent);
  const LayoutRect toBounds          = hasEffect ? ComputeBoundsEndpoint(currentBounds, parentLocalBounds, *effectPtr)
                                                 : currentBounds;

  // Cancel any in-flight CHANGE / ENTER on this child BEFORE we capture
  // the new EXIT's transient state. The cancel restores the previous
  // transition's transient state (e.g. CLIPPING_MODE) to the actor's
  // truly-original value; without that restoration first, the new
  // ApplyTransientStateForBoundsEffect below would save the previous
  // transition's mutated value as the "original" and leak it back onto
  // the actor when the EXIT finishes or is cancelled.
  CancelActiveAnimation(childImpl, SpecCancelPolicy::PRESERVE_CURRENT);
  CancelActiveAnimator(childImpl);

  // Pin the actor to the sampled visual bounds so the bounds animation
  // begins from a known state. Do this even for visual-spec-only EXIT:
  // cancelling an interrupted ENTER must not leave the actor at ENTER's
  // final bounds before the fade-out starts.
  if(childActor)
  {
    SetActorBoundsImmediate(childActor, currentBounds);
  }

  TransientActorState transientState =
    hasEffect ? ApplyTransientStateForBoundsEffect(childActor, effectPtr) : TransientActorState{};

  Animation anim = Animation::New(0.0f);
  if(hasEffect && childActor)
  {
    ApplyBoundsEndpointAtBoundsAnimation(anim, childActor, toBounds, *effectPtr);
  }
  if(hasSpec)
  {
    exitSpec.ApplyTo(anim, child);
  }

  if(anim.GetDuration() <= 0.0f)
  {
    // Composite duration 0 — bounds were settled inline and the visual
    // spec was empty / zero-duration. In-flight CHANGE / ENTER was
    // already cancelled above; restore the EXIT's own transient state
    // (captured against the truly-original value) and unparent.
    RestoreTransientActorState(childActor, transientState);
    parentHandle.Remove(child);
    return;
  }

  anim.FinishedSignal().Connect(this, &LayoutTransitionDispatcher::OnAnimationFinished);
  anim.Play();

  GhostExit ghost;
  ghost.parent     = WeakHandle<Ui::View>(parentHandle);
  ghost.child      = child;
  ghost.animation  = anim;
  ghost.transition = transition;
  // Disable input/focus on the ghost so a fading-out view cannot be tapped
  // or focused. The original values are restored if the EXIT is cancelled
  // (re-add of the same child to a different parent, reparent, or
  // destruction / scene disconnection); on a normal finish the actor is
  // unparented and restoration is unnecessary. Replacing the
  // LayoutTransition handle is NOT a cancellation — see header.
  ghost.savedInteraction   = SaveAndDisableGhostInteraction(child);
  ghost.transientState     = transientState;
  mPendingExits[childImpl] = std::move(ghost);

  // Clear focus AFTER registration. See StartAnimatorExit for rationale:
  // the synchronous FocusChangedSignal re-entry can reparent the ghost
  // child, which routes through OnChildReparented → CancelPendingExit.
  // CancelPendingExit calls animation.Stop() and erases the entry, so the
  // EXIT spec animation does not continue under the new parent.
  ClearGhostFocusIfHeld(child);

  // Re-check registration: if the focus listener reparented the child,
  // CancelPendingExit has already removed the entry and stopped the
  // animation. Skip OnStart so lifecycle stays silent on cancellation.
  if(mPendingExits.count(childImpl) > 0)
  {
    EmitLifecycle(GetImpl(transition).GetOnStartCallback(),
                  child,
                  LayoutTransitionSlot::EXIT);
  }
}

void LayoutTransitionDispatcher::OnChildReparented(ViewImpl* child)
{
  if(!child)
  {
    return;
  }
  // Drop every in-flight state. The new parent's next layout pass will
  // dispatch ENTER from the resurrected child's current actor properties.
  CancelActiveAnimation(child);
  CancelActiveAnimator(child);
  CancelPendingExit(child);
}

void LayoutTransitionDispatcher::OnViewDestroyed(ViewImpl* view)
{
  if(!view)
  {
    return;
  }

  // Reuse the helper cancel paths so EXIT ghosts have their interaction
  // state restored before the entry is dropped. OnViewDestroyed is also
  // reached from OnSceneDisconnection — at which point the application
  // may still hold the View handle and re-add the child to a different
  // parent. Skipping the restore would leave SENSITIVE / FOCUSABLE
  // / FOCUS_ON_TOUCH forced to false on the resurrected actor.
  CancelActiveAnimation(view);
  CancelPendingExit(view);
  CancelActiveAnimator(view);

  // Inherited EXIT: the destroyed view may be the GHOST'S DIRECT (visual)
  // parent — the intermediate container — rather than the exiting child
  // itself. The common on-scene case self-heals (destroying the container
  // scene-disconnects the ghost child, which reaches OnViewDestroyed as the
  // child key above), but an off-scene destroy of the container would leave
  // the entry — keyed by child, holding a now-stale parent weak handle —
  // dangling until the animation finished, firing OnFinished on a child whose
  // visual parent is gone. Cancel any pending-exit / EXIT-animator whose
  // visual parent resolves to the destroyed view. Cancellation is silent per
  // the lifecycle contract, and Cancel* is idempotent, so a later
  // child-keyed OnViewDestroyed on the same ghost is a no-op.
  std::vector<ViewImpl*> orphanedPendingExits;
  for(auto& entry : mPendingExits)
  {
    Ui::View ghostParent = entry.second.parent.GetHandle();
    if(ghostParent && &GetImpl(ghostParent) == view)
    {
      orphanedPendingExits.push_back(entry.first);
    }
  }
  for(ViewImpl* child : orphanedPendingExits)
  {
    CancelPendingExit(child);
  }

  std::vector<ViewImpl*> orphanedExitAnimators;
  for(auto& entry : mActiveAnimators)
  {
    if(entry.second.slot != LayoutTransitionSlot::EXIT)
    {
      continue;
    }
    Ui::View ghostParent = entry.second.parentRef.GetHandle();
    if(ghostParent && &GetImpl(ghostParent) == view)
    {
      orphanedExitAnimators.push_back(entry.first);
    }
  }
  for(ViewImpl* child : orphanedExitAnimators)
  {
    CancelActiveAnimator(child);
  }

  mCaptured.erase(view);
  mInitialMountViews.erase(view);
  // Drop captured entries that reference the destroyed view as either the
  // child OR the (SUBTREE-scope inherited) direct parent. A stale parent
  // pointer would otherwise be dereferenced by the pin / VisualBoundsOf /
  // ChildStillPresent paths in StartTransitionsForView when this snapshot
  // is processed.
  for(auto& entry : mCaptured)
  {
    auto& vec = entry.second;
    vec.erase(std::remove_if(vec.begin(), vec.end(),
                             [view](const CapturedBounds& cb)
    { return cb.child == view || cb.parent == view; }),
              vec.end());
  }

  // Inherited ENTER candidates: drop the whole list when the destroyed view is
  // the governing OWNER (the map key), and prune records that reference the
  // destroyed view as child or direct parent. Records whose weak handles have
  // already expired are caught by the dispatch-time re-validation, so this is a
  // best-effort proactive cleanup that bounds growth and avoids stale handles.
  mPendingInheritedEnters.erase(view);
  for(auto& entry : mPendingInheritedEnters)
  {
    auto& records = entry.second;
    records.erase(std::remove_if(records.begin(), records.end(),
                                 [view](const PendingInheritedEnter& r)
    {
      Ui::View c = r.child.GetHandle();
      Ui::View p = r.directParent.GetHandle();
      return (c && &GetImpl(c) == view) || (p && &GetImpl(p) == view);
    }),
                  records.end());
  }
}

void LayoutTransitionDispatcher::OnAnimationFinished(Animation finished)
{
  // List-copy pattern: iterate over a snapshot to stay safe when EmitLifecycle
  // (or the synchronous OnSceneDisconnection path triggered by parent.Remove)
  // mutates the underlying maps.
  std::vector<ViewImpl*> finishedActive;
  for(auto& entry : mActiveAnimations)
  {
    if(entry.second.animation == finished)
    {
      finishedActive.push_back(entry.first);
    }
  }
  for(ViewImpl* child : finishedActive)
  {
    auto it = mActiveAnimations.find(child);
    if(it == mActiveAnimations.end()) continue;

    Ui::LayoutTransition transition     = it->second.transition;
    LayoutTransitionSlot slot           = it->second.slot;
    TransientActorState  transientState = it->second.transientState;
    mActiveAnimations.erase(it);

    // Restore transient properties (clipping) before firing OnFinished so
    // the lifecycle callback observes the post-effect actor state.
    if(child)
    {
      RestoreTransientActorState(child->Self(), transientState);
    }

    if(transition && child)
    {
      EmitLifecycle(GetImpl(transition).GetOnFinishedCallback(),
                    Ui::View::DownCast(child->Self()),
                    slot);
    }
  }

  std::vector<ViewImpl*> finishedExits;
  for(auto& entry : mPendingExits)
  {
    if(entry.second.animation == finished)
    {
      finishedExits.push_back(entry.first);
    }
  }
  for(ViewImpl* child : finishedExits)
  {
    auto it = mPendingExits.find(child);
    if(it == mPendingExits.end()) continue;

    Ui::View             parentHandle     = it->second.parent.GetHandle();
    Ui::View             childHandle      = it->second.child;
    Ui::LayoutTransition transition       = it->second.transition;
    InteractionSnapshot  savedInteraction = it->second.savedInteraction;
    TransientActorState  transientState   = it->second.transientState;
    mPendingExits.erase(it); // erase before parent.Remove to avoid
                             // re-entrant erase via OnSceneDisconnection.

    // Restore transient state (clipping) and ghost interaction state
    // BEFORE unparenting. parentHandle.Remove fires synchronous signals
    // (DevelActor::ChildRemovedSignal) whose handlers may observe or
    // mutate the child's SENSITIVE / FOCUSABLE /
    // FOCUS_ON_TOUCH properties — those must reflect the application's
    // original values, not the disabled-during-EXIT values, by the time
    // any handler runs. Doing the restores first also means handlers
    // setting those properties on the unparented child are not
    // overwritten by a late restore call.
    RestoreTransientActorState(childHandle, transientState);
    RestoreGhostInteraction(childHandle, savedInteraction);

    if(parentHandle && childHandle && childHandle.GetParent() == parentHandle)
    {
      parentHandle.Remove(childHandle);
    }
    if(transition)
    {
      EmitLifecycle(GetImpl(transition).GetOnFinishedCallback(),
                    childHandle,
                    LayoutTransitionSlot::EXIT);
    }
  }
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
