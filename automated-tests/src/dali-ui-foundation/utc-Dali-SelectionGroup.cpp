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

#include <stdlib.h>
#include <iostream>

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/group-selectable-trait.h>
#include <dali-ui-foundation/public-api/selection-group.h>
#include <dali-ui-test-suite-utils.h>
#include <test-gesture-generator.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

// ============================================================================
// Signal callback helpers (mirror SelectionChangedSignalData in
// utc-Dali-SelectableTrait.cpp, adapted to (previous, current, event))
// ============================================================================

struct SelectedMemberChangedData
{
  void Reset()
  {
    called   = false;
    count    = 0;
    previous = View();
    current  = View();
    event    = InputEvent();
  }

  bool       called = false;
  int        count  = 0;
  View       previous;
  View       current;
  InputEvent event;
};

struct SelectedMemberChangedFunctor
{
  SelectedMemberChangedFunctor(SelectedMemberChangedData& data)
  : signalData(data)
  {
  }

  void operator()(View previous, View current, InputEvent event)
  {
    signalData.called = true;
    ++signalData.count;
    signalData.previous = previous;
    signalData.current  = current;
    signalData.event    = event;
  }

  SelectedMemberChangedData& signalData;
};

// Creates a View sized for tapping at a given scene position.
View CreateSceneView(UiTestApplication& application, float x = 0.0f, float y = 0.0f)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::POSITION, Vector3(x, y, 0.0f));
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  return view;
}

// Creates a View (NOT on a scene) and binds it to the named group. Named membership is
// the replacement for the removed SelectionGroup::Add(): it does NOT require the View to
// be on-scene, mirroring the old Add() behaviour for off-scene Views.
View CreateNamedMember(const std::string& name)
{
  View view = View::New();
  view.AsGroupSelectable().SetGroupName(name.c_str());
  return view;
}

// Creates an on-scene View and binds it to the named group, ready for tapping.
View CreateNamedSceneMember(UiTestApplication& application, const std::string& name, float x = 0.0f, float y = 0.0f)
{
  View view = CreateSceneView(application, x, y);
  view.AsGroupSelectable().SetGroupName(name.c_str());
  return view;
}

} // namespace

void utc_dali_selectiongroup_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectiongroup_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Construction / Handle  (BaseHandle Rule-of-5 collaborator, obtained via Find)
// ============================================================================

int UtcDaliSelectionGroupFindByNameP(void)
{
  UiTestApplication application;
  SelectionGroup    group = SelectionGroup::Find("UtcFindByName");
  DALI_TEST_CHECK(group);
  END_TEST;
}

int UtcDaliSelectionGroupFindByNameSameHandleP(void)
{
  UiTestApplication application;
  // While alive, Find(name) returns the same group for the same name. Keep a member so
  // the group is not purged between the two lookups.
  View member = CreateNamedMember("UtcFindSame");
  SelectionGroup a = SelectionGroup::Find("UtcFindSame");
  SelectionGroup b = SelectionGroup::Find("UtcFindSame");
  DALI_TEST_CHECK(a);
  DALI_TEST_CHECK(a == b);
  END_TEST;
}

int UtcDaliSelectionGroupFindByParentP(void)
{
  UiTestApplication application;
  View              parent = CreateSceneView(application, 0.0f, 0.0f);
  SelectionGroup    group  = SelectionGroup::Find(parent);
  DALI_TEST_CHECK(group);
  END_TEST;
}

int UtcDaliSelectionGroupDefaultConstructorN(void)
{
  UiTestApplication application;
  SelectionGroup    group;
  DALI_TEST_CHECK(!group);
  END_TEST;
}

int UtcDaliSelectionGroupCopyConstructorP(void)
{
  UiTestApplication application;
  View              member = CreateNamedMember("UtcCopyCtor");
  SelectionGroup    group  = SelectionGroup::Find("UtcCopyCtor");
  SelectionGroup    copy(group);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == group);
  END_TEST;
}

int UtcDaliSelectionGroupMoveConstructorP(void)
{
  UiTestApplication application;
  View              member = CreateNamedMember("UtcMoveCtor");
  SelectionGroup    group  = SelectionGroup::Find("UtcMoveCtor");
  SelectionGroup    moved(std::move(group));
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!group);
  END_TEST;
}

