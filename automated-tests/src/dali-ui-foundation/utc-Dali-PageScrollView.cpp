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

// ─── Viewport clamps, pending selections and empty layouts ──────────────────────────
// Each test below derives its expected values and states the earlier behaviour it guards
// against ("fails at 3d2c266e because ..."), or, for a guard or coverage keeper that also
// passes there, what it catches or executes.

namespace
{
/// Resizes the CONTENT along the scroll axis: that changes the page count, not the page
/// length.
void ResizeContentAlong(View content, bool vertical, float length)
{
  if(vertical)
  {
    content.SetRequestedHeight(length);
  }
  else
  {
    content.SetRequestedWidth(length);
  }
}

/// Lets a 300 ms snap or scroll started just before settle completely.
void SettleScroll(UiTestApplication& application)
{
  application.SendNotification();
  application.Render(16);
  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.SendNotification();
}

// A layout that clamps a settled pager moves the content without any animation. The next
// snap must start from that applied position, not from the one cached before the clamp.
//
// Pager 100 over 500 (five pages of 100). ScrollToPage(4, false) rests at offset 400. A
// 250 viewport leaves a range of 500 - 250 = 250, so the layout clamps the offset (and the
// content) to 250. Back at 100 the range is 400 again and the offset stays at 250, which is
// page round(250 / 100) = 3. ScrollToPage(4, true) then has 150 units to travel: it must
// animate and settle at 400 on page 4.
// Fails at 3d2c266e because the clamp left the cached content position at -400: the snap
// compared its target -400 with that cache, finished as a no-op and never scrolled.
void CheckSnapAfterViewportClamp(ScrollDirection direction)
{
  UiTestApplication application;
  Window            window   = application.GetWindow();
  const bool        vertical = (direction == ScrollDirection::Vertical);

  View           content;
  PageScrollView pager = BuildPagerAlong(direction, content);
  window.Add(pager);
  application.SendNotification();
  application.Render();

  pager.ScrollToPage(4, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -400.0f, 0.001f, TEST_LOCATION);

  ResizeAlong(pager, vertical, 250.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -250.0f, 0.001f, TEST_LOCATION);

  ResizeAlong(pager, vertical, 100.0f);
  application.SendNotification();
  application.Render();
  const Vector2 clamped = pager.GetScrollPosition();
  DALI_TEST_EQUALS(vertical ? clamped.y : clamped.x, 250.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);

  pager.ScrollToPage(4, true);
  DALI_TEST_CHECK(pager.IsScrolling());
  SettleScroll(application);

  const Vector2 settled = pager.GetScrollPosition();
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(vertical ? settled.y : settled.x, 400.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical ? settled.x : settled.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -400.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(CurrentContentPositionAlong(content, vertical), -400.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
}

// A NotifyPages* selection stays pending until a layout pass with a NEW geometry or a
// settled scroll releases it: post-layout the notify method already set the extent of the
// count it announced, so the pass that follows finds nothing changed and never reaches
// OnScrollableAreaChanged. A scroll started in that window is a newer intent than the
// recorded selection. When a count change arrives while that scroll is in flight, the
// pager must report the page the scroll owns - the clamped snap target for a snap, the
// live position for a free scroll - and not the page the stale selection recorded.
//
// `removed` picks the notify that leaves the selection pending. An insert appends a sixth
// page (extent 600, (0, 6) reported) and a snap heads for page 4 while the content grows,
// unannounced, to 650 = seven pages: the transient report is the snap target, (4, 7), and
// the settle commits page 4 again silently (two signals in total). A removal drops the
// fifth page (extent 400, (0, 4) reported) and a 300 ms free scroll heads for offset 300
// (page 3) while the content grows to 450 = five pages. 116 ms in, EASE_OUT gives
// 1 - (1 - 116 / 300)^3 = 0.769 of the way, i.e. offset 230.8, page round(2.308) = 2: the
// transient report is (2, 5), and the settle at 300 reports (3, 5) (three signals).
// Fails at 3d2c266e because the pending selection blocked the re-derivation although the
// scroll had kept it from being applied: the count change reported page 0.
void CheckPendingSelectionYieldsToScrollInFlight(ScrollDirection direction, bool snap, bool removed)
{
  UiTestApplication application;
  Window            window   = application.GetWindow();
  const bool        vertical = (direction == ScrollDirection::Vertical);

  const int   notifiedCount     = removed ? 4 : 6;
  const float notifiedExtent    = removed ? 400.0f : 600.0f;
  const int   destinationPage   = removed ? 3 : 4;
  const float destinationOffset = removed ? 300.0f : 400.0f;
  const float grownExtent       = removed ? 450.0f : 650.0f;
  const int   grownCount        = removed ? 5 : 7;

  View           content;
  PageScrollView pager = BuildPagerAlong(direction, content);
  window.Add(pager);

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 0, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  // The content takes the extent of the new count and the change is announced.
  ResizeContentAlong(content, vertical, notifiedExtent);
  if(removed)
  {
    pager.NotifyPagesRemoved(4, 1);
  }
  else
  {
    pager.NotifyPagesInserted(5, 1);
  }
  DALI_TEST_EQUALS(pager.GetPageCount(), notifiedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, notifiedCount, TEST_LOCATION);

  // The pass that follows measures exactly the extent the notify already set, so it
  // reports nothing and leaves the selection pending.
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), notifiedCount, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);

  if(snap)
  {
    pager.ScrollToPage(destinationPage, true);
  }
  else
  {
    pager.SetMinimumFlingDuration(300);
    pager.SetMaximumFlingDuration(300);
    pager.ScrollTo(vertical ? Vector2(0.0f, destinationOffset) : Vector2(destinationOffset, 0.0f), true);
  }
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(100);
  application.SendNotification();

  // Mid-flight, with the live position delivered: strictly between page 0 and the
  // destination, so the page it names differs from both the stale selection and the
  // destination.
  DALI_TEST_CHECK(pager.IsScrolling());
  const Vector2 live     = pager.GetScrollPosition();
  const float   liveAxis = vertical ? live.y : live.x;
  const int     livePage = static_cast<int>(liveAxis / PAGER_WIDTH + 0.5f);
  DALI_TEST_CHECK(livePage > 0 && livePage < destinationPage);

  ResizeContentAlong(content, vertical, grownExtent);
  application.SendNotification();

  const int expectedTransient = snap ? destinationPage : livePage;
  DALI_TEST_EQUALS(pager.GetPageCount(), grownCount, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].first, expectedTransient, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].second, grownCount, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), expectedTransient, TEST_LOCATION);

  application.Render(100);
  application.Render(100);
  application.Render(100);
  application.SendNotification();

  const Vector2 settled = pager.GetScrollPosition();

  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(vertical ? settled.y : settled.x, destinationOffset, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical ? settled.x : settled.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), destinationPage, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), grownCount, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), snap ? 2 : 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().first, destinationPage, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().second, grownCount, TEST_LOCATION);
}

