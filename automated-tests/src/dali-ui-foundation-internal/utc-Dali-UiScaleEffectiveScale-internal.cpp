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
#include <limits>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/internal/layout-scale-manager-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Ui::Extension::LayoutScaleManager;
using Dali::Ui::Internal::LayoutScaleManagerImpl;
using Dali::Ui::Internal::ViewDataImpl;
using Dali::Ui::Internal::WindowScalePolicy;

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
// LayoutScaleManagerImpl keeps the scale, the master switch AND the per-window scales in a
// process-wide singleton, so every test here records the state it is about to move and
// restores it before returning. A window scale in particular outlives the
// UiTestApplication that created the window it was set on, so a test that sets one
// resets it to 1.0 before returning. That reset is hygiene for a LIVE window only: the
// record of a window that has been DESTROYED is reclaimed by the prune that
// LayoutScaleManagerImpl runs from its layout-root hooks, with no application call at all,
// which is what the two WindowDestroyed cases further down pin -- one hook each.
//
// Those two cases observe the record set itself through the public static
// LayoutScaleManagerImpl::HasAnyWindowScale() -- the same gate the effective-scale path
// reads -- rather than through any test-only accessor.
//
// The WindowScalePolicy cases at the end set that framework-internal policy through
// ViewDataImpl::SetWindowScalePolicy(), the same white-box route the bit accessors take
// (no friend, no test-only accessor). A policy lives in its view and dies with it, so
// those cases restore only the process-wide state.

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

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

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
  LayoutScaleManager::Get().SetUiScale(1.5f);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// Anti-vacuity partner for the test above: a settled pass really does publish the
// arrange cache, so "false after SetScale" is a transition and not the only state
// this bit is ever in.
int UtcDaliUiScaleArrangeCachePublishedAfterCleanPassP(void)
{
  UiTestApplication application;
  tet_infoline("A clean arrange pass publishes the arrange cache");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

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

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// mEffectiveScaleValid is the sync bit for mEffectiveScale: false until the first
// GetEffectiveLayoutScale() ON SCENE computes a value, true afterwards, and false again
// after anything that can move the effective scale. A read off scene computes the value
// and memoizes nothing (UtcDaliUiScaleEffectiveScaleNotMemoizedOffSceneP pins that side).
int UtcDaliUiScaleEffectiveScaleSyncBitP(void)
{
  UiTestApplication application;
  tet_infoline("mEffectiveScaleValid tracks whether the cached effective scale is usable");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  // Nothing has asked for the effective scale yet.
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  // Off scene a read computes the value and memoizes nothing.
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  // On-scene so the view is a registered layout root and a global scale change
  // reaches it; the layout pass establishes the bit.
  application.GetScene().Add(view);
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  LayoutScaleManager::Get().SetUiScale(1.5f);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  // A policy change re-roots this view's scale derivation.
  view.SetUiScalePolicy(UiScalePolicy::DISABLED);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// Arrange() establishes this view's cached effective scale at pass entry.
//
// The default arrange of a CHILDLESS view reads the effective scale nowhere --
// ArrangeDefault's only read sits behind its child loop -- so without the read at
// pass entry an arrange-only visit leaves the sync bit false. The bit is memoized on
// scene only, so the leaf is a settled layout root whose bit a subtree reset has just
// dropped -- a cache-only reset: no dirty bit, no pass -- and the Arrange() below is
// the only thing that reads the scale before the check.
//
// The claim this test pins (stated, not demonstrated by a mutation run -- the suite is
// build-only here; the off-scene form of this case was verified by mutation): removing
// the `mViewImpl.GetEffectiveLayoutScale()` at the top of ViewDataImpl::Arrange makes
// the check right after the Arrange() fail.
int UtcDaliUiScaleArrangeEntryEstablishesEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("Arrange() establishes the cached effective scale for a childless view");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View leaf = View::New();
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(50.0f);

  // Never measured, never arranged: the cached effective scale is not established yet.
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid());

  application.GetScene().Add(leaf);
  Settle(application);
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  // Drop the bit, and both caches, without raising work.
  DataOf(leaf).ResetSubtreeScaleAndLayoutCaches();
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid());

  // Arrange alone -- no Measure() -- must leave the cached effective scale established.
  GetImpl(leaf).Arrange(LayoutRect(0.0f, 0.0f, 50.0f, 50.0f));
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());

  // And a settled childless leaf publishes its arrange cache, which the publish
  // gate only does while the effective-scale sync bit is live.
  leaf.InvalidateMeasure();
  Settle(application);

  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(leaf).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// Off scene the effective scale is never memoized. A view that is not on a scene is in no
// registered layout root, so nothing would clear its sync bit when SetUiScale() or
// SetScalable() move the UI term: the bit stays false there whatever reads the scale --
// GetEffectiveLayoutScale(), Measure() and Arrange() alike -- becomes true on scene, and is
// false again, for the whole subtree, once its scale root leaves the scene. The arrange
// cache, whose key has no scale term, is therefore never published off scene: its publish
// gate requires the bit.
//
// The claims this test pins (stated, not demonstrated by a mutation run -- the suite is
// build-only here): storing the value off scene (dropping the CONNECTED_TO_SCENE test from
// ViewDataImpl::GetEffectiveLayoutScale) fails the first off-scene checks and publishes the
// off-scene arrange entry; gating the scale-root reset in
// ViewDataImpl::OnViewSceneDisconnection on anything this case does not set -- a recorded
// window scale, say -- fails the checks after the removal.
int UtcDaliUiScaleEffectiveScaleNotMemoizedOffSceneP(void)
{
  UiTestApplication application;
  tet_infoline("The effective-scale sync bit is set on scene only, and dropped when the subtree leaves it");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View child = View::New();
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(40.0f);
  root.Add(child);

  // Off scene: every read computes, none memoizes.
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(child).IsEffectiveScaleValid());

  root.Measure(500.0f, 500.0f);
  GetImpl(root).Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));
  DALI_TEST_CHECK(DataOf(root).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());

  // On scene the layout pass memoizes it at every level and publishes the arrange cache.
  application.GetScene().Add(root);
  Settle(application);
  DALI_TEST_CHECK(DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());

  // The scale root leaves: the whole subtree's bit goes, with the arrange entry, and a
  // later read does not bring it back.
  application.GetScene().Remove(root);
  DALI_TEST_CHECK(!DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(root).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(child).IsEffectiveScaleValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
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

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

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

  LayoutScaleManager::Get().SetUiScale(originalScale);
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

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

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

  LayoutScaleManager::Get().SetUiScale(originalScale);
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

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

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
  LayoutScaleManager::Get().SetUiScale(1.5f);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleActorSynced());

  // ...and the next pass re-establishes it with the NEW scale.
  Settle(application);

  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.5f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
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
// On scene, because the gate is consulted only there: off scene mEffectiveScaleValid is
// never set, so Measure() re-reads the property on every call whatever the actor-side bit
// says, and that would hide the bit this case is about.
//
// Non-vacuity (verified by mutation in the off-scene form this case had before the scale
// was memoized on scene only; restated here, not re-run -- the suite is build-only here):
// removing `dataImpl.mEffectiveScaleActorSynced = false` from the
// VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX case of ViewDataImpl::SetProperty leaves the bit live,
// the hit below skips the push, and the property stays at the clobbered value -- the last
// two checks fail.
int UtcDaliUiScaleEffectiveScaleActorSyncRepairedOnMeasureCacheHitP(void)
{
  UiTestApplication application;
  tet_infoline("An external write of the effective-scale property is repaired by the next Measure, cache HIT included");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View view                   = View::New();
  gActorScaleSyncMeasureCount = 0;
  view.SetMeasureCallback(MeasureCallback::New(&CountingMeasure));

  application.GetScene().Add(view);
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());
  const int settledCount = gActorScaleSyncMeasureCount;

  // A Measure() at a constraint the pass did not use: a MISS. Publishes the cache entry;
  // the scale is already on the actor.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gActorScaleSyncMeasureCount, settledCount + 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  // Clobber the framework-owned property from outside.
  view.SetProperty(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX, 42.0f);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleActorSynced());

  // Nothing invalidated the measure cache, so the same constraint is a HIT.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gActorScaleSyncMeasureCount, settledCount + 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The effective scale as a measure-cache KEY (the measure scale key).
