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
#include <dali-ui-components/public-api/chart/bar-series.h>
#include <dali-ui-components/public-api/chart/chart-section.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/line-series.h>
#include <dali-ui-components/public-api/chart/pie-series.h>
#include <dali-ui-components/public-api/chart/scatter-series.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_chartview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_chartview_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
static bool gSignalReceived = false;

void OnDataPointSelected(ChartView, const ChartPointEventArgs&)
{
  gSignalReceived = true;
}

void RenderChart(UiTestApplication& application, ChartView chartView)
{
  application.GetScene().Add(chartView);
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();
}
} // namespace

int UtcDaliChartViewConstructorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView;
  DALI_TEST_CHECK(!chartView);
  END_TEST;
}

int UtcDaliChartViewNewLineP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewNewBarP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::BAR, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewNewPieP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::PIE, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewNewGaugeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::GAUGE, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewCopyConstructorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartView         copy(chartView);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(chartView == copy);
  END_TEST;
}

int UtcDaliChartViewMoveConstructor(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  DALI_TEST_EQUALS(1, chartView.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  ChartView moved = std::move(chartView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!chartView);
  END_TEST;
}

int UtcDaliChartViewAssignmentOperatorP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartView         copy;
  copy = chartView;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(chartView == copy);
  END_TEST;
}

int UtcDaliChartViewMoveAssignment(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  DALI_TEST_EQUALS(1, chartView.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  ChartView moved;
  moved = std::move(chartView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!chartView);
  END_TEST;
}

int UtcDaliChartViewDownCastP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  BaseHandle        handle(chartView);
  ChartView         downCast = ChartView::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliChartViewDownCastN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  BaseHandle        unInitializedObject;
  ChartView         downCast = ChartView::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliChartViewSetTitleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetTitle(Dali::String("Monthly Sales"));
  DALI_TEST_EQUALS(chartView.GetTitle(), Dali::String("Monthly Sales"), TEST_LOCATION);

  chartView.SetTitlePosition(ChartView::TitlePosition::TOP_LEFT);
  DALI_TEST_EQUALS(static_cast<int>(chartView.GetTitlePosition()),
                   static_cast<int>(ChartView::TitlePosition::TOP_LEFT),
                   TEST_LOCATION);

  chartView.SetTitleColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS(chartView.GetTitleColor(), Vector4(1.0f, 0.0f, 0.0f, 1.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewAnimationP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetAnimationDuration(500.0f);
  DALI_TEST_EQUALS(chartView.GetAnimationDuration(), 500.0f, 0.001f, TEST_LOCATION);

  chartView.SetAnimationEasing(ChartView::EasingType::EASE_OUT);
  DALI_TEST_EQUALS(static_cast<int>(chartView.GetAnimationEasing()),
                   static_cast<int>(ChartView::EasingType::EASE_OUT),
                   TEST_LOCATION);

  chartView.SetUpdateThrottle(50.0f);
  DALI_TEST_EQUALS(chartView.GetUpdateThrottle(), 50.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewAddRemoveSeriesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries        series    = LineSeries::New();
  series.SetName(Dali::String("series1"));

  chartView.AddSeries(series);
  bool removed = chartView.RemoveSeries(Dali::String("series1"));
  DALI_TEST_CHECK(removed);
  END_TEST;
}

int UtcDaliChartViewRemoveSeriesN(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  bool removed = chartView.RemoveSeries(Dali::String("nonexistent"));
  DALI_TEST_CHECK(!removed);
  END_TEST;
}

int UtcDaliChartViewRemoveAllSeriesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries        s1        = LineSeries::New();
  LineSeries        s2        = LineSeries::New();
  s1.SetName(Dali::String("s1"));
  s2.SetName(Dali::String("s2"));
  chartView.AddSeries(s1);
  chartView.AddSeries(s2);
  chartView.RemoveAllSeries();
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewZoomModeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::PAN_X));
  DALI_TEST_EQUALS(chartView.GetZoomMode(), static_cast<int>(ChartView::ZoomMode::PAN_X), TEST_LOCATION);

  chartView.SetZoomClampEnabled(false);
  DALI_TEST_CHECK(!chartView.IsZoomClampEnabled());

  chartView.SetAutoFitYOnPanEnabled(true);
  DALI_TEST_CHECK(chartView.IsAutoFitYOnPanEnabled());
  END_TEST;
}