// Once a direction has been measured, an empty layout is real (empty) content and is
// reported like any other count change, with or without a NotifyPages* count pending.
//
// Pager 100 over 500 (five pages, (0, 5) already reported by the first layout). Content 0:
// count 0, page -1, offset 0, one (-1, 0). A cross-axis resize repeats the callback for the
// same empty geometry and reports nothing. Content 500 reports (0, 5); content 0 again
// (-1, 0); content 300 reports (0, 3).
// Fails at 3d2c266e because a zero primary extent returned before anything was compared,
// so no change from or to the empty state was ever reported.
void CheckEmptyContentWithoutNotify(ScrollDirection direction)
{
  UiTestApplication application;
  const bool        vertical = (direction == ScrollDirection::Vertical);

  View           content;
  PageScrollView pager = BuildPagerAlong(direction, content);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  int         signalCount       = 0;
  const float restoredExtents[] = {500.0f, 300.0f};
  for(float extent : restoredExtents)
  {
    ResizeContentAlong(content, vertical, 0.0f);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(pager.GetPageCount(), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(pager.GetCurrentPage(), -1, TEST_LOCATION);
    DALI_TEST_EQUALS(pager.GetScrollPosition(), Vector2::ZERO, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), 0.0f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), ++signalCount, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.signals.back().first, -1, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.signals.back().second, 0, TEST_LOCATION);

    // A cross-axis viewport change produces another callback for the same page geometry.
    if(vertical)
    {
      pager.SetRequestedWidth(PAGER_HEIGHT + signalCount);
    }
    else
    {
      pager.SetRequestedHeight(PAGER_HEIGHT + signalCount);
    }
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), signalCount, TEST_LOCATION);

    ResizeContentAlong(content, vertical, extent);
    application.SendNotification();
    application.Render();
    const int count = static_cast<int>(extent / PAGER_WIDTH);
    DALI_TEST_EQUALS(pager.GetPageCount(), count, TEST_LOCATION);
    DALI_TEST_EQUALS(pager.GetCurrentPage(), 0, TEST_LOCATION);
    DALI_TEST_EQUALS(pager.GetScrollPosition(), Vector2::ZERO, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), ++signalCount, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.signals.back().first, 0, TEST_LOCATION);
    DALI_TEST_EQUALS(recorder.signals.back().second, count, TEST_LOCATION);

    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), signalCount, TEST_LOCATION);
  }
}

