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
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Internal::ViewDataImpl;

void utc_dali_ui_scale_effective_scale_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_ui_scale_effective_scale_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

// White-box coverage for the effective-scale sync bit (mEffectiveScaleValid) and
// for the arrange-cache bookkeeping that depends on it.
//
// The arrange cache now has a hit path, but it serves CHILDLESS views only, so the
// bookkeeping pinned here is still not observable through geometry from most of the
// shapes these tests build. These tests therefore read the bits directly through
// ViewDataImpl's white-box accessors, which states the claim about the bits
// themselves rather than about a hit that happens to depend on them. The
// producer-level, geometry-observable side is covered by the arrange cache suites
// (utc-Dali-ArrangeCacheHit-internal.cpp and the black-box cases in utc-Dali-View.cpp).
//
// UiScaleManagerImpl keeps the scale in a process-wide singleton, so every test
// here records the incoming scale and restores it before returning.

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

// Required test: a UI scale change must invalidate the arrange cache of every
// view in the affected subtree, not just the registered layout root.
//
// Non-vacuity (verified by mutation): removing `mArrangeCacheValid = false` from
// InvalidateLayoutCaches -- reached by the invalidation path SetScale runs over
// the subtree -- leaves both views' arrange caches valid and the second half of
// this test fails.
int UtcDaliUiScaleArrangeCacheInvalidatedByScaleChangeP(void)
{
  UiTestApplication application;
  tet_infoline("Arrange cache of a settled subtree is dropped by a UI scale change");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

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

  // No layout pass is run after the scale change: the invalidation itself, not a
  // subsequent pass, is what must clear the caches.
  UiScaleManager::Get().SetScale(1.5f);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// Anti-vacuity partner for the test above: a settled pass really does publish the
// arrange cache, so "false after SetScale" is a transition and not the only state
// this bit is ever in.
int UtcDaliUiScaleArrangeCachePublishedAfterCleanPassP(void)
{
  UiTestApplication application;
  tet_infoline("A clean arrange pass publishes the arrange cache");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  root.Add(child);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(root).IsArrangeDirty());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeDirty());

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// mEffectiveScaleValid is the sync bit for mEffectiveScale: false until the first
// GetEffectiveScale() computes a value, true afterwards, and false again after
// anything that can move the effective scale.
int UtcDaliUiScaleEffectiveScaleSyncBitP(void)
{
  UiTestApplication application;
  tet_infoline("mEffectiveScaleValid tracks whether the cached effective scale is usable");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  // Nothing has asked for the effective scale yet.
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  // On-scene so the view is a registered layout root and a global scale change
  // reaches it.
  application.GetScene().Add(view);
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  UiScaleManager::Get().SetScale(1.5f);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  // A policy change re-roots this view's scale derivation.
  view.SetUiScalePolicy(UiScalePolicy::DISABLED);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// Arrange() establishes this view's cached effective scale at pass entry.
//
// The default arrange of a CHILDLESS view reads the effective scale nowhere --
// ArrangeDefault's only read sits behind its child loop -- so without the read at
// pass entry an arrange-only visit leaves the sync bit false.
//
// Non-vacuity (verified by mutation): removing the `mViewImpl.GetEffectiveScale()`
// at the top of ViewDataImpl::Arrange makes the first check below fail.
int UtcDaliUiScaleArrangeEntryEstablishesEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("Arrange() establishes the cached effective scale for a childless view");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(50.0f);

  // Never measured, never arranged: the cached effective scale is not established yet.
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid());

  // Arrange alone -- no Measure() -- must leave the cached effective scale established.
  GetImpl(leaf).Arrange(LayoutRect(0.0f, 0.0f, 50.0f, 50.0f));
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  // And a settled childless leaf publishes its arrange cache, which the publish
  // gate only does while the effective-scale sync bit is live.
  application.GetScene().Add(leaf);
  Settle(application);

  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// A scale-context reset invalidates cached RESULTS; it must not swallow pending
