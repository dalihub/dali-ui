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
 */
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-components/integration-api/chart/chart-bar-geometry.h>
#include <dali-ui-components/integration-api/chart/chart-hit-tester.h>
#include <dali-ui-components/integration-api/chart/chart-layout-manager.h>
#include <dali-ui-components/integration-api/chart/chart-pie-geometry.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
#include <dali-ui-test-suite-utils.h>
#include <limits>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;
namespace ChartIntegration = Dali::Ui::Integration;

namespace
{
void RenderAppearance(UiTestApplication& application, ChartView chart)
{
  application.GetScene().Add(chart);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
}

ChartIntegration::ChartModel AppearanceModel(ChartSeries series)
{
  ChartIntegration::ChartModel model;
  model.mXAxis = ChartAxis::New();
  model.mYAxis = ChartAxis::New();
  model.AddSeries(series);
  return model;
}

/**
 * @brief Restores the global color override even when an assertion ends a test.
 */
struct ScopedChartColorOverride
{
  ~ScopedChartColorOverride()
  {
    UiColorManager::Get().ClearColorOverride();
  }
};

bool BlueChartColor(StringView id, Vector4& color)
{
  if(id == StringView("ChartAccent"))
  {
    color = Color::BLUE;
    return true;
  }
  return false;
}

bool RedChartColor(StringView id, Vector4& color)
{
  if(id == StringView("ChartAccent"))
  {
    color = Color::RED;
    return true;
  }
  return false;
}
} // namespace