// An empty layout also releases a NotifyPages* count left pending across an unchanged pass.
//
// Pager 100 over 500 at page 4 (offset 400). The content grows to 600 and a sixth page is
// announced: (4, 6), and the next pass measures the notified extent, so the count 6 stays
// pending. The content then becomes empty: count 0, page -1, offset 0, (-1, 0). Announcing
// one page over a 100 extent reports (0, 1); emptying the content again before the layout
// measures it reports (-1, 0).
// Fails at 3d2c266e because the empty layout returned before the override was released:
// the count stayed 6 on page 4 and nothing was reported.
void CheckPendingCountClearsOnEmptyContent(ScrollDirection direction)
{
  UiTestApplication application;
  Window            window   = application.GetWindow();
  const bool        vertical = (direction == ScrollDirection::Vertical);

  View           content;
  PageScrollView pager = BuildPagerAlong(direction, content);
  window.Add(pager);
  application.SendNotification();
  application.Render();
  pager.ScrollToPage(4, false);
  application.SendNotification();
  application.Render();

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  ResizeContentAlong(content, vertical, 600.0f);
  pager.NotifyPagesInserted(5, 1);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), 6, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 6, TEST_LOCATION);

  ResizeContentAlong(content, vertical, 0.0f);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition(), Vector2::ZERO, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().first, -1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().second, 0, TEST_LOCATION);

  // The notify sets the extent to 100, but the content is emptied again before the layout
  // measures it.
  ResizeContentAlong(content, vertical, 100.0f);
  pager.NotifyPagesInserted(0, 1);
  DALI_TEST_EQUALS(pager.GetPageCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().first, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().second, 1, TEST_LOCATION);

  ResizeContentAlong(content, vertical, 0.0f);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition(), Vector2::ZERO, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().first, -1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals.back().second, 0, TEST_LOCATION);
}

} // namespace

