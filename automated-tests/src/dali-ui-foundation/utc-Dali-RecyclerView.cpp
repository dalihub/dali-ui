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

// Per-position main-axis extents applied at bind time (heights, or widths for a horizontal
// list). Empty keeps every item at the size it was created with.
std::vector<float> gItemExtents;
bool               gItemExtentsAreWidths = false;

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
  if(holder.position < gItemExtents.size())
  {
    if(gItemExtentsAreWidths)
    {
      holder.view.SetRequestedWidth(gItemExtents[holder.position]);
    }
    else
    {
      holder.view.SetRequestedHeight(gItemExtents[holder.position]);
    }
  }
}

// Leaves position 5 without a view, as an adapter that cannot produce one would.
void TestCreateViewHolderExceptFive(ItemViewHolder& holder)
{
  if(holder.position != 5u)
  {
    TestCreateViewHolder(holder);
  }
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
  gItemExtents.clear();
  gItemExtentsAreWidths = false;
}

// The per-position extent table from (count, extent) runs.
std::vector<float> ExtentRuns(std::initializer_list<std::pair<uint32_t, float>> runs)
{
  std::vector<float> extents;
  for(const auto& run : runs)
  {
    extents.insert(extents.end(), run.first, run.second);
  }
  return extents;
}

// A view's main-axis position as arranged (event side) or as last rendered.
float MainPosition(View view, bool horizontal)
{
  return view.GetProperty<float>(horizontal ? Actor::Property::POSITION_X : Actor::Property::POSITION_Y);
}

float RenderedMainPosition(View view, bool horizontal)
{
  const Vector3 position = view.GetCurrentProperty<Vector3>(Actor::Property::POSITION);
  return horizontal ? position.x : position.y;
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

// Every realised item sits at its item offset (plus the leading decoration) and starts where its
// predecessor ends plus `gap` (trailing decoration, spacing, leading decoration): no seam, no gap.
void CheckRealisedWindow(RecyclerView recycler, LinearItemsLayouter layouter, bool horizontal, float leading, float gap, const char* location)
{
  auto&          impl  = GetRecyclerImpl(recycler);
  const uint32_t first = recycler.GetFirstVisiblePosition();
  const uint32_t last  = recycler.GetLastVisiblePosition();
  const float    cross = horizontal ? RECYCLER_HEIGHT : RECYCLER_WIDTH;
  DALI_TEST_EQUALS(static_cast<uint32_t>(impl.mActiveItems.size()), last - first + 1u, location);
  for(uint32_t position = first; position <= last; ++position)
  {
    const View view = impl.FindActiveView(position);
    DALI_TEST_CHECK(view);
    const LayoutRect slot = layouter.GetItemBounds(position, cross);
    DALI_TEST_EQUALS(MainPosition(view, horizontal), (horizontal ? slot.x : slot.y) + leading, 0.01f, location);
    if(position < last)
    {
      DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(position + 1u), horizontal), MainPosition(view, horizontal) + gItemExtents[position] + gap, 0.01f, location);
    }
  }
}

// Renders the running scroll animation frame by frame until it finishes. Between two frames every
// item rendered in both must move on screen by exactly the original ease-out path of `distance`
// over `duration`: a coordinate correction and the retargeting it causes must not show.
// `framesBefore` frames of the animation were rendered before the call.
uint32_t RenderScrollAnimation(UiTestApplication& application, RecyclerView recycler, bool horizontal, float distance, float duration, const char* location, uint32_t framesBefore = 0u)
{
  auto& impl    = GetRecyclerImpl(recycler);
  auto  easeOut = [](float progress)
  {
    progress = std::min(1.0f, std::max(0.0f, progress));
    return (progress - 1.0f) * (progress - 1.0f) * (progress - 1.0f) + 1.0f;
  };
  std::map<uint32_t, std::pair<View, float>> previous;
  uint32_t                                   frames   = 0u;
  uint32_t                                   compared = 0u;
  float                                      error    = 0.0f;
  while(impl.mScrollAnimation && frames < 200u)
  {
    application.Render(16u);
    ++frames;
    const float                                scroller = RenderedMainPosition(impl.mScroller, horizontal);
    std::map<uint32_t, std::pair<View, float>> current;
    for(const auto& holder : impl.mActiveItems)
    {
      current[holder.position] = {holder.view, RenderedMainPosition(holder.view, horizontal) + scroller};
    }
    const float frame = static_cast<float>(frames + framesBefore);
    const float moved = -distance * (easeOut(frame * 0.016f / duration) - easeOut((frame - 1.0f) * 0.016f / duration));
    for(const auto& entry : current)
    {
      const auto found = previous.find(entry.first);
      if(found != previous.end() && found->second.first == entry.second.first)
      {
        error = std::max(error, std::abs(entry.second.second - found->second.second - moved));
        ++compared;
      }
    }
    previous = current;
    application.SendNotification();
  }
  DALI_TEST_CHECK(compared > 0u);
  DALI_TEST_EQUALS(error, 0.0f, 0.05f, location);
  return frames;
}

