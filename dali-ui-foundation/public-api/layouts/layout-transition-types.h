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
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Identifies which layout transition slot is being applied to a view.
 *
 * Each slot has its own spec/animator/timing and is dispatched independently.
 */
enum class LayoutTransitionSlot : uint8_t
{
  ENTER  = 0, ///< View has been added as a child of a transition-attached parent
  EXIT   = 1, ///< View is being removed via View::RemoveChild / RemoveAllChildren
  CHANGE = 2  ///< Existing child's bounds (size or position) have changed
};

/**
 * @brief Identifies why a CHANGE slot dispatch occurred.
 *
 * Only the CHANGE slot has multiple causes — for ENTER and EXIT the slot
 * itself implies the cause (child added / removed). @c LayoutAnimatorContext
 * exposes a @c changeCause field that is meaningful only when
 * @c slot == CHANGE.
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
 * @note Applies to the CHANGE slot only. ENTER and EXIT remain scoped to the
 * direct parent. Inherited descendants resolve their CHANGE timing with
 * @c LayoutChangeCause::OTHER (or @c WINDOW_RESIZED during a window resize),
 * so a default CHANGE timing or animator should be configured for @c SUBTREE
 * to take effect. @c SUBTREE does not cross a standalone layout-mode boundary.
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
 *  - @c LayoutBoundsEffect::timing (ENTER/EXIT bounds channel)
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
 * @note Distinct from @c LayoutAnimatorTiming. The field layout is identical
 * but the semantics differ:
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
 * code instead of relying on @c LayoutTransitionTiming::delay.
 */
struct DALI_UI_API LayoutTransitionTiming
{
  Duration      duration{0.3f};
  AlphaFunction alpha{AlphaFunction::EASE_IN_OUT};
  Duration      delay;
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
 *    (timed non-identity @c sizeFactorX/Y). Offset-only slides are not
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
 * A @c LayoutBoundsEffect describes a translation (@c offset) and/or scale
 * (@c sizeFactorX/Y around @c anchorX/Y) that the dispatcher composes onto
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
 * Chained setters are provided so callers do not silently produce a no-op
 * effect by setting @c offset / @c sizeFactor without flipping the
 * @c hasOffset / @c hasSizeFactor flags. Use the @c LayoutBoundsEffects
 * factory functions (slide / expand / shrink) for the common cases.
 *
 * Validation: @c AlphaFunction::REVERSE, @c AlphaFunction::BOUNCE, and
 * @c AlphaFunction::SIN in @c timing, negative @c sizeFactorX/Y, and
 * @c anchorX/Y outside @c [0,1] are rejected at registration with
 * @c DALI_ABORT. Negative @c offset values and
 * @c sizeFactor > 1 are permitted.
 */
struct DALI_UI_API LayoutBoundsEffect
{
  LayoutTransitionTiming timing;

  bool               hasOffset{false};
  LayoutBoundsOffset offset;

  bool  hasSizeFactor{false};
  float sizeFactorX{1.0f};
  float sizeFactorY{1.0f};
  float anchorX{0.5f};
  float anchorY{0.5f};

  LayoutBoundsClipMode clipMode{LayoutBoundsClipMode::AUTO};

  LayoutBoundsEffect& SetTiming(const LayoutTransitionTiming& t)
  {
    timing = t;
    return *this;
  }

  LayoutBoundsEffect& SetOffset(LayoutBoundsOffset value)
  {
    hasOffset = true;
    offset    = value;
    return *this;
  }

  LayoutBoundsEffect& SetOffset(LayoutBoundsLength xv, LayoutBoundsLength yv)
  {
    hasOffset = true;
    offset    = {xv, yv};
    return *this;
  }

  LayoutBoundsEffect& ClearOffset()
  {
    hasOffset = false;
    offset    = {};
    return *this;
  }

  LayoutBoundsEffect& SetSizeFactor(float x, float y)
  {
    hasSizeFactor = true;
    sizeFactorX   = x;
    sizeFactorY   = y;
    return *this;
  }

  LayoutBoundsEffect& ClearSizeFactor()
  {
    hasSizeFactor = false;
    sizeFactorX   = 1.0f;
    sizeFactorY   = 1.0f;
    return *this;
  }

  LayoutBoundsEffect& SetAnchor(float x, float y)
  {
    anchorX = x;
    anchorY = y;
    return *this;
  }

  LayoutBoundsEffect& SetClipMode(LayoutBoundsClipMode mode)
  {
    clipMode = mode;
    return *this;
  }
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
 * @c LayoutBoundsEffect::timing additionally reject @c BOUNCE and @c SIN
 * because layout-owned bounds must finish at the target. See the
 * AlphaFunction restrictions section in
 * @c docs/layout-transition.md for the rationale.
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
  Duration      duration{0.3f};                    ///< Total duration of the progress sweep (default 300ms; should be positive)
  AlphaFunction alpha{AlphaFunction::EASE_IN_OUT}; ///< Shaping function applied to raw progress (BEZIER/SPRING fall back to linear in animator mode)
  Duration      delay;                             ///< Delay before progress starts to advance (only honoured when duration is positive)
};

/**
 * @brief Context passed to a layout animator callback on each frame.
 *
 * The framework allocates one context per (view, slot) pair while the
 * transition is in flight and updates @c progress / @c rawProgress every
 * frame. @c fromBounds / @c toBounds are captured at the start of the
 * transition and reset on interrupt.
 *
 * The callback should write to the view's @c Actor properties only
 * (POSITION_X/Y, SIZE_WIDTH/HEIGHT, OPACITY, SCALE etc.). It must not modify
 * the view tree, request layout, or rebind the LayoutTransition during the
 * callback.
 *
 * @note For ENTER and EXIT slots, @c fromBounds equals @c toBounds — the
 * layout system has already placed the child at its final position by
 * the time the animator runs, so the dispatcher has no concept of an
 * "enter from" or "exit to" position. The callback may write Actor
 * properties, including bounds properties, but any bounds override is
 * application-owned: the callback must provide the interpolation and
 * restore the desired final state. Only CHANGE animators see distinct
 * @c fromBounds (pre-pass) and @c toBounds (post-pass) from the
 * dispatcher.
 *
 * @note @c changeCause is meaningful only when @c slot == CHANGE.
 * For ENTER/EXIT it defaults to @c LayoutChangeCause::OTHER and
 * must not be branched on.
 */
struct DALI_UI_API LayoutAnimatorContext
{
  View                 view; ///< Target view handle. Safe to use during the callback. Do NOT capture by value into a long-lived closure — the dispatcher holds the LayoutTransition (which owns the callback), and the View holds the LayoutTransition, so a captured @c View forms a reference cycle. Use @c WeakHandle<View> if a captured reference is required.
  LayoutTransitionSlot slot;
  LayoutChangeCause    changeCause; ///< CHANGE only; @c OTHER for ENTER/EXIT
  /// Alpha-applied progress value passed to the animator callback.
  ///
  /// For the honoured built-in forward easing modes listed in the
  /// LayoutTransition alpha support table, this value follows the
  /// transition from 0 toward 1 as the transition advances.
  ///
  /// CUSTOM_FUNCTION output is application-owned and may produce values
  /// outside [0, 1], non-monotonic values, or reverse-shaped values.
  /// Do not use progress == 1.0 as a lifecycle completion signal; use
  /// SetOnFinished for completion, chaining, or view-tree mutation.
  float progress;

  /// Linear elapsed progress in [0, 1] before alpha.
  ///
  /// For positive duration, this remains 0 during delay, then advances
  /// monotonically from 0 toward 1 over the active duration and is
  /// clamped at 1 once the active duration has elapsed. For non-positive
  /// duration, the animator collapses to rawProgress = 1.0 on its first
  /// tick.
  ///
  /// rawProgress is useful as a per-frame linear signal. It is not the
  /// lifecycle completion contract; use SetOnFinished for completion,
  /// chaining, or view-tree mutation.
  float      rawProgress;
  LayoutRect fromBounds; ///< Bounds at slot entry (or last interrupt) — visual coords (RTL-mirrored on RTL parents)
  LayoutRect toBounds;   ///< Bounds the layout pass produced — visual coords (RTL-mirrored on RTL parents)
};

} // namespace Ui
} // namespace Dali
