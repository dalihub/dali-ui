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
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-test-suite-utils.h>
#include "dali-ui-foundation/public-api/ui-config.h"

using namespace Dali;
using namespace Dali::UI;

void utc_dali_unit_startup(void)
{
  test_return_value = TET_UNDEF;
  UIConfig::New().Apply();
}

void utc_dali_unit_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliUnitSpxUnsignedInt(void)
{
  TestApplication application;

  // Test unsigned long long int version
  float result = 100_spx;
  DALI_TEST_EQUALS(result, 100.0f, TEST_LOCATION);

  result = 0_spx;
  DALI_TEST_EQUALS(result, 0.0f, TEST_LOCATION);

  result = 50_spx;
  DALI_TEST_EQUALS(result, 50.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUnitSpxLongDouble(void)
{
  TestApplication application;

  // Test long double version
  float result = 100.5_spx;
  DALI_TEST_EQUALS(result, 100.5f, TEST_LOCATION);

  result = 0.0_spx;
  DALI_TEST_EQUALS(result, 0.0f, TEST_LOCATION);

  result = 50.25_spx;
  DALI_TEST_EQUALS(result, 50.25f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUnitDpUnsignedInt(void)
{
  TestApplication application;

  // Test unsigned long long int version
  float result = 100_dp;
  DALI_TEST_EQUALS(result, 100.0f, TEST_LOCATION);

  result = 0_dp;
  DALI_TEST_EQUALS(result, 0.0f, TEST_LOCATION);

  result = 50_dp;
  DALI_TEST_EQUALS(result, 50.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUnitDpLongDouble(void)
{
  TestApplication application;

  // Test long double version
  float result = 100.5_dp;
  DALI_TEST_EQUALS(result, 100.5f, TEST_LOCATION);

  result = 0.0_dp;
  DALI_TEST_EQUALS(result, 0.0f, TEST_LOCATION);

  result = 50.25_dp;
  DALI_TEST_EQUALS(result, 50.25f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUnitSdpUnsignedInt(void)
{
  TestApplication application;

  // Test unsigned long long int version
  float result = 100_sdp;
  DALI_TEST_EQUALS(result, 100.0f, TEST_LOCATION);

  result = 0_sdp;
  DALI_TEST_EQUALS(result, 0.0f, TEST_LOCATION);

  result = 50_sdp;
  DALI_TEST_EQUALS(result, 50.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUnitSdpLongDouble(void)
{
  TestApplication application;

  // Test long double version
  float result = 100.5_sdp;
  DALI_TEST_EQUALS(result, 100.5f, TEST_LOCATION);

  result = 0.0_sdp;
  DALI_TEST_EQUALS(result, 0.0f, TEST_LOCATION);

  result = 50.25_sdp;
  DALI_TEST_EQUALS(result, 50.25f, TEST_LOCATION);

  END_TEST;
}
