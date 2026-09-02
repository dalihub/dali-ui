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
#include <algorithm>
#include <iostream>
#include <vector>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/linear-items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/recycler-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_recycler_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_recycler_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// RecyclerView had no automated coverage at all before this file. These are black-box
// tests over the public handle: adapter/layouter wiring, scrolling, visible ranges,
// view recycling, data-change notifications and item geometry.

namespace
{
const float RECYCLER_WIDTH       = 200.0f;
const float RECYCLER_HEIGHT      = 400.0f;
const float RECYCLER_ITEM_HEIGHT = 50.0f;

uint32_t gItemCount     = 40u;
int      gCreatedViews  = 0;
int      gBoundViews    = 0;
int      gRecycledViews = 0;

std::vector<uint32_t> gBoundPositions;

uint32_t TestItemCount()
{
  return gItemCount;
}

void TestCreateViewHolder(ItemViewHolder& holder)
{
  ++gCreatedViews;
  View item = View::New();
  item.SetRequestedWidth(RECYCLER_WIDTH);
  item.SetRequestedHeight(RECYCLER_ITEM_HEIGHT);
  holder.view = item;
}

void TestBindViewHolder(ItemViewHolder& holder)
{
  ++gBoundViews;
  gBoundPositions.push_back(holder.position);
}

void TestRecycleViewHolder(ItemViewHolder&)
{
  ++gRecycledViews;
}

void ResetCounters()
{
  gItemCount     = 40u;
  gCreatedViews  = 0;
  gBoundViews    = 0;
  gRecycledViews = 0;
  gBoundPositions.clear();
}

// A ready-to-drive recycler: adapter and layouter are owned by the caller so each
// test can keep driving notifications on them after construction.
RecyclerView BuildRecycler(UiTestApplication& application, Window window, ItemAdapter& adapter,
                           LinearItemsLayouter& layouter, bool connectRecycled = false)
{
  adapter.GetItemCountSignal().Connect(&TestItemCount);
  adapter.CreateViewHolderSignal().Connect(&TestCreateViewHolder);
  adapter.BindViewHolderSignal().Connect(&TestBindViewHolder);
  if(connectRecycled)
  {
    adapter.RecycleViewHolderSignal().Connect(&TestRecycleViewHolder);
  }

  RecyclerView recycler = RecyclerView::New();
  recycler.SetRequestedWidth(RECYCLER_WIDTH);
  recycler.SetRequestedHeight(RECYCLER_HEIGHT);
  window.Add(recycler);

  recycler.SetItemsLayouter(layouter);
  recycler.SetAdapter(adapter);

  application.SendNotification();
  application.SendNotification();

  return recycler;
}

} // namespace

int UtcDaliRecyclerViewNewP(void)
{
  UiTestApplication application;

  RecyclerView recycler = RecyclerView::New();
  DALI_TEST_CHECK(recycler);
  END_TEST;
}

int UtcDaliRecyclerViewConstructorP(void)
{
  UiTestApplication application;

  RecyclerView recycler;
  DALI_TEST_CHECK(!recycler);
  END_TEST;
}

int UtcDaliRecyclerViewCopyAndMoveP(void)
{
  UiTestApplication application;

  RecyclerView recycler = RecyclerView::New();
  RecyclerView copy(recycler);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == recycler);

  RecyclerView assigned;
  assigned = recycler;
  DALI_TEST_CHECK(assigned == recycler);

  RecyclerView moved = std::move(copy);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!copy);
  END_TEST;
}

int UtcDaliRecyclerViewDownCastP(void)
{
  UiTestApplication application;

  RecyclerView recycler = RecyclerView::New();
  View         asView   = recycler;
  RecyclerView back     = RecyclerView::DownCast(asView);
  DALI_TEST_CHECK(back);
  DALI_TEST_CHECK(back == recycler);
  END_TEST;
}

