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
#include <dali-ui-components/integration-api/chart/chart-hit-tester.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/line-series.h>
#include <dali-ui-components/public-api/chart/pie-series.h>
#include <dali-ui-components/public-api/chart/scatter-series.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali/devel-api/events/pan-gesture-devel.h>
#include <dali/devel-api/events/pinch-gesture-devel.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <utility>

#define private public
#define protected public
#include <dali-ui-components/integration-api/chart/chart-view-impl.h>
#undef protected
#undef private

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

Dali::Ui::Integration::ChartViewImpl& GetChartImpl(ChartView view)
{
  return static_cast<Dali::Ui::Integration::ChartViewImpl&>(view.GetImplementation());
}
} // namespace

int UtcDaliChartPointEventArgsValueSemanticsP(void)
{
  ChartPointEventArgs empty;
  DALI_TEST_EQUALS(empty.GetSeriesIndex(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(empty.GetPointIndex(), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(empty.GetDataX(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(empty.GetDataY(), 0.0f, 0.001f, TEST_LOCATION);

  ChartPointEventArgs original(2, 4, 3.5f, -7.0f, "revenue", "April");
  DALI_TEST_EQUALS(original.GetSeriesIndex(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetPointIndex(), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetDataX(), 3.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetDataY(), -7.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetSeriesName(), Dali::String("revenue"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetXLabel(), Dali::String("April"), TEST_LOCATION);

  ChartPointEventArgs copy(original);
  DALI_TEST_EQUALS(copy.GetSeriesName(), Dali::String("revenue"), TEST_LOCATION);
  ChartPointEventArgs assigned;
  assigned = original;
  DALI_TEST_EQUALS(assigned.GetPointIndex(), 4, TEST_LOCATION);
  assigned = assigned;
  DALI_TEST_EQUALS(assigned.GetSeriesIndex(), 2, TEST_LOCATION);

  ChartPointEventArgs moved(std::move(copy));
  DALI_TEST_EQUALS(moved.GetDataY(), -7.0f, 0.001f, TEST_LOCATION);
  ChartPointEventArgs moveAssigned;
  moveAssigned = std::move(assigned);
  DALI_TEST_EQUALS(moveAssigned.GetXLabel(), Dali::String("April"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewGaugeStyleAndSignalsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::GAUGE, Vector2(360.0f, 360.0f));
  chartView.SetGaugeStartAngle(-90.0f);
  chartView.SetGaugeArcWidth(0.25f);
  chartView.SetGaugeTrackColor(Color::BLUE);
  chartView.SetGaugeProgressColor(Color::GREEN);
  DALI_TEST_EQUALS(chartView.GetGaugeStartAngle(), -90.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetGaugeArcWidth(), 0.25f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetGaugeTrackColor(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetGaugeProgressColor(), Color::GREEN, TEST_LOCATION);

  DALI_TEST_CHECK(chartView.LegendItemTappedSignal().Empty());
  DALI_TEST_CHECK(chartView.MultiPointSelectedSignal().Empty());
  DALI_TEST_CHECK(chartView.ZoomedSignal().Empty());
  END_TEST;
}

int UtcDaliChartViewPublicTooltipFormatterP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  chartView.SetTooltipFormatter([](const Dali::String& series, const Dali::String& label, float)
  {
    return series + label;
  });
  Dali::Ui::Integration::HitResult hit;
  hit.seriesName = "revenue";
  hit.xLabel = "April";
  hit.isValid = true;
  DALI_TEST_EQUALS(GetChartImpl(chartView).BuildMultiTooltipText({hit}), std::string("revenueApril"), TEST_LOCATION);

  END_TEST;
}

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

int UtcDaliChartViewInternalPropertiesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetProperty(ChartView::Property::SHOW_TOOLTIP, false);
  chartView.SetProperty(ChartView::Property::BACKGROUND_COLOR, Color::CYAN);
  chartView.SetProperty(ChartView::Property::GRID_COLOR, Color::MAGENTA);
  chartView.SetProperty(ChartView::Property::Y_AXIS_AUTO_RANGE, false);
  chartView.SetProperty(ChartView::Property::LEGEND_POSITION, 4);
  chartView.SetProperty(ChartView::Property::AXIS_LABEL_SIZE, 14.0f);
  chartView.SetProperty(ChartView::Property::TITLE_SIZE, 20.0f);
  chartView.SetProperty(ChartView::Property::LINE_WIDTH, 5.0f);
  chartView.SetProperty(ChartView::Property::SHOW_MARKERS, false);
  chartView.SetProperty(ChartView::Property::MARKER_RADIUS, 8.0f);
  chartView.SetProperty(ChartView::Property::HOVER_ENABLED, false);
  chartView.SetProperty(ChartView::Property::TOUCH_ENABLED, false);

  DALI_TEST_CHECK(!chartView.GetProperty<bool>(ChartView::Property::SHOW_TOOLTIP));
  DALI_TEST_EQUALS(chartView.GetProperty<Vector4>(ChartView::Property::BACKGROUND_COLOR), Color::CYAN, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetProperty<Vector4>(ChartView::Property::GRID_COLOR), Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_CHECK(!chartView.GetProperty<bool>(ChartView::Property::Y_AXIS_AUTO_RANGE));
  DALI_TEST_EQUALS(chartView.GetProperty<int>(ChartView::Property::LEGEND_POSITION), 4, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetProperty<float>(ChartView::Property::AXIS_LABEL_SIZE), 14.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetProperty<float>(ChartView::Property::TITLE_SIZE), 20.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetProperty<float>(ChartView::Property::LINE_WIDTH), 5.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!chartView.GetProperty<bool>(ChartView::Property::SHOW_MARKERS));
  DALI_TEST_EQUALS(chartView.GetProperty<float>(ChartView::Property::MARKER_RADIUS), 8.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!chartView.GetProperty<bool>(ChartView::Property::HOVER_ENABLED));
  DALI_TEST_CHECK(!chartView.GetProperty<bool>(ChartView::Property::TOUCH_ENABLED));

  chartView.SetProperty(ChartView::Property::Y_AXIS_AUTO_RANGE, true);
  DALI_TEST_CHECK(chartView.GetProperty<bool>(ChartView::Property::Y_AXIS_AUTO_RANGE));
  END_TEST;
}

int UtcDaliChartViewInternalViewportAndAnimationP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries line = LineSeries::New();
  line.SetName("line");
  line.SetValues({0.0f, 10.0f, 20.0f, 30.0f});
  chartView.AddSeries(line);
  RenderChart(application, chartView);
  Dali::Ui::Integration::ChartViewImpl& impl = GetChartImpl(chartView);

  DALI_TEST_EQUALS(Dali::Ui::Integration::ChartViewImpl::ApplyEasing(-1.0f, 0), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Integration::ChartViewImpl::ApplyEasing(0.5f, 1), 0.75f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Integration::ChartViewImpl::ApplyEasing(0.25f, 2), 0.125f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Integration::ChartViewImpl::ApplyEasing(0.75f, 2), 0.875f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Integration::ChartViewImpl::ApplyEasing(2.0f, 9), 1.0f, 0.001f, TEST_LOCATION);

  impl.InitViewportFromData();
  impl.mViewportXMin = impl.mDataXMin - 10.0f;
  impl.mViewportXMax = impl.mDataXMin + 1.0f;
  impl.mViewportYMin = impl.mDataYMin - 10.0f;
  impl.mViewportYMax = impl.mDataYMin + 1.0f;
  impl.ClampViewport();
  DALI_TEST_CHECK(impl.mViewportXMin >= impl.mDataXMin);
  DALI_TEST_CHECK(impl.mViewportYMin >= impl.mDataYMin);

  impl.mViewportXMin = impl.mDataXMax - 1.0f;
  impl.mViewportXMax = impl.mDataXMax + 10.0f;
  impl.mViewportYMin = impl.mDataYMax - 1.0f;
  impl.mViewportYMax = impl.mDataYMax + 10.0f;
  impl.ClampViewport();
  DALI_TEST_CHECK(impl.mViewportXMax <= impl.mDataXMax);
  DALI_TEST_CHECK(impl.mViewportYMax <= impl.mDataYMax);

  impl.mViewportXMin = 0.5f;
  impl.mViewportXMax = 2.5f;
  impl.FitYToViewport();
  DALI_TEST_CHECK(impl.mViewportYMax > impl.mViewportYMin);
  impl.ApplyViewportToScale();
  impl.mViewportXMax = impl.mViewportXMin;
  impl.mViewportYMax = impl.mViewportYMin;
  impl.ApplyViewportToScale();

  const auto canvasY = impl.CaptureCanvasY();
  DALI_TEST_EQUALS(canvasY.size(), 1u, TEST_LOCATION);
  impl.RebuildDataAnimated(canvasY, 1.0f);
  impl.mAnimOldCanvasY.clear();
  impl.mModel.mStyle.animation.duration = 1.0f;
  impl.mAnimStartTime = std::chrono::steady_clock::now() - std::chrono::milliseconds(10);
  DALI_TEST_CHECK(!impl.OnAnimTimer());
  impl.mModel.mStyle.animation.duration = 0.0f;
  DALI_TEST_CHECK(!impl.OnAnimTimer());

  chartView.ResetZoom();
  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::PAN_X) |
                        static_cast<int>(ChartView::ZoomMode::ZOOM_X));
  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::ZOOM_Y));
  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::NONE));
  END_TEST;
}

int UtcDaliChartViewInternalOverlayAndLegendP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries first = LineSeries::New();
  LineSeries second = LineSeries::New();
  first.SetName("first");
  first.SetValues({2.0f, 4.0f, 8.0f});
  second.SetName("second");
  second.SetValues({3.0f, 6.0f, 9.0f});
  chartView.AddSeries(first);
  chartView.AddSeries(second);
  chartView.SetProperty(ChartView::Property::SHOW_LEGEND, true);
  RenderChart(application, chartView);
  Dali::Ui::Integration::ChartViewImpl& impl = GetChartImpl(chartView);

  Dali::Ui::Integration::HitResult firstHit;
  firstHit.seriesName = "first";
  firstHit.xLabel = "B";
  firstHit.seriesColor = Color::RED;
  firstHit.canvasPos = Vector2(100.0f, 120.0f);
  firstHit.dataX = 1.0f;
  firstHit.dataY = 4.0f;
  firstHit.seriesIndex = 0;
  firstHit.pointIndex = 1;
  firstHit.isValid = true;
  Dali::Ui::Integration::HitResult secondHit = firstHit;
  secondHit.seriesName = "second";
  secondHit.dataY = 6.0f;
  secondHit.seriesIndex = 1;

  DALI_TEST_EQUALS(impl.BuildMultiTooltipText({firstHit, secondHit}), std::string("first: 4\nsecond: 6"), TEST_LOCATION);
  impl.SetTooltipFormatter([](const Dali::String& series, const Dali::String&, float) { return series; });
  DALI_TEST_EQUALS(impl.BuildMultiTooltipText({firstHit, secondHit}), std::string("first\nsecond"), TEST_LOCATION);
  impl.UpdateOverlay(firstHit);
  impl.UpdateOverlayMulti({firstHit, secondHit});
  impl.UpdateOverlayMulti({});
  impl.HideOverlay();

  DALI_TEST_EQUALS(impl.ComputeTooltipPosition(Vector2(5.0f, 5.0f), Vector2(130.0f, 52.0f)), Vector2(10.0f, 15.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.FindLegendItemAt(Vector2(-100.0f, -100.0f)), -1, TEST_LOCATION);
  if(!impl.mLastLayout.legendItems.empty())
  {
    const Vector2 legendPos = impl.mLastLayout.legendItems[0].textPos;
    DALI_TEST_EQUALS(impl.FindLegendItemAt(legendPos), 0, TEST_LOCATION);
    DALI_TEST_CHECK(impl.HandleLegendTap(legendPos));
    impl.HighlightLegendItem(0);
    impl.HighlightLegendItem(0);
    impl.ClearLegendHighlight();
  }
  DALI_TEST_CHECK(!impl.HandleLegendTap(Vector2(-100.0f, -100.0f)));

  impl.PerformHitAtPos(firstHit.canvasPos, false);
  impl.mModel.mStyle.interaction.findingStrategy = 1;
  impl.PerformHitAtPos(firstHit.canvasPos, false);
  impl.mModel.mStyle.interaction.findingStrategy = 2;
  impl.PerformHitAtPos(Vector2(-100.0f, -100.0f), false);
  END_TEST;
}

int UtcDaliChartViewInternalPieHitTestP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::PIE, Vector2(360.0f, 360.0f));
  PieSeries pie = PieSeries::New();
  pie.AddSlice("first", 25.0f, Color::RED);
  pie.AddSlice("second", 75.0f, Color::BLUE);
  pie.SetInnerRadiusRatio(0.4f);
  chartView.AddSeries(pie);
  RenderChart(application, chartView);
  Dali::Ui::Integration::ChartViewImpl& impl = GetChartImpl(chartView);
  impl.mLastLayout.plotArea = Rect<float>(0.0f, 0.0f, 300.0f, 300.0f);
  const Rect<float>& plot = impl.mLastLayout.plotArea;
  const Vector2 center(plot.x + plot.width * 0.5f, plot.y + plot.height * 0.5f);
  int seriesIndex = -1;

  DALI_TEST_EQUALS(impl.HitTestPie(center, seriesIndex), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(seriesIndex, 0, TEST_LOCATION);
  const float radialOffset = std::min(plot.width, plot.height) * 0.25f;
  const Vector2 outerPoint(center.x + radialOffset, center.y + radialOffset);
  DALI_TEST_CHECK(impl.HitTestPie(outerPoint, seriesIndex) >= 0);
  DALI_TEST_EQUALS(impl.HitTestPie(Vector2(-100.0f, -100.0f), seriesIndex), -1, TEST_LOCATION);

  pie.SetVisible(false);
  DALI_TEST_EQUALS(impl.HitTestPie(outerPoint, seriesIndex), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewWheelZoomAtPlotCenterP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries line = LineSeries::New();
  line.SetValues({0.0f, 10.0f, 20.0f, 30.0f});
  chartView.AddSeries(line);
  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::ZOOM_X) |
                        static_cast<int>(ChartView::ZoomMode::ZOOM_Y));
  RenderChart(application, chartView);

  auto& impl = GetChartImpl(chartView);
  impl.InitViewportFromData();
  const float originalXRange = impl.mViewportXMax - impl.mViewportXMin;
  const float originalYRange = impl.mViewportYMax - impl.mViewportYMin;
  const auto& plot = impl.mLastLayout.plotArea;
  const Vector2 center(plot.x + plot.width * 0.5f, plot.y + plot.height * 0.5f);

  WheelEvent zoomIn = WheelEvent::New(WheelEvent::MOUSE_WHEEL, 0, 0u, center, 1, 100u);
  DALI_TEST_CHECK(impl.OnWheel(chartView, zoomIn));
  DALI_TEST_CHECK(impl.mViewportXMax - impl.mViewportXMin < originalXRange);
  DALI_TEST_CHECK(impl.mViewportYMax - impl.mViewportYMin < originalYRange);

  WheelEvent outside = WheelEvent::New(WheelEvent::MOUSE_WHEEL, 0, 0u, Vector2(-100.0f, -100.0f), 1, 200u);
  DALI_TEST_CHECK(!impl.OnWheel(chartView, outside));

  chartView.ResetZoom();
  DALI_TEST_CHECK(!impl.mViewportActive);

  END_TEST;
}
int UtcDaliChartViewPanGestureMovesViewportP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries line = LineSeries::New();
  line.SetValues({5.0f, 10.0f, 20.0f, 35.0f});
  chartView.AddSeries(line);
  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::PAN_X) |
                        static_cast<int>(ChartView::ZoomMode::PAN_Y));
  chartView.SetZoomClampEnabled(false);
  RenderChart(application, chartView);

  auto& impl = GetChartImpl(chartView);
  PanGesture started = DevelPanGesture::New(GestureState::STARTED);
  DevelPanGesture::SetPosition(started, Vector2(100.0f, 100.0f));
  impl.OnPanGesture(chartView, started);
  DALI_TEST_CHECK(impl.mPanActive);
  const float startX = impl.mViewportXMin;
  const float startY = impl.mViewportYMin;

  PanGesture continuing = DevelPanGesture::New(GestureState::CONTINUING);
  DevelPanGesture::SetPosition(continuing, Vector2(140.0f, 120.0f));
  impl.OnPanGesture(chartView, continuing);
  DALI_TEST_CHECK(impl.mViewportXMin < startX);
  DALI_TEST_CHECK(impl.mViewportYMin < startY);

  PanGesture finished = DevelPanGesture::New(GestureState::FINISHED);
  impl.OnPanGesture(chartView, finished);
  DALI_TEST_CHECK(!impl.mPanActive);
  END_TEST;
}