//
// The arrange cache omits a scale term and relies on the invalidation pairing plus a
// DEBUG assert; the measure cache records the scale instead, because `s` is already
// read above its predicate for the constraint normalisation and because a missed
// invalidation then costs a MISS rather than a measured size computed at another
// scale.
//
// On scene, under the CURRENT pairing discipline -- every DropCachedEffectiveScale()
// caller also calls InvalidateLayoutCaches() -- the rejection branch of that term is
// unreachable, so it is defence in depth there. What IS observable, and what the first
// two tests pin, is that the key is recorded at the publish, that it does not cost the
// steady state its hit, and that a real scale change re-publishes it at the new value
// with the producer re-run rather than serving the old entry. Off scene the rejection
// branch is the mechanism for a view's OWN scale -- no invalidation reaches a view in no
// registered layout root -- and the third test pins it. A view with View children whose own
// term a DISABLED policy fixes keeps its key while a UiScalePolicy::ENABLED descendant follows
// the UI scale, so off scene it does not serve its entry at all; the fourth test pins that,
// and that the other views keep their hits.
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
// Non-vacuity (verified by mutation): removing the scale-key publish from the
// measure publish block leaves the member at its constructed NaN, so the recorded-value
// check fails and the second Measure() misses (NaN == s is false) -- the producer count
// becomes 2.
int UtcDaliUiScaleMeasureCacheKeyRecordsEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A measure publish records the effective scale it ran at, and the term keeps the hit");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View view               = View::New();
  gMeasureKeyMeasureCount = 0;
  view.SetMeasureCallback(MeasureCallback::New(&CountingKeyMeasure));

  // Never measured: no entry, so no key.
  DALI_TEST_CHECK(!DataOf(view).IsMeasureCacheValid());

  // First Measure: a MISS. Publishes the entry and both key halves.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  // Exact: the recorded key is a copy of the same GetEffectiveLayoutScale() value the
  // predicate compares against, so an epsilon comparison here would say less.
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);

  // Same constraint, same scale: still a HIT. The producer must not run again.
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// A real scale change re-publishes the key at the NEW scale, with the producer re-run.
// The producer count is what separates "re-published" from "served stale": the key
// could only move by a pass that actually measured.
int UtcDaliUiScaleMeasureCacheKeyRepublishedAtNewScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A scale change re-runs the measure producer and re-publishes the key at the new scale");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

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

  LayoutScaleManager::Get().SetUiScale(1.5f);
  Settle(application);

  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.5f);
  DALI_TEST_CHECK(gMeasureKeyMeasureCount > countAtUnitScale);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// Off scene a UI-scale change reaches no view -- nothing is registered and nothing is