// WORK.
//
// The reset recurses over the subtree, but the InvalidateMeasure() its callers
// follow it with only re-arms the node it is called on and that node's
// ANCESTORS. So a descendant's dirty bit, once cleared by the reset, has nothing
// to give it back -- it is dropped, not deferred. (Its one remaining reader is
// OnViewSceneConnection's standalone self-registration.)
//
// Non-vacuity (verified by mutation): restoring `mMeasureDirty = false` in
// ViewDataImpl::InvalidateLayoutCaches makes the post-reset check below fail.
int UtcDaliUiScaleResetKeepsDescendantMeasureDirtyP(void)
{
  UiTestApplication application;
  tet_infoline("An ancestor scale reset preserves a descendant's unconsumed measure dirty");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  root.Add(child);

  application.GetScene().Add(root);
  Settle(application);

  // Baseline: the settled pass consumed every dirty bit, so the dirty asserted
  // on below is genuinely raised by the next statement and not left over.
  DALI_TEST_CHECK(!DataOf(root).IsMeasureDirty());
  DALI_TEST_CHECK(!DataOf(child).IsMeasureDirty());

  // Raise real, unconsumed work on the DESCENDANT and do not run a pass: the
  // dirty is still standing when the reset arrives.
  child.SetRequestedWidth(80.0f);
  DALI_TEST_CHECK(DataOf(child).IsMeasureDirty());

  // Reset the scale context from the ANCESTOR. This recurses into the child;
  // the follow-up InvalidateMeasure() runs on the root and walks upward, so it
  // never reaches the child.
  root.SetUiScalePolicy(UiScalePolicy::DISABLED);

  // The reset retracts cached results; the pending work must survive it.
  // (The cache bits are not asserted here: the invalidation that raised the
  // dirty above already cleared them, so they would say nothing about the
  // reset. What the reset reaches on the descendant is covered by
  // UtcDaliUiScaleResetClearsWholeSubtreeEffectiveScaleP.)
  DALI_TEST_CHECK(DataOf(child).IsMeasureDirty());

  // And the layout that follows still services it.
  Settle(application);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 80.0f, 0.001f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// The reset is RECURSIVE: it must reach every descendant, not just the node it
// is called on and its direct children.
//
// Non-vacuity (verified by mutation): dropping the mChildren loop from
// ViewDataImpl::ResetSubtreeScaleAndLayoutCaches leaves the child and the
// grandchild with a live sync bit and the last two checks fail.
int UtcDaliUiScaleResetClearsWholeSubtreeEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A scale reset clears the cached effective scale of the whole subtree");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  root.Add(child);

  View grandChild = View::New();
  grandChild.SetRequestedWidth(50.0f);
  grandChild.SetRequestedHeight(50.0f);
  child.Add(grandChild);

  application.GetScene().Add(root);
  Settle(application);

  // Every level has a live sync bit before the reset, so each check below is a
  // transition rather than a state the tree happened to be in already.
  DALI_TEST_CHECK(DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(grandChild).IsEffectiveScaleValid());

  // No layout pass afterwards: the reset itself must clear the whole subtree.
  root.SetUiScalePolicy(UiScalePolicy::DISABLED);

  DALI_TEST_CHECK(!DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(grandChild).IsEffectiveScaleValid());

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The ACTOR-side half of the scale sync pair: mEffectiveScaleActorSynced.
//
// mEffectiveScaleValid says "the CACHED effective scale is usable";
// mEffectiveScaleActorSynced says "the ACTOR already holds that value in its
// animatable VIEW_EFFECTIVE_SCALE property". The second bit exists so that
// Measure() can skip reading that property -- a read that sat ABOVE the
// cache-hit test and therefore ran on every hit. These two tests pin the two
// halves of the claim that makes the elision behaviour-neutral: the bit is
// dropped by a scale-context change, and it is dropped by an external write of
// the property so the corrective push is still reached on a cache HIT.
// ---------------------------------------------------------------------------

// Non-vacuity (verified by mutation): removing `mEffectiveScaleActorSynced = false`
// from ViewDataImpl::DropCachedEffectiveScale leaves the bit live across the scale
// change below, so the next Measure() skips its push and the actor property stays
// at the OLD scale -- the post-change bit check and the final property check both
// fail.
int UtcDaliUiScaleEffectiveScaleActorSyncBitP(void)
{
  UiTestApplication application;
  tet_infoline("mEffectiveScaleActorSynced tracks whether the actor already holds the effective scale");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  // Never measured: nothing has pushed the scale to the actor yet.
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleActorSynced());

  application.GetScene().Add(view);
  Settle(application);

  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  // No layout pass in between: the scale invalidation itself, not a later pass,
  // is what must retract the actor-side claim.
  UiScaleManager::Get().SetScale(1.5f);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleActorSynced());

  // ...and the next pass re-establishes it with the NEW scale.
  Settle(application);

  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.5f, 0.001f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

