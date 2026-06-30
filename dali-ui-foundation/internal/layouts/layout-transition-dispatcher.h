#pragma once

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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// INTERNAL INCLUDES (handle types used by ghost storage)
#include <dali-ui-foundation/public-api/views/view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

namespace Dali
{
namespace Ui
{

class ViewImpl;

namespace Internal
{

/**
 * @brief Per-window dispatcher that animates layout transitions.
 *
 * Owned by LayoutControllerImpl. Around each layout pass it walks the
 * subtree of every layout root, collects every View that has a
 * LayoutTransition attached, and dispatches the ENTER / EXIT / CHANGE
 * slots for each transition-attached view's children:
 *
 *   - @c CaptureBeforeLayout: DFS-collects transition-attached views and
 *     snapshots each direct child's previous arranged bounds.
 *   - @c StartTransitionsAfterLayout: for each transition-attached view,
 *     compares new arranged bounds against the snapshot. New children
 *     fire ENTER, removed children (handled separately via
 *     ScheduleExit / View::Remove) fire EXIT, and remaining
 *     children whose bounds changed fire CHANGE.
 *   - @c TickAnimators: every frame, advances elapsed time on active
 *     animator-mode states and invokes their callbacks.
 *
 * Each slot supports either spec mode (declarative ViewAnimationSpec /
 * LayoutTransitionTiming applied via dali-core Animation) or animator-callback
 * mode (per-frame application callback that drives the actor properties
 * directly).
 */
class DALI_UI_API LayoutTransitionDispatcher : public ConnectionTracker
{
public:
  LayoutTransitionDispatcher();
  ~LayoutTransitionDispatcher();

  /**
   * @brief Snapshots child bounds before the layout pass runs on @p root.
   *
   * No-op when @p root has no LayoutTransition attached.
   *
   * @param[in] root The view whose layout pass is about to run
   */
  void CaptureBeforeLayout(ViewImpl* root);

  /**
   * @brief Starts CHANGE-slot animations for children whose bounds differ
   * from the snapshot taken by @c CaptureBeforeLayout.
   *
   * @param[in] root The view whose layout pass has just completed
   */
  void StartTransitionsAfterLayout(ViewImpl* root);

  /**
   * @brief Stops any active animation involving @p view and drops the snapshot
   * for it. Called when the view is destroyed or detached from the scene.
   *
   * @param[in] view The view being torn down
   */
  void OnViewDestroyed(ViewImpl* view);

  /**
   * @brief Schedules an EXIT-slot transition for @p child under @p parent.
   *
   * The dispatcher keeps a strong reference to @p child until the EXIT
   * animation finishes; the actor is unparented automatically at that
   * point. If no EXIT spec is configured the child is unparented
   * immediately (synchronous fallback).
   *
   * @p parent is the child's DIRECT (visual) parent: it owns the actor,
   * supplies the bounds frame, hosts the EXIT ghost, and is the final
   * unparent target. @p transitionOwner supplies the EXIT spec / bounds
   * effect / animator / lifecycle callbacks. For a direct EXIT the two are
   * the same view, so @p transitionOwner defaults to @c nullptr meaning
   * "use @p parent". For an inherited (SUBTREE-scope) EXIT they differ: the
   * ghost stays under the direct parent while the effect comes from an
   * ancestor owner (INV-GHOST-UNDER-DIRECT-PARENT).
   *
   * @param[in] parent          The child's direct (visual) parent
   * @param[in] child           The child view to remove
   * @param[in] transitionOwner The view whose LayoutTransition drives the
   *                            EXIT effect; @c nullptr means @p parent
   */
  void ScheduleExit(ViewImpl* parent, Ui::View child, ViewImpl* transitionOwner = nullptr);