// invalidated -- yet the next Measure() follows it. The scale it recomputes (nothing is
// memoized off scene) misses the measure-cache scale key, so the producer re-runs at the
// new scale and the key is re-published; and the actor push re-reads the
// VIEW_EFFECTIVE_SCALE property, because mEffectiveScaleValid is false there, and moves it
// although mEffectiveScaleActorSynced is still true from the last push. The switch is
// followed the same way.
//
// The claims this test pins (stated, not demonstrated by a mutation run -- the suite is
// build-only here): gating the push on mEffectiveScaleActorSynced alone again leaves the
// property at 1.0 after the change; memoizing the scale off scene makes the Measure() after
// the change a hit at the old scale, failing the producer-count and key checks.
int UtcDaliUiScaleEffectiveScaleOffSceneMeasureFollowsUiScaleP(void)
{
  UiTestApplication application;
  tet_infoline("Off scene, the next Measure() re-measures at a changed UI scale and pushes it to the actor");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View view               = View::New();
  gMeasureKeyMeasureCount = 0;
  view.SetMeasureCallback(MeasureCallback::New(&CountingKeyMeasure));

  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  // Nothing is invalidated: the entry and the actor-side bit both survive the change.
  LayoutScaleManager::Get().SetUiScale(1.5f);
  DALI_TEST_CHECK(DataOf(view).IsMeasureCacheValid());
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleActorSynced());

  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.5f);
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.5f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetScalable(false);
  view.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gMeasureKeyMeasureCount, 3, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).GetLastMeasureScale() == 1.0f);
  DALI_TEST_EQUALS(view.GetProperty<float>(Dali::Ui::Internal::VIEW_EFFECTIVE_SCALE_PROPERTY_INDEX), 1.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

namespace
{
int gFixedContainerMeasureCount   = 0;
int gFixedLeafMeasureCount        = 0;
int gInheritContainerMeasureCount = 0;
int gInheritLeafMeasureCount      = 0;

MeasuredSize CountingFixedLeafMeasure(View, float, float)
{
  ++gFixedLeafMeasureCount;
  return MeasuredSize(10.0f, 10.0f);
}

MeasuredSize CountingInheritLeafMeasure(View, float, float)
{
  ++gInheritLeafMeasureCount;
  return MeasuredSize(10.0f, 10.0f);
}

// Container producers: each measures its only child at its own constraint and reports what the
// child reports.
MeasuredSize CountingFixedContainerMeasure(View view, float width, float height)
{
  ++gFixedContainerMeasureCount;
  return view.GetChildViewAt(0u).Measure(width, height);
}

MeasuredSize CountingInheritContainerMeasure(View view, float width, float height)
{
  ++gInheritContainerMeasureCount;
  return view.GetChildViewAt(0u).Measure(width, height);
}
} // namespace

// Off a scene a view with View children whose UI-scale term a DISABLED policy fixes never serves
// its measure entry -- its own scale key cannot see an ENABLED descendant follow the UI scale --
// while a container whose term is the UI scale (INHERIT up to the root) and a childless view
// still hit on their own keys. On a scene all of them hit, because there the effective-scale
// sync bit the hit predicate reads first is set. The fixed container is INHERIT itself and takes
// the DISABLED policy of its (never measured) parent, so its refusal comes through the policy
// ascent; the INHERIT container is a root, so its ascent ends at the root.
//
// The claims this test pins (stated, not demonstrated by a mutation run -- the suite is
// build-only here): dropping the last term of the measure hit predicate makes the second
// off-scene fixed-container Measure() a hit (fixed container count 1), and so does answering it
// with a value test (the view's scale equal to the UI term) instead of the policy, since both are
// 1.0 here; dropping its `mChildren.Empty()` makes the fixed leaf, which ascends to the same
// DISABLED policy, miss too (fixed leaf count 2); refusing every off-scene container whatever
// its policy makes the INHERIT container miss (inherit container count 2); and a term that
// ignored the sync bit would keep the on-scene fixed container missing (fixed container count 4).
int UtcDaliUiScaleOffSceneFixedTermContainerReMeasuresP(void)
{
  UiTestApplication application;
  tet_infoline("Off scene a container whose UI-scale term DISABLED fixes re-measures on every call; other views hit");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View fixedParent = View::New();
  fixedParent.SetUiScalePolicy(UiScalePolicy::DISABLED);
  View fixedContainer = View::New(); // INHERIT: its term is the parent's DISABLED one
  fixedContainer.SetMeasureCallback(MeasureCallback::New(&CountingFixedContainerMeasure));
  View fixedLeaf = View::New();
  fixedLeaf.SetMeasureCallback(MeasureCallback::New(&CountingFixedLeafMeasure));
  fixedContainer.Add(fixedLeaf);
  fixedParent.Add(fixedContainer);

  View inheritContainer = View::New(); // INHERIT up to the root: its term is the UI scale
  inheritContainer.SetMeasureCallback(MeasureCallback::New(&CountingInheritContainerMeasure));
  View inheritLeaf = View::New();
  inheritLeaf.SetMeasureCallback(MeasureCallback::New(&CountingInheritLeafMeasure));
  inheritContainer.Add(inheritLeaf);

  gFixedContainerMeasureCount   = 0;
  gFixedLeafMeasureCount        = 0;
  gInheritContainerMeasureCount = 0;
  gInheritLeafMeasureCount      = 0;

  // Off scene: the fixed container runs its producer on every call and still publishes its
  // entry; its leaf, measured at the same constraint each time, hits on its own key. The INHERIT
  // container hits, and so its leaf is not asked again.
  fixedContainer.Measure(200.0f, 200.0f);
  fixedContainer.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gFixedContainerMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gFixedLeafMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(fixedContainer).IsMeasureCacheValid());
  DALI_TEST_CHECK(!DataOf(fixedContainer).IsEffectiveScaleValid());

  inheritContainer.Measure(200.0f, 200.0f);
  inheritContainer.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gInheritContainerMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(gInheritLeafMeasureCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(inheritContainer).IsEffectiveScaleValid());

  // On scene: connecting a root drops its subtree's entries, so the first call runs the
  // producers again; the second is a hit for every view. No notification is sent, so no layout
  // pass runs.
  application.GetScene().Add(fixedParent);
  application.GetScene().Add(inheritContainer);
  fixedContainer.Measure(200.0f, 200.0f);
  fixedContainer.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gFixedContainerMeasureCount, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(gFixedLeafMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(fixedContainer).IsEffectiveScaleValid());

  inheritContainer.Measure(200.0f, 200.0f);
  inheritContainer.Measure(200.0f, 200.0f);
  DALI_TEST_EQUALS(gInheritContainerMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(gInheritLeafMeasureCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(inheritContainer).IsEffectiveScaleValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The global master switch: LayoutScaleManager::IsScalable / SetScalable.
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

  DALI_TEST_CHECK(LayoutScaleManager::Get().IsScalable());
  END_TEST;
}

// With scaling off, every policy collapses to 1.0; re-enabling restores the
// stored scale on both an INHERIT root and an ENABLED view.
//
// No window scale is set here, so this pins the SYSTEM half of the switch;
// UtcDaliUiScaleWindowScaleCollapsesUnderSetScalableFalseP pins the per-window half.
int UtcDaliUiScaleSetScalableFalseCollapsesEffectiveScaleP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable(false) collapses every view's effective scale to 1.0 regardless of policy");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(2.0f);

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
  DALI_TEST_EQUALS(GetImpl(inheritView).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledView).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  // Switch off: both collapse to 1.0 even though the stored scale is still 2.0.
  LayoutScaleManager::Get().SetScalable(false);
  DALI_TEST_EQUALS(GetImpl(inheritView).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledView).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  // Switch back on: the preserved 2.0 is re-applied.
  LayoutScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(inheritView).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledView).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// Flipping the switch invalidates the WHOLE subtree, not just the layout root: it
// runs the same subtree reset SetScale does. No layout pass is run in between --
// the flip itself must clear the caches.
int UtcDaliUiScaleSetScalableInvalidatesSubtreeCachesP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable(false) alone drops the arrange cache of the whole subtree");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(2.0f);

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
  LayoutScaleManager::Get().SetScalable(false);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// Re-asserting the current switch value is a no-op: it must not run the subtree
// reset and so must leave a settled tree's caches intact.
int UtcDaliUiScaleSetScalableNoOpWhenUnchangedP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable with the current value is a no-op and preserves the layout caches");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

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
  DALI_TEST_CHECK(LayoutScaleManager::Get().IsScalable());
  LayoutScaleManager::Get().SetScalable(true);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// While disabled, SetScale stores the value (GetScale reads it back) but defers
// both the effect and the relayout: nothing moves and no cache is dropped until
// the switch is turned back on, which then applies the stored value.
int UtcDaliUiScaleSetScaleWhileDisabledStoresButDefersP(void)
{
  UiTestApplication application;
  tet_infoline("SetScale while disabled stores the value but defers effect and relayout until re-enabled");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View view = View::New(); // INHERIT root
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  application.GetScene().Add(view);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  // Disabling drops the cache; re-settling re-publishes it at the unscaled result.
  LayoutScaleManager::Get().SetScalable(false);
  DALI_TEST_CHECK(!DataOf(view).IsArrangeCacheValid());
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());

  // Change the stored scale while disabled: the value is recorded (I5)...
  LayoutScaleManager::Get().SetUiScale(2.0f);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetUiScale(), 2.0f, 0.001f, TEST_LOCATION);
  // ...but with the switch off the view stays unscaled and its cache is untouched.
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());

  // Re-enabling applies the deferred 2.0.
  LayoutScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// A STANDALONE view with a View parent that becomes DEFAULT stops being a scale root: its
// LayoutScaleManager registration is withdrawn with its layout-root registration, so a later
// scale change reaches it only through the root it now belongs to.
//
// Trace: the child registers as a root (with the controller and with LayoutScaleManager) while it
// is STANDALONE. The switch withdraws both registrations and the next pass measures it as a
// regular child of `parent`, publishing its measure entry. Removing `root` from the scene
// unregisters root (a parentless view) but not the child (a DEFAULT view with a View parent);
// root's scale-root reset drops the child's entry on the way out, and a direct Measure()
// re-publishes it off scene. The scale change below then finds no registered root that covers
// the child, and that entry survives.
//
// Non-vacuity (by mutation): dropping the LayoutScaleManager unregistration from the LAYOUT_MODE
// setter leaves the child registered, the scale change resets its subtree caches
// (InvalidateAllLayoutRoots), and the last check fails.
int UtcDaliUiScaleStandaloneToDefaultSwitchLeavesScaleRootSetP(void)
{
  UiTestApplication application;
  tet_infoline("A standalone view switched to DEFAULT under a View parent is no longer a LayoutScaleManager root");

  const float originalScale = LayoutScaleManager::Get().GetUiScale();
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(100.0f);

  View parent = View::New();
  parent.SetRequestedWidth(120.0f);
  parent.SetRequestedHeight(60.0f);
  root.Add(parent);

  View child = View::New();
  child.SetLayoutMode(LayoutMode::STANDALONE);
  child.SetRequestedWidth(30.0f);
  child.SetRequestedHeight(25.0f);
  parent.Add(child);

  application.GetScene().Add(root);
  Settle(application);

  child.SetLayoutMode(LayoutMode::DEFAULT);
  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsMeasureCacheValid());

  application.GetScene().Remove(root);
  child.Measure(30.0f, 25.0f);
  DALI_TEST_CHECK(DataOf(child).IsMeasureCacheValid());

  LayoutScaleManager::Get().SetUiScale(1.5f);
  DALI_TEST_CHECK(DataOf(child).IsMeasureCacheValid());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The per-window scale: LayoutScaleManager::GetWindowScale / SetWindowScale.