int UtcDaliSelectionGroupCopyAssignmentP(void)
{
  UiTestApplication application;
  View              member = CreateNamedMember("UtcCopyAssign");
  SelectionGroup    group  = SelectionGroup::Find("UtcCopyAssign");
  SelectionGroup    copy;
  copy = group;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == group);
  END_TEST;
}

int UtcDaliSelectionGroupMoveAssignmentP(void)
{
  UiTestApplication application;
  View              member = CreateNamedMember("UtcMoveAssign");
  SelectionGroup    group  = SelectionGroup::Find("UtcMoveAssign");
  SelectionGroup    moved;
  moved = std::move(group);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!group);
  END_TEST;
}

int UtcDaliSelectionGroupDownCastP(void)
{
  UiTestApplication application;
  View              member = CreateNamedMember("UtcDownCast");
  SelectionGroup    group  = SelectionGroup::Find("UtcDownCast");
  BaseHandle        handle(group);
  SelectionGroup    downcast = SelectionGroup::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliSelectionGroupDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        handle;
  SelectionGroup    downcast = SelectionGroup::DownCast(handle);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// ============================================================================
// Membership: established declaratively via SetGroupName; GetMemberCount
// ============================================================================

// Binding a View to a named group makes it GroupSelectable and registers it as a member.
int UtcDaliSelectionGroupNamedMembershipP(void)
{
  UiTestApplication application;
  View              view = View::New();

  DALI_TEST_CHECK(!view.IsGroupSelectable());

  view.AsGroupSelectable().SetGroupName("UtcNamedMembership");

  SelectionGroup group = SelectionGroup::Find("UtcNamedMembership");
  DALI_TEST_CHECK(view.IsGroupSelectable());
  DALI_TEST_EQUALS(group.GetMemberCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(view.AsGroupSelectable().GetGroup() == group);
  END_TEST;
}

int UtcDaliSelectionGroupNamedMembershipMultipleP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcMultiple");
  View              b = CreateNamedMember("UtcMultiple");
  View              c = CreateNamedMember("UtcMultiple");

  SelectionGroup group = SelectionGroup::Find("UtcMultiple");
  DALI_TEST_EQUALS(group.GetMemberCount(), 3u, TEST_LOCATION);
  END_TEST;
}

// Setting the same name twice is idempotent (still one membership).
int UtcDaliSelectionGroupNamedMembershipIdempotentP(void)
{
  UiTestApplication application;
  View              view = View::New();

  view.AsGroupSelectable().SetGroupName("UtcIdempotent");
  view.AsGroupSelectable().SetGroupName("UtcIdempotent");

  SelectionGroup group = SelectionGroup::Find("UtcIdempotent");
  DALI_TEST_EQUALS(group.GetMemberCount(), 1u, TEST_LOCATION);
  END_TEST;
}

// Clearing the name (off-scene, no parent) removes the View from the group.
int UtcDaliSelectionGroupClearNameRemovesP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcClearName");
  View              b = CreateNamedMember("UtcClearName");

  SelectionGroup group = SelectionGroup::Find("UtcClearName");
  DALI_TEST_EQUALS(group.GetMemberCount(), 2u, TEST_LOCATION);

  // Clear a's name. It is off-scene with no parent, so it falls into no group.
  a.AsGroupSelectable().SetGroupName("");
  DALI_TEST_EQUALS(group.GetMemberCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!a.AsGroupSelectable().GetGroup());
  END_TEST;
}

// Clearing the name of a member that is not the winner does not affect a surviving winner.
int UtcDaliSelectionGroupClearNameNonWinnerN(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcClearNonWinner");
  View              b = CreateNamedMember("UtcClearNonWinner");

  SelectionGroup group = SelectionGroup::Find("UtcClearNonWinner");

  b.AsGroupSelectable().SetGroupName(""); // b is not the winner
  DALI_TEST_EQUALS(group.GetMemberCount(), 1u, TEST_LOCATION);
  END_TEST;
}