  /**
   * @brief Advances active animator-mode transitions.
   *
   * For each active animator state the dispatcher recomputes progress,
   * builds a LayoutAnimatorContext, and invokes the application's
   * callback. When progress reaches 1.0 the state is finalized (and, for
   * EXIT animators, the ghost child is unparented). Time elapsed since
   * the previous tick is computed internally from the dispatcher's wall
   * clock so the per-frame ProcessEvents path and the periodic tick
   * timer share the same source of truth.
   */
  void TickAnimators();

  /**
   * @brief Returns whether at least one animator-mode transition is currently
   * in flight.
   */
  bool HasActiveAnimators() const;

  /**
   * @brief Cancels every in-flight transition for @p child.
   *
   * Called from @c ViewImpl::OnChildAdd when a child is reparented while
   * a CHANGE / ENTER / EXIT transition is still in flight under the old
   * parent. Without this, an animator-mode EXIT would keep firing the
   * application callback against the new parent's coordinate system, and
   * a spec-mode EXIT animation would keep fading the child it has just
   * been re-added under. Cancelling does not fire the OnFinished
   * lifecycle (see caveat in LayoutTransition).
   *
   * @param[in] child The child whose actor was just attached to a new parent
   */
  void OnChildReparented(ViewImpl* child);

  /**
   * @brief Registers a child added under a no-transition container as an
   * inherited (SUBTREE-scope) ENTER candidate.
   *
   * Called from @c ViewImpl::OnChildAdd (via @c LayoutController) when the
   * direct parent has no LayoutTransition of its own. The dispatcher walks up
   * to the closest ancestor SUBTREE owner that carries an ENTER effect and, if
   * found, records a weak-handle pending entry consumed at that owner's next
   * layout pass. No-op when no governing SUBTREE-ENTER owner exists.
   *
   * @param[in] directParent The child's direct (no-transition) parent
   * @param[in] child         The freshly added child
   */
  void NotifyChildAdded(ViewImpl* directParent, Ui::View child);

  /**
   * @brief Drops every inherited-ENTER candidate registered against @p owner.
   *
   * Called from @c ViewImpl::SetLayoutTransition when the owner's transition is
   * detached, mirroring the direct per-view pending-marker clear so a
   * detach -> reattach cycle does not surface a stale ENTER for a grand-child
   * added under the old transition.
   *
   * @param[in] owner The view whose transition was just detached
   */
  void ClearPendingInheritedEnters(ViewImpl* owner);

  /**
   * @brief Marks the next layout pass as window-resize-driven.
   *
   * The dispatcher uses this flag when resolving CHANGE causes:
   * (a) a CHANGE is tagged @c WINDOW_RESIZED only when no higher-
   *     precedence cause applies (cause precedence is
   *     @c REORDERED > @c SIBLING_ADDED > @c SIBLING_REMOVED >
   *     @c WINDOW_RESIZED > @c OTHER), and
   * (b) per-transition @c SetChangeOnWindowResize(false) suppresses
   *     only CHANGE entries whose resolved cause is @c WINDOW_RESIZED;
   *     sibling add / remove / reorder that coincides with a resize
   *     pass keeps its higher-precedence cause and is dispatched as
   *     usual.
   */
  void NotifyWindowResize();

  /**
   * @brief Signals the start of a layout-controller batch.
   *
   * Called by @c LayoutControllerImpl::ProcessLayouts before processing
   * any roots. Increments a recursion-depth counter so that a manual
   * @c LayoutController::ProcessLayouts call from inside a lifecycle
   * callback (which re-enters the dispatcher) does not prematurely
   * clear per-pass flags (e.g. @c mInWindowResize) when its inner
   * @c EndLayoutPass runs before the outer pass finishes.
   */
  void BeginLayoutPass();

