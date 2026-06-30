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
#include <dali-ui-foundation/internal/views/view/group-selectable-trait-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/view/core-interaction-object.h>
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>
#include <dali-ui-foundation/internal/views/view/selection-group-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/input-event.h>
#include <dali-ui-foundation/public-api/selectable-trait.h>
#include <dali-ui-foundation/public-api/view-accessibility-enums.h>
#include <dali-ui-foundation/public-api/view-impl.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali::Ui::Internal
{
namespace
{
SelectableTrait GetSelectableTrait(ViewImpl& viewImpl)
{
  auto* traitObject = ViewDataImpl::Get(viewImpl).GetCoreInteractionObject();
  return traitObject ? SelectableTrait::DownCast(BaseHandle(static_cast<BaseObject*>(traitObject))) : SelectableTrait();
}

constexpr int32_t CHECKED_MASK = 1 << static_cast<int32_t>(AccessibilityState::CHECKED);

/**
 * @brief Resolves the parent View of the given View, or an empty handle.
 */
View GetParentView(View view)
{
  if(!view)
  {
    return View();
  }
  return View::DownCast(view.GetParent());
}

} // unnamed namespace

GroupSelectableTraitImpl::GroupSelectableTraitImpl()
: mOwner(),
  mGroup(),
  mGroupName(),
  mAutoGroupParent(),
  mPreviousRole(static_cast<int32_t>(AccessibilityRole::NONE)),
  mRoleCached(false),
  mAttached(false)
{
}

GroupSelectableTraitImpl::~GroupSelectableTraitImpl()
{
  // Defensive insurance: if this trait impl is destroyed while still bound to a group
  // (e.g. unexpected teardown order), drop the strong group reference and unregister so
  // the group never holds a dangling pointer. Idempotent; a no-op when already unbound.
  UnbindFromGroup();
}

SelectionGroup GroupSelectableTraitImpl::GetGroup() const
{
  return SelectionGroup(mGroup.Get());
}

std::string GroupSelectableTraitImpl::GetGroupName() const
{
  return mGroupName;
}

void GroupSelectableTraitImpl::SetGroupName(const std::string& name)
{
  if(name == mGroupName)
  {
    return;
  }

  View owner = mOwner.GetHandle();

  // Record the new name FIRST, before any teardown. LeaveGroup() below can emit the
  // deferred P2-1 (previous, empty) change signal, whose callback may re-enter
  // SetGroupName() (e.g. a re-add to the same name). That re-entrant call must observe the
  // final intended name: if mGroupName were still the OLD name, a re-add to the new name
  // would be wrongly short-circuited by the name==mGroupName guard and silently lost.
  mGroupName = name;

  // An explicit name takes precedence over parent auto-grouping. Leaving the currently
  // bound group (whether named or parent-auto) via the core LeaveGroup() path also resets
  // mAutoGroupParent and honors the P2 fixes. LeaveGroup is a no-op when not bound.
  if(mGroup)
  {
    LeaveGroup();
  }

  if(!name.empty())
  {
    // Bind to the named group eagerly (cross-parent, persistent). Independent of scene
    // connection: named membership is not scene-scoped. JoinGroup is idempotent, so if a
    // re-entrant SetGroupName from the LeaveGroup emit already bound us to this group, this
    // is a harmless no-op.
    if(owner)
    {
      BindToNamedGroup(name);
    }
    return;
  }

  // Name cleared: fall back to parent auto-grouping. When on-scene, take over immediately
  // rather than waiting for the next reconnection; when off-scene, the next OnOwnerOnScene
  // performs the auto-join (preserving the scene-scoped semantics of auto-membership).
  if(owner && owner.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE))
  {
    JoinParentGroupIfApplicable();
  }
}

void GroupSelectableTraitImpl::BindToNamedGroup(const std::string& name)
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  // Resolve (creating if needed) the named group and bind through the core JoinGroup path.
  // mAutoGroupParent stays reset: a named member is never recorded as parent-auto.
  SelectionGroup group = SelectionGroup::Find(name.c_str());
  JoinGroup(&GetImpl(group));
}

