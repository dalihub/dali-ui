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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/callback.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>

namespace Dali
{
namespace Ui
{

class View;

namespace Internal DALI_INTERNAL
{
class LayoutTransitionImpl;
}

/**
 * @brief Animator callback driven each frame by the layout transition controller.
 *
 * Signature: void(const LayoutAnimatorContext&)
 *
 * The callback is invoked once per frame on the event thread, immediately
 * after the layout pass that started or updated the transition. The
 * callback should set the view's animatable properties from
 * @c ctx.fromBounds / @c ctx.toBounds and @c ctx.progress.
 *
 * @warning Do NOT capture the target View by value inside the callback
 * closure. The dispatcher holds a strong reference to the LayoutTransition
 * (which owns the callback), and the View holds a strong reference to the
 * LayoutTransition — capturing the View by value forms a reference cycle
 * that prevents the View from being destroyed. Use @c WeakHandle<View>
 * if a captured reference is required, or read @c ctx.view inside the
 * callback body each frame.
 *
 * @warning Do NOT call @c LayoutTransition::SetEnter/Exit/ChangeAnimator
 * from inside the callback body — replacing the in-flight callback while
 * it is executing is undefined behaviour. Schedule the change from a
 * lifecycle callback (OnFinished) or an external event handler instead.
 *
 * @warning Do NOT modify the view tree (Add / Remove / Unparent) or
 * call @c InvalidateMeasure from inside the callback. Such mutations are
 * supported only from lifecycle callbacks.
 */
using LayoutAnimatorCallback = Callback<void(const LayoutAnimatorContext&)>;

/**
 * @brief Lifecycle callback fired when a per-slot transition starts or finishes.
 *
 * Signature: void(View, LayoutTransitionSlot)
 *
 * Fired on the event thread; safe to mutate the view tree from inside.
 *
 * @warning As with @c LayoutAnimatorCallback, do not capture the View by
 * value inside the closure (cycle risk). Use @c WeakHandle<View>.
 *
 * @note OnFinished is fired only when a transition reaches **normal
 * completion** — i.e. the configured timing has fully elapsed
 * (@c rawProgress reaches 1.0 in animator mode, or dali-core's
 * @c Animation::FinishedSignal fires in spec mode). The shaped
 * @c ctx.progress value at that moment is not 1.0 in general: a
 * @c CUSTOM_FUNCTION pointer may return any value at the final tick.
 * (@c AlphaFunction::REVERSE is rejected by LayoutTransition
 * validation — see docs/layout-transition.md.) Cancellation paths drop the
 * in-flight callback silently — @c OnFinished is NOT fired in any of
 * these cases:
 *   - Same-slot supersession: the application starts a new spec/animator
 *     on the SAME slot of the SAME child before the in-flight transition
 *     finishes (e.g. CHANGE replaces an in-flight CHANGE).
 *   - Cross-slot supersession: a different slot is dispatched for the
 *     SAME child while an in-flight transition exists. Specifically:
 *     - @c Remove starts EXIT and supersedes any in-flight CHANGE
 *       or ENTER on that child (the cancelled slot's @c OnFinished is
 *       NOT fired).
 *     - A new layout pass starts CHANGE and supersedes any in-flight
 *       ENTER on that child.
 *   - Re-add of the child to a different parent (reparent).
 *   - View destruction / scene disconnection.
 * Replacing the @c LayoutTransition handle on a view via
 * @c View::SetLayoutTransition does NOT count as cancellation — each
 * in-flight transition finishes on its own and fires its own
 * @c OnFinished under the original transition handle.
 *
 * @note For @c slot==EXIT, the @c View has already been unparented from
 * its original parent by the time @c OnFinished fires (the dispatcher
 * unparents synchronously, then emits the lifecycle). Do not rely on
 * @c view.GetParent() inside an EXIT @c OnFinished — it returns an
 * uninitialized handle. The @c view handle itself is still valid.
 *
 * @note An *empty* @c ViewAnimationSpec (no entries, or all entries with
 * zero duration+delay) attached to ENTER/EXIT is treated as "no
 * transition": the framework unparents (EXIT) or skips (ENTER)
 * immediately and does NOT fire @c OnStart or @c OnFinished. To get
 * lifecycle hooks for an instant transition, use animator mode with a
 * positive @c duration, or add a non-zero entry to the spec.
 */
using LayoutLifecycleCallback = Callback<void(View, LayoutTransitionSlot)>;

/**
 * @brief Declares how a View animates between layout-pass results.
 *
 * A LayoutTransition is attached to a View with @c View::SetLayoutTransition().
 * When attached, the framework captures pre-pass and post-pass bounds for
 * each direct child and dispatches per-slot animations:
 *
 * - @c ENTER:  fired when a new child is added under this view AFTER the
 *              parent has completed its initial arrange pass (see
 *              @c SetEnterOnInitialMount for the initial-mount opt-in)
 * - @c EXIT:   fired when @c View::Remove / @c RemoveAllChildren removes
 *              a child (deferred until the EXIT slot finishes)
 * - @c CHANGE: fired when an existing child's bounds change between layout passes
 *
 * Default behavior of @c LayoutTransition::New():
 *  - CHANGE timing is enabled with default @c LayoutTransitionTiming
 *    (0.3s, EASE_IN_OUT). Attaching the transition automatically animates
 *    sibling add/remove/reorder driven CHANGE.
 *  - ENTER and EXIT are @b opt-in. Set a visual spec, bounds effect, or
 *    animator to make a slot dispatch.
 *  - Window-resize-driven CHANGE is opt-in via @c SetChangeOnWindowResize(true).
 *
 * Each slot accepts a declarative spec (visual property animation + bounds
 * effect) and/or an animator callback. When an animator is set for a slot,
 * the slot's declarative spec is ignored (animator wins). Exception:
 * @c SetChangeOnWindowResize(false) skips window-resize CHANGE even if a
 * CHANGE animator is set.
 *
 * @note A LayoutTransition handle is reference-counted and may be attached
 * to multiple Views simultaneously; each View runs its own per-pass state
 * but shares the spec/animator/timing configuration. Modifying the handle
 * (e.g. calling @c ClearChangeTiming) affects all Views it is attached to.
 *
 * @note ENTER is suppressed for children that are already present at the
 * parent's very first arrange pass (initial mount). Those children are
 * treated as the parent's initial visual state, because the first arrange
 * typically completes before the window surface is on screen and any ENTER
 * animation dispatched there would elapse while the user can see nothing.
 * Declarative ENTER visual specs are still settled to their final values
 * (e.g. opacity 0 to 1 lands at opacity 1) so a child that pre-sets a
 * fade-in start value does not stay hidden. Animator-mode ENTER is skipped
 * without settling property values. Use @c SetEnterOnInitialMount(true) to
 * opt back in to firing ENTER on the initial mount.
 *
 * @note When @c View::Remove(child, RemovePolicy::ANIMATE_EXIT) is called and
 * an EXIT slot is configured, the child is removed from the layout-tracking
 * list immediately so siblings reflow into the freed slot — but the actor
 * stays attached during the EXIT animation. If you instead call
 * @c Remove(child, RemovePolicy::IMMEDIATE), the child is unparented
 * immediately and its EXIT is skipped (though IMMEDIATE still honors the
 * in-flight-ghost guard: it is a no-op on a child already mid-EXIT, leaving
 * that animation to finish). The inherited one-argument @c Actor::Remove also
 * unparents immediately, bypassing the View remove path entirely.
 *
 * @note During the EXIT animation the child is a "ghost": it stays in the
 * actor tree but is absent from the parent's logical child list. Adding
 * the same child back to the SAME parent in this state via
 * @c View::Insert or inherited @c Actor::Add is silently ignored — the
 * EXIT continues, and the actor is unparented when the EXIT animation
 * finishes. To cancel an in-flight EXIT, reparent the child to a
 * DIFFERENT parent: the dispatcher auto-cancels the EXIT, restores
 * interaction state, and triggers ENTER under the new parent.
 *
 * @note Replacing a slot's spec/animator while a transition is in flight
 * does NOT cancel the in-flight transition. The dispatcher captures the
 * @c timing at start (immutable for the rest of that transition) but
 * reads the current @c callback every tick (mutable mid-flight). To
 * avoid timing/callback mismatch surprises, only replace specs/animators
 * while no transitions are running on the affected child.
 *
 * @code
 *   auto t = LayoutTransition::New();
 *   t.SetChangeTiming({Duration(0.3f), AlphaFunction(AlphaFunction::EASE_IN_OUT), {}});
 *   layout.SetLayoutTransition(t);
 * @endcode
 */
class DALI_UI_API LayoutTransition : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized handle.
   */
  LayoutTransition();