//
// A view's final scale is the product of two terms:
//     finalScale = windowScale * uiScale
// where the uiScale term is whichever of 1.0 / the system scale the view's
// UiScalePolicy selects. The window scale is orthogonal to that policy -- it applies
// to DISABLED views too -- but not to the master switch: SetScalable(false) collapses
// BOTH terms to 1.0, and a window scale set while the switch is off is stored and
// applied on SetScalable(true).
//
// ComputeEffectiveScale forms the product only at a node that leaves its INHERIT fast
// path or at a fast-path root; a fast-path node passes its parent's already-multiplied
// value through. That is invariant W1, and the chain test below is what pins it.
//
// The window scale lives in the same process-wide singleton as the scale and the
// switch, and -- unlike them -- it is keyed on a Window that can die before the next
// test runs, so each test here resets the window scales it set.
// ---------------------------------------------------------------------------

// A window nobody has set a window scale on reports the documented default, and so
// does an empty Window handle: there is no record to find and an empty handle must not
// be mistaken for "some window".
int UtcDaliUiScaleWindowScaleDefaultIsOneP(void)
{
  UiTestApplication application;
  tet_infoline("A window with no recorded window scale reports 1.0, and so does an empty Window");

  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(application.GetWindow()), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(Window()), 1.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// The product rule, on the simplest shape that exercises it: a parentless INHERIT view
// is a fast-path root, so it resolves both terms itself.
int UtcDaliUiScaleWindowScaleMultipliesUiScaleP(void)
{
  UiTestApplication application;
  tet_infoline("A view's effective scale is the window scale multiplied by the system scale");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);

  View root = View::New(); // INHERIT (default) and parentless: a fast-path root
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);

  application.GetScene().Add(root);
  Settle(application);

  // Baseline: no window scale yet, so the final scale is the system scale alone. This
  // is also what makes the check after the set a transition rather than a coincidence.
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// INVARIANT W1: the window scale enters an inheritance chain exactly ONCE. Every level
// of a three-deep INHERIT chain must read the same product, not scale^depth.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): multiplying the window scale into the INHERIT
// fast path of ComputeEffectiveScale, which returns the parent's already-multiplied
// memoized value, would make the child read 6.0 and the grandchild 12.0, and the
// second and third checks below would fail.
int UtcDaliUiScaleWindowScaleDoesNotCompoundThroughInheritP(void)
{
  UiTestApplication application;
  tet_infoline("The window scale is applied once per inheritance chain, not once per node");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View root = View::New(); // a fast-path root: forms the product
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New(); // the INHERIT fast path: a pass-through of root
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  root.Add(child);

  View grandChild = View::New(); // the INHERIT fast path: a pass-through of child
  grandChild.SetRequestedWidth(50.0f);
  grandChild.SetRequestedHeight(50.0f);
  child.Add(grandChild);

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(grandChild).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The window scale is orthogonal to UiScalePolicy: DISABLED opts out of the SYSTEM
// scale only, so a DISABLED view resolves to the window scale itself -- not to 1.0 --
// and its INHERIT descendants pass that value through.
int UtcDaliUiScaleWindowScaleAppliesToDisabledViewP(void)
{
  UiTestApplication application;
  tet_infoline("A DISABLED view still receives the window scale, and so do its INHERIT children");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);

  View panel = View::New();
  panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
  panel.SetRequestedWidth(200.0f);
  panel.SetRequestedHeight(200.0f);

  View icon = View::New(); // INHERIT under a DISABLED parent
  icon.SetRequestedWidth(50.0f);
  icon.SetRequestedHeight(50.0f);
  panel.Add(icon);

  application.GetScene().Add(panel);
  Settle(application);

  // Without a window scale, DISABLED is 1.0 -- the behaviour the policy has always had.
  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(icon).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  // With a window scale, the DISABLED subtree renders at the window scale: the system
  // scale is still excluded (3.0 would mean it leaked in), but 1.0 would mean the
  // window scale did not reach a policy that opts out of the system term.
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);
  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(icon).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// ENABLED forms its own product too, so a view that breaks out of a DISABLED subtree
// gets BOTH terms while its DISABLED parent gets only the window scale.
int UtcDaliUiScaleWindowScaleAppliesToEnabledInsideDisabledSubtreeP(void)
{
  UiTestApplication application;
  tet_infoline("An ENABLED view inside a DISABLED subtree resolves to the window scale times the system scale");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View panel = View::New();
  panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
  panel.SetRequestedWidth(200.0f);
  panel.SetRequestedHeight(200.0f);

  View badge = View::New();
  badge.SetUiScalePolicy(UiScalePolicy::ENABLED);
  badge.SetRequestedWidth(50.0f);
  badge.SetRequestedHeight(50.0f);
  panel.Add(badge);

  application.GetScene().Add(panel);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(badge).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The master switch is a complete kill switch: with a window scale in force,
// SetScalable(false) brings every view in the window to 1.0 -- an INHERIT root, a
// DISABLED view (whose system term was 1.0 already) and an ENABLED view alike -- while
// both stored values stay readable, and SetScalable(true) re-applies them.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): without the switch test in
// LayoutScaleManagerImpl::ResolveWindowScale() the three views read 2.0 while the switch
// is off.
int UtcDaliUiScaleWindowScaleCollapsesUnderSetScalableFalseP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable(false) collapses the window scale too, and SetScalable(true) re-applies it");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View root = View::New(); // INHERIT root: both terms
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);

  View panel = View::New(); // DISABLED: the window scale alone
  panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
  panel.SetRequestedWidth(200.0f);
  panel.SetRequestedHeight(200.0f);

  View badge = View::New(); // ENABLED inside the DISABLED panel: both terms
  badge.SetUiScalePolicy(UiScalePolicy::ENABLED);
  badge.SetRequestedWidth(50.0f);
  badge.SetRequestedHeight(50.0f);
  panel.Add(badge);

  application.GetScene().Add(root);
  application.GetScene().Add(panel);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(badge).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  // Switch off: every view is unscaled, the window scale included.
  LayoutScaleManager::Get().SetScalable(false);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(badge).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  // Both stored values are untouched by the flip, so they are still readable.
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetUiScale(), 1.5f, 0.001f, TEST_LOCATION);

  // Switch back on: both terms are re-applied.
  LayoutScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(badge).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The twin of UtcDaliUiScaleSetScaleWhileDisabledStoresButDefersP for the window term.
// While disabled, SetWindowScale stores the value (GetWindowScale reads it back, and the
// HasAnyWindowScale() gate follows the record) but defers both the effect and the
// relayout: nothing moves and no cache is dropped until the switch is turned back on,
// which then applies the stored value. Resetting to 1.0 while disabled erases the record
// at once and defers the relayout the same way.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): an unconditional per-window invalidation
// drops the arrange cache checked while the switch is off; a deferral that also
// skipped the store would leave GetWindowScale() at 1.0 and never reach 3.0; and a
// SetScalable(true) that did not re-apply the window term would stay at 1.5.
int UtcDaliUiScaleSetWindowScaleWhileDisabledStoresButDefersP(void)
{
  UiTestApplication application;
  tet_infoline("SetWindowScale while disabled stores the value but defers effect and relayout until re-enabled");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);

  View view = View::New(); // INHERIT root
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  application.GetScene().Add(view);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);

  // Disabling drops the cache; re-settling re-publishes it at the unscaled result.
  LayoutScaleManager::Get().SetScalable(false);
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  // A start-state check: no record exists yet, so the gate flips below are transitions.
  DALI_TEST_CHECK(!LayoutScaleManagerImpl::HasAnyWindowScale());

  // Set a window scale while disabled: the value is recorded...
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(LayoutScaleManagerImpl::HasAnyWindowScale());
  // ...but with the switch off the view stays unscaled and nothing was invalidated.
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  // Re-enabling applies the deferred window scale together with the stored system scale.
  LayoutScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  // The reset path: 1.0 while disabled erases the record at once and defers the relayout.
  LayoutScaleManager::Get().SetScalable(false);
  Settle(application);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());
  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  DALI_TEST_CHECK(!LayoutScaleManagerImpl::HasAnyWindowScale());
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsArrangeCacheValid());

  // Re-enabling now applies the system scale alone.
  LayoutScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// A window-scale change must invalidate the whole subtree of every root in that
