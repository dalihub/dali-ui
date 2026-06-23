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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/input-event.h>

namespace Dali
{

namespace Ui
{

// Forward declaration (view.h provides the full definition when needed).
// SelectionGroup deliberately does not include view.h to avoid a circular include
// (view.h pulls in the trait headers, which reach SelectionGroup through the trait).
class View;

namespace Internal
{
class SelectionGroupImpl;
}

/**
 * @brief SelectionGroup enforces single-selection (radio) semantics across a set of Views.
 *
 * A SelectionGroup is a controller/observer for a set of GroupSelectable Views that share
 * single-selection (radio) behaviour: at most one member can be selected at a time, and
 * selecting a member automatically unselects the previously selected one. When a member
 * responds to clicks (toggle-by-click enabled, the default), clicking it selects it and
 * clicking the already-selected member is a no-op (true radio behaviour). A gesture can
 * never empty the group. The group becomes empty (no selected
 * member) only through a non-gesture route: ClearSelection() or a programmatic
 * SetSelected(false) on the selected member's SelectableTrait.
 *
 * Membership is DECLARATIVE and is NOT established through this handle. A View becomes a
 * member through one of exactly two mechanisms (precedence: name wins over parent-auto):
 *  1. Parent auto-grouping (default): an AsGroupSelectable() View that is on-scene under a
 *     View parent and has no group name auto-joins that parent's group (scene-scoped).
 *  2. Named grouping: GroupSelectableTrait::SetGroupName(name) joins a named group
 *     (cross-parent, persistent). A set name always overrides parent auto-grouping.
 *
 * A SelectionGroup handle is OBTAINED, not created, via the Find() factories. Find(name)
 * returns the named group (creating it on first lookup); Find(parentView) returns the
 * parent auto-group. The same group is returned for the same key, so an application can
 * observe and control a declaratively-formed group.
 *
 * The group does not extend its members' lifetime: it tracks members by raw, non-owning
 * pointers and auto-unregisters each member when it is destroyed, so it never holds a
 * dangling pointer. The Find() registries hold the group only weakly, so a group is freed
 * once its last handle and last member are gone.
 *
 * @code
 *   // Named group: two Views in different parents share one selection.
 *   radioA.AsGroupSelectable().SetGroupName("payment");
 *   radioB.AsGroupSelectable().SetGroupName("payment");
 *   SelectionGroup group = SelectionGroup::Find("payment");
 *   group.SelectedMemberChangedSignal().Connect(&MyClass::OnRadioChanged);
 *
 *   // Parent auto-group: children of an on-scene parent are mutually exclusive.
 *   parent.Add(optionA.AsGroupSelectable());
 *   parent.Add(optionB.AsGroupSelectable());
 *   SelectionGroup parentGroup = SelectionGroup::Find(parent);
 * @endcode
 */
class DALI_UI_API SelectionGroup : public BaseHandle
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized SelectionGroup handle.
   *
   * Use SelectionGroup::Find() to obtain an initialized group.
   */
  SelectionGroup();

  /**
   * @brief Returns the named SelectionGroup, creating it on first lookup.
   *
   * The named group is the cross-parent, persistent group that members join with
   * GroupSelectableTrait::SetGroupName(name). Calling Find with the same name always
   * returns the same group while it is alive; once its last handle and last member are
   * gone the group is freed and a later Find(name) creates a fresh one.
   *
   * @note Names live in a single global, single-threaded namespace.
   *
   * @param[in] name The group name
   * @return The named SelectionGroup
   */
  static SelectionGroup Find(const Dali::String& name);

  /**
   * @brief Returns the parent auto-group for the given View parent, creating it on first
   * lookup.
   *
   * The parent auto-group is the scene-scoped group that an on-scene AsGroupSelectable()
   * child with no group name auto-joins. Calling Find with the same parent View always
   * returns the same group while it is alive.
   *
   * @param[in] parentView The parent View
   * @return The parent auto-group SelectionGroup
   */
  static SelectionGroup Find(View parentView);

  /**
   * @brief Downcasts a handle to a SelectionGroup handle.
   *
   * If the handle points to a SelectionGroup object, the downcast produces a valid
   * handle. Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle A handle to an object
   * @return A handle to a SelectionGroup object or an uninitialized handle
   */
  static SelectionGroup DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle A reference to the copied handle
   */
  SelectionGroup(const SelectionGroup& handle);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  SelectionGroup(SelectionGroup&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle A reference to the copied handle
   * @return A reference to this
   */
  SelectionGroup& operator=(const SelectionGroup& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  SelectionGroup& operator=(SelectionGroup&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~SelectionGroup();

public: // Signals
  /**
   * @brief Emitted when the selected member of the group changes.
   *
   * The callback signature is: void YourCallbackName(View previous, View current, InputEvent event)
   *
   * @c previous is the member that was selected before the change (an empty View
   * handle if there was none). @c current is the member that is now selected (an
   * empty View handle if the group is now empty). @c event carries the originating
   * input cause, or InputEvent::Programmatic() for API-driven changes.
   *
   * @return The selected-member-changed signal
   */
  Signal<void(View, View, InputEvent)>& SelectedMemberChangedSignal();

public: // API
  /**
   * @brief Returns the number of live members currently in the group.
   *
   * @return The member count
   */
  uint32_t GetMemberCount() const;

  /**
   * @brief Returns the currently selected member of the group.
   *
   * @return The selected member, or an empty View handle if the group has no
   * selected member
   */
  View GetSelectedMember() const;

  /**
   * @brief Clears the current selection, leaving the group with no selected member.
   *
   * This is one of the routes to an empty group (the others are a programmatic
   * SetSelected(false) on the selected member, or the selected member leaving the group
   * via SetGroupName("") / reparenting away / destruction); a gesture never empties the
   * group. If the group already has no selected member, this call is a no-op.
   */
  void ClearSelection();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Allows the creation of a SelectionGroup handle from an internal pointer.
   *
   * @note Not intended for application developers
   * @param[in] implementation A pointer to the internal implementation
   */
  explicit DALI_INTERNAL SelectionGroup(Internal::SelectionGroupImpl* implementation);
  /// @endcond
};

} // namespace Ui

} // namespace Dali