  /**
   * @brief Creates an initialized LayoutTransition with default-constructed slots.
   *
   * @return A handle to a newly allocated LayoutTransition
   */
  static LayoutTransition New();

  LayoutTransition(const LayoutTransition& other);
  LayoutTransition(LayoutTransition&& rhs) noexcept;
  ~LayoutTransition();
  LayoutTransition& operator=(const LayoutTransition& other);
  LayoutTransition& operator=(LayoutTransition&& rhs) noexcept;

  /**
   * @brief Downcasts a handle to LayoutTransition.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a LayoutTransition or an uninitialized handle
   */
  static LayoutTransition DownCast(BaseHandle handle);

public:
  // ──────────────────────────────────────────────────────────────────────
  // ENTER / EXIT visual property animation (Spec mode, declarative)
  //
  // Visual property only: opacity, scale, color, corner, borderline, etc.
  // Bounds properties (POSITION_X/Y, SIZE_WIDTH/HEIGHT) are rejected at
  // registration — use the bounds channel (SetEnterBoundsEffect /
  // SetExitBoundsEffect with LayoutBoundsEffects::SlideFrom / SlideTo /
  // ExpandFrom / ShrinkTo) for ENTER/EXIT slide / expand / shrink effects.
  // ──────────────────────────────────────────────────────────────────────

