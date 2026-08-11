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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
/**
 * @brief The window in which one invalidation's walk to the layout root can stand in
 * for the next one's.
 *
 * `InvalidateMeasure()` / `InvalidateArrange()` do two separable things: they record
 * local state on the view they are called on, and they walk the ancestor chain to
 * register a layout root with the LayoutController. The local half must run every
 * time. The walk is idempotent -- each level sets its own flags and the controller
 * coalesces duplicate registrations into a pending set -- so repeating it once the
 * root is already registered is pure cost, and it is not small: every level performs
 * two Actor::GetParent() calls and two handle DownCasts, and the root additionally
 * resolves its Window and re-registers with the UI scale manager. An application that
 * touches a few hundred views before the next layout pass pays all of that per touch.
 *
 * A generation is what lets the second and subsequent walks be skipped SAFELY. Each view
 * records the generation in which its walk completed; a later invalidation may skip the
 * walk only while that record still matches the current generation, which is exactly the
 * window in which "the root is registered and has not been processed yet" holds.
 *
 * @note This deliberately does NOT reintroduce the dirty-flag short-circuit that used
 * to live in InvalidateMeasure(). That one asked "is this view already dirty?", which
 * a view whose parent never arranges it answers "yes" forever, swallowing every later
 * invalidation. This asks "has the registration this walk would make already been
 * made and not yet consumed?", and the answer goes false again at the next drain.
 *
 * @note Event thread only, and not thread-safe: every reader and writer runs on the
 * event thread, in the same synchronous call graph as the layout pass itself.
 */
namespace LayoutInvalidation
{
/**
 * @brief Returns the current invalidation generation.
 *
 * Never returns 0, so 0 is usable as a "never propagated" initial value.
 *
 * @return The current generation
 */
DALI_UI_API uint32_t CurrentGeneration();

/**
 * @brief Ends the current generation and starts a new one.
 *
 * Called wherever a record could otherwise outlive the state its walk established:
 *
 *  - by the LayoutController when a pass drains the pending set, and when a root is
 *    dropped from it without being processed (the REGISTRATION half of the record's
 *    claim stops holding);
 *  - by the outermost Measure/Arrange pass guard on exit (the MARKING half: a pass is
 *    the only consumer of dirty bits, and a manual Measure()/Arrange() on an ancestor
 *    -- both public API -- can consume a walked chain's dirty without any drain).
 *
 * Every recorded generation becomes stale at that moment, so the next invalidation on any
 * view walks and re-registers in full. Cache hits construct no pass guard, so a
 * settled tree bumps nothing.
 *
 * Wrap-around is nearly harmless and self-healing: a stale record can only collide
 * with the current generation after 2^32 bumps, a collision merely coalesces one walk that
 * should have run, and the next pass re-ends the generation. Skipping 0 on wrap keeps the
 * "never propagated" value unreachable.
 */
DALI_UI_API void AdvanceGeneration();

} // namespace LayoutInvalidation
} // namespace Internal
} // namespace Ui
} // namespace Dali
