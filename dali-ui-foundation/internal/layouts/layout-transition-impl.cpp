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
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-transition-validation.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

namespace
{
constexpr float DEFAULT_CHANGE_DURATION_SEC = 0.3f;
constexpr float DEFAULT_ENTER_DURATION_SEC  = 0.3f;
constexpr float DEFAULT_EXIT_DURATION_SEC   = 0.2f;
} // namespace

LayoutTransitionImplPtr LayoutTransitionImpl::New()
{
  return LayoutTransitionImplPtr(new LayoutTransitionImpl());
}

LayoutTransitionImpl::LayoutTransitionImpl()
: mEnterBoundsEffectSet(false),
  mExitBoundsEffectSet(false),
  mChangeTiming{Duration(DEFAULT_CHANGE_DURATION_SEC), AlphaFunction(AlphaFunction::EASE_IN_OUT), Duration()},
  mChangeTimingEnabled(true),
  mChangeTimingByCauseSet{},
  mEnterAnimatorTiming{Duration(DEFAULT_ENTER_DURATION_SEC), AlphaFunction(AlphaFunction::EASE_OUT), Duration()},
  mExitAnimatorTiming{Duration(DEFAULT_EXIT_DURATION_SEC), AlphaFunction(AlphaFunction::EASE_IN), Duration()},
  mChangeAnimatorTiming{Duration(DEFAULT_CHANGE_DURATION_SEC), AlphaFunction(AlphaFunction::EASE_IN_OUT), Duration()},
  mEnterAnimatorSet(false),
  mExitAnimatorSet(false),
  mChangeAnimatorSet(false),
  mChangeOnWindowResize(false),
  mEnterOnInitialMount(false)
{
}

LayoutTransitionImpl::~LayoutTransitionImpl() = default;

// ─── Visual property animation channel ───────────────────────────────────────

void LayoutTransitionImpl::SetEnterVisualSpec(ViewAnimationSpec spec)
{
  AbortIfSpecHasReverseAlpha(spec);
  AbortIfSpecHasLayoutBoundsProperty(spec);
  mEnterVisualSpec = spec;
}

void LayoutTransitionImpl::SetExitVisualSpec(ViewAnimationSpec spec)
{
  AbortIfSpecHasReverseAlpha(spec);
  AbortIfSpecHasLayoutBoundsProperty(spec);
  mExitVisualSpec = spec;
}

void LayoutTransitionImpl::ClearEnterVisualSpec()
{
  mEnterVisualSpec = ViewAnimationSpec();
}

void LayoutTransitionImpl::ClearExitVisualSpec()
{
  mExitVisualSpec = ViewAnimationSpec();
}

ViewAnimationSpec LayoutTransitionImpl::GetEnterVisualSpec() const
{
  return mEnterVisualSpec;
}

ViewAnimationSpec LayoutTransitionImpl::GetExitVisualSpec() const
{
  return mExitVisualSpec;
}

// ─── Bounds effect channel ───────────────────────────────────────────────────

void LayoutTransitionImpl::SetEnterBoundsEffect(const LayoutBoundsEffect& effect)
{
  AbortIfInvalidBoundsEffect(effect);
  mEnterBoundsEffect    = effect;
  mEnterBoundsEffectSet = true;
}

void LayoutTransitionImpl::SetExitBoundsEffect(const LayoutBoundsEffect& effect)
{
  AbortIfInvalidBoundsEffect(effect);
  mExitBoundsEffect    = effect;
  mExitBoundsEffectSet = true;
}

void LayoutTransitionImpl::ClearEnterBoundsEffect()
{
  mEnterBoundsEffect    = LayoutBoundsEffect{};
  mEnterBoundsEffectSet = false;
}

void LayoutTransitionImpl::ClearExitBoundsEffect()
{
  mExitBoundsEffect    = LayoutBoundsEffect{};
  mExitBoundsEffectSet = false;
}

bool LayoutTransitionImpl::IsEnterBoundsEffectSet() const
{
  return mEnterBoundsEffectSet;
}

bool LayoutTransitionImpl::IsExitBoundsEffectSet() const
{
  return mExitBoundsEffectSet;
}

bool LayoutTransitionImpl::HasActiveEnterBoundsEffect() const
{
  return mEnterBoundsEffectSet && !IsNoopBoundsEffect(mEnterBoundsEffect);
}

bool LayoutTransitionImpl::HasActiveExitBoundsEffect() const
{
  return mExitBoundsEffectSet && !IsNoopBoundsEffect(mExitBoundsEffect);
}

const LayoutBoundsEffect& LayoutTransitionImpl::GetEnterBoundsEffect() const
{
  return mEnterBoundsEffect;
}

const LayoutBoundsEffect& LayoutTransitionImpl::GetExitBoundsEffect() const
{
  return mExitBoundsEffect;
}

// ─── CHANGE timing channel ───────────────────────────────────────────────────

void LayoutTransitionImpl::SetChangeTiming(const LayoutTransitionTiming& timing)
{
  AbortIfNonTerminalLayoutAlpha(timing.alpha);
  mChangeTiming        = timing;
  mChangeTimingEnabled = true;
}

void LayoutTransitionImpl::SetChangeTiming(LayoutChangeCause             cause,
                                           const LayoutTransitionTiming& timing)
{
  AbortIfNonTerminalLayoutAlpha(timing.alpha);
  const size_t index             = LayoutChangeCauseIndex(cause);
  mChangeTimingsByCause[index]   = timing;
  mChangeTimingByCauseSet[index] = true;
}

