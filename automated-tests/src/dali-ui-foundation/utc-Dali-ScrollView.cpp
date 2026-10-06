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
#include <dali-ui-foundation/public-api/views/scroll/page-scroll-view.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <stdlib.h>
#include <iostream>

#define private public
#define protected public
#include <dali-ui-foundation/integration-api/scroll-view-impl.h>
#include <dali-ui-foundation/integration-api/page-scroll-view-impl.h>
#undef protected
#undef private

using namespace Dali;
using namespace Dali::Ui;

namespace Test
{
void EmitGlobalTimerSignal();
}

namespace
{

Dali::Integration::TouchEvent GenerateTouch(PointState::Type state, const Vector2& screenPosition, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

Dali::Integration::TouchEvent GenerateTouch(PointState::Type state, const Vector2& screenPosition, uint32_t time, int32_t deviceId)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(deviceId);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

Dali::Integration::TouchEvent GenerateDoubleTouch(PointState::Type stateA, const Vector2& screenPositionA, PointState::Type stateB, const Vector2& screenPositionB, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(stateA);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPositionA);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);

  point.SetState(stateB);
  point.SetDeviceId(7);
  point.SetScreenPosition(screenPositionB);
  touchEvent.points.push_back(point);

  touchEvent.time = time;
  return touchEvent;
}

Dali::Ui::Integration::ScrollViewImpl& GetScrollImpl(ScrollView scrollView)
{
  return static_cast<Dali::Ui::Integration::ScrollViewImpl&>(scrollView.GetImplementation());
}

struct PressedChangedSignalData
{
  void Reset()
  {
    called     = false;
    pressed    = false;
    trueCount  = 0u;
    falseCount = 0u;
    view       = View();
  }

  bool     called{false};
  bool     pressed{false};
  uint32_t trueCount{0u};
  uint32_t falseCount{0u};
  View     view;
};

struct PressedChangedSignalFunctor
{
  PressedChangedSignalFunctor(PressedChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool pressed, InputEvent event)
  {
    signalData.called  = true;
    signalData.pressed = pressed;
    signalData.view    = view;
    if(pressed)
    {
      signalData.trueCount++;
    }
    else
    {
      signalData.falseCount++;
    }
  }

  PressedChangedSignalData& signalData;
};

class ScrollStartedCallback : public ConnectionTracker
{
public:
  ScrollStartedCallback()
  : called(false)
  {
  }

  void OnScrollStarted(ScrollView scrollView)
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

int UtcDaliUiConfigAmbiguousPressDefaultsAndSettersP(void)
{
  UiConfig config = UiConfig::New();

  DALI_TEST_CHECK(!UiConfig::HasCurrent());
  DALI_TEST_ASSERTION(UiConfig::GetCurrent(), "UICONFIG_NOT_APPLIED_MESSAGE");

  DALI_TEST_EQUALS(config.GetAmbiguousPressDelay(), 100u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetAmbiguousPressDuration(), 64u, TEST_LOCATION);

  config.SetAmbiguousPressDelay(120u);
  config.SetAmbiguousPressDuration(48u);

  DALI_TEST_EQUALS(config.GetAmbiguousPressDelay(), 120u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetAmbiguousPressDuration(), 48u, TEST_LOCATION);

  config.SetAmbiguousPressDelay(0u);
  config.SetAmbiguousPressDuration(0u);

  DALI_TEST_EQUALS(config.GetAmbiguousPressDelay(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetAmbiguousPressDuration(), 0u, TEST_LOCATION);

  config.Apply();

  DALI_TEST_CHECK(UiConfig::HasCurrent());
  UiConfig current = UiConfig::GetCurrent();
  DALI_TEST_EQUALS(current.GetAmbiguousPressDelay(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(current.GetAmbiguousPressDuration(), 0u, TEST_LOCATION);

  UiConfig secondConfig = UiConfig::New();
  DALI_TEST_ASSERTION(secondConfig.Apply(), "UiConfig::Apply() must be called only once");

  END_TEST;
}

// Constructor Tests

int UtcDaliScrollViewConstructorP(void)
{
  UiTestApplication application;

  ScrollView scrollView;
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

int UtcDaliScrollViewNewP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_CHECK(scrollView);

  END_TEST;
}

int UtcDaliScrollViewCopyConstructorP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  ScrollView copy(scrollView);

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(scrollView == copy);

  END_TEST;
}

int UtcDaliScrollViewMoveConstructorP(void)
{
  UiTestApplication application;

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
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  ScrollView copy;
  copy = scrollView;

  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(scrollView == copy);

  END_TEST;
}

int UtcDaliScrollViewMoveAssignmentP(void)
{
  UiTestApplication application;

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
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  BaseHandle object(scrollView);

  ScrollView scrollView2 = ScrollView::DownCast(object);
  DALI_TEST_CHECK(scrollView2);
  DALI_TEST_CHECK(scrollView == scrollView2);

  END_TEST;
}

int UtcDaliScrollViewDownCastN(void)
{
  UiTestApplication application;

  BaseHandle uninitialized;
  ScrollView scrollView = ScrollView::DownCast(uninitialized);
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

int UtcDaliScrollViewDownCastFromViewN(void)
{
  UiTestApplication application;

  // A plain View should not downcast to ScrollView
  View       view = View::New();
  BaseHandle object(view);

  ScrollView scrollView = ScrollView::DownCast(object);
  DALI_TEST_CHECK(!scrollView);

  END_TEST;
}

// Content Tests

int UtcDaliScrollViewSetGetContentP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  View       content    = View::New();

  scrollView.SetContent(content);
  View retrieved = scrollView.GetContent();

  DALI_TEST_CHECK(retrieved);
  DALI_TEST_CHECK(retrieved == content);

  END_TEST;
}

int UtcDaliScrollViewSetContentSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  View       content    = View::New();

  scrollView.SetContent(content);
  DALI_TEST_CHECK(scrollView.GetContent() == content);

  END_TEST;
}

// ScrollPosition Tests

int UtcDaliScrollViewSetGetScrollPositionP(void)
{
  UiTestApplication application;

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

int UtcDaliScrollViewSetScrollPositionSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Both);

  View content = View::New();
  content.SetRequestedWidth(1000.0f);
  content.SetRequestedHeight(1000.0f);
  scrollView.SetContent(content);

  const Vector2 position(50.0f, 75.0f);
  scrollView.SetScrollPosition(position);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition(), position, TEST_LOCATION);

  END_TEST;
}

// ScrollDirection Tests

