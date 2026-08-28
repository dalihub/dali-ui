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
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>

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
 *  - A node whose @c LayoutTransitionMode is @c ISOLATE_SUBTREE stops the walk
 *    before its own transition is even read: the cut applies to owners at or
 *    above the gate, itself included.
 *
 * @note When @p start itself carries a transition that lacks the requested slot
 * effect, the function returns @c nullptr — @p start is the closest owner and an
 * ancestor SUBTREE scope must not cross it. Callers therefore do not need to
 * pre-check @p start; the direct-parent-precedence invariant is enforced here.
 *
 * @note This is level 4 of @c ResolveGoverningTransition; a child that carries its
 * own transition is claimed at the self level and never reaches this walk.
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
    // Governance cut, checked BEFORE this node's own transition: ISOLATE_SUBTREE
    // stops every owner AT OR ABOVE the gate node from governing anything inside
    // its subtree, and the gate node's own children-role transition is one of
    // them. Reached both when `start` IS the gate (its children lose it) and when
    // an ancestor is (the scope may not cross it).
    if(node->GetLayoutTransitionMode() == Ui::LayoutTransitionMode::ISOLATE_SUBTREE)
    {
      return nullptr;
    }

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
    if(Integration::View::IsLayoutModeStandalone(*node))
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

/**
 * @brief Which attachment point supplies the transition governing a child.
 */
enum class LayoutTransitionRole
{
  NONE,             ///< Nothing governs the child for this slot
  SELF,             ///< The child's own transition (View::SetSelfLayoutTransition)
  DIRECT_PARENT,    ///< The direct parent's transition (View::SetLayoutTransition)
  INHERITED_SUBTREE ///< A SUBTREE-scope ancestor's transition
};

/**
 * @brief The result of one governing-transition resolution.
 */
struct GoverningTransition
{
  Ui::LayoutTransition transition;     ///< Winning handle; uninitialized when nothing governs
  ViewImpl*            owner{nullptr}; ///< View the handle is attached to (the child itself for SELF)
  LayoutTransitionRole role{LayoutTransitionRole::NONE};
};

/**
 * @brief Resolves WHICH LayoutTransition governs @p child for @p slot.
 *
 * The single decision point for every per-(child, slot) event. Each level
 * terminates on ATTACHMENT alone (wholesale), never per slot:
 *
 *   0. @p child 's LayoutTransitionMode is not AUTO -> NONE, terminal.
 *   1. @p child carries a self transition          -> SELF, terminal.
 *   2. @p directParent is ISOLATE_SUBTREE          -> NONE, terminal (no fall-through).
 *   3. @p directParent carries a transition        -> DIRECT_PARENT, terminal.
 *   4. @c FindGoverningSubtreeOwner(directParent)  -> INHERITED_SUBTREE or NONE.
 *
 * Level 3 terminating regardless of slot effect is the pre-existing semantics of
 * @c FindGoverningSubtreeOwner (see its @note): the closest transition-bearing
 * node wins even when it carries no effect for the slot, and an ancestor SUBTREE
 * scope must not cross it. The self level extends that same rule to distance zero,
 * so nothing about levels 3 and 4 changes when no self transition is set.
 *
 * @param[in] child        The child the event is about
 * @param[in] directParent The child's direct (visual) parent
 * @param[in] slot         The structural slot being resolved (ENTER or EXIT)
 * @return The governing transition, its owner, and the role that produced it
 */
inline GoverningTransition ResolveGoverningTransition(ViewImpl*  child,
                                                      ViewImpl*  directParent,
                                                      ReflowSlot slot)
{
  GoverningTransition result;
  if(!child || !directParent)
  {
    return result;
  }

  // Level 0: POLICY BEFORE VALUE. A child whose mode is not AUTO is never animated
  // by ANY transition, not even by the handle it carries itself. The handle stays
  // attached and GetSelfLayoutTransition() keeps returning it; it is simply not
  // consulted, so returning to AUTO restores its effect with no re-attach.
  if(child->GetLayoutTransitionMode() != Ui::LayoutTransitionMode::AUTO)
  {
    return result;
  }

  Ui::LayoutTransition selfTransition = child->GetSelfLayoutTransition();
  if(selfTransition)
  {
    result.transition = selfTransition;
    result.owner      = child;
    result.role       = LayoutTransitionRole::SELF;
    return result;
  }

  // The direct parent is at distance zero from the cut: ISOLATE_SUBTREE silences
  // its children-role transition for this child AND forbids falling through to an
  // ancestor, which is strictly higher and therefore also cut. (PASS_THROUGH does
  // NOT reach here: transitions pass through the parent as a TARGET only, never
  // touching its children.)
  if(directParent->GetLayoutTransitionMode() == Ui::LayoutTransitionMode::ISOLATE_SUBTREE)
  {
    return result;
  }

  Ui::LayoutTransition parentTransition = directParent->GetLayoutTransition();
  if(parentTransition)
  {
    result.transition = parentTransition;
    result.owner      = directParent;
    result.role       = LayoutTransitionRole::DIRECT_PARENT;
    return result;
  }

  if(ViewImpl* owner = FindGoverningSubtreeOwner(directParent, slot))
  {
    result.transition = owner->GetLayoutTransition();
    result.owner      = owner;
    result.role       = LayoutTransitionRole::INHERITED_SUBTREE;
  }
  return result;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