  /**
   * @brief Sets the ENTER slot visual property animation spec.
   *
   * @param[in] spec Declarative visual property animation spec
   *                 (POSITION/SIZE entries are rejected)
   * @return Reference to this for chaining
   */
  LayoutTransition& SetEnterVisualSpec(ViewAnimationSpec spec);

  /**
   * @brief Sets the EXIT slot visual property animation spec.
   *
   * @param[in] spec Declarative visual property animation spec
   *                 (POSITION/SIZE entries are rejected)
   * @return Reference to this for chaining
   */
  LayoutTransition& SetExitVisualSpec(ViewAnimationSpec spec);

  /**
   * @brief Clears the ENTER slot visual property animation spec.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearEnterVisualSpec();

  /**
   * @brief Clears the EXIT slot visual property animation spec.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearExitVisualSpec();

  // ──────────────────────────────────────────────────────────────────────
  // ENTER / EXIT bounds effect (Spec mode, declarative)
  //
  // Bounds effects choreograph the view's POSITION_X/Y and SIZE_WIDTH/HEIGHT
  // around the layout-arranged bounds. ENTER plays from the effect endpoint
  // toward the final layout bounds; EXIT plays from the current visual bounds
  // toward the effect endpoint. The dispatcher composes the bounds effect
  // with the slot's visual spec into a single Animation so lifecycle
  // callbacks fire exactly once per slot.
  // ──────────────────────────────────────────────────────────────────────

  /**
   * @brief Sets the ENTER slot bounds effect.
   *
   * @param[in] effect Bounds-effect descriptor (translation, scale, anchor,
   *                   clip mode, timing). Invalid alpha / size factor /
   *                   anchor values are rejected with @c DALI_ABORT.
   * @return Reference to this for chaining
   */
  LayoutTransition& SetEnterBoundsEffect(const LayoutBoundsEffect& effect);

  /**
   * @brief Sets the EXIT slot bounds effect.
   *
   * @param[in] effect Bounds-effect descriptor
   * @return Reference to this for chaining
   */
  LayoutTransition& SetExitBoundsEffect(const LayoutBoundsEffect& effect);