// Clearing the name of the winner clears the selection (member's own bool preserved).
int UtcDaliSelectionGroupClearNameWinnerClearsSelectionP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcClearWinner", 0.0f, 0.0f);
  View              b = CreateNamedSceneMember(application, "UtcClearWinner", 200.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcClearWinner");

  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  a.AsGroupSelectable().SetGroupName("");
  DALI_TEST_CHECK(!group.GetSelectedMember());
  // a's own selected bit is preserved.
  DALI_TEST_CHECK(a.AsSelectable().IsSelected());
  END_TEST;
}

// ============================================================================
// Single-selection mutual exclusion
// ============================================================================

// Programmatically selecting a second member unselects the first (radio).
int UtcDaliSelectionGroupSingleSelectionSwapP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcSwap", 0.0f, 0.0f);
  View              b = CreateNamedSceneMember(application, "UtcSwap", 200.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcSwap");

  SelectableTrait sa = a.AsSelectable();
  SelectableTrait sb = b.AsSelectable();

  sa.SetSelected(true);
  DALI_TEST_CHECK(sa.IsSelected());
  DALI_TEST_CHECK(!sb.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  sb.SetSelected(true);
  DALI_TEST_CHECK(!sa.IsSelected());
  DALI_TEST_CHECK(sb.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == b);
  END_TEST;
}

// A gesture tap drives the group arbitration: tapping a member selects it and,
// when another member was already selected, unselects the previous winner so only
// one member remains selected.
//
// Both members occupy the scene origin (a bare DALi UI View added to the Scene is
// not repositioned by Actor::Property::POSITION), so the tap is received by the
// topmost member (b, added last). Pre-selecting a programmatically lets the tap
// exercise the gesture-driven swap a -> b.
int UtcDaliSelectionGroupSingleSelectionByTapP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcByTap", 0.0f, 0.0f);
  View              b = CreateNamedSceneMember(application, "UtcByTap", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcByTap");

  SelectableTrait sa = a.AsSelectable();
  SelectableTrait sb = b.AsSelectable();

  // Pre-select a programmatically (a is the current winner).
  sa.SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  // Tap the topmost member b -> gesture selects b and unselects a (radio swap).
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(!sa.IsSelected());
  DALI_TEST_CHECK(sb.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == b);

  // Re-tapping the winner is a no-op (true radio); it stays selected and the group
  // is never emptied by a gesture.
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(sb.IsSelected());
  DALI_TEST_CHECK(group.GetSelectedMember() == b);
  END_TEST;
}

// Only one member can be selected at any time, regardless of order.
int UtcDaliSelectionGroupAtMostOneSelectedP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcAtMostOne", 0.0f, 0.0f);
  View              b = CreateNamedSceneMember(application, "UtcAtMostOne", 200.0f, 0.0f);
  View              c = CreateNamedSceneMember(application, "UtcAtMostOne", 0.0f, 200.0f);

  SelectionGroup group = SelectionGroup::Find("UtcAtMostOne");

  a.AsSelectable().SetSelected(true);
  b.AsSelectable().SetSelected(true);
  c.AsSelectable().SetSelected(true);

  int selectedCount = 0;
  selectedCount += a.AsSelectable().IsSelected() ? 1 : 0;
  selectedCount += b.AsSelectable().IsSelected() ? 1 : 0;
  selectedCount += c.AsSelectable().IsSelected() ? 1 : 0;

  DALI_TEST_EQUALS(selectedCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(group.GetSelectedMember() == c);
  END_TEST;
}

// ============================================================================
// SelectedMemberChangedSignal (previous, current, event)
// ============================================================================

int UtcDaliSelectionGroupSignalOnFirstSelectionP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcSigFirst", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcSigFirst");

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  a.AsSelectable().SetSelected(true);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!data.previous);  // no previous winner
  DALI_TEST_CHECK(data.current == a);
  END_TEST;
}

// On a swap the group signal fires exactly once with previous=old, current=new.
int UtcDaliSelectionGroupSignalOnSwapP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcSigSwap", 0.0f, 0.0f);
  View              b = CreateNamedSceneMember(application, "UtcSigSwap", 200.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcSigSwap");

  a.AsSelectable().SetSelected(true);

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  b.AsSelectable().SetSelected(true);

  // Exactly one group emit despite the re-entrant unselect of the old winner.
  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.previous == a);
  DALI_TEST_CHECK(data.current == b);
  END_TEST;
}

