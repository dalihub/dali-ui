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
#include <dali-ui-foundation/internal/layouts/layout-invalidation-generation.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/layouts/flex-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/grid-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/layout.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Internal::ViewDataImpl;
namespace LIE = Dali::Ui::Internal::LayoutInvalidation;

void utc_dali_layout_invalidation_generation_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_layout_invalidation_generation_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// White-box coverage for the invalidation PROPAGATION generation.
//
// InvalidateMeasure()/InvalidateArrange() do two separable things: local bookkeeping
// on the view they are called on, and a walk to the layout root that registers it
// with the controller. The walk is O(depth) with two handle DownCasts per level and a
// Window lookup at the root, and it is idempotent, so it is skipped while the
// registration it would make is still pending. These tests pin the three properties
// that make skipping safe:
//
//  - the record goes stale at every drain, so nothing is ever swallowed permanently
//    (the failure mode the removed dirty-flag short-circuit had);
//  - the measure and arrange records are independent;
//  - the skip is disabled outright while a layout pass is running, because mid-pass
//    the walk also poisons in-progress ancestors, which a root registration does not
//    stand in for.

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

// A three-level chain, deep enough that "the walk reached the root" is a real claim.
struct Chain
{
  View root;
  View mid;
  View leaf;
};

Chain BuildChain(UiTestApplication& application)
{
  Chain chain;

  chain.root = View::New();
  chain.root.SetRequestedWidth(200.0f);
  chain.root.SetRequestedHeight(200.0f);
  application.GetScene().Add(chain.root);

  chain.mid = View::New();
  chain.mid.SetRequestedWidth(120.0f);
  chain.mid.SetRequestedHeight(120.0f);
  chain.root.Add(chain.mid);

  chain.leaf = View::New();
  chain.leaf.SetRequestedWidth(40.0f);
  chain.leaf.SetRequestedHeight(40.0f);
  chain.mid.Add(chain.leaf);

  Settle(application);
  return chain;
}

// --- Test 3 harness: an arrange producer that invalidates a descendant twice, with
// --- that descendant's ancestor completing a pass in between.
View gStagedMid;
View gStagedLeaf;
int  gStagedRuns = 0;

LayoutRect StagedRootArrange(View, const LayoutRect& bounds)
{
  ++gStagedRuns;
  if(gStagedRuns == 1 && gStagedMid && gStagedLeaf)
  {
    // (a) First invalidation. Walks leaf -> mid -> root and records the generation on all
    //     three; mid is now arrange-dirty.
    gStagedLeaf.InvalidateArrange();

    // (b) mid runs a full arrange pass, which CONSUMES the dirty raised in (a) and
    //     publishes a fresh cache entry.
    gStagedMid.Arrange(LayoutRect(0.0f, 0.0f, 120.0f, 120.0f));

    // (c) Second invalidation of the same leaf, in the same generation. If the generation
    //     short-circuit were live during a layout pass this would return without
    //     touching mid, leaving the entry published in (b) standing over a subtree
    //     that has just been invalidated underneath it.
    gStagedLeaf.InvalidateArrange();
  }
  return bounds;
}

// --- Test 6 harness: a measure producer that genuinely sizes on the layout direction.
const float DIRECTION_SENSITIVE_LTR_WIDTH = 60.0f;
const float DIRECTION_SENSITIVE_RTL_WIDTH = 90.0f;

int gDirectionSensitiveMeasures = 0;

MeasuredSize DirectionSensitiveMeasure(View view, float, float)
{
  ++gDirectionSensitiveMeasures;
  const bool rtl = (view.GetEffectiveLayoutDirection() == LayoutDirection::RIGHT_TO_LEFT);
  return MeasuredSize(rtl ? DIRECTION_SENSITIVE_RTL_WIDTH : DIRECTION_SENSITIVE_LTR_WIDTH, 30.0f);
}

} // namespace

