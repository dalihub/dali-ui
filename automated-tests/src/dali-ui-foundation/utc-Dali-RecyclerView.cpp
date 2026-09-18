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
#include <map>
#include <vector>

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/recycler/item-adapter.h>
#include <dali-ui-foundation/public-api/views/recycler/item-decoration.h>
#include <dali-ui-foundation/public-api/views/recycler/items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/linear-items-layouter.h>
#include <dali-ui-foundation/public-api/views/recycler/recycler-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/events/pan-gesture-devel.h>
#include <dali.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/recycler-view-impl.h>
#undef protected
#undef private

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

// The live view bound to each position, so a test can read back the geometry the
// layouter arranged it at.
std::map<uint32_t, View> gViewsByPosition;

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
  gViewsByPosition[holder.position] = holder.view;
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
  gViewsByPosition.clear();
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

Dali::Ui::Integration::RecyclerViewImpl& GetRecyclerImpl(RecyclerView recycler)
{
  return static_cast<Dali::Ui::Integration::RecyclerViewImpl&>(recycler.GetImplementation());
}

class CountingDecoration : public ItemDecoration, public ItemDecorationInvalidationSupport
{
public:
  mutable int offsets = 0;
  int activated = 0;
  int recycled = 0;
  int layoutStarts = 0;
  int boundsUpdates = 0;
  Dali::Signal<void()> invalidated;

  Insets GetItemOffsets(const ItemViewHolder&) const override
  {
    ++offsets;
    return {1.0f, 4.0f, 2.0f, 8.0f};
  }

  void OnLayoutStart() override
  {
    ++layoutStarts;
  }

  void OnItemActivated(ItemViewHolder&, View) override
  {
    ++activated;
  }

  void OnItemRecycled(const ItemViewHolder&) override
  {
    ++recycled;
  }

  void OnItemBoundsUpdated(const ItemViewHolder&, const LayoutRect&) override
  {
    ++boundsUpdates;
  }

  Dali::Signal<void()>& LayoutInvalidatedSignal() override
  {
    return invalidated;
  }
};

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

