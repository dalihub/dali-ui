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
#include <utility>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-components/public-api/chart/pie-series.h>
#include <dali-ui-components/public-api/chart/scatter-series.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_pieseries_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pieseries_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ===========================================================================
// PieSeries
// ===========================================================================

int UtcDaliPieSeriesConstructorP(void)
{
  UiTestApplication application;
  PieSeries pieSeries;
  DALI_TEST_CHECK(!pieSeries);
  END_TEST;
}

int UtcDaliPieSeriesNewP(void)
{
  UiTestApplication application;
  PieSeries pieSeries = PieSeries::New();
  DALI_TEST_CHECK(pieSeries);
  END_TEST;
}

int UtcDaliPieSeriesCopyConstructorP(void)
{
  UiTestApplication application;
  PieSeries pieSeries = PieSeries::New();
  PieSeries copy(pieSeries);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(pieSeries == copy);
  END_TEST;
}

int UtcDaliPieSeriesMoveConstructor(void)
{
  UiTestApplication application;
  PieSeries pieSeries = PieSeries::New();
  DALI_TEST_EQUALS(1, pieSeries.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  PieSeries moved = std::move(pieSeries);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!pieSeries);
  END_TEST;
}

int UtcDaliPieSeriesDownCastP(void)
{
  UiTestApplication application;
  PieSeries  pieSeries = PieSeries::New();
  BaseHandle handle(pieSeries);
  PieSeries  downCast = PieSeries::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliPieSeriesDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitializedObject;
  PieSeries  downCast = PieSeries::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliPieSeriesAddSliceP(void)
{
  UiTestApplication application;
  PieSeries series = PieSeries::New();

  series.AddSlice(Dali::String("Apple"), 50.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(series.GetSliceCount(), 1, TEST_LOCATION);

  series.AddSlice(Dali::String("Banana"), 30.0f, Vector4(1.0f, 1.0f, 0.0f, 1.0f));
  series.AddSlice(Dali::String("Cherry"), 20.0f, Vector4(0.8f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(series.GetSliceCount(), 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPieSeriesClearSlicesP(void)
{
  UiTestApplication application;
  PieSeries series = PieSeries::New();
  series.AddSlice(Dali::String("A"), 50.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  series.AddSlice(Dali::String("B"), 50.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

  series.ClearSlices();
  DALI_TEST_EQUALS(series.GetSliceCount(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPieSeriesInnerRadiusP(void)
{
  UiTestApplication application;
  PieSeries series = PieSeries::New();

  series.SetInnerRadiusRatio(0.5f);
  DALI_TEST_EQUALS(series.GetInnerRadiusRatio(), 0.5f, 0.001f, TEST_LOCATION);

  series.SetCenterLabel(Dali::String("50%"));
  DALI_TEST_EQUALS(series.GetCenterLabel(), Dali::String("50%"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPieSeriesSliceGapP(void)
{
  UiTestApplication application;
  PieSeries series = PieSeries::New();

  series.SetSliceGap(2.0f);
  DALI_TEST_EQUALS(series.GetSliceGap(), 2.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPieSeriesDataLabelsP(void)
{
  UiTestApplication application;
  PieSeries series = PieSeries::New();

  series.SetDataLabelsVisible(true);
  DALI_TEST_CHECK(series.IsDataLabelsVisible());

  series.SetDataLabelMinAngle(20.0f);
  DALI_TEST_EQUALS(series.GetDataLabelMinAngle(), 20.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPieSeriesMethodChainingP(void)
{
  UiTestApplication application;
  PieSeries series = PieSeries::New();

  PieSeries& ref = series.SetInnerRadiusRatio(0.4f)
                         .SetSliceGap(1.5f)
                         .SetDataLabelsVisible(true);
  DALI_TEST_CHECK(&ref == &series);
  DALI_TEST_EQUALS(series.GetSliceGap(), 1.5f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// ===========================================================================
// ScatterSeries
// ===========================================================================

int UtcDaliScatterSeriesNewP(void)
{
  UiTestApplication application;
  ScatterSeries scatterSeries = ScatterSeries::New();
  DALI_TEST_CHECK(scatterSeries);
  END_TEST;
}

int UtcDaliScatterSeriesDownCastP(void)
{
  UiTestApplication application;
  ScatterSeries scatterSeries = ScatterSeries::New();
  BaseHandle    handle(scatterSeries);
  ScatterSeries downCast = ScatterSeries::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliScatterSeriesDownCastN(void)
{
  UiTestApplication application;
  BaseHandle    unInitializedObject;
  ScatterSeries downCast = ScatterSeries::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliScatterSeriesSetPropertiesP(void)
{
  UiTestApplication application;
  ScatterSeries series = ScatterSeries::New();

  series.SetColor(Vector4(0.0f, 0.6f, 1.0f, 1.0f));
  DALI_TEST_EQUALS(series.GetColor(), Vector4(0.0f, 0.6f, 1.0f, 1.0f), TEST_LOCATION);

  series.SetMarkerRadius(7.0f);
  DALI_TEST_EQUALS(series.GetMarkerRadius(), 7.0f, 0.001f, TEST_LOCATION);

  series.SetMarkerShape(ScatterSeries::MarkerShape::SQUARE);
  DALI_TEST_EQUALS(static_cast<int>(series.GetMarkerShape()),
                   static_cast<int>(ScatterSeries::MarkerShape::SQUARE),
                   TEST_LOCATION);
  END_TEST;
}