int UtcDaliPageScrollViewSnapAfterViewportClampHorizontalP(void)
{
  CheckSnapAfterViewportClamp(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewSnapAfterViewportClampVerticalP(void)
{
  CheckSnapAfterViewportClamp(ScrollDirection::Vertical);
  END_TEST;
}

int UtcDaliPageScrollViewPendingSelectionYieldsToSnapHorizontalP(void)
{
  CheckPendingSelectionYieldsToScrollInFlight(ScrollDirection::Horizontal, true, false);
  END_TEST;
}

int UtcDaliPageScrollViewPendingSelectionYieldsToFreeScrollHorizontalP(void)
{
  CheckPendingSelectionYieldsToScrollInFlight(ScrollDirection::Horizontal, false, true);
  END_TEST;
}

int UtcDaliPageScrollViewPendingSelectionYieldsToSnapVerticalP(void)
{
  CheckPendingSelectionYieldsToScrollInFlight(ScrollDirection::Vertical, true, false);
  END_TEST;
}

int UtcDaliPageScrollViewPendingSelectionYieldsToFreeScrollVerticalP(void)
{
  CheckPendingSelectionYieldsToScrollInFlight(ScrollDirection::Vertical, false, true);
  END_TEST;
}

int UtcDaliPageScrollViewEmptyContentWithoutNotifyHorizontalP(void)
{
  CheckEmptyContentWithoutNotify(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewEmptyContentWithoutNotifyVerticalP(void)
{
  CheckEmptyContentWithoutNotify(ScrollDirection::Vertical);
  END_TEST;
}

int UtcDaliPageScrollViewPendingCountClearsOnEmptyContentHorizontalP(void)
{
  CheckPendingCountClearsOnEmptyContent(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewPendingCountClearsOnEmptyContentVerticalP(void)
{
  CheckPendingCountClearsOnEmptyContent(ScrollDirection::Vertical);
  END_TEST;
}

// Guard: keeping the clamped position as a float matters near the no-op threshold.
//
// Pager 100 over 500 at page 4 (offset 400). A 100.6 viewport clamps the offset to
// 500 - 100.6 = 399.4. A 99.95 viewport does not clamp (range 400.05) but makes the count
// ceil(500 / 99.95) = 6, still on page round(399.4 / 99.95) = 4. ScrollToPage(4, true)
// targets 4 * 99.95 = 399.8, which is 0.4 from the applied 399.4: under half a unit, so
// nothing scrolls and the content stays at -399.4.
// Guard: fails if the cache synced after the clamp were rounded (-399 is 0.8 away, so the
// request would start a snap). Passes at 3d2c266e only because its stale cache -400 is
// also within half a unit of -399.8.
int UtcDaliPageScrollViewFractionalClampKeepsNoOpThresholdP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();
  pager.ScrollToPage(4, false);
  application.SendNotification();
  application.Render();

  ResizeAlong(pager, false, 100.6f);
  application.SendNotification();
  application.Render();
  ResizeAlong(pager, false, 99.95f);
  application.SendNotification();
  application.Render();

  const Vector2 clamped = pager.GetScrollPosition();
  DALI_TEST_EQUALS(clamped.x, 399.4f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 6, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);

  pager.ScrollToPage(4, true);
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetScrollPosition(), clamped, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -399.4f, 0.001f, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(CurrentContentPositionAlong(content, false), -399.4f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  END_TEST;
}

// Coverage keeper for the NotifyPagesRemoved branch where the removed range overlaps the
// current page (otherwise reached only by the internal suite).
//
// Pager 100 over 500 at page 3 (offset 300). The content shrinks to 300 and pages 2 and 3
// are removed: the range [2, 4) contains page 3, so the current page becomes the first
// removed index, 2, of a count of 3. The notify sets the extent to 300, which clamps the
// offset to 200 = page 2's resting offset, and reports (2, 3) once; the next layout measures
// the same extent and reports nothing.
// Passes at 3d2c266e as well (keeper, not a regression test): fails only if the overlap
// branch stops selecting the start of the removed range.
int UtcDaliPageScrollViewNotifyPagesRemovedOverlapP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();
  pager.ScrollToPage(3, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  ResizeContentAlong(content, false, 300.0f);
  pager.NotifyPagesRemoved(2, 2);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 3, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  END_TEST;
}

// ─── Explicit page requests on partial last pages ───────────────────────────────────

namespace
{
/// A pager 160 long over 500 units of content: ceil(500 / 160) = 4 pages, maximum offset
/// 500 - 160 = 340. The last page is partial: page 3 rests at min(3 * 160, 340) = 340, an
/// offset that rounds to page round(340 / 160) = round(2.125) = 2.
PageScrollView BuildPartialLastPagePager(UiTestApplication& application, ScrollDirection direction, View& content)
{
  const bool     vertical = (direction == ScrollDirection::Vertical);
  PageScrollView pager    = BuildPagerAlong(direction, content);
  ResizeAlong(pager, vertical, 160.0f);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);
  return pager;
}

/// The partial last page of BuildPartialLastPagePager is selected and shown.
void CheckPartialLastPageSelected(PageScrollView pager, View content, bool vertical)
{
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);
  const Vector2 offset = pager.GetScrollPosition();
  DALI_TEST_EQUALS(vertical ? offset.y : offset.x, 340.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical ? offset.x : offset.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -340.0f, 0.001f, TEST_LOCATION);
}

// An instant ScrollToPage to the partial last page selects that page.
//
// ScrollToPage(3, false) on the 160-over-500 pager moves to offset 340 and must report
// (3, 4) once; repeating the request and letting the frames run reports nothing more.
// Fails at 3d2c266e (and at this change's parent) because the instant request recorded no
// target, so OnScrollFinished rounded the settled offset 340 and selected page 2.
void CheckPartialPageInstantSelection(ScrollDirection direction)
{
  UiTestApplication application;
  const bool        vertical = (direction == ScrollDirection::Vertical);
  View              content;
  PageScrollView    pager = BuildPartialLastPagePager(application, direction, content);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);
  pager.ScrollToPage(3, false);
  CheckPartialLastPageSelected(pager, content, vertical);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 4, TEST_LOCATION);

  pager.ScrollToPage(3, false);
  SettleScroll(application);
  CheckPartialLastPageSelected(pager, content, vertical);
  DALI_TEST_EQUALS(CurrentContentPositionAlong(content, vertical), -340.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
}

// A snap retargeted onto the partial last page settles on that page.
//
// ScrollToPage(1, true) on the 160-over-500 pager starts a 300 ms snap to 160. 116 ms in,
// ScrollToPage(3, true) cancels it (the cancellation's ScrollFinished commits the new
// target, (3, 4)) and snaps from the live position to 340. That snap settles at 340, which
// rounds to page 2: the pager must stay on page 3 with no further report. While the frames
// run, the cancelled animation's own Finished arrives late and is ignored.
// Fails at 3d2c266e (and at this change's parent) because the settle re-rounded the offset:
// it reported (2, 4) after (3, 4) and ended on page 2.
void CheckPartialPageRetarget(ScrollDirection direction)
{
  UiTestApplication application;
  const bool        vertical = (direction == ScrollDirection::Vertical);
  View              content;
  PageScrollView    pager = BuildPartialLastPagePager(application, direction, content);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);
  pager.ScrollToPage(1, true);
  application.SendNotification();
  application.Render(16);
  application.Render(100);
  application.SendNotification();
  DALI_TEST_CHECK(pager.IsScrolling());
  DALI_TEST_CHECK(CurrentContentPositionAlong(content, vertical) < -1.0f);

  pager.ScrollToPage(3, true);
  DALI_TEST_CHECK(pager.IsScrolling());

  SettleScroll(application);
  CheckPartialLastPageSelected(pager, content, vertical);
  DALI_TEST_EQUALS(CurrentContentPositionAlong(content, vertical), -340.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 4, TEST_LOCATION);
}

} // namespace

int UtcDaliPageScrollViewPartialPageInstantSelectionHorizontalP(void)
{
  CheckPartialPageInstantSelection(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewPartialPageInstantSelectionVerticalP(void)
{
  CheckPartialPageInstantSelection(ScrollDirection::Vertical);
  END_TEST;
}

int UtcDaliPageScrollViewPartialPageRetargetHorizontalP(void)
{
  CheckPartialPageRetarget(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewPartialPageRetargetVerticalP(void)
{
  CheckPartialPageRetarget(ScrollDirection::Vertical);
  END_TEST;
}

// An animated ScrollToPage whose target rests where the view already is starts no scroll,
// yet it names a page: it must select it.
//
// Pager 250 x 80 over 500: two pages of 250. An explicit page size of 100 set after the
// layout makes five pages and reports (0, 5). Pages 3 and 4 both rest at the maximum offset
// 500 - 250 = 250. ScrollToPage(3, false) moves there and reports (3, 5) (round(2.5) = 3 is
// page 3 either way). ScrollToPage(4, true) targets the same offset, so nothing scrolls;
// the request commits page 4 itself and reports (4, 5). A further layout reports nothing.
// Fails at 3d2c266e (and at this change's parent) because the animated request that started
// no scroll only dropped its target: the pager stayed on page 3.
int UtcDaliPageScrollViewCoincidingPageRequestSelectsPageP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);
  pager.SetRequestedWidth(250.0f);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), 2, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  pager.SetPageSize(Vector2(100.0f, 80.0f));
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 0, TEST_LOCATION);

  pager.ScrollToPage(3, false);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 250.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);

  pager.ScrollToPage(4, true);
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 250.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].second, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[2].first, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[2].second, 5, TEST_LOCATION);
  END_TEST;
}