int UtcDaliLayoutInvalidationGenerationRecordedOnPropagationP(void)
{
  UiTestApplication application;
  tet_infoline("A view's propagation record is 0 until it propagates, then names the current generation");

  Chain chain = BuildChain(application);

  // A settled view has propagated at least once (during construction), and the drain
  // that settled it started a new generation, so every record is now stale.
  const uint32_t settledGeneration = LIE::CurrentGeneration();
  DALI_TEST_CHECK(DataOf(chain.leaf).GetMeasurePropagationGeneration() != settledGeneration);

  chain.leaf.InvalidateMeasure();

  // The walk ran and recorded itself at every level it passed through.
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), settledGeneration, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(chain.mid).GetMeasurePropagationGeneration(), settledGeneration, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(chain.root).GetMeasurePropagationGeneration(), settledGeneration, TEST_LOCATION);

  // ...and it reached the root, which is the point of the walk.
  DALI_TEST_CHECK(!DataOf(chain.root).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(chain.root).IsMeasureDirty());

  END_TEST;
}

int UtcDaliLayoutInvalidationRePropagatesAfterEveryPassP(void)
{
  UiTestApplication application;
  tet_infoline("Every pass ends the generation, so the NEXT invalidation walks and registers again");

  Chain chain = BuildChain(application);

  // Three rounds. Each one must reach the root on its own; a record that failed to go
  // stale would let round 2 skip its walk and leave the root's measure cache valid --
  // which is exactly the permanent-swallow failure the old dirty-flag short-circuit
  // had, in a different disguise.
  for(int round = 0; round < 3; ++round)
  {
    DALI_TEST_CHECK(DataOf(chain.root).IsMeasureCacheValid());

    const uint32_t generationBefore = LIE::CurrentGeneration();
    chain.leaf.InvalidateMeasure();

    DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), generationBefore, TEST_LOCATION);
    DALI_TEST_CHECK(!DataOf(chain.root).IsMeasureCacheValid());

    Settle(application);

    // The drain consumed the registration and started a new generation.
    DALI_TEST_CHECK(LIE::CurrentGeneration() != generationBefore);
    DALI_TEST_CHECK(DataOf(chain.leaf).GetMeasurePropagationGeneration() != LIE::CurrentGeneration());
  }

  END_TEST;
}

int UtcDaliLayoutInvalidationMeasureAndArrangeRecordsAreIndependentP(void)
{
  UiTestApplication application;
  tet_infoline("An arrange propagation must not suppress a later measure walk in the same generation");

  Chain chain = BuildChain(application);

  // Arrange first. This marks the ancestors arrange-dirty but deliberately leaves
  // their MEASURE caches alone.
  chain.leaf.InvalidateArrange();
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetArrangePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(chain.root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(chain.root).IsMeasureCacheValid());

  // Now measure, in the SAME generation. A single shared record would make this skip its
  // walk, and the root would keep serving its measure cache -- and an ancestor measure
  // HIT never re-measures its children, so the leaf's new measured size would never be
  // computed at all.
  chain.leaf.InvalidateMeasure();
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(chain.root).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(chain.root).IsMeasureDirty());

  END_TEST;
}

int UtcDaliLayoutInvalidationNotCoalescedDuringLayoutPassP(void)
{
  UiTestApplication application;
  tet_infoline("Mid-pass the skip is disabled, so a repeated invalidation still reaches an ancestor that has since completed its own pass");

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);
  application.GetScene().Add(root);

  View mid = View::New();
  mid.SetRequestedWidth(120.0f);
  mid.SetRequestedHeight(120.0f);
  root.Add(mid);

  View leaf = View::New();
  leaf.SetRequestedWidth(40.0f);
  leaf.SetRequestedHeight(40.0f);
  mid.Add(leaf);

  gStagedMid  = mid;
  gStagedLeaf = leaf;
  gStagedRuns = 0;

  // This staged sequence needs the producer on every pass, so it explicitly opts
  // out of unchanged-result reuse.
  root.SetArrangeCallback(ArrangeCallback::New(&StagedRootArrange), ArrangePolicy::ALWAYS);

  Settle(application);

  DALI_TEST_CHECK(gStagedRuns >= 1);

  // Step (c) must have landed on mid AFTER step (b) published its entry. If the generation
  // short-circuit were still live during the pass, step (c) would have returned early
  // -- leaf's record already named the current generation from step (a) -- and mid would
  // have ended the pass with a live cache entry over an invalidated subtree.
  DALI_TEST_CHECK(!DataOf(mid).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(mid).IsArrangeDirty());

  gStagedMid.Reset();
  gStagedLeaf.Reset();

  END_TEST;
}

