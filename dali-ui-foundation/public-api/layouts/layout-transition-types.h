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
#include <dali/public-api/animation/alpha-function.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace Internal DALI_INTERNAL
{
class LayoutTransitionDispatcher;
}

/**
 * @brief Identifies which layout transition slot is being applied to a view.
 *
 * Each slot has its own spec/animator/timing and is dispatched independently.
 */
enum class LayoutTransitionSlot : uint8_t
{
  ENTER  = 0, ///< View has been added as a child of a transition-attached parent
  EXIT   = 1, ///< View is being removed via View::Remove(child, RemovePolicy::ANIMATE_EXIT) / View::RemoveAll(RemovePolicy::ANIMATE_EXIT)
  CHANGE = 2  ///< Existing child's bounds (size or position) have changed
};

/**
 * @brief Identifies why a CHANGE slot dispatch occurred.
 *
 * Only the CHANGE slot has multiple causes — for ENTER and EXIT the slot
 * itself implies the cause (child added / removed). @c LayoutAnimatorContext
 * exposes @c GetChangeCause(), which is meaningful only when
 * @c GetSlot() returns @c CHANGE.
 *
 * @note @c SIBLING_ADDED and @c SIBLING_REMOVED do NOT mean "this child was
 * added/removed". They mean "this existing child moved BECAUSE a sibling was
 * added/removed under the same parent".
 *
 * Cause precedence (deterministic), used by the dispatcher when multiple
 * conditions could apply:
 *
 *     REORDERED > SIBLING_ADDED > SIBLING_REMOVED > WINDOW_RESIZED > OTHER
 */
enum class LayoutChangeCause : uint8_t
{
  SIBLING_ADDED   = 0, ///< Sibling was added under the same parent → this child moved
  SIBLING_REMOVED = 1, ///< Sibling was removed → this child moved
  REORDERED       = 2, ///< Sibling order changed
  WINDOW_RESIZED  = 3, ///< Top-level window was resized
  OTHER           = 4  ///< Any other layout-driven bounds change
};

/**
 * @brief Selects how far a CHANGE transition reaches into the view tree.
 *
 * A LayoutTransition normally animates only the DIRECT children of the view
 * it is attached to. A grand-child therefore animates only when its own
 * immediate parent also carries a transition. @c SUBTREE removes that
 * requirement for the CHANGE slot: one transition on a container reflows the
 * whole subtree under it with a single timing.
 *
 * Scope resolution: a node is animated by the CLOSEST ancestor that has a
 * transition. That ancestor reaches the node when it is the node's direct
 * parent, or when its scope is @c SUBTREE. A descendant that has its own
 * transition becomes the closest ancestor for its own children, so a
 * @c SUBTREE scope does not cross it (no double animation).
 *
 * @note Applies to CHANGE, and to ENTER / EXIT when the owner carries the
 * corresponding slot effect: a child added under a no-transition descendant
 * fires the owner's ENTER, and a child removed via
 * @c View::Remove(child, RemovePolicy::ANIMATE_EXIT) / @c View::RemoveAll(RemovePolicy::ANIMATE_EXIT)
 * fires the owner's EXIT (an immediate remove — the inherited
 * @c Actor::Remove or @c RemovePolicy::IMMEDIATE — is not deferred). The effect is sourced from the
 * owner while geometry and the EXIT ghost use the child's real direct parent.
 * The closest transition-bearing ancestor always wins, so a descendant with its
 * own transition governs its own subtree. Inherited descendants resolve their
 * CHANGE timing with @c LayoutChangeCause::OTHER (or @c WINDOW_RESIZED during a
 * window resize), so a default CHANGE timing or animator should be configured
 * for @c SUBTREE CHANGE to take effect. @c SUBTREE does not cross a standalone
 * layout-mode boundary.
 */
enum class LayoutReflowScope : uint8_t
{
  DIRECT_CHILDREN = 0, ///< (default) Animate only the attached view's direct children
  SUBTREE         = 1  ///< Also reach deeper descendants; recursion stops at (and includes) a descendant with its own transition, which then governs its subtree
};