// window, not just the roots themselves -- the same contract SetScale and SetScalable
// have. No layout pass is run after the change: the invalidation itself must clear the
// caches.
int UtcDaliUiScaleWindowScaleInvalidatesSubtreeCachesP(void)
{
  UiTestApplication application;
  tet_infoline("Setting a window scale alone drops the arrange cache of the whole subtree");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

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

  // No layout pass afterwards.
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  DALI_TEST_CHECK(!DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The window scale is PER WINDOW, and so is its invalidation. A window scale on the
// second window moves its root's scale and leaves the first window's settled root
// untouched -- the still-valid arrange cache on the first root is what proves the
// invalidation walk is window-scoped rather than global.
int UtcDaliUiScaleWindowScaleIsPerWindowP(void)
{
  UiTestApplication application;
  tet_infoline("A window scale affects only the roots that live in that window");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  Window mainWindow = application.GetWindow();
  Window other      = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-other");

  View mainRoot = View::New();
  mainRoot.SetRequestedWidth(200.0f);
  mainRoot.SetRequestedHeight(200.0f);
  application.GetScene().Add(mainRoot);
  Settle(application);

  View otherRoot = View::New();
  otherRoot.SetRequestedWidth(200.0f);
  otherRoot.SetRequestedHeight(200.0f);
  other.Add(otherRoot);

  // Baseline for both windows. The arrange cache is asserted on the MAIN root only,
  // because only the main window's layout pass is driven here; the other root's
  // scale is read lazily and needs no pass.
  DALI_TEST_CHECK(DataOf(mainRoot).IsArrangeCacheValid());
  DALI_TEST_EQUALS(GetImpl(mainRoot).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(otherRoot).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(other, 2.0f);

  // The other window's root follows its own window...
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(other), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(otherRoot).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  // ...and the main window is untouched, record, scale and caches alike.
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(mainWindow), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(mainRoot).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(mainRoot).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetWindowScale(other, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The window scale is a term of the effective scale the layout divides by, so it takes
// exactly the bounds SetScale takes: positive, finite and normal. Every rejected value
// leaves the current window scale in place, and so does an empty Window.
int UtcDaliUiScaleWindowScaleInvalidValuesIgnoredN(void)
{
  UiTestApplication application;
  tet_infoline("Non-finite, non-positive and subnormal window scales are ignored, as is an empty Window");

  Window window = application.GetWindow();

  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 2.0f, 0.001f, TEST_LOCATION);

  const float invalidScales[] = {
    std::numeric_limits<float>::quiet_NaN(),
    std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    0.0f,
    -1.5f,
    std::numeric_limits<float>::min() / 2.0f // subnormal: 1 / scale overflows
  };

  for(float invalid : invalidScales)
  {
    LayoutScaleManager::Get().SetWindowScale(window, invalid);
    DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 2.0f, 0.001f, TEST_LOCATION);
  }

  // An empty window has no views to scale, so there is nothing to record it against.
  LayoutScaleManager::Get().SetWindowScale(Window(), 3.0f);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  END_TEST;
}

// 1.0 is the documented default and is exactly representable, so passing it IS the
// reset: the record is dropped and the window's views go back to the plain uiScale.
int UtcDaliUiScaleWindowScaleResetToOneRestoresDefaultP(void)
{
  UiTestApplication application;
  tet_infoline("SetWindowScale(window, 1.0f) resets the window to the default window scale");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);

  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(window), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// Re-setting the window scale a window already has is a no-op: it must not run the
// per-window invalidation walk, so a settled tree keeps its caches.
int UtcDaliUiScaleWindowScaleNoOpWhenUnchangedP(void)
{
  UiTestApplication application;
  tet_infoline("Re-setting the current window scale is a no-op and preserves the layout caches");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

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

  // Re-asserting the value the window already holds invalidates nothing.
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  DALI_TEST_CHECK(DataOf(root).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// A record is keyed on a WeakHandle and compared through the strong handle it
// materialises, so a record that outlives its window can never match a later window
// -- including one the allocator happens to place at the same address. This is why
// the window scale is not keyed on a raw object pointer the way the layout-root set is.
int UtcDaliUiScaleWindowScaleNotInheritedByReusedWindowAddressP(void)
{
  UiTestApplication application;
  tet_infoline("A window scale recorded for a destroyed window never applies to a new window");

  const void* firstAddress = nullptr;
  {
    Window first = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-first");
    firstAddress = first.GetObjectPtr();

    LayoutScaleManager::Get().SetWindowScale(first, 2.0f);
    DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(first), 2.0f, 0.001f, TEST_LOCATION);

    // The record must not keep the window alive: the weak handle empties as soon as
    // the last strong reference goes.
    WeakHandle<Window> weakFirst(first);
    first.Reset();
    DALI_TEST_CHECK(!weakFirst.GetHandle());
  }

  Window second = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-second");

  // Whether the allocator reused the address is out of this test's control, so it is
  // reported rather than asserted. The assertion below holds either way; it is only
  // the address-reuse case that makes it interesting.
  tet_printf("first window address %p, second window address %p (reused: %s)\n", firstAddress, static_cast<const void*>(second.GetObjectPtr()), (firstAddress == second.GetObjectPtr()) ? "yes" : "no");

  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(second), 1.0f, 0.001f, TEST_LOCATION);

  // Setting a window scale prunes before it grows, and RegisterLayoutRoot and
  // UnregisterLayoutRoot prune too (see the two cases below), so exercise the growth
  // point once more.
  LayoutScaleManager::Get().SetWindowScale(second, 1.5f);
  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(second), 1.5f, 0.001f, TEST_LOCATION);
  LayoutScaleManager::Get().SetWindowScale(second, 1.0f);
  END_TEST;
}

// The RegisterLayoutRoot() prune, on its own. Destroying a window is enough even when
// it has no layout root left: its record goes at the next layout-root registration,
// and with it the gate the window term of an effective-scale cache miss tests. The
// application never resets the window scale.
//
// `extra` gives up its only layout root while it is still alive, so it takes none into
// its destruction: no view of it disconnects in its teardown, so the
// UnregisterLayoutRoot() prune never runs for it there and its record outlives it. From
// then until the final check the only prune that runs is RegisterLayoutRoot()'s --
// `mainRoot` connects, nothing disconnects and no window scale is set -- so removing
// that prune leaves the record in place and the final check fails. A window that still
// HAS a root when it dies is reclaimed inside its own teardown instead; that is the
// next case's.
int UtcDaliUiScaleWindowScaleRecordDroppedWhenWindowDestroyedP(void)
{
  UiTestApplication application;
  tet_infoline("A destroyed window's scale record is dropped with no application reset");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  // A start-state check, not the anti-vacuity one (that is the armed-gate check inside
  // the scope). Leaked state could only make the final check FAIL, never pass, and this
  // would blame such a failure on the leak; every case runs in a process of its own
  // (test-harness.cpp), so it is not expected to fire.
  DALI_TEST_CHECK(!LayoutScaleManagerImpl::HasAnyWindowScale());

  {
    Window extra = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-destroyed");

    View extraRoot = View::New();
    extraRoot.SetRequestedWidth(100.0f);
    extraRoot.SetRequestedHeight(100.0f);
    extra.Add(extraRoot);

    LayoutScaleManager::Get().SetWindowScale(extra, 2.0f);

    DALI_TEST_CHECK(LayoutScaleManagerImpl::HasAnyWindowScale());
    DALI_TEST_EQUALS(GetImpl(extraRoot).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

    // Last, while `extra` is alive: the UnregisterLayoutRoot() prune this runs finds a
    // live record and keeps it, and the window is left with no layout root to take
    // into its destruction.
    extra.Remove(extraRoot);
  }
  // `extra` is gone, rootless, and SetWindowScale(extra, 1.0f) was deliberately NOT
  // called. Nothing is asserted at this point ON PURPOSE: whether the record is still
  // there the instant the window dies is timing, not a contract this test may pin.

  // One layout-root registration in the surviving window is all the cleanup needs:
  // connecting a parentless view registers it (OnViewSceneConnection ->
  // RegisterWithLayoutController -> RegisterLayoutRoot). This is also the shape the
  // defect was reported in: destroy a window, then build the next one.
  View mainRoot = View::New();
  mainRoot.SetRequestedWidth(100.0f);
  mainRoot.SetRequestedHeight(100.0f);
  application.GetScene().Add(mainRoot);
  Settle(application);

  DALI_TEST_CHECK(!LayoutScaleManagerImpl::HasAnyWindowScale());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The UnregisterLayoutRoot() prune, and the requirement stated behaviourally: an
// application that destroys a window-scaled window WITHOUT resetting the window scale
// first must keep working normally. The replacement window starts from the documented
// default, its views are laid out without a window scale, and the dead record does not
// hold the HasAnyWindowScale() gate open.
//
// The record is already gone by the time the window is, and the check right after the
// scope pins that. `first` still holds `firstRoot` when it dies, and a dying window's
// views disconnect inside its teardown -- its Scene member goes after ~SceneHolder's
// body and unparents the root view -- so the root comes through UnregisterLayoutRoot()
// there. By then the window's weak handles are already null: dali-core runs
// BaseObject::OnDestroy() before it deletes the object, documents that on
// RefObject::OnDestroy() and pins it with
// UtcDaliWeakHandleInvalidDuringSelfDestruction. So the record reads as expired and
// that prune reclaims it. Nothing else prunes in that teardown --
// RegisterWithLayoutController finds no live window for a view of a dying one, and no
// window scale is set -- so removing the prune from UnregisterLayoutRoot() makes that
// check fail.
int UtcDaliUiScaleWindowScaleNeedsNoResetBeforeWindowDestroyedP(void)
{
  UiTestApplication application;
  tet_infoline("An application that never resets a window scale before destroying its window still behaves normally");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  {
    Window first = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-noreset-first");

    View firstRoot = View::New();
    firstRoot.SetRequestedWidth(100.0f);
    firstRoot.SetRequestedHeight(100.0f);
    first.Add(firstRoot);

    LayoutScaleManager::Get().SetWindowScale(first, 2.0f);
    DALI_TEST_CHECK(LayoutScaleManagerImpl::HasAnyWindowScale());
    DALI_TEST_EQUALS(GetImpl(firstRoot).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  }

  // No reset anywhere above, and no layout activity since the window went: its own
  // teardown has already reclaimed the record.
  DALI_TEST_CHECK(!LayoutScaleManagerImpl::HasAnyWindowScale());

  // The replacement window is a brand new one, and it starts from the default.
  Window second = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-noreset-second");

  View secondRoot = View::New();
  secondRoot.SetRequestedWidth(100.0f);
  secondRoot.SetRequestedHeight(100.0f);
  second.Add(secondRoot);
  Settle(application);

  DALI_TEST_EQUALS(LayoutScaleManager::Get().GetWindowScale(second), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(secondRoot).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!LayoutScaleManagerImpl::HasAnyWindowScale());

  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// The window scale may be set before the window has any layout root at all: the
// per-window walk matches nothing, and the first view to land in the window reads the
// window scale lazily when it first resolves its effective scale.
int UtcDaliUiScaleWindowScaleSetBeforeAnyRootExistsP(void)
{
  UiTestApplication application;
  tet_infoline("A window scale set before any root exists applies to the first view added");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  // Nothing is on the scene yet, so there is no root to invalidate.
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// A view that resolves its scale while OFF-scene has no window and therefore no window
// scale. The window scale cannot reach it there -- it is not a registered root -- and it
// does not have to: off scene the scale is not memoized, so the first read in the window
// resolves the window term, and the layout-root reset that scene connection performs drops
// any layout result recorded at the off-scene scale.
int UtcDaliUiScaleWindowScaleAppliedOnSceneConnectionP(void)
{
  UiTestApplication application;
  tet_infoline("Scene connection re-derives the effective scale of a view settled off-scene");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);

  // Off-scene: no window, so no window scale, and nothing is memoized.
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  // The window scale cannot reach a view no window holds: still 1.0, still not memoized.
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!DataOf(view).IsEffectiveScaleValid());

  // In the window the next read resolves the window term, and memoizes it.
  application.GetScene().Add(view);
  DALI_TEST_EQUALS(GetImpl(view).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(view).IsEffectiveScaleValid());

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// A tree that moves between windows picks up the destination window's scale, in both
// directions. Nothing new is needed for this: the disconnect/connect pair already runs
// the subtree scale reset, so the next resolution asks the new window.
int UtcDaliUiScaleWindowScaleFollowsTreeMovedBetweenWindowsP(void)
{
  UiTestApplication application;
  tet_infoline("A tree moved between windows follows the destination window's scale");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  Window mainWindow = application.GetWindow();
  Window other      = Window::New(PositionSize(0, 0, 480, 800), "ui-scale-window-scale-move");
  LayoutScaleManager::Get().SetWindowScale(other, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(100.0f);
  root.SetRequestedHeight(100.0f);

  View child = View::New(); // the INHERIT fast path: follows root
  child.SetRequestedWidth(50.0f);
  child.SetRequestedHeight(50.0f);
  root.Add(child);

  mainWindow.Add(root);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  mainWindow.Remove(root);
  other.Add(root);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  // ...and back again.
  other.Remove(root);
  mainWindow.Add(root);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(other, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// Geometry, not bits: the window scale is a layout-unit multiplier, not a zoom of a
// fixed layout. The window-sized root constraint is already physical, so a
// MATCH_PARENT root still fills the window exactly, while the content inside it is laid
// out at the final scale -- a 100-unit child becomes 100 * scale pixels wide. Raising
// the window scale therefore makes content bigger and fits LESS logical content in the
// window.
int UtcDaliUiScaleWindowScaleKeepsWindowSizedRootConstraintPhysicalP(void)
{
  UiTestApplication application;
  tet_infoline("A window-scaled window still sizes a MATCH_PARENT root to the window, and scales its content");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.0f);

  const Vector2 windowSize = application.GetScene().GetSize();

  View root = View::New();
  root.SetRequestedWidth(MATCH_PARENT);
  root.SetRequestedHeight(MATCH_PARENT);

  View child = View::New();
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  root.Add(child);

  application.GetScene().Add(root);
  Settle(application);

  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), windowSize.width, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 100.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);
  Settle(application);

  // The root's constraint comes from the window's pixel size and is not a logical
  // size to be scaled, so the root neither grows nor shrinks.
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_WIDTH), windowSize.width, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(root.GetProperty<float>(Actor::Property::SIZE_HEIGHT), windowSize.height, 0.001f, TEST_LOCATION);
  // The child's requested size is logical, so it is laid out at the final scale.
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_WIDTH), 200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 200.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// ---------------------------------------------------------------------------
// The framework-internal WindowScalePolicy: the window-term counterpart of
// UiScalePolicy.
//
// A view that is INHERIT for BOTH terms takes ComputeEffectiveScale's fast path and
// returns its parent's memoized product unchanged. Any other view resolves each term
// on its own by walking up to the nearest ancestor POLICY that decides it -- never by
// dividing an ancestor's product -- and multiplies the two once. Application code has
// no setter for this policy; the framework views that are handed already-scaled
// geometry set it to DISABLED, and these cases set it through ViewDataImpl.
//
// Every case uses a system scale of 1.5 and a window scale of 2.0, so the four
// possible products 1.0 / 1.5 / 2.0 / 3.0 tell every term error apart.
// ---------------------------------------------------------------------------

namespace
{
// A 100x100 view with both policies set: the shape every case below builds.
View MakePolicyView(UiScalePolicy systemPolicy, WindowScalePolicy windowPolicy)
{
  View view = View::New();
  view.SetUiScalePolicy(systemPolicy);
  DataOf(view).SetWindowScalePolicy(windowPolicy);
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  return view;
}
} // namespace

// (a) The fast path is unchanged: an INHERIT/INHERIT view returns its parent's memoized
// product verbatim, a view that opts out of the window term forms its own product, and
// an INHERIT/INHERIT view under THAT view passes the opted-out value through rather
// than multiplying the window scale in again.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): a fast-path test that read only UiScalePolicy
// would send `windowOff` down the fast path to 3.0; a fast path that multiplied the
// window scale again would make `leaf` read 3.0; and a wrong default for the new member
// fails the first check.
int UtcDaliUiScaleWindowScalePolicyInheritFastPathP(void)
{
  UiTestApplication application;
  tet_infoline("An INHERIT/INHERIT view passes its parent's product through, under either window policy");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View root = View::New(); // INHERIT/INHERIT and parentless: a fast-path root
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View plainChild = View::New(); // INHERIT/INHERIT: the fast path
  plainChild.SetRequestedWidth(50.0f);
  plainChild.SetRequestedHeight(50.0f);
  root.Add(plainChild);

  View windowOff = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::DISABLED); // leaves the fast path
  root.Add(windowOff);

  View leaf = View::New(); // INHERIT/INHERIT under windowOff: the fast path again
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(50.0f);
  windowOff.Add(leaf);

  application.GetScene().Add(root);
  Settle(application);

  // Every view starts INHERIT for the window term; only the explicit set moved one.
  DALI_TEST_CHECK(DataOf(plainChild).GetWindowScalePolicy() == WindowScalePolicy::INHERIT);
  DALI_TEST_CHECK(DataOf(windowOff).GetWindowScalePolicy() == WindowScalePolicy::DISABLED);

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  // Exact: the fast path hands the parent's memoized value over, nothing is recombined.
  DALI_TEST_CHECK(GetImpl(plainChild).GetEffectiveLayoutScale() == GetImpl(root).GetEffectiveLayoutScale());
  DALI_TEST_EQUALS(GetImpl(windowOff).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(GetImpl(leaf).GetEffectiveLayoutScale() == GetImpl(windowOff).GetEffectiveLayoutScale());

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// (b) WindowScalePolicy::DISABLED removes the window term and only that: the system
// term the view's UiScalePolicy selects is unchanged, with and without a window scale
// in force. Four roots, so every product is formed at a root and no inheritance is
// involved.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): a resolver that ignored the window policy
// would make `windowOff` read 3.0 once the window scale is set, and one that let it
// touch the system term would make it read 1.0. `plain` moving from 1.5 to 3.0 is what
// makes the window scale observable at all.
int UtcDaliUiScaleWindowScalePolicyDisabledDropsWindowTermP(void)
{
  UiTestApplication application;
  tet_infoline("WindowScalePolicy::DISABLED drops the window term and leaves the system term alone");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);

  View windowOff         = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::DISABLED);
  View windowOffEnabled  = MakePolicyView(UiScalePolicy::ENABLED, WindowScalePolicy::DISABLED);
  View windowOffDisabled = MakePolicyView(UiScalePolicy::DISABLED, WindowScalePolicy::DISABLED);
  View plain             = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::INHERIT);

  application.GetScene().Add(windowOff);
  application.GetScene().Add(windowOffEnabled);
  application.GetScene().Add(windowOffDisabled);
  application.GetScene().Add(plain);
  Settle(application);

  // No window scale yet: each root reads its system term alone.
  DALI_TEST_EQUALS(GetImpl(windowOff).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOffEnabled).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOffDisabled).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(plain).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);

  // With a window scale: only the root that keeps the window term moves.
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);
  DALI_TEST_EQUALS(GetImpl(windowOff).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOffEnabled).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOffDisabled).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(plain).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// (c) A root that opts out of BOTH terms -- the shape the framework gives a view it feeds already-
