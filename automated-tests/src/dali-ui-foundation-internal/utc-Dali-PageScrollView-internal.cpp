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
 */

#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/views/scroll/page-scroll-view.h>
#include <dali-ui-test-suite-utils.h>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/page-scroll-view-impl.h>
#undef protected
#undef private

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

void utc_dali_page_scroll_view_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_page_scroll_view_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
PageScrollViewImpl& GetPageImpl(PageScrollView view)
{
  return static_cast<PageScrollViewImpl&>(view.GetImplementation());
}
} // unnamed namespace

int UtcDaliPageScrollViewInternalPagingP(void)
{
  UiTestApplication application(UiConfig::New());
  PageScrollView view = PageScrollView::New();
  view.SetContent(View::New());
  PageScrollViewImpl& impl = GetPageImpl(view);
  int signalCount = 0;
  impl.PageChangedSignal().Connect(&application, [&signalCount](int, int) { ++signalCount; });

  DALI_TEST_EQUALS(impl.GetPageSize(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPageCount(), 1, TEST_LOCATION);
  impl.SetScrollDirection(ScrollDirection::Horizontal);
  impl.SetPageSize(Vector2(100.0f, 80.0f));
  impl.SetScrollableWidth(500.0f);
  DALI_TEST_EQUALS(impl.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetCurrentPage(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.ScrollPositionForPage(3), Vector2(300.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.PageForScrollPosition(Vector2(240.0f, 0.0f)), 2, TEST_LOCATION);

  impl.ScrollToPage(2, false);
  impl.OnScrollFinished(view);
  impl.ScrollToPage(99, true);
  application.SendNotification();
  application.Render(400u);
  impl.OnScrollFinished(view);
  DALI_TEST_CHECK(signalCount > 0);

  impl.SetPageSize(Vector2(50.0f, 80.0f));
  DALI_TEST_EQUALS(impl.GetPageCount(), 10, TEST_LOCATION);
  impl.SetPageSize(Vector2(50.0f, 80.0f));
  END_TEST;
}

int UtcDaliPageScrollViewInternalSnapP(void)
{
  UiTestApplication application(UiConfig::New());
  PageScrollView view = PageScrollView::New();
  view.SetContent(View::New());
  PageScrollViewImpl& impl = GetPageImpl(view);
  impl.SetScrollDirection(ScrollDirection::Horizontal);
  impl.SetPageSize(Vector2(100.0f, 100.0f));
  impl.SetScrollableWidth(500.0f);

  Vector2 target(450.0f, 0.0f);
  float duration = 1.0f;
  impl.OnBeforeScrollAnimation(target, duration);
  DALI_TEST_EQUALS(duration, 0.3f, TEST_LOCATION);

  impl.mSnapTargetPage = -1;
  impl.SetScrollPosition(Vector2(40.0f, 0.0f));
  target = Vector2::ZERO;
  duration = 0.0f;
  impl.OnBeforeScrollAnimation(target, duration);

  impl.mSnapTargetPage = -1;
  impl.mCurrentPage = 2;
  impl.SetScrollPosition(Vector2(150.0f, 0.0f));
  duration = 0.0f;
  impl.OnBeforeScrollAnimation(target, duration);

  impl.mCurrentPage = 2;
  impl.SetScrollPosition(Vector2(210.0f, 0.0f));
  duration = 0.0f;
  impl.OnBeforeScrollAnimation(target, duration);

  impl.SetPageSize(Vector2::ZERO);
  impl.mPageSize = Vector2::ZERO;
  impl.OnBeforeScrollAnimation(target, duration);
  END_TEST;
}

int UtcDaliPageScrollViewInternalDynamicPagesP(void)
{
  UiTestApplication application(UiConfig::New());
  PageScrollView view = PageScrollView::New();
  view.SetContent(View::New());
  PageScrollViewImpl& impl = GetPageImpl(view);
  impl.SetPageSize(Vector2(60.0f, 90.0f));
  impl.SetScrollDirection(ScrollDirection::Vertical);
  impl.SetScrollableHeight(360.0f);
  DALI_TEST_EQUALS(impl.GetPageCount(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.ScrollPositionForPage(2), Vector2(0.0f, 180.0f), TEST_LOCATION);

  impl.mCurrentPage = 2;
  impl.NotifyPagesInserted(1, 2);
  DALI_TEST_EQUALS(impl.GetPageCount(), 6, TEST_LOCATION);
  impl.NotifyPagesInserted(20, 0);
  impl.NotifyPagesRemoved(5, 1);
  impl.NotifyPagesRemoved(0, 1);
  impl.NotifyPagesRemoved(1, 2);
  impl.NotifyPagesRemoved(0, 0);
  impl.NotifyPagesRemoved(99, 99);

  impl.mNotifyInProgress = true;
  impl.OnScrollableAreaChanged();
  impl.OnScrollFinished(view);
  impl.mNotifyInProgress = false;
  impl.mExpectedPageCount = -1;
  impl.SetScrollableHeight(0.0f);
  impl.OnScrollableAreaChanged();
  impl.SetScrollableHeight(180.0f);
  impl.OnScrollableAreaChanged();
  impl.OnScrollableAreaChanged();

  impl.mExpectedPageCount = 0;
  DALI_TEST_EQUALS(impl.GetCurrentPage(), -1, TEST_LOCATION);
  impl.CommitPage(0);
  END_TEST;
}
