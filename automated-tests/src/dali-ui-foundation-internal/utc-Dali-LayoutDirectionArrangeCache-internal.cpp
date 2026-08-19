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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/internal/layouts/layout-invalidation-generation.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/actors/actor-devel.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Internal::ViewDataImpl;

namespace LIE = Dali::Ui::Internal::LayoutInvalidation;

void utc_dali_layout_direction_arrange_cache_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layout_direction_arrange_cache_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// White-box coverage for the layout direction as an ARRANGE-CACHE dependency.
//
// Two separate things are pinned here, neither of which is visible through
// geometry: (1) a layout-direction change clears mArrangeCacheValid on every
// affected view, and (2) a clean arrange pass records the direction it produced
// its result under, in mLastArrangeDirection. The recorded direction is the
// cache KEY term that keeps a future cache HIT honest: the direction lives in
// dali-core and can be moved through actors dali-ui does not own, so a missed
// invalidation must degrade to "no hit", never to an arrangement mirrored the
// wrong way round.
//
// Both are cache bookkeeping rather than geometry, so they are observed here
// through ViewDataImpl's white-box accessors. The arrange cache-HIT path that
// consumes them is exercised from the outside in utc-Dali-ArrangeCacheHit-internal
// and in the UtcDaliViewArrangeCache* cases of utc-Dali-View.
//
// THE MECHANISM under test, since it is no longer one connection per View:
//  - a layout root that registers with the LayoutController ON A LIVE WINDOW
//    connects the actor layout-direction signal lazily, once, in
//    ViewDataImpl::RegisterWithLayoutController(), and its handler
//    (OnLayoutDirectionChanged) walks the subtree with
//    InvalidateSubtreeLayoutForDirectionChange();
//  - a direction property WRITE on any View reaches
//    ViewDataImpl::OnPropertySet, which raises the same walk -- that is what
//    covers a mid-tree View holding no hook of its own, and an off-scene write;
//  - an OFF-SCENE direction move needs no hook: no pass can run without a window,
//    and reconnection drops the subtree's caches before registering;
//  - the walk PRUNES at any child that holds a direction of its own, mirroring
//    dali-core's inherit walk, so such a child keeps its cache entry;
//  - mLastArrangeDirection remains the KEY-term backstop under all of it.

namespace
{
ViewDataImpl& DataOf(View view)
{
  return ViewDataImpl::Get(GetImpl(view));
}

// Drives one full layout batch to completion.
void Settle(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
}

} // namespace

// Required test: a layout-direction change must invalidate the arrange cache of
// every view whose EFFECTIVE direction moved -- the view it was set on and every
// descendant that inherits it -- with no layout pass in between. The invalidation
// itself, not a subsequent pass, is what has to clear the caches.
//
// Here the change is set on the layout ROOT, which is the one View that does hold
// the actor signal hook, so this exercises the hook path and the subtree walk it
// drives.
//
// Non-vacuity (verified by mutation): emptying the body of
// ViewDataImpl::OnLayoutDirectionChanged leaves both caches valid and the second
// half of this test fails. It stays non-vacuous under the OnPropertySet
// interception, because a HOOKED view's OnPropertySet defers to the hook (it
// returns as soon as it sees mLayoutDirectionSignalConnected) and therefore raises
// no walk of its own.
int UtcDaliLayoutDirectionArrangeCacheInvalidatedByDirectionChangeP(void)
{
  UiTestApplication application;
  tet_infoline("Arrange cache of a settled subtree is dropped by a layout-direction change");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  root.Add(child);

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  // Set on the root only; the child merely inherits the change. The root is the
  // one view holding the signal hook, and its handler walks down to the child.
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(root).IsArrangeDirty());
  DALI_TEST_CHECK(DataOf(child).IsArrangeDirty());

  END_TEST;
}