int UtcDaliLayoutInvalidationGenerationRetractedBySubtreeScaleResetP(void)
{
  UiTestApplication application;
  tet_infoline("A scale-context reset retracts the propagation records of the whole subtree");

  Chain chain = BuildChain(application);

  chain.leaf.InvalidateMeasure();
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(chain.mid).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);

  // Any path that can re-root a subtree's context runs through
  // ResetSubtreeScaleAndLayoutCaches, which is where the records are retracted: a
  // record written against the OLD ancestor chain must not authorise skipping a walk
  // that now has a different root to reach.
  chain.root.SetUiScalePolicy(UiScalePolicy::DISABLED);

  // The DESCENDANTS' records are retracted and stay retracted: the reset walks DOWN
  // the subtree, and the InvalidateMeasure() the policy setter follows it with walks
  // UP from the root only.
  DALI_TEST_EQUALS(DataOf(chain.mid).GetMeasurePropagationGeneration(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetArrangePropagationGeneration(), 0u, TEST_LOCATION);

  // The root's own record was retracted too, and then re-established by that
  // follow-up walk -- which is the point: a retracted record forces a real walk
  // rather than being trusted.
  DALI_TEST_EQUALS(DataOf(chain.root).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);

  // And the descendants' retraction is load-bearing: the next invalidation down there
  // must walk, not coalesce onto the pre-reset record.
  chain.leaf.InvalidateMeasure();
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);
  DALI_TEST_EQUALS(DataOf(chain.mid).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliLayoutInvalidationReparentReachesTheNewRootP(void)
{
  UiTestApplication application;
  tet_infoline("A view invalidated under one root, then reparented, invalidates its NEW root");

  View oldRoot = View::New();
  oldRoot.SetRequestedWidth(200.0f);
  oldRoot.SetRequestedHeight(200.0f);
  application.GetScene().Add(oldRoot);

  View newRoot = View::New();
  newRoot.SetRequestedWidth(300.0f);
  newRoot.SetRequestedHeight(300.0f);
  application.GetScene().Add(newRoot);

  View child = View::New();
  child.SetRequestedWidth(40.0f);
  child.SetRequestedHeight(40.0f);
  oldRoot.Add(child);

  Settle(application);

  // Dirty under the old root, with no pass in between -- the record now names the OLD
  // chain.
  child.InvalidateMeasure();
  DALI_TEST_EQUALS(DataOf(child).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);

  newRoot.Add(child);
  Settle(application);

  // Re-dirty under the new root and confirm the walk gets there.
  child.InvalidateMeasure();
  DALI_TEST_CHECK(!DataOf(newRoot).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(newRoot).IsMeasureDirty());

  END_TEST;
}

int UtcDaliLayoutInvalidationManualPassEndsTheGenerationP(void)
{
  UiTestApplication application;
  tet_infoline("A manual Measure/Arrange pass ends the generation, so an invalidation after it walks in full");

  Chain chain = BuildChain(application);

  // First invalidation: walks, marks the whole chain, registers the root.
  chain.leaf.SetRequestedWidth(60.0f);
  DALI_TEST_EQUALS(DataOf(chain.leaf).GetMeasurePropagationGeneration(), LIE::CurrentGeneration(), TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(chain.root).IsMeasureCacheValid());

  // A MANUAL pass on the root -- Measure() and Arrange() are public API -- consumes
  // the chain's dirty bits and republishes every cache, with NO controller drain.
  // This is the one way a walked chain can go clean while the walk's registration is
  // still pending, which is exactly what the pass-guard generation bump exists for: if the
  // record written by the first walk were still current here, the invalidation below
  // would skip its walk, the root's caches would stay valid, and the next drain would
  // serve a measure HIT that never re-measures the leaf.
  const MeasuredSize measured = chain.root.Measure(200.0f, 200.0f);
  chain.root.Arrange(LayoutRect(0.0f, 0.0f, measured.width, measured.height));
  DALI_TEST_EQUALS(chain.leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 60.0f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(chain.root).IsMeasureCacheValid());

  // The manual pass ended the generation...
  DALI_TEST_CHECK(DataOf(chain.leaf).GetMeasurePropagationGeneration() != LIE::CurrentGeneration());

  // ...so this invalidation walks in full and re-marks the chain.
  chain.leaf.SetRequestedWidth(70.0f);
  DALI_TEST_CHECK(!DataOf(chain.root).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(chain.root).IsMeasureDirty());

  // And the drain services it: the leaf ends at the SECOND width, not the first.
  Settle(application);
  DALI_TEST_EQUALS(chain.leaf.GetProperty<float>(Actor::Property::SIZE_WIDTH), 70.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLayoutManagerSetterInvalidatesOwnerStackP(void)
{
  UiTestApplication application;
  tet_infoline("StackLayoutManager's own setters invalidate the View they are attached to");

  View owner = View::New();
  owner.SetRequestedWidth(200.0f);
  owner.SetRequestedHeight(200.0f);
  application.GetScene().Add(owner);

  auto                managed = Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f);
  StackLayoutManager* manager = managed.Get();
  owner.AttachLayoutManager(std::move(managed));

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  owner.Add(child);

  Settle(application);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());

  // Reaching the manager directly used to leave the change unscheduled: nothing
  // retracted the owner's cached result, and with the manager declared IF_CHANGED the
  // arrange cache would have kept serving the pre-change placement indefinitely.
  manager->SetSpacing(20.0f);
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(owner).IsMeasureDirty());

  Settle(application);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());

  // Equality-guarded: writing the value it already holds must not schedule anything.
  manager->SetSpacing(20.0f);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureDirty());

  manager->SetOrientation(StackOrientation::HORIZONTAL);
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureCacheValid());

  END_TEST;
}