int UtcDaliRecyclerViewInternalGeometryAndStateP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  auto& impl = GetRecyclerImpl(recycler);

  DALI_TEST_EQUALS(impl.OnMeasure(-10.0f, -20.0f).ToVector2(), Vector2::ZERO, TEST_LOCATION);
  impl.OnArrange(LayoutRect(0.0f, 0.0f, RECYCLER_WIDTH, RECYCLER_HEIGHT));
  impl.EnsureScroller();
  impl.UpdateScrollerSize();
  impl.UpdateScrollBar();
  DALI_TEST_EQUALS(impl.GetViewportExtent(), RECYCLER_HEIGHT, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetCrossExtent(), RECYCLER_WIDTH, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(impl.GetMaxScrollOffset() > 0.0f);
  DALI_TEST_EQUALS(impl.ClampScrollOffset(-50.0f), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.ClampScrollOffset(100000.0f), impl.GetMaxScrollOffset(), 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(impl.CalculateScrollDuration(100.0f) > 0.0f);
  DALI_TEST_CHECK(impl.SyncScrollOffsetFromScroller() >= 0.0f);

  recycler.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);
  recycler.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  DALI_TEST_EQUALS(static_cast<int>(recycler.GetVerticalScrollBarVisibility()), static_cast<int>(ScrollBarVisibility::Always), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recycler.GetHorizontalScrollBarVisibility()), static_cast<int>(ScrollBarVisibility::Never), TEST_LOCATION);
  recycler.SetOverScrollMode(OverScrollMode::Never);
  DALI_TEST_CHECK(!impl.CanOverScroll());
  recycler.SetOverScrollMode(OverScrollMode::Always);
  DALI_TEST_CHECK(impl.CanOverScroll());
  recycler.SetOverScrollMode(OverScrollMode::ContentScrolls);
  DALI_TEST_CHECK(impl.CanOverScroll());

  int scrollStarted = 0;
  int scrollFinished = 0;
  int dragStarted = 0;
  int dragFinished = 0;
  impl.ScrollStartedSignal().Connect(&application, [&scrollStarted](RecyclerView) { ++scrollStarted; });
  impl.ScrollFinishedSignal().Connect(&application, [&scrollFinished](RecyclerView) { ++scrollFinished; });
  impl.DragStartedSignal().Connect(&application, [&dragStarted](RecyclerView) { ++dragStarted; });
  impl.DragFinishedSignal().Connect(&application, [&dragFinished](RecyclerView) { ++dragFinished; });
  impl.SendScrollStarted();
  impl.SendScrollStarted();
  impl.SendDragStarted();
  impl.SendScrollFinished();
  impl.SendDragFinished();
  impl.SendScrollFinished();
  DALI_TEST_EQUALS(scrollStarted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollFinished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragStarted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragFinished, 1, TEST_LOCATION);

  recycler.SetKeyScrollEnabled(true);
  recycler.SetKeyScrollStep(-10.0f);
  recycler.SetScrollOnFocus(false);
  recycler.SetFocusScrollPeek(-10.0f);
  DALI_TEST_CHECK(recycler.IsKeyScrollEnabled());
  DALI_TEST_EQUALS(recycler.GetKeyScrollStep(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!recycler.GetScrollOnFocus());
  DALI_TEST_EQUALS(recycler.GetFocusScrollPeek(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(impl.IsLayoutAxisDirection(FocusDirection::UP));
  DALI_TEST_CHECK(!impl.IsLayoutAxisDirection(FocusDirection::LEFT));
  DALI_TEST_CHECK(impl.IsForwardDirection(FocusDirection::DOWN));
  DALI_TEST_CHECK(!impl.IsForwardDirection(FocusDirection::UP));
  DALI_TEST_CHECK(impl.IsAtScrollBoundary(FocusDirection::UP));
  DALI_TEST_EQUALS(impl.NextItemPosition(0u, FocusDirection::DOWN), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.NextItemPosition(0u, FocusDirection::UP), impl.INVALID_ITEM_POSITION, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.NextItemPosition(gItemCount - 1u, FocusDirection::DOWN), impl.INVALID_ITEM_POSITION, TEST_LOCATION);
  DALI_TEST_CHECK(impl.FindActiveView(0u));
  DALI_TEST_EQUALS(impl.FindActiveItemPosition(impl.FindActiveView(0u)), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FindActiveItemPosition(View::New()), impl.INVALID_ITEM_POSITION, TEST_LOCATION);

  impl.ScrollToItemMakeVisible(gItemCount, false);
  impl.ScrollToItemMakeVisible(0u, false);
  impl.ScrollToItemMakeVisible(20u, false);
  impl.mKeyRepeatTargetPos = impl.INVALID_ITEM_POSITION;
  DALI_TEST_CHECK(!impl.OnKeyRepeatTimerTick());
  impl.StartKeyRepeatTimer();
  impl.StartKeyRepeatTimer();
  impl.StopKeyRepeatTimer();
  impl.AbortScroll();
  recycler.SetKeyScrollEnabled(false);
  END_TEST;
}

int UtcDaliRecyclerViewInternalAnimationAndRecyclingP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto& impl = GetRecyclerImpl(recycler);

  impl.StartScrollAnimation(0.0f, 0.0f);
  impl.StartScrollAnimation(300.0f, 0.1f);
  DALI_TEST_CHECK(impl.mScrollAnimation);
  application.Render(200u);
  impl.OnScrollAnimationFinished(impl.mScrollAnimation);
  impl.CancelScrollAnimation();

  ItemViewHolder recycled;
  recycled.view = View::New();
  recycled.viewType = 77u;
  for(size_t i = 0u; i < 10u; ++i)
  {
    impl.CacheRecycledItem(recycled);
  }
  DALI_TEST_CHECK(impl.mRecycledItems.size() <= 8u);
  DALI_TEST_CHECK(impl.ObtainItemView(30u, 77u));
  DALI_TEST_CHECK(impl.ObtainItemView(31u, 88u));
  impl.RecycleRecord(impl.mActiveItems.size());
  if(!impl.mActiveItems.empty())
  {
    impl.RecycleRecord(0u);
  }
  impl.RecycleAll();
  DALI_TEST_CHECK(impl.mActiveItems.empty());

  impl.EnsureDefaultEdgeEffects();
  impl.UpdateEdgeEffectSources();
  impl.PullEdgeEffect(-10.0f, 0.0f);
  impl.PullEdgeEffect(10.0f, 0.0f);
  impl.PullEdgeEffect(10.0f, 10.0f);
  impl.ReleaseEdgeEffects(1.0f);
  impl.mStartEdgeActive = true;
  impl.mEndEdgeActive = true;
  impl.ReleaseEdgeEffects(10000.0f);
  impl.FinishEdgeEffects();
  END_TEST;
}

int UtcDaliRecyclerViewInternalHorizontalP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::HORIZONTAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  auto& impl = GetRecyclerImpl(recycler);
  impl.OnArrange(LayoutRect(0.0f, 0.0f, RECYCLER_WIDTH, RECYCLER_HEIGHT));

  DALI_TEST_EQUALS(impl.GetViewportExtent(), RECYCLER_WIDTH, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetCrossExtent(), RECYCLER_HEIGHT, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(impl.IsLayoutAxisDirection(FocusDirection::LEFT));
  DALI_TEST_CHECK(!impl.IsLayoutAxisDirection(FocusDirection::UP));
  recycler.SetScrollOffset(100.0f);
  impl.ApplyScrollerPosition();
  impl.UpdateScrollerSize();
  impl.UpdateScrollBar();
  DALI_TEST_CHECK(impl.SyncScrollOffsetFromScroller() >= 0.0f);
  impl.StartScrollAnimation(200.0f, 0.1f);
  application.Render(200u);
  impl.CancelScrollAnimation();
  END_TEST;
}

int UtcDaliRecyclerViewKeyboardAndWheelNavigationP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  auto& impl = GetRecyclerImpl(recycler);
  impl.OnArrange(LayoutRect(0.0f, 0.0f, RECYCLER_WIDTH, RECYCLER_HEIGHT));
  recycler.SetProperty(Actor::Property::FOCUSABLE, true);
  recycler.SetKeyScrollEnabled(true);
  recycler.SetKeyScrollStep(80.0f);

  DALI_TEST_CHECK(impl.HasIntrinsicWheelHandling());
  WheelEvent wheel = WheelEvent::New(WheelEvent::MOUSE_WHEEL, 0, 0u, Vector2::ZERO, -1, 100u);
  DALI_TEST_CHECK(impl.OnWheelEvent(wheel));

  KeyEvent key = KeyEvent::New();
  key.SetState(KeyEvent::UP);
  key.SetKeyName("Down");
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));

  key.SetState(KeyEvent::DOWN);
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(recycler));

  key.SetKeyName("InvalidKey");
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));
  key.SetKeyName("Left");
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));
  key.SetKeyName("Down");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  key.SetKeyName("Next");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));

  key.SetKeyName("End");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  DALI_TEST_EQUALS(impl.mKeyRepeatTargetPos, gItemCount - 1u, TEST_LOCATION);
  impl.StopKeyRepeatTimer();

  key.SetKeyName("Home");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  DALI_TEST_EQUALS(impl.mKeyRepeatTargetPos, 0u, TEST_LOCATION);
  impl.StopKeyRepeatTimer();
  FocusManager::Get().ClearFocus();
  impl.CancelScrollAnimation();
  impl.FinishEdgeEffects();
  END_TEST;
}

