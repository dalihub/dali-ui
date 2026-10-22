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
#include <dali-ui-foundation/internal/layouts/layout-transition-validation.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/animation/view-animation-spec-impl.autogen.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
constexpr const char* REVERSE_ALPHA_NOT_SUPPORTED =
  "AlphaFunction::REVERSE is not supported by LayoutTransition";
constexpr const char* BOUNDS_PROPERTY_NOT_SUPPORTED =
  "LayoutTransition visual spec entries must not target layout-owned bounds "
  "properties (POSITION_X/Y, SIZE_WIDTH/HEIGHT); use the bounds-effect channel";
constexpr const char* NON_TERMINAL_ALPHA_NOT_SUPPORTED =
  "AlphaFunction must end at the target value for LayoutTransition bounds "
  "animation";
constexpr const char* BOUNDS_EFFECT_NEGATIVE_SIZE_FACTOR =
  "LayoutBoundsEffect sizeFactor must be non-negative";
constexpr const char* BOUNDS_EFFECT_ANCHOR_OUT_OF_RANGE =
  "LayoutBoundsEffect anchor must be in [0, 1]";
constexpr const char* BOUNDS_EFFECT_NON_FINITE =
  "LayoutBoundsEffect sizeFactor, anchor, offset and timing must be finite";
constexpr const char* TIMING_NON_FINITE =
  "LayoutTransitionTiming duration and delay must be finite";
constexpr const char* ANIMATOR_TIMING_NON_FINITE =
  "LayoutAnimatorTiming duration and delay must be finite";

constexpr float BOUNDS_EFFECT_EPSILON = 1.0e-5f;

bool ApproxEqual(float a, float b)
{
  return std::fabs(a - b) <= BOUNDS_EFFECT_EPSILON;
}
} //namespace

bool IsReverseAlpha(const Dali::AlphaFunction& alpha)
{
  return alpha.GetMode() == Dali::AlphaFunction::BUILTIN_FUNCTION &&
         alpha.GetBuiltinFunction() == Dali::AlphaFunction::REVERSE;
}

void AbortIfReverseAlpha(const Dali::AlphaFunction& alpha)
{
  if(IsReverseAlpha(alpha))
  {
    DALI_ABORT(REVERSE_ALPHA_NOT_SUPPORTED);
  }
}

bool IsNonTerminalLayoutAlpha(const Dali::AlphaFunction& alpha)
{
  if(alpha.GetMode() != Dali::AlphaFunction::BUILTIN_FUNCTION)
  {
    return false;
  }
  const auto builtin = alpha.GetBuiltinFunction();
  return builtin == Dali::AlphaFunction::REVERSE ||
         builtin == Dali::AlphaFunction::BOUNCE ||
         builtin == Dali::AlphaFunction::SIN;
}

void AbortIfNonTerminalLayoutAlpha(const Dali::AlphaFunction& alpha)
{
  if(IsReverseAlpha(alpha))
  {
    DALI_ABORT(REVERSE_ALPHA_NOT_SUPPORTED);
  }
  if(IsNonTerminalLayoutAlpha(alpha))
  {
    DALI_ABORT(NON_TERMINAL_ALPHA_NOT_SUPPORTED);
  }
}

void AbortIfSpecHasReverseAlpha(const Dali::Ui::ViewAnimationSpec& spec)
{
  if(spec && GetImpl(spec).ContainsReverseAlpha())
  {
    DALI_ABORT(REVERSE_ALPHA_NOT_SUPPORTED);
  }
}

void AbortIfSpecHasLayoutBoundsProperty(const Dali::Ui::ViewAnimationSpec& spec)
{
  if(spec && GetImpl(spec).ContainsLayoutBoundsProperty())
  {
    DALI_ABORT(BOUNDS_PROPERTY_NOT_SUPPORTED);
  }
}