  /**
   * @brief Signals the end of a layout-controller batch.
   *
   * Called by @c LayoutControllerImpl::ProcessLayouts after every dirty
   * root in this turn has been processed via @c CaptureBeforeLayout +
   * @c StartTransitionsAfterLayout. Decrements the recursion-depth
   * counter started by @c BeginLayoutPass; only resets per-pass flags
   * (window-resize notification, pending-removal markers) when the
   * outermost pass exits. Per-root reset would clear the flags after
   * the first root and misclassify subsequent roots.
   */
  void EndLayoutPass();

public:
  /// Captures actor properties that the dispatcher mutates transiently for
  /// the duration of an ENTER/EXIT bounds effect. Exposed publicly so that
  /// the dispatcher's translation-unit helpers can capture and restore the
  /// state without referencing private types.
  struct TransientActorState
  {
    bool hasClippingMode{false};
    int  clippingMode{0};
  };

private:
  LayoutTransitionDispatcher(const LayoutTransitionDispatcher&)            = delete;
  LayoutTransitionDispatcher(LayoutTransitionDispatcher&&)                 = delete;
  LayoutTransitionDispatcher& operator=(const LayoutTransitionDispatcher&) = delete;
  LayoutTransitionDispatcher& operator=(LayoutTransitionDispatcher&&)      = delete;

  void OnAnimationFinished(Animation finished);

  /// Returns @p child 's arranged bounds expressed in the actor coordinate
  /// system after @c ApplyLayoutDirection has run on @p parent. When
  /// @p parent 's effective layout direction is RIGHT_TO_LEFT, the
  /// returned x is mirrored: @c parentWidth - logicalX - childWidth.
  /// dispatcher uses this so the from / to bounds it feeds into
  /// SetProperty / AnimateTo match the actor's actual on-screen position
  /// instead of the unmirrored logical bounds @c GetArrangedBounds returns.
  LayoutRect VisualBoundsOf(ViewImpl* parent, ViewImpl* child) const;

  /// DFS-collects @p node and every descendant View that has a
  /// LayoutTransition attached. Allows nested layouts (non-root views) to
  /// participate in transition dispatch.
  void CollectTransitionViews(ViewImpl* node, std::vector<ViewImpl*>& out);

  /// Snapshots a single transition-attached view's children (helper for the
  /// per-root traversal). When the view's reflow scope is SUBTREE, also
  /// snapshots inherited descendants via @c CaptureGovernedChildren.
  void CaptureSingleView(ViewImpl* view);

  /// Dispatches ENTER / CHANGE for a single transition-attached view's
  /// children using the snapshot taken in CaptureSingleView.
  void StartTransitionsForView(ViewImpl* view);

  /// Dispatches inherited (SUBTREE-scope) ENTER for the candidates registered
  /// against @p owner by @c NotifyChildAdded, re-validating current parentage,
  /// governance, and owner ENTER eligibility before firing. When
  /// @p suppressInitialEnter is true (owner's first arrange) the records are
  /// dropped without firing, matching direct-ENTER initial-mount suppression.
  void DispatchPendingInheritedEnters(ViewImpl* owner, bool suppressInitialEnter);

  void StartChangeTransition(ViewImpl*             child,
                             const LayoutRect&     from,
                             const LayoutRect&     to,
                             Ui::LayoutTransition& transition,
                             LayoutChangeCause     cause);

  /// Cancel any in-flight CHANGE / animator / pending EXIT for @p child
  /// and snap its actor bounds to @p to without animation. Shared by
  /// the no-CHANGE-timing, duration-zero, and resize-opt-out branches so
  /// every "no new animation" path leaves the actor on the post-layout
  /// bounds instead of letting a stale in-flight animation keep driving
  /// the old target.
  void SettleChangeWithoutAnimation(ViewImpl* child, const LayoutRect& to);

  /// Compute the EXIT-start bounds for @p child preserving visual
  /// continuity with any in-flight CHANGE / ENTER on the same child.
  /// Used by both the animator EXIT and the bounds-effect EXIT paths so
  /// they begin from the same on-screen state. EXIT callers pin the
  /// sampled value back onto the actor after cancelling the old state.
  LayoutRect CurrentVisualBoundsForExit(ViewImpl*   parent,
                                        ViewImpl*   child,
                                        Dali::Actor actor) const;