int UtcDaliScrollViewSetGetScrollDirectionVerticalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Vertical, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetScrollDirectionHorizontalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Horizontal);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Horizontal, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetScrollDirectionBothP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Both);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Both, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetScrollDirectionSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  DALI_TEST_EQUALS(scrollView.GetScrollDirection(), ScrollDirection::Vertical, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewPanScrollEnabledP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  DragStartedCallback dragStarted;
  scrollView.DragStartedSignal().Connect(
    &dragStarted,
    &DragStartedCallback::OnDragStarted);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(scrollView.IsPanScrollEnabled());
  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 160.0f), 100u));

  // Match a child long-press drag: the ScrollView has already observed DOWN
  // when the child drag starts and suspends pan scrolling.
  scrollView.SetPanScrollEnabled(false);
  DALI_TEST_CHECK(!scrollView.IsPanScrollEnabled());

  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 120.0f), 116u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 80.0f), 132u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 148u));
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 40.0f), 164u));

  DALI_TEST_CHECK(!dragStarted.called);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition(), Vector2::ZERO, TEST_LOCATION);

  // Programmatic scrolling remains available for edge auto-scroll.
  scrollView.ScrollToY(120.0f, false);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition().y, 120.0f, TEST_LOCATION);

  scrollView.SetPanScrollEnabled(true);
  DALI_TEST_CHECK(scrollView.IsPanScrollEnabled());

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 160.0f), 200u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 120.0f), 216u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 80.0f), 232u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 248u));
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 40.0f), 264u));

  DALI_TEST_CHECK(dragStarted.called);
  DALI_TEST_CHECK(scrollView.GetScrollPosition().y > 120.0f);

  END_TEST;
}

// MaxFlingDistance Tests

int UtcDaliScrollViewSetGetMaxFlingDistanceP(void)
{
  UiTestApplication application;

  ScrollView  scrollView   = ScrollView::New();
  const float testDistance = 3000.0f;

  scrollView.SetMaxFlingDistance(testDistance);
  DALI_TEST_EQUALS(scrollView.GetMaxFlingDistance(), testDistance, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetMaxFlingDistanceSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetMaxFlingDistance(1000.0f);

  DALI_TEST_EQUALS(scrollView.GetMaxFlingDistance(), 1000.0f, TEST_LOCATION);

  END_TEST;
}

// MinimumFlingDuration Tests

int UtcDaliScrollViewSetGetMinimumFlingDurationP(void)
{
  UiTestApplication application;

  ScrollView scrollView   = ScrollView::New();
  const int  testDuration = 500;

  scrollView.SetMinimumFlingDuration(testDuration);
  DALI_TEST_EQUALS(scrollView.GetMinimumFlingDuration(), testDuration, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetMinimumFlingDurationSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetMinimumFlingDuration(500);

  DALI_TEST_EQUALS(scrollView.GetMinimumFlingDuration(), 500, TEST_LOCATION);

  END_TEST;
}

// MaximumFlingDuration Tests

int UtcDaliScrollViewSetGetMaximumFlingDurationP(void)
{
  UiTestApplication application;

  ScrollView scrollView   = ScrollView::New();
  const int  testDuration = 3000;

  scrollView.SetMaximumFlingDuration(testDuration);
  DALI_TEST_EQUALS(scrollView.GetMaximumFlingDuration(), testDuration, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetMaximumFlingDurationSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetMaximumFlingDuration(3000);

  DALI_TEST_EQUALS(scrollView.GetMaximumFlingDuration(), 3000, TEST_LOCATION);

  END_TEST;
}

// FlingSensitivity Tests

int UtcDaliScrollViewSetGetFlingSensitivityP(void)
{
  UiTestApplication application;

  ScrollView  scrollView      = ScrollView::New();
  const float testSensitivity = 2.0f;

  scrollView.SetFlingSensitivity(testSensitivity);
  DALI_TEST_EQUALS(scrollView.GetFlingSensitivity(), testSensitivity, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetFlingSensitivitySetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetFlingSensitivity(1.5f);

  DALI_TEST_EQUALS(scrollView.GetFlingSensitivity(), 1.5f, TEST_LOCATION);

  END_TEST;
}

// DecelerationRate Tests

int UtcDaliScrollViewSetGetDecelerationRateP(void)
{
  UiTestApplication application;

  ScrollView  scrollView = ScrollView::New();
  const float testRate   = 0.95f;

  scrollView.SetDecelerationRate(testRate);
  DALI_TEST_EQUALS(scrollView.GetDecelerationRate(), testRate, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetDecelerationRateSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetDecelerationRate(0.95f);

  DALI_TEST_EQUALS(scrollView.GetDecelerationRate(), 0.95f, TEST_LOCATION);

  END_TEST;
}

// OverScrollMode Tests

int UtcDaliScrollViewSetGetOverScrollModeNeverP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::Never);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::Never, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetOverScrollModeAlwaysP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::Always);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetOverScrollModeContentScrollsP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::ContentScrolls, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetOverScrollModeSetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetOverScrollMode(OverScrollMode::Never);

  DALI_TEST_EQUALS(scrollView.GetOverScrollMode(), OverScrollMode::Never, TEST_LOCATION);

  END_TEST;
}

// ScrollBar Visibility Tests

int UtcDaliScrollViewSetGetVerticalScrollBarVisibilityP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);

  DALI_TEST_EQUALS(scrollView.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetVerticalScrollBarVisibilityNeverP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);

  DALI_TEST_EQUALS(scrollView.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Never, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetVerticalScrollBarVisibilitySetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);

  DALI_TEST_EQUALS(scrollView.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Auto, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetGetHorizontalScrollBarVisibilityP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Always);

  DALI_TEST_EQUALS(scrollView.GetHorizontalScrollBarVisibility(), ScrollBarVisibility::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewSetHorizontalScrollBarVisibilitySetterP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);

  DALI_TEST_EQUALS(scrollView.GetHorizontalScrollBarVisibility(), ScrollBarVisibility::Never, TEST_LOCATION);

  END_TEST;
}

// IsScrolling Tests

int UtcDaliScrollViewIsScrollingInitialP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  DALI_TEST_CHECK(!scrollView.IsScrolling());

  END_TEST;
}

// ScrollTo Tests

