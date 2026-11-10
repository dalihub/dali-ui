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
#include <dali/public-api/common/dali-namespace.h>
#include <cstdint>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @brief Specifies how corner radius values are interpreted.
 *
 * @SINCE_3_0.0
 * @note The values match the transform policy used internally by the visual system.
 */
enum class CornerRadiusPolicy
{
  RELATIVE = 0, ///< Relative to the view size (percentage [0.0, 0.5] of the shorter side) @SINCE_3_0.0
  ABSOLUTE = 1, ///< Absolute value in world units (default) @SINCE_3_0.0
};

/**
 * @brief Controls whether sibling-order changes (Raise/Lower/RaiseAbove/LowerBelow)
 * also reorder the layout children list.
 *
 * The actor (sibling) order and the layout order are kept in sync by default. With
 * PRESERVE only the actor order changes, which sets the draw and hit-test order, while
 * the layout order stays the same.
 *
 * @deprecated Used only by the deprecated View and ViewImpl sibling-order overloads and
 * removed together with them. Instead of UPDATE, call the inherited Dali::Actor method
 * without a policy. Instead of PRESERVE, call the Extension::View function of the same base
 * name with the suffix PreservingLayout (dali-ui-foundation/extension-api/view.h), for example
 * Extension::View::RaiseToTopPreservingLayout(view) for RaiseToTop(LayoutOrderPolicy::PRESERVE).
 * See also docs/view-sibling-order-migration.md in the dali-ui source tree.
 * @SINCE_3_0.0
 */
enum class LayoutOrderPolicy
{
  UPDATE   = 0, ///< Also reorder layout children to match the new sibling order. @SINCE_3_0.0
  PRESERVE = 1, ///< Keep layout children order unchanged; only the actor (sibling) order changes. @SINCE_3_0.0
};

/**
 * @brief Controls how View::Remove(View, RemovePolicy) and
 * View::RemoveAll(RemovePolicy) treat an attached LayoutTransition's EXIT slot.
 *
 * ENTER is dispatched automatically for every add path (Actor::Add,
 * Actor::InsertAbove/InsertBelow),
 * but EXIT cannot be hooked transparently on removal, so the EXIT intent must
 * be requested explicitly through this policy.
 *
 * @SINCE_3_0.0
 * @note IMMEDIATE is not identical to the inherited Actor::Remove(Actor): it
 * still runs the View's child bookkeeping and the in-flight-ghost guard, but
 * unparents now and skips BOTH the view's own EXIT slot and any inherited
 * SUBTREE-scope EXIT effect.
 */
enum class RemovePolicy
{
  IMMEDIATE    = 0, ///< Unparent now; do not run any EXIT transition. @SINCE_3_0.0
  ANIMATE_EXIT = 1, ///< Run the attached EXIT transition (own or inherited SUBTREE) first, then unparent; immediate when no EXIT slot is configured. @SINCE_3_0.0
};

/**
 * @brief Per-view policy deciding how layout transitions treat this view.
 *
 * Read BEFORE any transition handle is resolved, so it is a policy, not a value:
 * a view whose mode is not @c AUTO is not animated even by the handle it
 * carries through @c View::SetSelfLayoutTransition(). Handles are never detached
 * by a mode change — @c View::GetSelfLayoutTransition() keeps returning them, and
 * returning to @c AUTO restores their effect.
 *
 * @SINCE_3_0.0
 * @note Changing the mode does not interrupt in-flight transitions — the same
 * contract as replacing a transition handle. It applies from the next
 * per-(view, slot) event.
 */
enum class LayoutTransitionMode : uint8_t
{
  AUTO            = 0, ///< (default) Normal resolution: own self transition > direct parent's > closest SUBTREE-scope ancestor's. @SINCE_3_0.0
  PASS_THROUGH    = 1, ///< Layout transitions pass through this view: it is never their target, not even of the handle it attached itself with @c View::SetSelfLayoutTransition() — CHANGE snaps to the arranged bounds, EXIT unparents immediately, ENTER is skipped and nothing is settled. Inheritance keeps flowing to its descendants, and its own children-role transition keeps governing its children. @SINCE_3_0.0
  ISOLATE_SUBTREE = 2  ///< This view and its whole subtree are isolated from every owner at or above it — this view's own children-role transition included: nothing from above animates anything inside. Declarations made strictly below the gate still work (a descendant's self transition, a descendant's children-role transition). @SINCE_3_0.0
};

} // namespace Ui
} //namespace DALI_NAMESPACE