namespace
{
int gActorScaleSyncMeasureCount = 0;

MeasuredSize CountingMeasure(View, float, float)
{
  ++gActorScaleSyncMeasureCount;
  return MeasuredSize(40.0f, 30.0f);
}
} // namespace

// The behaviour the read-elision must not lose: before the gate, Measure() read the
// actor property back on EVERY call and corrected any external clobber, cache hit
// included. It is preserved because dali-core routes every event-side write of a
// registered animatable property through its set function -- ViewDataImpl::SetProperty
// -- which retracts the sync bit, so the very next Measure() re-reads and re-pushes.
//
// The measure-callback counter is what makes "cache HIT" a fact rather than an
// assumption: the producer must run exactly once across the two Measure() calls.
//
// Non-vacuity (verified by mutation): removing `dataImpl.mEffectiveScaleActorSynced = false`
// from the VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX case of ViewDataImpl::SetProperty leaves
// the bit live, the hit below skips the push, and the property stays at the clobbered
// value -- the last two checks fail.
int UtcDaliUiScaleEffectiveScaleActorSyncRepairedOnMeasureCacheHitP(void)
{
  UiTestApplication application;
  tet_infoline("An external write of the effective-scale property is repaired by the next Measure, cache HIT included");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View view                   = View::New();
  gActorScaleSyncMeasureCount = 0;
  view.SetMeasureCallback(MeasureCallback::New(&CountingMeasure));

  // First Measure: a MISS. Publishes the cache entry and pushes the scale.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gActorScaleSyncMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  // Clobber the framework-owned property from outside.
  view.SetProperty(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX, 42.0f);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleActorSynced());

  // Nothing invalidated the measure cache, so the same constraint is a HIT.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gActorScaleSyncMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The effective scale as a measure-cache KEY (mLastMeasureScale).
//
// The arrange cache omits a scale term and relies on the invalidation pairing plus a
// DEBUG assert; the measure cache records the scale instead, because `s` is already
// read above its predicate for the constraint normalisation and because a missed
// invalidation then costs a MISS rather than a measured size computed at another
// scale.
//
// Under the CURRENT pairing discipline -- every DropCachedEffectiveScale() caller also
// calls InvalidateLayoutCaches() -- the rejection branch of that term is unreachable,
// so it is defence in depth. What IS observable, and what these two tests pin, is that
// the key is recorded at the publish, that it does not cost the steady state its hit,
// and that a real scale change re-publishes it at the new value with the producer
// re-run rather than serving the old entry.
// ---------------------------------------------------------------------------

namespace
{
int gMeasureKeyMeasureCount = 0;

MeasuredSize CountingKeyMeasure(View, float, float)
{
  ++gMeasureKeyMeasureCount;
  return MeasuredSize(40.0f, 30.0f);
}
} // namespace

