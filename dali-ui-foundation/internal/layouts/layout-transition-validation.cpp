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

namespace Dali
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

void AbortIfInvalidBoundsEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  AbortIfNonTerminalLayoutAlpha(effect.timing.alpha);

  if(effect.sizeFactorX < 0.0f || effect.sizeFactorY < 0.0f)
  {
    DALI_ABORT(BOUNDS_EFFECT_NEGATIVE_SIZE_FACTOR);
  }

  if(effect.anchorX < 0.0f || effect.anchorX > 1.0f ||
     effect.anchorY < 0.0f || effect.anchorY > 1.0f)
  {
    DALI_ABORT(BOUNDS_EFFECT_ANCHOR_OUT_OF_RANGE);
  }
}

bool IsNoopBoundsEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  const bool offsetIsZero = !effect.hasOffset ||
                            (ApproxEqual(effect.offset.x.value, 0.0f) &&
                             ApproxEqual(effect.offset.y.value, 0.0f));

  const bool sizeIsIdentity = !effect.hasSizeFactor ||
                              (ApproxEqual(effect.sizeFactorX, 1.0f) &&
                               ApproxEqual(effect.sizeFactorY, 1.0f));

  return offsetIsZero && sizeIsIdentity;
}

bool HasTimedSizeEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  if(effect.timing.duration.InSeconds() <= 0.0f)
  {
    return false;
  }
  if(!effect.hasSizeFactor)
  {
    return false;
  }
  return !ApproxEqual(effect.sizeFactorX, 1.0f) ||
         !ApproxEqual(effect.sizeFactorY, 1.0f);
}

bool IsTimedEffect(const Dali::Ui::LayoutBoundsEffect& effect)
{
  return effect.timing.duration.InSeconds() > 0.0f &&
         !IsNoopBoundsEffect(effect);
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