// Renders a vertical scroll animation that heads for the end of the content until it finishes. The end
// moves while the items near it are measured, so the path is not one ease-out. Every frame the content
// must move as one piece, forward only and never faster than in the frame before (the speed carries over
// when the animation follows the end), and the scroller must never show more than the content.
uint32_t RenderScrollToEnd(UiTestApplication& application, RecyclerView recycler, const char* location)
{
  auto&                                      impl = GetRecyclerImpl(recycler);
  std::map<uint32_t, std::pair<View, float>> previous;
  float                                      previousScroller = 0.0f;
  float                                      previousMove     = 1.0e9f;
  uint32_t                                   frames           = 0u;
  float                                      seam             = 0.0f;
  float                                      backward         = 0.0f;
  float                                      speedUp          = 0.0f;
  float                                      overshoot        = 0.0f;
  while(impl.mScrollAnimation && frames < 400u)
  {
    application.Render(16u);
    ++frames;
    const float                                scroller = RenderedMainPosition(impl.mScroller, false);
    std::map<uint32_t, std::pair<View, float>> current;
    for(const auto& holder : impl.mActiveItems)
    {
      current[holder.position] = {holder.view, RenderedMainPosition(holder.view, false) + scroller};
    }
    if(frames > 1u)
    {
      const float move = previousScroller - scroller; // toward the end
      for(const auto& entry : current)
      {
        const auto found = previous.find(entry.first);
        if(found != previous.end() && found->second.first == entry.second.first)
        {
          seam = std::max(seam, std::abs(entry.second.second - found->second.second + move));
        }
      }
      backward     = std::max(backward, -move);
      speedUp      = std::max(speedUp, move - previousMove);
      previousMove = move;
    }
    previous         = current;
    previousScroller = scroller;
    application.SendNotification();
    overshoot = std::max(overshoot, -scroller - impl.GetMaxScrollOffset());
  }
  DALI_TEST_EQUALS(seam, 0.0f, 0.05f, location);
  DALI_TEST_EQUALS(backward, 0.0f, 0.01f, location);
  DALI_TEST_CHECK(speedUp <= 0.05f);
  DALI_TEST_CHECK(overshoot <= 0.01f);
  return frames;
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

// Scrolling up into items that were never measured and are larger than the estimate. Laying them
// out top-down from estimated offsets made them overlap the first surviving item. They must be
// glued upward from its top edge instead, and because measuring them moves every offset below
// them, the window and the scroll offset move with them, so that nothing visible moves.
int UtcDaliRecyclerViewScrollUpGluesLargerItemsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);

  // Items 40..59 are 80 high but nothing has measured them; from item 60 on the estimate is exact.
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 3000.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 60u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 67u, TEST_LOCATION);
  const View survivor = impl.FindActiveView(60u);
  DALI_TEST_EQUALS(MainPosition(survivor, false), 3000.0f, 0.001f, TEST_LOCATION);

  gBoundPositions.clear();
  gRecycledViews = 0;

  // Up by 300: items 59..56 are glued above item 60, each 30 larger than estimated. The offsets
  // below them move by 4 x 30 = 120, and so does the scroll offset: 3000 - 300 + 120.
  recycler.ScrollBy(-300.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2820.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 56u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 61u, TEST_LOCATION);
  const std::vector<uint32_t> glued{59u, 58u, 57u, 56u};
  DALI_TEST_CHECK(gBoundPositions == glued);
  DALI_TEST_EQUALS(gRecycledViews, 6, TEST_LOCATION);
  // The surviving item moved on screen by exactly the scrolled distance.
  DALI_TEST_CHECK(impl.FindActiveView(60u) == survivor);
  DALI_TEST_EQUALS(MainPosition(survivor, false), 3120.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(survivor, false) - recycler.GetScrollOffset(), 300.0f, 0.001f, TEST_LOCATION);
  // Item 59 ends where item 60 starts; item 56 sits at its item offset 40 x 50 + 16 x 50.
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(59u), false) + 80.0f, 3120.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(56u), false), 2800.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  // The scroller follows the corrected offset; the content extent counts the measured items.
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), -2820.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 5120.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// The same with items smaller than the estimate: top-down placement opened a gap above the first
// surviving item instead of an overlap.
int UtcDaliRecyclerViewScrollUpGluesSmallerItemsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 30.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();
  const View survivor = impl.FindActiveView(60u);

  gBoundPositions.clear();
  gRecycledViews = 0;

  // Up by 300 glues items 59..50 (each 20 smaller than estimated): 3000 - 300 - 10 x 20.
  recycler.ScrollBy(-300.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2500.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 50u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 61u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions.size(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecycledViews, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(survivor, false) - recycler.GetScrollOffset(), 300.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(59u), false) + 30.0f, MainPosition(survivor, false), 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(50u), false), 2500.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 4800.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// After a far jump with every measurement dropped, a scroll past the top must still end with item