// A clean arrange pass records the effective direction its result was produced
// under, alongside the input bounds it was produced for. Both are cache KEY
// terms and are only meaningful while mArrangeCacheValid is true, so that is
// asserted first.
//
// Non-vacuity (verified by mutation): removing the `mLastArrangeDirection = ...`
// write from the arrange publish block leaves the member at its constructed
// LEFT_TO_RIGHT and the RIGHT_TO_LEFT half of this test fails.
int UtcDaliLayoutDirectionArrangeCachePublishRecordsDirectionP(void)
{
  UiTestApplication application;
  tet_infoline("A clean arrange pass records the effective layout direction it ran under");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  root.Add(child);

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  DALI_TEST_EQUALS(DataOf(root).GetLastArrangeDirection(), root.GetEffectiveLayoutDirection(), TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), child.GetEffectiveLayoutDirection(), TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(root).GetLastArrangeDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);

  // Under an RTL parent the recorded value follows the INHERITED direction, not
  // a per-view property: the child was never given a direction of its own.
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  Settle(application);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  DALI_TEST_EQUALS(DataOf(root).GetLastArrangeDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), child.GetEffectiveLayoutDirection(), TEST_LOCATION);

  END_TEST;
}

// Phase 4c (Area-2 no-gap pin). A direct actor-geometry write -- the sanctioned
// Extension::SetPositionX escape hatch that ScrollView / RecyclerView use to move
// content without a layout pass -- must NOT invalidate the arrange cache. This
// locks in that ViewDataImpl::OnPropertySet does not treat POSITION as a
// layout-invalidating change; adding an InvalidateArrange() to a POSITION_X case
// there would break scroll performance and fail this test.
int UtcDaliViewArrangeCacheSurvivesUnrelatedActorGeometryWriteP(void)
{
  UiTestApplication application;

  View v = View::New();
  v.SetRequestedWidth(80.0f);
  v.SetRequestedHeight(40.0f);
  application.GetScene().Add(v);
  Settle(application);

  DALI_TEST_CHECK(DataOf(v).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(v).IsArrangeDirty());

  // A geometry write that bypasses layout (the ScrollView/RecyclerView pattern).
  Dali::Ui::Extension::View::SetPositionX(v, 5.0f);

  // The arrange cache must survive it: geometry writes do not feed back into layout.
  DALI_TEST_CHECK(DataOf(v).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(v).IsArrangeDirty());

  END_TEST;
}

// ---------------------------------------------------------------------------
// The right-to-left mirror (ViewDataImpl::ApplyLayoutDirection) is read-compare-write,
// exactly like ApplySelfBoundsIfChanged: it computes the mirrored x from the child's
// published LOGICAL bounds and writes it only when the actor does not already hold it.
// That runs at EVERY node of a cache-hit replay, so an unconditional write cost one
// scene-graph message per direct child per pass for a settled right-to-left subtree.
//
// The two halves of the guard are pinned separately below, because they can fail in
// opposite directions: under-suppression (writing when nothing moved) is a cost, and
// over-suppression (declining to write when the actor is wrong) is a correctness bug.
// ---------------------------------------------------------------------------

namespace
{
LayoutRect SlotOf(View view)
{
  return LayoutRect(view.GetProperty<float>(Actor::Property::POSITION_X),
                    view.GetProperty<float>(Actor::Property::POSITION_Y),
                    view.GetProperty<float>(Actor::Property::SIZE_WIDTH),
                    view.GetProperty<float>(Actor::Property::SIZE_HEIGHT));
}

// A settled right-to-left parent with two direct children, one of which asked for a
// logical x of its own so the mirror is observably not the identity.
void BuildSettledRtlParent(UiTestApplication& application, View& parent, View& first, View& second)
{
  parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(parent);

  first = View::New();
  first.SetRequestedWidth(50.0f);
  first.SetRequestedHeight(20.0f);
  parent.Add(first);

  second = View::New();
  second.SetRequestedX(30.0f);
  second.SetRequestedWidth(40.0f);
  second.SetRequestedHeight(20.0f);
  parent.Add(second);

  Settle(application);
}
} // namespace