int UtcDaliScrollViewScrollToPositionNoAnimP(void)
{
  UiTestApplication application;

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
  UiTestApplication application;

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
  UiTestApplication application;

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
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  ScrollStartedCallback callback;
  scrollView.ScrollStartedSignal().Connect(&callback, &ScrollStartedCallback::OnScrollStarted);

  DALI_TEST_CHECK(!callback.called);
  DALI_TEST_CHECK(scrollView.ScrollStartedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewScrollFinishedSignalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  ScrollFinishedCallback callback;
  scrollView.ScrollFinishedSignal().Connect(&callback, &ScrollFinishedCallback::OnScrollFinished);

  DALI_TEST_CHECK(scrollView.ScrollFinishedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewScrollingSignalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  ScrollingCallback callback;
  scrollView.ScrollingSignal().Connect(&callback, &ScrollingCallback::OnScrolling);

  DALI_TEST_CHECK(scrollView.ScrollingSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDragStartedSignalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  DragStartedCallback callback;
  scrollView.DragStartedSignal().Connect(&callback, &DragStartedCallback::OnDragStarted);

  DALI_TEST_CHECK(scrollView.DragStartedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDragFinishedSignalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  DragFinishedCallback callback;
  scrollView.DragFinishedSignal().Connect(&callback, &DragFinishedCallback::OnDragFinished);

  DALI_TEST_CHECK(scrollView.DragFinishedSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDraggingSignalP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  DraggingCallback callback;
  scrollView.DraggingSignal().Connect(&callback, &DraggingCallback::OnDragging);

  DALI_TEST_CHECK(scrollView.DraggingSignal().GetConnectionCount() > 0u);

  END_TEST;
}

int UtcDaliScrollViewDoesNotFocusTouchFocusableChildWhenDraggingP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);

  View child = View::New();
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);
  child.SetFocusable(true);
  child.SetFocusOnTouchEnabled(true);
  child.TouchEventSignal().Connect([](Actor, TouchEvent)
  { return true; });

  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  DragStartedCallback callback;
  scrollView.DragStartedSignal().Connect(&callback, &DragStartedCallback::OnDragStarted);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 116u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 132u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 100.0f), 148u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 160.0f), 164u));
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 160.0f), 180u));

  DALI_TEST_CHECK(callback.called);
  DALI_TEST_CHECK(FocusManager::Get().GetCurrentFocusView() != child);

  END_TEST;
}

int UtcDaliScrollViewDefersInteractiveChildPressedWhileDisambiguatingP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);

  View child = View::New();
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);

  InteractiveTrait            interactive = child.AsInteractive();
  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  interactive.PressedChangedSignal().Connect(&application, functor);

  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100u));

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(data.trueCount == 0u);
  DALI_TEST_CHECK(!interactive.IsPressed());

  END_TEST;
}

int UtcDaliScrollViewAmbiguousPressDelayExpiresP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);

  View child = View::New();
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);

  InteractiveTrait            interactive = child.AsInteractive();
  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  interactive.PressedChangedSignal().Connect(&application, functor);

  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100u));

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(!interactive.IsPressed());

  Test::EmitGlobalTimerSignal();

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.pressed);
  DALI_TEST_CHECK(data.trueCount == 1u);
  DALI_TEST_CHECK(interactive.IsPressed());

  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 20.0f), 120u));

  DALI_TEST_CHECK(data.falseCount == 1u);
  DALI_TEST_CHECK(!interactive.IsPressed());

  END_TEST;
}

int UtcDaliScrollViewMultiTouchFlushesPendingPressAndPressesNewTouchP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);

  View childA = View::New();
  childA.SetPivot(Pivot::TOP_LEFT);
  childA.SetParentOrigin(ParentOrigin::TOP_LEFT);
  childA.SetRequestedWidth(80.0f);
  childA.SetRequestedHeight(80.0f);

  View childB = View::New();
  childB.SetPivot(Pivot::TOP_LEFT);
  childB.SetParentOrigin(ParentOrigin::TOP_LEFT);
  childB.SetRequestedX(100.0f);
  childB.SetRequestedWidth(80.0f);
  childB.SetRequestedHeight(80.0f);

  InteractiveTrait interactiveA = childA.AsInteractive();
  InteractiveTrait interactiveB = childB.AsInteractive();

  PressedChangedSignalData    dataA;
  PressedChangedSignalFunctor functorA(dataA);
  interactiveA.PressedChangedSignal().Connect(&application, functorA);

  PressedChangedSignalData    dataB;
  PressedChangedSignalFunctor functorB(dataB);
  interactiveB.PressedChangedSignal().Connect(&application, functorB);

  content.Add(childA);
  content.Add(childB);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100u, 4));

  DALI_TEST_CHECK(!dataA.called);
  DALI_TEST_CHECK(!interactiveA.IsPressed());

  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(120.0f, 20.0f), 110u));

  DALI_TEST_CHECK(dataA.called);
  DALI_TEST_CHECK(dataA.pressed);
  DALI_TEST_CHECK(dataA.trueCount == 1u);
  DALI_TEST_CHECK(interactiveA.IsPressed());

  DALI_TEST_CHECK(dataB.called);
  DALI_TEST_CHECK(dataB.pressed);
  DALI_TEST_CHECK(dataB.trueCount == 1u);
  DALI_TEST_CHECK(interactiveB.IsPressed());

  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::UP, Vector2(120.0f, 20.0f), 120u));
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 20.0f), 130u, 4));

  DALI_TEST_CHECK(dataA.falseCount == 1u);
  DALI_TEST_CHECK(!interactiveA.IsPressed());
  DALI_TEST_CHECK(dataB.falseCount == 1u);
  DALI_TEST_CHECK(!interactiveB.IsPressed());

  END_TEST;
}

int UtcDaliScrollViewDoesNotPressInteractiveChildWhenDraggingP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);

  View child = View::New();
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);

  InteractiveTrait            interactive = child.AsInteractive();
  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  interactive.PressedChangedSignal().Connect(&application, functor);

  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  DragStartedCallback callback;
  scrollView.DragStartedSignal().Connect(&callback, &DragStartedCallback::OnDragStarted);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 116u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 132u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 100.0f), 148u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(20.0f, 160.0f), 164u));
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 160.0f), 180u));

  DALI_TEST_CHECK(callback.called);
  DALI_TEST_CHECK(data.trueCount == 0u);
  DALI_TEST_CHECK(!interactive.IsPressed());

  END_TEST;
}

int UtcDaliScrollViewPressesInteractiveChildOnTapReleaseP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);

  View child = View::New();
  child.SetPivot(Pivot::TOP_LEFT);
  child.SetParentOrigin(ParentOrigin::TOP_LEFT);
  child.SetRequestedWidth(100.0f);
  child.SetRequestedHeight(100.0f);

  InteractiveTrait            interactive = child.AsInteractive();
  PressedChangedSignalData    data;
  PressedChangedSignalFunctor functor(data);
  interactive.PressedChangedSignal().Connect(&application, functor);

  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);

  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100u));

  DALI_TEST_CHECK(!data.called);
  DALI_TEST_CHECK(!interactive.IsPressed());

  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(20.0f, 20.0f), 120u));

  DALI_TEST_CHECK(data.trueCount == 1u);
  DALI_TEST_CHECK(data.falseCount == 0u);
  DALI_TEST_CHECK(interactive.IsPressed());

  Test::EmitGlobalTimerSignal();

  DALI_TEST_CHECK(data.falseCount == 1u);
  DALI_TEST_CHECK(!interactive.IsPressed());

  END_TEST;
}

// Setter Tests

int UtcDaliScrollViewSettersP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();
  View       content    = View::New();

  scrollView.SetContent(content);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetMaxFlingDistance(5000.0f);
  scrollView.SetMinimumFlingDuration(800);
  scrollView.SetMaximumFlingDuration(2500);
  scrollView.SetFlingSensitivity(1.2f);
  scrollView.SetDecelerationRate(0.99f);
  scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);

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

