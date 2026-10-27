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
#include "chart-case-presenter.h"
#include <dali-ui-foundation/extension-api/accessibility.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>

using namespace Dali;
using namespace Dali::Ui;

namespace ChartStandard
{
namespace
{
void Place(View view, float x, float y, float width, float height)
{
  view.SetLayoutParams(AbsoluteLayoutParams::New().SetX(x).SetY(y).SetWidth(width).SetHeight(height));
}

Dali::Vector<float> ToVector(const std::vector<float>& values)
{
  Dali::Vector<float> result;
  result.Reserve(values.size());
  for(float value : values)
  {
    result.PushBack(value);
  }
  return result;
}

Dali::Ui::Gradient::Linear CreateAreaGradient(bool dark)
{
  Dali::Ui::Gradient::Linear gradient(Vector2(0.0f, -0.5f), Vector2(0.0f, 0.5f));
  gradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, UiColor(0x0000FFu)), Dali::Ui::Gradient::StopNode(1.0f, UiColor(dark ? 0x262626u : 0xFFFFFFu))});
  return gradient;
}

Dali::Ui::Gradient::Linear CreateRainbowGradient()
{
  Dali::Ui::Gradient::Linear gradient(Vector2(0.0f, -0.5f), Vector2(0.0f, 0.5f));
  gradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, UiColor(0xFF0000u)), Dali::Ui::Gradient::StopNode(0.5f, UiColor(0xFFFF00u)), Dali::Ui::Gradient::StopNode(1.0f, UiColor(0x0000FFu))});
  return gradient;
}
} //namespace

CasePresenter::CasePresenter(const ReferenceCase& spec, CaseState& state, float width, float height, bool dark, bool freeze)
: mSpec(spec),
  mState(state),
  mRoot(AbsoluteLayout::New()),
  mWidth(width),
  mHeight(height),
  mDark(dark),
  mFreeze(freeze)
{
  mRoot.SetRequestedWidth(width);
  mRoot.SetRequestedHeight(height);
  mRoot.SetBackgroundColor(UiColor(Color(dark ? 0x262626 : 0xFFFFFF)));
  mRoot.SetCornerRadius(width < 500.0f ? 14.0f : 20.0f);
  mRoot.SetBorderlineWidth(1.0f);
  mRoot.SetBorderlineColor(UiColor(Color(dark ? 0x3B3B40 : 0xE5E5E9)));
  mRoot.SetAccessibilityName(Dali::String(spec.title.c_str()));
  Build();
}

CasePresenter::~CasePresenter()
{
  if(mChart && mReady)
  {
    const auto range   = mChart.GetViewport();
    mState.viewport[0] = range.x;
    mState.viewport[1] = range.y;
    mState.viewport[2] = range.z;
    mState.viewport[3] = range.w;
    mState.hasViewport = true;
  }
  if(mNumber)
  {
    mState.numberValue   = mNumber->GetValue();
    mState.numberRunning = mNumber->IsRunning();
    mState.numberInput   = mInput.GetText().CStr();
  }
  mRoot.Unparent();
}

Vector4 CasePresenter::Color(uint32_t rgb, float alpha) const
{
  return UiColor(rgb, alpha).GetRgba();
}

Label CasePresenter::AddLabel(View parent, const std::string& text, float size, float x, float y, float width, float height, uint32_t color)
{
  auto label = Label::New(Dali::String(text.c_str()));
  label.SetFontSize(size);
  label.SetTextColor(UiColor(Color(color ? color : mDark ? 0xEFEFF2
                                                         : 0x222225)));
  label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  label.SetProperty(Actor::Property::SENSITIVE, false);
  Place(label, x, y, width, height);
  parent.Add(label);
  return label;
}

InteractiveView CasePresenter::AddButton(const std::string& text, float x, float y, float width, std::function<void()> callback)
{
  auto button = InteractiveView::New();
  button.SetFocusable(true);
  button.SetAccessibilityName(Dali::String(text.c_str()));
  button.SetBackgroundColor(UiColor(Color(mDark ? 0x383B44 : 0xECF1FC)));
  button.SetCornerRadius(18.0f);
  Place(button, x, y, width, 36.0f);
  auto content = AbsoluteLayout::New();
  content.SetRequestedWidth(MATCH_PARENT);
  content.SetRequestedHeight(MATCH_PARENT);
  button.Add(content);
  auto label = AddLabel(content, text, 13.0f, 0, 0, width, 36);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.ClickedSignal().Connect(this, [this, callback](View, InputEvent)
  {
    if(activated)
    {
      activated();
    }
    callback();
  });
  mRoot.Add(button);
  return button;
}