int UtcDaliChartAppearanceUiColorTokensAndRuntimeOverride(void)
{
  UiTestApplication        application(Components::UiConfig::New());
  ScopedChartColorOverride reset;
  auto                     manager = UiColorManager::Get();
  manager.SetColorOverride(&BlueChartColor);
  const UiColor accent("ChartAccent");
  auto          style = ChartViewStyle::Builder().SetBackgroundColor(accent).SetLabelColor(accent).SetLegendTextColor(accent).SetTooltipBackgroundColor(accent).SetTooltipTextColor(accent).Build();
  auto          clone = style.Configure().SetPadding(16.0f).Build();
  DALI_TEST_EQUALS(clone.GetLabelColor().GetColorId(), String("ChartAccent"), TEST_LOCATION);
  DALI_TEST_EQUALS(clone.GetLegendTextColor().GetColorId(), String("ChartAccent"), TEST_LOCATION);
  DALI_TEST_EQUALS(clone.GetTooltipBackgroundColor().GetColorId(), String("ChartAccent"), TEST_LOCATION);
  auto chart = ChartView::New(ChartView::Type::BAR, Vector2(480.0f, 360.0f), clone);
  auto bar   = BarSeries::New();
  bar.SetValues({10.0f, 20.0f});
  bar.SetPointColor(0u, accent);
  chart.AddSeries(bar);
  auto axis = ChartAxis::New();
  axis.SetLabelColor(accent);
  axis.SetTickLabel(10.0f, "Token tick", accent);
  chart.SetYAxis(axis);
  auto note = ChartAnnotation::New(ChartAnnotation::Type::TEXT);
  note.SetText("Token annotation");
  note.SetPosition(Vector2(0.0f, 10.0f));
  note.SetColor(accent);
  chart.AddAnnotation(note);
  uint32_t          updates = 0u;
  ConnectionTracker tracker;
  chart.ContentUpdatedSignal().Connect(&tracker, [&updates](ChartView)
  { ++updates; });
  RenderAppearance(application, chart);
  DALI_TEST_EQUALS(chart.GetProperty<Vector4>(ChartView::Property::BACKGROUND_COLOR), Color::BLUE, TEST_LOCATION);
  const auto before = updates;
  manager.SetColorOverride(&RedChartColor);
  application.RunIdles();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(updates > before);
  DALI_TEST_EQUALS(chart.GetProperty<Vector4>(ChartView::Property::BACKGROUND_COLOR), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(bar.GetPointColor(0u).GetColorId(), String("ChartAccent"), TEST_LOCATION);
  DALI_TEST_EQUALS(bar.GetPointColor(0u).GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.GetLabelColor().GetColorId(), String("ChartAccent"), TEST_LOCATION);
  uint32_t tokenLabels = 0u;
  for(uint32_t i = 0u; i < chart.GetChildCount(); ++i)
  {
    auto label = Label::DownCast(chart.GetChildAt(i));
    if(label && (label.GetText() == "Token tick" || label.GetText() == "Token annotation"))
    {
      DALI_TEST_EQUALS(label.GetTextColor().GetColorId(), String("ChartAccent"), TEST_LOCATION);
      DALI_TEST_EQUALS(label.GetTextColor().GetRgba(), Color::RED, TEST_LOCATION);
      ++tokenLabels;
    }
  }
  DALI_TEST_EQUALS(tokenLabels, 2u, TEST_LOCATION);
  chart.SetProperty(ChartView::Property::BACKGROUND_COLOR, Color::GREEN);
  manager.SetColorOverride(&BlueChartColor);
  application.RunIdles();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(chart.GetProperty<Vector4>(ChartView::Property::BACKGROUND_COLOR), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetBackgroundColor().GetColorId(), String("ChartAccent"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearanceUiColorBarGeometryAndBorders(void)
{
  UiTestApplication        application(Components::UiConfig::New());
  ScopedChartColorOverride reset;
  auto                     manager = UiColorManager::Get();
  manager.SetColorOverride(&BlueChartColor);
  const UiColor accent("ChartAccent");
  auto          bar = BarSeries::New();
  bar.SetValues({10.0f, 20.0f});
  bar.SetPointColor(0u, accent);
  bar.SetBorder(1.0f, accent);
  bar.SetPointBorder(1u, 2.0f, accent, 2.0f, 2.0f);
  auto                          model = AppearanceModel(bar);
  ChartIntegration::ScaleEngine scale;
  scale.SetDataRange(-0.5f, 1.5f, 0.0f, 30.0f);
  auto geometry = ChartIntegration::BuildChartBars(model, scale);
  DALI_TEST_EQUALS(geometry.front().color, Color::BLUE, TEST_LOCATION);
  manager.SetColorOverride(&RedChartColor);
  geometry = ChartIntegration::BuildChartBars(model, scale);
  DALI_TEST_EQUALS(geometry.front().color, Color::RED, TEST_LOCATION);
  auto& impl = GetImplementation(bar);
  DALI_TEST_EQUALS(impl.GetPointBorder(0u).color.GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).color.GetColorId(), String("ChartAccent"), TEST_LOCATION);
  bar.ClearPointColors();
  bar.ClearPointBorder(1u);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).width, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).color.GetColorId(), String("ChartAccent"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearancePieAndDonutHitGeometry(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              pie = PieSeries::New();
  pie.AddSlice("A", 60.0f, Color::BLUE);
  pie.AddSlice("B", 40.0f, Color::GREEN);
  pie.SetInnerRadiusRatio(0.5f);
  pie.SetSliceGap(4.0f);
  auto model       = AppearanceModel(pie);
  model.mChartType = static_cast<int>(ChartView::Type::PIE);
  ChartIntegration::ScaleEngine scale;
  scale.SetPlotArea(Rect<float>(0, 0, 200, 200));
  ChartIntegration::ChartHitTester tester;
  const auto                       first = tester.FindNearest(Vector2(170, 100), model, scale);
  DALI_TEST_CHECK(first.isValid);
  DALI_TEST_EQUALS(first.pointIndex, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(first.xLabel, Dali::String("A"), TEST_LOCATION);
  DALI_TEST_EQUALS(first.dataY, 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(first.seriesColor, Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(tester.FindNearest(Vector2(30, 100), model, scale).pointIndex, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!tester.FindNearest(Vector2(100, 100), model, scale).isValid);
  DALI_TEST_CHECK(!tester.FindNearest(Vector2(201, 100), model, scale).isValid);
  DALI_TEST_CHECK(!tester.FindNearest(Vector2(100, 30), model, scale).isValid);
  DALI_TEST_EQUALS(tester.FindBySameX(Vector2(170, 100), model, scale, 30, false).size(), 1u, TEST_LOCATION);
  pie.SetInnerRadiusRatio(0);
  DALI_TEST_CHECK(tester.FindNearest(Vector2(100, 100), model, scale).isValid);
  pie.SetVisible(false);
  DALI_TEST_CHECK(!tester.FindNearest(Vector2(170, 100), model, scale).isValid);
  pie.SetVisible(true);
  pie.ClearSlices();
  for(float value : {63.0f, 19.0f, 11.0f, 4.0f, 3.0f})
  {
    pie.AddSlice("Slice", value, Color::BLUE);
  }
  pie.SetInnerRadiusRatio(.5f);
  float boundary = 270.0f;
  for(float value : {63.0f, 19.0f, 11.0f, 4.0f, 3.0f})
  {
    const float   radians = boundary * Math::PI / 180.0f;
    const Vector2 gap     = Vector2(100, 100) + Vector2(std::cos(radians), std::sin(radians)) * 70.0f;
    DALI_TEST_CHECK(!tester.FindNearest(gap, model, scale).isValid);
    boundary += value * 3.6f;
  }
  END_TEST;
}

int UtcDaliChartAppearanceRoundedBodyHitAndPointColor(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              series = BarSeries::New();
  series.SetValues({80.0f, -10.0f});
  series.SetColor(Color::BLUE);
  series.SetPointColor(0u, Color::RED);
  series.SetCornerRadius(20.0f);
  auto                          model = AppearanceModel(series);
  ChartIntegration::ScaleEngine scale;
  scale.SetPlotArea(Rect<float>(0.0f, 0.0f, 200.0f, 120.0f));
  scale.SetDataRange(-0.5f, 1.5f, -20.0f, 100.0f);
  const auto bars = ChartIntegration::BuildChartBars(model, scale);
  DALI_TEST_EQUALS(bars.size(), 2u, TEST_LOCATION);
  const Vector2 inside(bars[0].bounds.x + bars[0].bounds.width * 0.5f,
                       bars[0].bounds.y + bars[0].bounds.height * 0.5f);
  DALI_TEST_CHECK(!bars[0].Contains(Vector2(bars[0].bounds.x, bars[0].bounds.y)));
  ChartIntegration::ChartHitTester tester;
  const auto                       hit = tester.FindNearest(inside, model, scale, 0.0f);
  DALI_TEST_CHECK(hit.isValid);
  DALI_TEST_EQUALS(hit.pointIndex, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(hit.seriesColor, Color::RED, TEST_LOCATION);
  const Vector2 negative(bars[1].bounds.x + bars[1].bounds.width * 0.5f,
                         bars[1].bounds.y + bars[1].bounds.height * 0.5f);
  DALI_TEST_EQUALS(tester.FindNearest(negative, model, scale, 0.0f).dataY, -10.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearanceStackedSegmentHit(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              lower = BarSeries::New();
  lower.SetStacked(true);
  lower.SetValues({40.0f});
  auto upper = BarSeries::New();
  upper.SetStacked(true);
  upper.SetValues({30.0f});
  upper.SetPointColor(0u, Color::GREEN);
  auto model = AppearanceModel(lower);
  model.AddSeries(upper);
  ChartIntegration::ScaleEngine scale;
  scale.SetPlotArea(Rect<float>(0.0f, 0.0f, 200.0f, 100.0f));
  scale.SetDataRange(-0.5f, 0.5f, 0.0f, 100.0f);
  ChartIntegration::ChartHitTester tester;
  const auto                       hit = tester.FindNearest(scale.ToCanvas(0.0f, 55.0f), model, scale, 0.0f);
  DALI_TEST_EQUALS(hit.seriesIndex, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(hit.dataY, 30.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(hit.seriesColor, Color::GREEN, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearanceRangeBarsAndReplacement(void)
{
  UiTestApplication     application(Components::UiConfig::New());
  auto                  series = BarSeries::New();
  Dali::Vector<Vector3> ranges;
  ranges.PushBack(Vector3(0.0f, 20.0f, 70.0f));
  ranges.PushBack(Vector3(1.0f, 40.0f, 50.0f));
  series.SetRangeValues(ranges);
  auto model = AppearanceModel(series);
  model.ComputeAutoRange();
  DALI_TEST_CHECK(model.mYAxis.GetMinimumLimit() > 0.0f);
  ChartIntegration::ScaleEngine scale;
  scale.SetPlotArea(Rect<float>(0.0f, 0.0f, 200.0f, 100.0f));
  scale.SetDataRange(-0.5f, 1.5f, 0.0f, 100.0f);
  ChartIntegration::ChartHitTester tester;
  DALI_TEST_CHECK(tester.FindNearest(scale.ToCanvas(0.0f, 45.0f), model, scale, 0.0f).isValid);
  DALI_TEST_CHECK(!tester.FindNearest(scale.ToCanvas(0.0f, 5.0f), model, scale, 0.0f).isValid);
  series.SetValues({10.0f});
  DALI_TEST_CHECK(!GetImplementation(series).HasRangeValues());
  END_TEST;
}

int UtcDaliChartAppearanceLogRoundTripAndInvalidValues(void)
{
  UiTestApplication             application(Components::UiConfig::New());
  ChartIntegration::ScaleEngine scale;
  scale.SetPlotArea(Rect<float>(10.0f, 20.0f, 300.0f, 240.0f));
  scale.SetLogarithmic(true, true);
  scale.SetDataRange(1.0f, 1000.0f, 1.0f, 1000.0f);
  const auto point = scale.ToCanvas(10.0f, 100.0f);
  DALI_TEST_EQUALS(point.x, 110.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(point.y, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(scale.ToDataX(point.x), 10.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(scale.ToDataY(point.y), 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!scale.IsValid(0.0f, 10.0f));
  DALI_TEST_CHECK(!scale.IsValid(10.0f, -1.0f));
  DALI_TEST_CHECK(!scale.IsValid(10.0f, std::numeric_limits<float>::infinity()));
  auto series = LineSeries::New();
  series.SetValues({0.0f, -1.0f, std::numeric_limits<float>::quiet_NaN()});
  auto model = AppearanceModel(series);
  model.mYAxis.SetScaleType(ChartAxis::ScaleType::LOGARITHMIC);
  model.ComputeAutoRange();
  DALI_TEST_CHECK(model.mYAxis.GetMinimumLimit() > 0.0f);
  DALI_TEST_CHECK(model.mYAxis.GetMaximumLimit() > model.mYAxis.GetMinimumLimit());
  END_TEST;
}

int UtcDaliChartAppearanceStyleBuilderMoveAndClone(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              original = ChartViewStyle::Builder().SetLabelColor(UiColor("OriginalLabel")).SetPadding(12.0f).Build();
  auto              source   = original.Configure();
  source.SetBackgroundColor(UiColor("BuilderBackground"))
    .SetLabelColor(UiColor("BuilderLabel"))
    .SetLegendTextColor(UiColor("BuilderLegend"))
    .SetTooltipBackgroundColor(UiColor("BuilderTooltip"))
    .SetTooltipTextColor(UiColor("BuilderTooltipText"))
    .SetPadding(18.0f)
    .SetTooltipPadding(6.0f);
  auto                    moved = ChartViewStyle::Builder(std::move(source));
  ChartViewStyle::Builder assigned;
  assigned.SetPadding(90.0f);
  assigned    = std::move(moved);
  auto result = std::move(assigned).Build();
  DALI_TEST_EQUALS(result.GetBackgroundColor().GetColorId(), String("BuilderBackground"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetLabelColor().GetColorId(), String("BuilderLabel"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetLegendTextColor().GetColorId(), String("BuilderLegend"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetTooltipBackgroundColor().GetColorId(), String("BuilderTooltip"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetTooltipTextColor().GetColorId(), String("BuilderTooltipText"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetPadding(), 18.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(result.GetTooltipPadding(), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetLabelColor().GetColorId(), String("OriginalLabel"), TEST_LOCATION);
  DALI_TEST_EQUALS(original.GetPadding(), 12.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearanceStyleBuilderInvalidAfterMoveOrBuild(void)
{
  UiTestApplication       application(Components::UiConfig::New());
  ChartViewStyle::Builder source;
  auto                    target = ChartViewStyle::Builder(std::move(source));
  auto                    style  = std::move(target).Build();
  DALI_TEST_CHECK(style);
  for(auto* builder : {&source, &target})
  {
    DALI_TEST_ASSERTION(builder->SetBackgroundColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(builder->SetLabelColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(builder->SetLegendTextColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(builder->SetTooltipBackgroundColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(builder->SetTooltipTextColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(builder->SetPadding(1.0f), "mImpl");
    DALI_TEST_ASSERTION(builder->SetTooltipPadding(1.0f), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetBackgroundColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetLabelColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetLegendTextColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetTooltipBackgroundColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetTooltipTextColor(Color::BLUE), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetPadding(1.0f), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).SetTooltipPadding(1.0f), "mImpl");
    DALI_TEST_ASSERTION(std::move(*builder).Build(), "mImpl");
    *builder = ChartViewStyle::Builder();
    DALI_TEST_CHECK(std::move(*builder).Build());
  }
  END_TEST;
}

int UtcDaliChartAppearanceAxisTickVisibilityIndependent(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              model            = AppearanceModel(BarSeries::New());
  model.mStyle.visibility.showLegend = false;
  model.mXAxis.SetMinimumLimit(0.0f);
  model.mXAxis.SetMaximumLimit(4.0f);
  model.mYAxis.SetMinimumLimit(0.0f);
  model.mYAxis.SetMaximumLimit(40.0f);
  for(uint32_t index = 0u; index < 3u; ++index)
  {
    model.mXAxis.SetTickLabel(static_cast<float>(index * 2u), "X", Color::BLACK);
    model.mYAxis.SetTickLabel(static_cast<float>(index * 20u), "Y", Color::BLACK);
  }
  ChartIntegration::ChartLayoutManager manager;
  ChartIntegration::ScaleEngine        scale;
  scale.SetDataRange(0.0f, 4.0f, 0.0f, 40.0f);
  for(auto position : {ChartAxis::Position::NEAR, ChartAxis::Position::FAR})
  {
    model.mXAxis.SetPosition(position);
    model.mYAxis.SetPosition(position);
    for(bool labelsVisible : {false, true})
    {
      model.mXAxis.SetLabelsVisible(labelsVisible);
      model.mYAxis.SetLabelsVisible(labelsVisible);
      for(bool ticksVisible : {false, true})
      {
        model.mXAxis.SetTickMarksVisible(ticksVisible);
        model.mYAxis.SetTickMarksVisible(ticksVisible);
        const auto layout = manager.ComputeLayout(Vector2(480.0f, 360.0f), model, scale);
        DALI_TEST_EQUALS(layout.xTicks.size(), 3u, TEST_LOCATION);
        DALI_TEST_EQUALS(layout.yTicks.size(), 3u, TEST_LOCATION);
        DALI_TEST_EQUALS(layout.xTickLabels.size(), labelsVisible ? 3u : 0u, TEST_LOCATION);
        DALI_TEST_EQUALS(layout.yTickLabels.size(), labelsVisible ? 3u : 0u, TEST_LOCATION);
        for(uint32_t index = 0u; index < 3u; ++index)
        {
          DALI_TEST_EQUALS(layout.xTicks[index], static_cast<float>(index * 2u), TEST_LOCATION);
          DALI_TEST_EQUALS(layout.yTicks[index], static_cast<float>(index * 20u), TEST_LOCATION);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliChartAppearanceXAxisTicksWithCulledLabels(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              model            = AppearanceModel(BarSeries::New());
  model.mStyle.visibility.showLegend = false;
  model.mXAxis.SetMinimumLimit(0.0f);
  model.mXAxis.SetMaximumLimit(8.0f);
  for(uint32_t index = 0u; index < 9u; ++index)
  {
    model.mXAxis.SetTickLabel(static_cast<float>(index), "A long category label", Color::BLACK);
  }
  ChartIntegration::ChartLayoutManager manager;
  ChartIntegration::ScaleEngine        scale;
  scale.SetDataRange(0.0f, 8.0f, 0.0f, 10.0f);
  const auto layout = manager.ComputeLayout(Vector2(180.0f, 240.0f), model, scale);
  DALI_TEST_EQUALS(layout.xTicks.size(), 9u, TEST_LOCATION);
  DALI_TEST_CHECK(!layout.xTickLabels.empty());
  DALI_TEST_CHECK(layout.xTickLabels.size() < layout.xTicks.size());
  for(uint32_t index = 0u; index < 9u; ++index)
  {
    DALI_TEST_EQUALS(layout.xTicks[index], static_cast<float>(index), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliChartAppearanceXAxisTickSourcesAndViewport(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              model            = AppearanceModel(BarSeries::New());
  model.mStyle.visibility.showLegend = false;
  model.mXAxis.SetMinimumLimit(0.0f);
  model.mXAxis.SetMaximumLimit(4.0f);
  model.mXAxis.SetLabels({"A", "B", "C", "D", "E"});
  ChartIntegration::ChartLayoutManager manager;
  ChartIntegration::ScaleEngine        scale;
  scale.SetDataRange(1.0f, 3.0f, 0.0f, 10.0f);
  auto layout = manager.ComputeLayout(Vector2(480.0f, 360.0f), model, scale);
  DALI_TEST_EQUALS(layout.xTicks.size(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.xTickLabels.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.xTickLabels.front().text, String("B"), TEST_LOCATION);
  DALI_TEST_EQUALS(layout.xTickLabels.back().text, String("D"), TEST_LOCATION);
  model.mXAxis.SetLabelsVisible(false);
  layout = manager.ComputeLayout(Vector2(480.0f, 360.0f), model, scale);
  DALI_TEST_EQUALS(layout.xTicks.size(), 5u, TEST_LOCATION);
  DALI_TEST_CHECK(layout.xTickLabels.empty());
  model.mXAxis.SetLabels({});
  scale.SetDataRange(0.0f, 4.0f, 0.0f, 10.0f);
  layout = manager.ComputeLayout(Vector2(480.0f, 360.0f), model, scale);
  DALI_TEST_CHECK(!layout.xTicks.empty());
  DALI_TEST_EQUALS(layout.xTicks.front(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.xTicks.back(), 4.0f, TEST_LOCATION);
  model.mXAxis.SetMinimumLimit(1.0f);
  model.mXAxis.SetMaximumLimit(1000.0f);
  model.mXAxis.SetScaleType(ChartAxis::ScaleType::LOGARITHMIC);
  scale.SetLogarithmic(true, false);
  scale.SetDataRange(1.0f, 1000.0f, 0.0f, 10.0f);
  layout = manager.ComputeLayout(Vector2(480.0f, 360.0f), model, scale);
  DALI_TEST_EQUALS(layout.xTicks.size(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.xTicks[0], 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.xTicks[5], 1000.0f, TEST_LOCATION);
  DALI_TEST_CHECK(layout.xTickLabels.empty());
  END_TEST;
}

int UtcDaliChartAppearanceStyleCloneAndFarTicks(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              original = ChartViewStyle::Builder().SetBackgroundColor(Color::WHITE).SetLabelColor(Color::BLUE).Build();
  auto              dark     = original.Configure().SetBackgroundColor(Color::BLACK).SetLabelColor(Color::WHITE).Build();
  DALI_TEST_EQUALS(original.GetBackgroundColor().GetRgba(), Color::WHITE, TEST_LOCATION);
  DALI_TEST_EQUALS(dark.GetBackgroundColor().GetRgba(), Color::BLACK, TEST_LOCATION);
  auto chart  = ChartView::New(ChartView::Type::BAR, Vector2(480.0f, 360.0f), dark);
  auto series = BarSeries::New();
  series.SetValues({20.0f, 50.0f, 80.0f});
  chart.AddSeries(series);
  auto axis = ChartAxis::New();
  axis.SetPosition(ChartAxis::Position::FAR);
  axis.SetTickMarksVisible(false);
  axis.SetTickLabel(20.0f, "Good / 좋음", Color::GREEN);
  axis.SetMinimumLimit(0.0f);
  axis.SetMaximumLimit(100.0f);
  chart.SetYAxis(axis);
  RenderAppearance(application, chart);
  bool found = false;
  for(uint32_t i = 0u; i < chart.GetChildCount(); ++i)
  {
    auto label = Label::DownCast(chart.GetChildAt(i));
    if(label && label.GetProperty<Dali::String>(Label::Property::TEXT) == "Good / 좋음")
    {
      found           = true;
      const auto size = label.GetProperty<Vector3>(Actor::Property::SIZE);
      DALI_TEST_CHECK(size.x > 0.0f && size.y > 0.0f);
      DALI_TEST_EQUALS(label.GetProperty<Vector4>(Label::Property::TEXT_COLOR), Color::GREEN, TEST_LOCATION);
      DALI_TEST_CHECK(label.GetProperty<Vector3>(Actor::Property::POSITION).x > 240.0f);
    }
  }
  DALI_TEST_CHECK(found);
  END_TEST;
}

int UtcDaliChartAppearanceCustomTooltipAndKeyboardSelection(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart  = ChartView::New(ChartView::Type::LINE, Vector2(300.0f, 200.0f));
  auto              series = LineSeries::New();
  series.SetValues({12.0f, 48.0f, 31.0f});
  chart.AddSeries(series);
  auto content = AbsoluteLayout::New();
  content.SetRequestedWidth(220.0f);
  content.SetRequestedHeight(64.0f);
  auto text = Label::New("before");
  text.SetLayoutParams(AbsoluteLayoutParams::New().SetX(4.0f).SetY(4.0f).SetWidth(210.0f).SetHeight(52.0f));
  content.Add(text);
  chart.SetTooltipContent(content);
  ConnectionTracker tracker;
  int               updates = 0;
  chart.TooltipUpdatedSignal().Connect(&tracker, [&](ChartView, const Dali::Vector<ChartPointEventArgs>& points)
  {
    ++updates;
    updates += points.Count() == 1u ? 0 : 100;
    text.SetText("긴 툴팁 / a long measured tooltip");
  });
  RenderAppearance(application, chart);
  DALI_TEST_CHECK(chart.SelectPoint(0u, 1u));
  DALI_TEST_EQUALS(updates, 1, TEST_LOCATION);
  DALI_TEST_CHECK(content.GetProperty<bool>(Actor::Property::VISIBLE));
  DALI_TEST_CHECK(text.GetProperty<Vector3>(Actor::Property::SIZE).x > 0.0f);
  const auto position = content.GetProperty<Vector3>(Actor::Property::POSITION);
  const auto size     = content.GetProperty<Vector3>(Actor::Property::SIZE);
  DALI_TEST_CHECK(position.x >= 0.0f && position.y >= 0.0f);
  DALI_TEST_CHECK(position.x + size.x <= 300.01f && position.y + size.y <= 200.01f);
  DALI_TEST_CHECK(!chart.SelectPoint(0u, 99u));
  chart.SetTooltipContent(View());
  DALI_TEST_CHECK(!content.GetParent());
  END_TEST;
}

int UtcDaliChartAppearanceAnnotationOwnershipAndRemoval(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::LINE, Vector2(400.0f, 300.0f));
  auto              line  = LineSeries::New();
  line.SetValues({20.0f, 40.0f, 30.0f});
  chart.AddSeries(line);
  auto         annotation = ChartAnnotation::New(ChartAnnotation::Type::TEXT);
  Dali::String text("owned text");
  annotation.SetText(text);
  text = "replacement";
  DALI_TEST_EQUALS(annotation.GetText(), Dali::String("owned text"), TEST_LOCATION);
  annotation.SetDataPoint(line, 1u);
  annotation.SetOffset(Vector2(0.0f, -20.0f));
  chart.AddAnnotation(annotation);
  chart.AddAnnotation(annotation);
  RenderAppearance(application, chart);
  const uint32_t count = chart.GetChildCount();
  chart.RemoveAnnotation(annotation);
  DALI_TEST_EQUALS(chart.GetChildCount(), count - 1u, TEST_LOCATION);
  auto shared = annotation;
  shared.SetText("shared handle");
  DALI_TEST_EQUALS(annotation.GetText(), Dali::String("shared handle"), TEST_LOCATION);
  chart.AddAnnotation(shared);
  chart.ClearAnnotations();
  END_TEST;
}

int UtcDaliChartAppearanceViewportRoundTrip(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));
  auto              line  = LineSeries::New();
  line.SetValues({10.0f, 20.0f, 30.0f});
  chart.AddSeries(line);
  RenderAppearance(application, chart);
  const Vector4 requested(0.5f, 5.0f, 1.5f, 25.0f);
  chart.SetViewport(requested);
  DALI_TEST_EQUALS(chart.GetViewport(), requested, TEST_LOCATION);
  chart.ResetZoom();
  const auto range = chart.GetViewport();
  DALI_TEST_CHECK(range.x <= 0.0f && range.z >= 2.0f && range.y <= 10.0f && range.w >= 30.0f);
  END_TEST;
}

int UtcDaliChartAppearanceDestroyWithAttachedAnnotation(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::LINE, Vector2(400.0f, 300.0f));
  auto              line  = LineSeries::New();
  line.SetValues({20.0f, 40.0f, 30.0f});
  chart.AddSeries(line);
  auto annotation = ChartAnnotation::New(ChartAnnotation::Type::TEXT);
  annotation.SetDataPoint(line, 1u);
  annotation.SetText("Retained annotation");
  chart.AddAnnotation(annotation);
  RenderAppearance(application, chart);
  chart.Unparent();
  chart.Reset();
  annotation.SetText("Chart has been destroyed");
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(annotation.GetText(), Dali::String("Chart has been destroyed"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearanceDestroyBeforeQueuedRedraw(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::BAR, Vector2(400.0f, 300.0f));
  auto              bars  = BarSeries::New();
  bars.SetValues({20.0f, 40.0f, 30.0f});
  chart.AddSeries(bars);
  application.GetScene().Add(chart);
  application.SendNotification();
  application.Render();
  chart.Unparent();
  chart.Reset();
  application.RunIdles();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(bars.IsVisible());
  END_TEST;
}

int UtcDaliChartAppearanceBarDashAndThreshold(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              bar = BarSeries::New();
  DALI_TEST_EQUALS(bar.GetBorderDash(), Vector2::ZERO, TEST_LOCATION);
  bar.SetBorderDash(2.0f, 3.0f);
  DALI_TEST_EQUALS(bar.GetBorderDash(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_ASSERTION(bar.SetBorderDash(-1.0f, 2.0f), "");
  DALI_TEST_ASSERTION(bar.SetBorderDash(1.0f, std::numeric_limits<float>::infinity()), "");
  DALI_TEST_ASSERTION(bar.SetFillThreshold(std::numeric_limits<float>::quiet_NaN(), Color::BLUE, Color::RED), "");
  bar.SetFillThreshold(50.0f, Color::BLUE, Color::RED);
  const auto& impl = GetImplementation(bar);
  DALI_TEST_CHECK(impl.mHasFillThreshold);
  DALI_TEST_EQUALS(impl.mFillThreshold, 50.0f, TEST_LOCATION);
  bar.SetFillGradient(Gradient::Linear(Vector2(0, -.5f), Vector2(0, .5f)));
  DALI_TEST_CHECK(!impl.mHasFillThreshold && impl.mHasFillGradient);
  bar.SetFillThreshold(40.0f, Color::BLUE, Color::RED);
  DALI_TEST_CHECK(impl.mHasFillThreshold && !impl.mHasFillGradient);
  bar.ClearFillThreshold();
  DALI_TEST_CHECK(!impl.mHasFillThreshold);
  END_TEST;
}

int UtcDaliChartAppearancePointBorderOverrides(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              bar = BarSeries::New();
  bar.SetValues({50.0f, 70.0f, 40.0f});
  bar.SetBorder(3.0f, Color::RED);
  bar.SetBorderDash(4.0f, 5.0f);
  auto& impl = GetImplementation(bar);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).width, 3.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).dash, Vector2(4.0f, 5.0f), TEST_LOCATION);
  ConnectionTracker tracker;
  int               changes = 0;
  impl.DataChangedSignal().Connect(&tracker, [&changes]()
  { ++changes; });
  bar.SetPointBorder(1u, 1.0f, Color::BLUE, 2.0f, 2.0f);
  DALI_TEST_EQUALS(changes, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).width, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).color.GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).dash, Vector2(2.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(0u).color.GetRgba(), Color::RED, TEST_LOCATION);
  bar.SetPointColor(1u, Color::GREEN);
  bar.ClearPointColors();
  bar.SetValues({20.0f, 30.0f});
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).color.GetRgba(), Color::BLUE, TEST_LOCATION);
  bar.SetBorder(2.0f, Color::GREEN);
  bar.ClearPointBorder(1u);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).width, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).color.GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).dash, Vector2(4.0f, 5.0f), TEST_LOCATION);
  bar.SetPointBorder(0u, 0.0f, Color::BLUE);
  bar.SetPointBorder(1u, 1.0f, Color::BLUE);
  DALI_TEST_EQUALS(impl.GetPointBorder(0u).width, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(1u).dash, Vector2::ZERO, TEST_LOCATION);
  bar.SetPointBorder(99u, 1.0f, Color::BLUE, 2.0f, 0.0f);
  DALI_TEST_EQUALS(impl.GetPointBorder(99u).dash, Vector2(2.0f, 0.0f), TEST_LOCATION);
  bar.SetPointColor(1u, Color::RED);
  bar.ClearPointBorders();
  DALI_TEST_EQUALS(impl.GetPointBorder(0u).width, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.GetPointBorder(99u).color.GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(bar.GetPointColor(1u).GetRgba(), Color::RED, TEST_LOCATION);
  const int beforeClear = changes;
  bar.ClearPointBorder(99u);
  bar.ClearPointBorders();
  DALI_TEST_EQUALS(changes, beforeClear, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAppearancePointBorderRejectsInvalidLengths(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              bar      = BarSeries::New();
  const float       nan      = std::numeric_limits<float>::quiet_NaN();
  const float       infinity = std::numeric_limits<float>::infinity();
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, -1.0f, Color::BLUE), "");
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, nan, Color::BLUE), "");
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, infinity, Color::BLUE), "");
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, 1.0f, Color::BLUE, -1.0f, 2.0f), "");
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, 1.0f, Color::BLUE, 2.0f, -1.0f), "");
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, 1.0f, Color::BLUE, nan, 2.0f), "");
  DALI_TEST_ASSERTION(bar.SetPointBorder(0u, 1.0f, Color::BLUE, 2.0f, infinity), "");
  DALI_TEST_CHECK(GetImplementation(bar).mPointBorders.empty());
  END_TEST;
}
