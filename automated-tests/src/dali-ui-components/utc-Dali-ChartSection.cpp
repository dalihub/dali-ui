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
#include <dali-ui-components/public-api/chart/chart-section.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_chartsection_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_chartsection_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliChartSectionConstructorP(void)
{
  UiTestApplication application;
  ChartSection section;
  DALI_TEST_CHECK(!section);
  END_TEST;
}

int UtcDaliChartSectionNewP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();
  DALI_TEST_CHECK(section);
  END_TEST;
}

int UtcDaliChartSectionCopyConstructorP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();
  ChartSection copy(section);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(section == copy);
  END_TEST;
}

int UtcDaliChartSectionAssignmentOperatorP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();
  ChartSection copy;
  copy = section;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(section == copy);
  END_TEST;
}

int UtcDaliChartSectionSetBoundsP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();

  section.SetXMin(1.0f);
  DALI_TEST_EQUALS(section.GetXMin(), 1.0f, 0.001f, TEST_LOCATION);

  section.SetXMax(5.0f);
  DALI_TEST_EQUALS(section.GetXMax(), 5.0f, 0.001f, TEST_LOCATION);

  section.SetYMin(0.0f);
  DALI_TEST_EQUALS(section.GetYMin(), 0.0f, 0.001f, TEST_LOCATION);

  section.SetYMax(100.0f);
  DALI_TEST_EQUALS(section.GetYMax(), 100.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartSectionSetFillColorP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();

  section.SetFillColor(Vector4(0.0f, 0.5f, 0.0f, 0.3f));
  DALI_TEST_EQUALS(section.GetFillColor(), Vector4(0.0f, 0.5f, 0.0f, 0.3f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartSectionSetStrokeP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();

  section.SetStrokeColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(section.GetStrokeColor(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);

  section.SetStrokeWidth(2.0f);
  DALI_TEST_EQUALS(section.GetStrokeWidth(), 2.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartSectionMethodChainingP(void)
{
  UiTestApplication application;
  ChartSection section = ChartSection::New();

  ChartSection& ref = section.SetXMin(0.0f)
                             .SetXMax(10.0f)
                             .SetFillColor(Vector4(0.0f, 0.0f, 1.0f, 0.2f))
                             .SetStrokeWidth(1.5f);
  DALI_TEST_CHECK(&ref == &section);
  DALI_TEST_EQUALS(section.GetXMax(), 10.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}