void CasePresenter::Build()
{
  auto title = AddLabel(mRoot, mSpec.title, 18.0f, 28, 16, mWidth - 56, 42);
  title.SetProperty(Label::Property::MULTI_LINE, true);
  title.SetFontWeight(Text::FontWeight::BOLD);
  const float buttonY = mHeight - 54.0f;
  mSelection          = AddLabel(mRoot, "Select a value", 11, 28, buttonY - 34, mWidth - 56, 24, mDark ? 0xACAFB9 : 0x797984);
  if(!mSpec.webReference)
  {
    BuildAdditionalChart();
    return;
  }
  if(mSpec.layout == LayoutPreset::NUMBER)
  {
    mNumber = MakeUnique<NumberTransition>(mRoot, 60, 236, mWidth - 120, Color(mDark ? 0xEFEFF2 : 0x222225));
    if(!mState.numberInitialized)
    {
      mState.numberValue       = mFreeze ? mSpec.to : mSpec.from;
      mState.numberTarget      = mSpec.to;
      mState.numberInput       = FormatNumber(mSpec.to, mSpec.fractionDigits);
      mState.numberRunning     = !mFreeze;
      mState.numberInitialized = true;
    }
    mNumber->SetValue(mState.numberValue, mSpec.fractionDigits);
    if(mState.numberRunning && !mFreeze)
    {
      mNumber->Start(mState.numberTarget, mSpec.numberMode, mSpec.fractionDigits, mSpec.duration);
    }
    AddLabel(mRoot, "Value", 14, 28, buttonY - 3, 50, 36);
    mInput = InputField::New();
    mInput.SetText(Dali::String(mState.numberInput.c_str()));
    mInput.SetFontSize(15);
    mInput.SetTextColor(UiColor(Color(mDark ? 0xEEEEEE : 0x222222)));
    mInput.SetBackgroundColor(UiColor(Color(mDark ? 0x33333A : 0xF7F8FC)));
    mInput.SetAccessibilityName("Target number");
    Place(mInput, 80, buttonY, 100, 36);
    mRoot.Add(mInput);
    AddButton("Animate", 190, buttonY, 90, [this]
    { Update(); });
    mError = AddLabel(mRoot, "", 11, 28, buttonY - 40, mWidth - 56, 28, 0xEF3136);
    mSelection.SetProperty(Actor::Property::VISIBLE, false);
    mReady = true;
    return;
  }

  if(mSpec.action == Action::TABS)
  {
    const char* periods[] = {"Day", "Week", "Month"};
    for(int period = 0; period < 3; ++period)
    {
      mTabButtons.push_back(AddButton(periods[period], 28.0f + static_cast<float>(period) * 90.0f, 76, 82, [this, period]
      { SetPeriod(period); }));
    }
    RefreshPeriodButtons();
  }
  float width  = std::min(mWidth - 56, mSpec.chartWidth);
  float height = mSpec.chartHeight;
  float x      = (mWidth - width) * 0.5f;
  float y      = 142.0f;
  // Cards without authored text slots intentionally contain only the chart.
  if(y + height > buttonY - 60)
  {
    height = std::max(80.0f, buttonY - 60 - y);
  }
  if(mSpec.secondaryChart)
  {
    width = std::max(100.0f, (mWidth - 84.0f) * 0.5f);
    x     = 28;
  }
  const float offset = (mWidth - mSpec.referenceWidth) * 0.5f;
  if(!mSpec.frames.empty())
  {
    const auto frame = mSpec.frames.front();
    width            = std::min(mWidth - 56.0f, frame.width);
    height           = frame.height;
    x                = std::clamp(offset + frame.x, 28.0f, std::max(28.0f, mWidth - 28.0f - width));
    y                = frame.y;
  }
  std::map<int, float> rowEnds;
  for(const auto& text : mSpec.texts)
  {
    float textX        = std::clamp(offset + text.x, 28.0f, mWidth - 60.0f);
    float desiredWidth = std::max(text.width + (text.tag ? 2.0f : 16.0f), text.tag ? 70.0f : 55.0f);
    if(text.tag || text.fontSize >= 20.0f)
    {
      const int row     = static_cast<int>(std::round((text.y + text.height * .5f) / 8.0f));
      auto      measure = Label::New(Dali::String(text.text.c_str()));
      measure.SetFontSize(text.fontSize);
      if(!text.tag)
      {
        measure.SetFontWeight(Text::FontWeight::BOLD);
      }
      desiredWidth = std::max(desiredWidth, measure.Measure(10000.0f, 1000.0f).width + 4.0f);
      auto found   = rowEnds.find(row);
      if(found != rowEnds.end())
      {
        textX = std::max(textX, found->second + 8.0f);
      }
      textX        = std::min(textX, mWidth - 29.0f);
      rowEnds[row] = textX + desiredWidth;
    }
    const float textWidth = std::min(mWidth - 28.0f - textX, desiredWidth);
    auto        label     = AddLabel(mRoot, text.text, text.fontSize, textX, text.y, textWidth, text.tag ? text.height : std::max(24.0f, text.height + 6.0f), text.tag ? (mDark ? text.darkColor : text.color) : mDark ? 0xEFEFF2
                                                                                                                                                                                                                       : text.color);
    if(!text.tag && text.height > 35.0f)
    {
      label.SetProperty(Label::Property::MULTI_LINE, true);
      label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    }
    if(!text.tag && text.fontSize >= 20.0f)
    {
      label.SetFontWeight(Text::FontWeight::BOLD);
    }
    if(text.tag)
    {
      label.SetBackgroundColor(UiColor(Color(mDark ? text.darkBackground : text.background)));
      label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      label.SetVerticalTextAlignment(Text::Alignment::CENTER);
      label.SetCornerRadius(6.0f);
    }
  }
  BuildChart(x, y, width, height);
  mCompanionReady.assign(mSpec.companions.size(), false);
  for(size_t i = 0; i < mSpec.companions.size(); ++i)
  {
    const auto& spec       = mSpec.companions[i];
    const auto& frame      = mSpec.frames[i + 1];
    const float chartWidth = std::min(mWidth - 56.0f, frame.width);
    const float chartX     = std::clamp(offset + frame.x, 28.0f, std::max(28.0f, mWidth - 28.0f - chartWidth));
    auto        style      = ChartViewStyle::Builder().SetBackgroundColor(Color(mDark ? 0x262626 : 0xFFFFFF)).SetLabelColor(Color(mDark ? 0xAAAAAA : 0x777777)).SetPadding(8).Build();
    auto        chart      = ChartView::New(spec.kind == ChartKind::LINE ? ChartView::Type::LINE : ChartView::Type::BAR, Vector2(chartWidth, frame.height), style);
    Place(chart, chartX, frame.y, chartWidth, frame.height);
    auto axis = ChartAxis::New();
    axis.SetMinimumLimit(spec.minimum);
    axis.SetMaximumLimit(spec.maximum);
    axis.SetPosition(ChartAxis::Position::FAR);
    axis.SetAxisLineWidth(0);
    axis.SetTickMarksVisible(false);
    axis.SetLabelColor(Color(mDark ? 0xAAAAAA : 0x888888));
    axis.SetGridColor(Color(mDark ? 0x414147 : 0xE8E8EC));
    axis.SetGridDash(4, 0);
    axis.SetLabelsVisible(spec.yAxis.visible);
    axis.SetShowGridLinesEnabled(spec.yAxis.visible && mSpec.grid);
    for(const auto& tick : spec.yAxis.ticks)
    {
      axis.SetTickLabel(tick.value, Dali::String(tick.text.c_str()), Color(mDark ? 0xAAAAAA : 0x888888));
    }
    auto xAxis = ChartAxis::New();
    xAxis.SetAxisLineWidth(0);
    xAxis.SetTickMarksVisible(false);
    xAxis.SetLabelsVisible(spec.xAxis.visible);
    for(const auto& tick : spec.xAxis.ticks)
    {
      xAxis.SetTickLabel(tick.value, Dali::String(tick.text.c_str()), Color(mDark ? 0xAAAAAA : 0x888888));
    }
    chart.SetXAxis(xAxis);
    chart.SetYAxis(axis);
    chart.SetProperty(ChartView::Property::SHOW_LEGEND, false);
    chart.SetProperty(ChartView::Property::AXIS_LABEL_SIZE, 11.0f);
    chart.SetAnimationDuration(mFreeze ? 0.0f : 500.0f);
    ChartSeries series;
    if(spec.kind == ChartKind::LINE)
    {
      auto line = LineSeries::New();
      line.SetColor(Color(spec.line.color, spec.line.alpha));
      line.SetLineWidth(spec.line.width);
      line.SetMarkersVisible(spec.line.radius > 0.0f);
      line.SetMarkerRadius(spec.line.radius);
      line.SetMarkerColor(Color(spec.line.markerColor, spec.line.markerAlpha));
      series = line;
    }
    else
    {
      auto bar = BarSeries::New();
      bar.SetCornerRadius(mSpec.cornerRadius);
      bar.SetBarGroupWidth(mSpec.groupWidth);
      series = bar;
    }
    series.SetName("Measurement");
    if(spec.kind != ChartKind::LINE)
    {
      series.SetColor(Color(mSpec.color));
    }
    ReferenceCase data;
    data.values  = spec.values;
    data.minimum = spec.minimum;
    data.maximum = spec.maximum;
    data.action  = mSpec.action;
    series.SetValues(ToVector(BuildValues(data, mState)));
    chart.AddSeries(series);
    chart.ContentUpdatedSignal().Connect(this, [this, i](ChartView)
    { mCompanionReady[i] = true; });
    mCompanions.push_back(chart);
    mCompanionSeries.push_back(series);
    mRoot.Add(chart);
  }
  if(mSpec.legend && mSpec.legendItems.empty())
  {
    const float legendY = mSpec.legendHeader ? 104.0f : buttonY - 65.0f;
    AddButton("● Measurement", 28, legendY, 132, [this]
    { ToggleSeries(0); });
    if(mSeries.size() > 1)
    {
      AddButton("● Activity", 168, legendY, 112, [this]
      { ToggleSeries(1); });
    }
    if(mSpec.legendImages)
    {
      auto image = ImageView::New(RESOURCES_DIR "chart-marker.svg");
      image.Measure(24.0f, 24.0f);
      Place(image, 288, legendY + 5, 24, 24);
      mRoot.Add(image);
    }
  }
  for(const auto& legend : mSpec.legendItems)
  {
    const float legendX = std::clamp(offset + legend.x, 28.0f, mWidth - 60.0f);
    if(legend.image)
    {
      auto image = ImageView::New(RESOURCES_DIR "chart-heart.svg");
      image.Measure(14.0f, 14.0f);
      Place(image, legendX, legend.y, 14, 14);
      mRoot.Add(image);
    }
    else
    {
      AddLabel(mRoot, "●", 11, legendX, legend.y, 12, 20, legend.color);
    }
    AddLabel(mRoot, legend.label, 11, legendX + 16, legend.y, legend.width + 10, 20, mDark ? 0xAAAAAA : 0x777777);
  }
  if(!mSpec.unit.empty() && mSpec.layout == LayoutPreset::CHART && mSpec.category == "Unit")
  {
    AddLabel(mRoot, mSpec.unit, 11, x + width - 45, y - 18, 45, 20);
  }
  AddButton(mSpec.action == Action::SHOW_HIDE ? "Show / Hide" : mSpec.action == Action::TABS ? "Period"
                                                                                             : "Update",
            28, buttonY, 94, [this]
  { Update(); });
  if(mSpec.navigation != Navigation::NONE)
  {
    AddButton("Previous", 130, buttonY, 90, [this]
    { Navigate(-1); });
    AddButton("Next", 228, buttonY, 70, [this]
    { Navigate(1); });
    AddButton("Select", 306, buttonY, 74, [this]
    { Select(1); });
  }
  else
  {
    AddButton("Legend", 130, buttonY, 84, [this]
    { ToggleSeries(0); });
    AddButton("Select", 222, buttonY, 84, [this]
    { Select(1); });
  }
}