// Under-suppression half. A replay of a settled right-to-left subtree must leave every
// mirrored position BIT-identical -- which is the observable consequence of the write
// being skipped, and the property the mirror has to keep whether or not it is guarded.
//
// The stronger claim, "no scene-graph message is sent", is not observable from a UTC:
// the event-side property holds the same value either way, because the mirror is a pure
// function of the child's logical bounds and is therefore idempotent. That half of the
// claim is pinned by review of ApplyLayoutDirection, not by this test. What this test
// does guard is the regression that would make the guard worth removing: a mirror that
// is not idempotent, or one that reads the actor rather than the published logical
// bounds, breaks here immediately.
int UtcDaliLayoutDirectionRtlSettledReplayKeepsMirroredPositionsP(void)
{
  UiTestApplication application;
  tet_infoline("Replaying a settled right-to-left subtree leaves every mirrored position bit-identical");

  View parent, first, second;
  BuildSettledRtlParent(application, parent, first, second);

  DALI_TEST_EQUALS(parent.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(first).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(second).IsArrangeCacheValid());

  const float firstX  = first.GetProperty<float>(Actor::Property::POSITION_X);
  const float secondX = second.GetProperty<float>(Actor::Property::POSITION_X);

  // Mirrored, not logical: `second` asked for a logical x of 30.
  DALI_TEST_CHECK(secondX != 30.0f);

  const LayoutRect parentSlot = SlotOf(parent);

  for(int pass = 0; pass < 3; ++pass)
  {
    parent.Arrange(parentSlot);

    // Exact, not epsilon: an idempotent mirror reproduces the same float, and only an
    // exact comparison can say so.
    DALI_TEST_CHECK(first.GetProperty<float>(Actor::Property::POSITION_X) == firstX);
    DALI_TEST_CHECK(second.GetProperty<float>(Actor::Property::POSITION_X) == secondX);
    DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());
  }

  END_TEST;
}

// Over-suppression half, and the guard's real risk. A geometry write from outside layout
// -- the sanctioned Extension::SetPositionX escape hatch -- does NOT invalidate the
// arrange cache, so the next pass is still a HIT. The mirror must nevertheless notice
// that the actor no longer holds the mirrored value and repair it, exactly as the
// unconditional write did.
//
// Non-vacuity (verified by mutation): inverting the guard in ApplyLayoutDirection to
// `== mirrored` -- write only when the actor already agrees, which is over-suppression
// in its purest form -- leaves both children at their LOGICAL x, so the baseline check
// below and the two post-replay checks fail.
int UtcDaliLayoutDirectionRtlExternalClobberStillRepairedOnReplayP(void)
{
  UiTestApplication application;
  tet_infoline("A foreign POSITION_X write under a right-to-left parent is repaired by the next replay");

  View parent, first, second;
  BuildSettledRtlParent(application, parent, first, second);

  const float      firstX     = first.GetProperty<float>(Actor::Property::POSITION_X);
  const float      secondX    = second.GetProperty<float>(Actor::Property::POSITION_X);
  const LayoutRect parentSlot = SlotOf(parent);

  // Baseline: the recorded values are the MIRRORED ones, not the logical ones. Without
  // this the rest of the test would also pass on a build that never mirrors at all.
  DALI_TEST_CHECK(secondX != 30.0f);

  // The clobber. This is the ScrollView / RecyclerView pattern: it moves the actor and
  // deliberately leaves the layout caches alone.
  Dali::Ui::Extension::View::SetPositionX(first, firstX + 17.0f);
  Dali::Ui::Extension::View::SetPositionX(second, secondX - 23.0f);

  DALI_TEST_CHECK(first.GetProperty<float>(Actor::Property::POSITION_X) != firstX);
  DALI_TEST_CHECK(second.GetProperty<float>(Actor::Property::POSITION_X) != secondX);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());

  // A cache HIT, and it still reconciles.
  parent.Arrange(parentSlot);

  DALI_TEST_CHECK(first.GetProperty<float>(Actor::Property::POSITION_X) == firstX);
  DALI_TEST_CHECK(second.GetProperty<float>(Actor::Property::POSITION_X) == secondX);
  DALI_TEST_CHECK(DataOf(parent).IsArrangeCacheValid());

  END_TEST;
}