// 0 exactly at the top: the items above are glued with their measured extents, and the
// correction they cause is followed by the clamp to the start.
int UtcDaliRecyclerViewScrollToTopGluesUnmeasuredItemsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 80.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f); // drops every measurement: items 0..29 are estimated again
  application.SendNotification();

  recycler.SetScrollOffset(900.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 900.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 12u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(12u), false), 600.0f, 0.001f, TEST_LOCATION);

  gBoundPositions.clear();
  recycler.SetOverScrollMode(OverScrollMode::Never); // the request overscrolls: keep the scroller still
  recycler.ScrollBy(-100000.0f, false);
  application.SendNotification();

  // Items 11..0 were glued (each 30 larger: the offsets below moved by 360) and the request past
  // the start still lands on it.
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 8u, TEST_LOCATION);
  std::vector<uint32_t> glued;
  for(uint32_t position = 12u; position-- > 0u;)
  {
    glued.push_back(position);
  }
  DALI_TEST_CHECK(gBoundPositions == glued);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(8u), false), 640.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 3750.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// SetScrollOffset is absolute. Its relative scroll to 0 glues the items above, whose correction
// moves the coordinates the request was made in; it aims again and ends with item 0 at the top.
int UtcDaliRecyclerViewSetScrollOffsetZeroReachesTopP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 80.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f);
  recycler.SetScrollOffset(900.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 12u, TEST_LOCATION);

  gBoundPositions.clear();
  gRecycledViews = 0;

  // The first pass glues items 11..0 (offset 0 + 360); the aim scrolls the remaining 360.
  recycler.SetScrollOffset(0.0f);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions.size(), 12u, TEST_LOCATION);
  DALI_TEST_EQUALS(gRecycledViews, 16, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// The same with items smaller than estimated: the correction would put the offset above the top,
// so the list is clamped flush instead of showing space above item 0.
int UtcDaliRecyclerViewScrollToTopClampsFlushP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 30.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f);
  application.SendNotification();
  recycler.SetScrollOffset(900.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 12u, TEST_LOCATION);

  recycler.SetOverScrollMode(OverScrollMode::Never); // the request overscrolls: keep the scroller still
  recycler.ScrollBy(-100000.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 23u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(23u), false), 690.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 2400.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// A rebuilt window is anchored with estimated extents. When the anchor measures shorter than its
// estimate it ends above the window, and is dropped rather than kept realised.
int UtcDaliRecyclerViewRebuildDropsShortAnchorP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 30.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 34u, TEST_LOCATION);

  gBoundPositions.clear();
  gRecycledViews = 0;

  // Offset 935 with 300 of cache: the window starts at 635, inside item 12's estimate (600..650).
  // Measured at 30 it ends at 630 and is dropped; item 13 starts at 630.
  recycler.SetScrollOffset(935.0f);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 935.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 13u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 39u, TEST_LOCATION);
  DALI_TEST_EQUALS(gBoundPositions.size(), 28u, TEST_LOCATION); // 12..39
  DALI_TEST_EQUALS(gRecycledViews, 21, TEST_LOCATION);          // the old 34..53 and item 12
  DALI_TEST_CHECK(!impl.FindActiveView(12u));
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(13u), false), 630.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  END_TEST;
}

// A request past the end measures the items through to the last one, so that the last item's end
// is the content extent and the maximum offset puts it at the bottom of the viewport. Here the
// unmeasured tail is shorter than estimated.
int UtcDaliRecyclerViewScrollToEndMeetsShorterContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 30.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  auto&        impl     = GetRecyclerImpl(recycler);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 3000.0f, 0.001f, TEST_LOCATION);

  recycler.SetScrollOffset(100000.0f);
  application.SendNotification();

  // 40 x 50 + 20 x 30 = 2600, the last item from 2570.
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 2600.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 2200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 59u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(59u), false) + 30.0f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);

  recycler.ScrollBy(-500.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1700.0f, 0.001f, TEST_LOCATION);
  recycler.ScrollBy(10000.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2200.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// The same with a tail longer than estimated: the end moves out as it is measured, and the
// request past it follows until the last item meets the bottom of the viewport.
int UtcDaliRecyclerViewScrollToEndMeetsLongerContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter);
  auto&        impl     = GetRecyclerImpl(recycler);

  recycler.SetScrollOffset(100000.0f);
  application.SendNotification();

  // Items 46..59 are measured (80); 40..45 are still estimated: 2000 + 6 x 50 + 14 x 80.
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 3420.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 3020.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 59u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(59u), false) + 80.0f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);

  // Back up glues item 45 (30 larger): the end moves with it, and a scroll past it lands on it.
  recycler.ScrollBy(-500.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2550.0f, 0.001f, TEST_LOCATION);
  recycler.ScrollBy(10000.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 3050.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 3450.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// A correction made while an animation owns the scroller. The animation is moved onto the
// corrected coordinates: on screen the scroll continues along its original ease-out path, the
// target keeps its remaining distance, and the scroll finishes once.
int UtcDaliRecyclerViewAnimatedScrollRetargetsOnCorrectionP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollBy(-800.0f, true);
  DALI_TEST_CHECK(impl.mScrollAnimation);
  const float duration = impl.mScrollAnimation.GetDuration();
  DALI_TEST_EQUALS(duration, (200.0f + 800.0f / 4600.0f * 1000.0f) / 1000.0f, 0.0001f, TEST_LOCATION);
  // A layout pass while the animation owns the scroller leaves it alone: writing the position
  // would re-base the running interpolation.
  impl.OnArrange(LayoutRect(0.0f, 0.0f, RECYCLER_WIDTH, RECYCLER_HEIGHT));
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), -2200.0f, 0.001f, TEST_LOCATION);
  application.SendNotification();

  RenderScrollAnimation(application, recycler, false, -800.0f, duration, TEST_LOCATION);

  // Items 59..50 were glued on the way (10 x 30): the target 2200 kept its distance.
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2500.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 50u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(50u), false), 2500.0f, 0.01f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), -2500.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}