  /**
   * @brief Clears the ENTER slot bounds effect.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearEnterBoundsEffect();

  /**
   * @brief Clears the EXIT slot bounds effect.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearExitBoundsEffect();

  // ──────────────────────────────────────────────────────────────────────
  // CHANGE timing (Spec mode, declarative; bounds driven by layout)
  //
  // The framework interpolates @c POSITION_X/Y and @c SIZE_WIDTH/HEIGHT
  // from the previous arranged bounds to the new measured bounds.
  // Only timing is configurable.
  //
  // Default behavior:
  //   - @c LayoutTransition::New() enables default CHANGE timing
  //     (0.3s, EASE_IN_OUT).
  //   - @c SetChangeTiming(timing) updates the default value and ensures
  //     default is enabled.
  //   - @c SetChangeTiming(cause, timing) sets a cause-specific override
  //     (independent of default's enabled state).
  //   - @c ClearChangeTiming() disables default (value preserved).
  //   - @c ClearChangeTiming(cause) clears a cause-specific override.
  //
  // Cause-specific timing takes precedence over default. If neither is
  // active for a given cause, the CHANGE is settled to the new bounds
  // without animation.
  // ──────────────────────────────────────────────────────────────────────

  /**
   * @brief Sets the default CHANGE slot timing and enables it.
   *
   * @param[in] timing Timing parameters
   * @return Reference to this for chaining
   */
  LayoutTransition& SetChangeTiming(const LayoutTransitionTiming& timing);

  /**
   * @brief Sets a cause-specific CHANGE slot timing override.
   *
   * Overrides default CHANGE timing for the specific cause. Cause-specific
   * timing is active independent of the default's enabled state.
   *
   * @param[in] cause  CHANGE cause to override
   * @param[in] timing Timing parameters
   * @return Reference to this for chaining
   */
  LayoutTransition& SetChangeTiming(LayoutChangeCause             cause,
                                    const LayoutTransitionTiming& timing);

  /**
   * @brief Disables the default CHANGE slot timing.
   *
   * After this call, CHANGE causes without a cause-specific timing are
   * not animated. Cause-specific timings remain active.
   *
   * @note The stored default timing value is preserved; subsequent
   * @c SetChangeTiming() call re-enables with the new value.
   *
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearChangeTiming();

  /**
   * @brief Clears a cause-specific CHANGE slot timing override.
   *
   * @param[in] cause CHANGE cause whose override is cleared
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearChangeTiming(LayoutChangeCause cause);

  // ──────────────────────────────────────────────────────────────────────
  // Animator mode (imperative): application drives interpolation per frame.
  //
  // The framework increments progress 0..1 over the timing's duration with
  // the timing's alpha curve and invokes the callback once per frame.
  // When set, the slot's spec is ignored.
  // ──────────────────────────────────────────────────────────────────────

  /**
   * @brief Sets the ENTER slot animator callback and its progress timing.
   * @param[in] callback Animator callback (ownership transferred)
   * @param[in] timing   Progress driver timing
   * @return Reference to this for chaining
   */
  LayoutTransition& SetEnterAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing);

