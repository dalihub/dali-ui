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
#include <dali-ui-test-suite-utils.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/input-options.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
/**
 * @brief Helper to create a focused interactive View, so that it receives key events.
 */
View CreateFocusedInteractiveView(TestApplication& application)
{
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  view.AsInteractive();

  FocusManager::Get().SetCurrentFocusView(view);

  application.SendNotification();
  application.Render();

  return view;
}

/**
 * @brief Helper to send a single "Return" key down event.
 */
void SendReturnKeyDown(TestApplication& application, uint32_t time)
{
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, time, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);
}

} // unnamed namespace

int UtcDaliUiConfigGestureOptionsP(void)
{
  UiConfig config = UiConfig::New();

  config.SetLongPressGestureMinimumHoldingTime(700u);
  config.SetTapGestureMaximumMultiTapInterval(400u);
  config.SetTapGestureMaximumHoldingTime(250u);
  config.SetTapGestureMaximumMotionDistance(35.0f);

  // The getters report the requested values before the application is created
  DALI_TEST_EQUALS(config.GetLongPressGestureMinimumHoldingTime(), 700u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMultiTapInterval(), 400u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumHoldingTime(), 250u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMotionDistance(), 35.0f, TEST_LOCATION);

  UiTestApplication application(config);

  // The values have been applied to DALi once the application is created
  DALI_TEST_EQUALS(Dali::Integration::GetLongPressGestureMinimumHoldingTime(), 700u, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Integration::GetTapGestureMaximumMultiTapInterval(), 400u, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Integration::GetTapGestureMaximumHoldingTime(), 250u, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Integration::GetTapGestureMaximumMotionDistance(), 35.0f, TEST_LOCATION);

  // The getters still report the same values after they have been applied
  DALI_TEST_EQUALS(config.GetLongPressGestureMinimumHoldingTime(), 700u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMultiTapInterval(), 400u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumHoldingTime(), 250u, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMotionDistance(), 35.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiConfigGestureOptionsDefault(void)
{
  UiConfig config = UiConfig::New();

  // Nothing is set, so the getters report the DALi defaults
  DALI_TEST_EQUALS(config.GetLongPressGestureMinimumHoldingTime(), Dali::Integration::DEFAULT_LONG_PRESS_GESTURE_MINIMUM_HOLDING_TIME, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMultiTapInterval(), Dali::Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MULTI_TAP_INTERVAL, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumHoldingTime(), Dali::Integration::DEFAULT_TAP_GESTURE_MAXIMUM_HOLDING_TIME, TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMotionDistance(), Dali::Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MOTION_DISTANCE, TEST_LOCATION);

  UiTestApplication application(config);

  // The DALi values are kept as they are, and the getters report the effective values
  DALI_TEST_EQUALS(config.GetLongPressGestureMinimumHoldingTime(), Dali::Integration::GetLongPressGestureMinimumHoldingTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMultiTapInterval(), Dali::Integration::GetTapGestureMaximumMultiTapInterval(), TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumHoldingTime(), Dali::Integration::GetTapGestureMaximumHoldingTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(config.GetTapGestureMaximumMotionDistance(), Dali::Integration::GetTapGestureMaximumMotionDistance(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiConfigGestureOptionsFrozenN(void)
{
  UiConfig config = UiConfig::New();

  UiTestApplication application(config);

  // The config is frozen by UiConfig::Apply(), so the setters must assert
  DALI_TEST_ASSERTION(config.SetLongPressGestureMinimumHoldingTime(700u), "UiConfig is frozen");
  DALI_TEST_ASSERTION(config.SetTapGestureMaximumMultiTapInterval(400u), "UiConfig is frozen");
  DALI_TEST_ASSERTION(config.SetTapGestureMaximumHoldingTime(250u), "UiConfig is frozen");
  DALI_TEST_ASSERTION(config.SetTapGestureMaximumMotionDistance(35.0f), "UiConfig is frozen");

  END_TEST;
}

int UtcDaliUiConfigLongPressKeyEventMinimumCountP(void)
{
  UiConfig config = UiConfig::New();

  // The default is the initial key down plus two repeats
  DALI_TEST_EQUALS(config.GetLongPressKeyEventMinimumCount(), 3u, TEST_LOCATION);

  config.SetLongPressKeyEventMinimumCount(5u);
  DALI_TEST_EQUALS(config.GetLongPressKeyEventMinimumCount(), 5u, TEST_LOCATION);

  UiTestApplication application(config);

  // The getter still reports the same value after the config has been applied
  DALI_TEST_EQUALS(config.GetLongPressKeyEventMinimumCount(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUiConfigLongPressKeyEventMinimumCountBehaviourP(void)
{
  // The count includes the initial key down, so a value of 2 means
  // the long press is recognized on the first repeat.
  UiConfig config = UiConfig::New();
  config.SetLongPressKeyEventMinimumCount(2u);

  UiTestApplication application(config);

  View view = CreateFocusedInteractiveView(application);
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  bool longPressed = false;
  view.AsInteractive().LongPressedSignal().Connect(
    &application,
    [&longPressed](View, InputEvent) -> bool {
      longPressed = true;
      return false;
    });

  // The initial key down alone must not be recognized as a long press
  SendReturnKeyDown(application, 100u);
  DALI_TEST_CHECK(!longPressed);

  // The first repeat reaches the minimum count
  SendReturnKeyDown(application, 120u);
  DALI_TEST_CHECK(longPressed);

  END_TEST;
}

int UtcDaliUiConfigLongPressKeyEventMinimumCountFrozenN(void)
{
  UiConfig config = UiConfig::New();

  UiTestApplication application(config);

  // The config is frozen by UiConfig::Apply(), so the setter must assert
  DALI_TEST_ASSERTION(config.SetLongPressKeyEventMinimumCount(5u), "UiConfig is frozen");

  END_TEST;
}