// Unselecting the winner emits (previous=winner, current=empty).
int UtcDaliSelectionGroupSignalOnEmptyP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcSigEmpty", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcSigEmpty");
  a.AsSelectable().SetSelected(true);

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  a.AsSelectable().SetSelected(false);

  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.previous == a);
  DALI_TEST_CHECK(!data.current);  // empty -> no selection
  END_TEST;
}

// The originating cause is propagated: a programmatic unselect is Programmatic.
int UtcDaliSelectionGroupSignalCauseProgrammaticP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcSigCause", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcSigCause");
  a.AsSelectable().SetSelected(true);

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  a.AsSelectable().SetSelected(false);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.event.IsProgrammatic());
  END_TEST;
}

// ============================================================================
// ClearSelection (one of the routes to an empty group)
// ============================================================================

int UtcDaliSelectionGroupClearSelectionP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcClearSel", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcClearSel");
  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  group.ClearSelection();
  DALI_TEST_CHECK(!group.GetSelectedMember());
  DALI_TEST_CHECK(!a.AsSelectable().IsSelected());
  END_TEST;
}

int UtcDaliSelectionGroupClearSelectionEmitsSignalP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcClearSelSig", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcClearSelSig");
  a.AsSelectable().SetSelected(true);

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  group.ClearSelection();

  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.previous == a);
  DALI_TEST_CHECK(!data.current);
  END_TEST;
}

// ClearSelection on an already-empty group is a no-op (no signal).
int UtcDaliSelectionGroupClearSelectionWhenEmptyN(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcClearEmpty", 0.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcClearEmpty");

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  group.ClearSelection();

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(!group.GetSelectedMember());
  END_TEST;
}

// ============================================================================
// GetSelectedMember
// ============================================================================

int UtcDaliSelectionGroupGetSelectedMemberEmptyByDefaultP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcGetSelEmpty");

  SelectionGroup group = SelectionGroup::Find("UtcGetSelEmpty");
  DALI_TEST_CHECK(!group.GetSelectedMember());
  END_TEST;
}

// ============================================================================
// Lifetime: no ownership cycle; member destroy auto-unregisters
// ============================================================================

// The group holds weak references to its members, so members are not kept alive
// by the group.
int UtcDaliSelectionGroupNoOwnershipCycleP(void)
{
  UiTestApplication application;

  SelectionGroup group = SelectionGroup::Find("UtcNoCycle");
  {
    View view = View::New();
    view.AsGroupSelectable().SetGroupName("UtcNoCycle");
    DALI_TEST_EQUALS(group.GetMemberCount(), 1u, TEST_LOCATION);
    // view goes out of scope here; group's weak ref must not keep it alive.
  }

  application.SendNotification();
  application.Render();

  // The dead member is auto-unregistered from the group at destruction.
  DALI_TEST_EQUALS(group.GetMemberCount(), 0u, TEST_LOCATION);
  END_TEST;
}

// A member that is destroyed is auto-unregistered from the group even when the
// group already has a (different, surviving) selected winner. This exercises the
// destroy-time unregister path (keyed on the stable GroupSelectableTraitImpl*
// identity, so it works mid-destruction) with a live winner present, and verifies
// the surviving winner is left untouched.
//
// NOTE: the destroyed member is deliberately NOT the selected one. Selecting a View
// engages the global ViewStateManager transition machinery, which retains a handle
// to the View, so a *selected* off-scene View is not destroyed merely by dropping
// its handle. The winner-cleared-on-destroy branch shares the same code path
// (UnregisterMember erases the member and clears mWinner if it matches) and the
// winner==nullptr seed of that path is covered by NoOwnershipCycleP.
int UtcDaliSelectionGroupMemberDestroyUnregistersWithWinnerP(void)
{
  UiTestApplication application;
  View              winner = CreateNamedMember("UtcDestroyWithWinner");

  SelectionGroup group = SelectionGroup::Find("UtcDestroyWithWinner");
  winner.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == winner);

  {
    View other = View::New();
    other.AsGroupSelectable().SetGroupName("UtcDestroyWithWinner");
    DALI_TEST_EQUALS(group.GetMemberCount(), 2u, TEST_LOCATION);
    // other (never selected) goes out of scope here -> ViewImpl destroyed.
  }

  // The destroyed member is unregistered; the surviving winner is untouched.
  DALI_TEST_EQUALS(group.GetMemberCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(group.GetSelectedMember() == winner);
  END_TEST;
}