  /**
   * @brief Sets the EXIT slot animator callback and its progress timing.
   * @param[in] callback Animator callback (ownership transferred)
   * @param[in] timing   Progress driver timing
   * @return Reference to this for chaining
   */
  LayoutTransition& SetExitAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing);

  /**
   * @brief Sets the CHANGE slot animator callback and its progress timing.
   * @param[in] callback Animator callback (ownership transferred)
   * @param[in] timing   Progress driver timing
   * @return Reference to this for chaining
   */
  LayoutTransition& SetChangeAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing);

  /**
   * @brief Clears the ENTER slot animator callback.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearEnterAnimator();

  /**
   * @brief Clears the EXIT slot animator callback.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearExitAnimator();

  /**
   * @brief Clears the CHANGE slot animator callback.
   * @return Reference to this for chaining
   */
  LayoutTransition& ClearChangeAnimator();

  // ──────────────────────────────────────────────────────────────────────
  // Composition options
  // ──────────────────────────────────────────────────────────────────────

  /**
   * @brief Controls whether window-resize-driven layout changes trigger
   * the CHANGE slot.
   *
   * Default is @c false: window resize updates bounds without animation
   * to avoid mass-spike on rotation / window restore.
   *
   * @note This option takes precedence over a CHANGE animator. If set to
   * @c false, window-resize CHANGE is skipped even when a CHANGE animator
   * is configured (the slot opt-out is the user's explicit intent that
   * window resize should not animate).
   *
   * @param[in] enable @c true to animate on window resize
   * @return Reference to this for chaining
   */
  LayoutTransition& SetChangeOnWindowResize(bool enable);

  /**
   * @brief Controls whether the ENTER slot fires for children that are
   * present at the parent's very first arrange pass (initial mount).
   *
   * Default is @c false. The first arrange typically completes before the
   * window surface is presented to the display, so an ENTER animation
   * dispatched there would elapse while the window is still invisible.
   * Suppressing initial-mount ENTER lets the application show its initial
   * content immediately, with ENTER reserved for adds that happen after
   * the parent has been arranged and can present the transition on
   * screen.
   *
   * When suppressed, declarative ENTER specs are settled to their final
   * values without lifecycle callbacks (e.g. an @c Opacity(1.0) spec lands
   * the child at opacity 1.0 even though it was pre-set to 0). Animator-
   * mode ENTER is skipped without settling property values.
   *
   * Set to @c true to opt in to firing ENTER for initially-mounted
   * children. This is useful when the application intentionally wants a launch
   * animation and accepts that the effect may be partially or fully
   * elapsed by the time the surface becomes visible.
   *
   * @param[in] enable @c true to fire ENTER on initial mount
   * @return Reference to this for chaining
   */
  LayoutTransition& SetEnterOnInitialMount(bool enable);

  /**
   * @brief Selects how far the CHANGE slot reaches into the view tree.
   *
   * Default is @c LayoutReflowScope::DIRECT_CHILDREN: only the attached
   * view's direct children animate, so a grand-child reflows only when its
   * own immediate parent also carries a transition.
   *
   * @c LayoutReflowScope::SUBTREE reflows the whole subtree under the
   * attached view with this transition's timing, without a transition on
   * every intermediate container. A descendant that has its own transition
   * stops the scope at that boundary (it governs its own children).
   *
   * @note Applies to CHANGE, and to ENTER / EXIT when the owner carries the
   * corresponding slot effect: a child added under a no-transition descendant
   * fires the owner's ENTER, and a child removed via
   * @c View::Remove(child, RemovePolicy::ANIMATE_EXIT) / @c RemoveAllChildren
   * fires the owner's EXIT (an immediate remove — the inherited
   * @c Actor::Remove or @c RemovePolicy::IMMEDIATE — is not deferred). The effect is sourced from
   * this owner while geometry and the EXIT ghost use the child's real direct
   * parent. The closest transition-bearing ancestor wins (a descendant with its
   * own transition governs its own subtree). Inherited descendants use
   * @c LayoutChangeCause::OTHER (or @c WINDOW_RESIZED during a window resize)
   * for CHANGE timing, so configure a default CHANGE timing or animator for
   * @c SUBTREE CHANGE to take effect. The scope does not cross a standalone
   * layout-mode boundary.
   *
   * @param[in] scope The reflow scope to apply
   * @return Reference to this for chaining
   */
  LayoutTransition& SetReflowScope(LayoutReflowScope scope);

  // ──────────────────────────────────────────────────────────────────────
  // Lifecycle hooks (event-thread, per child × slot)
  // ──────────────────────────────────────────────────────────────────────

  /**
   * @brief Sets a callback fired when a per-(view, slot) transition starts.
   * @param[in] callback Lifecycle callback (ownership transferred)
   * @return Reference to this for chaining
   */
  LayoutTransition& SetOnStart(LayoutLifecycleCallback callback);

  /**
   * @brief Sets a callback fired when a per-(view, slot) transition reaches
   * normal completion (configured timing fully elapsed).
   *
   * @note Cancelled transitions DROP the in-flight callback silently —
   * @c OnFinished is NOT fired for any of these cancellation paths:
   *   - Re-add of the same child to a different parent (reparent).
   *   - Same-slot supersession: a new spec/animator started for the SAME
   *     slot on the SAME child before the in-flight transition finishes.
   *   - Cross-slot supersession on the SAME child:
   *     - @c Remove starts EXIT and cancels any in-flight CHANGE
   *       or ENTER on the child.
   *     - A new layout pass starts CHANGE and cancels any in-flight
   *       ENTER on the child.
   *   - @c View destruction / scene disconnection.
   * Replacing the @c LayoutTransition handle on a view via
   * @c View::SetLayoutTransition does NOT cancel in-flight transitions —
   * each finishes on its own and fires its own @c OnFinished. See
   * @c LayoutLifecycleCallback for the full semantics.
   *
   * @param[in] callback Lifecycle callback (ownership transferred)
   * @return Reference to this for chaining
   */
  LayoutTransition& SetOnFinished(LayoutLifecycleCallback callback);

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL explicit LayoutTransition(Internal::LayoutTransitionImpl* impl);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