/**
 * @brief Per-slot timing for declarative spec-mode layout transitions.
 *
 * Used by:
 *  - @c LayoutBoundsEffect::GetTiming() (ENTER/EXIT bounds channel)
 *  - default CHANGE timing (via @c LayoutTransition::SetChangeTiming(timing))
 *  - cause-specific CHANGE timing (via @c SetChangeTiming(cause, timing))
 *
 * These channels animate layout-owned bounds through dali-core's
 * @c Animation. The alpha function must finish at the target bounds, so
 * built-in functions that finish at the initial state are rejected at
 * registration:
 *  - @c AlphaFunction::REVERSE
 *  - @c AlphaFunction::BOUNCE
 *  - @c AlphaFunction::SIN
 * Target-ending curves such as @c EASE_OUT_BACK are accepted.
 *
 * @note Distinct from @c LayoutAnimatorTiming. Both expose duration, alpha,
 * and delay, but the semantics differ:
 *  - @c LayoutTransitionTiming: dali-core Animation entry timing (declarative).
 *  - @c LayoutAnimatorTiming: callback progress driver (imperative).
 * The two types are not interchangeable — setter signatures enforce this.
 *
 * @note Default-constructed @c LayoutTransitionTiming yields
 * @c {Duration(0.3f), AlphaFunction::EASE_IN_OUT, Duration()}.
 *
 * @note Zero or negative @c duration is treated as "settle the endpoint
 * immediately, no animation". In that case @c delay is also ignored —
 * the endpoint is applied synchronously without any waiting period.
 * To delay an instant settle, schedule the operation from application
 * code instead of relying on @c LayoutTransitionTiming::SetDelay().
 */
struct DALI_UI_API LayoutTransitionTiming
{
  /** @brief Creates timing with a 300ms duration, EASE_IN_OUT alpha, and no delay. */
  LayoutTransitionTiming();

  /**
   * @brief Creates timing from a duration, alpha function, and optional delay.
   * @param[in] duration The duration of the transition
   * @param[in] alpha The alpha function
   * @param[in] delay The delay before the transition
   * @note Validation is performed when this timing is registered with a LayoutTransition.
   */
  LayoutTransitionTiming(Duration      duration,
                         AlphaFunction alpha = AlphaFunction(AlphaFunction::EASE_IN_OUT),
                         Duration      delay = Duration());

  /** @brief Copies the timing into independent storage. */
  LayoutTransitionTiming(const LayoutTransitionTiming& other);

  /** @brief Moves the timing. The source may only be destroyed or reassigned. */
  LayoutTransitionTiming(LayoutTransitionTiming&& other) noexcept;

  /** @brief Copies the timing into independent storage. */
  LayoutTransitionTiming& operator=(const LayoutTransitionTiming& other);

  /** @brief Moves the timing. The source may only be destroyed or reassigned. */
  LayoutTransitionTiming& operator=(LayoutTransitionTiming&& other) noexcept;

  /** @brief Destroys the timing. */
  ~LayoutTransitionTiming();

  /** @brief Sets the duration and returns this timing. Non-positive values are permitted. */
  LayoutTransitionTiming& SetDuration(Duration duration);

  /** @brief Returns the duration. */
  Duration GetDuration() const;

  /** @brief Sets the alpha function and returns this timing. */
  LayoutTransitionTiming& SetAlpha(AlphaFunction alpha);

  /**
   * @brief Returns the alpha function.
   * @return A reference valid until this timing is assigned, moved from, or destroyed
   * @note SetAlpha() updates the value observed through this reference.
   */
  const AlphaFunction& GetAlpha() const;

  /** @brief Sets the delay and returns this timing. */
  LayoutTransitionTiming& SetDelay(Duration delay);

  /** @brief Returns the delay. */
  Duration GetDelay() const;

private:
  struct Impl;
  Impl* mImpl;
};

/**
 * @brief Unit of a @c LayoutBoundsLength.
 *
 * Bounds effect offsets and slide distances are expressed in one of three
 * units. The unit determines what the @c value field of a
 * @c LayoutBoundsLength is multiplied by when the dispatcher resolves the
 * effect into pixel coordinates each frame.
 */