int UtcDaliScrollViewFocusAndKeyScrollPropertiesP(void)
{
  UiTestApplication application;
  ScrollView        scrollView = ScrollView::New();

  scrollView.SetScrollOnFocus(false);
  DALI_TEST_CHECK(!scrollView.GetScrollOnFocus());
  scrollView.SetScrollOnFocus(true);
  DALI_TEST_CHECK(scrollView.GetScrollOnFocus());

  scrollView.SetFocusScrollToPosition(ScrollToPosition::Center);
  DALI_TEST_EQUALS(scrollView.GetFocusScrollToPosition(), ScrollToPosition::Center, TEST_LOCATION);
  scrollView.SetFocusScrollPeek(-5.0f);
  DALI_TEST_EQUALS(scrollView.GetFocusScrollPeek(), 0.0f, TEST_LOCATION);
  scrollView.SetFocusScrollPeek(24.0f);
  DALI_TEST_EQUALS(scrollView.GetFocusScrollPeek(), 24.0f, TEST_LOCATION);

  scrollView.SetKeyScrollEnabled(true);
  DALI_TEST_CHECK(scrollView.IsKeyScrollEnabled());
  scrollView.SetKeyScrollStep(0.0f);
  DALI_TEST_EQUALS(scrollView.GetKeyScrollStep(), 1.0f, TEST_LOCATION);
  scrollView.SetKeyScrollStep(75.0f);
  DALI_TEST_EQUALS(scrollView.GetKeyScrollStep(), 75.0f, TEST_LOCATION);
  scrollView.SetKeyScrollEnabled(false);
  DALI_TEST_CHECK(!scrollView.IsKeyScrollEnabled());
  END_TEST;
}

int UtcDaliScrollViewKeyNavigationSelectsVisibleContentP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetKeyScrollEnabled(true);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);

  View content = View::New();
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(800.0f);

  View first = View::New();
  first.SetRequestedWidth(40.0f);
  first.SetRequestedHeight(40.0f);
  first.SetRequestedY(20.0f);
  first.SetProperty(Actor::Property::FOCUSABLE, true);

  View middle = View::New();
  middle.SetRequestedWidth(40.0f);
  middle.SetRequestedHeight(40.0f);
  middle.SetRequestedY(240.0f);
  middle.SetProperty(Actor::Property::FOCUSABLE, true);

  View last = View::New();
  last.SetRequestedWidth(40.0f);
  last.SetRequestedHeight(40.0f);
  last.SetRequestedY(700.0f);
  last.SetProperty(Actor::Property::FOCUSABLE, true);

  content.Add(first);
  content.Add(middle);
  content.Add(last);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();

  auto& impl = GetScrollImpl(scrollView);
  DALI_TEST_CHECK(impl.IsDirectionCompatible(FocusDirection::DOWN));
  DALI_TEST_CHECK(!impl.IsDirectionCompatible(FocusDirection::RIGHT));
  DALI_TEST_EQUALS(impl.FindNextFocusableInContent(View(), Vector2::ZERO, FocusDirection::DOWN), first, TEST_LOCATION);
  DALI_TEST_CHECK(impl.IsChildInViewport(first));
  DALI_TEST_CHECK(!impl.IsChildInViewport(middle));
  DALI_TEST_EQUALS(impl.PageScrollAndFocus(FocusDirection::PAGE_DOWN), middle, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FullScrollAndFocus(true), last, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FullScrollAndFocus(false), first, TEST_LOCATION);

  middle.SetProperty(Actor::Property::ENABLED, false);
  DALI_TEST_EQUALS(impl.FindNextFocusableInContent(first, Vector2(0.0f, 20.0f), FocusDirection::DOWN), last, TEST_LOCATION);
  scrollView.ScrollToY(600.0f, false);
  DALI_TEST_CHECK(impl.IsAtScrollBoundary(FocusDirection::DOWN));
  DALI_TEST_CHECK(!impl.IsAtScrollBoundary(FocusDirection::UP));
  scrollView.ScrollToY(0.0f, false);
  DALI_TEST_CHECK(impl.IsAtScrollBoundary(FocusDirection::UP));

  END_TEST;
}

int UtcDaliScrollViewKeyboardScrollingWithoutFocusableContentP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  View content = View::New();
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetScrollDirection(ScrollDirection::Both);
  scrollView.SetKeyScrollEnabled(true);
  scrollView.SetKeyScrollStep(60.0f);
  scrollView.SetProperty(Actor::Property::FOCUSABLE, true);
  content.SetRequestedWidth(600.0f);
  content.SetRequestedHeight(600.0f);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();

  auto& impl = GetScrollImpl(scrollView);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(scrollView));
  DALI_TEST_EQUALS(FocusManager::Get().GetCurrentFocusView(), scrollView, TEST_LOCATION);

  KeyEvent key = KeyEvent::New();
  key.SetKeyName("Down");
  key.SetState(KeyEvent::UP);
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));
  key.SetState(KeyEvent::DOWN);
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  DALI_TEST_CHECK(scrollView.IsScrolling());
  impl.CancelScrollAnimation();

  key.SetKeyName("Right");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  impl.CancelScrollAnimation();
  key.SetKeyName("Next");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  impl.CancelScrollAnimation();
  key.SetKeyName("Prior");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  impl.CancelScrollAnimation();
  key.SetKeyName("End");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  impl.CancelScrollAnimation();
  key.SetKeyName("Home");
  DALI_TEST_CHECK(impl.OnKeyEvent(key));
  impl.CancelScrollAnimation();
  key.SetKeyName("Unmapped");
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));

  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  key.SetKeyName("Left");
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));
  scrollView.SetKeyScrollEnabled(false);
  key.SetKeyName("Down");
  DALI_TEST_CHECK(!impl.OnKeyEvent(key));
  FocusManager::Get().ClearFocus();
  END_TEST;
}

