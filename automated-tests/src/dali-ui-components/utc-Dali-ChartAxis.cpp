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

#include <dali-ui-components/public-api/chart/chart-axis.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <stdlib.h>
#include <iostream>
#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_chartaxis_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_chartaxis_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliChartAxisConstructorP(void)
{
  UiTestApplication application;
  ChartAxis         axis;
  DALI_TEST_CHECK(!axis);
  END_TEST;
}

int UtcDaliChartAxisNewP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();
  DALI_TEST_CHECK(axis);
  END_TEST;
}

int UtcDaliChartAxisCopyConstructorP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();
  ChartAxis         copy(axis);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(axis == copy);
  END_TEST;
}

int UtcDaliChartAxisMoveConstructor(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();
  DALI_TEST_EQUALS(1, axis.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  ChartAxis moved = std::move(axis);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!axis);
  END_TEST;
}

int UtcDaliChartAxisDownCastP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();
  BaseHandle        handle(axis);
  ChartAxis         downCast = ChartAxis::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliChartAxisDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  ChartAxis         downCast = ChartAxis::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliChartAxisSetLabelsP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  std::vector<Dali::String> labels = {Dali::String("Jan"),
                                      Dali::String("Feb"),
                                      Dali::String("Mar")};
  axis.SetLabels(labels);
  std::vector<Dali::String> result = axis.GetLabels();
  DALI_TEST_EQUALS(static_cast<int>(result.size()), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0], Dali::String("Jan"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[2], Dali::String("Mar"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSetTitleP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetTitle(Dali::String("Month"));
  DALI_TEST_EQUALS(axis.GetTitle(), Dali::String("Month"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSetRangeLimitsP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetMinLimit(10.0f);
  DALI_TEST_EQUALS(axis.GetMinLimit(), 10.0f, 0.001f, TEST_LOCATION);

  axis.SetMaxLimit(200.0f);
  DALI_TEST_EQUALS(axis.GetMaxLimit(), 200.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSetAutoRangeP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetAutoRange(false);
  DALI_TEST_CHECK(!axis.IsAutoRange());

  axis.SetAutoRange(true);
  DALI_TEST_CHECK(axis.IsAutoRange());
  END_TEST;
}

int UtcDaliChartAxisSetGridLinesP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetShowGridLines(false);
  DALI_TEST_CHECK(!axis.GetShowGridLines());

  axis.SetGridColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
  DALI_TEST_EQUALS(axis.GetGridColor(), Vector4(0.5f, 0.5f, 0.5f, 1.0f), TEST_LOCATION);

  axis.SetGridDash(6.0f, 3.0f);
  float dash = 0.0f, gap = 0.0f;
  axis.GetGridDash(dash, gap);
  DALI_TEST_EQUALS(dash, 6.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(gap, 3.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSetAxisLineP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetAxisLineColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f));
  DALI_TEST_EQUALS(axis.GetAxisLineColor(), Vector4(0.2f, 0.2f, 0.2f, 1.0f), TEST_LOCATION);

  axis.SetAxisLineWidth(3.0f);
  DALI_TEST_EQUALS(axis.GetAxisLineWidth(), 3.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSetDataPaddingP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetDataPadding(0.1f);
  DALI_TEST_EQUALS(axis.GetDataPadding(), 0.1f, 0.001f, TEST_LOCATION);

  axis.SetMinStep(5.0f);
  DALI_TEST_EQUALS(axis.GetMinStep(), 5.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSetLabelsRotationP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetLabelsRotation(45.0f);
  DALI_TEST_EQUALS(axis.GetLabelsRotation(), 45.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAxisSettersP(void)
{
  UiTestApplication application;
  ChartAxis         axis = ChartAxis::New();

  axis.SetTitle(Dali::String("Value"));
  axis.SetAutoRange(true);
  axis.SetShowGridLines(true);
  axis.SetDataPadding(0.05f);
  DALI_TEST_EQUALS(axis.GetTitle(), Dali::String("Value"), TEST_LOCATION);
  END_TEST;
}