int UtcDaliLayoutManagerSetterInvalidatesOwnerFlexP(void)
{
  UiTestApplication application;
  tet_infoline("FlexLayoutManager's own setters invalidate the View they are attached to");

  View owner = View::New();
  owner.SetRequestedWidth(200.0f);
  owner.SetRequestedHeight(200.0f);
  application.GetScene().Add(owner);

  auto               managed = Dali::MakeUnique<FlexLayoutManager>(FlexDirection::ROW, FlexWrap::NO_WRAP, FlexJustify::FLEX_START, FlexAlign::FLEX_START, FlexAlign::FLEX_START);
  FlexLayoutManager* manager = managed.Get();
  owner.AttachLayoutManager(std::move(managed));

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  owner.Add(child);

  Settle(application);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 0.0f, TEST_LOCATION);

  // PLACEMENT-only state invalidates the ARRANGE axis alone: no measured size
  // can change (Measure() never reads the justification), so every measure
  // cache in the subtree stays warm while the re-arrange is scheduled.
  manager->SetJustifyContent(FlexJustify::CENTER);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(owner).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(owner).IsArrangeDirty());

  // ...and the narrower invalidation still reaches the screen: the scheduled
  // pass re-runs Arrange and the new justification lands.
  Settle(application);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::POSITION_X), 75.0f, TEST_LOCATION);

  manager->SetJustifyContent(FlexJustify::CENTER); // same value
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(owner).IsArrangeDirty());

  // A MEASURE input keeps the full invalidation: the direction remaps the axes,
  // so line breaking and the measured extents both change with it.
  manager->SetDirection(FlexDirection::COLUMN);
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureCacheValid());

  END_TEST;
}

int UtcDaliLayoutManagerSetterInvalidatesOwnerGridP(void)
{
  UiTestApplication application;
  tet_infoline("GridLayoutManager's own setters invalidate the View they are attached to");

  View owner = View::New();
  owner.SetRequestedWidth(200.0f);
  owner.SetRequestedHeight(200.0f);
  application.GetScene().Add(owner);

  Dali::Vector<GridLength> rows;
  rows.PushBack(GridLength::Star(1.0f));
  Dali::Vector<GridLength> columns;
  columns.PushBack(GridLength::Star(1.0f));

  auto               managed = Dali::MakeUnique<GridLayoutManager>(rows, columns, 0.0f, 0.0f);
  GridLayoutManager* manager = managed.Get();
  owner.AttachLayoutManager(std::move(managed));

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  owner.Add(child);

  Settle(application);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());

  manager->SetRowSpacing(8.0f);
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureCacheValid());

  Settle(application);
  manager->SetRowSpacing(8.0f); // same value
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());

  // Definitions are invalidated unguarded: GridLength has no equality operator.
  manager->SetColumnDefinitions(columns);
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureCacheValid());

  END_TEST;
}