int UtcDaliScrollViewScrollToChildVariantsP(void)
{
  UiTestApplication application;
  ScrollView        scrollView = ScrollView::New();
  View              content    = View::New();
  View              child      = View::New();

  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(160.0f);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  content.SetRequestedWidth(600.0f);
  content.SetRequestedHeight(500.0f);
  child.SetRequestedWidth(60.0f);
  child.SetRequestedHeight(40.0f);
  child.SetRequestedX(420.0f);
  child.SetRequestedY(360.0f);
  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();

  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.ScrollTo(child, false, ScrollToPosition::Start);
  scrollView.ScrollTo(child, false, ScrollToPosition::Center);
  scrollView.ScrollTo(child, false, ScrollToPosition::End);
  scrollView.SetFocusScrollPeek(12.0f);
  scrollView.ScrollTo(child, false, ScrollToPosition::MakeVisible);

  scrollView.SetScrollDirection(ScrollDirection::Horizontal);
  scrollView.ScrollTo(child, false, ScrollToPosition::MakeVisible);

  scrollView.SetScrollDirection(ScrollDirection::Both);
  scrollView.ScrollTo(Vector2::ZERO, false);
  scrollView.ScrollTo(child, false, ScrollToPosition::MakeVisible);
  DALI_TEST_CHECK(scrollView.GetScrollPosition().x > 0.0f);
  DALI_TEST_CHECK(scrollView.GetScrollPosition().y > 0.0f);

  scrollView.SetMinimumFlingDuration(10);
  scrollView.SetMaximumFlingDuration(20);
  scrollView.ScrollTo(Vector2(100.0f, 100.0f), true);
  DALI_TEST_CHECK(scrollView.IsScrolling());
  application.SendNotification();
  application.Render(30u);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!scrollView.IsScrolling());

  scrollView.SetProperty(Actor::Property::VISIBLE, false);
  scrollView.ScrollTo(Vector2(30.0f, 40.0f), true);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition(), Vector2(30.0f, 40.0f), TEST_LOCATION);
  END_TEST;
}

// View Inheritance Test

int UtcDaliScrollViewIsViewP(void)
{
  UiTestApplication application;

  ScrollView scrollView = ScrollView::New();

  // ScrollView should be usable as a View
  View view = scrollView;
  DALI_TEST_CHECK(view);

  END_TEST;
}