// ---------------------------------------------------------------------------
// The hook is made by LAYOUT ROOTS only, so the mechanism that has to carry a
// change set on a plain mid-tree View is the property interception in
// ViewDataImpl::OnPropertySet, plus the subtree walk it raises. The cases below
// pin that path, its prune, its redundant-write guard, and the two boundary
// shapes (a standalone descendant, and which views end up holding the hook).
// ---------------------------------------------------------------------------

namespace
{
// root -> mid -> leaf, settled left-to-right, built so that `root` is the only one
// of the three holding the actor signal hook.
//
// The views are parented before they are configured. That ordering is NOT
// load-bearing -- the hook is only ever made by a view registering with a live
// window, so a parentless off-scene invalidation connects nothing either way -- and
// it is kept purely as documentation of the shape these tests mean to exercise:
// `mid` and `leaf` are plain children whose invalidations propagate up to `root`,
// so the only mechanism that can observe a direction write on `mid` is
// ViewDataImpl::OnPropertySet. UtcDaliLayoutDirectionSignalHookConnectedOnlyAtLayoutRootsP
// pins the build-then-add variant directly.
void BuildSettledChain(UiTestApplication& application, View& root, View& mid, View& leaf)
{
  root = View::New();
  mid  = View::New();
  leaf = View::New();

  root.Add(mid);
  mid.Add(leaf);
  application.GetScene().Add(root);

  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(200.0f);

  mid.SetRequestedWidth(150.0f);
  mid.SetRequestedHeight(100.0f);

  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(40.0f);
  leaf.SetRequestedX(10.0f);

  Settle(application);
}
} // namespace

// A direction write on a MID-TREE View -- one that is not a layout root and so
// holds no signal hook -- must still invalidate itself and its inheriting
// subtree, and the next pass must mirror the subtree.
//
// This is the case a hook-only design cannot see: core emits its signal on `mid`
// and `leaf`, but neither of them subscribed, so the only thing that observes the
// change is ViewDataImpl::OnPropertySet on `mid`, which raises the walk.
//
// Non-vacuity (verified by mutation): removing the three layout-direction cases
// from ViewDataImpl::OnPropertySet leaves both caches valid, both dirty bits
// clear and the leaf unmirrored, so every check after the write fails.
int UtcDaliLayoutDirectionMidTreeWriteInvalidatesSubtreeP(void)
{
  UiTestApplication application;
  tet_infoline("A direction write on a mid-tree View invalidates its whole inheriting subtree");

  View root, mid, leaf;
  BuildSettledChain(application, root, mid, leaf);

  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_EQUALS(leaf.GetPositionX(), 10.0f, TEST_LOCATION);

  // The premise of the test: `mid` is NOT hooked, so nothing but the property
  // interception can observe this write.
  DALI_TEST_CHECK(DataOf(root).IsLayoutDirectionSignalConnected());
  DALI_TEST_CHECK(!DataOf(mid).IsLayoutDirectionSignalConnected());
  DALI_TEST_CHECK(!DataOf(leaf).IsLayoutDirectionSignalConnected());

  mid.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  // Both caches dropped and both dirty bits raised, on the written view AND on
  // the descendant that merely inherits from it, with no pass in between.
  DALI_TEST_EQUALS(leaf.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(mid).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(mid).IsMeasureDirty());
  DALI_TEST_CHECK(DataOf(mid).IsArrangeDirty());
  DALI_TEST_CHECK(DataOf(leaf).IsMeasureDirty());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeDirty());

  // ...and the invalidation actually scheduled a pass that mirrors the subtree:
  // 150 - 10 - 50 = 90 inside `mid`, while `mid` itself is unmoved because the
  // root it sits in is still left-to-right.
  Settle(application);
  DALI_TEST_EQUALS(leaf.GetPositionX(), 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(mid.GetPositionX(), 0.0f, TEST_LOCATION);

  END_TEST;
}

