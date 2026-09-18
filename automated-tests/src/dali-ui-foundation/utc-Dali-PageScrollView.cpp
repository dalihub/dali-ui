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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <stdlib.h>
#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_page_scroll_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_page_scroll_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// PageScrollView had no automated coverage. These tests pin the page-notification
// contract of OnScrollableAreaChanged and OnScrollFinished: what the view reports
// while a snap is in flight, when a viewport resize changes the page count, and
// when a page is selected before the first layout pass has any real dimensions.

namespace
{
const float PAGER_WIDTH   = 100.0f;
const float PAGER_HEIGHT  = 80.0f;
const float CONTENT_WIDTH = 500.0f;

/// Records every (page, count) pair PageChangedSignal reports, in order.
struct PageSignalRecorder : public ConnectionTracker
{
  void OnPageChanged(int page, int count)
  {
    signals.push_back(std::make_pair(page, count));
  }

  std::vector<std::pair<int, int>> signals;
};

/// A pager 100 long x 80 across over 500 units of content along `direction`: five pages
/// of 100. The content is NOT yet on the scene, so no layout pass has run.
PageScrollView BuildPagerAlong(ScrollDirection direction, View& content)
{
  const bool vertical = (direction == ScrollDirection::Vertical);

  PageScrollView pager = PageScrollView::New();
  pager.SetScrollDirection(direction);
  pager.SetRequestedWidth(vertical ? PAGER_HEIGHT : PAGER_WIDTH);
  pager.SetRequestedHeight(vertical ? PAGER_WIDTH : PAGER_HEIGHT);

  content = View::New();
  content.SetRequestedWidth(vertical ? PAGER_HEIGHT : CONTENT_WIDTH);
  content.SetRequestedHeight(vertical ? CONTENT_WIDTH : PAGER_HEIGHT);
  pager.SetContent(content);

  return pager;
}

/// The horizontal pager the tests written before the vertical axis was covered use.
PageScrollView BuildPager(View& content)
{
  return BuildPagerAlong(ScrollDirection::Horizontal, content);
}

/// Resizes the pager along its scroll axis: that is what changes the page length.
void ResizeAlong(PageScrollView pager, bool vertical, float length)
{
  if(vertical)
  {
    pager.SetRequestedHeight(length);
  }
  else
  {
    pager.SetRequestedWidth(length);
  }
}

/// The content actor position along the scroll axis, i.e. minus the scroll offset. This is
/// the EVENT-side value, which is what ApplyScrollPosition writes when a scroll settles.
float ContentPositionAlong(View content, bool vertical)
{
  return content.GetProperty<float>(vertical ? Actor::Property::POSITION_Y : Actor::Property::POSITION_X);
}

/// The same position as the SCENE GRAPH currently holds it. AnimateTo sets the event-side
/// value to its target the moment it plays, so only this one moves during an animation.
float CurrentContentPositionAlong(View content, bool vertical)
{
  return content.GetCurrentProperty<float>(vertical ? Actor::Property::POSITION_Y : Actor::Property::POSITION_X);
}

// A viewport resize can leave a FRACTIONAL maximum scroll offset. An animation that was
// started before the resize keeps its original endpoint, which is now outside the range,
// so the settled position is repaired - and the repair must apply the clamped FLOAT,
// exactly as the non-animated ScrollTo applies its clamped target. Rounding the clamp
// first parks the content one unit past the maximum (376 rather than 375.6), a position
// the view can never be scrolled to and which no later pass corrects.
void CheckFractionalRangeRepair(ScrollDirection direction)
{
  UiTestApplication application;
  Window            window   = application.GetWindow();
  const bool        vertical = (direction == ScrollDirection::Vertical);

  View           content;
  PageScrollView pager = BuildPagerAlong(direction, content);
  window.Add(pager);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  // Snap to the last page: the animation targets the scroll offset 400.
  pager.ScrollToPage(4, true);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(pager.IsScrolling());

  // The page length becomes 124.4, so the count stays ceil(500 / 124.4) == 5 and nothing
  // is reported, but the maximum scroll offset is now the fractional 500 - 124.4.
  ResizeAlong(pager, vertical, 124.4f);
  application.SendNotification();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 0, TEST_LOCATION);

  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.SendNotification();

  const Vector2 settled = pager.GetScrollPosition();

  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(vertical ? settled.y : settled.x, 375.6f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical ? settled.x : settled.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -375.6f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  // Exactly one notification, from the snap that settles on the page it was heading to.
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);
}

// A count shrink during a snap whose LAST PAGE IS PARTIAL: the page the scroll is heading
// to (the snap target clamped to the new count) is not the page nearest the position it
// will settle on. Reporting the position-derived page announces a page the view then
// leaves, and the settled notification has to correct it. The transient report must name
// the page OnScrollFinished will commit, so the whole resize reports exactly once.
void CheckPartialPageCountShrinkDuringSnap(ScrollDirection direction)
{
  UiTestApplication application;
  Window            window   = application.GetWindow();
  const bool        vertical = (direction == ScrollDirection::Vertical);

  View           content;
  PageScrollView pager = BuildPagerAlong(direction, content);
  window.Add(pager);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  pager.ScrollToPage(4, true);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(100);
  application.SendNotification();

  // About 116 ms into a 300 ms snap. The SCENE-GRAPH position has moved off zero, so the
  // property notification that rewrites mScrollPosition to the live animated position has
  // been delivered: the resize below is answered from a live position, not from the
  // animation target that the event-side property already holds.
  DALI_TEST_CHECK(pager.IsScrolling());
  DALI_TEST_CHECK(CurrentContentPositionAlong(content, vertical) < -1.0f);

  // The page length becomes 160: the count is ceil(500 / 160) == 4 and the last page is
  // partial. The snap is heading to page 3 (target 4 clamped), while the offset it will
  // settle on, 500 - 160 == 340, is nearest page 2.
  ResizeAlong(pager, vertical, 160.0f);
  application.SendNotification();

  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 4, TEST_LOCATION);

  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.SendNotification();

  const Vector2 settled = pager.GetScrollPosition();

  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(vertical ? settled.y : settled.x, 340.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical ? settled.x : settled.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -340.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);

  // The settled commit finds the page already reported, so it stays silent.
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
}

} // namespace