// scaled geometry -- holding views that set only a UiScalePolicy. Their window term is inherited
// from the root, so none of them picks the window scale back up -- a DISABLED child stays at 1.0
// and an ENABLED child gets the system scale alone -- while an ordinary root beside them is
// window-scaled.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): resolving a mixed node's window term from its
// own window without the policy ascent would make `disabledChild` read 2.0 and
// `enabledChild` and `grandChild` read 3.0.
int UtcDaliUiScaleWindowScalePolicyDisabledSubtreeStaysUnscaledP(void)
{
  UiTestApplication application;
  tet_infoline("Views under a window-DISABLED root inherit its window term, whatever their UiScalePolicy");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View optedOutRoot  = MakePolicyView(UiScalePolicy::DISABLED, WindowScalePolicy::DISABLED);
  View disabledChild = MakePolicyView(UiScalePolicy::DISABLED, WindowScalePolicy::INHERIT);
  View enabledChild  = MakePolicyView(UiScalePolicy::ENABLED, WindowScalePolicy::INHERIT);
  View grandChild    = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::INHERIT);
  optedOutRoot.Add(disabledChild);
  optedOutRoot.Add(enabledChild);
  enabledChild.Add(grandChild);

  View plain = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::INHERIT);

  application.GetScene().Add(optedOutRoot);
  application.GetScene().Add(plain);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(optedOutRoot).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(disabledChild).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(enabledChild).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(grandChild).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(plain).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// (d) WindowScalePolicy::ENABLED breaks out of a window-DISABLED ancestor the way
