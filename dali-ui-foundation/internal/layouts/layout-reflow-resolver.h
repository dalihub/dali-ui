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
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/internal/layouts/layout-transition-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>
#include <dali-ui-foundation/public-api/layouts/layout-transition.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Which inherited (SUBTREE-scope) slot a reflow resolution targets.
 *
 * CHANGE is resolved inline by the dispatcher's per-pass capture and is not
 * routed through this resolver; only the structural-event slots (ENTER on
 * child-add, EXIT on child-remove) walk the ancestor chain.
 */
enum class ReflowSlot
{
  ENTER,
  EXIT
};

/**
 * @brief Finds the closest ancestor that governs a child placed directly under
 * @p start via a SUBTREE-scope LayoutTransition carrying the requested slot
 * effect.
 *
 * The walk encodes the same governed-subtree boundary as
 * @c LayoutTransitionDispatcher::CaptureGovernedChildren (INV-BOUNDARY):
 *
 *  - The CLOSEST transition-bearing node (at or above @p start) is the owner of
 *    the child. It reaches a child placed directly under @p start only when its
 *    scope is @c SUBTREE and it carries the requested slot effect; otherwise the
 *    child is governed directly by that closer node and no inherited dispatch
 *    happens (returns @c nullptr).
 *  - A node with no transition must be a non-standalone container for an ancestor
 *    SUBTREE owner to descend through it. A standalone layout-mode boundary stops
 *    the scope (returns @c nullptr).
 *
 * @note When @p start itself carries a transition that lacks the requested slot
 * effect, the function returns @c nullptr — @p start is the closest owner and an
 * ancestor SUBTREE scope must not cross it. Callers therefore do not need to
 * pre-check @p start; the direct-parent-precedence invariant is enforced here.
 *
 * @param[in] start The child's direct parent (the container the child was added
 *                  to / removed from)
 * @param[in] slot  The structural slot being resolved (ENTER or EXIT)
 * @return The governing owner ViewImpl, or @c nullptr when no SUBTREE owner with
 *         the requested slot effect governs the child
 */
inline ViewImpl* FindGoverningSubtreeOwner(ViewImpl* start, ReflowSlot slot)
{
  ViewImpl* node = start;
  while(node)
  {
    Ui::LayoutTransition transition = node->GetLayoutTransition();
    if(transition)
    {
      // Closest transition-bearing node: it governs the child. It only reaches a
      // child placed directly under `start` when SUBTREE-scoped and carrying the
      // requested slot effect.
      LayoutTransitionImpl& impl    = GetImpl(transition);
      const bool            subtree = impl.GetReflowScope() == LayoutReflowScope::SUBTREE;
      const bool            hasFx   = (slot == ReflowSlot::ENTER) ? impl.HasEnterFx() : impl.HasExitFx();
      return (subtree && hasFx) ? node : nullptr;
    }

    // No transition here: an ancestor SUBTREE owner descends through this node
    // only if it is not a standalone layout root (matches CaptureGovernedChildren).
    if(IntegrationView::IsLayoutModeStandalone(*node))
    {
      return nullptr;
    }

    // Ascend to the parent View; a non-View parent (layer/window root) ends the walk.
    Dali::Actor parentActor = node->Self().GetParent();
    Ui::View    parentView  = parentActor ? Ui::View::DownCast(parentActor) : Ui::View();
    node                    = parentView ? &GetImpl(parentView) : nullptr;
  }
  return nullptr;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