// A shrinking page count while a snap animation is in flight must report the page the
// scroll is heading to, not the stale settled page: while a snap is in flight that is the
// snap target clamped to the new count, which is what OnScrollFinished commits. That
// commit then finds the same page and stays silent, so the whole resize reports once.
int UtcDaliPageScrollViewCountShrinkDuringSnapP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View           content;
  PageScrollView pager = BuildPager(content);
  window.Add(pager);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  // Snap to the last page (scroll position 400, the clamped maximum).
  pager.ScrollToPage(4, true);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(pager.IsScrolling());

  // The viewport grows to 250, so the page length becomes 250 and the count 2. The
  // clamped animation target is 250, i.e. page 1.
  pager.SetRequestedWidth(250.0f);
  application.SendNotification();

  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 2, TEST_LOCATION);

  // Let the snap animation finish. Its endpoint is outside the shrunk range, so the
  // settled position is repaired, and the snap target clamps onto the page already
  // reported - no second notification.
  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.SendNotification();

  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetPageCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 250.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  END_TEST;
}

// A viewport-only resize reaches the page view at all: a settled pager whose own width
// changes re-derives its count and page and reports them once.
int UtcDaliPageScrollViewViewportResizeNotifiesP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View           content;
  PageScrollView pager = BuildPager(content);
  window.Add(pager);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  pager.SetRequestedWidth(250.0f);
  application.SendNotification();

  DALI_TEST_EQUALS(pager.GetPageCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 2, TEST_LOCATION);
  END_TEST;
}

// A page selected before the first layout pass can only be recorded: the page length is
// still 0, so the physical scroll position cannot be resolved. The first pass that has
// real dimensions must apply it AND keep it, rather than re-deriving page 0 from the
// scroll position that was never moved.
int UtcDaliPageScrollViewPreLayoutSelectionAppliedP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View           content;
  PageScrollView pager = BuildPager(content);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  // Pre-layout the count reads as 1, so three insertions make four pages and shift the
  // current page to 3. Nothing can move yet.
  pager.NotifyPagesInserted(0, 3);
  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 0.0f, 0.001f, TEST_LOCATION);

  window.Add(pager);
  application.SendNotification();
  application.Render();

  // Page 3 of the five real pages: scroll position 300, and the selection survived the
  // layout-derived count change that is reported alongside it.
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 300.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_CHECK(!recorder.signals.empty());
  DALI_TEST_EQUALS(recorder.signals.back().first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().second, 5, TEST_LOCATION);
  END_TEST;
}

// F1 regression, both axes: the fractional-range repair applies the clamped float.
int UtcDaliPageScrollViewFractionalRangeRepairHorizontalP(void)
{
  CheckFractionalRangeRepair(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewFractionalRangeRepairVerticalP(void)
{
  CheckFractionalRangeRepair(ScrollDirection::Vertical);
  END_TEST;
}

// F2 regression, both axes: the transient page during a snap is the clamped snap target.
int UtcDaliPageScrollViewPartialPageCountShrinkDuringSnapHorizontalP(void)
{
  CheckPartialPageCountShrinkDuringSnap(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewPartialPageCountShrinkDuringSnapVerticalP(void)
{
  CheckPartialPageCountShrinkDuringSnap(ScrollDirection::Vertical);
  END_TEST;
}

// An animated ScrollToPage whose target is where the view already rests starts no scroll:
// ScrollToWithDuration finishes synchronously, and the ScrollFinished it sends is a no-op
// because nothing was scrolling. The snap target it recorded would then never be cleared,
// and a later count change would report the page that dead target names rather than the page
// the view is actually showing. The target must be dropped when no scroll started.
int UtcDaliPageScrollViewAnimatedScrollToCurrentPageClearsSnapTargetP(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();

  View           content;
  PageScrollView pager = BuildPager(content);
  window.Add(pager);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  // An instant scroll to page 2 settles at offset 200 and commits the page from the
  // settled position, so it reports (2, 5) once.
  pager.ScrollToPage(2, false);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);

  // The animated request targets the offset the view already rests on, so no animation is
  // created and nothing is in flight.
  pager.ScrollToPage(2, true);
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);

  // The page length becomes 160: the count is ceil(500 / 160) == 4 and the offset 200 is
  // nearest page round(200 / 160) == 1. With a stale snap target the view would announce
  // page 2 - a page it is not showing and will never scroll to.
  ResizeAlong(pager, false, 160.0f);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().second, 4, TEST_LOCATION);
  END_TEST;
}
