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
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <samples/chart-view/chart-case-presenter.h>
#include <samples/chart-view/chart-number-transition.h>
#include <samples/chart-view/chart-reference-cases.h>
#include <algorithm>
#include <cmath>
#include <set>

using namespace Dali;
using namespace Dali::Ui;
using namespace ChartStandard;

namespace
{
ReferenceCase FindReference(const std::string& id)
{
  const auto cases = BuildReferenceCases();
  const auto found = std::find_if(cases.begin(), cases.end(), [&id](const auto& spec)
  { return spec.id == id; });
  DALI_ASSERT_ALWAYS(found != cases.end());
  return *found;
}

void RenderReference(UiTestApplication& application, ChartView chart)
{
  application.GetScene().Add(chart);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
}
} //namespace

int UtcDaliChartReferenceCatalogCompleteness(void)
{
  const auto            cases = BuildReferenceCases();
  std::set<std::string> ids, categories;
  DALI_TEST_EQUALS(cases.size(), 141u, TEST_LOCATION);
  DALI_TEST_EQUALS(std::count_if(cases.begin(), cases.end(), [](const auto& spec)
  { return spec.webReference; }),
                   137, TEST_LOCATION);
  for(const auto& spec : cases)
  {
    DALI_TEST_CHECK(ids.insert(spec.id).second);
    categories.insert(spec.category);
    DALI_TEST_CHECK(!spec.expected.empty());
    if(spec.kind != ChartKind::NONE)
    {
      DALI_TEST_CHECK(!spec.values.empty());
      DALI_TEST_CHECK(!spec.frames.empty());
      DALI_TEST_CHECK(spec.frames.front().width > 0);
    }
  }
  DALI_TEST_EQUALS(categories.size(), 20u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferenceEmptyAxisAndSelection(void)
{
  UiTestApplication application(Components::UiConfig::New());
  for(auto preset : {AxisPreset::CENTER, AxisPreset::EDGE, AxisPreset::TIME_UNIT, AxisPreset::TIME_PERIOD})
  {
    auto spec = FindReference("line-12");
    spec.axis = preset;
    spec.values.clear();
    spec.labels.clear();
    CaseState     state;
    CasePresenter presenter(spec, state, 674.0f, 516.0f, false, true);
    RenderReference(application, presenter.GetChart());
    DALI_TEST_CHECK(presenter.IsReady());
    presenter.Select(-1);
    presenter.Select(1);
    DALI_TEST_CHECK(!state.hasSelection);
    const auto viewport = presenter.GetChart().GetViewport();
    DALI_TEST_CHECK(std::isfinite(viewport.x) && std::isfinite(viewport.z));
    DALI_TEST_CHECK(viewport.x < viewport.z);
  }
  END_TEST;
}

int UtcDaliChartAdditionalPieUpdatesKeepSliceIdentity(void)
{
  for(const auto& spec : BuildReferenceCases())
  {
    if(spec.webReference || spec.kind != ChartKind::PIE)
    {
      continue;
    }
    CaseState state;
    for(int revision = 0; revision <= 120; ++revision)
    {
      state.revision    = revision;
      const auto values = BuildValues(spec, state);
      DALI_TEST_EQUALS(values.size(), spec.labels.size(), TEST_LOCATION);
      float total = 0.0f;
      for(float value : values)
      {
        DALI_TEST_CHECK(std::isfinite(value) && value > 0.0f);
        total += value;
      }
      if(spec.unit == "%")
      {
        DALI_TEST_EQUALS(total, 100.0f, 0.001f, TEST_LOCATION);
      }
    }
  }
  END_TEST;
}

int UtcDaliChartReferenceAxisAndMultiplePointContracts(void)
{
  DALI_TEST_EQUALS(FindReference("axis-01").values.size(), 7u, TEST_LOCATION);
  DALI_TEST_CHECK(FindReference("axis-01").axis == AxisPreset::WEEK_UNIT);
  DALI_TEST_CHECK(FindReference("axis-02").axis == AxisPreset::WEEK_PERIOD);
  DALI_TEST_CHECK(FindReference("axis-06").axis == AxisPreset::CENTER);
  DALI_TEST_CHECK(FindReference("axis-07").axis == AxisPreset::EDGE);
  const auto points = FindReference("line-06");
  DALI_TEST_CHECK(points.kind == ChartKind::LINE);
  DALI_TEST_CHECK(!points.secondaryChart);
  DALI_TEST_EQUALS(points.values[0], 10.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(points.values.size(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(points.values[1], 18.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(FindReference("pattern-03").kind == ChartKind::BAR);
  DALI_TEST_CHECK(FindReference("sleep-report-06").kind == ChartKind::BAR);
  END_TEST;
}

int UtcDaliChartReferenceScaleActionsAndStatusLevels(void)
{
  DALI_TEST_CHECK(FindReference("vertical-bar-07").action == Action::X_RANGE);
  DALI_TEST_CHECK(FindReference("vertical-bar-08").action == Action::Y_RANGE);
  DALI_TEST_CHECK(FindReference("custom-05").action == Action::Y_RANGE);
  std::set<uint32_t> colors;
  for(float value : {5.0f, 25.0f, 45.0f, 65.0f, 85.0f, 99.0f})
  {
    colors.insert(StatusColor(value, 3));
  }
  DALI_TEST_EQUALS(colors.size(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferenceBoundaryAnnotations(void)
{
  const auto near  = FindReference("annotation-09");
  const auto exact = FindReference("annotation-10");
  DALI_TEST_EQUALS(near.annotations.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(near.annotations[0].y, 97.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(near.annotations[1].y, 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(exact.annotations[0].y, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(exact.annotations[1].y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(FindReference("annotation-14").annotations[0].ellipsis);
  const auto peak = FindReference("annotation-11").annotations[0];
  DALI_TEST_EQUALS(peak.text, std::string("Peak"), TEST_LOCATION);
  DALI_TEST_EQUALS(peak.x, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(peak.y, 7.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(FindReference("annotation-12").annotations[0].point, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(FindReference("annotation-13").annotations[0].text, std::string("Low Point"), TEST_LOCATION);
  DALI_TEST_EQUALS(FindReference("annotation-08").annotations.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(FindReference("vertical-bar-29").annotations.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(FindReference("line-14").annotations.size(), 3u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferenceStackedSegmentsFitScale(void)
{
  for(const auto& id : {"stacked-bar-01", "stacked-bar-02", "stacked-bar-03", "stacked-bar-04", "pattern-19", "a11y-02"})
  {
    const auto spec = FindReference(id);
    DALI_TEST_EQUALS(spec.values.size(), 7u, TEST_LOCATION);
    DALI_TEST_EQUALS(spec.values[0], 1.8f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(spec.secondary[0], 0.6f, 0.001f, TEST_LOCATION);
    for(size_t index = 0; index < spec.values.size(); ++index)
    {
      const float total = spec.values[index] + spec.secondary[index] + (index < spec.tertiary.size() ? spec.tertiary[index] : 0.0f);
      DALI_TEST_CHECK(total > spec.minimum && total < spec.maximum);
    }
  }
  DALI_TEST_EQUALS(FindReference("stacked-bar-05").values.size(), 12u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferenceNumberTargetsAndInput(void)
{
  const float expectedFrom[] = {100, 32, 1200, 100, 1000, 0, 18.2f, 900, 1024};
  const float expectedTo[]   = {220, 25, 1800, 3500, 0, 10000, 26.5f, 1024.5f, 24500};
  const auto  cases          = BuildReferenceCases();
  int         index          = 0;
  for(const auto& spec : cases)
  {
    if(spec.category != "Interaction")
    {
      continue;
    }
    DALI_TEST_EQUALS(spec.from, expectedFrom[index], 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(spec.to, expectedTo[index], 0.001f, TEST_LOCATION);
    DALI_TEST_CHECK(spec.numberMode == (index < 5 ? NumberMode::REVOLVE : index < 8 ? NumberMode::COUNT
                                                                                    : NumberMode::FADE));
    ++index;
  }
  double value = 17;
  DALI_TEST_CHECK(!ParseNumber("nan", value));
  DALI_TEST_CHECK(!ParseNumber("22x", value));
  DALI_TEST_CHECK(!ParseNumber("", value));
  DALI_TEST_EQUALS(value, 17.0, TEST_LOCATION);
  DALI_TEST_CHECK(ParseNumber("-25.5", value));
  DALI_TEST_EQUALS(value, -25.5, TEST_LOCATION);
  DALI_TEST_EQUALS(FormatNumber(10000, 0), std::string("10000"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferenceNativeNumberTransitions(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              root = AbsoluteLayout::New();
  NumberTransition  number(root, 0, 0, 300, Color::BLACK);
  number.SetValue(32, 0);
  number.Start(25, NumberMode::REVOLVE, 0, 0.5f);
  number.Advance(0.25f);
  DALI_TEST_CHECK(number.GetValue() > 25 && number.GetValue() < 32);
  DALI_TEST_CHECK(number.GetView().GetChildCount() > 2u);
  number.Advance(0.25f);
  DALI_TEST_EQUALS(number.GetValue(), 25.0, TEST_LOCATION);
  DALI_TEST_CHECK(!number.IsRunning());
  number.SetValue(18.2, 1);
  number.Start(26.5, NumberMode::COUNT, 1, 0.5f);
  number.Advance(0.5f);
  DALI_TEST_EQUALS(number.GetValue(), 26.5, TEST_LOCATION);
  number.SetValue(1024, 0);
  number.Start(24500, NumberMode::FADE, 0, 0.5f);
  number.Advance(0.25f);
  DALI_TEST_EQUALS(number.GetView().GetChildAt(0).GetProperty<float>(Actor::Property::COLOR_MULTIPLIER_ALPHA), 0.5f, 0.001f, TEST_LOCATION);
  number.Advance(0.25f);
  DALI_TEST_EQUALS(number.GetValue(), 24500.0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferenceTabsAndAnnouncements(void)
{
  auto      spec = FindReference("custom-06");
  CaseState state;
  state.period     = 1;
  const auto empty = BuildValues(spec, state);
  DALI_TEST_EQUALS(empty.size(), 20u, TEST_LOCATION);
  DALI_TEST_CHECK(std::all_of(empty.begin(), empty.end(), [](float value)
  { return std::isnan(value); }));
  state.period     = 2;
  const auto month = BuildValues(spec, state);
  DALI_TEST_EQUALS(month.size(), 31u, TEST_LOCATION);
  DALI_TEST_CHECK(month.front() >= 200 && month.front() <= 800);
  spec = FindReference("a11y-07");
  DALI_TEST_CHECK(spec.navigation == Navigation::SWIPE);
  DALI_TEST_CHECK(spec.customAnnouncement);
  DALI_TEST_CHECK(PageCount(spec) > 1);
  DALI_TEST_CHECK(PageAnnouncement(spec, 1).find("Now viewing items") != std::string::npos);
  END_TEST;
}

int UtcDaliChartContentUpdatedRestoresSelection(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::LINE, Vector2(360, 240));
  auto              line  = LineSeries::New();
  line.SetValues({10, 20, 30});
  chart.AddSeries(line);
  ConnectionTracker tracker;
  int               updates    = 0;
  int               selections = 0;
  chart.DataPointSelectedSignal().Connect(&tracker, [&](ChartView, const ChartPointEventArgs& args)
  {
    ++selections;
    DALI_TEST_EQUALS(args.GetPointIndex(), 1, TEST_LOCATION);
  });
  chart.ContentUpdatedSignal().Connect(&tracker, [&](ChartView ready)
  {
    ++updates;
    DALI_TEST_CHECK(ready.SelectPoint(0, 1));
  });
  RenderReference(application, chart);
  DALI_TEST_EQUALS(updates, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(selections, 1, TEST_LOCATION);
  chart.SetViewport(Vector4(-.5f, 0, 2.5f, 100));
  application.SendNotification();
  application.Render();
  application.RunIdles();
  DALI_TEST_EQUALS(updates, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(selections, 2, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartAnnotationTextWidthAndOverflow(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::LINE, Vector2(360, 240));
  auto              line  = LineSeries::New();
  line.SetValues({10, 20, 30});
  chart.AddSeries(line);
  auto annotation = ChartAnnotation::New(ChartAnnotation::Type::TEXT);
  annotation.SetDataPoint(line, 1);
  annotation.SetText("A long annotation label that must be constrained");
  annotation.SetTextMaximumWidth(90);
  annotation.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  chart.AddAnnotation(annotation);
  RenderReference(application, chart);
  bool found = false;
  for(uint32_t i = 0; i < chart.GetChildCount(); ++i)
  {
    auto label = Label::DownCast(chart.GetChildAt(i));
    if(label && label.GetText() == annotation.GetText())
    {
      found = true;
      DALI_TEST_CHECK(label.GetProperty<Vector3>(Actor::Property::SIZE).x <= 90.01f);
      DALI_TEST_CHECK(label.GetTextOverflowMode() == Text::OverflowMode::ELLIPSIS);
    }
  }
  DALI_TEST_CHECK(found);
  DALI_TEST_EQUALS(annotation.GetTextMaximumWidth(), 90.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferencePresenterScaleAxisIsolation(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("vertical-bar-08");
  CaseState         state;
  CasePresenter     presenter(spec, state, 674, 516, false, true);
  RenderReference(application, presenter.GetChart());
  const auto before = presenter.GetChart().GetViewport();
  presenter.Update();
  application.SendNotification();
  application.Render();
  application.RunIdles();
  const auto after = presenter.GetChart().GetViewport();
  DALI_TEST_EQUALS(before.x, after.x, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(before.z, after.z, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(after.w, 100.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartReferencePresenterSelectionAcrossTheme(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("vertical-bar-01");
  CaseState         state;
  {
    CasePresenter light(spec, state, 674, 516, false, true);
    RenderReference(application, light.GetChart());
    light.Select(1);
    light.Select(1);
    DALI_TEST_CHECK(state.hasSelection);
    DALI_TEST_EQUALS(state.selected, 1u, TEST_LOCATION);
  }
  {
    CasePresenter dark(spec, state, 674, 516, true, true);
    RenderReference(application, dark.GetChart());
    DALI_TEST_CHECK(dark.IsReady());
    DALI_TEST_CHECK(dark.Snapshot().find("Select a value") == std::string::npos);
    DALI_TEST_EQUALS(state.selected, 1u, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliChartReferencePresenterNumberInputPreserved(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("interaction-01");
  CaseState         state;
  {
    CasePresenter presenter(spec, state, 674, 516, false, true);
    presenter.Update();
    presenter.Advance(1);
    DALI_TEST_EQUALS(state.numberValue, 220.0, TEST_LOCATION);
    for(uint32_t i = 0; i < presenter.GetView().GetChildCount(); ++i)
    {
      auto input = InputField::DownCast(presenter.GetView().GetChildAt(i));
      if(input)
      {
        input.SetText("3500");
      }
    }
    presenter.Update();
    presenter.Advance(1);
    DALI_TEST_EQUALS(state.numberValue, 3500.0, TEST_LOCATION);
  }
  {
    CasePresenter presenter(spec, state, 674, 516, true, true);
    DALI_TEST_EQUALS(state.numberValue, 3500.0, TEST_LOCATION);
    DALI_TEST_EQUALS(state.numberInput, std::string("3500"), TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliChartReferenceTabRestoredAfterRecreation(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("custom-06");
  CaseState         state;
  {
    CasePresenter presenter(spec, state, 674, CardHeight(spec), false, true);
    RenderReference(application, presenter.GetChart());
    presenter.Update();
    presenter.Update();
    application.SendNotification();
    application.Render();
    application.RunIdles();
    DALI_TEST_EQUALS(state.period, 2, TEST_LOCATION);
    DALI_TEST_EQUALS(presenter.GetChart().GetViewport().w, 800.0f, 0.001f, TEST_LOCATION);
  }
  {
    CasePresenter presenter(spec, state, 674, CardHeight(spec), true, true);
    RenderReference(application, presenter.GetChart());
    const auto range = presenter.GetChart().GetViewport();
    DALI_TEST_EQUALS(range.z, 30.5f, 0.001f, TEST_LOCATION);
    DALI_TEST_EQUALS(range.w, 800.0f, 0.001f, TEST_LOCATION);
    ConnectionTracker tracker;
    bool              selected = false;
    presenter.GetChart().DataPointSelectedSignal().Connect(&tracker, [&](ChartView, const ChartPointEventArgs& args)
    {
      selected = true;
      DALI_TEST_EQUALS(args.GetXLabel(), Dali::String("Day 1"), TEST_LOCATION);
    });
    presenter.Select(1);
    DALI_TEST_CHECK(selected);
  }
  END_TEST;
}

int UtcDaliChartReferenceAllCompanionChartsUpdated(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("pattern-14");
  CaseState         state;
  CasePresenter     presenter(spec, state, 674, CardHeight(spec), false, true);
  application.GetScene().Add(presenter.GetView());
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
  application.RunIdles();
  DALI_TEST_CHECK(presenter.IsReady());
  std::vector<ChartView> charts;
  for(uint32_t i = 0; i < presenter.GetView().GetChildCount(); ++i)
  {
    auto chart = ChartView::DownCast(presenter.GetView().GetChildAt(i));
    if(chart)
    {
      charts.push_back(chart);
      DALI_TEST_CHECK(chart.GetProperty<Vector3>(Actor::Property::POSITION).y + chart.GetProperty<Vector3>(Actor::Property::SIZE).y < CardHeight(spec) - 80.0f);
    }
  }
  DALI_TEST_EQUALS(charts.size(), 3u, TEST_LOCATION);
  ConnectionTracker tracker;
  float             selected = -1.0f;
  charts.back().DataPointSelectedSignal().Connect(&tracker, [&](ChartView, const ChartPointEventArgs& args)
  { selected = args.GetDataY(); });
  DALI_TEST_CHECK(charts.back().SelectPoint(0, 0));
  DALI_TEST_EQUALS(selected, 15.46244856f, 0.001f, TEST_LOCATION);
  presenter.Update();
  application.SendNotification();
  application.Render();
  application.RunIdles();
  DALI_TEST_CHECK(charts.back().SelectPoint(0, 0));
  DALI_TEST_CHECK(std::abs(selected - 15.46244856f) > 0.01f);
  END_TEST;
}

int UtcDaliChartReferenceVariablePageViewport(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("line-14");
  CaseState         state;
  DALI_TEST_EQUALS(PageCount(spec), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(PageSize(spec, 0), 6, TEST_LOCATION);
  DALI_TEST_EQUALS(PageSize(spec, 1), 3, TEST_LOCATION);
  CasePresenter presenter(spec, state, 674, CardHeight(spec), false, true);
  RenderReference(application, presenter.GetChart());
  presenter.Navigate(1);
  application.SendNotification();
  application.Render();
  application.RunIdles();
  DALI_TEST_EQUALS(state.page, 1, TEST_LOCATION);
  const auto range = presenter.GetChart().GetViewport();
  DALI_TEST_EQUALS(range.x, 5.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(range.z, 8.5f, 0.001f, TEST_LOCATION);
  presenter.Navigate(1);
  DALI_TEST_EQUALS(state.page, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliChartSelectPointOutsideViewport(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart = ChartView::New(ChartView::Type::LINE, Vector2(360, 240));
  auto              line  = LineSeries::New();
  line.SetValues({10, 20, 30});
  chart.AddSeries(line);
  chart.SetViewport(Vector4(-.5f, 0, 1.5f, 100));
  RenderReference(application, chart);
  DALI_TEST_CHECK(chart.SelectPoint(0, 1));
  DALI_TEST_CHECK(!chart.SelectPoint(0, 2));
  END_TEST;
}

int UtcDaliChartReferenceLogViewportRestored(void)
{
  UiTestApplication application(Components::UiConfig::New());
  for(const auto& spec : BuildReferenceCases())
  {
    if(!spec.logarithmic)
    {
      continue;
    }
    CaseState state;
    Vector4   range;
    {
      CasePresenter presenter(spec, state, 674, CardHeight(spec), false, true);
      RenderReference(application, presenter.GetChart());
      range = presenter.GetChart().GetViewport();
      DALI_TEST_CHECK(range.y > 0.0f && range.w > range.y);
      if(spec.id == "scale-01")
      {
        DALI_TEST_CHECK(range.y <= 0.02f);
        DALI_TEST_CHECK(range.x < 0.0f && range.z > 32.0f);
        DALI_TEST_CHECK(presenter.GetChart().SelectPoint(0, 31));
      }
    }
    {
      CasePresenter presenter(spec, state, 674, CardHeight(spec), true, true);
      RenderReference(application, presenter.GetChart());
      DALI_TEST_EQUALS(presenter.GetChart().GetViewport(), range, 0.001f, TEST_LOCATION);
      DALI_TEST_CHECK(presenter.IsReady());
    }
  }
  END_TEST;
}

int UtcDaliChartContentUpdatedUsesPendingDataRange(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              chart  = ChartView::New(ChartView::Type::LINE, Vector2(360, 240));
  auto              series = LineSeries::New();
  chart.AddSeries(series);
  series.SetValues({10, 20, 30, 40, 50});
  ConnectionTracker tracker;
  int               updates = 0;
  chart.ContentUpdatedSignal().Connect(&tracker, [&](ChartView ready)
  {
    ++updates;
    const auto range = ready.GetViewport();
    DALI_TEST_CHECK(range.x <= 0.0f && range.z >= 4.0f);
    DALI_TEST_CHECK(ready.SelectPoint(0, 4));
  });
  RenderReference(application, chart);
  DALI_TEST_CHECK(updates > 0);
  END_TEST;
}

int UtcDaliChartReferenceAxesAndMultilineTicks(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("pattern-14");
  auto              week = FindReference("axis-01");
  CaseState         state;
  CasePresenter     presenter(week, state, 674, CardHeight(week), false, true);
  RenderReference(application, presenter.GetChart());
  bool sunday = false;
  for(uint32_t i = 0; i < presenter.GetChart().GetChildCount(); ++i)
  {
    auto label = Label::DownCast(presenter.GetChart().GetChildAt(i));
    if(label && label.GetText() == "S")
    {
      sunday = true;
    }
  }
  DALI_TEST_CHECK(sunday);
  DALI_TEST_CHECK(!FindReference("line-01").xAxis.visible);
  DALI_TEST_CHECK(!spec.xAxis.visible);
  DALI_TEST_CHECK(!spec.companions[0].xAxis.visible);
  DALI_TEST_CHECK(spec.companions[1].xAxis.visible);
  auto chart = ChartView::New(ChartView::Type::LINE, Vector2(360, 240));
  auto line  = LineSeries::New();
  line.SetValues({10, 20, 30});
  chart.AddSeries(line);
  auto axis = ChartAxis::New();
  axis.SetTickLabel(1, "12AM\n2026", Color::BLACK);
  chart.SetXAxis(axis);
  RenderReference(application, chart);
  bool found = false;
  for(uint32_t i = 0; i < chart.GetChildCount(); ++i)
  {
    auto label = Label::DownCast(chart.GetChildAt(i));
    if(label && label.GetText() == "12AM\n2026")
    {
      found = true;
      DALI_TEST_CHECK(label.GetProperty<bool>(Label::Property::MULTI_LINE));
      auto single = Label::New("12AM");
      single.SetFontSize(label.GetFontSize());
      const auto measured = single.Measure(360.0f, 240.0f);
      DALI_TEST_CHECK(label.GetProperty<Vector3>(Actor::Property::SIZE).y > measured.height + 0.5f);
    }
  }
  DALI_TEST_CHECK(found);
  END_TEST;
}

int UtcDaliChartReferenceResponsiveMetricRows(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto              spec = FindReference("responsive-01");
  CaseState         state;
  CasePresenter     presenter(spec, state, 448, CardHeight(spec), false, true);
  application.GetScene().Add(presenter.GetView());
  application.SendNotification();
  application.Render();
  application.RunIdles();
  application.SendNotification();
  application.Render();
  application.RunIdles();
  Label first, second;
  for(uint32_t i = 0; i < presenter.GetView().GetChildCount(); ++i)
  {
    auto label = Label::DownCast(presenter.GetView().GetChildAt(i));
    if(label && label.GetText() == "100 Unit")
    {
      first = label;
    }
    if(label && label.GetText() == "100 kWh")
    {
      second = label;
    }
  }
  DALI_TEST_CHECK(first && second);
  const auto start = first.GetProperty<Vector3>(Actor::Property::POSITION);
  const auto size  = first.GetProperty<Vector3>(Actor::Property::SIZE);
  const auto next  = second.GetProperty<Vector3>(Actor::Property::POSITION);
  DALI_TEST_CHECK(next.x >= start.x + size.x);
  END_TEST;
}

int UtcDaliChartReferenceSleepAreaBounds(void)
{
  const auto spec = FindReference("sleep-report-01");
  DALI_TEST_CHECK(!spec.logarithmic);
  DALI_TEST_CHECK(spec.annotations.front().hasXRange);
  DALI_TEST_EQUALS(spec.annotations.front().minimumX, 11.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(spec.annotations.front().maximumX, 19.5f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(spec.annotations.front().y, 35.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(spec.annotations.front().maximumY, 55.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}