int UtcDaliRecyclerViewPublicEffectsAndSignalsP(void)
{
  UiTestApplication application;
  RecyclerView recycler = RecyclerView::New();
  recycler.SetOverScrollMode(OverScrollMode::Always);
  DALI_TEST_EQUALS(recycler.GetOverScrollMode(), OverScrollMode::Always, TEST_LOCATION);

  EdgeEffect start = EdgeEffect::New();
  EdgeEffect end = EdgeEffect::New();
  recycler.SetStartEdgeEffect(start);
  recycler.SetEndEdgeEffect(end);
  DALI_TEST_CHECK(recycler.GetStartEdgeEffect() == start);
  DALI_TEST_CHECK(recycler.GetEndEdgeEffect() == end);
  DALI_TEST_CHECK(!recycler.IsScrolling());

  int started = 0;
  int finished = 0;
  int dragStarted = 0;
  int dragFinished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView) { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView) { ++finished; });
  recycler.DragStartedSignal().Connect(&application, [&dragStarted](RecyclerView) { ++dragStarted; });
  recycler.DragFinishedSignal().Connect(&application, [&dragFinished](RecyclerView) { ++dragFinished; });
  auto& impl = GetRecyclerImpl(recycler);
  impl.SendScrollStarted();
  impl.SendDragStarted();
  impl.SendDragFinished();
  impl.SendScrollFinished();
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragStarted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragFinished, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliRecyclerViewDecorationLifecycleP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  DALI_TEST_CHECK(recycler.GetAdapter() == adapter);
  auto& impl = GetRecyclerImpl(recycler);

  CountingDecoration decoration;
  recycler.AddItemDecoration(decoration);
  DALI_TEST_EQUALS(impl.mDecorations.size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(decoration.activated > 0);
  DALI_TEST_CHECK(decoration.offsets > 0);
  DALI_TEST_CHECK(decoration.layoutStarts > 0);
  DALI_TEST_CHECK(decoration.boundsUpdates > 0);

  recycler.AddItemDecoration(decoration);
  DALI_TEST_EQUALS(impl.mDecorations.size(), 1u, TEST_LOCATION);

  const int previousBounds = decoration.boundsUpdates;
  decoration.invalidated.Emit();
  application.SendNotification();
  recycler.ScrollBy(80.0f, false);
  application.SendNotification();
  DALI_TEST_CHECK(decoration.boundsUpdates > previousBounds);

  recycler.RemoveItemDecoration(decoration);
  DALI_TEST_CHECK(decoration.recycled > 0);
  DALI_TEST_CHECK(impl.mDecorations.empty());
  recycler.RemoveItemDecoration(decoration);

  {
    CountingDecoration temporary;
    recycler.AddItemDecoration(temporary);
    DALI_TEST_EQUALS(impl.mDecorations.size(), 1u, TEST_LOCATION);
  }
  DALI_TEST_CHECK(impl.mDecorations.empty());

  RecyclerView withoutLayouter = RecyclerView::New();
  CountingDecoration early;
  withoutLayouter.AddItemDecoration(early);
  DALI_TEST_CHECK(GetRecyclerImpl(withoutLayouter).mDecorations.size() == 1u);
  withoutLayouter.RemoveItemDecoration(early);
  END_TEST;
}