void GroupSelectableTraitImpl::LeaveAutoGroup()
{
  // Leave the parent auto-group, if any, through the core LeaveGroup() path (which also
  // resets mAutoGroupParent and honors the P2 fixes). Only acts when an auto-group parent
  // is recorded, so a named member (mAutoGroupParent empty) is untouched.
  if(mAutoGroupParent.GetHandle())
  {
    LeaveGroup();
  }
  mAutoGroupParent.Reset();
}

void GroupSelectableTraitImpl::JoinParentGroupIfApplicable()
{
  // Caller guarantees no explicit name is set (name wins over parent-auto).
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  View parent = GetParentView(owner);
  if(!parent)
  {
    return;
  }

  // Idempotent: already auto-joined this same parent.
  if(mAutoGroupParent.GetHandle() == parent)
  {
    return;
  }

  // Leave a previously auto-joined parent group (e.g. reparent before a scene disconnect)
  // before joining the new parent's group.
  LeaveAutoGroup();

  SelectionGroup group = SelectionGroup::Find(parent);
  JoinGroup(&GetImpl(group));
  mAutoGroupParent = parent;
}

void GroupSelectableTraitImpl::OnOwnerOnScene(Dali::Actor /*actor*/)
{
  // Explicit name wins: SetGroupName already bound the named group eagerly and named
  // membership is not scene-scoped, so nothing to do here.
  if(!mGroupName.empty())
  {
    return;
  }

  JoinParentGroupIfApplicable();
}

void GroupSelectableTraitImpl::OnOwnerOffScene(Dali::Actor /*actor*/)
{
  // Parent-auto membership is scene-scoped: leave it on disconnect. Explicit-named
  // membership is NOT touched here (it persists across scene connection cycles).
  LeaveAutoGroup();
}

View GroupSelectableTraitImpl::GetOwner() const
{
  return mOwner.GetHandle();
}

void GroupSelectableTraitImpl::JoinGroup(SelectionGroupImpl* group)
{
  // Idempotent re-join to the same group.
  if(mGroup.Get() == group)
  {
    return;
  }

  // Cross-group rebind: leave the previous group first (restores accessibility).
  if(mGroup)
  {
    LeaveGroup();
  }

  mGroup = group;
  if(!mGroup)
  {
    return;
  }

  // M2 ordering: register FIRST so the group's join reconciliation settles the
  // selected bit (a member joining a group that already has a winner is forced
  // false; a member joining an empty group becomes the seed winner). The
  // force-false path routes through OnSelectionChanged, which clears CHECKED.
  mGroup->RegisterMember(this);

  // THEN, while attached, apply the membership-gated click policy (set the sibling's
  // internal select-only flag so a click never unselects the winner; toggle-by-click
  // untouched) and apply the radio costume so CHECKED is seeded from the now-final selected
  // state (the seed winner ends up CHECKED, a displaced loser ends up not CHECKED) while
  // the role is RADIO_BUTTON.
  if(mAttached)
  {
    ApplyGroupClickPolicy();
    ApplyRadioAccessibility();
  }
}

