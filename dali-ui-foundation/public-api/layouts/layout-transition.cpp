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
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>

namespace Dali
{
namespace Ui
{

LayoutTransition::LayoutTransition() = default;

LayoutTransition::LayoutTransition(const LayoutTransition& other) = default;

LayoutTransition::LayoutTransition(LayoutTransition&& rhs) noexcept = default;

LayoutTransition::~LayoutTransition() = default;

LayoutTransition& LayoutTransition::operator=(const LayoutTransition& other) = default;

LayoutTransition& LayoutTransition::operator=(LayoutTransition&& rhs) noexcept = default;

LayoutTransition LayoutTransition::New()
{
  Internal::LayoutTransitionImplPtr p = Internal::LayoutTransitionImpl::New();
  return LayoutTransition(p.Get());
}

LayoutTransition LayoutTransition::DownCast(BaseHandle handle)
{
  return LayoutTransition(dynamic_cast<Internal::LayoutTransitionImpl*>(handle.GetObjectPtr()));
}

LayoutTransition::LayoutTransition(Internal::LayoutTransitionImpl* impl)
: BaseHandle(impl)
{
}

// ─── Visual property animation channel ───────────────────────────────────────

LayoutTransition& LayoutTransition::SetEnterVisualSpec(ViewAnimationSpec spec)
{
  Internal::GetImpl(*this).SetEnterVisualSpec(spec);
  return *this;
}

LayoutTransition& LayoutTransition::SetExitVisualSpec(ViewAnimationSpec spec)
{
  Internal::GetImpl(*this).SetExitVisualSpec(spec);
  return *this;
}

LayoutTransition& LayoutTransition::ClearEnterVisualSpec()
{
  Internal::GetImpl(*this).ClearEnterVisualSpec();
  return *this;
}

LayoutTransition& LayoutTransition::ClearExitVisualSpec()
{
  Internal::GetImpl(*this).ClearExitVisualSpec();
  return *this;
}

// ─── Bounds effect channel ───────────────────────────────────────────────────

LayoutTransition& LayoutTransition::SetEnterBoundsEffect(const LayoutBoundsEffect& effect)
{
  Internal::GetImpl(*this).SetEnterBoundsEffect(effect);
  return *this;
}

LayoutTransition& LayoutTransition::SetExitBoundsEffect(const LayoutBoundsEffect& effect)
{
  Internal::GetImpl(*this).SetExitBoundsEffect(effect);
  return *this;
}

LayoutTransition& LayoutTransition::ClearEnterBoundsEffect()
{
  Internal::GetImpl(*this).ClearEnterBoundsEffect();
  return *this;
}

LayoutTransition& LayoutTransition::ClearExitBoundsEffect()
{
  Internal::GetImpl(*this).ClearExitBoundsEffect();
  return *this;
}

// ─── CHANGE timing channel ───────────────────────────────────────────────────

LayoutTransition& LayoutTransition::SetChangeTiming(const LayoutTransitionTiming& timing)
{
  Internal::GetImpl(*this).SetChangeTiming(timing);
  return *this;
}

LayoutTransition& LayoutTransition::SetChangeTiming(LayoutChangeCause             cause,
                                                    const LayoutTransitionTiming& timing)
{
  Internal::GetImpl(*this).SetChangeTiming(cause, timing);
  return *this;
}

LayoutTransition& LayoutTransition::ClearChangeTiming()
{
  Internal::GetImpl(*this).ClearChangeTiming();
  return *this;
}

LayoutTransition& LayoutTransition::ClearChangeTiming(LayoutChangeCause cause)
{
  Internal::GetImpl(*this).ClearChangeTiming(cause);
  return *this;
}

// ─── Animator channel ────────────────────────────────────────────────────────

LayoutTransition& LayoutTransition::SetEnterAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing)
{
  Internal::GetImpl(*this).SetEnterAnimator(std::move(callback), timing);
  return *this;
}

LayoutTransition& LayoutTransition::SetExitAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing)
{
  Internal::GetImpl(*this).SetExitAnimator(std::move(callback), timing);
  return *this;
}

LayoutTransition& LayoutTransition::SetChangeAnimator(LayoutAnimatorCallback callback, const LayoutAnimatorTiming& timing)
{
  Internal::GetImpl(*this).SetChangeAnimator(std::move(callback), timing);
  return *this;
}

LayoutTransition& LayoutTransition::ClearEnterAnimator()
{
  Internal::GetImpl(*this).ClearEnterAnimator();
  return *this;
}

LayoutTransition& LayoutTransition::ClearExitAnimator()
{
  Internal::GetImpl(*this).ClearExitAnimator();
  return *this;
}

LayoutTransition& LayoutTransition::ClearChangeAnimator()
{
  Internal::GetImpl(*this).ClearChangeAnimator();
  return *this;
}

// ─── Composition options ─────────────────────────────────────────────────────

LayoutTransition& LayoutTransition::SetChangeOnWindowResize(bool enable)
{
  Internal::GetImpl(*this).SetChangeOnWindowResize(enable);
  return *this;
}

LayoutTransition& LayoutTransition::SetEnterOnInitialMount(bool enable)
{
  Internal::GetImpl(*this).SetEnterOnInitialMount(enable);
  return *this;
}

LayoutTransition& LayoutTransition::SetReflowScope(LayoutReflowScope scope)
{
  Internal::GetImpl(*this).SetReflowScope(scope);
  return *this;
}

// ─── Lifecycle ───────────────────────────────────────────────────────────────

LayoutTransition& LayoutTransition::SetOnStart(LayoutLifecycleCallback callback)
{
  Internal::GetImpl(*this).SetOnStart(std::move(callback));
  return *this;
}

LayoutTransition& LayoutTransition::SetOnFinished(LayoutLifecycleCallback callback)
{
  Internal::GetImpl(*this).SetOnFinished(std::move(callback));
  return *this;
}

} // namespace Ui
} // namespace Dali