// An animation to the top keeps the top as its target through corrections (the top's offset never
// changes), and lands with item 0 at the top.
int UtcDaliRecyclerViewAnimatedScrollToTopKeepsTargetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 80.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(900.0f);
  application.SendNotification();

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollToPosition(0u, true);
  DALI_TEST_CHECK(impl.mScrollAnimation);
  application.SendNotification();
  for(uint32_t frame = 0u; frame < 200u && impl.mScrollAnimation; ++frame)
  {
    application.Render(16u);
    application.SendNotification();
  }

  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  application.Render(16u);
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), 0.0f, 0.01f, TEST_LOCATION);
  END_TEST;
}

// A correction that leaves the animation nothing to travel finishes it at once: the items above
// are so much smaller than estimated that the corrected offset is past the top, so the list is
// clamped flush, the scroller follows, and the scroll finishes exactly once.
int UtcDaliRecyclerViewAnimatedScrollFinishesAtCorrectedTopP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 5.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(400.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 8u, TEST_LOCATION);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollToPosition(0u, true);
  DALI_TEST_CHECK(impl.mScrollAnimation);
  application.SendNotification();

  // First frame: the layouter follows the scroller up, glues items 7..0 (45 smaller each) and the
  // corrected offset is clamped to the top. Nothing is left to animate.
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);

  // The stopped animation does not finish the scroll a second time.
  for(uint32_t frame = 0u; frame < 10u; ++frame)
  {
    application.Render(16u);
    application.SendNotification();
  }
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}

// The glue and the correction along the horizontal axis.
int UtcDaliRecyclerViewHorizontalScrollUpGluesItemsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount            = 100u;
  gItemExtents          = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});
  gItemExtentsAreWidths = true;

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::HORIZONTAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 60u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 63u, TEST_LOCATION);
  const View survivor = impl.FindActiveView(60u);

  // The 200 wide viewport moves left by 150: items 59 and 58 are glued (2 x 30).
  recycler.ScrollBy(-150.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2910.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 58u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 60u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(survivor, true), 3060.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(survivor, true) - recycler.GetScrollOffset(), 150.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(58u), true), 2900.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FindActiveView(58u).GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, true, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_X), -2910.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 5060.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// The animated retarget along the horizontal axis.
int UtcDaliRecyclerViewHorizontalAnimatedScrollRetargetsP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount            = 100u;
  gItemExtents          = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});
  gItemExtentsAreWidths = true;

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::HORIZONTAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();

  int finished = 0;
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });
  recycler.ScrollBy(-400.0f, true);
  const float duration = impl.mScrollAnimation.GetDuration();
  application.SendNotification();
  RenderScrollAnimation(application, recycler, true, -400.0f, duration, TEST_LOCATION);

  // Items 59..55 were glued on the way (5 x 30) and the target 2600 kept its distance.
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2750.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 55u, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, true, 0.0f, 0.0f, TEST_LOCATION);
  END_TEST;
}

// Decoration offsets and item spacing are part of every measured slot the glue walks.
int UtcDaliRecyclerViewDecoratedSpacedScrollUpGluesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 45.0f}, {20u, 80.0f}, {40u, 45.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(55.0f); // a 45 item plus decoration top 2 and bottom 8
  layouter.SetItemSpacing(4.0f);
  RecyclerView       recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&              impl     = GetRecyclerImpl(recycler);
  CountingDecoration decoration; // start 1, end 4, top 2, bottom 8
  recycler.AddItemDecoration(decoration);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3540.0f); // 60 x (55 + 4)
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 60u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(60u), false), 3542.0f, 0.001f, TEST_LOCATION);

  // Items 59..56 are glued, each slot 90 instead of 55: 3540 - 300 + 4 x 35.
  recycler.ScrollBy(-300.0f, false);
  application.SendNotification();

  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 3380.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 56u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 61u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(56u), false), 3306.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(60u), false), 3682.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FindActiveView(56u).GetProperty<float>(Actor::Property::POSITION_X), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FindActiveView(56u).GetProperty<float>(Actor::Property::SIZE_WIDTH), RECYCLER_WIDTH - 5.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 2.0f, 8.0f + 4.0f + 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.GetItemBounds(56u, RECYCLER_WIDTH).height, 90.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 6036.0f, 0.001f, TEST_LOCATION);
  recycler.RemoveItemDecoration(decoration);
  END_TEST;
}