enum class LayoutBoundsUnit : uint8_t
{
  PIXEL           = 0, ///< DALi coordinate units (1 unit = 1 pixel after scaling)
  SELF_FRACTION   = 1, ///< Fraction of the child's own arranged width/height
  PARENT_FRACTION = 2  ///< Fraction of the parent's arranged width/height
};

/**
 * @brief A scalar length expressed in pixels, child fraction, or parent fraction.
 *
 * Negative values are permitted — they encode a direction (e.g. a slide
 * starts from the negative side of an edge).
 *
 * @note The public value and unit fields form a fixed ABI representation.
 * Their types, order and count, and the underlying type of LayoutBoundsUnit,
 * must remain unchanged. Compound lengths require a separate type or API.
 * Finite-value validation occurs when the containing effect is registered.
 *
 * @code
 *   LayoutBoundsLength::Pixel(40.0f);          // 40px
 *   LayoutBoundsLength::SelfFraction(1.0f);    // one child width/height
 *   LayoutBoundsLength::ParentFraction(0.5f);  // half of parent width/height
 * @endcode
 */
struct DALI_UI_API LayoutBoundsLength
{
  float            value{0.0f};
  LayoutBoundsUnit unit{LayoutBoundsUnit::PIXEL};

  static LayoutBoundsLength Pixel(float v)
  {
    return {v, LayoutBoundsUnit::PIXEL};
  }
  static LayoutBoundsLength SelfFraction(float v)
  {
    return {v, LayoutBoundsUnit::SELF_FRACTION};
  }
  static LayoutBoundsLength ParentFraction(float v)
  {
    return {v, LayoutBoundsUnit::PARENT_FRACTION};
  }
};

/**
 * @brief 2D offset composed of two independent @c LayoutBoundsLength values.
 *
 * Used for bounds-effect translation (slide offsets). Each axis may use its
 * own unit; mixing PIXEL on one axis and SELF_FRACTION on the other is
 * supported.
 *
 * @note The two public axes and their scalar lengths are a fixed ABI
 * representation. Copies contain independent axis values.
 */
struct DALI_UI_API LayoutBoundsOffset
{
  LayoutBoundsLength x;
  LayoutBoundsLength y;
};

/**
 * @brief Controls transient clipping while a bounds effect is in flight.
 *
 * A bounds effect can move or scale a view outside its layout-arranged
 * rectangle. To prevent the animation from drawing over siblings or
 * outside the view's own visual region, the dispatcher may apply transient
 * clipping for the duration of the effect.
 *
 * Modes:
 *  - @c AUTO: clip only when the effect changes the view's size factor
 *    (timed non-identity size factors). Offset-only slides are not
 *    clipped automatically because the effect is intended to be visible
 *    outside the layout-arranged region.
 *  - @c NONE: never apply transient clipping. Use when the parent already
 *    clips children, or when overshoot outside the view is desired.
 *  - @c CLIP_TO_BOUNDING_BOX: always clip while the effect is a timed
 *    non-noop, including offset-only slides. Clips the view itself and
 *    its children to the view's own arranged bounding rectangle. To
 *    clip a sliding child to its parent's region instead, set the
 *    parent's clipping mode explicitly.
 */
enum class LayoutBoundsClipMode : uint8_t
{
  AUTO                 = 0,
  NONE                 = 1,
  CLIP_TO_BOUNDING_BOX = 2
};

/**
 * @brief Declarative bounds effect attached to the ENTER or EXIT slot.
 *
 * A @c LayoutBoundsEffect describes an offset and/or size factors around
 * an anchor that the dispatcher composes onto
 * the layout-arranged bounds. The dispatcher synthesises a single
 * @c Animation for each ENTER/EXIT slot dispatch that combines the visual
 * spec entries and the bounds endpoint, so lifecycle callbacks fire
 * exactly once per slot.
 *
 * Endpoint semantics:
 *
 * | Slot  | base                     | from                          | to                           |
 * |-------|--------------------------|-------------------------------|------------------------------|
 * | ENTER | final layout bounds      | ComputeEndpoint(base, effect) | base                         |
 * | EXIT  | current visual bounds    | base                          | ComputeEndpoint(base, effect)|
 *
 * SetOffset() and SetSizeFactor() enable their channels; ClearOffset() and
 * ClearSizeFactor() disable and reset them. Copies hold independent settings.
 * Use the @c LayoutBoundsEffects factory functions (slide / expand / shrink)
 * for the common cases.
 *
 * Validation: @c AlphaFunction::REVERSE, @c AlphaFunction::BOUNCE, and
 * @c AlphaFunction::SIN in the timing, negative size factors, and
 * anchor values outside @c [0,1] are rejected at registration with
 * @c DALI_ABORT. Negative offsets and size factors greater than 1 are
 * permitted.
 */