void GroupSelectableTraitImpl::LeaveGroup()
{
  if(!mGroup)
  {
    return;
  }

  // Keep the group alive across mGroup.Reset() so the deferred change signal can be
  // emitted from it after the member is fully torn down.
  IntrusivePtr<SelectionGroupImpl> group = mGroup;

  // Structural removal ONLY (no signal here): erases this member and, if it was the
  // winner, clears the winner and returns the previous View (empty on the destroy path).
  // The signal is deferred to the end of this method so a re-add issued from the change
  // callback sees this member as a fully detached, plain Selectable.
  View previous = group->UnregisterMember(this);

  // Member side: no longer in a group. Done before restoring the click policy so the selection
  // observer (which no-ops once mGroup is null) cannot re-enter group arbitration during
  // teardown.
  mGroup.Reset();

  // Leaving the bound group also ends any parent-auto membership: the member is no longer
  // in ANY group. The declarative layer re-records mAutoGroupParent after a fresh
  // parent-auto bind. (mGroupName is the persistent named intent and is NOT cleared here;
  // it is only cleared by SetGroupName("") / UnbindFromGroup.)
  mAutoGroupParent.Reset();

  if(mAttached)
  {
    // Revert the membership-gated click policy: clear the sibling's internal select-only
    // flag (toggle-by-click untouched), so the member behaves once again as a plain
    // Selectable. THEN restore accessibility (role + the CHECKED bit per the restored role,
    // FIX-5).
    RestoreClickPolicy();
    RestoreAccessibility();
  }

  if(previous)
  {
    // A structural winner removal is a real selection change. Announce it AFTER the
    // member is fully restored (group dropped, interaction reverted, accessibility
    // restored), so a re-add via SetGroupName()/reparenting from this callback succeeds. The cause is
    // Programmatic because the removal is an API/structural action, not a gesture.
    group->EmitSelectedMemberChanged(previous, View(), InputEvent::Programmatic());
  }
}

void GroupSelectableTraitImpl::OnAttached(View& view)
{
  DALI_ASSERT_ALWAYS(!(mOwner.GetHandle()) && "The trait can not be attached multiple target views");
  mOwner    = view;
  mAttached = true;

  // Install the membership-INDEPENDENT selection observer only. The click policy
  // is membership-GATED and is applied by JoinGroup(); in practice mGroup is null here
  // because AsGroupSelectable() attaches the trait BEFORE any declarative JoinGroup() runs. The
  // if(mGroup) branch below is defensive for a (currently impossible) attach-after-join.
  ConnectSelectionObserver();

  // Observe the owner's scene lifetime for parent auto-grouping. These are
  // membership-INDEPENDENT (always connected while attached, dropped on teardown) so the
  // member can join/leave its parent auto-group across scene connection cycles.
  view.SceneConnectedSignal().Connect(this, &GroupSelectableTraitImpl::OnOwnerOnScene);
  view.SceneDisconnectedSignal().Connect(this, &GroupSelectableTraitImpl::OnOwnerOffScene);

  if(mGroup)
  {
    ApplyGroupClickPolicy();
    ApplyRadioAccessibility();
  }

  // If the trait is created while the owner is ALREADY on-scene (AsGroupSelectable() on a
  // connected View), OnSceneSignal will not fire retroactively, so attempt the parent
  // auto-join now. No-op when a name is set (named membership wins) or the owner is not
  // on-scene under a View parent.
  if(mGroupName.empty() && view.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE))
  {
    JoinParentGroupIfApplicable();
  }
}

void GroupSelectableTraitImpl::OnDetaching(View& view)
{
  // CoreInteractionObject is attach-once, so this is effectively a dead path
  // (core-interaction-object.cpp:131 asserts on detach; :100 is the attach-multiple
  // assert). All live teardown also happens in OnViewDestroying and the declarative
  // LeaveGroup() path. Kept for symmetry.
  if(mGroup)
  {
    UnbindFromGroup();
    RestoreClickPolicy();
    RestoreAccessibility();
  }

  DisconnectSelectionObserver();

  // Drop the scene observers connected in OnAttached. Only ever disconnect THIS trait's
  // own handlers; user-connected callbacks on the same signals are untouched.
  if(View owner = mOwner.GetHandle())
  {
    owner.SceneConnectedSignal().Disconnect(this, &GroupSelectableTraitImpl::OnOwnerOnScene);
    owner.SceneDisconnectedSignal().Disconnect(this, &GroupSelectableTraitImpl::OnOwnerOffScene);
  }

  mAttached = false;
  mOwner.Reset();
}

void GroupSelectableTraitImpl::OnViewDestroying(ViewImpl* viewImpl)
{
  // Runs from ~ViewImpl while the CoreInteractionObject (and this trait impl) are
  // still alive. mOwner.GetHandle() is already empty here, which is why the group is
  // keyed on the stable GroupSelectableTraitImpl* identity: UnregisterMember(this)
  // erases the exact entry and clears the winner deterministically. Accessibility
  // restore is meaningless mid-destruction, so it is intentionally skipped.
  UnbindFromGroup();
}