// An instant ScrollToPage made from inside a ScrollFinished handler moves the content within
// that emission, and the ScrollFinished it sends itself is dropped (a signal ignores a
// nested Emit of itself), so OnScrollFinished never sees it. The request must still commit
// its page.
//
// Pager 100 over 500. A one-shot ScrollFinished handler requests ScrollToPage(2, false).
// ScrollToPage(1, false): the outer ScrollFinished commits (1, 5) first (the pager's own
// slot was connected first); the handler then moves the content to 200 and the request
// commits (2, 5). A later ScrollTo(400, false) settles on page 4, so no stale target is
// left behind. (The nested emission logs an error; it is expected.)
// Fails at 3d2c266e (and at this change's parent) because nothing consumed the nested
// request: the pager stayed on page 1 while showing offset 200.
int UtcDaliPageScrollViewNestedInstantPageRequestCommitsP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  ConnectionTracker tracker;
  bool              requested = false;
  pager.ScrollFinishedSignal().Connect(&tracker, [&pager, &requested](ScrollView)
  {
    if(!requested)
    {
      requested = true;
      pager.ScrollToPage(2, false);
    }
  });

  pager.ScrollToPage(1, false);

  DALI_TEST_CHECK(requested);
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -200.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].first, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].second, 5, TEST_LOCATION);

  pager.ScrollTo(Vector2(400.0f, 0.0f), false);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[2].first, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[2].second, 5, TEST_LOCATION);
  END_TEST;
}