int UtcDaliChartViewTouchHoverAndLegendInteractionP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries series = LineSeries::New();
  series.SetName("traffic");
  series.SetValues({2.0f, 5.0f, 9.0f, 4.0f});
  chartView.AddSeries(series);
  RenderChart(application, chartView);

  auto& impl = GetChartImpl(chartView);
  TouchEvent started = TouchEvent::New(1u);
  started.AddPoint(1, PointState::STARTED, Vector2(100.0f, 100.0f));
  DALI_TEST_CHECK(impl.OnTouch(chartView, started));
  TouchEvent motion = TouchEvent::New(2u);
  motion.AddPoint(1, PointState::MOTION, Vector2(140.0f, 120.0f));
  DALI_TEST_CHECK(impl.OnTouch(chartView, motion));
  TouchEvent finished = TouchEvent::New(3u);
  finished.AddPoint(1, PointState::FINISHED, Vector2(140.0f, 120.0f));
  DALI_TEST_CHECK(impl.OnTouch(chartView, finished));
  DALI_TEST_CHECK(!impl.mTouchActive);

  HoverEvent entered = HoverEvent::New(4u);
  entered.AddPoint(1, PointState::STARTED, Vector2(110.0f, 100.0f));
  DALI_TEST_CHECK(impl.OnHover(chartView, entered));
  HoverEvent exited = HoverEvent::New(5u);
  exited.AddPoint(1, PointState::FINISHED, Vector2(110.0f, 100.0f));
  DALI_TEST_CHECK(impl.OnHover(chartView, exited));

  impl.mLastLayout.hasLegend = true;
  impl.mModel.mStyle.visibility.showLegend = true;
  impl.mLastLayout.legendItems.push_back({Vector2(100.0f, 200.0f), Vector2(120.0f, 195.0f), "traffic"});
  impl.mLegendLabels.push_back(Label::New());
  const Vector2 legendPosition(100.0f, 195.0f);
  DALI_TEST_EQUALS(impl.FindLegendItemAt(legendPosition), 0, TEST_LOCATION);
  impl.HighlightLegendItem(0);
  DALI_TEST_EQUALS(impl.mHoveredLegendIndex, 0, TEST_LOCATION);
  impl.ClearLegendHighlight();
  DALI_TEST_EQUALS(impl.mHoveredLegendIndex, -1, TEST_LOCATION);
  DALI_TEST_CHECK(series.IsVisible());
  DALI_TEST_CHECK(impl.HandleLegendTap(legendPosition));
  DALI_TEST_CHECK(!series.IsVisible());

  impl.mModel.mStyle.interaction.touchEnabled = false;
  DALI_TEST_CHECK(!impl.OnTouch(chartView, started));
  impl.mModel.mStyle.interaction.hoverEnabled = false;
  DALI_TEST_CHECK(!impl.OnHover(chartView, entered));
  END_TEST;
}