// ---------------------------------------------------------------------------
// Phase 5c: ScrollViewLayoutManager must stay ALWAYS.
//
// ScrollViewLayoutManager::Arrange takes the scrolled child's CURRENT actor position
// as that child's arrange input (childBounds.x = child.GetPositionX() * s, and the
// same for y). That read is how a scroll survives a layout pass: scrolling is applied
// with Ui::Extension::SetPositionX/Y on the content (ScrollViewImpl::ApplyScrollPosition),
// which writes the actor property without invalidating layout.
//
// The manager therefore explicitly selects ALWAYS. If it instead selected
// IF_CHANGED, a settled ScrollView
// would serve its subtree from the arrange cache and re-apply the bounds published
// before the scroll -- the content would snap back and scrolling would freeze.
//
// The scroll bars are switched off on purpose. A visible bar re-lays itself out on
// every scroll (ScrollBarImpl::SetVBarBounds -> SetLayoutParams -> InvalidateMeasure),
// which would leave the ScrollView's subtree dirty and force a miss for a reason that
// has nothing to do with the manager -- and would make the mutation below invisible.
// With the bars off, the ScrollView's whole subtree is cacheable and the manager's own
// declaration is the only thing standing between the content and a stale replay.
//
// Non-vacuity (verified by mutation): adding
//   SetArrangePolicy(ArrangePolicy::IF_CHANGED);
// to ScrollViewLayoutManager's constructor makes the settled ScrollView hit, the
// content snaps back to 0 and every assertion after the scroll fails.
int UtcDaliScrollViewScrolledContentSurvivesSettledLayoutPassP(void)
{
  UiTestApplication application;
  tet_infoline("A settled ScrollView still re-places its scrolled content on a later layout pass");

  View root = View::New();
  root.SetRequestedWidth(400.0f);
  root.SetRequestedHeight(400.0f);
  application.GetScene().Add(root);

  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetRequestedWidth(300.0f);
  scrollView.SetRequestedHeight(300.0f);
  root.Add(scrollView);

  View content = View::New();
  content.SetRequestedWidth(300.0f);
  content.SetRequestedHeight(1000.0f);
  scrollView.SetContent(content);

  // The reason a later pass happens at all: a sibling of the ScrollView, so its
  // invalidation walks up to the root and never touches the ScrollView.
  View sibling = View::New();
  sibling.SetRequestedWidth(30.0f);
  sibling.SetRequestedHeight(30.0f);
  root.Add(sibling);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(content.GetProperty<float>(Actor::Property::POSITION_Y), 0.0f, TEST_LOCATION);

  // Scroll. This moves the content behind layout's back, exactly as a pan would.
  scrollView.ScrollToY(120.0f, false);

  const float scrolledY = content.GetProperty<float>(Actor::Property::POSITION_Y);
  DALI_TEST_EQUALS(scrolledY, -120.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition().y, 120.0f, TEST_LOCATION);

  // A layout pass sweeps past the settled ScrollView.
  sibling.SetRequestedX(11.0f);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  // The pass really happened...
  DALI_TEST_EQUALS(sibling.GetProperty<float>(Actor::Property::POSITION_X), 11.0f, TEST_LOCATION);

  // ...and the scroll survived it.
  DALI_TEST_EQUALS(content.GetProperty<float>(Actor::Property::POSITION_Y), scrolledY, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition().y, 120.0f, TEST_LOCATION);

  // Idle frames, and a second scroll, so this is not a one-pass accident.
  for(int i = 0; i < 3; ++i)
  {
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(content.GetProperty<float>(Actor::Property::POSITION_Y), scrolledY, TEST_LOCATION);
  }

  scrollView.ScrollToY(200.0f, false);
  const float scrolledAgainY = content.GetProperty<float>(Actor::Property::POSITION_Y);
  DALI_TEST_EQUALS(scrolledAgainY, -200.0f, TEST_LOCATION);

  sibling.SetRequestedX(12.0f);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(sibling.GetProperty<float>(Actor::Property::POSITION_X), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(content.GetProperty<float>(Actor::Property::POSITION_Y), scrolledAgainY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollViewInternalMathP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(300.0f);
  View content = View::New();
  content.SetRequestedWidth(600.0f);
  content.SetRequestedHeight(900.0f);
  View child = View::New();
  child.SetProperty(Actor::Property::POSITION, Vector3(50.0f, 100.0f, 0.0f));
  View grandchild = View::New();
  grandchild.SetProperty(Actor::Property::POSITION, Vector3(10.0f, 20.0f, 0.0f));
  child.Add(grandchild);
  content.Add(child);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();
  auto& impl = GetScrollImpl(scrollView);
  Dali::Ui::Extension::View::SetPositionX(child, 50.0f);
  Dali::Ui::Extension::View::SetPositionY(child, 100.0f);
  Dali::Ui::Extension::View::SetPositionX(grandchild, 10.0f);
  Dali::Ui::Extension::View::SetPositionY(grandchild, 20.0f);
  impl.mViewportWidth = 200.0f;
  impl.mViewportHeight = 300.0f;
  impl.mScrollableWidth = 600.0f;
  impl.mScrollableHeight = 900.0f;
  impl.mMinimumStartX = -400.0f;
  impl.mMinimumStartY = -600.0f;

  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  DALI_TEST_EQUALS(impl.AdjustMovement(Vector2(20.0f, 10.0f)), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.AdjustMovement(Vector2(10.0f, 20.0f)), Vector2(0.0f, 20.0f), TEST_LOCATION);
  scrollView.SetScrollDirection(ScrollDirection::Horizontal);
  DALI_TEST_EQUALS(impl.AdjustMovement(Vector2(20.0f, 10.0f)), Vector2(20.0f, 0.0f), TEST_LOCATION);
  scrollView.SetScrollDirection(ScrollDirection::Both);
  DALI_TEST_EQUALS(impl.AdjustMovement(Vector2(20.0f, 10.0f)), Vector2(20.0f, 10.0f), TEST_LOCATION);

  DALI_TEST_EQUALS(impl.AdjustDelta(Vector2(500.0f, 700.0f), Vector2(-100.0f, -100.0f)), Vector2(100.0f, 100.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.AdjustDelta(Vector2(-500.0f, -700.0f), Vector2(-100.0f, -100.0f)), Vector2(-300.0f, -500.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.AdjustScrollPosition(Vector2(-10.0f, 1000.0f)), Vector2(0.0f, 600.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetScrollPositionForChild(content, Vector2(1.0f, 2.0f)), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetScrollPositionForChild(child, Vector2::ZERO), Vector2(50.0f, 100.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetScrollPositionForChild(grandchild, Vector2::ZERO), Vector2(60.0f, 120.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetScrollPositionForChild(View::New(), Vector2(3.0f, 4.0f)), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.ContentPositionToScrollPosition(Vector2(-20.0f, -30.0f)), Vector2(20.0f, 30.0f), TEST_LOCATION);
  impl.mScrollPosition = Vector2(100.0f, 200.0f);
  DALI_TEST_EQUALS(impl.DeltaFromScrollPosition(Vector2(30.0f, 50.0f)), Vector2(70.0f, 150.0f), TEST_LOCATION);

  impl.mVelocityTracker.Clear();
  DALI_TEST_EQUALS(impl.mVelocityTracker.Compute(), Vector2::ZERO, TEST_LOCATION);
  impl.mVelocityTracker.Add(0u, Vector2::ZERO);
  impl.mVelocityTracker.Add(20u, Vector2(20.0f, 40.0f));
  impl.mVelocityTracker.Add(40u, Vector2(40.0f, 80.0f));
  DALI_TEST_EQUALS(impl.mVelocityTracker.Compute(), Vector2(1.0f, 2.0f), 0.01f, TEST_LOCATION);
  impl.mVelocityTracker.Add(500u, Vector2(40.0f, 80.0f));
  DALI_TEST_EQUALS(impl.mVelocityTracker.Compute(), Vector2::ZERO, TEST_LOCATION);
  END_TEST;
}

int UtcDaliScrollViewInternalStateAndSignalsP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(300.0f);
  View content = View::New();
  content.SetRequestedWidth(600.0f);
  content.SetRequestedHeight(900.0f);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();
  auto& impl = GetScrollImpl(scrollView);

  int scrollStarted = 0;
  int scrollFinished = 0;
  int scrolling = 0;
  int dragStarted = 0;
  int dragging = 0;
  int dragFinished = 0;
  impl.ScrollStartedSignal().Connect(&application, [&scrollStarted](ScrollView) { ++scrollStarted; });
  impl.ScrollFinishedSignal().Connect(&application, [&scrollFinished](ScrollView) { ++scrollFinished; });
  impl.ScrollingSignal().Connect(&application, [&scrolling](ScrollView) { ++scrolling; });
  impl.DragStartedSignal().Connect(&application, [&dragStarted](ScrollView) { ++dragStarted; });
  impl.DraggingSignal().Connect(&application, [&dragging](ScrollView, float, float) { ++dragging; });
  impl.DragFinishedSignal().Connect(&application, [&dragFinished](ScrollView) { ++dragFinished; });
  impl.SendScrollStarted();
  impl.SendScrollStarted();
  impl.SendScrolling();
  impl.SendDragStarted();
  impl.SendDragging(1.0f, 2.0f);
  impl.SendDragFinished();
  impl.SendScrollFinished();
  DALI_TEST_EQUALS(scrollStarted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollFinished, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(scrolling, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragStarted, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragging, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(dragFinished, 1, TEST_LOCATION);

  scrollView.SetMaxFlingDistance(10.0f);
  scrollView.SetFlingSensitivity(2.0f);
  scrollView.SetDecelerationRate(0.9f);
  scrollView.SetMaximumFlingDuration(1000);
  DALI_TEST_EQUALS(impl.VelocityToMovement(Vector2(10000.0f, -10000.0f)), Vector2(10.0f, -10.0f), 0.01f, TEST_LOCATION);

  scrollView.SetPanScrollEnabled(false);
  scrollView.SetPanScrollEnabled(false);
  DALI_TEST_CHECK(!scrollView.IsPanScrollEnabled());
  scrollView.SetPanScrollEnabled(true);
  DALI_TEST_CHECK(scrollView.IsPanScrollEnabled());
  scrollView.SetKeyScrollEnabled(true);
  scrollView.SetKeyScrollStep(-5.0f);
  scrollView.SetFocusScrollPeek(-5.0f);
  scrollView.SetScrollOnFocus(false);
  DALI_TEST_CHECK(scrollView.IsKeyScrollEnabled());
  DALI_TEST_EQUALS(scrollView.GetKeyScrollStep(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(scrollView.GetFocusScrollPeek(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!scrollView.GetScrollOnFocus());

  impl.UpdateScrollingProperties();
  impl.ApplyScrollPosition(Vector2(20.0f, 30.0f));
  impl.ScrollToWithDuration(Vector2(20.0f, 30.0f), 0.1f);
  impl.ScrollToWithDuration(Vector2(100.0f, 200.0f), 0.1f);
  DALI_TEST_CHECK(impl.mScrollAnimation);
  impl.CancelScrollAnimation();
  impl.OnScrollAnimationFinished(Animation());
  impl.SetContent(View());
  impl.UpdateScrollingProperties();
  impl.ApplyScrollPosition(Vector2::ZERO);
  impl.ScrollToWithDuration(Vector2::ZERO, 0.1f);
  END_TEST;
}

int UtcDaliPageScrollViewPublicPagingAndHandlesP(void)
{
  UiTestApplication application(UiConfig::New());
  PageScrollView pager = PageScrollView::New();
  DALI_TEST_CHECK(pager);
  DALI_TEST_CHECK(PageScrollView::DownCast(pager));
  DALI_TEST_CHECK(!PageScrollView::DownCast(View::New()));

  PageScrollView copied(pager);
  PageScrollView moved(std::move(copied));
  PageScrollView assigned;
  assigned = moved;
  DALI_TEST_CHECK(assigned == pager);
  PageScrollView moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_CHECK(moveAssigned == pager);

  pager.SetContent(View::New());
  pager.SetScrollDirection(ScrollDirection::Horizontal);
  pager.SetPageSize(Vector2(100.0f, 80.0f));
  auto& impl = static_cast<Dali::Ui::Integration::PageScrollViewImpl&>(pager.GetImplementation());
  impl.SetScrollableWidth(500.0f);
  DALI_TEST_EQUALS(pager.GetPageSize(), Vector2(100.0f, 80.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 0, TEST_LOCATION);

  int pageChanges = 0;
  pager.PageChangedSignal().Connect(&application, [&pageChanges](int, int) { ++pageChanges; });
  DALI_TEST_CHECK(pager.DestroyingSignal().Empty());
  pager.ScrollToPage(2, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(pager.GetCurrentPage(), 2, TEST_LOCATION);
  DALI_TEST_CHECK(pageChanges > 0);

  pager.NotifyPagesInserted(1, 2);
  DALI_TEST_EQUALS(pager.GetPageCount(), 7, TEST_LOCATION);
  pager.NotifyPagesRemoved(1, 2);
  DALI_TEST_EQUALS(pager.GetPageCount(), 5, TEST_LOCATION);
  END_TEST;
}

int UtcDaliScrollViewEdgeEffectsAndWheelP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  auto& impl = GetScrollImpl(scrollView);
  DALI_TEST_CHECK(impl.HasIntrinsicWheelHandling());

  WheelEvent vertical = WheelEvent::New(WheelEvent::MOUSE_WHEEL, 0, 0u, Vector2(50.0f, 50.0f), 1, 100u);
  DALI_TEST_CHECK(!impl.OnWheelEvent(vertical));

  EdgeEffect start = EdgeEffect::New();
  EdgeEffect end = EdgeEffect::New();
  scrollView.SetStartEdgeEffect(start);
  scrollView.SetEndEdgeEffect(end);
  DALI_TEST_CHECK(scrollView.GetStartEdgeEffect() == start);
  DALI_TEST_CHECK(scrollView.GetEndEdgeEffect() == end);

  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Both);
  View content = View::New();
  content.SetRequestedWidth(600.0f);
  content.SetRequestedHeight(600.0f);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(impl.OnWheelEvent(vertical));
  impl.CancelScrollAnimation();

  WheelEvent horizontal = WheelEvent::New(WheelEvent::MOUSE_WHEEL, 1, 0u, Vector2(50.0f, 50.0f), 1, 200u);
  DALI_TEST_CHECK(impl.OnWheelEvent(horizontal));
  impl.CancelScrollAnimation();

  scrollView.SetScrollDirection(ScrollDirection::Horizontal);
  DALI_TEST_CHECK(impl.OnWheelEvent(vertical));
  impl.CancelScrollAnimation();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  DALI_TEST_CHECK(!impl.OnWheelEvent(horizontal));

  scrollView.SetStartEdgeEffect(EdgeEffect());
  scrollView.SetEndEdgeEffect(EdgeEffect());
  DALI_TEST_CHECK(!scrollView.GetStartEdgeEffect());
  DALI_TEST_CHECK(!scrollView.GetEndEdgeEffect());
  END_TEST;
}

int UtcDaliScrollViewFocusNavigationThroughContentP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetKeyScrollEnabled(true);
  scrollView.SetKeyScrollStep(70.0f);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetProperty(Actor::Property::FOCUSABLE, true);

  View content = View::New();
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(700.0f);
  View first = View::New();
  first.SetRequestedWidth(40.0f);
  first.SetRequestedHeight(40.0f);
  first.SetRequestedY(20.0f);
  first.SetProperty(Actor::Property::FOCUSABLE, true);
  View second = View::New();
  second.SetRequestedWidth(40.0f);
  second.SetRequestedHeight(40.0f);
  second.SetRequestedY(140.0f);
  second.SetProperty(Actor::Property::FOCUSABLE, true);
  View third = View::New();
  third.SetRequestedWidth(40.0f);
  third.SetRequestedHeight(40.0f);
  third.SetRequestedY(520.0f);
  third.SetProperty(Actor::Property::FOCUSABLE, true);
  content.Add(first);
  content.Add(second);
  content.Add(third);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();

  auto& impl = GetScrollImpl(scrollView);
  DALI_TEST_EQUALS(impl.OnFocusRequested(), scrollView, TEST_LOCATION);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(first));
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::DOWN));
  DALI_TEST_EQUALS(FocusManager::Get().GetCurrentFocusView(), second, TEST_LOCATION);
  FocusManager::Get().MoveFocus(FocusDirection::DOWN);
  DALI_TEST_CHECK(scrollView.IsScrolling() || FocusManager::Get().GetCurrentFocusView() == third);
  impl.CancelScrollAnimation();

  scrollView.SetKeyScrollEnabled(false);
  DALI_TEST_CHECK(impl.OnFocusRequested());
  FocusManager::Get().ClearFocus();
  END_TEST;
}

int UtcDaliScrollViewHorizontalFocusNavigationP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Horizontal);
  scrollView.SetKeyScrollEnabled(true);
  scrollView.SetKeyScrollStep(70.0f);
  scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(120.0f);
  scrollView.SetFocusable(true);

  View content = View::New();
  content.SetRequestedWidth(650.0f);
  content.SetRequestedHeight(120.0f);
  View first = View::New();
  first.SetRequestedWidth(40.0f);
  first.SetRequestedHeight(40.0f);
  first.SetRequestedX(20.0f);
  first.SetFocusable(true);
  View second = View::New();
  second.SetRequestedWidth(40.0f);
  second.SetRequestedHeight(40.0f);
  second.SetRequestedX(140.0f);
  second.SetFocusable(true);
  View last = View::New();
  last.SetRequestedWidth(40.0f);
  last.SetRequestedHeight(40.0f);
  last.SetRequestedX(520.0f);
  last.SetFocusable(true);
  content.Add(first);
  content.Add(second);
  content.Add(last);
  scrollView.SetContent(content);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();

  auto& impl = GetScrollImpl(scrollView);
  DALI_TEST_CHECK(impl.IsDirectionCompatible(FocusDirection::RIGHT));
  DALI_TEST_CHECK(!impl.IsDirectionCompatible(FocusDirection::DOWN));
  DALI_TEST_EQUALS(impl.FindNextFocusableInContent(View(), Vector2::ZERO, FocusDirection::RIGHT), first, TEST_LOCATION);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(first));
  DALI_TEST_CHECK(FocusManager::Get().MoveFocus(FocusDirection::RIGHT));
  DALI_TEST_EQUALS(FocusManager::Get().GetCurrentFocusView(), second, TEST_LOCATION);
  scrollView.ScrollToX(450.0f, false);
  DALI_TEST_CHECK(impl.IsChildInViewport(last));
  DALI_TEST_CHECK(impl.IsAtScrollBoundary(FocusDirection::RIGHT));
  FocusManager::Get().ClearFocus();
  END_TEST;
}

int UtcDaliScrollViewEdgePullReverseAndReleaseP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetPivot(Pivot::TOP_LEFT);
  scrollView.SetParentOrigin(ParentOrigin::TOP_LEFT);
  scrollView.SetRequestedWidth(200.0f);
  scrollView.SetRequestedHeight(200.0f);
  scrollView.SetScrollDirection(ScrollDirection::Vertical);

  View content = View::New();
  content.SetPivot(Pivot::TOP_LEFT);
  content.SetParentOrigin(ParentOrigin::TOP_LEFT);
  content.SetRequestedWidth(200.0f);
  content.SetRequestedHeight(600.0f);
  scrollView.SetContent(content);
  BounceEdgeEffect start = BounceEdgeEffect::New(ScrollDirection::Vertical);
  BounceEdgeEffect end = BounceEdgeEffect::New(ScrollDirection::Vertical);
  scrollView.SetStartEdgeEffect(start);
  scrollView.SetEndEdgeEffect(end);
  application.GetScene().Add(scrollView);
  application.SendNotification();
  application.Render();

  auto& impl = GetScrollImpl(scrollView);
  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(100.0f, 40.0f), 100u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 80.0f), 116u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 120.0f), 132u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 160.0f), 148u));
  DALI_TEST_CHECK(impl.mStartEdgeActive);
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 80.0f), 164u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 40.0f), 180u));
  DALI_TEST_CHECK(!impl.mStartEdgeActive);
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(100.0f, 40.0f), 196u));
  impl.CancelScrollAnimation();
  start.Finish();
  end.Finish();

  scrollView.ScrollToY(400.0f, false);
  DALI_TEST_EQUALS(scrollView.GetScrollPosition().y, 400.0f, TEST_LOCATION);
  application.ProcessEvent(GenerateTouch(PointState::DOWN, Vector2(100.0f, 160.0f), 300u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 120.0f), 316u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 80.0f), 332u));
  application.ProcessEvent(GenerateTouch(PointState::MOTION, Vector2(100.0f, 40.0f), 348u));
  DALI_TEST_CHECK(impl.mEndEdgeActive);
  application.ProcessEvent(GenerateTouch(PointState::UP, Vector2(100.0f, 40.0f), 364u));
  DALI_TEST_CHECK(!impl.mEndEdgeActive);
  impl.CancelScrollAnimation();
  start.Finish();
  end.Finish();
  END_TEST;
}