void GroupSelectableTraitImpl::UnbindFromGroup()
{
  if(mGroup)
  {
    // Destroy/detach path only (NOT LeaveGroup). Structural removal followed by dropping
    // the group reference. The returned View is intentionally discarded and NO change
    // signal is emitted: on the destroy path UnregisterMember returns an empty View
    // anyway (mOwner already resolves empty), and destruction must not emit.
    mGroup->UnregisterMember(this);
    mGroup.Reset();
  }

  // Declarative state has no meaning once the trait is being torn down.
  mAutoGroupParent.Reset();
  mGroupName.clear();
}

void GroupSelectableTraitImpl::ConnectSelectionObserver()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  // Membership-INDEPENDENT: install the sibling SelectableTraitImpl's INTERNAL post-commit
  // observer (a stateless function pointer, DispatchCommit, invoked just before the public
  // SelectionChangedSignal) for the whole attached lifetime, so group arbitration settles
  // before any user-facing SelectionChangedSignal callback runs regardless of connection
  // order. The dispatcher re-resolves this view's group trait, and OnSelectionChanged no-ops
  // while mGroup is null, so keeping it installed when ungrouped is harmless.
  SelectableTrait selectable = GetSelectableTrait(GetImpl(owner));
  DALI_ASSERT_ALWAYS(selectable && "GroupSelectableTraitImpl requires SelectableTrait");
  GetImpl(selectable).SetSelectionCommitObserver(&GroupSelectableTraitImpl::DispatchCommit);
}

void GroupSelectableTraitImpl::DisconnectSelectionObserver()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  // Clear the post-commit observer THIS trait installed. User-connected callbacks on the
  // public SelectionChangedSignal are untouched (this trait uses the separate INTERNAL
  // function-pointer observer slot). The dispatcher is stateless, so a missed clear (e.g. the
  // dead detach path) cannot dangle anyway.
  SelectableTrait selectable = GetSelectableTrait(GetImpl(owner));
  if(selectable)
  {
    GetImpl(selectable).SetSelectionCommitObserver(nullptr);
  }
}

void GroupSelectableTraitImpl::ApplyGroupClickPolicy()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  SelectableTrait selectable = GetSelectableTrait(GetImpl(owner));
  DALI_ASSERT_ALWAYS(selectable && "GroupSelectableTraitImpl requires SelectableTrait");

  // Set the sibling's internal select-only flag so its OnClickedForToggle keeps an
  // already-selected member selected (a re-click on the winner is a no-op). Toggle-by-click
  // is NOT touched: grouping neither saves nor restores it.
  GetImpl(selectable).SetSelectOnlyByClick(true);
}

void GroupSelectableTraitImpl::RestoreClickPolicy()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  SelectableTrait selectable = GetSelectableTrait(GetImpl(owner));
  if(selectable)
  {
    // Clear the internal select-only flag so the ungrouped member behaves exactly as a
    // plain Selectable again. Toggle-by-click is untouched (grouping never changed it).
    GetImpl(selectable).SetSelectOnlyByClick(false);
  }
}

void GroupSelectableTraitImpl::DispatchCommit(View view, bool selected, InputEvent event)
{
  // Stateless post-commit dispatcher installed on the sibling SelectableTraitImpl. It captures
  // NO object identity (so it can never dangle); instead it re-resolves THIS view's group trait
  // from the owner View and forwards to the instance handler. Resolution mirrors the sibling
  // lookups in this file (GetSelectableTrait) and in selectable-trait-impl.cpp.
  CoreInteractionObject* core = ViewDataImpl::Get(GetImpl(view)).GetCoreInteractionObject();
  if(!core)
  {
    return;
  }
  if(GroupSelectableTraitImpl* group = core->GetGroupSelectableTraitImpl())
  {
    group->OnSelectionChanged(view, selected, event);
  }
}

