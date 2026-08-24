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
#include <dali-ui-foundation/internal/layouts/layout-invalidation-generation.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace LayoutInvalidation
{
namespace
{
/// The current generation. Starts at 1 so that 0 can mean "never propagated" in every
/// view's record without a separate flag. A plain global with a constant initializer:
/// no dynamic init, no thread-local slot (the event thread is the only user).
///
/// Deliberately ONE counter for the whole process rather than one per window. Bumping
/// it is what makes every view's record stale, and a view's record is compared without
/// reference to which window it belongs to, so a per-window counter would have to be
/// resolved per comparison -- for a saving that only exists in the multi-window case
/// and only lets one window's drain leave another window's coalescing intact. Sharing
/// one counter over-invalidates across windows, which costs an extra walk and can
/// never cost correctness.
uint32_t gGeneration = 1u;

/// How many LayoutFinished emits are currently on the stack.
///
/// Thread-local for the same reason as the layout pass depth it partners with: the
/// emit runs synchronously on the event thread, inside the same call graph as the
/// Process(post) frame that opened it, so a per-thread counter is the accurate
/// description of "is an emit on MY stack". A counter rather than a bool because a
/// slot may drive a nested settle whose emit re-enters this scope; the window must
/// stay open until the OUTERMOST emit unwinds.
thread_local uint32_t gLayoutFinishedEmitDepth = 0u;

} // namespace

uint32_t CurrentGeneration()
{
  return gGeneration;
}

void AdvanceGeneration()
{
  // Skip 0 on wrap so it keeps meaning "never propagated".
  if(++gGeneration == 0u)
  {
    gGeneration = 1u;
  }
}

bool IsLayoutFinishedEmitInProgress()
{
  return gLayoutFinishedEmitDepth != 0u;
}

void BeginLayoutFinishedEmit()
{
  ++gLayoutFinishedEmitDepth;
}

void EndLayoutFinishedEmit()
{
  --gLayoutFinishedEmitDepth;
}

} // namespace LayoutInvalidation
} // namespace Internal
} // namespace Ui
} // namespace Dali