// The same nested instant request, issued from the ScrollFinished that the first layout's
// pending-selection apply sends from inside the layout pass.
//
// Before the first layout the count reads 1, so three insertions make four pages and select
// page 3, reporting (3, 4). The first layout applies page 3 (offset 300) and a one-shot
// ScrollFinished handler, connected after the insertion, then requests ScrollToPage(1,
// false). The request commits (1, 5) itself (five real pages); the apply keeps that newer
// page and reports nothing more: page 1 at offset 100.
// Fails at 3d2c266e (and at this change's parent) because the nested request was never
// committed: the apply reported its own selection, (3, 5), while the content showed page 1.
int UtcDaliPageScrollViewNestedInstantPageRequestDuringFirstLayoutP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);
  pager.NotifyPagesInserted(0, 3);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 4, TEST_LOCATION);

  ConnectionTracker tracker;
  bool              requested = false;
  pager.ScrollFinishedSignal().Connect(&tracker, [&pager, &requested](ScrollView)
  {
    if(!requested)
    {
      requested = true;
      pager.ScrollToPage(1, false);
    }
  });

  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(requested);
  DALI_TEST_CHECK(!pager.IsScrolling());
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].second, 5, TEST_LOCATION);
  END_TEST;
}

// ─── Viewport clamps of a settled pager with a fixed page size ──────────────────────

