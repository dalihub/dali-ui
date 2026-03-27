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
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

class ScrollStartedCallback : public ConnectionTracker
{
public:
  ScrollStartedCallback()
    : called(false)
  {
  }

  void OnScrollStarted(ScrollView scrollView)
  {
    called          = true;
    receivedView = scrollView;
  }

  void Reset()
  {
    called = false;
  }

  bool       called;
  ScrollView receivedView;
};

class ScrollFinishedCallback : public ConnectionTracker
{
public:
  ScrollFinishedCallback()
    : called(false)
  {
  }

  void OnScrollFinished(ScrollView scrollView)
  {
    called       = true;
    receivedView = scrollView;
  }

  void Reset()
  {
    called = false;
  }

  bool       called;
  ScrollView receivedView;
};

class ScrollingCallback : public ConnectionTracker
{
public:
  ScrollingCallback()
    : called(false)
  {
  }

  void OnScrolling(ScrollView scrollView)
  {
    called       = true;
    receivedView = scrollView;
  }

  bool       called;
  ScrollView receivedView;
};

class DragStartedCallback : public ConnectionTracker
{
public:
  DragStartedCallback()
    : called(false)
  {
  }

  void OnDragStarted(ScrollView scrollView)
  {
    called       = true;
    receivedView = scrollView;
  }

  bool       called;
  ScrollView receivedView;
};

class DragFinishedCallback : public ConnectionTracker
{
public:
  DragFinishedCallback()
    : called(false)
  {
  }

  void OnDragFinished(ScrollView scrollView)
  {
    called       = true;
    receivedView = scrollView;
  }

  bool       called;
  ScrollView receivedView;
};

class DraggingCallback : public ConnectionTracker
{
public:
  DraggingCallback()
    : called(false),
      deltaX(0.0f),
      deltaY(0.0f)
  {
  }

  void OnDragging(ScrollView scrollView, float dx, float dy)
  {
    called       = true;
    receivedView = scrollView;
    deltaX       = dx;
    deltaY       = dy;
  }

  void Reset()
  {
    called = false;
    deltaX = 0.0f;
    deltaY = 0.0f;
  }

  bool       called;
  ScrollView receivedView;
  float      deltaX;
  float      deltaY;
};

} // namespace

void utc_dali_scroll_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_scroll_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Constructor Tests

int UtcDaliScrollViewConstructorP(void)
{
  TestApplication application;

  ScrollView scrollView;
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

int UtcDaliScrollViewNewP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_CHECK(scrollView);

  END_TEST;
}

int UtcDaliScrollViewCopyConstructorP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  ScrollView copy(scrollView);

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(scrollView == copy);

  END_TEST;
}

int UtcDaliScrollViewMoveConstructorP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_EQUALS(1, scrollView.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  ScrollView moved = std::move(scrollView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

int UtcDaliScrollViewCopyAssignmentP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  ScrollView copy;
  copy = scrollView;

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(scrollView == copy);

  END_TEST;
}

int UtcDaliScrollViewMoveAssignmentP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_EQUALS(1, scrollView.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  ScrollView moved;
  moved = std::move(scrollView);

  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

// DownCast Tests

int UtcDaliScrollViewDownCastP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  BaseHandle object(scrollView);

  ScrollView scrollView2 = ScrollView::DownCast(object);
  DALI_TEST_CHECK(scrollView2);
  DALI_TEST_CHECK(scrollView == scrollView2);

  END_TEST;
}

int UtcDaliScrollViewDownCastN(void)
{
  TestApplication application;

  BaseHandle uninitialized;
  ScrollView scrollView = ScrollView::DownCast(uninitialized);
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

int UtcDaliScrollViewDownCastFromViewN(void)
{
  TestApplication application;

  // A plain View should not downcast to ScrollView
  View view = View::New();
  BaseHandle object(view);

  ScrollView scrollView = ScrollView::DownCast(object);
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

// Content Tests

int UtcDaliScrollViewSetGetContentP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  View       content    = View::New();

  scrollView.SetContent(content);
  View retrieved = scrollView.GetContent();

  DALI_TEST_CHECK(retrieved);
  DALI_TEST_CHECK(retrieved == content);

  END_TEST;
}

int UtcDaliScrollViewSetContentChainingP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  View       content    = View::New();

  ScrollView& result = scrollView.SetContent(content);
  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// ScrollPosition Tests

int UtcDaliScrollViewSetGetScrollPositionP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Both);

  View content = View::New();
  content.SetRequestedWidth(1000.0f);
  content.SetRequestedHeight(1000.0f);
  scrollView.SetContent(content);

  const Vector2 position(100.0f, 200.0f);
  scrollView.SetScrollPosition(position);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition(), position, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetScrollPositionChainingP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Both);

  View content = View::New();
  content.SetRequestedWidth(1000.0f);
  content.SetRequestedHeight(1000.0f);
  scrollView.SetContent(content);

  const Vector2  position(50.0f, 75.0f);
  ScrollView& result = scrollView.SetScrollPosition(position);
  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// ScrollDirection Tests