void CasePresenter::BuildChart(float x, float y, float width, float height)
{
  auto style = ChartViewStyle::Builder().SetBackgroundColor(Color(mDark ? 0x262626 : 0xFFFFFF)).SetLabelColor(Color(mDark ? 0xAAAAAA : 0x777777)).SetLegendTextColor(Color(mDark ? 0xBBBBBB : 0x777777)).SetTooltipBackgroundColor(Color(mDark ? 0x3D414A : 0xE8F2FF)).SetTooltipTextColor(Color(mDark ? 0xFFFFFF : 0x222222)).SetPadding(8).Build();
  mChart     = ChartView::New(mSpec.kind == ChartKind::LINE ? ChartView::Type::LINE : ChartView::Type::BAR, Vector2(width, height), style);
  Place(mChart, x, y, width, height);
  mChart.SetProperty(ChartView::Property::SHOW_LEGEND, false);
  mChart.SetProperty(ChartView::Property::AXIS_LABEL_SIZE, 11.0f);
  mChart.SetProperty(ChartView::Property::HOVER_ENABLED, mSpec.interactive);
  mChart.SetProperty(ChartView::Property::TOUCH_ENABLED, mSpec.interactive);
  mChart.SetAnimationDuration(mFreeze ? 0.0f : 500.0f);
  mChart.SetFindingStrategy(mSpec.kind == ChartKind::COMBO ? ChartView::FindingStrategy::SAME_X : ChartView::FindingStrategy::NEAREST);
  mChart.SetAccessibilityName(Dali::String(mSpec.title.c_str()));
  mChart.SetAccessibilityDescription("Use arrow keys to select points and Page Up or Page Down to change pages.");
  mChart.SetAccessibilityHighlightable(true);
  ConfigureAxes();
  auto bars = [this](int number)
  {
    auto bar = BarSeries::New();
    bar.SetName(number == 0 ? "Measurement" : number == 1 ? "Activity"
                                                          : "Recovery");
    bar.SetColor(Color(number == 0 ? mSpec.color : number == 1 ? 0x78ACFF
                                                               : 0xB8D4FF,
                       mSpec.alpha));
    bar.SetCornerRadius(mSpec.axis != AxisPreset::DEFAULT ? 100.0f : mSpec.kind == ChartKind::STACKED ? 3.0f
                                                                                                      : mSpec.cornerRadius);
    bar.SetBarGroupWidth(mSpec.groupWidth);
    bar.SetStacked(mSpec.kind == ChartKind::STACKED);
    if(mSpec.border || mSpec.forecast)
    {
      bar.SetColor(Color(mSpec.color, 0.12f));
      bar.SetBorder(mSpec.forecast ? 1.0f : 2.0f, Color(mSpec.color));
      if(mSpec.forecast)
      {
        bar.SetBorderDash(2.0f, 2.0f);
      }
    }
    if(mSpec.gradientFill)
    {
      bar.SetFillGradient(CreateRainbowGradient());
    }
    if(mSpec.highlight)
    {
      const float threshold = *std::max_element(mSpec.values.begin(), mSpec.values.end()) * .5f;
      bar.SetFillThreshold(threshold, Color(0xC5D8FF), Color(0x387AFF));
    }
    mSeries.push_back(bar);
    mChart.AddSeries(bar);
  };
  auto line = [this](bool secondary)
  {
    auto series = LineSeries::New();
    series.SetName(secondary ? "Activity" : "Measurement");
    series.SetColor(Color(secondary ? 0xFF7F20 : mSpec.muted ? 0xA2AABA
                                                             : mSpec.color));
    series.SetLineWidth(2.5f);
    series.SetMarkersVisible(mSpec.markers && !mSpec.simple);
    series.SetMarkerRadius(3);
    series.SetMarkerBorderWidth(1.2f);
    series.SetMarkerBorderColor(Color(mDark ? 0x262626 : 0xFFFFFF));
    const size_t appearanceIndex = mSpec.kind == ChartKind::COMBO ? 0u : secondary ? 1u
                                                                                   : 0u;
    if(appearanceIndex < mSpec.lines.size())
    {
      const auto& appearance = mSpec.lines[appearanceIndex];
      series.SetColor(Color(appearance.color, appearance.alpha));
      series.SetLineWidth(appearance.width);
      series.SetMarkersVisible(appearance.radius > 0.0f);
      series.SetMarkerRadius(appearance.radius);
      series.SetMarkerColor(Color(appearance.markerColor, appearance.markerAlpha));
    }
    series.SetFillEnabled(mSpec.fill || mSpec.gradientFill);
    series.SetFillColor(Color(mSpec.fillColor, mSpec.fillAlpha));
    if(mSpec.lineBehind)
    {
      series.SetZIndex(-1);
    }
    if(mSpec.forecast)
    {
      series.SetLineDash(5, 5);
    }
    if(mSpec.gradientFill)
    {
      series.SetFillGradient(CreateAreaGradient(mDark));
    }
    if(mSpec.gradientStroke)
    {
      series.SetStrokeGradient(CreateRainbowGradient());
    }
    if(mSpec.segmentStyle)
    {
      series.SetSegmentStyle(0, 3, Color(0xCCCCCC));
      series.SetSegmentStyle(3, 7, Color(0xFF7F20));
      if(mSpec.values.size() > 8)
      {
        DALI_ASSERT_ALWAYS(mSpec.values.size() <= std::numeric_limits<uint32_t>::max());
        series.SetSegmentStyle(7u, static_cast<uint32_t>(mSpec.values.size() - 1u), Color(mSpec.color), 5, 4);
      }
    }
    mSeries.push_back(series);
    mChart.AddSeries(series);
  };
  if(mSpec.kind == ChartKind::LINE)
  {
    line(false);
    if(mSpec.lineSeriesCount > 1)
    {
      line(true);
    }
  }
  else
  {
    bars(0);
    if(mSpec.kind == ChartKind::STACKED)
    {
      bars(1);
      if(mSpec.segments == 3)
      {
        bars(2);
      }
    }
    if(mSpec.kind == ChartKind::COMBO)
    {
      line(true);
    }
  }
  ApplyData();
  ApplyViewport();
  BuildAnnotations();
  mChart.ContentUpdatedSignal().Connect(this, &CasePresenter::OnContentUpdated);
  mChart.DataPointSelectedSignal().Connect(this, &CasePresenter::OnSelected);
  mChart.ZoomedSignal().Connect(this, [this](ChartView chart, const Dali::Bounds&)
  {
    const auto range   = chart.GetViewport();
    mState.hasViewport = true;
    mState.viewport[0] = range.x;
    mState.viewport[1] = range.y;
    mState.viewport[2] = range.z;
    mState.viewport[3] = range.w;
  });
  auto tooltip = AbsoluteLayout::New();
  tooltip.SetRequestedWidth(190);
  tooltip.SetRequestedHeight(86);
  tooltip.SetBackgroundColor(UiColor(Color(mDark ? 0x3D414A : 0xE8F2FF)));
  tooltip.SetCornerRadius(10);
  AddLabel(tooltip, mSpec.subtitle.empty() ? "Measurement" : mSpec.subtitle, 11, 12, 4, 166, 22);
  auto value = AddLabel(tooltip, "", 20, 12, 27, 166, 30);
  AddLabel(tooltip, mSpec.description, 10, 12, 58, 166, 22, mDark ? 0xBBBBBB : 0x777777);
  mChart.SetTooltipContent(tooltip);
  mChart.TooltipUpdatedSignal().Connect(this, [this, value](ChartView, const Dali::Vector<ChartPointEventArgs>& points) mutable
  {
    if(!points.Empty())
    {
      value.SetText(Dali::String((FormatNumber(points[0].GetDataY(), 1) + " " + mSpec.unit).c_str()));
    }
  });
  if(mSpec.navigation == Navigation::SCROLL)
  {
    mChart.SetZoomMode(static_cast<int>(ChartView::ZoomMode::PAN_X));
  }
  else if(mSpec.navigation == Navigation::SWIPE)
  {
    mPan = PanGestureDetector::New();
    mPan.DetectedSignal().Connect(this, &CasePresenter::OnPan);
    mPan.Attach(mChart);
  }
  mChart.SetProperty(Actor::Property::VISIBLE, !mState.hidden);
  mRoot.Add(mChart);
  mEmpty = AddLabel(mRoot, "No data", 15, x + 12, y + height * .4f, width - 24, 28, mDark ? 0xBBBBBB : 0x777777);
  mEmpty.SetProperty(Actor::Property::VISIBLE, mSpec.action == Action::TABS && mState.period % 3 == 1);
  if((mSpec.axis == AxisPreset::TIME_UNIT || mSpec.axis == AxisPreset::TIME_PERIOD) &&
     std::none_of(mSpec.xAxis.ticks.begin(), mSpec.xAxis.ticks.end(), [](const auto& tick)
  { return tick.text.find('\n') != std::string::npos; }))
  {
    AddLabel(mRoot, mSpec.axis == AxisPreset::TIME_UNIT ? "2026" : "1/1", 10, x + width * .3f, y + height + 2, 60, 18, 0x909098);
  }
}