int UtcDaliLayoutManagerSetterBeforeAttachIsSafeP(void)
{
  UiTestApplication application;
  tet_infoline("A manager setter called before the manager is attached is a no-op, not a crash");

  auto                managed = Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f);
  StackLayoutManager* manager = managed.Get();

  // No owner yet: nothing to retract, nothing to schedule.
  manager->SetSpacing(12.0f);
  DALI_TEST_EQUALS(manager->GetSpacing(), 12.0f, TEST_LOCATION);

  View owner = View::New();
  owner.SetRequestedWidth(200.0f);
  owner.SetRequestedHeight(200.0f);
  application.GetScene().Add(owner);
  owner.AttachLayoutManager(std::move(managed));

  Settle(application);
  DALI_TEST_CHECK(DataOf(owner).IsMeasureCacheValid());

  manager->SetSpacing(24.0f);
  DALI_TEST_CHECK(!DataOf(owner).IsMeasureCacheValid());

  END_TEST;
}

int UtcDaliLayoutDirectionChangeInvalidatesMeasureP(void)
{
  UiTestApplication application;
  tet_infoline("A direction change re-measures, so a measure producer may size on the layout direction");

  View root = View::New();
  root.SetRequestedWidth(300.0f);
  root.SetRequestedHeight(300.0f);
  application.GetScene().Add(root);

  View sized = View::New();
  sized.SetMeasureCallback(MeasureCallback::New(&DirectionSensitiveMeasure));
  root.Add(sized);

  gDirectionSensitiveMeasures = 0;
  Settle(application);

  DALI_TEST_CHECK(gDirectionSensitiveMeasures > 0);
  DALI_TEST_EQUALS(sized.GetProperty<float>(Actor::Property::SIZE_WIDTH), DIRECTION_SENSITIVE_LTR_WIDTH, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(sized).IsMeasureCacheValid());

  const int measuresBefore = gDirectionSensitiveMeasures;

  root.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);

  // The direction hook invalidates MEASURE, not arrange alone: the measure cache key
  // has no direction term, so an arrange-only invalidation would leave this producer
  // pinned at its pre-change measured size until some unrelated invalidation arrived.
  DALI_TEST_CHECK(!DataOf(sized).IsMeasureCacheValid());

  Settle(application);

  DALI_TEST_CHECK(gDirectionSensitiveMeasures > measuresBefore);
  DALI_TEST_EQUALS(sized.GetProperty<float>(Actor::Property::SIZE_WIDTH), DIRECTION_SENSITIVE_RTL_WIDTH, TEST_LOCATION);

  // ...and back again, so the invalidation is not a one-way latch.
  root.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
  Settle(application);
  DALI_TEST_EQUALS(sized.GetProperty<float>(Actor::Property::SIZE_WIDTH), DIRECTION_SENSITIVE_LTR_WIDTH, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLayoutInvalidationBatchLeavesGeometryUnchangedP(void)
{
  UiTestApplication application;
  tet_infoline("Coalescing a batch of invalidations changes when work is scheduled, never the result");

  View root = View::New();
  root.SetRequestedWidth(400.0f);
  root.SetRequestedHeight(400.0f);
  application.GetScene().Add(root);

  View container = View::New();
  container.SetRequestedWidth(400.0f);
  container.SetRequestedHeight(400.0f);
  container.AttachLayoutManager(Dali::MakeUnique<StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
  root.Add(container);

  const uint32_t     CHILD_COUNT = 8u;
  Dali::Vector<View> children;
  for(uint32_t i = 0; i < CHILD_COUNT; ++i)
  {
    View child = View::New();
    child.SetRequestedWidth(30.0f);
    child.SetRequestedHeight(20.0f);
    container.Add(child);
    children.PushBack(child);
  }

  Settle(application);

  // A batch of invalidations before a single pass: only the first of each axis walks,
  // but every one of them must still be serviced by that pass.
  for(uint32_t i = 0; i < CHILD_COUNT; ++i)
  {
    children[i].SetRequestedHeight(40.0f);
  }
  Settle(application);

  for(uint32_t i = 0; i < CHILD_COUNT; ++i)
  {
    DALI_TEST_EQUALS(children[i].GetProperty<float>(Actor::Property::SIZE_HEIGHT), 40.0f, TEST_LOCATION);
    DALI_TEST_EQUALS(children[i].GetProperty<float>(Actor::Property::POSITION_Y), static_cast<float>(i) * 40.0f, TEST_LOCATION);
  }

  END_TEST;
}
