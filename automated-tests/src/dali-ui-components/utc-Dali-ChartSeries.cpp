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

#include <dali-ui-components/public-api/chart/bar-series.h>
#include <dali-ui-components/public-api/chart/chart-series.h>
#include <dali-ui-components/public-api/chart/line-series.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <stdlib.h>
#include <iostream>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_chartseries_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_chartseries_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ===========================================================================
// ChartSeries (base)
// ===========================================================================

int UtcDaliChartSeriesDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        lineSeries = LineSeries::New();
  BaseHandle        handle(lineSeries);
  ChartSeries       series = ChartSeries::DownCast(handle);
  DALI_TEST_CHECK(series);
  END_TEST;
}

int UtcDaliChartSeriesDownCastN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BaseHandle        unInitializedObject;
  ChartSeries       series = ChartSeries::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!series);
  END_TEST;
}

int UtcDaliChartSeriesSetNameP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();
  series.SetName(Dali::String("temperature"));
  DALI_TEST_EQUALS(series.GetName(), Dali::String("temperature"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartSeriesSetValuesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();
  series.SetValues({1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
  DALI_TEST_CHECK(series);
  END_TEST;
}

int UtcDaliChartSeriesAppendValueP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();
  series.AppendValue(10.0f);
  series.AppendValue(1.0f, 10.0f);
  series.AppendValues({5.0f, 6.0f, 7.0f});
  DALI_TEST_CHECK(series);
  END_TEST;
}

int UtcDaliChartSeriesSetVisibleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();
  series.SetVisible(false);
  DALI_TEST_CHECK(!series.IsVisible());
  series.SetVisible(true);
  DALI_TEST_CHECK(series.IsVisible());
  END_TEST;
}

int UtcDaliChartSeriesSetMaxDataPointsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();
  series.SetMaximumDataPoints(50);
  DALI_TEST_EQUALS(series.GetMaximumDataPoints(), 50, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartSeriesSetZIndexP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();
  series.SetZIndex(3);
  DALI_TEST_EQUALS(series.GetZIndex(), 3, TEST_LOCATION);
  END_TEST;
}

// ===========================================================================
// LineSeries
// ===========================================================================

int UtcDaliLineSeriesConstructorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        lineSeries;
  DALI_TEST_CHECK(!lineSeries);
  END_TEST;
}

int UtcDaliLineSeriesNewP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        lineSeries = LineSeries::New();
  DALI_TEST_CHECK(lineSeries);
  END_TEST;
}

int UtcDaliLineSeriesCopyConstructorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        lineSeries = LineSeries::New();
  LineSeries        copy(lineSeries);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(lineSeries == copy);
  END_TEST;
}

int UtcDaliLineSeriesMoveConstructor(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        lineSeries = LineSeries::New();
  DALI_TEST_EQUALS(1, lineSeries.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  LineSeries moved = std::move(lineSeries);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!lineSeries);
  END_TEST;
}

int UtcDaliLineSeriesDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        lineSeries = LineSeries::New();
  BaseHandle        handle(lineSeries);
  LineSeries        downCast = LineSeries::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliLineSeriesDownCastN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BaseHandle        unInitializedObject;
  LineSeries        downCast = LineSeries::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliLineSeriesSetLinePropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();

  series.SetLineWidth(3.0f);
  DALI_TEST_EQUALS(series.GetLineWidth(), 3.0f, 0.001f, TEST_LOCATION);

  series.SetSmoothness(0.8f);
  DALI_TEST_EQUALS(series.GetSmoothness(), 0.8f, 0.001f, TEST_LOCATION);

  series.SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(series.GetColor(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLineSeriesSetMarkerPropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();

  series.SetMarkersVisible(true);
  DALI_TEST_CHECK(series.IsMarkersVisible());

  series.SetMarkerRadius(6.0f);
  DALI_TEST_EQUALS(series.GetMarkerRadius(), 6.0f, 0.001f, TEST_LOCATION);

  series.SetMarkerShape(LineSeries::MarkerShape::DIAMOND);
  DALI_TEST_EQUALS(static_cast<int>(series.GetMarkerShape()),
                   static_cast<int>(LineSeries::MarkerShape::DIAMOND),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliLineSeriesFillP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();

  series.SetFillEnabled(true);
  DALI_TEST_CHECK(series.IsFillEnabled());

  series.SetFillColor(Vector4(0.0f, 0.5f, 1.0f, 0.3f));
  DALI_TEST_EQUALS(series.GetFillColor(), Vector4(0.0f, 0.5f, 1.0f, 0.3f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLineSeriesDataLabelsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();

  series.SetDataLabelsVisible(true);
  DALI_TEST_CHECK(series.IsDataLabelsVisible());

  series.SetDataLabelFormat(Dali::String("%.2f"));
  DALI_TEST_EQUALS(series.GetDataLabelFormat(), Dali::String("%.2f"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLineSeriesSettersP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  LineSeries        series = LineSeries::New();

  series.SetLineWidth(2.0f);
  series.SetSmoothness(0.5f);
  series.SetMarkersVisible(true);
  DALI_TEST_EQUALS(series.GetLineWidth(), 2.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

// ===========================================================================
// BarSeries
// ===========================================================================

int UtcDaliBarSeriesNewP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BarSeries         barSeries = BarSeries::New();
  DALI_TEST_CHECK(barSeries);
  END_TEST;
}

int UtcDaliBarSeriesDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BarSeries         barSeries = BarSeries::New();
  BaseHandle        handle(barSeries);
  BarSeries         downCast = BarSeries::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliBarSeriesDownCastN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BaseHandle        unInitializedObject;
  BarSeries         downCast = BarSeries::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliBarSeriesSetPropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BarSeries         series = BarSeries::New();

  series.SetColor(Vector4(0.2f, 0.4f, 0.8f, 1.0f));
  DALI_TEST_EQUALS(series.GetColor(), Vector4(0.2f, 0.4f, 0.8f, 1.0f), TEST_LOCATION);

  series.SetBarGroupWidth(0.6f);
  DALI_TEST_EQUALS(series.GetBarGroupWidth(), 0.6f, 0.001f, TEST_LOCATION);

  series.SetStacked(true);
  DALI_TEST_CHECK(series.IsStacked());
  END_TEST;
}

int UtcDaliBarSeriesDataLabelsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BarSeries         series = BarSeries::New();

  series.SetDataLabelsVisible(true);
  DALI_TEST_CHECK(series.IsDataLabelsVisible());

  series.SetDataLabelFormat(Dali::String("%.0f"));
  DALI_TEST_EQUALS(series.GetDataLabelFormat(), Dali::String("%.0f"), TEST_LOCATION);
  END_TEST;
}
