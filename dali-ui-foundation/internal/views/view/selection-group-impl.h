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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>
#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/selection-group.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{

class GroupSelectableTraitImpl;

/**
 * @brief Internal implementation of SelectionGroup.
 *
 * SelectionGroupImpl owns the cross-View single-selection (radio) state for a set of
 * member trait implementations. It is the analogue, one level up, of how
 * CoreInteractionObject's trait implementations collaborate: a member's
 * GroupSelectableTraitImpl holds a STRONG reference to its SelectionGroupImpl, while the
 * group holds only RAW (non-owning) pointers to the member trait implementations, so
 * there is no ownership cycle.
 *
 * Membership is keyed on the stable GroupSelectableTraitImpl* identity (NOT on a View
 * handle), so a member can be unregistered deterministically even while its View is
 * being destroyed (when its WeakHandle<View> already resolves empty). The trait
 * implementations are owned by CoreInteractionObject (via std::unique_ptr) and notify
 * the group from OnViewDestroying() before they are destroyed, so the group never holds
 * a dangling pointer.
 *
 * The group never drives selection by intercepting it (there is no veto). It collaborates
 * purely through the trait's selection signal: when a member reports that it became
 * selected, the group records the new winner and unselects the previous one.
 */
class SelectionGroupImpl : public BaseObject
{
public:
  /**
   * @brief Creates a new SelectionGroupImpl.
   *
   * @return An intrusive pointer to the newly created implementation
   */
  static IntrusivePtr<SelectionGroupImpl> New();

public: // Signals
  /**
   * @copydoc Dali::Ui::SelectionGroup::SelectedMemberChangedSignal
   */
  Signal<void(View, View, InputEvent)>& SelectedMemberChangedSignal();

public: // Public observers (mirrored on the SelectionGroup handle)
  /**
   * @copydoc Dali::Ui::SelectionGroup::GetMemberCount
   */
  uint32_t GetMemberCount() const;

  /**
   * @copydoc Dali::Ui::SelectionGroup::GetSelectedMember
   */
  View GetSelectedMember() const;

  /**
   * @copydoc Dali::Ui::SelectionGroup::ClearSelection
   */
  void ClearSelection();

public: // Trait collaboration (called by GroupSelectableTraitImpl)
  /**
   * @brief Registers a member in the group.
   *
   * Called by GroupSelectableTraitImpl::JoinGroup(). Idempotent: a member already
   * registered is not added twice. If the joining member is already selected and the
   * group already has a winner, the joining member is unselected to preserve the
   * single-selection invariant; if the group has no winner, the joining member becomes
   * the winner with no signal (seed).
   *
   * @param[in] member The member trait implementation to register
   */
  void RegisterMember(GroupSelectableTraitImpl* member);

  /**
   * @brief Unregisters a member from the group (structural removal only; no signal).
   *
   * Called by GroupSelectableTraitImpl on group-leave (LeaveGroup), detach, or View destruction. Performs
   * structural removal ONLY: it erases the member from the member list and, if the member
   * was the current winner, clears the winner (the group becomes empty; no promotion). It
   * does NOT emit SelectedMemberChangedSignal; the caller is responsible for emitting after
   * the member is fully torn down (see EmitSelectedMemberChanged), so a re-add from the
   * change callback is not lost against a half-torn-down member. Keyed on the stable
   * trait-implementation pointer, so it works even when the member's View is being
   * destroyed.
   *
   * @param[in] member The member trait implementation to unregister
   * @return The removed winner's View if this member was the winner and its View is still
   *         resolvable, otherwise an empty View. During View destruction the member's View
   *         already resolves empty, so an empty View is returned and no emit happens.
   */
  View UnregisterMember(GroupSelectableTraitImpl* member);

  /**
   * @brief Emits SelectedMemberChangedSignal(previous, current, event).
   *
   * Entry point for GroupSelectableTraitImpl to announce a structural winner removal AFTER
   * the member has been fully torn down (group reference dropped, interaction reverted,
   * accessibility restored), so a re-add issued from the change callback succeeds.
   *
   * @param[in] previous The previous winner's View
   * @param[in] current  The current winner's View (empty after a structural removal)
   * @param[in] event    The originating input cause
   */
  void EmitSelectedMemberChanged(View previous, View current, InputEvent event);

  /**
   * @brief Notifies the group that a member became selected.
   *
   * The group records the new winner BEFORE unselecting the previous one, so the
   * re-entrant unselection of the old winner is recognized (winner != old) and
   * ignored. Emits SelectedMemberChangedSignal exactly once.
   *
   * @param[in] member The member trait implementation that became selected
   * @param[in] event  The originating input cause
   */
  void OnMemberSelected(GroupSelectableTraitImpl* member, InputEvent event);

  /**
   * @brief Notifies the group that a member became unselected.
   *
   * If the unselected member is the current winner (programmatic unselect or
   * ClearSelection), the group becomes empty and emits the change signal. If it is not
   * the current winner (the displaced old winner during a swap), this is ignored.
   *
   * @param[in] member The member trait implementation that became unselected
   * @param[in] event  The originating input cause
   */
  void OnMemberUnselected(GroupSelectableTraitImpl* member, InputEvent event);

protected:
  /**
   * @brief Constructs a new SelectionGroupImpl.
   */
  SelectionGroupImpl();

  /**
   * @brief Destructor.
   */
  ~SelectionGroupImpl() override;

private:
  SelectionGroupImpl(const SelectionGroupImpl&)            = delete;
  SelectionGroupImpl(SelectionGroupImpl&&)                 = delete;
  SelectionGroupImpl& operator=(const SelectionGroupImpl&) = delete;
  SelectionGroupImpl& operator=(SelectionGroupImpl&&)      = delete;

  /**
   * @brief Returns the index of @a member in the member list, or -1 if absent.
   */
  int32_t FindMember(GroupSelectableTraitImpl* member) const;

  /**
   * @brief Resolves a member trait implementation to its owner View handle.
   */
  static View ResolveView(GroupSelectableTraitImpl* member);

private:
  std::vector<GroupSelectableTraitImpl*> mMembers; ///< Non-owning pointers to all member trait implementations.
  GroupSelectableTraitImpl*              mWinner;  ///< Non-owning pointer to the currently selected member (nullptr if none).
  Signal<void(View, View, InputEvent)>   mSelectedMemberChangedSignal;
};

} // namespace Internal

/**
 * @brief Retrieves the SelectionGroupImpl from a SelectionGroup handle.
 *
 * Declared (not defined) in this internal header so it is not exported from the public
 * API, mirroring GetImpl(SelectableTrait&).
 *
 * @param[in] obj The SelectionGroup handle
 * @return A reference to the internal implementation
 */
Internal::SelectionGroupImpl&       GetImpl(SelectionGroup& obj);
const Internal::SelectionGroupImpl& GetImpl(const SelectionGroup& obj);

} // namespace Ui

} // namespace Dali