// The publish records the scale it ran at, and the term does not spoil the steady
// state: an unchanged scale with an unchanged constraint is still a HIT.
//
// Non-vacuity (verified by mutation): removing `mLastMeasureScale = s;` from the
// measure publish block leaves the member at its constructed NaN, so the recorded-value
// check fails and the second Measure() misses (NaN == s is false) -- the producer count
// becomes 2.
int UtcDaliUiScaleMeasureCacheKeyRecordsEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A measure publish records the effective scale it ran at, and the term keeps the hit");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View view               = View::New();
  gMeasureKeyMeasureCount = 0;
  view.SetMeasureCallback(MeasureCallback::New(&CountingKeyMeasure));

  // Never measured: no entry, so no key.
  DALI_TEST_CHECK(!DataOf(view).IsMeasureCacheValid());

  // First Measure: a MISS. Publishes the entry and both key halves.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  // Exact: the recorded key is a copy of the same GetEffectiveScale() value the
  // predicate compares against, so an epsilon comparison here would say less.
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);

  // Same constraint, same scale: still a HIT. The producer must not run again.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// A real scale change re-publishes the key at the NEW scale, with the producer re-run.
// The producer count is what separates "re-published" from "served stale": the key
// could only move by a pass that actually measured.
int UtcDaliUiScaleMeasureCacheKeyRepublishedAtNewScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A scale change re-runs the measure producer and re-publishes the key at the new scale");

  const float originalScale = UiScaleManager::Get().GetScale();
  UiScaleManager::Get().SetScale(1.0f);

  View view               = View::New();
  gMeasureKeyMeasureCount = 0;
  view.SetMeasureCallback(MeasureCallback::New(&CountingKeyMeasure));
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  // On-scene so the view is a registered layout root and a global scale change
  // reaches it.
  application.GetScene().Add(view);
  Settle(application);

  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);
  const int countAtUnitScale = gMeasureKeyMeasureCount;
  DALI_TEST_CHECK(countAtUnitScale > 0);

  UiScaleManager::Get().SetScale(1.5f);
  Settle(application);

  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.5f);
  DALI_TEST_CHECK(gMeasureKeyMeasureCount > countAtUnitScale);

  UiScaleManager::Get().SetScale(originalScale);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The global master switch: UiScaleManager::IsScalable / SetScalable.
//
// ComputeEffectiveScale is the single choke point that reads the switch, so when
// scaling is off every view resolves to 1.0 regardless of its UiScalePolicy. The
// stored scale is preserved so re-enabling re-applies it. These tests exercise
// the switch through the same white-box bits the rest of this file pins.
//
// The switch lives in the same process-wide singleton as the scale, so each test
// records BOTH on entry and restores BOTH before returning.
// ---------------------------------------------------------------------------

// The switch defaults to on: an untouched process scales normally.
int UtcDaliUiScaleScalableFlagDefaultTrueP(void)
{
  UiTestApplication application;
  tet_infoline("UI scaling is enabled by default");

  DALI_TEST_CHECK(UiScaleManager::Get().IsScalable());
  END_TEST;
}

// With scaling off, every policy collapses to 1.0; re-enabling restores the
// stored scale on both an INHERIT root and an ENABLED view.
int UtcDaliUiScaleSetScalableFalseCollapsesEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable(false) collapses every view's effective scale to 1.0 regardless of policy");

  const float originalScale    = UiScaleManager::Get().GetScale();
  const bool  originalScalable = UiScaleManager::Get().IsScalable();
  UiScaleManager::Get().SetScalable(true);
  UiScaleManager::Get().SetScale(2.0f);

  View inheritView = View::New(); // INHERIT (default): a root inherits the global scale
  inheritView.SetRequestedWidth(100.0f);
  inheritView.SetRequestedHeight(100.0f);

  View enabledView = View::New();
  enabledView.SetUiScalePolicy(UiScalePolicy::ENABLED); // always tracks the global scale
  enabledView.SetRequestedWidth(100.0f);
  enabledView.SetRequestedHeight(100.0f);

  application.GetScene().Add(inheritView);
  application.GetScene().Add(enabledView);
  Settle(application);

  // Baseline: the switch is on, so the stored 2.0 reaches both.
  DALI_TEST_EQUALS(GetImpl(inheritView).GetEffectiveScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledView).GetEffectiveScale(), 2.0f, 0.001f, TEST_LOCATION);

  // Switch off: both collapse to 1.0 even though the stored scale is still 2.0.
  UiScaleManager::Get().SetScalable(false);
  DALI_TEST_EQUALS(GetImpl(inheritView).GetEffectiveScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledView).GetEffectiveScale(), 1.0f, 0.001f, TEST_LOCATION);

  // Switch back on: the preserved 2.0 is re-applied.
  UiScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(inheritView).GetEffectiveScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledView).GetEffectiveScale(), 2.0f, 0.001f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// Flipping the switch invalidates the WHOLE subtree, not just the layout root: it