  void StartEnterTransition(ViewImpl* child, Ui::LayoutTransition& transition);

  /// Settle a suppressed initial-mount ENTER. For spec mode, evaluates the
  /// ENTER spec at progress = 1.0 and BAKE_FINALs the target values onto
  /// the actor so any pre-set fade-in start (e.g. opacity = 0) does not
  /// leave the child stuck at the start value. For animator mode, no
  /// settle is performed: the application owns the property writes, and
  /// firing a 0-progress tick during suppression would defeat the purpose
  /// of suppressing the launch animation.
  void SettleInitialEnter(ViewImpl* child, Ui::LayoutTransition& transition);

  void StartAnimatorChange(ViewImpl* child, const LayoutRect& from, const LayoutRect& to, Ui::LayoutTransition& transition, LayoutChangeCause cause);
  void StartAnimatorEnter(ViewImpl* child, const LayoutRect& bounds, Ui::LayoutTransition& transition);
  void StartAnimatorExit(ViewImpl*             parent,
                         Ui::View              child,
                         Ui::LayoutTransition& transition);

  enum class SpecCancelPolicy
  {
    PRESERVE_CURRENT,
    SETTLE_ENTER_TO_FINAL
  };

  void CancelActiveAnimation(ViewImpl* child, SpecCancelPolicy policy = SpecCancelPolicy::PRESERVE_CURRENT);

  /// Cancels an in-flight EXIT spec animation for @p child without firing
  /// the OnFinished lifecycle. Called when the same child is re-added (or
  /// reparented) while its EXIT is still in flight, so the resurrected
  /// child is not unparented when the orphan animation finishes.
  void CancelPendingExit(ViewImpl* child);

  void CancelActiveAnimator(ViewImpl* child);

  void DispatchOneTick(ViewImpl* child);

  void FinalizeAnimator(ViewImpl* child);

  /// Snapshot of an actor's interaction state (sensitivity / focusability)
  /// taken at EXIT entry, so the dispatcher can restore the state when the
  /// EXIT is cancelled (e.g. the child is re-added or reparented). On a
  /// normal EXIT-finish the actor is unparented, so restoration is a no-op
  /// even if performed.
  struct InteractionSnapshot
  {
    bool sensitive;
    bool keyboardFocusable;
    bool touchFocusable;
  };

  /// Saves @p actor 's interaction properties and disables them so the
  /// fading-out ghost cannot be tapped, focused, or reached by touch
  /// focus during EXIT. Does NOT clear keyboard focus — call
  /// @c ClearGhostFocusIfHeld AFTER the EXIT state has been registered
  /// in @c mActiveAnimators / @c mPendingExits, so a reparent triggered
  /// from the synchronous @c FocusChangedSignal callback can be observed
  /// and cancelled by @c OnChildReparented.
  static InteractionSnapshot SaveAndDisableGhostInteraction(Dali::Actor actor);

  /// If @p actor currently holds keyboard focus, clears it via
  /// @c FocusManager::ClearFocus. Must be called only AFTER the EXIT
  /// state has been registered (see @c SaveAndDisableGhostInteraction
  /// note); the caller must re-check that its registration still exists
  /// before emitting @c OnStart or arming the animator timer.
  static void ClearGhostFocusIfHeld(Dali::Actor actor);

  /// Restores @p actor 's interaction properties to the values captured by
  /// @c SaveAndDisableGhostInteraction. Called only when an EXIT is
  /// cancelled (re-add / reparent).
  static void RestoreGhostInteraction(Dali::Actor actor, const InteractionSnapshot& snap);