int UtcDaliChartViewDenseLineAndNumericBarsP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView dense = ChartView::New(ChartView::Type::LINE, Vector2(280.0f, 220.0f));
  LineSeries line = LineSeries::New();
  line.SetName("Dense samples");
  line.SetMarkersVisible(true);
  line.SetMarkerShape(LineSeries::MarkerShape::CIRCLE);
  Dali::Vector<Vector2> points;
  for(uint32_t index = 0u; index < 1400u; ++index)
  {
    const float value = static_cast<float>(static_cast<int>(index % 37u) - 18);
    points.PushBack(Vector2(static_cast<float>(index), index == 301u
                                                         ? std::numeric_limits<float>::quiet_NaN()
                                                         : value));
  }
  line.SetValues(points);
  dense.AddSeries(line);
  RenderChart(application, dense);
  DALI_TEST_CHECK(dense.GetChildCount() > 0u);

  ChartView bars = ChartView::New(ChartView::Type::BAR, Vector2(280.0f, 220.0f));
  BarSeries bar = BarSeries::New();
  bar.SetName("Irregular intervals");
  bar.SetValues({Vector2(0.0f, -4.0f), Vector2(0.5f, 8.0f),
                 Vector2(2.5f, 3.0f), Vector2(8.0f, 12.0f)});
  bars.AddSeries(bar);
  RenderChart(application, bars);
  DALI_TEST_CHECK(bars.GetChildCount() > 0u);
  END_TEST;
}