// Distinct physical edges catch constructor-order mistakes and verify that
// multiple decorations are summed before either orientation is laid out.
int UtcDaliRecyclerViewDecorationInsetsGeometryP(void)
{
  UiTestApplication application;
  for(auto orientation : {LinearItemsLayouter::Orientation::VERTICAL, LinearItemsLayouter::Orientation::HORIZONTAL})
  {
    ResetCounters();
    ItemAdapter adapter = ItemAdapter::New();
    LinearItemsLayouter layouter = LinearItemsLayouter::New(orientation);
    RecyclerView recycler = BuildRecycler(application, application.GetWindow(), adapter, layouter);
    CountingDecoration firstDecoration;
    CountingDecoration secondDecoration;
    recycler.AddItemDecoration(firstDecoration);
    recycler.AddItemDecoration(secondDecoration);
    application.SendNotification();

    auto& impl = GetRecyclerImpl(recycler);
    impl.OnArrange(LayoutRect(0.0f, 0.0f, RECYCLER_WIDTH, RECYCLER_HEIGHT));
    const bool horizontal = orientation == LinearItemsLayouter::Orientation::HORIZONTAL;
    const float crossExtent = horizontal ? RECYCLER_HEIGHT : RECYCLER_WIDTH;
    const LayoutRect firstSlot = layouter.GetItemBounds(0u, crossExtent);
    const LayoutRect secondSlot = layouter.GetItemBounds(1u, crossExtent);
    if(horizontal)
    {
      DALI_TEST_EQUALS(firstSlot.width, RECYCLER_WIDTH + 10.0f, 0.001f, TEST_LOCATION);
      DALI_TEST_EQUALS(secondSlot.x, firstSlot.width, 0.001f, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(firstSlot.height, RECYCLER_ITEM_HEIGHT + 20.0f, 0.001f, TEST_LOCATION);
      DALI_TEST_EQUALS(secondSlot.y, firstSlot.height, 0.001f, TEST_LOCATION);
    }

    bool foundFirst = false;
    for(const auto& holder : impl.mActiveItems)
    {
      if(holder.position == 0u)
      {
        foundFirst = true;
        const Vector3 position = holder.view.GetProperty<Vector3>(Actor::Property::POSITION);
        const Vector3 size = holder.view.GetProperty<Vector3>(Actor::Property::SIZE);
        DALI_TEST_EQUALS(position.x, 2.0f, 0.001f, TEST_LOCATION);
        DALI_TEST_EQUALS(position.y, 4.0f, 0.001f, TEST_LOCATION);
        DALI_TEST_EQUALS(size.width, horizontal ? RECYCLER_WIDTH : RECYCLER_WIDTH - 10.0f, 0.001f, TEST_LOCATION);
        DALI_TEST_EQUALS(size.height, horizontal ? RECYCLER_HEIGHT - 20.0f : RECYCLER_ITEM_HEIGHT, 0.001f, TEST_LOCATION);
      }
    }
    DALI_TEST_CHECK(foundFirst);
    recycler.RemoveItemDecoration(secondDecoration);
    recycler.RemoveItemDecoration(firstDecoration);
    application.GetWindow().Remove(recycler);
  }
  END_TEST;
}

int UtcDaliRecyclerViewPanGestureLifecycleP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();
  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  auto& impl = GetRecyclerImpl(recycler);

  unsigned int dragStarted = 0u;
  unsigned int dragFinished = 0u;
  recycler.DragStartedSignal().Connect(&application, [&](RecyclerView) { ++dragStarted; });
  recycler.DragFinishedSignal().Connect(&application, [&](RecyclerView) { ++dragFinished; });

  PanGesture started = DevelPanGesture::New(GestureState::STARTED);
  DevelPanGesture::SetPosition(started, Vector2(60.0f, 160.0f));
  impl.OnPanGesture(recycler, started);
  DALI_TEST_CHECK(impl.mIsDragging);

  PanGesture continuing = DevelPanGesture::New(GestureState::CONTINUING);
  DevelPanGesture::SetPosition(continuing, Vector2(60.0f, 100.0f));
  impl.OnPanGesture(recycler, continuing);
  DALI_TEST_CHECK(recycler.GetScrollOffset() > 0.0f);

  PanGesture finished = DevelPanGesture::New(GestureState::FINISHED);
  DevelPanGesture::SetVelocity(finished, Vector2::ZERO);
  impl.OnPanGesture(recycler, finished);
  DALI_TEST_CHECK(!impl.mIsDragging);
  DALI_TEST_EQUALS(dragStarted, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(dragFinished, 1u, TEST_LOCATION);

  impl.OnPanGesture(recycler, started);
  PanGesture cancelled = DevelPanGesture::New(GestureState::CANCELLED);
  impl.OnPanGesture(recycler, cancelled);
  DALI_TEST_CHECK(!impl.mIsDragging);
  DALI_TEST_EQUALS(dragStarted, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(dragFinished, 2u, TEST_LOCATION);

  impl.OnPanGesture(recycler, started);
  DevelPanGesture::SetVelocity(finished, Vector2(0.0f, -1000.0f));
  impl.OnPanGesture(recycler, finished);
  impl.CancelScrollAnimation();
  application.SendNotification();
  application.Render();
  END_TEST;
}

int UtcDaliRecyclerViewFocusEntryAndMoveP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();
  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  recycler.SetKeyScrollEnabled(true);
  recycler.SetKeyScrollStep(80.0f);
  auto& impl = GetRecyclerImpl(recycler);
  DALI_TEST_CHECK(!impl.mActiveItems.empty());
  for(auto& record : impl.mActiveItems)
  {
    if(record.view)
    {
      record.view.SetFocusable(true);
    }
  }

  View first = impl.OnFocusRequested();
  DALI_TEST_CHECK(first);
  DALI_TEST_CHECK(first != recycler);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(first));
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::DOWN));
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() != first);
  FocusManager::Get().ClearFocus();

  recycler.SetKeyScrollEnabled(false);
  DALI_TEST_CHECK(!impl.OnFocusRequested());
  END_TEST;
}