  struct CapturedBounds
  {
    ViewImpl* child;
    ViewImpl* parent;      ///< The child's DIRECT parent at capture time. Equals the
                           ///< owner for direct children; differs for SUBTREE-scope
                           ///< inherited descendants.
    LayoutRect bounds;     ///< Visual bounds in @c parent 's local space.
    bool       freshChild; ///< True if the child had NOT completed its first
                           ///< arrange at capture time, i.e. @c bounds is a
                           ///< pre-arrange (zero) sentinel. Used to suppress a
                           ///< spurious inherited CHANGE that would otherwise
                           ///< animate a never-arranged grand-child from zero.
  };

  /// Appends @p parent 's direct children to @p out (each tagged with
  /// @p parent). When @p recurse is true, descends into children that have
  /// no transition of their own and are not standalone layout roots, so a
  /// SUBTREE-scope owner captures the whole governed subtree in one snapshot.
  void CaptureGovernedChildren(ViewImpl* parent, std::vector<CapturedBounds>& out, bool recurse);

  /// State for an in-flight spec-mode CHANGE / ENTER animation. Records the
  /// transition handle and slot so OnFinished can be emitted with the
  /// correct arguments.
  struct ActiveSpecAnimation
  {
    Animation            animation;
    Ui::LayoutTransition transition;
    LayoutTransitionSlot slot;
    TransientActorState  transientState; ///< Properties to restore at finish/cancel
  };

  /// Ghost storage for an EXIT-slot transition: keeps the child alive and
  /// records its parent so the actor can be unparented when the animation
  /// finishes.
  struct GhostExit
  {
    WeakHandle<Ui::View> parent;
    Ui::View             child; ///< Strong reference; prevents destruction during EXIT
    Animation            animation;
    Ui::LayoutTransition transition;
    InteractionSnapshot  savedInteraction; ///< Restored on cancel/reparent
    TransientActorState  transientState;   ///< Properties to restore at finish/cancel
  };

  /// Per-(child, slot) state for an active animator-mode transition.
  ///
  /// The dispatcher drives @c elapsed each frame from TickAnimators and
  /// invokes the application callback through the held LayoutTransition
  /// handle. EXIT animators additionally hold a strong reference to the
  /// child and a weak handle to the parent so the ghost child can be
  /// unparented when progress reaches 1.0.
  struct AnimatorState
  {
    LayoutTransitionSlot slot;
    LayoutChangeCause    cause;
    LayoutAnimatorTiming timing;
    LayoutRect           fromBounds;
    LayoutRect           toBounds;
    LayoutRect           lastLerped;
    float                elapsed;    ///< Seconds since the animator started
    Ui::LayoutTransition transition; ///< Source of the per-frame callback
    Ui::View             childRef;   ///< Strong ref for EXIT only
    WeakHandle<Ui::View> parentRef;  ///< For EXIT unparent
    bool                 finished;   ///< Set true after the final tick to mark for cleanup
    /// True for the very first tick after creation. The first TickAnimators
    /// call after StartAnimator* must not advance @c elapsed by the
    /// accumulated @c deltaSec — otherwise an idle interval (during which
    /// dali-core does not invoke this Processor and @c mLastTickTime stayed
    /// stale) would jump @c progress directly to 1.0 and the transition
    /// would never animate visibly. Cleared after the first tick.
    bool freshlyCreated;
    /// Filled only for slot==EXIT; used to restore actor interaction on
    /// cancellation (re-add of the same child to a different parent,
    /// reparent, or destruction / scene disconnection while EXIT is in
    /// flight). Replacing the @c LayoutTransition handle is NOT a
    /// cancellation — in-flight EXIT continues to completion under the
    /// original handle and the actor is unparented normally; this snapshot
    /// is unused on the normal-finish path.
    InteractionSnapshot savedInteraction;
  };