int UtcDaliScrollViewFocusEntryUsesVisibleEdgeItemsP(void)
{
  UiTestApplication application;
  ScrollView scrollView = ScrollView::New();
  scrollView.SetScrollDirection(ScrollDirection::Vertical);
  scrollView.SetKeyScrollEnabled(true);
  scrollView.SetRequestedX(100.0f);
  scrollView.SetRequestedY(300.0f);
  scrollView.SetRequestedWidth(180.0f);
  scrollView.SetRequestedHeight(180.0f);
  scrollView.SetFocusable(true);

  View content = View::New();
  content.SetRequestedWidth(180.0f);
  content.SetRequestedHeight(500.0f);
  View first = View::New();
  first.SetRequestedWidth(50.0f);
  first.SetRequestedHeight(40.0f);
  first.SetRequestedY(20.0f);
  first.SetFocusable(true);
  View last = View::New();
  last.SetRequestedWidth(50.0f);
  last.SetRequestedHeight(40.0f);
  last.SetRequestedY(390.0f);
  last.SetFocusable(true);
  content.Add(first);
  content.Add(last);
  scrollView.SetContent(content);

  View above = View::New();
  above.SetRequestedX(100.0f);
  above.SetRequestedY(20.0f);
  above.SetRequestedWidth(40.0f);
  above.SetRequestedHeight(40.0f);
  above.SetFocusable(true);
  View below = View::New();
  below.SetRequestedX(100.0f);
  below.SetRequestedY(700.0f);
  below.SetRequestedWidth(40.0f);
  below.SetRequestedHeight(40.0f);
  below.SetFocusable(true);

  application.GetScene().Add(scrollView);
  application.GetScene().Add(above);
  application.GetScene().Add(below);
  application.SendNotification();
  application.Render();

  auto& impl = GetScrollImpl(scrollView);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(above));
  DALI_TEST_EQUALS(impl.OnFocusRequested(), first, TEST_LOCATION);

  scrollView.ScrollToY(320.0f, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(below));
  DALI_TEST_EQUALS(impl.OnFocusRequested(), last, TEST_LOCATION);

  first.SetFocusable(false);
  last.SetFocusable(false);
  DALI_TEST_CHECK(FocusManager::Get().SetCurrentFocusView(above));
  DALI_TEST_EQUALS(impl.OnFocusRequested(), scrollView, TEST_LOCATION);
  FocusManager::Get().ClearFocus();
  END_TEST;
}