int UtcDaliRecyclerViewLayouterReconfigurationP(void)
{
  UiTestApplication application;
  Window window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New();
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  DALI_TEST_EQUALS(layouter.GetOrientation(), ItemsLayouter::Orientation::VERTICAL, TEST_LOCATION);
  layouter.SetItemExtent(0.0f);
  DALI_TEST_EQUALS(layouter.GetItemExtent(), 1.0f, TEST_LOCATION);
  layouter.SetItemSpacing(-4.0f);
  DALI_TEST_EQUALS(layouter.GetItemSpacing(), 0.0f, TEST_LOCATION);
  layouter.SetItemExtent(64.0f);
  layouter.SetItemSpacing(8.0f);
  DALI_TEST_EQUALS(layouter.GetItemExtent(), 64.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.GetItemSpacing(), 8.0f, TEST_LOCATION);

  layouter.SetOrientation(ItemsLayouter::Orientation::HORIZONTAL);
  DALI_TEST_EQUALS(layouter.GetOrientation(), ItemsLayouter::Orientation::HORIZONTAL, TEST_LOCATION);
  const LayoutRect horizontal = layouter.GetItemBounds(2u, 100.0f);
  DALI_TEST_CHECK(horizontal.width > 0.0f);
  DALI_TEST_EQUALS(horizontal.height, 100.0f, 0.001f, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  recycler.ScrollBy(96.0f, false);
  application.SendNotification();
  DALI_TEST_CHECK(recycler.GetScrollOffset() >= 0.0f);

  layouter.SetOrientation(ItemsLayouter::Orientation::HORIZONTAL);
  layouter.SetOrientation(ItemsLayouter::Orientation::VERTICAL);
  const LayoutRect vertical = layouter.GetItemBounds(2u, 100.0f);
  DALI_TEST_EQUALS(vertical.width, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(vertical.height > 0.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(layouter.CanScrollVertically());
  END_TEST;
}

// A scroll that lands entirely past the realised window has no surviving boundary to
// extend from. Filling forward from the old window's end would bind every skipped item
// and anchor the new first item at the old window's end offset, leaving the gap items
// parented and on screen. The exact new range must be rebuilt instead, and the anchor
// offset must be the new first item's own offset.
int UtcDaliRecyclerViewScrollByDisjointJumpP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  // Estimate unmeasured items at their real extent, so the derived offsets below are
  // not a function of the layouter's default estimate.
  layouter.SetItemExtent(RECYCLER_ITEM_HEIGHT);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);

  // The item views live on the recycler's scroller child; the other child is the bar.
  View scroller;
  for(uint32_t i = 0u; i < recycler.GetChildCount(); ++i)
  {
    View child = View::DownCast(recycler.GetChildAt(i));
    if(child && !ScrollBar::DownCast(child))
    {
      scroller = child;
    }
  }
  DALI_TEST_CHECK(scroller);

  // 40 items of 50 over a 400 viewport with 300 of cache either side: the initial
  // window is 0..13, so the content is 2000 and the maximum offset 1600.
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 13u, TEST_LOCATION);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 14u, TEST_LOCATION);

  gBoundPositions.clear();
  gRecycledViews = 0;

  // Jump to offset 1500: the new window is 24..39, disjoint from 0..13.
  recycler.ScrollBy(1500.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1500.0f, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 24u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 39u, TEST_LOCATION);

  // Only the new range was bound, and the whole old window was recycled.
  std::vector<uint32_t> expected;
  for(uint32_t pos = 24u; pos <= 39u; ++pos)
  {
    expected.push_back(pos);
  }
  DALI_TEST_EQUALS(gBoundPositions.size(), expected.size(), TEST_LOCATION);
  DALI_TEST_CHECK(gBoundPositions == expected);
  DALI_TEST_EQUALS(gRecycledViews, 14, TEST_LOCATION);
  // 16 realised views, not 26: the skipped gap was never materialised.
  DALI_TEST_EQUALS(scroller.GetChildCount(), 16u, TEST_LOCATION);

  gBoundPositions.clear();

  // A small scroll back brings item 23 into the window. Its offset is derived from the
  // rebuilt anchor (24 at 1200), so it lands at 1150.
  recycler.ScrollBy(-50.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1450.0f, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 23u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions[0], 23u, TEST_LOCATION);
  DALI_TEST_CHECK(gViewsByPosition[23u]);
  DALI_TEST_EQUALS(gViewsByPosition[23u].GetProperty<float>(Actor::Property::POSITION_Y), 1150.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// A programmatic scroll can arrive before the layouter has realised any window: the
// last layout pass saw no items, so there is no boundary to extend and nothing to
// recycle. The scroll must build the exact range from scratch, anchored at the new first
// item's own offset, and that window must then serve as a normal anchor.
int UtcDaliRecyclerViewScrollByWithoutRealisedWindowP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount = 0u; // the first layout pass sees no items and realises no window

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(RECYCLER_ITEM_HEIGHT);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);

  View scroller;
  for(uint32_t i = 0u; i < recycler.GetChildCount(); ++i)
  {
    View child = View::DownCast(recycler.GetChildAt(i));
    if(child && !ScrollBar::DownCast(child))
    {
      scroller = child;
    }
  }
  DALI_TEST_CHECK(scroller);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundViews, 0, TEST_LOCATION);

  // The adapter now reports 40 items of 50, but nothing has been notified, so the
  // layouter still has no window when the scroll arrives.
  gItemCount = 40u;
  recycler.ScrollBy(120.0f, false);
  application.SendNotification();

  // Offset 120 over a 400 viewport with 300 of cache either side: items 0..16 are
  // realised from offset 0 and nothing was recycled.
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 120.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 16u, TEST_LOCATION);
  std::vector<uint32_t> expected;
  for(uint32_t pos = 0u; pos <= 16u; ++pos)
  {
    expected.push_back(pos);
  }
  DALI_TEST_EQUALS(gBoundPositions.size(), expected.size(), TEST_LOCATION);
  DALI_TEST_CHECK(gBoundPositions == expected);
  DALI_TEST_EQUALS(gRecycledViews, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 17u, TEST_LOCATION);
  DALI_TEST_CHECK(gViewsByPosition[0u]);
  DALI_TEST_EQUALS(gViewsByPosition[0u].GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(gViewsByPosition[16u]);
  DALI_TEST_EQUALS(gViewsByPosition[16u].GetProperty<float>(Actor::Property::POSITION_Y), 800.0f, 0.001f, TEST_LOCATION);

  // The rebuilt window is a normal anchor: a small scroll extends it from its end.
  gBoundPositions.clear();
  recycler.ScrollBy(50.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 170.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 17u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions[0], 17u, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecycledViews, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(scroller.GetChildCount(), 18u, TEST_LOCATION);
  DALI_TEST_CHECK(gViewsByPosition[17u]);
  DALI_TEST_EQUALS(gViewsByPosition[17u].GetProperty<float>(Actor::Property::POSITION_Y), 850.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}