// The prune. A descendant that holds a direction OF ITS OWN does not inherit the
// ancestor's change: dali-core's inherit walk stops at it, so its resolved
// direction did not move and its arrange cache entry -- and the direction
// recorded in it -- must survive the ancestor's flip untouched.
//
// Non-vacuity (verified by mutation): removing the
// `GetLayoutDirection() != INHERIT` prune from
// InvalidateSubtreeLayoutForDirectionChange drops the child's caches, so the two
// checks after the flip fail.
int UtcDaliLayoutDirectionArrangeCacheKeptForNonInheritingDescendantP(void)
{
  UiTestApplication application;
  tet_infoline("An ancestor's direction flip leaves a locally-directed descendant's arrange cache alone");

  // Parented before configured, matching BuildSettledChain: not load-bearing (the
  // hook needs a live window), kept as documentation that `child` is a plain child.
  View root       = View::New();
  View child      = View::New();
  View grandChild = View::New();

  root.Add(child);
  child.Add(grandChild);
  application.GetScene().Add(root);

  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(200.0f);

  child.SetRequestedWidth(150.0f);
  child.SetRequestedHeight(100.0f);
  // A direction of its own: this is what makes it a prune point.
  child.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);

  grandChild.SetRequestedWidth(50.0f);
  grandChild.SetRequestedHeight(40.0f);

  Settle(application);

  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(grandChild).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);

  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  // The root moved...
  DALI_TEST_EQUALS(root.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());

  // ...the locally-directed child did not, so neither it nor its own subtree was
  // invalidated. The child is still mirrored BY the root (that is the root's own
  // arrange pass), but nothing inside the child needs recomputing.
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(grandChild).IsArrangeCacheValid());

  END_TEST;
}

// Toggling a view between "inherit" and an explicit direction invalidates exactly
// when the RESOLVED direction moves, in both directions, and not otherwise. The
// property index written is the same one in all four steps, so what is pinned here
// is the value comparison rather than the index dispatch. The child is unhooked
// (it is a plain child, and the hook is only made by a view registering with a live
// window), so the path under test is OnPropertySet's.
int UtcDaliLayoutDirectionInheritToggleInvalidatesArrangeCacheP(void)
{
  UiTestApplication application;
  tet_infoline("Inherit/explicit toggles invalidate exactly when the resolved direction moves");

  View parent = View::New();
  View child  = View::New();
  parent.Add(child);
  application.GetScene().Add(parent);

  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  parent.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);

  Settle(application);
  DALI_TEST_CHECK(!DataOf(child).IsLayoutDirectionSignalConnected());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);

  // Explicit LEFT_TO_RIGHT -> INHERIT under a RIGHT_TO_LEFT parent: resolved
  // moves, so the entry goes.
  child.SetLayoutDirection(LayoutDirection::INHERIT);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  // Back to explicit LEFT_TO_RIGHT: resolved moves again, in the other
  // direction, so the entry goes again.
  child.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);

  // Now make the parent agree with the child, and settle.
  parent.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);

  // The negative half: explicit LEFT_TO_RIGHT -> INHERIT under a LEFT_TO_RIGHT
  // parent is a real property change that moves NO resolved direction, so the
  // entry must survive it.
  child.SetLayoutDirection(LayoutDirection::INHERIT);
  DALI_TEST_EQUALS(child.GetLayoutDirection(), LayoutDirection::INHERIT, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  END_TEST;
}