// UiScalePolicy::ENABLED breaks out of a DISABLED one, and only for the window term:
// `breakout` keeps inheriting its system term (1.0, from the DISABLED panel) and takes
// the window scale, `fullBreakout` breaks out of both.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): treating window ENABLED as INHERIT would make
// `breakout` read 1.0; resolving its system term at itself instead of by the ascent
// would make it read 3.0; and a fast path broken under a mixed parent would move
// `grandChild` off 2.0.
int UtcDaliUiScaleWindowScalePolicyEnabledBreaksOutP(void)
{
  UiTestApplication application;
  tet_infoline("WindowScalePolicy::ENABLED takes the window scale back under a window-DISABLED ancestor");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View panel        = MakePolicyView(UiScalePolicy::DISABLED, WindowScalePolicy::DISABLED);
  View breakout     = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::ENABLED);
  View grandChild   = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::INHERIT);
  View fullBreakout = MakePolicyView(UiScalePolicy::ENABLED, WindowScalePolicy::ENABLED);
  panel.Add(breakout);
  breakout.Add(grandChild);
  panel.Add(fullBreakout);

  application.GetScene().Add(panel);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(panel).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(breakout).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(grandChild).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(fullBreakout).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// (e) Changing an ancestor's window policy invalidates its whole subtree -- the INHERIT
