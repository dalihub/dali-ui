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
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Internal::ViewDataImpl;

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
// Non-vacuity (verified by mutation): emptying the body of
// ViewDataImpl::OnLayoutDirectionChanged leaves both caches valid and the second
// half of this test fails.
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

  // Set on the root only; the child merely inherits the change. Core emits the
  // signal on both, so both hooks run.
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
int UtcDaliLayoutDirectionRtlSettledReplayWritesNothingP(void)
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