// Size changes keep the window consistent: a re-measured item in the window moves only the items
// below it, and an item re-measured while it is glued back corrects the offsets like any other.
int UtcDaliRecyclerViewItemChangedRemeasuresP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  recycler.ScrollBy(-300.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2820.0f, 0.001f, TEST_LOCATION);

  // Item 57 grows by 20: the items below it move, the anchor and the offset do not.
  gItemExtents[57u] = 100.0f;
  adapter.NotifyItemChanged(57u);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2820.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 56u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(58u), false), 2980.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 5140.0f, 0.001f, TEST_LOCATION);

  // Item 56 leaves the window, changes while it is out (its cached 80 is stale), and is glued back
  // at 110: a correction of 30 against the cached extent.
  recycler.ScrollBy(200.0f, false);
  gItemExtents[56u] = 110.0f;
  adapter.NotifyItemChanged(56u);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 58u, TEST_LOCATION);
  const View  survivor = impl.FindActiveView(60u);
  const float before   = MainPosition(survivor, false) - recycler.GetScrollOffset();
  recycler.ScrollBy(-200.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2850.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 56u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(57u), false), 2910.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(survivor, false) - recycler.GetScrollOffset(), before + 200.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 5170.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// ScrollToPosition brings the item to the start of the viewport with its measured offset, not the
// estimate it was computed from (here every unmeasured item is estimated at 64 but is 50).
int UtcDaliRecyclerViewScrollToPositionLandsOnMeasuredItemP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter);
  auto&               impl     = GetRecyclerImpl(recycler);

  recycler.ScrollToPosition(20u, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1000.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(layouter.GetItemBounds(20u, RECYCLER_WIDTH).y, 1000.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(20u), false), 1000.0f, 0.001f, TEST_LOCATION);

  // The last item is clamped to the end of the content it has measured.
  recycler.ScrollToPosition(39u, false);
  application.SendNotification();
  DALI_TEST_EQUALS(layouter.ComputeScrollRange(), 2000.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1600.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(39u), false) + RECYCLER_ITEM_HEIGHT, 2000.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// An adapter that cannot produce a view for an item stops the fill at it in either direction.
int UtcDaliRecyclerViewMissingItemViewStopsFillN(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();

  ItemAdapter adapter = ItemAdapter::New();
  adapter.GetItemCountSignal().Connect(&TestItemCount);
  adapter.CreateViewHolderSignal().Connect(&TestCreateViewHolderExceptFive);
  adapter.BindViewHolderSignal().Connect(&TestBindViewHolder);
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(RECYCLER_ITEM_HEIGHT);
  RecyclerView recycler = RecyclerView::New();
  recycler.SetRequestedWidth(RECYCLER_WIDTH);
  recycler.SetRequestedHeight(RECYCLER_HEIGHT);
  window.Add(recycler);
  recycler.SetItemsLayouter(layouter);
  recycler.SetAdapter(adapter);
  recycler.SetCacheExtent(0.0f, 0.0f);
  application.SendNotification();
  application.SendNotification();

  // Downward from the top the fill stops before item 5.
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 4u, TEST_LOCATION);

  // Rebuilt below item 5, then nudged up by one: the glue stops at item 5.
  recycler.SetScrollOffset(300.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 6u, TEST_LOCATION);
  gBoundPositions.clear();
  recycler.ScrollBy(-1.0f, false);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 299.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 6u, TEST_LOCATION);
  DALI_TEST_CHECK(gBoundPositions.empty());
  END_TEST;
}

// A scroll animation that replaces a running one. The replaced animation reports its finish a frame
// later; that report must neither drop nor finish the newer animation, which keeps owning the scroller
// and keeps being moved onto corrected coordinates until it ends.
int UtcDaliRecyclerViewChainedAnimatedScrollKeepsRetargetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  // Three frames of a first animation (300 over 0.265 s): frames 1 and 2 glue items 59 and 58 (30
  // larger each) and move it onto the corrected coordinates; frame 3 ends at about
  // 3000 + 60 - 300 x (1 - (1 - 0.048 / 0.265)^3) = 2924.8, inside item 58 (2900..2980), so nothing
  // is glued and no correction is waiting to be rendered when the second scroll starts.
  recycler.ScrollBy(-300.0f, true);
  application.SendNotification();
  for(uint32_t frame = 0u; frame < 3u; ++frame)
  {
    application.Render(16u);
    application.SendNotification();
  }
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 58u, TEST_LOCATION);

  // The second scroll replaces the first animation and aims 300 above the synchronised offset.
  recycler.ScrollBy(-300.0f, true);
  const float base     = recycler.GetScrollOffset();
  const float duration = impl.mScrollAnimation.GetDuration();
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), base - 300.0f, 0.01f, TEST_LOCATION);
  application.SendNotification();

  // Its first frame glues item 57 and moves it onto the corrected coordinates (target + 30). The finish
  // of the replaced animation arrives in the same frame and leaves the newer one running.
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_CHECK(impl.mScrollAnimation);
  DALI_TEST_CHECK(recycler.IsScrolling());
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), base - 270.0f, 0.01f, TEST_LOCATION);
  // It still owns the scroller: a layout pass leaves the scroller alone.
  impl.OnArrange(LayoutRect(0.0f, 0.0f, RECYCLER_WIDTH, RECYCLER_HEIGHT));
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), base - 270.0f, 0.01f, TEST_LOCATION);

  RenderScrollAnimation(application, recycler, false, -300.0f, duration, TEST_LOCATION, 1u);

  // Items 57..54 were glued on the way (4 x 30): base - 300 + 120. Items 40..53 are still estimated,
  // so item 54 starts at 40 x 50 + 14 x 50 = 2700 and holds the top; item 59 (3100..3180) the bottom.
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), base - 180.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 54u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 59u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(54u), false), 2700.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), 180.0f - base, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}