void GroupSelectableTraitImpl::OnSelectionChanged(View view, bool selected, InputEvent event)
{
  // Post-commit: the SelectableTraitImpl invokes its internal commit observer right after the
  // state is committed and just before the public SelectionChangedSignal in SetSelectedInternal.
  // Mirror CHECKED accessibility first.
  if(mGroup)
  {
    WriteCheckedState(view, selected);

    if(selected)
    {
      // The group records the new winner BEFORE unselecting the previous one, so
      // the displaced winner's re-entrant SelectionChanged(false) is ignored
      // (winner != self). Exactly one group signal fires, from OnMemberSelected.
      mGroup->OnMemberSelected(this, event);
    }
    else
    {
      // Reached only by programmatic SetSelected(false) on the winner or
      // SelectionGroupImpl::ClearSelection(); never by gesture.
      mGroup->OnMemberUnselected(this, event);
    }
  }
}

void GroupSelectableTraitImpl::ApplyRadioAccessibility()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  if(!mRoleCached)
  {
    mPreviousRole = owner.GetProperty<int32_t>(View::Property::ACCESSIBILITY_ROLE);
    mRoleCached   = true;
  }

  owner.SetProperty(View::Property::ACCESSIBILITY_ROLE, static_cast<int32_t>(AccessibilityRole::RADIO_BUTTON));

  // Seed CHECKED from the member's current selected state, reusing the sibling-handle
  // helper (mirrors selectable-trait-impl.cpp:34-38) rather than an inline re-DownCast.
  SelectableTrait selectable = GetSelectableTrait(GetImpl(owner));
  WriteCheckedState(owner, selectable && selectable.IsSelected());
}

void GroupSelectableTraitImpl::RestoreAccessibility()
{
  View owner = mOwner.GetHandle();
  if(!owner)
  {
    return;
  }

  // Capture the role being restored BEFORE mRoleCached/mPreviousRole are reset below.
  // If the role was never cached (ApplyRadioAccessibility never ran), the member never
  // became RADIO_BUTTON, so fall back to the View's current role.
  const int32_t restoredRole = mRoleCached ? mPreviousRole : owner.GetProperty<int32_t>(View::Property::ACCESSIBILITY_ROLE);

  if(mRoleCached)
  {
    owner.SetProperty(View::Property::ACCESSIBILITY_ROLE, mPreviousRole);
    mRoleCached = false;
  }

  // CHECKED is meaningful only for checkable roles; after leaving the group the role is
  // restored (typically NONE), so the bit must follow the restored role, not raw
  // IsSelected(). The logical/visual SELECTED state is carried independently by
  // ViewState::SELECTED, so clearing CHECKED here does not unselect the member. For the
  // typical NONE case this yields CHECKED=false (same as the previous unconditional clear).
  const bool      checkable  = (restoredRole == static_cast<int32_t>(AccessibilityRole::CHECK_BOX) ||
                          restoredRole == static_cast<int32_t>(AccessibilityRole::RADIO_BUTTON) ||
                          restoredRole == static_cast<int32_t>(AccessibilityRole::TOGGLE_BUTTON));
  SelectableTrait selectable = GetSelectableTrait(GetImpl(owner));
  WriteCheckedState(owner, checkable && selectable && selectable.IsSelected());
}

void GroupSelectableTraitImpl::WriteCheckedState(View view, bool checked)
{
  if(!view)
  {
    return;
  }

  // ACCESSIBILITY_STATES is a whole-bitset INTEGER property whose SetProperty
  // replaces the value wholesale (view-data-impl.cpp). Use read-modify-write so
  // other bits (e.g. ENABLED) survive.
  int32_t raw = view.GetProperty<int32_t>(View::Property::ACCESSIBILITY_STATES);
  view.SetProperty(View::Property::ACCESSIBILITY_STATES, checked ? (raw | CHECKED_MASK) : (raw & ~CHECKED_MASK));
}

} // namespace Dali::Ui::Internal