int UtcDaliRecyclerViewDownCastN(void)
{
  UiTestApplication application;

  BaseHandle   empty;
  RecyclerView recycler = RecyclerView::DownCast(empty);
  DALI_TEST_CHECK(!recycler);

  View         plain = View::New();
  RecyclerView wrong = RecyclerView::DownCast(plain);
  DALI_TEST_CHECK(!wrong);
  END_TEST;
}

// Wiring an adapter and a layouter must produce a populated viewport: the adapter is
// asked to create and bind views, and only the visible window's worth of them.
int UtcDaliRecyclerViewAdapterAndLayouterWiringP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  DALI_TEST_CHECK(recycler.GetItemsLayouter() == layouter);
  DALI_TEST_CHECK(gCreatedViews > 0);
  DALI_TEST_CHECK(gBoundViews > 0);
  // Virtualisation: far fewer views than items are realised.
  DALI_TEST_CHECK(gCreatedViews < static_cast<int>(gItemCount));
  END_TEST;
}

// The visible range starts at the top and covers at least one viewport of items.
int UtcDaliRecyclerViewVisiblePositionsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() >= recycler.GetFirstVisiblePosition());
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() < gItemCount);
  END_TEST;
}

int UtcDaliRecyclerViewScrollByP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);

  const uint32_t lastBefore = recycler.GetLastVisiblePosition();

  recycler.ScrollBy(500.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 500.0f, 1.0f, TEST_LOCATION);
  // The realised window moved down with the offset.
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() > lastBefore);
  END_TEST;
}

int UtcDaliRecyclerViewSetScrollOffsetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  recycler.SetScrollOffset(500.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 500.0f, 1.0f, TEST_LOCATION);

  // Back to the top.
  recycler.SetScrollOffset(0.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  END_TEST;
}

// Scrolling far must reuse views rather than create one per item.
int UtcDaliRecyclerViewRecycleOnScrollP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter, true);

  // Drop the read-ahead cache so the realised window is exactly the viewport: any
  // view needed after this point must come from the recycle pool, not a new creation.
  recycler.SetCacheExtent(0.0f, 0.0f);
  application.SendNotification();

  const int createdAfterFirstLayout = gCreatedViews;
  const int boundAfterFirstLayout   = gBoundViews;

  for(int i = 0; i < 10; ++i)
  {
    recycler.ScrollBy(100.0f, false);
    application.SendNotification();
  }

  tet_printf("recycle-on-scroll: created %d (was %d), bound %d (was %d), recycled %d\n",
             gCreatedViews, createdAfterFirstLayout, gBoundViews, boundAfterFirstLayout, gRecycledViews);

  // New positions were bound as they scrolled in...
  DALI_TEST_CHECK(gBoundViews > boundAfterFirstLayout + 10);
  // ...and views were handed back for reuse...
  DALI_TEST_CHECK(gRecycledViews > 0);
  // ...so scrolling 20 items' worth created (almost) no new views.
  DALI_TEST_CHECK(gCreatedViews <= createdAfterFirstLayout + 4);
  END_TEST;
}

int UtcDaliRecyclerViewScrollToPositionP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  recycler.ScrollToPosition(20u, false);
  application.SendNotification();

  DALI_TEST_CHECK(recycler.GetScrollOffset() > 0.0f);
  DALI_TEST_CHECK(recycler.GetFirstVisiblePosition() <= 20u);
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() >= 20u);
  END_TEST;
}

int UtcDaliRecyclerViewNotifyDataSetChangedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  const int boundBefore = gBoundViews;

  gItemCount = 5u;
  adapter.NotifyDataSetChanged();
  application.SendNotification();

  DALI_TEST_CHECK(gBoundViews > boundBefore);
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() < gItemCount);
  END_TEST;
}