// ScrollToPosition while a running animation is being moved onto corrected coordinates. The scroll it
// starts must aim from the offset its own synchronisation produced: synchronising again would read the
// scroller before it has rendered that correction, and undo it.
int UtcDaliRecyclerViewScrollToPositionDuringCorrectionTargetsItemP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  // Frame 1 of an 800 scroll ends near 2901.6 and glues items 59 and 58 (30 larger each).
  recycler.ScrollBy(-800.0f, true);
  application.SendNotification();
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 58u, TEST_LOCATION);

  // Frame 2 ends near 2871.7, above item 58 (2900): ScrollToPosition is the first to synchronise with
  // it, glues item 57 (30 larger) and aims at item 55 from there. Items 40..54 are still estimated, so
  // item 55 starts at 40 x 50 + 15 x 50 = 2750.
  application.Render(16u);
  const float physical = -RenderedMainPosition(impl.mScroller, false);
  recycler.ScrollToPosition(55u, true);
  DALI_TEST_CHECK(impl.mScrollAnimation);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 57u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), physical + 30.0f, 0.01f, TEST_LOCATION);
  const float itemStart = layouter.GetItemBounds(55u, RECYCLER_WIDTH).y;
  DALI_TEST_EQUALS(itemStart, 2750.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), impl.ClampScrollOffset(itemStart), 0.01f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);

  application.SendNotification();
  for(uint32_t frame = 0u; frame < 200u && impl.mScrollAnimation; ++frame)
  {
    application.Render(16u);
    application.SendNotification();
  }
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  END_TEST;
}

// A late frame can take a scroll to the top all the way in one step. The items measured on that frame
// correct the offset after the animation has ended; the scroll still ends with item 0 at the top.
int UtcDaliRecyclerViewStalledScrollToTopEndsAtTopP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 80.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f); // drops every measurement: items 0..29 are estimated again
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(400.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 8u, TEST_LOCATION);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollToPosition(0u, true);
  const float duration = impl.mScrollAnimation.GetDuration();
  application.SendNotification();

  // Frames 1 and 2 glue items 7 and 6 (30 larger each). The target stays the top; the distance to it
  // grew by 30, and the time scaled with it is capped at the animation's own duration.
  for(uint32_t frame = 0u; frame < 2u; ++frame)
  {
    application.Render(16u);
    application.SendNotification();
    DALI_TEST_CHECK(impl.mScrollAnimation);
    DALI_TEST_EQUALS(impl.mScrollAnimation.GetDuration(), duration, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 6u, TEST_LOCATION);

  // A 400 ms frame ends the animation at 0. Items 5..0 are glued on it (6 x 30), which leaves the
  // layouter at 180 with no time left to animate: the scroll to the top finishes flush with it.
  application.Render(400u);
  application.SendNotification();
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}

// A scroll to the top that measures items smaller than estimated: the top comes nearer, and the time
// left shrinks in proportion, so that the ease-out keeps the speed it had.
int UtcDaliRecyclerViewAnimatedScrollToTopKeepsSpeedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 60u;
  gItemExtents = ExtentRuns({{30u, 30.0f}, {30u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetScrollOffset(2000.0f);
  layouter.SetItemExtent(50.0f); // drops every measurement: items 0..29 are estimated again
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(400.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 8u, TEST_LOCATION);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollToPosition(0u, true);
  const float duration = impl.mScrollAnimation.GetDuration();
  application.SendNotification();

  // Frame 1 ends near 350.6: items 7 and 6 are glued (20 smaller each), so the offset is 40 nearer the
  // top than the scroller. Time left before: duration - 0.016 for `physical`; after: scaled by
  // (physical - 40) / physical. The new ease-out starts at 3 x (physical - 40) / that time, which is
  // 3 x physical / (duration - 0.016): the speed the replaced one had.
  application.Render(16u);
  const float physical = -RenderedMainPosition(impl.mScroller, false);
  application.SendNotification();
  DALI_TEST_CHECK(impl.mScrollAnimation);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), physical - 40.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScrollAnimation.GetDuration(), (duration - 0.016f) * (physical - 40.0f) / physical, 0.0001f, TEST_LOCATION);

  for(uint32_t frame = 0u; frame < 200u && impl.mScrollAnimation; ++frame)
  {
    application.Render(16u);
    application.SendNotification();
  }
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(0u), false), 0.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  END_TEST;
}

