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
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/traits/selectable-trait.h>
#include <dali-ui-foundation/public-api/views/selection-group.h>

namespace Dali
{

namespace Ui
{

// Forward declarations
namespace Internal
{
class CoreInteractionObject;
}

/**
 * @brief GroupSelectableTrait is a state trait that provides single-selection
 * (mutual exclusion / radio) behavior to a View.
 *
 * GroupSelectable implies Selectable implies Interactive. A View with
 * GroupSelectableTrait also has SelectableTrait behavior (a boolean selected
 * state and SelectionChangedSignal) and InteractiveTrait behavior (click
 * handling, pressed state). GroupSelectableTrait derives from SelectableTrait
 * so callers can use the returned handle for grouping, selection, and
 * interactive APIs alike.
 *
 * A grouped member belongs to a SelectionGroup that enforces "only one
 * selected". Membership is DECLARATIVE, established through one of exactly two
 * mechanisms with a fixed precedence (a set name always wins over parent-auto):
 *  1. Parent auto-grouping (default, no setup): a View made GroupSelectable that
 *     is on-scene under a View parent and has no group name auto-joins that
 *     parent's group. This membership is scene-scoped: the member leaves the
 *     parent group when it goes off-scene and re-joins on reconnection.
 *  2. Named grouping: SetGroupName(name) joins the named group
 *     SelectionGroup::Find(name) eagerly (cross-parent, persistent across scene
 *     connection cycles). Setting a name suppresses parent auto-grouping;
 *     clearing it (SetGroupName("")) falls back to parent-auto when on-scene.
 *
 * Clicking the already-selected member is a no-op (true radio); a group never
 * becomes empty through a gesture, only through a non-gesture route such as
 * SelectionGroup::ClearSelection() or a programmatic SetSelected(false) on the
 * selected member.
 *
 * Internally the interactive, selectable, and group-selectable trait
 * implementations share the single core interaction trait slot.
 *
 * @note InteractiveTrait, SelectableTrait, and GroupSelectableTrait are facets of a
 * single shared interaction object on a View. Comparing handles with operator== compares
 * that same underlying object, and DownCast is presence-based (does the requested facet's
 * sub-implementation exist?) rather than identity-based.
 */
class DALI_UI_API GroupSelectableTrait : public SelectableTrait
{
public:
  // Typedefs

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized GroupSelectableTrait handle.
   */
  GroupSelectableTrait();

  /**
   * @brief Downcasts a handle to GroupSelectableTrait handle.
   *
   * If the handle refers to a GroupSelectableTrait, the downcast produces a
   * valid handle. Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object stored in View's core interaction trait slot
   * @return A handle to GroupSelectableTrait or an uninitialized handle
   */
  static GroupSelectableTrait DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] groupSelectableTrait Handle to copy
   */
  GroupSelectableTrait(const GroupSelectableTrait& groupSelectableTrait);

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~GroupSelectableTrait();

public: // API
  /**
   * @brief Sets the explicit group name for this member.
   *
   * Joins the named group SelectionGroup::Find(name) eagerly. A named group is
   * cross-parent and persistent (it survives scene connection cycles), and a set
   * name takes precedence over parent auto-grouping: while a non-empty name is
   * set the member never participates in its parent's auto-group.
   *
   * Passing an empty string clears the name. When the member is on-scene under a
   * View parent at the time the name is cleared, it immediately falls back to
   * parent auto-grouping; when off-scene it auto-joins on the next scene
   * connection.
   *
   * @param[in] name The group name, or an empty string to clear it
   */
  void SetGroupName(const Dali::String& name);

  /**
   * @brief Returns this member's explicit group name.
   *
   * @return The group name, or an empty string if no explicit name is set (the
   * member then participates in parent auto-grouping when on-scene)
   */
  Dali::String GetGroupName() const;

  /**
   * @brief Returns the SelectionGroup this member is bound to.
   *
   * The bound group is the named group when a name is set, otherwise the parent
   * auto-group when on-scene under a View parent, otherwise an empty handle.
   * Membership is changed declaratively through SetGroupName() and parent
   * grouping; this accessor is read-only.
   *
   * @return The bound SelectionGroup, or an uninitialized handle if unbound
   */
  SelectionGroup GetGroup() const;

public: // Not intended for application developers
  /**
   * @brief Creates an internal GroupSelectableTrait handle.
   *
   * The returned handle stores a CoreInteractionObject and owns the interactive,
   * selectable, and group-selectable trait implementations. Application
   * developers should obtain this trait through View::AsGroupSelectable().
   *
   * @return A handle to a newly allocated GroupSelectableTrait.
   */
  DALI_INTERNAL static GroupSelectableTrait New();

  /**
   * @brief Creates a handle using the internal core interaction trait object.
   *
   * @param[in] container The core interaction trait object
   * @return A handle to GroupSelectableTrait
   */
  DALI_INTERNAL static GroupSelectableTrait New(Internal::CoreInteractionObject* container);

private:
  explicit DALI_INTERNAL GroupSelectableTrait(Internal::CoreInteractionObject* container);
};

} // namespace Ui

} // namespace Dali