  /// A child added under a no-transition container, pending inherited
  /// (SUBTREE-scope) ENTER dispatch at the governing owner's next layout pass.
  /// Both handles are weak: the owner (the map key) supplies the effect, the
  /// direct parent supplies the bounds frame. Parentage and governance are
  /// re-validated at dispatch — transition replace on the owner is tolerated
  /// (the record survives and the owner's CURRENT state decides); only detach,
  /// loss of governance, or destruction drops it.
  struct PendingInheritedEnter
  {
    WeakHandle<Ui::View> directParent;
    WeakHandle<Ui::View> child;
  };

  std::unordered_map<ViewImpl*, std::vector<CapturedBounds>>        mCaptured;               ///< Per-root snapshot list, valid for one layout pass
  std::unordered_set<ViewImpl*>                                     mInitialMountViews;      ///< Transition roots captured before their first @c Arrange. Consumed by @c StartTransitionsForView to decide whether to suppress ENTER for that pass
  std::unordered_map<ViewImpl*, ActiveSpecAnimation>                mActiveAnimations;       ///< Active CHANGE / ENTER spec animations, keyed by child
  std::unordered_map<ViewImpl*, GhostExit>                          mPendingExits;           ///< In-flight EXIT spec animations, keyed by child
  std::unordered_map<ViewImpl*, AnimatorState>                      mActiveAnimators;        ///< In-flight animator-callback transitions, keyed by child
  std::unordered_map<ViewImpl*, std::vector<PendingInheritedEnter>> mPendingInheritedEnters; ///< Inherited (SUBTREE) ENTER candidates, keyed by governing owner
  bool                                                              mInWindowResize{false};
  int                                                               mLayoutPassDepth{0}; ///< Recursion depth for ProcessLayouts re-entry safety

  /// Periodic timer that drives animator ticks while the event loop has
  /// no other work. The primary arm happens in @c EnsureAnimatorTicking
  /// called from each @c StartAnimator* helper at state insertion;
  /// @c TickAnimators retains a defensive arm for paths that bypass
  /// @c StartAnimator*.
  ///
  /// Lifecycle: @c TickAnimators resets this handle when @c mActiveAnimators
  /// drains. The reset is safe because @c Dali::Timer::Tick takes a local
  /// guard reference to keep the underlying timer source alive across the
  /// signal emission, so dropping the dispatcher's reference here releases
  /// only one ref — the source is destroyed only after @c Tick unwinds.
  /// @c OnTickTimer always returns @c true; the source is removed by the
  /// destructor when the last reference unwinds, not by a @c false return.
  /// Always-true return is safe even when @c FlushUpdateMessages destroys
  /// @c this mid-call (window-invalid self-erase): @c OnTickTimer touches
  /// no members after the flush, and the literal value cannot UAF.
  ///
  /// dali-core only wakes the event thread when the @c NotificationManager
  /// produces a message, which a pure animator-callback transition never
  /// does — so without this timer the next @c ProcessEvents (and the next
  /// tick) would only fire on user input, not per frame.
  Dali::Timer mTickTimer;

  /// Wall-clock timestamp of the last tick. TickAnimators uses this to
  /// compute deltaSec internally regardless of which path drove the tick
  /// (LayoutControllerImpl::Process or @c mTickTimer). The value is reset
  /// whenever the active set transitions from empty to non-empty so a long
  /// idle interval cannot collapse the next tick into a single jump.
  std::chrono::steady_clock::time_point mLastTickTime;

  bool OnTickTimer();

  /// Lazily creates and starts @c mTickTimer when the active animator set
  /// transitions from empty to non-empty. Called from each @c StartAnimator*
  /// helper immediately after the new state has been inserted, so the timer
  /// is armed at the moment of state insertion rather than at the next
  /// LayoutControllerImpl::Process — making animator-mode dispatch robust
  /// against call sites that do not run inside a layout pass (e.g. the
  /// EXIT-via-Remove path). Also resets @c mLastTickTime so the first
  /// tick after a long idle interval starts from a fresh wall clock.
  void EnsureAnimatorTicking();
};

} // namespace Internal
} // namespace Ui
} // namespace Dali