// Making an item visible without animation aims again once the scroll has measured the items it
// brought in, upward (larger items glued above the window) and downward (larger items filled below).
int UtcDaliRecyclerViewMakeItemVisibleAimsAtMeasuredItemP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{40u, 50.0f}, {20u, 80.0f}, {40u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  recycler.SetScrollOffset(3000.0f);
  application.SendNotification();

  // Up to item 56, estimated at 2800: the scroll of 200 glues 59..57 (+90, offset 2890) and item 56
  // is still above the viewport. Two aims glue it (+30, offset 2830) and reach its measured start.
  gBoundPositions.clear();
  gRecycledViews = 0;
  impl.ScrollToItemMakeVisible(56u, false);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2800.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 56u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 61u, TEST_LOCATION);
  const std::vector<uint32_t> glued{59u, 58u, 57u, 56u};
  DALI_TEST_CHECK(gBoundPositions == glued);
  DALI_TEST_EQUALS(gRecycledViews, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(56u), false), 2800.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(60u), false), 3120.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), -2800.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);

  // Down to item 45, estimated at 2250..2300: the scroll to 1900 fills 40..43 (80 each) and item 45
  // is still below the viewport. The aims fill 44 and 45 (offset 2020), then reach its measured end
  // 40 x 50 + 6 x 80 = 2480 at the bottom of the viewport: offset 2080.
  recycler.SetScrollOffset(1600.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 32u, TEST_LOCATION);
  gBoundPositions.clear();
  gRecycledViews = 0;
  impl.ScrollToItemMakeVisible(45u, false);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 2080.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 41u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 45u, TEST_LOCATION);
  const std::vector<uint32_t> filled{40u, 41u, 42u, 43u, 44u, 45u};
  DALI_TEST_CHECK(gBoundPositions == filled);
  DALI_TEST_EQUALS(gRecycledViews, 9, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(45u), false) + 80.0f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), -2080.0f, 0.001f, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);
  END_TEST;
}

// A structural data change lays the list out again from the top. A running scroll animation would
// keep the scroller where it was going, so the change cancels it; the scroll still finishes once.
int UtcDaliRecyclerViewStructuralChangeCancelsScrollAnimationP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemExtents = ExtentRuns({{41u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollBy(500.0f, true);
  application.SendNotification();
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_CHECK(impl.mScrollAnimation);
  DALI_TEST_CHECK(recycler.GetScrollOffset() > 0.0f);

  // An insert restarts the layout at offset 0: the animation is cancelled and the scroller is written
  // there at once.
  gItemCount = 41u;
  adapter.NotifyItemInserted(0u, 1u);
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 0, TEST_LOCATION);
  CheckRealisedWindow(recycler, layouter, false, 0.0f, 0.0f, TEST_LOCATION);

  // The next frame renders the top, and the cancelled animation's finish ends the scroll once.
  application.SendNotification();
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  END_TEST;
}

// An animated scroll to the end over rows larger than the estimate (300 rows of 80, the default estimate
// 64). The end known at the start is 9 x 80 + 291 x 64 - 400 = 18944 (rows 0..8 measured); it moves out
// while the rows near it are measured. The animation follows the end and reaches the measured end in one
// go, 300 x 80 - 400.
int UtcDaliRecyclerViewAnimatedScrollToEndReachesMeasuredEndP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 300u;
  gItemExtents = ExtentRuns({{300u, 80.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&               impl     = GetRecyclerImpl(recycler);
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 18944.0f, 0.001f, TEST_LOCATION);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollBy(100000.0f, true);
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 18944.0f, 0.001f, TEST_LOCATION);
  application.SendNotification();
  RenderScrollToEnd(application, recycler, TEST_LOCATION);

  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 23600.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 23600.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 299u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(299u), false) + 80.0f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.01f, TEST_LOCATION);
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), -23600.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}

// The same over rows smaller than the estimate (100 rows of 50, estimate 64). The end known at the start
// is 14 x 50 + 86 x 64 - 400 = 5804 (rows 0..13 measured); it moves in while the rows near it are
// measured. The animation follows it and stops at 100 x 50 - 400 = 4600 without ever showing space past
// the last row.
int UtcDaliRecyclerViewAnimatedScrollToEndStopsAtShorterContentP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{100u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&               impl     = GetRecyclerImpl(recycler);
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 5804.0f, 0.001f, TEST_LOCATION);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollBy(100000.0f, true);
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 5804.0f, 0.001f, TEST_LOCATION);
  application.SendNotification();
  RenderScrollToEnd(application, recycler, TEST_LOCATION);

  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!impl.mScrollAnimation);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 4600.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 4600.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 99u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(99u), false) + 50.0f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.01f, TEST_LOCATION);
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), -4600.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}