struct DALI_UI_API LayoutBoundsEffect
{
  /** @brief Creates an effect with no offset or size-factor channel enabled. */
  LayoutBoundsEffect();

  /** @brief Copies the effect and its timing into independent storage. */
  LayoutBoundsEffect(const LayoutBoundsEffect& other);

  /** @brief Moves the effect. The source may only be destroyed or reassigned. */
  LayoutBoundsEffect(LayoutBoundsEffect&& other) noexcept;

  /** @brief Copies the effect and its timing into independent storage. */
  LayoutBoundsEffect& operator=(const LayoutBoundsEffect& other);

  /** @brief Moves the effect. The source may only be destroyed or reassigned. */
  LayoutBoundsEffect& operator=(LayoutBoundsEffect&& other) noexcept;

  /** @brief Destroys the effect. */
  ~LayoutBoundsEffect();

  /** @brief Copies the timing and returns this effect. */
  LayoutBoundsEffect& SetTiming(const LayoutTransitionTiming& timing);

  /**
   * @brief Returns the effect's timing.
   * @return A reference valid until this effect is assigned, moved from, or destroyed
   * @note SetTiming() updates the value observed through this reference.
   */
  const LayoutTransitionTiming& GetTiming() const;

  /** @brief Enables the offset channel, sets its value, and returns this effect. */
  LayoutBoundsEffect& SetOffset(LayoutBoundsOffset value);

  /** @brief Enables the offset channel, sets both axes, and returns this effect. */
  LayoutBoundsEffect& SetOffset(LayoutBoundsLength x, LayoutBoundsLength y);

  /** @brief Disables the offset channel, resets it to zero pixels, and returns this effect. */
  LayoutBoundsEffect& ClearOffset();

  /** @brief Returns whether the offset channel is enabled. */
  bool HasOffset() const;

  /**
   * @brief Returns the offset without copying its axis values.
   * @return A reference valid until this effect is assigned, moved from, or destroyed
   * @note SetOffset() and ClearOffset() update the value observed through this
   * reference. To edit an offset, copy it, modify the copy, then call SetOffset().
   */
  const LayoutBoundsOffset& GetOffset() const;

  /** @brief Enables the size-factor channel, sets both axes, and returns this effect. */
  LayoutBoundsEffect& SetSizeFactor(float x, float y);

  /** @brief Disables the size-factor channel, resets both axes to 1, and returns this effect. */
  LayoutBoundsEffect& ClearSizeFactor();

  /** @brief Returns whether the size-factor channel is enabled. */
  bool HasSizeFactor() const;

  /** @brief Returns the horizontal size factor. */
  float GetSizeFactorX() const;

  /** @brief Returns the vertical size factor. */
  float GetSizeFactorY() const;

  /** @brief Sets the anchor on both axes and returns this effect. */
  LayoutBoundsEffect& SetAnchor(float x, float y);

  /** @brief Returns the horizontal anchor, which defaults to 0.5. */
  float GetAnchorX() const;

  /** @brief Returns the vertical anchor, which defaults to 0.5. */
  float GetAnchorY() const;

  /** @brief Sets the clipping mode and returns this effect. */
  LayoutBoundsEffect& SetClipMode(LayoutBoundsClipMode mode);

  /** @brief Returns the clipping mode, which defaults to AUTO. */
  LayoutBoundsClipMode GetClipMode() const;

private:
  struct Impl;
  Impl* mImpl;
};

