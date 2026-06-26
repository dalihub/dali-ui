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

#include <dali-ui-foundation/public-api/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/selectable-view.h>

namespace Dali
{

namespace Ui
{

namespace Provider
{
class GroupSelectableViewImpl;
}

/**
 * @brief GroupSelectableView is a SelectableView subclass with single-selection
 * (radio) grouping built in.
 *
 * GroupSelectableView guarantees that a GroupSelectableTrait is attached for the
 * lifetime of the view. Since GroupSelectableTrait implies SelectableTrait (and
 * InteractiveTrait), GroupSelectableView also exposes the inherited SelectableView
 * and InteractiveView API directly.
 *
 * GroupSelectableView is intended as a base class for radio-style UI components
 * whose members belong to a SelectionGroup that enforces "only one selected".
 *
 * @see GroupSelectableTrait
 * @see SelectableView
 * @see SelectionGroup
 */
class DALI_UI_API GroupSelectableView : public SelectableView
{
public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized GroupSelectableView handle.
   */
  GroupSelectableView();

  /**
   * @brief Creates an initialized GroupSelectableView.
   *
   * @return A handle to a newly allocated GroupSelectableView
   */
  static GroupSelectableView New();

  /**
   * @brief Downcasts a handle to a GroupSelectableView handle.
   *
   * If the handle refers to a GroupSelectableView, the downcast produces a valid handle.
   * Otherwise the returned handle is uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a GroupSelectableView or an uninitialized handle
   */
  static GroupSelectableView DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * @param[in] view Handle to copy
   */
  GroupSelectableView(const GroupSelectableView& view);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  GroupSelectableView(GroupSelectableView&& rhs) noexcept;

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~GroupSelectableView();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  GroupSelectableView& operator=(const GroupSelectableView& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  GroupSelectableView& operator=(GroupSelectableView&& rhs) noexcept = default;

  DALI_UI_VIEW_WITH(GroupSelectableView)

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
   * @see GroupSelectableTrait::SetGroupName
   */
  void SetGroupName(const Dali::String& name);

  /**
   * @brief Returns this member's explicit group name.
   *
   * @return The group name, or an empty string if no explicit name is set (the
   * member then participates in parent auto-grouping when on-scene)
   * @see GroupSelectableTrait::GetGroupName
   */
  Dali::String GetGroupName() const;

  /**
   * @brief Returns the SelectionGroup this member is bound to.
   *
   * The bound group is the named group when a name is set, otherwise the parent
   * auto-group when on-scene under a View parent, otherwise an empty handle.
   *
   * @return The bound SelectionGroup, or an uninitialized handle if unbound
   * @see GroupSelectableTrait::GetGroup
   */
  SelectionGroup GetGroup() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The GroupSelectableView implementation
   */
  explicit DALI_UI_API GroupSelectableView(Provider::GroupSelectableViewImpl& implementation);

  /**
   * @brief Allows the creation of this GroupSelectableView from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API GroupSelectableView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
};

} // namespace Ui

} // namespace Dali