// The two LEGACY property indices reach the same walk. They are separate indices
// with separate handling in dali-core -- one takes the direction as an
// enumeration, the other a bool inherit flag -- and both move the resolved
// direction, so both have to be listed in the OnPropertySet switch. Dropping
// either case from it fails this test.
int UtcDaliLayoutDirectionLegacyIndexInvalidatesArrangeCacheP(void)
{
  UiTestApplication application;
  tet_infoline("Both legacy layout-direction property indices invalidate the arrange cache");

  View root  = View::New();
  View child = View::New();
  root.Add(child);
  application.GetScene().Add(root);

  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);

  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsLayoutDirectionSignalConnected());
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);

  // Index 1: the direction itself, as an enumeration value. This also clears the
  // inherit flag in dali-core, so the child now holds a direction of its own.
  child.SetProperty(DevelActor::Property::LAYOUT_DIRECTION_LEGACY,
                    static_cast<int32_t>(LayoutDirection::RIGHT_TO_LEFT));
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeDirty());

  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  // Index 2: the inherit flag. Re-enabling inheritance under a LEFT_TO_RIGHT
  // root moves the resolved direction back.
  child.SetProperty(DevelActor::Property::INHERIT_LAYOUT_DIRECTION_LEGACY, true);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::LEFT_TO_RIGHT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeDirty());

  END_TEST;
}