/**
 * @brief Per-slot timing for callback-mode animators (progress driver).
 *
 * The animator callback is the progress driver: the framework increments
 * @c rawProgress from 0.0 to 1.0 across @c duration with @c alpha shaping
 * the curve, after waiting @c delay seconds.
 *
 * @note Animator-mode shapes raw progress with @c alpha server-side
 * (the dispatcher evaluates the curve before invoking the callback).
 * The set of @c alpha modes honoured server-side is **smaller** than
 * what spec mode supports through dali-core's @c Animation:
 *
 * | Mode                                   | Animator mode                              |
 * |----------------------------------------|--------------------------------------------|
 * | DEFAULT, LINEAR                        | honoured                                   |
 * | REVERSE                                | rejected by LayoutTransition validation    |
 * | EASE_IN_SQUARE, EASE_OUT_SQUARE        | honoured                                   |
 * | EASE_IN, EASE_OUT, EASE_IN_OUT         | honoured                                   |
 * | EASE_IN_SINE, EASE_OUT_SINE,           | honoured                                   |
 * |   EASE_IN_OUT_SINE                     |                                            |
 * | BOUNCE, SIN, EASE_OUT_BACK             | linear fallback                            |
 * | BEZIER, SPRING, CUSTOM_SPRING          | linear fallback                            |
 * | @c CUSTOM_FUNCTION pointer             | honoured (application-owned output)        |
 *
 * Spec-mode visual specs and bounds animations route entries that pass
 * LayoutTransition validation through dali-core's @c Animation. Visual
 * specs reject @c AlphaFunction::REVERSE. CHANGE timing and
 * bounds-effect timing additionally reject @c BOUNCE and @c SIN
 * because layout-owned bounds must finish at the target. See the
 * AlphaFunction restrictions section in
 * @c wiki/Layout-Transition.md for the rationale.
 *
 * @note @c duration should be positive. A non-positive @c duration
 * collapses @c rawProgress to @c 1.0 on the first (and only) tick,
 * regardless of @c delay; the framework dispatches one callback with
 * @c rawProgress=1.0 and immediately fires @c OnFinished in the same
 * @c TickAnimators turn. To insert a real wait before the animation
 * starts, set @c delay together with a strictly positive @c duration.
 *
 * @note Distinct from @c LayoutTransitionTiming. See its docstring.
 */
struct DALI_UI_API LayoutAnimatorTiming
{
  /** @brief Creates timing with a 300ms duration, EASE_IN_OUT alpha, and no delay. */
  LayoutAnimatorTiming();

  /**
   * @brief Creates timing from a duration, alpha function, and optional delay.
   * @param[in] duration The duration of the transition
   * @param[in] alpha The alpha function
   * @param[in] delay The delay before the transition
   * @note Validation is performed when this timing is registered with a LayoutTransition.
   */
  LayoutAnimatorTiming(Duration      duration,
                       AlphaFunction alpha = AlphaFunction(AlphaFunction::EASE_IN_OUT),
                       Duration      delay = Duration());

  /** @brief Copies the timing into independent storage. */
  LayoutAnimatorTiming(const LayoutAnimatorTiming& other);

  /** @brief Moves the timing. The source may only be destroyed or reassigned. */
  LayoutAnimatorTiming(LayoutAnimatorTiming&& other) noexcept;

  /** @brief Copies the timing into independent storage. */
  LayoutAnimatorTiming& operator=(const LayoutAnimatorTiming& other);

  /** @brief Moves the timing. The source may only be destroyed or reassigned. */
  LayoutAnimatorTiming& operator=(LayoutAnimatorTiming&& other) noexcept;

  /** @brief Destroys the timing. */
  ~LayoutAnimatorTiming();

  /** @brief Sets the duration and returns this timing. Non-positive values are permitted. */
  LayoutAnimatorTiming& SetDuration(Duration duration);

  /** @brief Returns the duration. */
  Duration GetDuration() const;

  /** @brief Sets the alpha function and returns this timing. */
  LayoutAnimatorTiming& SetAlpha(AlphaFunction alpha);

  /**
   * @brief Returns the alpha function.
   * @return A reference valid until this timing is assigned, moved from, or destroyed
   * @note SetAlpha() updates the value observed through this reference.
   */
  const AlphaFunction& GetAlpha() const;

