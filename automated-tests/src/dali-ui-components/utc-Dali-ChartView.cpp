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
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/line-series.h>
#include <dali-ui-components/public-api/chart/chart-axis.h>
#include <dali-ui-components/public-api/chart/chart-section.h>
#include <dali-ui-test-suite-utils.h>

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

void OnDataPointSelected(const ChartPointEventArgs&)
{
  gSignalReceived = true;
}
} // namespace

int UtcDaliChartViewConstructorP(void)
{
  UiTestApplication application;
  ChartView chartView;
  DALI_TEST_CHECK(!chartView);
  END_TEST;
}

int UtcDaliChartViewNewLineP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewNewBarP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::BAR, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewNewPieP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::PIE, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewNewGaugeP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::GAUGE, Vector2(480.0f, 360.0f));
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewCopyConstructorP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartView copy(chartView);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(chartView == copy);
  END_TEST;
}

int UtcDaliChartViewMoveConstructor(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  DALI_TEST_EQUALS(1, chartView.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  ChartView moved = std::move(chartView);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!chartView);
  END_TEST;
}

int UtcDaliChartViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartView copy;
  copy = chartView;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(chartView == copy);
  END_TEST;
}

int UtcDaliChartViewMoveAssignment(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
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
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  BaseHandle handle(chartView);
  ChartView  downCast = ChartView::DownCast(handle);
  DALI_TEST_CHECK(downCast);
  END_TEST;
}

int UtcDaliChartViewDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitializedObject;
  ChartView  downCast = ChartView::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!downCast);
  END_TEST;
}

int UtcDaliChartViewSetTitleP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

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
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

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
  UiTestApplication application;
  ChartView  chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries series    = LineSeries::New();
  series.SetName(Dali::String("series1"));

  chartView.AddSeries(series);
  bool removed = chartView.RemoveSeries(Dali::String("series1"));
  DALI_TEST_CHECK(removed);
  END_TEST;
}

int UtcDaliChartViewRemoveSeriesN(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  bool removed = chartView.RemoveSeries(Dali::String("nonexistent"));
  DALI_TEST_CHECK(!removed);
  END_TEST;
}

int UtcDaliChartViewRemoveAllSeriesP(void)
{
  UiTestApplication application;
  ChartView  chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  LineSeries s1        = LineSeries::New();
  LineSeries s2        = LineSeries::New();
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
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  chartView.SetZoomMode(static_cast<int>(ChartView::ZoomMode::PAN_X));
  DALI_TEST_EQUALS(chartView.GetZoomMode(), static_cast<int>(ChartView::ZoomMode::PAN_X), TEST_LOCATION);

  chartView.SetZoomClampEnabled(false);
  DALI_TEST_CHECK(!chartView.IsZoomClampEnabled());

  chartView.SetAutoFitYOnPan(true);
  DALI_TEST_CHECK(chartView.IsAutoFitYOnPan());
  END_TEST;
}

int UtcDaliChartViewHitTestingP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

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
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

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
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  gSignalReceived = false;
  chartView.DataPointSelectedSignal().Connect(OnDataPointSelected);
  DALI_TEST_CHECK(!gSignalReceived);
  chartView.DataPointSelectedSignal().Disconnect(OnDataPointSelected);
  END_TEST;
}

int UtcDaliChartViewGaugeP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::GAUGE, Vector2(480.0f, 360.0f));

  chartView.SetGaugeValue(75.0f);
  DALI_TEST_EQUALS(chartView.GetGaugeValue(), 75.0f, 0.001f, TEST_LOCATION);

  chartView.SetGaugeMinValue(0.0f);
  chartView.SetGaugeMaxValue(100.0f);
  DALI_TEST_EQUALS(chartView.GetGaugeMinValue(), 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(chartView.GetGaugeMaxValue(), 100.0f, 0.001f, TEST_LOCATION);

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
  UiTestApplication application;
  ChartView    chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  ChartSection section   = ChartSection::New();
  section.SetXMin(1.0f).SetXMax(3.0f);

  chartView.AddSection(section);
  chartView.RemoveSection(section);
  chartView.ClearSections();
  DALI_TEST_CHECK(chartView);
  END_TEST;
}

int UtcDaliChartViewMethodChainingP(void)
{
  UiTestApplication application;
  ChartView chartView = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

  ChartView& ref = chartView.SetTitle(Dali::String("Test"))
                             .SetAnimationDuration(300.0f)
                             .SetHitThreshold(15.0f);
  DALI_TEST_CHECK(&ref == &chartView);
  DALI_TEST_EQUALS(chartView.GetTitle(), Dali::String("Test"), TEST_LOCATION);
  END_TEST;
}