// The group itself can outlive having members and remains valid.
int UtcDaliSelectionGroupOutlivesMembersP(void)
{
  UiTestApplication application;

  SelectionGroup group = SelectionGroup::Find("UtcOutlives");
  {
    View a = View::New();
    View b = View::New();
    a.AsGroupSelectable().SetGroupName("UtcOutlives");
    b.AsGroupSelectable().SetGroupName("UtcOutlives");
    a.AsGroupSelectable().SetGroupName("");
    b.AsGroupSelectable().SetGroupName("");
  }

  DALI_TEST_CHECK(group);
  DALI_TEST_EQUALS(group.GetMemberCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!group.GetSelectedMember());
  END_TEST;
}

// ============================================================================
// Join behaviour relative to existing winner
// ============================================================================

// Joining while already selected, when the group is empty, seeds the winner
// without emitting a group signal.
int UtcDaliSelectionGroupJoinSelectedSeedsWinnerP(void)
{
  UiTestApplication application;
  View              a = CreateSceneView(application, 0.0f, 0.0f);

  // Make selectable & select before naming any group.
  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(a.AsSelectable().IsSelected());

  SelectionGroup group = SelectionGroup::Find("UtcSeedWinner");

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  a.AsGroupSelectable().SetGroupName("UtcSeedWinner");

  DALI_TEST_CHECK(group.GetSelectedMember() == a);
  DALI_TEST_CHECK(!data.called); // seeding does not emit
  END_TEST;
}

// Joining while selected, when the group already has a winner, forces the
// joining member to unselect to preserve at-most-one-selected.
int UtcDaliSelectionGroupJoinSelectedWithExistingWinnerP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcJoinWithWinner", 0.0f, 0.0f);
  View              b = CreateSceneView(application, 200.0f, 0.0f);

  SelectionGroup group = SelectionGroup::Find("UtcJoinWithWinner");
  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  // b is selected before joining.
  b.AsSelectable().SetSelected(true);

  b.AsGroupSelectable().SetGroupName("UtcJoinWithWinner");

  // The existing winner is preserved; the joining member is forced false.
  DALI_TEST_CHECK(group.GetSelectedMember() == a);
  DALI_TEST_CHECK(a.AsSelectable().IsSelected());
  DALI_TEST_CHECK(!b.AsSelectable().IsSelected());
  END_TEST;
}

// ============================================================================
// FIX-1: re-entrancy in a nested user-callback cascade (no stale terminal emit)
// ============================================================================

// A user callback on member A's SelectableTrait::SelectionChangedSignal selects C while A
// is being unselected as part of selecting B. The final winner must be C, and NO terminal
// group SelectedMemberChangedSignal may announce current == B (the superseded election).
int UtcDaliSelectionGroupReentrantSelectionNoStaleEmitP(void)
{
  UiTestApplication application;
  View              a = CreateNamedSceneMember(application, "UtcReentrant", 0.0f, 0.0f);
  View              b = CreateNamedSceneMember(application, "UtcReentrant", 200.0f, 0.0f);
  View              c = CreateNamedSceneMember(application, "UtcReentrant", 0.0f, 200.0f);

  SelectionGroup group = SelectionGroup::Find("UtcReentrant");

  // A is the current winner.
  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  // When A is unselected (which happens while electing B), re-enter and select C.
  bool reentered = false;
  a.AsSelectable().SelectionChangedSignal().Connect(&application, [&](View, bool selected, InputEvent) {
    if(!selected && !reentered)
    {
      reentered = true;
      c.AsSelectable().SetSelected(true);
    }
  });

  // Observe EVERY group emission and record whether B was ever announced as current.
  bool announcedB    = false;
  bool announcedC    = false;
  int  terminalCount = 0;
  group.SelectedMemberChangedSignal().Connect(&application, [&](View, View current, InputEvent) {
    ++terminalCount;
    if(current == b)
    {
      announcedB = true;
    }
    if(current == c)
    {
      announcedC = true;
    }
  });

  // Elect B; the cascade re-selects C from inside A's unselect callback.
  b.AsSelectable().SetSelected(true);

  // Final winner is C, not B.
  DALI_TEST_CHECK(group.GetSelectedMember() == c);
  DALI_TEST_CHECK(c.AsSelectable().IsSelected());
  DALI_TEST_CHECK(!b.AsSelectable().IsSelected());
  DALI_TEST_CHECK(!a.AsSelectable().IsSelected());

  // The superseded inner election of B must NOT produce a terminal emit; only the final
  // C election is announced (the mWinner==member guard suppresses the stale B emit).
  DALI_TEST_CHECK(reentered);
  DALI_TEST_CHECK(!announcedB);
  DALI_TEST_CHECK(announcedC);
  DALI_TEST_EQUALS(terminalCount, 1, TEST_LOCATION);
  END_TEST;
}