void CasePresenter::ConfigureAxes()
{
  mXAxis = ChartAxis::New();
  mYAxis = ChartAxis::New();
  mXAxis.SetTickMarksVisible(false);
  mYAxis.SetTickMarksVisible(false);
  mXAxis.SetAxisLineWidth(0);
  mYAxis.SetAxisLineWidth(0);
  mXAxis.SetLabelColor(Color(mDark ? 0xAAAAAA : 0x888888));
  mYAxis.SetLabelColor(Color(mDark ? 0xAAAAAA : 0x888888));
  mYAxis.SetPosition(ChartAxis::Position::FAR);
  mYAxis.SetGridColor(Color(mDark ? 0x414147 : 0xE8E8EC));
  mYAxis.SetGridDash(4, 0);
  mYAxis.SetMinimumLimit(mSpec.minimum);
  mYAxis.SetMaximumLimit(mSpec.action == Action::TABS && mState.period == 2 ? 800.0f : mSpec.maximum);
  mYAxis.SetScaleType(mSpec.logarithmic ? ChartAxis::ScaleType::LOGARITHMIC : ChartAxis::ScaleType::LINEAR);
  mYAxis.SetShowGridLinesEnabled(mSpec.grid && !mSpec.simple);
  mYAxis.SetLabelsVisible(!mSpec.simple);
  Dali::Vector<Dali::String> labels;
  const auto                 values = BuildValues(mSpec, mState);
  for(size_t i = 0; i < values.size(); ++i)
  {
    const auto text = mSpec.action == Action::TABS ? (mState.period == 0 ? "Hour " : mState.period == 1 ? "Week "
                                                                                                        : "Day ") +
                                                       std::to_string(i + 1)
                      : i < mSpec.labels.size() ? mSpec.labels[i]
                                                : std::to_string(i);
    labels.PushBack(Dali::String(text.c_str()));
  }
  mXAxis.SetLabels(labels);
  if(mSpec.axis == AxisPreset::WEEK_UNIT || mSpec.axis == AxisPreset::WEEK_PERIOD || mSpec.axis == AxisPreset::WEEK_SIMPLE)
  {
    const char* days[] = {"S", "M", "T", "W", "T", "F", "S"};
    for(int i = 0; i < 7; ++i)
    {
      mXAxis.SetTickLabel(static_cast<float>(i), days[i], Color(i == 0 ? 0xFF3030 : mDark ? 0xAAAAAA
                                                                                          : 0x888888));
    }
  }
  else if(mSpec.axis == AxisPreset::CENTER && !values.empty())
  {
    mXAxis.SetTickLabel(static_cast<float>(values.size() - 1u) * 0.5f, "This Week", Color(0x888888));
  }
  else if(mSpec.axis == AxisPreset::EDGE && !values.empty())
  {
    mXAxis.SetTickLabel(0, "Start", Color(0x888888));
    mXAxis.SetTickLabel(static_cast<float>(values.size() - 1u), "End", Color(0x888888));
  }
  else if(!values.empty() && (mSpec.axis == AxisPreset::TIME_UNIT || mSpec.axis == AxisPreset::TIME_PERIOD || mSpec.category == "Sleep Report"))
  {
    const char* times[]   = {"6PM", "12AM", "1AM", "2AM"};
    const char* periods[] = {"12AM", "1/12", "12PM", "6PM", "12AM"};
    const int   count     = mSpec.axis == AxisPreset::TIME_PERIOD ? 5 : 4;
    for(int i = 0; i < count; ++i)
    {
      mXAxis.SetTickLabel(static_cast<float>(i) * static_cast<float>(values.size() - 1u) / static_cast<float>(count - 1), mSpec.axis == AxisPreset::TIME_PERIOD ? periods[i] : times[i], Color(0x888888));
    }
  }
  if(mSpec.statusLevels)
  {
    for(int i = 0; i < mSpec.statusLevels; ++i)
    {
      const char* three[] = {"Low", "Moderate", "High"};
      const char* five[]  = {"Good", "Moderate", "Slightly unhealthy", "Unhealthy", "Very unhealthy"};
      const float y       = (static_cast<float>(i) + 0.5f) * 100.0f / static_cast<float>(mSpec.statusLevels);
      mYAxis.SetTickLabel(y, mSpec.statusLevels == 3 ? three[i] : five[i], Color(StatusColor(y, mSpec.statusLevels)));
    }
  }
  if(mSpec.action != Action::TABS)
  {
    mXAxis.SetLabelsVisible(mSpec.xAxis.visible);
    mYAxis.SetLabelsVisible(mSpec.yAxis.visible);
    mYAxis.SetShowGridLinesEnabled(mSpec.yAxis.visible && mSpec.grid);
    if(mSpec.navigation == Navigation::NONE && !mSpec.xAxis.ticks.empty())
    {
      mXAxis.ClearTickLabels();
      for(const auto& tick : mSpec.xAxis.ticks)
      {
        const bool sunday = (mSpec.axis == AxisPreset::WEEK_UNIT || mSpec.axis == AxisPreset::WEEK_PERIOD || mSpec.axis == AxisPreset::WEEK_SIMPLE) && tick.value == 0.0f;
        mXAxis.SetTickLabel(tick.value, Dali::String(tick.text.c_str()), Color(sunday ? 0xFF3030 : mDark ? 0xAAAAAA
                                                                                                         : 0x888888));
      }
    }
    if(!mSpec.yAxis.ticks.empty())
    {
      mYAxis.ClearTickLabels();
      for(const auto& tick : mSpec.yAxis.ticks)
      {
        mYAxis.SetTickLabel(tick.value, Dali::String(tick.text.c_str()), Color(mSpec.statusLevels ? StatusColor(tick.value, mSpec.statusLevels) : mDark ? 0xAAAAAA
                                                                                                                                                        : 0x888888));
      }
    }
  }
  mChart.SetXAxis(mXAxis);
  mChart.SetYAxis(mYAxis);
}