namespace
{
// A fixed page size keeps the count and the page length when the viewport grows, but the
// clamp still moves a settled pager: the page must follow the offset.
//
// Pager 100 over 500 with an explicit page size of 100 along the scroll axis: five pages.
// ScrollToPage(4, false) rests at 400. A 350 viewport leaves a range of 500 - 350 = 150, so
// the layout clamps the offset to 150 while the count (5) and the page length (100) stay:
// the page becomes round(150 / 100) = round(1.5) = 2, reported once as (2, 5). Another
// layout reports nothing, and a 300 viewport (range 200) does not move the offset and
// reports nothing either.
// Fails at 3d2c266e (and at the parent of this change) because the geometry and the count
// were unchanged: the clamped pager kept page 4 and reported nothing.
void CheckFixedPageSizeViewportClamp(ScrollDirection direction)
{
  UiTestApplication application;
  const bool        vertical = (direction == ScrollDirection::Vertical);
  View              content;
  PageScrollView    pager = BuildPagerAlong(direction, content);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();

  pager.SetPageSize(vertical ? Vector2(PAGER_HEIGHT, PAGER_WIDTH) : Vector2(PAGER_WIDTH, PAGER_HEIGHT));
  pager.ScrollToPage(4, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -400.0f, 0.001f, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  ResizeAlong(pager, vertical, 350.0f);
  application.SendNotification();
  application.Render();

  const Vector2 clamped = pager.GetScrollPosition();
  DALI_TEST_EQUALS(vertical ? clamped.y : clamped.x, 150.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vertical ? clamped.x : clamped.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, vertical), -150.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);

  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);

  ResizeAlong(pager, vertical, 300.0f);
  application.SendNotification();
  application.Render();
  const Vector2 unclamped = pager.GetScrollPosition();
  DALI_TEST_EQUALS(vertical ? unclamped.y : unclamped.x, 150.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
}

} // namespace

int UtcDaliPageScrollViewFixedPageSizeViewportClampHorizontalP(void)
{
  CheckFixedPageSizeViewportClamp(ScrollDirection::Horizontal);
  END_TEST;
}

int UtcDaliPageScrollViewFixedPageSizeViewportClampVerticalP(void)
{
  CheckFixedPageSizeViewportClamp(ScrollDirection::Vertical);
  END_TEST;
}

// Guard: a snap that settles on the partial last page records the offset it settled at.
//
// ScrollToPage(3, true) on the 160-over-500 pager snaps to 340 and commits (3, 4). A
// cross-axis resize (80 -> 90) then repeats the callback with the same geometry, count and
// offset: nothing may change.
// Guard: fails if OnScrollFinished stopped recording the offset it committed the page at
// (the callback would compare 340 with the 0 recorded by the first layout, take it for a
// clamp and re-round to page round(340 / 160) = 2). Passes at the parent of this change,
// which never re-derives on an unchanged geometry and count.
int UtcDaliPageScrollViewSettledPartialPageSurvivesCrossAxisResizeP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPartialLastPagePager(application, ScrollDirection::Horizontal, content);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);
  pager.ScrollToPage(3, true);
  DALI_TEST_CHECK(pager.IsScrolling());
  SettleScroll(application);
  CheckPartialLastPageSelected(pager, content, false);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 4, TEST_LOCATION);

  pager.SetRequestedHeight(90.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 90.0f, 0.001f, TEST_LOCATION);
  CheckPartialLastPageSelected(pager, content, false);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  END_TEST;
}