int UtcDaliRecyclerViewNotifyItemVariantsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  // Content-only change rebinds the affected active views without a relayout.
  int boundBefore = gBoundViews;
  adapter.NotifyItemContentChanged(0u, 2u);
  application.SendNotification();
  DALI_TEST_CHECK(gBoundViews > boundBefore);

  // A size-affecting change relayouts.
  boundBefore = gBoundViews;
  adapter.NotifyItemChanged(0u, 2u);
  application.SendNotification();
  DALI_TEST_CHECK(gBoundViews > boundBefore);

  // Insert / remove / move keep the recycler consistent with the adapter count.
  gItemCount += 3u;
  adapter.NotifyItemInserted(1u, 3u);
  application.SendNotification();
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() < gItemCount);

  gItemCount -= 3u;
  adapter.NotifyItemRemoved(1u, 3u);
  application.SendNotification();
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() < gItemCount);

  adapter.NotifyItemMoved(0u, 4u);
  application.SendNotification();
  DALI_TEST_CHECK(recycler.GetLastVisiblePosition() < gItemCount);
  END_TEST;
}

// Item geometry: consecutive items are stacked along the scroll axis with the
// layouter's item extent, and span the cross extent.
int UtcDaliRecyclerViewItemGeometryP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  const LayoutRect first  = layouter.GetItemBounds(0u, RECYCLER_WIDTH);
  const LayoutRect second = layouter.GetItemBounds(1u, RECYCLER_WIDTH);

  DALI_TEST_EQUALS(first.x, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(first.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(first.width, RECYCLER_WIDTH, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(first.height, RECYCLER_ITEM_HEIGHT, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(second.y, first.y + first.height, 0.001f, TEST_LOCATION);

  DALI_TEST_CHECK(layouter.CanScrollVertically());
  DALI_TEST_CHECK(!layouter.CanScrollHorizontally());
  DALI_TEST_EQUALS(layouter.ComputeScrollExtent(), RECYCLER_HEIGHT, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRecyclerViewHorizontalLayouterP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::HORIZONTAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  DALI_TEST_CHECK(layouter.CanScrollHorizontally());
  DALI_TEST_CHECK(!layouter.CanScrollVertically());
  DALI_TEST_CHECK(gCreatedViews > 0);

  recycler.ScrollBy(150.0f, false);
  application.SendNotification();
  DALI_TEST_CHECK(recycler.GetScrollOffset() > 0.0f);
  END_TEST;
}

int UtcDaliRecyclerViewCacheExtentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);

  float before = -1.0f;
  float after  = -1.0f;
  recycler.GetCacheExtent(before, after);
  DALI_TEST_CHECK(before >= 0.0f && after >= 0.0f);

  recycler.SetCacheExtent(0.0f, 0.0f);
  application.SendNotification();
  recycler.GetCacheExtent(before, after);
  DALI_TEST_EQUALS(before, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(after, 0.0f, 0.001f, TEST_LOCATION);

  // Negative values are clamped, not stored.
  recycler.SetCacheExtent(-100.0f, -100.0f);
  application.SendNotification();
  recycler.GetCacheExtent(before, after);
  DALI_TEST_EQUALS(before, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(after, 0.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRecyclerViewClearAdapterP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter, true);

  DALI_TEST_CHECK(gCreatedViews > 0);

  recycler.ClearAdapter();
  application.SendNotification();

  // All realised views were handed back; scrolling afterwards is a no-op.
  DALI_TEST_CHECK(gRecycledViews > 0);
  recycler.ScrollBy(100.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRecyclerViewNoAdapterN(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  RecyclerView recycler = RecyclerView::New();
  recycler.SetRequestedWidth(RECYCLER_WIDTH);
  recycler.SetRequestedHeight(RECYCLER_HEIGHT);
  window.Add(recycler);
  application.SendNotification();

  // Every scroll entry point must tolerate a missing layouter/adapter.
  recycler.ScrollBy(100.0f, false);
  recycler.SetScrollOffset(100.0f);
  recycler.ScrollToPosition(3u, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  END_TEST;
}