int UtcDaliChartViewPinchZoomUpdatesBothViewportAxesP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries line = LineSeries::New();
  line.SetValues({0.0f, 10.0f, 20.0f, 30.0f});
  chartView.AddSeries(line);
  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::ZOOM_X) |
                        static_cast<int>(ChartView::ZoomMode::ZOOM_Y));
  chartView.SetZoomClampEnabled(false);
  chartView.SetAutoFitYOnPanEnabled(false);
  RenderChart(application, chartView);

  auto& impl = GetChartImpl(chartView);
  impl.InitViewportFromData();
  const float originalXRange = impl.mViewportXMax - impl.mViewportXMin;
  const float originalYRange = impl.mViewportYMax - impl.mViewportYMin;

  PinchGesture started = DevelPinchGesture::New(GestureState::STARTED);
  impl.OnPinchGesture(chartView, started);
  DALI_TEST_CHECK(impl.mPinchStartXMax > impl.mPinchStartXMin);
  DALI_TEST_CHECK(impl.mPinchStartYMax > impl.mPinchStartYMin);

  PinchGesture continuing = DevelPinchGesture::New(GestureState::CONTINUING);
  impl.OnPinchGesture(chartView, continuing);
  DALI_TEST_CHECK(impl.mViewportActive);
  DALI_TEST_CHECK(impl.mViewportXMax - impl.mViewportXMin > originalXRange);
  DALI_TEST_CHECK(impl.mViewportYMax - impl.mViewportYMin > originalYRange);

  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::NONE));
  const float unchangedXMin = impl.mViewportXMin;
  impl.OnPinchGesture(chartView, continuing);
  DALI_TEST_EQUALS(impl.mViewportXMin, unchangedXMin, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartViewTitlePlacementAndLabelPoolLifecycleP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  chartView.SetTitle("Revenue");
  ChartAxis xAxis = ChartAxis::New();
  ChartAxis yAxis = ChartAxis::New();
  xAxis.SetTitle("Month");
  yAxis.SetTitle("Amount");
  xAxis.SetLabels({"Jan", "Feb", "Mar"});
  chartView.SetXAxis(xAxis);
  chartView.SetYAxis(yAxis);
  LineSeries line = LineSeries::New();
  line.SetValues({1.0f, 3.0f, 2.0f});
  line.SetName("Revenue");
  chartView.AddSeries(line);
  chartView.SetProperty(ChartView::Property::SHOW_LEGEND, true);
  RenderChart(application, chartView);

  auto& impl = GetChartImpl(chartView);
  DALI_TEST_CHECK(impl.mTitleLabel);
  DALI_TEST_CHECK(impl.mXAxisTitleLabel);
  DALI_TEST_CHECK(impl.mYAxisTitleLabel);
  DALI_TEST_CHECK(!impl.mXTickLabels.empty());
  DALI_TEST_CHECK(!impl.mYTickLabels.empty());

  struct TitleCase
  {
    ChartView::TitlePosition position;
    Vector3 pivot;
  };
  const TitleCase titleCases[] = {
    {ChartView::TitlePosition::TOP_LEFT, Pivot::TOP_LEFT},
    {ChartView::TitlePosition::TOP_RIGHT, Pivot::TOP_RIGHT},
    {ChartView::TitlePosition::BOTTOM_CENTER, Pivot::BOTTOM_CENTER},
    {ChartView::TitlePosition::TOP_CENTER, Pivot::TOP_CENTER},
  };
  for(const TitleCase& titleCase : titleCases)
  {
    chartView.SetTitlePosition(titleCase.position);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(impl.mTitleLabel.GetProperty<Vector3>(Actor::Property::PIVOT), titleCase.pivot, TEST_LOCATION);
  }
  auto layout = impl.mLastLayout;
  layout.hasLegend = true;
  layout.legendItems.push_back({Vector2(20.0f, 20.0f), Vector2(40.0f, 20.0f), "Revenue"});
  impl.PlaceTextLabels(layout);
  DALI_TEST_CHECK(!impl.mLegendLabels.empty());

  layout.hasLegend = false;
  layout.hasTitle = false;
  layout.hasXAxisTitle = false;
  layout.hasYAxisTitle = false;
  impl.PlaceTextLabels(layout);
  DALI_TEST_CHECK(impl.mLegendLabels.empty());
  DALI_TEST_CHECK(!impl.mTitleLabel.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_CHECK(!impl.mXAxisTitleLabel.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_CHECK(!impl.mYAxisTitleLabel.GetProperty<bool>(Actor::Property::VISIBLE));

  const size_t childrenBeforeClear = chartView.GetChildCount();
  impl.ClearLabelPool(impl.mYTickLabels);
  DALI_TEST_CHECK(impl.mYTickLabels.empty());
  DALI_TEST_CHECK(chartView.GetChildCount() < childrenBeforeClear);
  END_TEST;
}

int UtcDaliChartHitTesterSameXAndNearestYP(void)
{
  UiTestApplication application(Components::UiConfig::New());
  Dali::Ui::Integration::ChartModel model;
  model.mXAxis = ChartAxis::New();
  model.mYAxis = ChartAxis::New();
  model.mXAxis.SetLabels({"zero", "one"});

  LineSeries low = LineSeries::New();
  low.SetName("low");
  low.SetValues({Vector2(1.0f, 2.0f), Vector2(3.0f, 4.0f)});
  LineSeries high = LineSeries::New();
  high.SetName("high");
  high.SetValues({Vector2(1.0f, 8.0f)});
  model.AddSeries(low);
  model.AddSeries(high);

  Dali::Ui::Integration::ScaleEngine scale;
  scale.SetPlotArea(Rect<float>(0.0f, 0.0f, 100.0f, 100.0f));
  scale.SetDataRange(0.0f, 10.0f, 0.0f, 10.0f);
  Dali::Ui::Integration::ChartHitTester tester;
  DALI_TEST_CHECK(tester.IsInsidePlotArea(Vector2(10.0f, 20.0f), scale));
  DALI_TEST_CHECK(!tester.IsInsidePlotArea(Vector2(-1.0f, 20.0f), scale));

  auto matches = tester.FindBySameX(Vector2(10.0f, 20.0f), model, scale, 15.0f, false);
  DALI_TEST_EQUALS(matches.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(matches[0].xLabel, String("one"), TEST_LOCATION);
  auto nearest = tester.FindBySameX(Vector2(10.0f, 20.0f), model, scale, 15.0f, true);
  DALI_TEST_EQUALS(nearest.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(nearest[0].seriesName, String("high"), TEST_LOCATION);
  DALI_TEST_EQUALS(nearest[0].dataY, 8.0f, TEST_LOCATION);
  auto closest = tester.FindNearest(Vector2(10.0f, 20.0f), model, scale, 15.0f);
  DALI_TEST_CHECK(closest.isValid);
  DALI_TEST_EQUALS(closest.seriesName, String("high"), TEST_LOCATION);
  DALI_TEST_EQUALS(closest.xLabel, String("one"), TEST_LOCATION);
  DALI_TEST_EQUALS(closest.pointIndex, 0, TEST_LOCATION);
  DALI_TEST_CHECK(!tester.FindNearest(Vector2(90.0f, 20.0f), model, scale, 1.0f).isValid);

  high.SetVisible(false);
  nearest = tester.FindBySameX(Vector2(10.0f, 20.0f), model, scale, 15.0f, true);
  DALI_TEST_EQUALS(nearest.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(nearest[0].seriesName, String("low"), TEST_LOCATION);
  DALI_TEST_CHECK(tester.FindBySameX(Vector2(90.0f, 20.0f), model, scale, 1.0f, true).empty());
  END_TEST;
}
