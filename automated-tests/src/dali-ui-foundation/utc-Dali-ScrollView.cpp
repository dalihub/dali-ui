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
#include <dali/integration-api/events/touch-event-integ.h>
#include <stdlib.h>
#include <iostream>

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
