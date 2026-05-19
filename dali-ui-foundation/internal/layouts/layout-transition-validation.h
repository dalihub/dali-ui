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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Returns true if the alpha function is AlphaFunction::REVERSE.
 */
bool IsReverseAlpha(const Dali::AlphaFunction& alpha);

/**
 * @brief Aborts with a LayoutTransition policy message if @p alpha is REVERSE.
 */
void AbortIfReverseAlpha(const Dali::AlphaFunction& alpha);

/**
 * @brief Returns true when @p alpha cannot end a layout-bounds animation at
 * its target value.
 */
bool IsNonTerminalLayoutAlpha(const Dali::AlphaFunction& alpha);

/**
 * @brief Aborts when @p alpha is invalid for CHANGE / bounds-effect timing.
 */
void AbortIfNonTerminalLayoutAlpha(const Dali::AlphaFunction& alpha);

/**
 * @brief Aborts with a LayoutTransition policy message if @p spec contains
 * any entry whose alpha is REVERSE. No-op if @p spec is uninitialized.
 */
void AbortIfSpecHasReverseAlpha(const Dali::Ui::ViewAnimationSpec& spec);

/**
 * @brief Aborts if @p spec contains an entry targeting a layout-owned
 * bounds property (POSITION_X/Y, SIZE_WIDTH/HEIGHT).
 *
 * ENTER / EXIT visual specs are for non-bounds properties (opacity, scale,
 * color, corner, borderline, etc.). Bounds belong to the bounds-effect
 * channel so the dispatcher can compose layout-driven positions with
 * application-declared slide / expand / shrink offsets in a single
 * @c Animation. Allowing a visual spec to drive bounds directly would
 * race the bounds channel and produce incoherent end states.
 *
 * No-op if @p spec is uninitialized.
 */
void AbortIfSpecHasLayoutBoundsProperty(const Dali::Ui::ViewAnimationSpec& spec);

/**
 * @brief Aborts if @p effect contains a value rejected by LayoutTransition
 * (REVERSE/BOUNCE/SIN alpha, negative size factor, anchor out of @c [0,1]).
 *
 * Validation runs against the registered effect, before any noop check —
 * an invalid effect is rejected even if it would otherwise be a no-op.
 */
void AbortIfInvalidBoundsEffect(const Dali::Ui::LayoutBoundsEffect& effect);

/**
 * @brief Returns true if @p effect leaves the bounds unchanged.
 *
 * A no-op effect either has no offset / size factor flags set, or has all
 * offset values zero and size factor identity. The dispatcher must not
 * synthesise an animation segment for a no-op effect.
 */
bool IsNoopBoundsEffect(const Dali::Ui::LayoutBoundsEffect& effect);

/**
 * @brief Returns true if @p effect has a positive duration and an
 * active (non-identity) size factor.
 *
 * Used by the dispatcher's @c AUTO clip-mode policy: transient clipping is
 * applied automatically only when the effect changes the view's size.
 * Offset-only effects render outside the layout-arranged region on
 * purpose and are not auto-clipped.
 */
bool HasTimedSizeEffect(const Dali::Ui::LayoutBoundsEffect& effect);

/**
 * @brief Returns true if @p effect would produce a visible timed animation.
 *
 * True iff @c timing.duration > 0 AND @c !IsNoopBoundsEffect(effect).
 * The non-noop check is required so that an effect with @c hasOffset set
 * to true but every offset value zero (i.e. a logical no-op) is not
 * mistaken for a timed effect.
 */
bool IsTimedEffect(const Dali::Ui::LayoutBoundsEffect& effect);

} // namespace Internal
} // namespace Ui
} // namespace Dali