  /** @brief Sets the delay and returns this timing. */
  LayoutAnimatorTiming& SetDelay(Duration delay);

  /** @brief Returns the delay. */
  Duration GetDelay() const;

private:
  struct Impl;
  Impl* mImpl;
};

/**
 * @brief Context passed to a layout animator callback on each frame.
 *
 * The framework supplies a read-only context for each callback invocation.
 * The callback reference is valid only during that invocation. Copying the
 * context creates an independently owned snapshot that remains valid after
 * the callback returns and does not change on later ticks. The snapshot
 * retains the same View handle; it does not copy the View itself.
 *
 * GetFromBounds() and GetToBounds() return bounds captured at the start of
 * the transition and reset on interrupt. Contexts are created internally;
 * applications obtain snapshots by copying a callback context.
 *
 * The callback should write to the view's @c Actor properties only
 * (POSITION_X/Y, SIZE_WIDTH/HEIGHT, OPACITY, SCALE etc.). It must not modify
 * the view tree, request layout, or rebind the LayoutTransition during the
 * callback.
 *
 * @note For ENTER and EXIT slots, GetFromBounds() equals GetToBounds() — the
 * layout system has already placed the child at its final position by
 * the time the animator runs, so the dispatcher has no concept of an
 * "enter from" or "exit to" position. The callback may write Actor
 * properties, including bounds properties, but any bounds override is
 * application-owned: the callback must provide the interpolation and
 * restore the desired final state. Only CHANGE animators see distinct
 * GetFromBounds() (pre-pass) and GetToBounds() (post-pass) from the
 * dispatcher.
 *
 * @note GetChangeCause() is meaningful only when GetSlot() returns CHANGE.
 * For ENTER/EXIT it defaults to @c LayoutChangeCause::OTHER and
 * must not be branched on.
 */
struct DALI_UI_API LayoutAnimatorContext
{
  /** @brief Creates an independently owned snapshot of a callback context. */
  LayoutAnimatorContext(const LayoutAnimatorContext& other);

  /** @brief Replaces this snapshot with an independent copy of another context. */
  LayoutAnimatorContext& operator=(const LayoutAnimatorContext& other);

  /** @brief Destroys this context or snapshot. */
  ~LayoutAnimatorContext();

  /**
   * @brief Returns the target View handle.
   * @warning Do not retain this handle or a context snapshot in the owning
   * transition's callback closure: View and LayoutTransition references can
   * form a cycle. Use WeakHandle<View> for a captured reference.
   */
  View GetView() const;

  /** @brief Returns the transition slot. */
  LayoutTransitionSlot GetSlot() const;

  /** @brief Returns the CHANGE cause, or OTHER for ENTER and EXIT. */
  LayoutChangeCause GetChangeCause() const;

  /**
   * @brief Returns progress after the alpha function has been applied.
   *
   * Honoured built-in forward easing modes follow progress from 0 toward 1.
   * CUSTOM_FUNCTION output is application-owned and may fall outside [0, 1],
   * be non-monotonic, or be reverse-shaped. Use SetOnFinished for completion,
   * chaining, or view-tree mutation instead of comparing progress with 1.
   */
  float GetProgress() const;

  /**
   * @brief Returns linear elapsed progress in [0, 1] before alpha.
   *
   * For positive duration, progress stays at 0 during delay and then advances
   * to 1 over the active duration. Non-positive duration gives 1 on the first
   * tick. Use SetOnFinished for lifecycle completion and view-tree mutation.
   */
  float GetRawProgress() const;

  /** @brief Returns the entry bounds in visual coordinates, mirrored for RTL parents. */
  LayoutRect GetFromBounds() const;

  /** @brief Returns the target bounds in visual coordinates, mirrored for RTL parents. */
  LayoutRect GetToBounds() const;

private:
  friend class Internal::LayoutTransitionDispatcher;

  struct Impl;

  /** @brief Borrows invocation-local storage owned by the dispatcher. */
  DALI_INTERNAL explicit LayoutAnimatorContext(Impl* impl);

  Impl* mImpl;
  bool  mOwnsImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE
