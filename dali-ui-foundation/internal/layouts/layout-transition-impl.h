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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <array>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

class LayoutTransitionImpl;
using LayoutTransitionImplPtr = IntrusivePtr<LayoutTransitionImpl>;

/**
 * @brief Internal implementation of LayoutTransition.
 *
 * Owns the per-slot visual spec / bounds effect / change timing / animator
 * callback and the composition options.
 *
 * Storage policy:
 *  - Visual spec / bounds effect are opt-in (per-slot set flag).
 *  - Default CHANGE timing is opt-out: @c mChangeTimingEnabled starts true with
 *    @c LayoutTransitionTiming default values (0.3s EASE_IN_OUT).
 *  - Cause-specific CHANGE timing is per-cause set flag.
 *  - Animator is opt-in.
 *
 * Lookup precedence for CHANGE timing:
 *   cause-specific (if set) > default (if enabled) > no animation.
 *
 * State is read by the dispatcher (LayoutTransitionDispatcher) when
 * dispatching transitions on each frame.
 */
class DALI_UI_API LayoutTransitionImpl : public BaseObject
{
public:
  static constexpr size_t LAYOUT_CHANGE_CAUSE_COUNT = 5u; ///< 5 = enum count of LayoutChangeCause

  static LayoutTransitionImplPtr New();

  // ─── Visual property animation channel ──────────────────────────────────
  void SetEnterVisualSpec(ViewAnimationSpec spec);
  void SetExitVisualSpec(ViewAnimationSpec spec);
  void ClearEnterVisualSpec();
  void ClearExitVisualSpec();

  ViewAnimationSpec GetEnterVisualSpec() const;
  ViewAnimationSpec GetExitVisualSpec() const;

  // ─── Bounds effect channel ──────────────────────────────────────────────
  void SetEnterBoundsEffect(const LayoutBoundsEffect& effect);
  void SetExitBoundsEffect(const LayoutBoundsEffect& effect);
  void ClearEnterBoundsEffect();
  void ClearExitBoundsEffect();

  /// Whether a bounds effect has been registered via @c SetEnterBoundsEffect.
  /// Note: this is true even for a no-op effect (one whose offset and size
  /// factor leave the bounds unchanged). Validation runs against the
  /// *registered* effect; animation dispatch runs against the *active*
  /// effect (see @c HasActiveEnterBoundsEffect).
  bool IsEnterBoundsEffectSet() const;
  bool IsExitBoundsEffectSet() const;

  /// Whether a registered bounds effect would actually change the bounds.
  /// True iff the effect is set and is not a no-op (offset non-zero or
  /// size factor non-identity). Used by the dispatcher to decide whether
  /// to synthesise a bounds animation segment.
  bool HasActiveEnterBoundsEffect() const;
  bool HasActiveExitBoundsEffect() const;

  const LayoutBoundsEffect& GetEnterBoundsEffect() const;
  const LayoutBoundsEffect& GetExitBoundsEffect() const;

  // ─── CHANGE timing channel ──────────────────────────────────────────────
  void SetChangeTiming(const LayoutTransitionTiming& timing);
  void SetChangeTiming(LayoutChangeCause cause, const LayoutTransitionTiming& timing);
  void ClearChangeTiming();
  void ClearChangeTiming(LayoutChangeCause cause);

  /**
   * @brief Cause-specific → default-if-enabled fallback.
   *
   * Lookup order:
   *   1. cause-specific timing (if set) → out, return true
   *   2. default timing (if enabled)    → out, return true
   *   3. no animation                   → return false
   *
   * @param[in]  cause CHANGE cause
   * @param[out] out   Selected timing on success
   * @return true if a timing applies, false otherwise
   */
  bool TryGetChangeTiming(LayoutChangeCause cause, LayoutTransitionTiming& out) const;

  // ─── Animator channel ───────────────────────────────────────────────────
  void SetEnterAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing);
  void SetExitAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing);
  void SetChangeAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing);
  void ClearEnterAnimator();
  void ClearExitAnimator();
  void ClearChangeAnimator();

  bool                        HasEnterAnimator() const;
  bool                        HasExitAnimator() const;
  bool                        HasChangeAnimator() const;
  const LayoutAnimatorTiming& GetEnterAnimatorTiming() const;
  const LayoutAnimatorTiming& GetExitAnimatorTiming() const;
  const LayoutAnimatorTiming& GetChangeAnimatorTiming() const;

  /// Returns a pointer to the slot's callback, or nullptr when not set.
  /// Used by the layout transition dispatcher to invoke the callback each
  /// frame without copying the move-only handle.
  LayoutAnimatorCallback* GetEnterAnimatorCallback();
  LayoutAnimatorCallback* GetExitAnimatorCallback();
  LayoutAnimatorCallback* GetChangeAnimatorCallback();

  // ─── Composite slot-effect predicates ───────────────────────────────────

  /// Whether the ENTER slot has any dispatchable effect: a visual spec, an
  /// active (non-noop) bounds effect, or an animator. Used to decide whether
  /// a SUBTREE owner should reach an inherited descendant's ENTER, and by
  /// ViewImpl when deciding the ENTER path.
  bool HasEnterFx() const;

  /// Whether the EXIT slot has any dispatchable effect: a visual spec, an
  /// active (non-noop) bounds effect, or an animator. Mirrors the inline
  /// predicate ViewImpl::Remove uses to decide deferral, and is reused
  /// by the inherited (SUBTREE) EXIT routing.
  bool HasExitFx() const;

  // ─── Composition options ────────────────────────────────────────────────
  void SetChangeOnWindowResize(bool enable);
  bool GetChangeOnWindowResize() const;

  /// Opt in to firing ENTER for children present at the parent's very
  /// first arrange pass. Default is @c false: children in the initial
  /// layout are treated as the view's "already present" state and the
  /// dispatcher settles their
  /// declarative ENTER visual spec to its target values without firing
  /// lifecycle callbacks. Setting this to @c true restores firing ENTER
  /// on initial mount.
  void SetEnterOnInitialMount(bool enable);
  bool GetEnterOnInitialMount() const;

  /// Selects whether a CHANGE transition reaches only the attached view's
  /// direct children (default) or the whole subtree beneath it. See
  /// @c LayoutReflowScope.
  void              SetReflowScope(LayoutReflowScope scope);
  LayoutReflowScope GetReflowScope() const;

  // ─── Lifecycle ──────────────────────────────────────────────────────────
  void SetOnStart(LayoutLifecycleCallback callback);
  void SetOnFinished(LayoutLifecycleCallback callback);

  bool HasOnStart() const;
  bool HasOnFinished() const;

  /// Returns a pointer to the lifecycle callback, or nullptr when not set.
  /// Used by the layout transition dispatcher to invoke without copying the
  /// move-only handle.
  LayoutLifecycleCallback* GetOnStartCallback();
  LayoutLifecycleCallback* GetOnFinishedCallback();

protected:
  LayoutTransitionImpl();
  ~LayoutTransitionImpl() override;

private:
  LayoutTransitionImpl(const LayoutTransitionImpl&)            = delete;
  LayoutTransitionImpl(LayoutTransitionImpl&&)                 = delete;
  LayoutTransitionImpl& operator=(const LayoutTransitionImpl&) = delete;
  LayoutTransitionImpl& operator=(LayoutTransitionImpl&&)      = delete;

  // ─── Visual ──────────
  ViewAnimationSpec mEnterVisualSpec;
  ViewAnimationSpec mExitVisualSpec;

  // ─── Bounds effect ───
  LayoutBoundsEffect mEnterBoundsEffect;
  LayoutBoundsEffect mExitBoundsEffect;
  bool               mEnterBoundsEffectSet;
  bool               mExitBoundsEffectSet;

  // ─── CHANGE timing ───
  LayoutTransitionTiming mChangeTiming;        ///< Default value: 0.3s, EASE_IN_OUT
  bool                   mChangeTimingEnabled; ///< Default true (opt-out via ClearChangeTiming)

  std::array<LayoutTransitionTiming, LAYOUT_CHANGE_CAUSE_COUNT> mChangeTimingsByCause;
  std::array<bool, LAYOUT_CHANGE_CAUSE_COUNT>                   mChangeTimingByCauseSet;

  // ─── Animator ────────
  LayoutAnimatorCallback mEnterAnimator;
  LayoutAnimatorCallback mExitAnimator;
  LayoutAnimatorCallback mChangeAnimator;
  LayoutAnimatorTiming   mEnterAnimatorTiming;
  LayoutAnimatorTiming   mExitAnimatorTiming;
  LayoutAnimatorTiming   mChangeAnimatorTiming;
  bool                   mEnterAnimatorSet;
  bool                   mExitAnimatorSet;
  bool                   mChangeAnimatorSet;

  // ─── Options ─────────
  bool              mChangeOnWindowResize;
  bool              mEnterOnInitialMount;
  LayoutReflowScope mReflowScope;

  // ─── Lifecycle ───────
  LayoutLifecycleCallback mOnStart;
  LayoutLifecycleCallback mOnFinished;
};

inline LayoutTransitionImpl& GetImpl(LayoutTransition& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<LayoutTransitionImpl&>(handle);
}

inline const LayoutTransitionImpl& GetImpl(const LayoutTransition& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const LayoutTransitionImpl&>(handle);
}

/**
 * @brief Returns the index into per-cause arrays for the given cause.
 *
 * Aborts on out-of-range cause values.
 */
inline size_t LayoutChangeCauseIndex(LayoutChangeCause cause)
{
  const auto index = static_cast<size_t>(cause);
  DALI_ASSERT_ALWAYS(index < LayoutTransitionImpl::LAYOUT_CHANGE_CAUSE_COUNT &&
                     "LayoutChangeCause value out of range");
  return index;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