// The redundant-write guard. Writing the direction a view already resolves to is
// a no-op for layout: dali-core emits no signal (its own value guard), and the
// property interception must not turn the write into an invalidation either --
// otherwise a view that re-asserts its direction on every locale poll would drop
// its subtree's caches every time.
//
// Non-vacuity (verified by mutation): removing the
// `mLastArrangeDirection == GetEffectiveLayoutDirection()` guard from the
// OnPropertySet case invalidates the subtree and walks to the root, so both the
// cache checks and the propagation-generation check below fail.
int UtcDaliLayoutDirectionRedundantWriteKeepsArrangeCacheP(void)
{
  UiTestApplication application;
  tet_infoline("Re-writing the direction a view already resolves to costs it nothing");

  View root, mid, leaf;
  BuildSettledChain(application, root, mid, leaf);

  mid.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  Settle(application);

  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_EQUALS(DataOf(mid).GetLastArrangeDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  const uint32_t midGeneration  = DataOf(mid).GetMeasurePropagationGeneration();
  const uint32_t leafGeneration = DataOf(leaf).GetMeasurePropagationGeneration();

  // The settled pass ended its generation, so a fresh walk would record a
  // DIFFERENT value -- which is what makes the equality checks below meaningful.
  DALI_TEST_CHECK(midGeneration != LIE::CurrentGeneration());

  // The redundant write.
  mid.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  DALI_TEST_CHECK(DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(mid).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(leaf).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(mid).IsArrangeDirty());
  DALI_TEST_CHECK(!DataOf(mid).IsMeasureDirty());

  // No ancestor walk either: nothing was registered, so no pass is pending.
  DALI_TEST_EQUALS(DataOf(mid).GetMeasurePropagationGeneration(), midGeneration, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(leaf).GetMeasurePropagationGeneration(), leafGeneration, TEST_LOCATION);

  END_TEST;
}

// WHICH views hold the hook. This is the memory claim of the change stated as a
// test: the actor signal connection is made lazily, at the point a view registers
// with the LayoutController ON A LIVE WINDOW, and nowhere else.
//
// The window is part of the predicate, not incidental to it, and that is what makes
// the claim worth a test. Two of the cases below are the ones that would otherwise
// surprise:
//
//  - a PARENTLESS view has no ancestor to propagate an invalidation to, so its own
//    invalidation does reach RegisterWithLayoutController -- but OFF-SCENE that
//    function registers nothing and connects nothing. So a view configured BEFORE
//    being added to its parent -- the usual build-then-add idiom -- stays UNHOOKED,
//    which is precisely what keeps the saving real for the common case. Nothing is
//    lost by it: no layout pass can run without a window, and reconnection drops
//    the subtree's caches before registering (see the two off-scene cases below).
//  - a STANDALONE child is its own scheduling boundary and registers itself, so it
//    IS hooked -- registered at scene connection as a dirty standalone boundary
//    (OnViewSceneConnection's standalone+dirty path), with a live window in hand.
//    That is not an anomaly: a standalone view is a layout root in every sense the
//    layout pipeline cares about.
//
// The flag, once set, is never cleared: the claim it makes ("dali-core emits this
// signal on this actor") stays true across reparenting AND scene disconnection,
// which is what keeps OnPropertySet's skip-if-hooked short-circuit sound.
int UtcDaliLayoutDirectionSignalHookConnectedOnlyAtLayoutRootsP(void)
{
  UiTestApplication application;
  tet_infoline("Only views registering with the LayoutController on a live window connect the direction signal");

  // A freshly constructed View has invalidated nothing yet, so it has not
  // registered and holds no connection.
  View fresh = View::New();
  DALI_TEST_CHECK(!DataOf(fresh).IsLayoutDirectionSignalConnected());

  // Parented BEFORE any layout property is written, so every invalidation the
  // child raises propagates up to the root. (The build-then-add variant is pinned
  // separately at the end of this test.)
  View root  = View::New();
  View child = View::New();
  root.Add(child);
  application.GetScene().Add(root);

  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  Settle(application);

  // The root registered; the plain child propagated its invalidation UP to the
  // root instead of registering, so it never connected.
  DALI_TEST_CHECK(DataOf(root).IsLayoutDirectionSignalConnected());
  DALI_TEST_CHECK(!DataOf(child).IsLayoutDirectionSignalConnected());

  // A standalone child stops propagation at itself and registers itself instead --
  // here at scene connection, as a dirty standalone boundary with a live window in
  // hand -- so it is hooked. See the note above this test.
  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(40.0f);
  standalone.SetRequestedHeight(30.0f);
  root.Add(standalone);
  Settle(application);

  DALI_TEST_CHECK(DataOf(standalone).IsLayoutDirectionSignalConnected());

  // And the plain child is still unhooked after all of that: another view's
  // registration is not contagious.
  DALI_TEST_CHECK(!DataOf(child).IsLayoutDirectionSignalConnected());

  // THE MEMORY CLAIM FOR THE COMMON IDIOM. build-then-add: this view invalidated
  // itself while parentless, which does reach RegisterWithLayoutController -- but
  // with no window, so nothing was registered and nothing was connected. This is
  // the assertion that says the saving survives the way applications actually build
  // trees; if the connect ever moves back above the window check, this line is what
  // fails.
  View configuredBeforeAdd = View::New();
  configuredBeforeAdd.SetRequestedWidth(30.0f);
  DALI_TEST_CHECK(!DataOf(configuredBeforeAdd).IsLayoutDirectionSignalConnected());

  // Still unhooked after being reparented under a real, on-scene layout root: it is
  // a plain child now, so its invalidations propagate up to `root` and it never
  // registers on its own account.
  root.Add(configuredBeforeAdd);
  Settle(application);
  DALI_TEST_CHECK(!DataOf(configuredBeforeAdd).IsLayoutDirectionSignalConnected());

  END_TEST;
}

// The first of the two OFF-SCENE cases, and the one that shows why the hook does
// not need to exist before a window does: a direction written on a detached tree is
// observed by the window-independent OnPropertySet interception, and the first
// on-scene pass then applies it.
int UtcDaliLayoutDirectionOffSceneRootWriteAppliedOnFirstOnScenePassP(void)
{
  UiTestApplication application;
  tet_infoline("A direction written off-scene is applied by the first on-scene layout pass");

  View root  = View::New();
  View child = View::New();
  root.Add(child);

  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);

  // Off-scene, so nothing is hooked: RegisterWithLayoutController has been reached
  // (by the parentless root's invalidation) but found no window.
  DALI_TEST_CHECK(!DataOf(root).IsLayoutDirectionSignalConnected());

  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  DALI_TEST_EQUALS(child.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  application.GetScene().Add(root);
  Settle(application);

  // Mirrored on the very first pass: 200 - 20 - 50 = 130.
  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(child).GetLastArrangeDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);

  // ...and now that it has registered with a live window, the root is hooked.
  DALI_TEST_CHECK(DataOf(root).IsLayoutDirectionSignalConnected());

  END_TEST;
}