// runs the same subtree reset SetScale does. No layout pass is run in between --
// the flip itself must clear the caches.
int UtcDaliUiScaleSetScalableInvalidatesSubtreeCachesP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable(false) alone drops the arrange cache of the whole subtree");

  const float originalScale    = UiScaleManager::Get().GetScale();
  const bool  originalScalable = UiScaleManager::Get().IsScalable();
  UiScaleManager::Get().SetScalable(true);
  UiScaleManager::Get().SetScale(2.0f);

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

  // No layout pass afterwards: flipping the switch itself must clear the caches.
  UiScaleManager::Get().SetScalable(false);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  UiScaleManager::Get().SetScale(originalScale);
  UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// Re-asserting the current switch value is a no-op: it must not run the subtree
// reset and so must leave a settled tree's caches intact.
int UtcDaliUiScaleSetScalableNoOpWhenUnchangedP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable with the current value is a no-op and preserves the layout caches");

  const float originalScale    = UiScaleManager::Get().GetScale();
  const bool  originalScalable = UiScaleManager::Get().IsScalable();
  UiScaleManager::Get().SetScalable(true);
  UiScaleManager::Get().SetScale(1.0f);

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

  // Re-asserting the value the switch already holds invalidates nothing.
  DALI_TEST_CHECK(UiScaleManager::Get().IsScalable());
  UiScaleManager::Get().SetScalable(true);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  UiScaleManager::Get().SetScale(originalScale);
  UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// While disabled, SetScale stores the value (GetScale reads it back) but defers
// both the effect and the relayout: nothing moves and no cache is dropped until
// the switch is turned back on, which then applies the stored value.
int UtcDaliUiScaleSetScaleWhileDisabledStoresButDefersP(void)
{
  UiTestApplication application;
  tet_infoline("SetScale while disabled stores the value but defers effect and relayout until re-enabled");

  const float originalScale    = UiScaleManager::Get().GetScale();
  const bool  originalScalable = UiScaleManager::Get().IsScalable();
  UiScaleManager::Get().SetScalable(true);
  UiScaleManager::Get().SetScale(1.0f);

  View view = View::New(); // INHERIT root
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  application.GetScene().Add(view);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveScale(), 1.0f, 0.001f, TEST_LOCATION);

  // Disabling drops the cache; re-settling re-publishes it at the unscaled result.
  UiScaleManager::Get().SetScalable(false);
  DALI_TEST_CHECK(!DataOf(view).IsArrangeCacheValid());
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());

  // Change the stored scale while disabled: the value is recorded (I5)...
  UiScaleManager::Get().SetScale(2.0f);
  DALI_TEST_EQUALS(UiScaleManager::Get().GetScale(), 2.0f, 0.001f, TEST_LOCATION);
  // ...but with the switch off the view stays unscaled and its cache is untouched.
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());

  // Re-enabling applies the deferred 2.0.
  UiScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveScale(), 2.0f, 0.001f, TEST_LOCATION);

  UiScaleManager::Get().SetScale(originalScale);
  UiScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}