void CasePresenter::ApplyData()
{
  if(!mSpec.webReference)
  {
    ApplyAdditionalData();
    return;
  }
  for(size_t i = 0; i < mSeries.size(); ++i)
  {
    auto values = BuildValues(mSpec, mState, static_cast<uint32_t>(i));
    DALI_ASSERT_ALWAYS(values.size() <= std::numeric_limits<uint32_t>::max());
    auto bar = BarSeries::DownCast(mSeries[i]);
    if(bar && !mSpec.rangeLows.empty())
    {
      Dali::Vector<Vector3> ranges;
      for(size_t j = 0; j < values.size(); ++j)
      {
        const float low = j < mSpec.rangeLows.size() ? mSpec.rangeLows[j] : 0;
        ranges.PushBack(Vector3(static_cast<float>(j), std::min(low, values[j]), values[j]));
      }
      bar.SetRangeValues(ranges);
    }
    else
    {
      mSeries[i].SetValues(ToVector(values));
    }
    mSeries[i].ClearPointColors();
    for(size_t j = 0; j < values.size(); ++j)
    {
      if(mSpec.statusLevels)
      {
        mSeries[i].SetPointColor(static_cast<uint32_t>(j), Color(mState.revision == 0 && i == 0 && j < mSpec.pointColors.size() ? mSpec.pointColors[j] : StatusColor(values[j], mSpec.statusLevels)));
      }
      else if(mSpec.pointThemes)
      {
        mSeries[i].SetPointColor(static_cast<uint32_t>(j), Color(j < mSpec.pointColors.size() ? mSpec.pointColors[j] : mSpec.color));
      }
      else if(mSpec.category == "Sleep Report")
      {
        mSeries[i].SetPointColor(static_cast<uint32_t>(j), Color(j < 7 || j > 18 ? 0xCCCCCC : j == 11 ? 0xFF7F20
                                                                                                      : 0x387AFF));
      }
    }
    if(bar)
    {
      bar.ClearPointBorders();
      for(const auto& border : mSpec.pointBorders)
      {
        bar.SetPointBorder(border.index, border.width, Color(border.color), border.dash, border.gap);
      }
    }
    mSeries[i].SetVisible(i == 0 ? mState.primaryVisible : mState.secondaryVisible);
  }
}