// ============================================================================
// FIX-2: structural winner removal emits (previous, empty) exactly once
// ============================================================================

// Clearing the winner's name (off-scene, no parent fallback) is the structural removal of
// the winner: it emits SelectedMemberChangedSignal(previous=winner, current=empty) once.
int UtcDaliSelectionGroupRemoveWinnerEmitsSignalP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcRemoveWinnerEmits");
  View              b = CreateNamedMember("UtcRemoveWinnerEmits");

  SelectionGroup group = SelectionGroup::Find("UtcRemoveWinnerEmits");

  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  a.AsGroupSelectable().SetGroupName(""); // off-scene, no parent -> leaves the group entirely

  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.previous == a);
  DALI_TEST_CHECK(!data.current);
  DALI_TEST_CHECK(!group.GetSelectedMember());
  // The removed winner keeps its own selected state.
  DALI_TEST_CHECK(a.AsSelectable().IsSelected());
  END_TEST;
}

// Clearing the name of a NON-winner member must NOT emit the change signal.
int UtcDaliSelectionGroupRemoveNonWinnerNoSignalN(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcRemoveNonWinner");
  View              b = CreateNamedMember("UtcRemoveNonWinner");

  SelectionGroup group = SelectionGroup::Find("UtcRemoveNonWinner");

  a.AsSelectable().SetSelected(true); // a is the winner

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  b.AsGroupSelectable().SetGroupName(""); // remove the non-winner

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);
  END_TEST;
}

// ============================================================================
// FIX-2: cross-group move of a winner emits (A, empty) once from the OLD group
// ============================================================================

