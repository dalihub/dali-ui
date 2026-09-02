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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <cstdint>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/traits/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/views/selection-group.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{

namespace Ui
{

class InputEvent;

namespace Internal
{
class CoreInteractionObject;
class SelectionGroupImpl;

/**
 * @brief Internal implementation of GroupSelectable trait.
 *
 * GroupSelectableTraitImpl is stored inside CoreInteractionObject alongside the
 * InteractiveTraitImpl and SelectableTraitImpl. It implements single-selection
 * (radio / mutual exclusion) by collaborating with a SelectionGroupImpl through
 * signals only:
 *
 *  - The click policy is GATED ON GROUP MEMBERSHIP. While the member is bound to
 *    a group it sets the sibling SelectableTrait's INTERNAL select-only flag, so
 *    the sibling's own OnClickedForToggle handler keeps an already-selected member
 *    selected (a click can never unselect the winner and empty the group). Clicking
 *    the already-selected member is therefore a free no-op (true radio). Grouping
 *    NEVER changes the sibling's public toggle-by-click setting: it is neither saved
 *    nor restored, so if toggle-by-click was disabled the click path stays inert.
 *    While the member is ungrouped (AsGroupSelectable() with no group name and not
 *    under a parent's auto-group, or after it has left its group) it behaves exactly
 *    as a plain Selectable: the internal select-only flag is cleared.
 *  - The selection observer is MEMBERSHIP-INDEPENDENT: it stays installed as the
 *    sibling SelectableTraitImpl's INTERNAL post-commit observer (a stateless function
 *    pointer invoked just before the public SelectionChangedSignal) for the whole
 *    attached lifetime, no-opping while mGroup is null. When this member becomes selected
 *    (and is grouped) it notifies its SelectionGroupImpl, which records the new
 *    winner and unselects the previous winner.
 *
 * It also keeps the member's accessibility in lock-step: RADIO_BUTTON role on
 * join, CHECKED state mirrored to the selected state via read-modify-write.
 *
 * On top of the binding/arbitration core above, this trait implements the
 * DECLARATIVE membership layer. A member's group is resolved from exactly two
 * mutually-exclusive sources, name winning over parent-auto:
 *  - mGroupName (explicit name): bound eagerly to SelectionGroup::Find(name).
 *  - mAutoGroupParent (parent-auto, scene-scoped): when no name is set and the
 *    owner is on-scene under a View parent, bound to SelectionGroup::Find(parent)
 *    and left on scene disconnection.
 * The declarative layer only chooses WHICH group; it binds/unbinds through the
 * core JoinGroup()/LeaveGroup() path, preserving all arbitration and the P2
 * fixes. To observe the owner's scene lifetime it connects to the owner View's
 * OnScene/OffScene signals (membership-independent, dropped on teardown).
 */
class GroupSelectableTraitImpl : public ConnectionTracker
{
public:
  /**
   * @copydoc Dali::Ui::GroupSelectableTrait::GroupSelectableTrait
   */
  GroupSelectableTraitImpl();

public: // API
  /**
   * @copydoc Dali::Ui::GroupSelectableTrait::SetGroupName
   */
  void SetGroupName(const std::string& name);

  /**
   * @copydoc Dali::Ui::GroupSelectableTrait::GetGroupName
   */
  std::string GetGroupName() const;

  /**
   * @copydoc Dali::Ui::GroupSelectableTrait::GetGroup
   */
  SelectionGroup GetGroup() const;

  /**
   * @copydoc Dali::Ui::GroupSelectableTrait::~GroupSelectableTrait
   */
  virtual ~GroupSelectableTraitImpl();

public: // SelectionGroupImpl collaboration
  // GetOwner() is public because the collaborator SelectionGroupImpl (a SEPARATE,
  // non-friend class) calls it from selection-group-impl.cpp. JoinGroup()/LeaveGroup()
  // are the core bind/unbind primitives driven by this class's own declarative layer
  // (SetGroupName / parent-auto); they sit here for proximity to that collaboration.

  /**
   * @brief Binds this member to a group.
   *
   * Called by the declarative layer. Sets the strong group back-pointer, registers
   * this member with the group (which reconciles its selected state with the group's
   * current winner) and then, if attached, applies the membership-gated select-only click
   * policy (sets the sibling's internal select-only flag; toggle-by-click is untouched)
   * and applies the radio accessibility costume.
   *
   * @param[in] group The owning group implementation
   */
  void JoinGroup(SelectionGroupImpl* group);

  /**
   * @brief Unbinds this member from its group.
   *
   * Called when the member leaves its group. Performs the teardown in order: structural
   * removal (no signal) capturing the previous-winner View, drops the group back-pointer,
   * then -- if attached -- reverts the click policy (clears the sibling's internal
   * select-only flag; toggle-by-click is untouched) and restores accessibility, and ONLY THEN emits the change signal if
   * this member was the winner. Deferring the emit to the very end means the member is a
   * fully detached plain Selectable when the callback runs, so a re-add via SetGroupName()
   * or reparenting from the callback is not lost. The member's own selected state is preserved.
   */
  void LeaveGroup();

  /**
   * @brief Gets the owner view.
   *
   * @return The owner view, or an uninitialized handle if not attached
   */
  View GetOwner() const;

protected:
  friend class CoreInteractionObject;

  void OnAttached(View& view);

  void OnDetaching(View& view);

  void OnViewDestroying(ViewImpl* viewImpl);

private:
  /**
   * @brief Called when the owner View is connected to a scene.
   *
   * When no explicit name is set, parent auto-grouping takes over: the member joins its
   * parent View's auto-group. When a name is set this is a no-op (the named membership was
   * already bound by SetGroupName and persists across scene cycles).
   */
  void OnOwnerOnScene(Dali::Actor actor);

  /**
   * @brief Called when the owner View is disconnected from a scene.
   *
   * Parent-auto membership is scene-scoped, so the member leaves its parent auto-group on
   * disconnect. Explicit-named membership is NOT touched (it persists).
   */
  void OnOwnerOffScene(Dali::Actor actor);

  /**
   * @brief Binds this member to the named group SelectionGroup::Find(name).
   *
   * Resolves the named group and joins it through the core JoinGroup() path.
   */
  void BindToNamedGroup(const std::string& name);

  /**
   * @brief Leaves the parent auto-group, if currently auto-joined, and clears the recorded
   * auto-group parent.
   *
   * Uses the core LeaveGroup() path so the deferred change signal and the click-policy
   * revert (clearing the sibling's internal select-only flag; toggle-by-click untouched)
   * are honored. Does not touch mGroupName.
   */
  void LeaveAutoGroup();

  /**
   * @brief Joins the owner View's parent auto-group when applicable.
   *
   * Applicable when no explicit name is set, the owner and its parent View resolve, and the
   * parent's auto-group is not already the recorded auto-group. Leaves a previously
   * auto-joined parent first. Callers must only invoke this when no name is set.
   */
  void JoinParentGroupIfApplicable();

  /**
   * @brief Installs the membership-INDEPENDENT selection observer.
   *
   * Installs the sibling SelectableTraitImpl's INTERNAL post-commit observer (a stateless
   * function pointer, DispatchCommit, invoked just before the public SelectionChangedSignal),
   * so group arbitration settles before any user-facing SelectionChangedSignal callback
   * regardless of connection order. The handler no-ops while mGroup is null (if(mGroup)
   * guard), so it is safe to keep installed for the whole attached lifetime regardless of
   * group membership.
   */
  void ConnectSelectionObserver();

  /**
   * @brief Clears the selection observer installed by ConnectSelectionObserver().
   */
  void DisconnectSelectionObserver();

  /**
   * @brief Applies the membership-GATED select-only click policy.
   *
   * Sets the sibling SelectableTrait's group-owned select-only policy so its
   * OnClickedForToggle handler keeps an already-selected member selected (a re-click on the
   * winner is a no-op).
   * Grouping NEVER changes the sibling's public toggle-by-click setting: it is neither saved
   * nor restored. If toggle-by-click was disabled the click path stays inert; programmatic
   * arbitration still works through SetSelected/SelectionChangedSignal.
   */
  void ApplyGroupClickPolicy();

  /**
   * @brief Reverts the click policy applied by ApplyGroupClickPolicy().
   *
   * Clears only the sibling SelectableTrait's group-owned select-only policy. An independent
   * persistent policy remains active. Toggle-by-click is untouched (grouping never changed it).
   */
  void RestoreClickPolicy();

  /**
   * @brief Stateless post-commit dispatcher installed on the sibling SelectableTraitImpl.
   *
   * Captures no object identity (cannot dangle); re-resolves this view's group trait from the
   * owner View and forwards to OnSelectionChanged. Installed/cleared by
   * ConnectSelectionObserver/DisconnectSelectionObserver.
   */
  static void DispatchCommit(View view, bool selected, InputEvent event);

  /**
   * @brief Post-commit selection-changed handler.
   *
   * Mirrors CHECKED accessibility and notifies the group of the change.
   */
  void OnSelectionChanged(View view, bool selected, InputEvent event);

  /**
   * @brief Destroy/detach-only group teardown for OnViewDestroying() and the (dead)
   * OnDetaching() path. NOT used by LeaveGroup() (which defers the change signal).
   *
   * Performs structural removal on the stable trait-pointer identity and drops the strong
   * group reference, discarding the returned View and emitting NO change signal: on the
   * destroy path the owner View already resolves empty, so the removal yields an empty
   * View and destruction must not emit. Accessibility restore is the caller's
   * responsibility (it is meaningful only while the owner View is alive).
   */
  void UnbindFromGroup();

  /**
   * @brief Applies radio accessibility to the owner: caches the prior role,
   * sets RADIO_BUTTON, and seeds the CHECKED bit from the current selected state.
   */
  void ApplyRadioAccessibility();

  /**
   * @brief Restores the owner's accessibility: restores the cached role and sets the
   * CHECKED bit to follow the RESTORED role (CHECKED only for checkable roles
   * CHECK_BOX/RADIO_BUTTON/TOGGLE_BUTTON, and only when still selected). For the typical
   * NONE restored role this clears CHECKED. The logical SELECTED state is independent of
   * CHECKED, so this never unselects the member.
   */
  void RestoreAccessibility();

  /**
   * @brief Writes the CHECKED accessibility bit using read-modify-write so that
   * other state bits (e.g. ENABLED) are preserved.
   *
   * @param[in] view    The owner view
   * @param[in] checked The desired CHECKED bit value
   */
  void WriteCheckedState(View view, bool checked);

private:
  WeakHandle<View>                 mOwner;
  IntrusivePtr<SelectionGroupImpl> mGroup;           ///< Strong reference; the group holds only a non-owning pointer back (no cycle).
  std::string                      mGroupName;       ///< Explicit group name; empty means none (then parent-auto applies on-scene).
  WeakHandle<View>                 mAutoGroupParent; ///< WEAK: the parent View whose auto-group was joined, if any.
  int32_t                          mPreviousRole;
  bool                             mRoleCached : 1;
  bool                             mAttached : 1;
};

} // namespace Internal

Internal::GroupSelectableTraitImpl&       GetImpl(GroupSelectableTrait& obj);
const Internal::GroupSelectableTraitImpl& GetImpl(const GroupSelectableTrait& obj);

} // namespace Ui

} // namespace Dali