// descendant that memoized the ancestor's product AND the mixed descendant that read the
// ancestor's policy on its ascent -- and schedules a measure pass, exactly like a
// UiScalePolicy change. Re-asserting the current policy invalidates nothing.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): dropping ResetSubtreeScaleAndLayoutCaches()
// from SetWindowScalePolicy leaves `child` and `leaf` with a live sync bit and stale
// values; dropping InvalidateMeasure() leaves the root's dirty bit down; dropping the
// early-out makes the re-assert drop `child`'s arrange cache.
int UtcDaliUiScaleWindowScalePolicyChangeInvalidatesSubtreeP(void)
{
  UiTestApplication application;
  tet_infoline("A window policy change drops the subtree's cached scale and layout and schedules a pass");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View root = View::New();
  root.SetRequestedWidth(200.0f);
  root.SetRequestedHeight(200.0f);

  View child = View::New(); // INHERIT/INHERIT: memoizes root's product
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  root.Add(child);

  View leaf = View::New(); // system DISABLED, window INHERIT: reads root's policy on its ascent
  leaf.SetUiScalePolicy(UiScalePolicy::DISABLED);
  leaf.SetRequestedWidth(50.0f);
  leaf.SetRequestedHeight(50.0f);
  child.Add(leaf);

  application.GetScene().Add(root);
  Settle(application);

  // Baseline, and every bit the change below must clear is live first.
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(leaf).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(leaf).IsEffectiveScaleValid());
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(!DataOf(root).IsMeasureDirty());

  // No layout pass afterwards: the setter itself must invalidate.
  DataOf(root).SetWindowScalePolicy(WindowScalePolicy::DISABLED);

  DALI_TEST_CHECK(!DataOf(child).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(leaf).IsEffectiveScaleValid());
  DALI_TEST_CHECK(!DataOf(child).IsArrangeCacheValid());
  DALI_TEST_CHECK(DataOf(root).IsMeasureDirty());

  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(leaf).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  // Re-asserting the current policy is a no-op: the settled caches survive it.
  Settle(application);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());
  DataOf(root).SetWindowScalePolicy(WindowScalePolicy::DISABLED);
  DALI_TEST_CHECK(DataOf(child).IsArrangeCacheValid());

  // And back: the window term returns to the whole subtree.
  DataOf(root).SetWindowScalePolicy(WindowScalePolicy::INHERIT);
  DALI_TEST_EQUALS(GetImpl(root).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(child).GetEffectiveLayoutScale(), 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(leaf).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// (f) The master switch reaches the window term on EVERY path that reads it, not only
// at a fast-path root: `windowOn` takes the window scale through its own ENABLED window
// policy, and it must collapse with the switch like every other view.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): a switch gate placed on the fast-path root
// alone would leave `windowOn` at 2.0 while the switch is off; `windowOff` pins the
// system half of the same switch on a node that left the fast path.
int UtcDaliUiScaleWindowScalePolicyCollapsesUnderSetScalableFalseP(void)
{
  UiTestApplication application;
  tet_infoline("SetScalable(false) collapses both terms on views that left the fast path too");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  View windowOff = MakePolicyView(UiScalePolicy::INHERIT, WindowScalePolicy::DISABLED);
  View windowOn  = MakePolicyView(UiScalePolicy::DISABLED, WindowScalePolicy::ENABLED);

  application.GetScene().Add(windowOff);
  application.GetScene().Add(windowOn);
  Settle(application);

  DALI_TEST_EQUALS(GetImpl(windowOff).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOn).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetScalable(false);
  DALI_TEST_EQUALS(GetImpl(windowOff).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOn).GetEffectiveLayoutScale(), 1.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetScalable(true);
  DALI_TEST_EQUALS(GetImpl(windowOff).GetEffectiveLayoutScale(), 1.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetImpl(windowOn).GetEffectiveLayoutScale(), 2.0f, 0.001f, TEST_LOCATION);

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}

// (g) The whole policy space of a parent/child pair: 9 parent combinations x 9 child
// combinations, each checked against LITERAL per-term tables -- the specification
// written out, not re-derived from the implementation -- and then all 81 again with the
// master switch off, where every view is 1.0. The parent is a root, so its own terms
// are the root row.
//
// The claim this test pins (stated, not demonstrated by a mutation run here -- no
// build was performed for this change): any per-term inheritance error, a swapped term,
// an ENABLED/DISABLED confusion or a missing switch gate on any path produces one of the
// other three products for at least one combination.
int UtcDaliUiScaleWindowScalePolicyCombinationsP(void)
{
  UiTestApplication application;
  tet_infoline("Every parent/child combination of the two policies resolves to the per-term table");

  const float originalScale    = LayoutScaleManager::Get().GetUiScale();
  const bool  originalScalable = LayoutScaleManager::Get().IsScalable();
  Window      window           = application.GetWindow();

  LayoutScaleManager::Get().SetScalable(true);
  LayoutScaleManager::Get().SetUiScale(1.5f);
  LayoutScaleManager::Get().SetWindowScale(window, 2.0f);

  // Index order == enum value order (INHERIT 0, ENABLED 1, DISABLED 2) for both enums.
  const UiScalePolicy     systemPolicies[3] = {UiScalePolicy::INHERIT, UiScalePolicy::ENABLED, UiScalePolicy::DISABLED};
  const WindowScalePolicy windowPolicies[3] = {WindowScalePolicy::INHERIT, WindowScalePolicy::ENABLED, WindowScalePolicy::DISABLED};

  // The terms, with S = 1.5 and W = 2.0: [policy] for the root parent, and
  // [parent policy][child policy] for the child.
  const float rootSystemTerm[3]     = {1.5f, 1.5f, 1.0f};
  const float rootWindowTerm[3]     = {2.0f, 2.0f, 1.0f};
  const float childSystemTerm[3][3] = {{1.5f, 1.5f, 1.0f}, {1.5f, 1.5f, 1.0f}, {1.0f, 1.5f, 1.0f}};
  const float childWindowTerm[3][3] = {{2.0f, 2.0f, 1.0f}, {2.0f, 2.0f, 1.0f}, {1.0f, 2.0f, 1.0f}};

  for(int pass = 0; pass < 2; ++pass)
  {
    const bool scalable = (pass == 0);
    LayoutScaleManager::Get().SetScalable(scalable);

    for(int ps = 0; ps < 3; ++ps)
    {
      for(int pw = 0; pw < 3; ++pw)
      {
        for(int cs = 0; cs < 3; ++cs)
        {
          for(int cw = 0; cw < 3; ++cw)
          {
            View parent = MakePolicyView(systemPolicies[ps], windowPolicies[pw]);
            View child  = MakePolicyView(systemPolicies[cs], windowPolicies[cw]);
            parent.Add(child);
            application.GetScene().Add(parent);

            // The effective scale is resolved lazily, so no layout pass is needed.
            const float expectedParent = scalable ? rootSystemTerm[ps] * rootWindowTerm[pw] : 1.0f;
            const float expectedChild  = scalable ? childSystemTerm[ps][cs] * childWindowTerm[pw][cw] : 1.0f;
            const float actualParent   = GetImpl(parent).GetEffectiveLayoutScale();
            const float actualChild    = GetImpl(child).GetEffectiveLayoutScale();
            // All 162 combinations share the two locations below, so name the one that broke.
            if(!Dali::Equals(actualParent, expectedParent, 0.001f) || !Dali::Equals(actualChild, expectedChild, 0.001f))
            {
              tet_printf("scalable=%d parent(system %d, window %d) child(system %d, window %d): parent %f (expected %f), child %f (expected %f)\n",
                         scalable ? 1 : 0,
                         ps,
                         pw,
                         cs,
                         cw,
                         actualParent,
                         expectedParent,
                         actualChild,
                         expectedChild);
            }
            DALI_TEST_EQUALS(actualParent, expectedParent, 0.001f, TEST_LOCATION);
            DALI_TEST_EQUALS(actualChild, expectedChild, 0.001f, TEST_LOCATION);

            application.GetScene().Remove(parent);
          }
        }
      }
    }
  }

  LayoutScaleManager::Get().SetWindowScale(window, 1.0f);
  LayoutScaleManager::Get().SetUiScale(originalScale);
  LayoutScaleManager::Get().SetScalable(originalScalable);
  END_TEST;
}