// A is the winner of named group g1; changing A's name to g2 moves A. The move goes
// JoinGroup -> LeaveGroup -> UnregisterMember on g1, which emits (A, empty) from g1 once,
// and g1.GetSelectedMember() becomes empty. A's own selected bit is preserved, so A seeds
// as the winner of g2.
int UtcDaliSelectionGroupCrossGroupMoveEmitsFromOldGroupP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcCrossMoveG1");

  SelectionGroup g1 = SelectionGroup::Find("UtcCrossMoveG1");
  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(g1.GetSelectedMember() == a);

  SelectedMemberChangedData    g1data;
  SelectedMemberChangedFunctor g1functor(g1data);
  g1.SelectedMemberChangedSignal().Connect(&application, g1functor);

  a.AsGroupSelectable().SetGroupName("UtcCrossMoveG2"); // moves a from g1 to g2
  SelectionGroup g2 = SelectionGroup::Find("UtcCrossMoveG2");

  // g1 emits (a, empty) exactly once and is now empty.
  DALI_TEST_EQUALS(g1data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(g1data.previous == a);
  DALI_TEST_CHECK(!g1data.current);
  DALI_TEST_CHECK(!g1.GetSelectedMember());

  // a is now a member of g2 only.
  DALI_TEST_EQUALS(g1.GetMemberCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(g2.GetMemberCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(a.AsGroupSelectable().GetGroup() == g2);

  // a kept its own selected bit, so it seeds as the winner of g2.
  DALI_TEST_CHECK(a.AsSelectable().IsSelected());
  DALI_TEST_CHECK(g2.GetSelectedMember() == a);
  END_TEST;
}

// ============================================================================
// P2-1: deferred structural-removal signal (emit AFTER full member teardown)
// ============================================================================

// P2-1a: Clearing the winner's name (no parent fallback) defers the change signal to AFTER
// the member is fully torn down. At emit time the removed member is already a plain
// Selectable again: its AsGroupSelectable().GetGroup() is empty and the group reports no
// selected member. This proves there is no stale member-side state visible from the
// change callback.
int UtcDaliSelectionGroupRemoveWinnerDeferredEmitNoStaleStateP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcDeferredNoStale");
  View              b = CreateNamedMember("UtcDeferredNoStale");

  SelectionGroup group = SelectionGroup::Find("UtcDeferredNoStale");

  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  bool callbackRan       = false;
  bool memberGroupEmpty  = false;
  bool groupSelectionNil = false;
  group.SelectedMemberChangedSignal().Connect(&application, [&](View previous, View, InputEvent) {
    callbackRan = true;
    // The removed winner is fully detached at emit time: it no longer belongs to a group,
    // and the group already reports no selected member.
    memberGroupEmpty  = !previous.AsGroupSelectable().GetGroup();
    groupSelectionNil = !group.GetSelectedMember();
  });

  a.AsGroupSelectable().SetGroupName("");

  DALI_TEST_CHECK(callbackRan);
  DALI_TEST_CHECK(memberGroupEmpty);
  DALI_TEST_CHECK(groupSelectionNil);
  END_TEST;
}

// P2-1b: re-add the removed winner from inside the SelectedMemberChangedSignal callback by
// re-setting its name. Because the signal is emitted AFTER teardown (mGroup is null,
// interaction/accessibility restored), the re-add via SetGroupName() is honored (JoinGroup
// proceeds) and is NOT lost. After control returns the View is a member again.
int UtcDaliSelectionGroupRemoveWinnerReAddInCallbackNotLostP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcDeferredReAdd");
  View              b = CreateNamedMember("UtcDeferredReAdd");

  SelectionGroup group = SelectionGroup::Find("UtcDeferredReAdd");

  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  // Re-add the removed member from inside the change callback (guarded against recursion).
  bool reAdded = false;
  group.SelectedMemberChangedSignal().Connect(&application, [&](View previous, View, InputEvent) {
    if(!reAdded && previous)
    {
      reAdded = true;
      previous.AsGroupSelectable().SetGroupName("UtcDeferredReAdd");
    }
  });

  a.AsGroupSelectable().SetGroupName("");

  // The re-add is not silently lost: a is a member of the group again.
  DALI_TEST_CHECK(reAdded);
  DALI_TEST_CHECK(a.AsGroupSelectable().GetGroup() == group);
  DALI_TEST_EQUALS(group.GetMemberCount(), 2u, TEST_LOCATION);

  // The re-added member still works: selecting it makes it the winner.
  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);
  END_TEST;
}

// P2-1c: regression -- after deferring the emit, clearing the winner's name still emits
// (previous=winner, current=empty) EXACTLY ONCE (no double emit, no lost emit).
int UtcDaliSelectionGroupRemoveWinnerEmitsOnceP(void)
{
  UiTestApplication application;
  View              a = CreateNamedMember("UtcDeferredOnce");
  View              b = CreateNamedMember("UtcDeferredOnce");

  SelectionGroup group = SelectionGroup::Find("UtcDeferredOnce");

  a.AsSelectable().SetSelected(true);
  DALI_TEST_CHECK(group.GetSelectedMember() == a);

  SelectedMemberChangedData    data;
  SelectedMemberChangedFunctor functor(data);
  group.SelectedMemberChangedSignal().Connect(&application, functor);

  a.AsGroupSelectable().SetGroupName("");

  DALI_TEST_EQUALS(data.count, 1, TEST_LOCATION);
  DALI_TEST_CHECK(data.previous == a);
  DALI_TEST_CHECK(!data.current);
  DALI_TEST_CHECK(!group.GetSelectedMember());
  END_TEST;
}