void CasePresenter::ApplyViewport()
{
  if(mSpec.action == Action::TABS)
  {
    const auto values = BuildValues(mSpec, mState);
    mChart.SetViewport(Vector4(-.5f, mSpec.minimum, static_cast<float>(values.size()) - .5f, mState.period == 2 ? 800.0f : mSpec.maximum));
  }
  else if(mState.hasViewport)
  {
    mChart.SetViewport(Vector4(mState.viewport[0], mState.viewport[1], mState.viewport[2], mState.viewport[3]));
  }
  else if(mSpec.navigation != Navigation::NONE)
  {
    const float first = static_cast<float>(PageStart(mSpec, mState.page)) - .5f;
    mChart.SetViewport(Vector4(first, mSpec.minimum, first + static_cast<float>(PageSize(mSpec, mState.page)), mSpec.maximum));
  }
}

void CasePresenter::BuildAnnotations()
{
  if(mSpec.highlight && !mSpec.values.empty())
  {
    auto        section = ChartSection::New();
    const float value   = *std::max_element(mSpec.values.begin(), mSpec.values.end()) * .5f;
    section.SetMinimumY(value);
    section.SetMaximumY(value);
    section.SetStrokeColor(Color(0x000000));
    section.SetStrokeWidth(2);
    mChart.AddSection(section);
  }
  for(const auto& spec : mSpec.annotations)
  {
    if(spec.kind == AnnotationKind::LINE || spec.kind == AnnotationKind::AREA)
    {
      auto section = ChartSection::New();
      section.SetMinimumY(spec.y);
      section.SetMaximumY(spec.maximumY);
      section.SetStrokeColor(Color(spec.color));
      section.SetStrokeWidth(spec.kind == AnnotationKind::LINE ? spec.lineWidth : 0.0f);
      section.SetFillColor(Color(spec.color, 0.13f));
      section.SetCornerRadius(spec.cornerRadius);
      if(spec.hasXRange)
      {
        section.SetMinimumX(spec.minimumX);
        section.SetMaximumX(spec.maximumX);
      }
      section.SetPatternEnabled(spec.pattern);
      mChart.AddSection(section);
      if(spec.text.empty())
      {
        continue;
      }
    }
    auto annotation = ChartAnnotation::New(spec.kind == AnnotationKind::IMAGE ? ChartAnnotation::Type::IMAGE : spec.kind == AnnotationKind::MARKER ? ChartAnnotation::Type::MARKER
                                                                                                                                                   : ChartAnnotation::Type::TEXT);
    if(spec.point >= 0)
    {
      annotation.SetDataPoint(mSeries.front(), static_cast<uint32_t>(spec.point));
    }
    else
    {
      annotation.SetPosition(Vector2(spec.x, spec.y));
    }
    annotation.SetText(Dali::String(spec.text.c_str()));
    annotation.SetColor(Color(spec.color));
    annotation.SetOffset(Vector2(spec.offsetX, spec.offsetY));
    annotation.SetTextMaximumWidth(spec.width);
    annotation.SetTextOverflowMode(spec.ellipsis ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
    if(spec.kind == AnnotationKind::IMAGE)
    {
      annotation.SetImageUrl(RESOURCES_DIR "chart-heart.svg");
      annotation.SetSize(Vector2(20, 18));
    }
    mChart.AddAnnotation(annotation);
  }
  if(mSpec.statusBand)
  {
    for(int i = 0; i < 5; ++i)
    {
      auto band = ChartSection::New();
      band.SetMinimumX(static_cast<float>(mSpec.values.size()) - .5f);
      band.SetMaximumX(static_cast<float>(mSpec.values.size()) - .5f);
      const float minimum = static_cast<float>(i) * 20.0f;
      band.SetMinimumY(minimum);
      band.SetMaximumY(minimum + 20.0f);
      band.SetStrokeWidth(2);
      band.SetStrokeColor(Color(StatusColor(minimum + 1.0f, 5)));
      mChart.AddSection(band);
    }
  }
}

void CasePresenter::OnContentUpdated(ChartView)
{
  mReady = true;
  if(mState.hasSelection)
  {
    mRestoringSelection = true;
    mChart.SelectPoint(mState.series, mState.selected);
    mRestoringSelection = false;
  }
}

void CasePresenter::OnSelected(ChartView, const ChartPointEventArgs& point)
{
  if(point.GetSeriesIndex() < 0 || point.GetPointIndex() < 0)
  {
    return;
  }
  mState.hasSelection = true;
  mState.series       = static_cast<uint32_t>(point.GetSeriesIndex());
  mState.selected     = static_cast<uint32_t>(point.GetPointIndex());
  std::string text    = std::string(point.GetXLabel().CStr()) + " / " + FormatNumber(point.GetDataY(), 1) + " " + mSpec.unit;
  if(!mSpec.webReference)
  {
    if(mSpec.kind == ChartKind::PIE)
    {
      const auto values = BuildValues(mSpec, mState);
      double     total  = 0.0;
      for(float value : values)
      {
        total += value;
      }
      text = std::string(point.GetXLabel().CStr()) + " / " + FormatNumber(point.GetDataY(), 0) + mSpec.unit + " / " + FormatNumber(point.GetDataY() / total * 100.0, 1) + "%";
    }
    else
    {
      text = std::string(point.GetSeriesName().CStr()) + " / X " + FormatNumber(point.GetDataX(), 1) + " / Y " + FormatNumber(point.GetDataY(), 1);
    }
  }
  mSelection.SetText(Dali::String(text.c_str()));
  mSelection.SetAccessibilityValue(Dali::String(text.c_str()));
  if(!mSpec.webReference)
  {
    RefreshAdditionalLabels();
  }
  if(activated && !mRestoringSelection)
  {
    activated();
  }
}

void CasePresenter::Update()
{
  if(mNumber)
  {
    double value       = 0;
    mState.numberInput = mInput.GetText().CStr();
    if(!ParseNumber(mInput.GetText().CStr(), value))
    {
      mError.SetText("Enter a finite number.");
      return;
    }
    mError.SetText("");
    if(mFreeze)
    {
      mNumber->SetValue(value, mSpec.fractionDigits);
    }
    else
    {
      mNumber->Start(value, mSpec.numberMode, mSpec.fractionDigits, mSpec.duration);
    }
    mState.numberValue   = value;
    mState.numberTarget  = value;
    mState.numberRunning = !mFreeze;
    return;
  }
  if(mSpec.action == Action::TABS)
  {
    SetPeriod((mState.period + 1) % 3);
    return;
  }
  ++mState.revision;
  if(mSpec.action == Action::SHOW_HIDE)
  {
    mState.hidden = !mState.hidden;
    mChart.SetProperty(Actor::Property::VISIBLE, !mState.hidden);
    return;
  }
  if(mSpec.action == Action::RECREATE)
  {
    if(recreate)
    {
      recreate();
    }
    return;
  }
  if(mSpec.action == Action::X_RANGE || mSpec.action == Action::Y_RANGE || mSpec.action == Action::DATA_X)
  {
    auto range = mChart.GetViewport();
    if(mSpec.action == Action::Y_RANGE)
    {
      range.w = mState.revision % 2 ? 100.0f : 80.0f;
    }
    else
    {
      range.z = mState.revision % 2 ? std::max(1.0f, static_cast<float>(mSpec.values.size()) * .75f) : static_cast<float>(mSpec.values.size()) - .5f;
    }
    mReady = false;
    mChart.SetViewport(range);
    mState.hasViewport = true;
    mState.viewport[0] = range.x;
    mState.viewport[1] = range.y;
    mState.viewport[2] = range.z;
    mState.viewport[3] = range.w;
    if(mSpec.action != Action::DATA_X)
    {
      return;
    }
  }

  mReady = false;
  ApplyData();
  for(size_t i = 0; i < mSpec.companions.size(); ++i)
  {
    const auto&   spec = mSpec.companions[i];
    ReferenceCase data;
    data.values        = spec.values;
    data.minimum       = spec.minimum;
    data.maximum       = spec.maximum;
    data.action        = mSpec.action;
    mCompanionReady[i] = false;
    mCompanionSeries[i].SetValues(ToVector(BuildValues(data, mState)));
  }
}

void CasePresenter::RefreshPeriodButtons()
{
  for(size_t i = 0; i < mTabButtons.size(); ++i)
  {
    const bool selected = static_cast<int>(i) == mState.period;
    mTabButtons[i].SetBackgroundColor(UiColor(Color(selected ? 0x387AFF : mDark ? 0x383B44
                                                                                : 0xECF1FC)));
    auto content = mTabButtons[i].GetChildAt(0);
    auto label   = Label::DownCast(content.GetChildAt(0));
    label.SetTextColor(UiColor(Color(selected ? 0xFFFFFF : mDark ? 0xEFEFF2
                                                                 : 0x222225)));
  }
}

void CasePresenter::SetPeriod(int period)
{
  if(period == mState.period)
  {
    return;
  }
  ++mState.revision;
  mState.period       = period;
  mState.page         = 0;
  mState.hasSelection = false;
  mReady              = false;
  mEmpty.SetProperty(Actor::Property::VISIBLE, period == 1);
  ConfigureAxes();
  ApplyViewport();
  ApplyData();
  RefreshPeriodButtons();
}

void CasePresenter::ToggleSeries(size_t index)
{
  if(index >= mSeries.size())
  {
    return;
  }
  bool& visible = index == 0 ? mState.primaryVisible : mState.secondaryVisible;
  mReady        = false;
  visible       = !visible;
  mSeries[index].SetVisible(visible);
  if(!mSpec.webReference && mState.hasSelection && mState.series == index && !visible)
  {
    mState.hasSelection = false;
    mSelection.SetText("Select a point");
  }
}

void CasePresenter::Navigate(int direction)
{
  if(mSpec.navigation == Navigation::NONE)
  {
    return;
  }
  const int next = std::clamp(mState.page + direction, 0, PageCount(mSpec) - 1);
  if(next == mState.page)
  {
    return;
  }
  mReady              = false;
  mState.page         = next;
  mState.hasViewport  = false;
  mState.hasSelection = false;
  ApplyViewport();
  Announce();
}

void CasePresenter::Announce()
{
  const auto text = PageAnnouncement(mSpec, mState.page);
  mSelection.SetText(Dali::String(text.c_str()));
  mRoot.SetAccessibilityValue(Dali::String(text.c_str()));
  if(mSpec.category == "Accessibility")
  {
    Extension::Accessibility::SayAccessibilityText(Dali::String(text.c_str()), true);
  }
}

void CasePresenter::Select(int direction)
{
  if(!mChart || !mSpec.interactive)
  {
    return;
  }
  const size_t count = BuildValues(mSpec, mState).size();
  if(count == 0)
  {
    return;
  }
  DALI_ASSERT_ALWAYS(count <= std::numeric_limits<uint32_t>::max());
  const auto viewport   = mChart.GetViewport();
  int64_t    next       = mState.hasSelection ? static_cast<int64_t>(mState.selected) + direction : mSpec.webReference ? static_cast<int64_t>(std::ceil(viewport.x))
                                                                                                                       : 0;
  next                  = std::clamp(next, int64_t{0}, static_cast<int64_t>(count - 1u));
  const uint32_t series = mSpec.kind == ChartKind::SCATTER && !mState.primaryVisible ? 1u : 0u;
  mChart.SelectPoint(series, static_cast<uint32_t>(next));
}

void CasePresenter::OnPan(Actor, PanGesture gesture)
{
  if(gesture.GetState() == GestureState::STARTED)
  {
    mPanStart = gesture.GetPosition();
  }
  else if(gesture.GetState() == GestureState::FINISHED)
  {
    const auto delta = gesture.GetPosition() - mPanStart;
    if(std::abs(delta.x) > 32 && std::abs(delta.x) > std::abs(delta.y))
    {
      Navigate(delta.x < 0 ? 1 : -1);
    }
  }
}

void CasePresenter::Advance(float seconds)
{
  if(mNumber)
  {
    mNumber->Advance(seconds);
    mState.numberValue   = mNumber->GetValue();
    mState.numberRunning = mNumber->IsRunning();
  }
}

AbsoluteLayout CasePresenter::GetView() const
{
  return mRoot;
}

ChartView CasePresenter::GetChart() const
{
  return mChart;
}

bool CasePresenter::IsReady() const
{
  return mReady && std::all_of(mCompanionReady.begin(), mCompanionReady.end(), [](bool ready)
  { return ready; });
}

std::string CasePresenter::Snapshot() const
{
  std::ostringstream out;
  out << "{\"id\":" << std::quoted(mSpec.id) << ",\"title\":" << std::quoted(mSpec.title)
      << ",\"ready\":" << (IsReady() ? "true" : "false") << ",\"series\":" << mSeries.size()
      << ",\"chartCount\":" << (mChart ? 1u + mCompanions.size() : 0u)
      << ",\"cardHeight\":" << mHeight
      << ",\"revision\":" << mState.revision << ",\"period\":" << mState.period
      << ",\"cardWidth\":" << mWidth
      << ",\"points\":" << BuildValues(mSpec, mState).size() << ",\"page\":" << mState.page
      << ",\"hasSelection\":" << (mState.hasSelection ? "true" : "false") << ",\"selected\":" << mState.selected
      << ",\"selectionText\":" << std::quoted(std::string(mSelection.GetText().CStr()))
      << ",\"numberInput\":" << std::quoted(mInput ? std::string(mInput.GetText().CStr()) : std::string())
      << ",\"numberError\":" << std::quoted(mError ? std::string(mError.GetText().CStr()) : std::string())
      << ",\"metric\":" << FormatNumber(mState.numberValue, mSpec.fractionDigits)
      << ",\"dark\":" << (mDark ? "true" : "false") << "}";
  auto result = out.str();
  result.pop_back();
  if(mChart)
  {
    const auto         viewport = mChart.GetViewport();
    std::ostringstream range;
    range.imbue(std::locale::classic());
    range << ",\"viewport\":[" << viewport.x << "," << viewport.y << "," << viewport.z << "," << viewport.w << "]";
    result += range.str();
  }
  if(!mSpec.webReference)
  {
    std::ostringstream extra;
    extra.imbue(std::locale::classic());
    extra << ",\"webReference\":false,\"primaryVisible\":" << (mState.primaryVisible ? "true" : "false")
          << ",\"secondaryVisible\":" << (mState.secondaryVisible ? "true" : "false");
    if(mSpec.kind == ChartKind::GAUGE)
    {
      extra << ",\"gaugeValue\":" << mChart.GetGaugeValue();
    }
    result += extra.str();
  }
  return result + "}";
}
} //namespace ChartStandard