// Guard: a nested instant request that commits its page itself also records that page's
// offset.
//
// A one-shot ScrollFinished handler requests ScrollToPage(3, false) on the 160-over-500
// pager. ScrollToPage(1, false) commits (1, 4) at offset 160; the handler's request then
// moves the content to 340 and, its own ScrollFinished being dropped, commits (3, 4)
// itself. A cross-axis resize (80 -> 90) repeats the callback with the same geometry, count
// and offset: nothing may change.
// Guard: fails if that commit stopped recording the offset (the callback would compare 340
// with the 160 recorded for page 1, take it for a clamp and re-round to page 2). Passes at
// the parent of this change, which never re-derives on an unchanged geometry and count.
int UtcDaliPageScrollViewNestedPartialPageSurvivesCrossAxisResizeP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPartialLastPagePager(application, ScrollDirection::Horizontal, content);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  ConnectionTracker tracker;
  bool              requested = false;
  pager.ScrollFinishedSignal().Connect(&tracker, [&pager, &requested](ScrollView)
  {
    if(!requested)
    {
      requested = true;
      pager.ScrollToPage(3, false);
    }
  });

  pager.ScrollToPage(1, false);
  DALI_TEST_CHECK(requested);
  CheckPartialLastPageSelected(pager, content, false);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].first, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[1].second, 4, TEST_LOCATION);

  pager.SetRequestedHeight(90.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 90.0f, 0.001f, TEST_LOCATION);
  CheckPartialLastPageSelected(pager, content, false);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 2, TEST_LOCATION);
  END_TEST;
}

// A half-unit maximum offset must not read as a clamp. A pager 124.5 long over 500 has
// ceil(500 / 124.5) = 5 pages and a maximum offset of 500 - 124.5 = 375.5; its last page is a
// sliver resting there although round(375.5 / 124.5) = 3. An instant scroll caches the
// ROUNDED content position, 376 here, while the content actor rests at -375.5, until the next
// layout re-clamps the cache to 375.5 without moving anything.
//
// ScrollToPage(4, false) selects page 4: (4, 5) once, content -375.5, offset within half a
// unit of 375.5 (the rounded cache). A cross-axis resize (80 -> 90) re-clamps the cache to
// 375.5 and must report nothing: page 4, offset 375.5.
// Fails at ed9b5bd1 (the commit before this fix) because the settled offset was recorded as
// the rounded 376, so the re-clamp to 375.5 compared as a half-unit move and re-rounded the
// page to 3, emitting (3, 5). Fails at 3d2c266e already at the request, which selected page
// round(376 / 124.5) = 3.
int UtcDaliPageScrollViewHalfPixelMaximumKeepsPartialPageP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);
  pager.SetRequestedWidth(124.5f);
  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);

  pager.ScrollToPage(4, false);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 375.5f, 0.501f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -375.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);

  pager.SetRequestedHeight(90.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 90.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 375.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -375.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  END_TEST;
}

// Guard against a regression of 3d2c266e on the same half-unit pager: a page selected by
// NotifyPagesInserted before the first layout is applied by that layout and kept.
//
// Before any layout the count reads 1, so four insertions make five pages and select page 4,
// reporting (4, 5). The first layout's width callback applies page 4 (content -375.5, the
// instant scroll caching 376); its height callback, in the same pass, re-clamps the cache to
// 375.5. The selection must survive with nothing more reported: page 4 of 5 at 375.5.
// Passes at 3d2c266e. Fails at ed9b5bd1 (the commit before this fix) because the height
// callback took the re-clamp for a move and re-rounded the selection to page 3, emitting
// (3, 5) while the last page was shown.
int UtcDaliPageScrollViewHalfPixelMaximumKeepsNotifiedPartialPageP(void)
{
  UiTestApplication application;
  View              content;
  PageScrollView    pager = BuildPager(content);
  pager.SetRequestedWidth(124.5f);

  PageSignalRecorder recorder;
  pager.PageChangedSignal().Connect(&recorder, &PageSignalRecorder::OnPageChanged);
  pager.NotifyPagesInserted(0, 4);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);

  application.GetWindow().Add(pager);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetScrollPosition().x, 375.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(ContentPositionAlong(content, false), -375.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(recorder.signals.size()), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].first, 4, TEST_LOCATION);
  DALI_TEST_EQUALS(recorder.signals[0].second, 5, TEST_LOCATION);
  END_TEST;
}