// The second off-scene case: the flag is never cleared, so a root that was hooked
// while on-scene keeps its connection after leaving the scene. The hook therefore
// fires on an off-scene direction change -- harmlessly, since it only marks state
// -- and the re-add settles to the mirrored geometry.
//
// This is the pin for "never disconnected": if the connection were dropped on
// scene disconnection, the immediate IsMeasureDirty() check below would fail.
int UtcDaliLayoutDirectionHookedRootOffSceneWriteSurvivesReAddP(void)
{
  UiTestApplication application;
  tet_infoline("A hooked root keeps its hook off-scene, and an off-scene direction write survives the re-add");

  View root  = View::New();
  View child = View::New();
  root.Add(child);
  application.GetScene().Add(root);

  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  child.SetRequestedX(20.0f);
  Settle(application);

  DALI_TEST_CHECK(DataOf(root).IsLayoutDirectionSignalConnected());
  DALI_TEST_EQUALS(child.GetPositionX(), 20.0f, TEST_LOCATION);

  // Leave the scene. The LayoutController registration goes; the signal connection
  // does not.
  application.GetScene().Remove(root);
  DALI_TEST_CHECK(DataOf(root).IsLayoutDirectionSignalConnected());

  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  // The hook fired even though the view is off-scene: the handler ran and raised the
  // dirty bit, with no window and therefore no registration behind it.
  DALI_TEST_CHECK(DataOf(root).IsMeasureDirty());
  DALI_TEST_CHECK(DataOf(child).IsMeasureDirty());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_EQUALS(child.GetPositionX(), 130.0f, TEST_LOCATION);

  END_TEST;
}

// A STANDALONE descendant of the changed view. It inherits the direction, so the
// walk reaches it, but its invalidation does not propagate to its parent -- it is
// a scheduling boundary. So the walk must give it a full InvalidateMeasure()
// rather than the cache-drop-plus-dirty the non-standalone nodes get, or its
// re-arrange would never be scheduled at all.
//
// The branch is load-bearing whichever way the node is reached. A standalone view
// self-registers with the LayoutController, so it also holds a signal hook of its
// own -- but that hook's handler runs the very same walk, whose FIRST node is this
// standalone view, so removing the branch would leave it with dropped caches, a
// raised dirty bit and nothing scheduled either way.
//
// The observable difference is the propagation record: only a view that actually
// walked and registered carries the CURRENT generation.
int UtcDaliLayoutDirectionStandaloneDescendantIsRescheduledP(void)
{
  UiTestApplication application;
  tet_infoline("A standalone descendant of a direction change re-registers itself");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);
  application.GetScene().Add(root);

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(50.0f);
  standalone.SetRequestedHeight(30.0f);
  standalone.SetRequestedX(10.0f);
  root.Add(standalone);

  Settle(application);

  DALI_TEST_CHECK(DataOf(standalone).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(standalone).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(standalone).GetMeasurePropagationGeneration() != LIE::CurrentGeneration());

  // Set on the root, which holds the hook; the standalone child inherits it.
  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  DALI_TEST_EQUALS(standalone.GetEffectiveLayoutDirection(), LayoutDirection::RIGHT_TO_LEFT, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(standalone).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(standalone).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(standalone).IsMeasureDirty());
  DALI_TEST_CHECK(DataOf(standalone).IsArrangeDirty());

  // Self-registered: the record names the live generation, which only a
  // completed walk writes.
  DALI_TEST_EQUALS(DataOf(standalone).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);

  END_TEST;
}