int UtcDaliChartViewHitTestingP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetHitThreshold(20.0f);
  DALI_TEST_EQUALS(chartView.GetHitThreshold(), 20.0f, 0.001f, TEST_LOCATION);

  chartView.SetFindingStrategy(ChartView::FindingStrategy::SAME_X);
  DALI_TEST_EQUALS(static_cast<int>(chartView.GetFindingStrategy()),
                   static_cast<int>(ChartView::FindingStrategy::SAME_X),
                   TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewPropertyP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetProperty(ChartView::Property::SHOW_GRID, false);
  DALI_TEST_CHECK(!chartView.GetProperty<bool>(ChartView::Property::SHOW_GRID));

  chartView.SetProperty(ChartView::Property::SHOW_LEGEND, true);
  DALI_TEST_CHECK(chartView.GetProperty<bool>(ChartView::Property::SHOW_LEGEND));

  chartView.SetProperty(ChartView::Property::ANIMATION_DURATION, 200.0f);
  DALI_TEST_EQUALS(chartView.GetProperty<float>(ChartView::Property::ANIMATION_DURATION), 200.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewSignalP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  gSignalReceived = false;
  chartView.DataPointSelectedSignal().Connect(OnDataPointSelected);
  DALI_TEST_CHECK(!gSignalReceived);
  chartView.DataPointSelectedSignal().Disconnect(OnDataPointSelected);
  END_TEST;
}

int UtcDaliChartViewGaugeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::GAUGE, Vector2(480.0f, 360.0f));

  chartView.SetGaugeValue(75.0f);
  DALI_TEST_EQUALS(chartView.GetGaugeValue(), 75.0f, 0.001f, TEST_LOCATION);

  chartView.SetGaugeMinimumValue(0.0f);
  chartView.SetGaugeMaximumValue(100.0f);
  DALI_TEST_EQUALS(chartView.GetGaugeMinimumValue(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetGaugeMaximumValue(), 100.0f, 0.001f, TEST_LOCATION);

  chartView.SetGaugeArcSpan(270.0f);
  DALI_TEST_EQUALS(chartView.GetGaugeArcSpan(), 270.0f, 0.001f, TEST_LOCATION);

  chartView.SetGaugeCenterLabel(Dali::String("75%"));
  DALI_TEST_EQUALS(chartView.GetGaugeCenterLabel(), Dali::String("75%"), TEST_LOCATION);

  chartView.AddGaugeRange(0.0f, 60.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
  chartView.ClearGaugeRanges();
  END_TEST;
}

int UtcDaliChartViewSectionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartSection      section   = ChartSection::New();
  section.SetMinimumX(1.0f);
  section.SetMaximumX(3.0f);

  chartView.AddSection(section);
  chartView.RemoveSection(section);
  chartView.ClearSections();
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewSettersP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetTitle(Dali::String("Test"));
  chartView.SetAnimationDuration(300.0f);
  chartView.SetHitThreshold(15.0f);
  DALI_TEST_EQUALS(chartView.GetTitle(), Dali::String("Test"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewRenderLineAndScatterP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartAxis         xAxis     = ChartAxis::New();
  ChartAxis         yAxis     = ChartAxis::New();
  LineSeries        line      = LineSeries::New();
  ScatterSeries     scatter   = ScatterSeries::New();
  ChartSection      band      = ChartSection::New();
  ChartSection      lineBand  = ChartSection::New();

  xAxis.SetLabels({"Jan", "Feb", "Mar", "Apr", "May"});
  xAxis.SetTitle("Month");
  yAxis.SetTitle("Value");
  yAxis.SetMinimumLimit(-10.0f);
  yAxis.SetMaximumLimit(50.0f);
  chartView.SetXAxis(xAxis);
  chartView.SetYAxis(yAxis);

  line.SetName("Revenue");
  line.SetValues({10.0f, 24.0f, std::numeric_limits<float>::quiet_NaN(), -5.0f, 42.0f});
  line.SetSmoothness(0.8f);
  line.SetFillEnabled(true);
  line.SetFillColor(Vector4(0.2f, 0.5f, 0.9f, 0.25f));
  line.SetMarkerShape(LineSeries::MarkerShape::DIAMOND);
  line.SetMarkerRadius(6.0f);
  line.SetMarkerBorderWidth(2.0f);
  line.SetMarkerBorderColor(Color::BLACK);
  line.SetDataLabelsVisible(true);
  line.SetDataLabelFormat("%.2f");
  chartView.AddSeries(line);

  scatter.SetName("Events");
  scatter.SetValues({Vector2(0.5f, 5.0f), Vector2(2.5f, 30.0f), Vector2(4.0f, 15.0f)});
  scatter.SetMarkerShape(ScatterSeries::MarkerShape::TRIANGLE);
  scatter.SetMarkerRadius(5.0f);
  chartView.AddSeries(scatter);

  band.SetMinimumX(1.0f);
  band.SetMaximumX(3.0f);
  band.SetMinimumY(0.0f);
  band.SetMaximumY(20.0f);
  band.SetFillColor(Vector4(1.0f, 0.8f, 0.0f, 0.2f));
  chartView.AddSection(band);

  lineBand.SetMinimumY(25.0f);
  lineBand.SetMaximumY(25.0f);
  lineBand.SetStrokeWidth(2.0f);
  chartView.AddSection(lineBand);

  chartView.SetTitle("Quarterly overview");
  chartView.SetProperty(ChartView::Property::SHOW_LEGEND, true);
  RenderChart(application, chartView);

  DALI_TEST_CHECK(chartView.GetChildCount() > 3u);
  END_TEST;
}

int UtcDaliChartViewRenderGroupedAndStackedBarsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::BAR, Vector2(480.0f, 360.0f));
  ChartAxis         xAxis     = ChartAxis::New();
  ChartAxis         yAxis     = ChartAxis::New();
  BarSeries         grouped   = BarSeries::New();
  BarSeries         positive  = BarSeries::New();
  BarSeries         negative  = BarSeries::New();

  xAxis.SetLabels({"A", "B", "C", "D"});
  yAxis.SetMinimumLimit(-30.0f);
  yAxis.SetMaximumLimit(80.0f);
  chartView.SetXAxis(xAxis);
  chartView.SetYAxis(yAxis);

  grouped.SetName("Grouped");
  grouped.SetValues({20.0f, -15.0f, 0.0f, 55.0f});
  grouped.SetColor(Vector4(0.2f, 0.6f, 0.9f, 1.0f));
  grouped.SetDataLabelsVisible(true);
  grouped.SetDataLabelFormat("%.0f");
  chartView.AddSeries(grouped);

  positive.SetName("Positive stack");
  positive.SetValues({12.0f, 25.0f, 30.0f, 18.0f});
  positive.SetStacked(true);
  positive.SetDataLabelsVisible(true);
  chartView.AddSeries(positive);

  negative.SetName("Negative stack");
  negative.SetValues({-8.0f, -12.0f, -20.0f, -5.0f});
  negative.SetStacked(true);
  negative.SetDataLabelsVisible(true);
  chartView.AddSeries(negative);

  chartView.SetProperty(ChartView::Property::SHOW_LEGEND, true);
  RenderChart(application, chartView);

  grouped.SetValues({30.0f, -20.0f, 15.0f, 60.0f});
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(chartView.GetChildCount() > 3u);
  END_TEST;
}