void LayoutTransitionImpl::ClearChangeTiming()
{
  mChangeTimingEnabled = false;
  // mChangeTiming value is preserved — a subsequent SetChangeTiming() overwrites it.
}

void LayoutTransitionImpl::ClearChangeTiming(LayoutChangeCause cause)
{
  const size_t index             = LayoutChangeCauseIndex(cause);
  mChangeTimingByCauseSet[index] = false;
  // mChangeTimingsByCause[index] value is preserved.
}

bool LayoutTransitionImpl::TryGetChangeTiming(LayoutChangeCause       cause,
                                              LayoutTransitionTiming& out) const
{
  const size_t index = LayoutChangeCauseIndex(cause);

  // 1. cause-specific override
  if(mChangeTimingByCauseSet[index])
  {
    out = mChangeTimingsByCause[index];
    return true;
  }

  // 2. default if enabled
  if(mChangeTimingEnabled)
  {
    out = mChangeTiming;
    return true;
  }

  // 3. no animation
  return false;
}

// ─── Animator channel ────────────────────────────────────────────────────────

void LayoutTransitionImpl::SetEnterAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing)
{
  AbortIfReverseAlpha(timing.alpha);
  mEnterAnimator       = std::move(callback);
  mEnterAnimatorTiming = timing;
  mEnterAnimatorSet    = static_cast<bool>(mEnterAnimator);
}

void LayoutTransitionImpl::SetExitAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing)
{
  AbortIfReverseAlpha(timing.alpha);
  mExitAnimator       = std::move(callback);
  mExitAnimatorTiming = timing;
  mExitAnimatorSet    = static_cast<bool>(mExitAnimator);
}

void LayoutTransitionImpl::SetChangeAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing)
{
  AbortIfReverseAlpha(timing.alpha);
  mChangeAnimator       = std::move(callback);
  mChangeAnimatorTiming = timing;
  mChangeAnimatorSet    = static_cast<bool>(mChangeAnimator);
}

void LayoutTransitionImpl::ClearEnterAnimator()
{
  mEnterAnimator    = LayoutAnimatorCallback();
  mEnterAnimatorSet = false;
}

void LayoutTransitionImpl::ClearExitAnimator()
{
  mExitAnimator    = LayoutAnimatorCallback();
  mExitAnimatorSet = false;
}

void LayoutTransitionImpl::ClearChangeAnimator()
{
  mChangeAnimator    = LayoutAnimatorCallback();
  mChangeAnimatorSet = false;
}

bool LayoutTransitionImpl::HasEnterAnimator() const
{
  return mEnterAnimatorSet;
}

bool LayoutTransitionImpl::HasExitAnimator() const
{
  return mExitAnimatorSet;
}

bool LayoutTransitionImpl::HasChangeAnimator() const
{
  return mChangeAnimatorSet;
}

const LayoutAnimatorTiming& LayoutTransitionImpl::GetEnterAnimatorTiming() const
{
  return mEnterAnimatorTiming;
}

const LayoutAnimatorTiming& LayoutTransitionImpl::GetExitAnimatorTiming() const
{
  return mExitAnimatorTiming;
}

const LayoutAnimatorTiming& LayoutTransitionImpl::GetChangeAnimatorTiming() const
{
  return mChangeAnimatorTiming;
}

LayoutAnimatorCallback* LayoutTransitionImpl::GetEnterAnimatorCallback()
{
  return mEnterAnimatorSet ? &mEnterAnimator : nullptr;
}

LayoutAnimatorCallback* LayoutTransitionImpl::GetExitAnimatorCallback()
{
  return mExitAnimatorSet ? &mExitAnimator : nullptr;
}

LayoutAnimatorCallback* LayoutTransitionImpl::GetChangeAnimatorCallback()
{
  return mChangeAnimatorSet ? &mChangeAnimator : nullptr;
}

// ─── Composition options ─────────────────────────────────────────────────────

void LayoutTransitionImpl::SetChangeOnWindowResize(bool enable)
{
  mChangeOnWindowResize = enable;
}

bool LayoutTransitionImpl::GetChangeOnWindowResize() const
{
  return mChangeOnWindowResize;
}

void LayoutTransitionImpl::SetEnterOnInitialMount(bool enable)
{
  mEnterOnInitialMount = enable;
}

bool LayoutTransitionImpl::GetEnterOnInitialMount() const
{
  return mEnterOnInitialMount;
}

// ─── Lifecycle ───────────────────────────────────────────────────────────────

void LayoutTransitionImpl::SetOnStart(LayoutLifecycleCallback callback)
{
  mOnStart = std::move(callback);
}

void LayoutTransitionImpl::SetOnFinished(LayoutLifecycleCallback callback)
{
  mOnFinished = std::move(callback);
}

bool LayoutTransitionImpl::HasOnStart() const
{
  return static_cast<bool>(mOnStart);
}

bool LayoutTransitionImpl::HasOnFinished() const
{
  return static_cast<bool>(mOnFinished);
}

LayoutLifecycleCallback* LayoutTransitionImpl::GetOnStartCallback()
{
  return mOnStart ? &mOnStart : nullptr;
}

LayoutLifecycleCallback* LayoutTransitionImpl::GetOnFinishedCallback()
{
  return mOnFinished ? &mOnFinished : nullptr;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
