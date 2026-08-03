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
#include <dali/integration-api/input-options.h>

using namespace Dali;
using namespace Dali::Ui;

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