int UtcDaliChartViewRenderPieAndDonutP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::PIE, Vector2(420.0f, 360.0f));
  PieSeries         pie       = PieSeries::New();

  pie.AddSlice("Alpha", 55.0f, Vector4(0.9f, 0.2f, 0.2f, 1.0f));
  pie.AddSlice("Beta", 30.0f, Vector4(0.2f, 0.7f, 0.3f, 1.0f));
  pie.AddSlice("Gamma", 15.0f, Vector4(0.2f, 0.4f, 0.9f, 1.0f));
  pie.SetInnerRadiusRatio(0.45f);
  pie.SetSliceGap(2.0f);
  pie.SetCenterLabel("Total");
  pie.SetDataLabelsVisible(true);
  pie.SetDataLabelMinAngle(5.0f);
  chartView.AddSeries(pie);
  chartView.SetTitle("Distribution");
  chartView.SetProperty(ChartView::Property::SHOW_LEGEND, true);
  RenderChart(application, chartView);

  DALI_TEST_CHECK(chartView.GetChildCount() > 3u);
  END_TEST;
}

int UtcDaliChartViewRenderGaugeP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView         chartView = ChartView::New(ChartView::Type::GAUGE, Vector2(360.0f, 360.0f));

  chartView.SetGaugeMinimumValue(0.0f);
  chartView.SetGaugeMaximumValue(100.0f);
  chartView.SetGaugeValue(72.0f);
  chartView.SetGaugeArcSpan(270.0f);
  chartView.SetGaugeCenterLabel("72%");
  chartView.AddGaugeRange(0.0f, 40.0f, Vector4(0.9f, 0.2f, 0.2f, 1.0f));
  chartView.AddGaugeRange(40.0f, 80.0f, Vector4(0.9f, 0.7f, 0.1f, 1.0f));
  chartView.AddGaugeRange(80.0f, 100.0f, Vector4(0.2f, 0.7f, 0.3f, 1.0f));
  RenderChart(application, chartView);

  DALI_TEST_CHECK(chartView.GetChildCount() > 3u);
  END_TEST;
}
