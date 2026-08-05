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
// There is no arrange cache-hit path yet, so both are library-side write-only
// and can only be observed through ViewDataImpl's white-box accessors.

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