int UtcDaliScrollViewSetGetScrollDirectionVerticalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Vertical, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetScrollDirectionHorizontalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Horizontal);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Horizontal, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetScrollDirectionBothP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Both);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Both, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetScrollDirectionChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetScrollDirection(ScrollDirection::Vertical);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// MaxFlingDistance Tests

int UtcDaliScrollViewSetGetMaxFlingDistanceP(void)
{
  TestApplication application;

  ScrollView  scrollView  = ScrollView::New();
  const float testDistance = 3000.0f;

  scrollView.SetMaxFlingDistance(testDistance);
  DALI_TEST_EQUALS(scrollView.GetMaxFlingDistance(), testDistance, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetMaxFlingDistanceChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetMaxFlingDistance(1000.0f);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// MinimumFlingDuration Tests

int UtcDaliScrollViewSetGetMinimumFlingDurationP(void)
{
  TestApplication application;

  ScrollView scrollView   = ScrollView::New();
  const int  testDuration = 500;

  scrollView.SetMinimumFlingDuration(testDuration);
  DALI_TEST_EQUALS(scrollView.GetMinimumFlingDuration(), testDuration, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetMinimumFlingDurationChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetMinimumFlingDuration(500);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// MaximumFlingDuration Tests

int UtcDaliScrollViewSetGetMaximumFlingDurationP(void)
{
  TestApplication application;

  ScrollView scrollView   = ScrollView::New();
  const int  testDuration = 3000;

  scrollView.SetMaximumFlingDuration(testDuration);
  DALI_TEST_EQUALS(scrollView.GetMaximumFlingDuration(), testDuration, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetMaximumFlingDurationChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetMaximumFlingDuration(3000);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// FlingSensitivity Tests

int UtcDaliScrollViewSetGetFlingSensitivityP(void)
{
  TestApplication application;

  ScrollView  scrollView      = ScrollView::New();
  const float testSensitivity = 2.0f;

  scrollView.SetFlingSensitivity(testSensitivity);
  DALI_TEST_EQUALS(scrollView.GetFlingSensitivity(), testSensitivity, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetFlingSensitivityChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetFlingSensitivity(1.5f);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// DecelerationRate Tests

int UtcDaliScrollViewSetGetDecelerationRateP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  const float testRate   = 0.95f;

  scrollView.SetDecelerationRate(testRate);
  DALI_TEST_EQUALS(scrollView.GetDecelerationRate(), testRate, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetDecelerationRateChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetDecelerationRate(0.95f);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// OverScrollMode Tests

int UtcDaliScrollViewSetGetOverScrollModeNeverP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::Never);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::Never, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetOverScrollModeAlwaysP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::Always);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetOverScrollModeContentScrollsP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::ContentScrolls, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetOverScrollModeChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetOverScrollMode(OverScrollMode::Never);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// ScrollBar Visibility Tests

int UtcDaliScrollViewSetGetVerticalScrollBarVisibilityP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);

  DALI_TEST_EQUALS(scrollView.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetVerticalScrollBarVisibilityNeverP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);

  DALI_TEST_EQUALS(scrollView.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Never, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetVerticalScrollBarVisibilityChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

int UtcDaliScrollViewSetGetHorizontalScrollBarVisibilityP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Always);

  DALI_TEST_EQUALS(scrollView.GetHorizontalScrollBarVisibility(), ScrollBarVisibility::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetHorizontalScrollBarVisibilityChainingP(void)
{
  TestApplication application;

  ScrollView  scrollView = ScrollView::New();
  ScrollView& result     = scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);

  DALI_TEST_CHECK(&result == &scrollView);

  END_TEST;
}

// IsScrolling Tests

int UtcDaliScrollViewIsScrollingInitialP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_CHECK(!scrollView.IsScrolling());

  END_TEST;
}

// ScrollTo Tests

int UtcDaliScrollViewScrollToPositionNoAnimP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Both);

  View content = View::New();
  content.SetRequestedWidth(1000.0f);
  content.SetRequestedHeight(1000.0f);
  scrollView.SetContent(content);

  scrollView.SetRequestedWidth(300.0f);
  scrollView.SetRequestedHeight(300.0f);

  scrollView.ScrollTo(Vector2(100.0f, 100.0f), false);

  // No animation, position applied immediately
  DALI_TEST_EQUALS(scrollView.GetScrollPosition(), Vector2(100.0f, 100.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewScrollToXNoAnimP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Horizontal);

  View content = View::New();
  content.SetRequestedWidth(1000.0f);
  content.SetRequestedHeight(300.0f);
  scrollView.SetContent(content);

  scrollView.SetRequestedWidth(300.0f);
  scrollView.SetRequestedHeight(300.0f);

  scrollView.ScrollToX(150.0f, false);

  DALI_TEST_EQUALS(scrollView.GetScrollPosition().x, 150.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewScrollToYNoAnimP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetRequestedWidth(300.0f);
  content.SetRequestedHeight(1000.0f);
  scrollView.SetContent(content);

  scrollView.SetRequestedWidth(300.0f);
  scrollView.SetRequestedHeight(300.0f);

  scrollView.ScrollToY(200.0f, false);

  DALI_TEST_EQUALS(scrollView.GetScrollPosition().y, 200.0f, TEST_LOCATION);

  END_TEST;
}

// Signal Tests

int UtcDaliScrollViewScrollStartedSignalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  ScrollStartedCallback callback;
  scrollView.ScrollStartedSignal().Connect(&callback, &ScrollStartedCallback::OnScrollStarted);

  DALI_TEST_CHECK(!callback.called);
  DALI_TEST_CHECK(scrollView.ScrollStartedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewScrollFinishedSignalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  ScrollFinishedCallback callback;
  scrollView.ScrollFinishedSignal().Connect(&callback, &ScrollFinishedCallback::OnScrollFinished);

  DALI_TEST_CHECK(scrollView.ScrollFinishedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewScrollingSignalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  ScrollingCallback callback;
  scrollView.ScrollingSignal().Connect(&callback, &ScrollingCallback::OnScrolling);

  DALI_TEST_CHECK(scrollView.ScrollingSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDragStartedSignalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  DragStartedCallback callback;
  scrollView.DragStartedSignal().Connect(&callback, &DragStartedCallback::OnDragStarted);

  DALI_TEST_CHECK(scrollView.DragStartedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDragFinishedSignalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  DragFinishedCallback callback;
  scrollView.DragFinishedSignal().Connect(&callback, &DragFinishedCallback::OnDragFinished);

  DALI_TEST_CHECK(scrollView.DragFinishedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDraggingSignalP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  DraggingCallback callback;
  scrollView.DraggingSignal().Connect(&callback, &DraggingCallback::OnDragging);

  DALI_TEST_CHECK(scrollView.DraggingSignal().GetConnectionCount() > 0u);

  END_TEST;
}

// Method Chaining Tests

int UtcDaliScrollViewMethodChainingP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();
  View       content    = View::New();

  // Verify all setters can be chained
  ScrollView& result = scrollView
                         .SetContent(content)
                         .SetScrollDirection(ScrollDirection::Vertical)
                         .SetMaxFlingDistance(5000.0f)
                         .SetMinimumFlingDuration(800)
                         .SetMaximumFlingDuration(2500)
                         .SetFlingSensitivity(1.2f)
                         .SetDecelerationRate(0.99f)
                         .SetOverScrollMode(OverScrollMode::ContentScrolls)
                         .SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto)
                         .SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);

  DALI_TEST_CHECK(&result == &scrollView);

  // Verify all values were set correctly
  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Vertical, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetMaxFlingDistance(), 5000.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetMinimumFlingDuration(), 800, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetMaximumFlingDuration(), 2500, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetFlingSensitivity(), 1.2f, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetDecelerationRate(), 0.99f, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::ContentScrolls, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Auto, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetHorizontalScrollBarVisibility(), ScrollBarVisibility::Never, TEST_LOCATION);

  END_TEST;
}

// View Inheritance Test

int UtcDaliScrollViewIsViewP(void)
{
  TestApplication application;

  ScrollView scrollView = ScrollView::New();

  // ScrollView should be usable as a View
  View view = scrollView;
  DALI_TEST_CHECK(view);

  END_TEST;
}