void AbortIfNonFiniteTiming(const Dali::Ui::LayoutTransitionTiming& timing)
{
  // Same reasoning as the bounds-effect gate below: a relational test cannot reject a
  // NaN (`duration <= 0` is false for it), so the duration-zero opt-out lets it through
  // into Animation::New and TimePeriod.
  if(!std::isfinite(timing.GetDuration().InSeconds()) || !std::isfinite(timing.GetDelay().InSeconds()))
  {
    DALI_ABORT(TIMING_NON_FINITE);
  }
}

void AbortIfNonFiniteAnimatorTiming(const Dali::Ui::LayoutAnimatorTiming& timing)
{
  if(!std::isfinite(timing.GetDuration().InSeconds()) || !std::isfinite(timing.GetDelay().InSeconds()))
  {
    DALI_ABORT(ANIMATOR_TIMING_NON_FINITE);
  }
}

void AbortIfInvalidBoundsEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  AbortIfNonTerminalLayoutAlpha(effect.GetTiming().GetAlpha());
  const LayoutBoundsOffset& offset = effect.GetOffset();

  // Checked BEFORE the range tests below, because a NaN silently PASSES every
  // relational test (`NaN < 0` and `NaN > 1` are both false) and would then be
  // composed into the endpoint by ComputeBoundsEndpoint, producing a NaN rect
  // that propagates into the arranged geometry of the animated subtree. An
  // infinite descriptor does the same, and a non-finite duration/delay reaches
  // Animation::SetDuration. Registration is the last point at which the value
  // can still be reported to the caller, so every descriptor the endpoint maths
  // reads is required to be finite here.
  if(!std::isfinite(effect.GetSizeFactorX()) || !std::isfinite(effect.GetSizeFactorY()) ||
     !std::isfinite(effect.GetAnchorX()) || !std::isfinite(effect.GetAnchorY()) ||
     !std::isfinite(effect.GetTiming().GetDuration().InSeconds()) ||
     !std::isfinite(effect.GetTiming().GetDelay().InSeconds()) ||
     (effect.HasOffset() && (!std::isfinite(offset.x.value) ||
                             !std::isfinite(offset.y.value))))
  {
    DALI_ABORT(BOUNDS_EFFECT_NON_FINITE);
  }

  if(effect.GetSizeFactorX() < 0.0f || effect.GetSizeFactorY() < 0.0f)
  {
    DALI_ABORT(BOUNDS_EFFECT_NEGATIVE_SIZE_FACTOR);
  }

  if(effect.GetAnchorX() < 0.0f || effect.GetAnchorX() > 1.0f ||
     effect.GetAnchorY() < 0.0f || effect.GetAnchorY() > 1.0f)
  {
    DALI_ABORT(BOUNDS_EFFECT_ANCHOR_OUT_OF_RANGE);
  }
}

bool IsNoopBoundsEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  const LayoutBoundsOffset& offset       = effect.GetOffset();
  const bool                offsetIsZero = !effect.HasOffset() ||
                            (ApproxEqual(offset.x.value, 0.0f) &&
                             ApproxEqual(offset.y.value, 0.0f));

  const bool sizeIsIdentity = !effect.HasSizeFactor() ||
                              (ApproxEqual(effect.GetSizeFactorX(), 1.0f) &&
                               ApproxEqual(effect.GetSizeFactorY(), 1.0f));

  return offsetIsZero && sizeIsIdentity;
}

bool HasTimedSizeEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  if(effect.GetTiming().GetDuration().InSeconds() <= 0.0f)
  {
    return false;
  }
  if(!effect.HasSizeFactor())
  {
    return false;
  }
  return !ApproxEqual(effect.GetSizeFactorX(), 1.0f) ||
         !ApproxEqual(effect.GetSizeFactorY(), 1.0f);
}

bool IsTimedEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  return effect.GetTiming().GetDuration().InSeconds() > 0.0f &&
         !IsNoopBoundsEffect(effect);
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