// A late first frame takes a scroll to the end all the way to the end known at its start (18944). The
// rows measured there move the end out after the animation has ended: the scroll glides on to the new
// end and finishes once.
int UtcDaliRecyclerViewStalledScrollToEndGlidesOnP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 300u;
  gItemExtents = ExtentRuns({{300u, 80.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&               impl     = GetRecyclerImpl(recycler);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollBy(100000.0f, true);
  application.SendNotification();

  // The 2 s frame ends the animation at 18944. The window rebuilt there starts at row 289 (at
  // 9 x 80 + 280 x 64 = 18640) and measures rows 289..299 at 80: the end is now
  // 9 x 80 + 280 x 64 + 11 x 80 - 400 = 19120, and a new animation glides on to it.
  application.Render(2000u);
  application.SendNotification();
  DALI_TEST_CHECK(impl.mScrollAnimation);
  DALI_TEST_CHECK(recycler.IsScrolling());
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 18944.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 289u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 19120.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 19120.0f, 0.01f, TEST_LOCATION);

  RenderScrollToEnd(application, recycler, TEST_LOCATION);

  // Row 299 starts at 18640 + 10 x 80 = 19440 and ends at the bottom of the viewport.
  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 19120.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 19120.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(299u), false), 19440.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(299u), false) + 80.0f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.01f, TEST_LOCATION);
  END_TEST;
}

// Making an item longer than the viewport visible aims at one edge only: its end when it lies below the
// viewport, its start when it lies above. Re-aiming at whichever edge is out of view would alternate.
int UtcDaliRecyclerViewMakeTallItemVisibleAlignsOneEdgeP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemExtents = ExtentRuns({{20u, 50.0f}, {1u, 600.0f}, {19u, 50.0f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  layouter.SetItemExtent(50.0f);
  RecyclerView recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&        impl     = GetRecyclerImpl(recycler);
  recycler.SetCacheExtent(0.0f, 0.0f);
  application.SendNotification();

  // Down to item 20, estimated at 1000..1050: the scroll to 650 measures it at 600 (1000..1600) and one
  // aim brings its end to the bottom of the viewport: 1600 - 400.
  impl.ScrollToItemMakeVisible(20u, false);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(20u), false), 1000.0f, 0.001f, TEST_LOCATION);

  // Up to it from below (rows 29..36 at 2000): its start goes to the top of the viewport and stays there.
  recycler.SetScrollOffset(2000.0f);
  application.SendNotification();
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 29u, TEST_LOCATION);
  impl.ScrollToItemMakeVisible(20u, false);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 1000.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetFirstVisiblePosition(), 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(20u), false), 1000.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), -1000.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// An animated scroll to the end over rows a quarter pixel larger than the estimate (100 rows of 64.25, the
// default estimate 64; every offset below is a multiple of 0.25 under 8192, exact in float). The first
// layout realises rows 0..10 (row 10 starts at 642.5 < 700), so the end known at the start is
// 11 x 64.25 + 89 x 64 - 400 = 6002.75. Each row measured on the way moves the end by 0.25, often by less
// than 0.5 within a frame: the animation follows the distance between the moving end and its target, and
// ends at 100 x 64.25 - 400 = 6025. Following only the movement within each frame ended 3.75 short.
int UtcDaliRecyclerViewAnimatedScrollToEndFollowsSmallDriftP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  ResetCounters();
  gItemCount   = 100u;
  gItemExtents = ExtentRuns({{100u, 64.25f}});

  ItemAdapter         adapter  = ItemAdapter::New();
  LinearItemsLayouter layouter = LinearItemsLayouter::New(LinearItemsLayouter::Orientation::VERTICAL);
  RecyclerView        recycler = BuildRecycler(application, window, adapter, layouter, true);
  auto&               impl     = GetRecyclerImpl(recycler);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 6002.75f, 0.001f, TEST_LOCATION);

  int started  = 0;
  int finished = 0;
  recycler.ScrollStartedSignal().Connect(&application, [&started](RecyclerView)
  { ++started; });
  recycler.ScrollFinishedSignal().Connect(&application, [&finished](RecyclerView)
  { ++finished; });

  recycler.ScrollBy(100000.0f, true);
  DALI_TEST_EQUALS(-impl.mScroller.GetProperty<float>(Actor::Property::POSITION_Y), 6002.75f, 0.001f, TEST_LOCATION);
  application.SendNotification();
  RenderScrollToEnd(application, recycler, TEST_LOCATION);

  DALI_TEST_EQUALS(started, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!recycler.IsScrolling());
  DALI_TEST_EQUALS(impl.GetMaxScrollOffset(), 6025.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(std::abs(recycler.GetScrollOffset() - impl.GetMaxScrollOffset()) < 0.5f);
  DALI_TEST_EQUALS(recycler.GetScrollOffset(), 6025.0f, 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(recycler.GetLastVisiblePosition(), 99u, TEST_LOCATION);
  DALI_TEST_EQUALS(MainPosition(impl.FindActiveView(99u), false) + 64.25f, recycler.GetScrollOffset() + RECYCLER_HEIGHT, 0.5f, TEST_LOCATION);
  application.Render(16u);
  application.SendNotification();
  DALI_TEST_EQUALS(RenderedMainPosition(impl.mScroller, false), -recycler.GetScrollOffset(), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(finished, 1, TEST_LOCATION);
  END_TEST;
}