int UtcDaliBounceEdgeEffectPropertiesAndDownCastP(void)
{
  UiTestApplication application;
  BounceEdgeEffect effect = BounceEdgeEffect::New(ScrollDirection::Horizontal);
  DALI_TEST_EQUALS(effect.GetAxis(), ScrollDirection::Horizontal, TEST_LOCATION);

  EdgeEffect base = effect;
  BounceEdgeEffect downcast = BounceEdgeEffect::DownCast(base);
  DALI_TEST_CHECK(downcast);
  DALI_TEST_EQUALS(downcast.GetAxis(), ScrollDirection::Horizontal, TEST_LOCATION);
  DALI_TEST_EQUALS(BounceEdgeEffect::DownCast(EdgeEffect()), BounceEdgeEffect(), TEST_LOCATION);

  effect.SetPullResistance(0.75f).SetBounceDuration(0.4f);
  DALI_TEST_EQUALS(effect.GetPullResistance(), 0.75f, TEST_LOCATION);
  DALI_TEST_EQUALS(effect.GetBounceDuration(), 0.4f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliEdgeEffectStateTransitionsAndSignalsP(void)
{
  UiTestApplication application;
  EdgeEffect effect = EdgeEffect::New();
  View source = View::New();
  effect.SetSource(source);
  DALI_TEST_CHECK(effect.GetSource() == source);
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::IDLE);

  effect.OnRelease();
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::IDLE);
  effect.OnPull(2.0f, 6.0f);
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::PULL);
  effect.OnPull(3.0f, 9.0f);
  effect.OnRelease();
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::RECEDE);
  effect.OnAbsorb(5.0f);
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::ABSORB);
  effect.OnAbsorb(6.0f);
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::ABSORB);
  effect.Finish();
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::IDLE);
  effect.OnAbsorb(7.0f);
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::ABSORB);
  effect.OnPull(1.0f, 1.0f);
  DALI_TEST_CHECK(effect.GetState() == EdgeEffect::State::PULL);
  effect.Finish();

  DALI_TEST_CHECK(&effect.PullSignal());
  DALI_TEST_CHECK(&effect.ReleaseSignal());
  DALI_TEST_CHECK(&effect.AbsorbSignal());
  DALI_TEST_CHECK(&effect.FinishedSignal());
  END_TEST;
}
