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
#include <algorithm>
#include <numeric>
#include "chart-case-presenter.h"

using namespace Dali;
using namespace Dali::Ui;

namespace ChartStandard
{
void CasePresenter::BuildAdditionalChart()
{
  const float buttonY = mHeight - 54.0f;
  AddLabel(mRoot, mSpec.description, 12, 28, 66, mWidth - 56, 24, mDark ? 0xACAFB9 : 0x797984);
  const auto type  = mSpec.kind == ChartKind::PIE ? ChartView::Type::PIE : mSpec.kind == ChartKind::GAUGE ? ChartView::Type::GAUGE
                                                                                                          : ChartView::Type::LINE;
  auto       style = ChartViewStyle::Builder()
                 .SetBackgroundColor(Color(mDark ? 0x262626 : 0xFFFFFF))
                 .SetLabelColor(Color(mDark ? 0xEFEFF2 : 0x222225))
                 .SetTooltipBackgroundColor(Color(mDark ? 0x3D414A : 0xE8F2FF))
                 .SetTooltipTextColor(Color(mDark ? 0xFFFFFF : 0x222225))
                 .SetPadding(8)
                 .Build();
  mChart = ChartView::New(type, Vector2(mWidth - 56, 300), style);
  mChart.SetLayoutParams(AbsoluteLayoutParams::New().SetX(28).SetY(116).SetWidth(mWidth - 56).SetHeight(300));
  mChart.SetProperty(ChartView::Property::SHOW_LEGEND, false);
  mChart.SetProperty(ChartView::Property::SHOW_TOOLTIP, mSpec.interactive);
  mChart.SetProperty(ChartView::Property::HOVER_ENABLED, mSpec.interactive);
  mChart.SetProperty(ChartView::Property::TOUCH_ENABLED, mSpec.interactive);
  mChart.SetAnimationDuration(mFreeze ? 0.0f : 500.0f);
  mChart.SetAccessibilityName(Dali::String(mSpec.title.c_str()));
  mChart.SetAccessibilityHighlightable(mSpec.interactive);
  if(mSpec.kind == ChartKind::PIE)
  {
    auto pie = PieSeries::New();
    pie.SetName(Dali::String(mSpec.subtitle.c_str()));
    pie.SetInnerRadiusRatio(mSpec.innerRadius);
    pie.SetSliceGap(2.0f);
    mSeries.push_back(pie);
    mChart.AddSeries(pie);
    const float legendWidth = (mWidth - 64.0f) * 0.5f;
    for(size_t index = 0u; index < mSpec.labels.size(); ++index)
    {
      const float x      = 28.0f + static_cast<float>(index % 2u) * (legendWidth + 8.0f);
      const float y      = 438.0f + static_cast<float>(index / 2u) * 40.0f;
      auto        button = AddButton("", x, y, legendWidth, [this, index]
             {
        mChart.SelectPoint(0u, static_cast<uint32_t>(index));
      });
      auto        label  = Label::DownCast(button.GetChildAt(0).GetChildAt(0));
      label.SetFontSize(mWidth < 400.0f ? 11.0f : 13.0f);
      label.SetProperty(Label::Property::MULTI_LINE, mWidth < 400.0f);
      mLegendValues.push_back(label);
      AddLabel(mRoot, "●", 11, x + 7, y + 7, 14, 22, mSpec.pointColors[index]);
    }
    if(mSpec.innerRadius > 0.0f)
    {
      mCenterValue = AddLabel(mRoot, "", 28, 28, 236, mWidth - 56, 40);
      mCenterValue.SetFontWeight(Text::FontWeight::BOLD);
      mCenterValue.SetHorizontalTextAlignment(Text::Alignment::CENTER);
      auto caption = AddLabel(mRoot, "TOTAL", 11, 28, 278, mWidth - 56, 24, mDark ? 0xACAFB9 : 0x797984);
      caption.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    }
    mSelection.SetText("Select a segment or its legend");
  }
  else if(mSpec.kind == ChartKind::GAUGE)
  {
    mChart.SetGaugeMinimumValue(0);
    mChart.SetGaugeMaximumValue(100);
    mChart.SetGaugeArcSpan(270);
    mChart.SetGaugeStartAngle(135);
    mChart.SetGaugeArcWidth(0.18f);
    mChart.SetGaugeTrackColor(Color(mDark ? 0x414147 : 0xE8E8EC));
    mChart.SetGaugeProgressColor(Color(0x387AFF));
    const uint32_t colors[] = {0x78ACFFu, 0x70CDB5u, 0xFFB877u};
    const char*    labels[] = {"0–60", "60–80", "80–100"};
    for(int index = 0; index < 3; ++index)
    {
      mChart.AddGaugeRange(index == 0 ? 0.0f : index == 1 ? 60.0f
                                                          : 80.0f,
                           index == 0 ? 60.0f : index == 1 ? 80.0f
                                                           : 100.0f,
                           Color(colors[index], mDark ? .45f : .55f));
      const float x = 28.0f + static_cast<float>(index) * (mWidth - 56) / 3.0f;
      AddLabel(mRoot, "●", 11, x, 448, 16, 24, colors[index]);
      AddLabel(mRoot, labels[index], 12, x + 20, 448, (mWidth - 56) / 3.0f - 20, 24);
    }
    auto caption = AddLabel(mRoot, "Weekly activity goal", 14, 28, 496, mWidth - 56, 28);
    caption.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mSelection.SetText("Update changes progress within 0–100%");
  }
  else
  {
    ConfigureAxes();
    mChart.SetViewport(Vector4(0, 0, 10, 10));
    for(int index = 0; index < 2; ++index)
    {
      auto scatter = ScatterSeries::New();
      scatter.SetName(index == 0 ? "Group A" : "Group B");
      scatter.SetColor(Color(index == 0 ? 0x387AFF : 0xFF7F20));
      scatter.SetMarkerRadius(6.0f);
      scatter.SetMarkerShape(index == 0 ? ScatterSeries::MarkerShape::CIRCLE : ScatterSeries::MarkerShape::DIAMOND);
      mSeries.push_back(scatter);
      mChart.AddSeries(scatter);
      const float width = (mWidth - 64) * .5f;
      const float x     = 28.0f + static_cast<float>(index) * (width + 8);
      AddButton(index == 0 ? "● Group A" : "◆ Group B", x, 452, width, [this, index]
      { ToggleSeries(static_cast<size_t>(index)); });
    }
    mSelection.SetText("Select a point / legends toggle each group");
  }
  AddButton("Update", 28, buttonY, 94, [this]
  { Update(); });
  if(mSpec.interactive)
  {
    AddButton("Select", 130, buttonY, 84, [this]
    { Select(1); });
  }
  AddButton("Reset", mSpec.interactive ? 222.0f : 130.0f, buttonY, 84, [this]
  {
    mState = CaseState{};
    mReady = false;
    mSelection.SetText(mSpec.kind == ChartKind::GAUGE ? "Update changes progress within 0–100%" : "Select a value");
    ApplyAdditionalData();
  });
  mChart.ContentUpdatedSignal().Connect(this, &CasePresenter::OnContentUpdated);
  mChart.DataPointSelectedSignal().Connect(this, &CasePresenter::OnSelected);
  mRoot.Add(mChart);
  // Overlay labels must stay above the chart canvas, while allowing pointer input through.
  if(mCenterValue)
  {
    mCenterValue.RaiseToTop();
    for(uint32_t index = 0u; index < mRoot.GetChildCount(); ++index)
    {
      auto label = Label::DownCast(mRoot.GetChildAt(index));
      if(label && label.GetText() == "TOTAL")
      {
        label.RaiseToTop();
        break;
      }
    }
  }
  ApplyAdditionalData();
}

void CasePresenter::ApplyAdditionalData()
{
  const auto values = BuildValues(mSpec, mState);
  if(mSpec.kind == ChartKind::PIE)
  {
    auto pie = PieSeries::DownCast(mSeries.front());
    pie.ClearSlices();
    for(size_t index = 0u; index < values.size(); ++index)
    {
      pie.AddSlice(Dali::String(mSpec.labels[index].c_str()), values[index], Color(mSpec.pointColors[index]));
    }
  }
  else if(mSpec.kind == ChartKind::GAUGE)
  {
    mChart.SetGaugeValue(values.front());
    mChart.SetGaugeCenterLabel(Dali::String((FormatNumber(values.front(), 0) + "%").c_str()));
  }
  else
  {
    for(size_t index = 0u; index < mSeries.size(); ++index)
    {
      const auto            y = BuildValues(mSpec, mState, static_cast<uint32_t>(index));
      const auto&           x = index == 0u ? mSpec.xValues : mSpec.secondaryXValues;
      Dali::Vector<Vector2> points;
      for(size_t point = 0u; point < y.size(); ++point)
      {
        points.PushBack(Vector2(x[point], y[point]));
      }
      mSeries[index].SetValues(points);
      mSeries[index].SetVisible(index == 0u ? mState.primaryVisible : mState.secondaryVisible);
    }
  }
  RefreshAdditionalLabels();
}

void CasePresenter::RefreshAdditionalLabels()
{
  if(mSpec.kind != ChartKind::PIE)
  {
    return;
  }
  const auto   values = BuildValues(mSpec, mState);
  const double total  = std::accumulate(values.begin(), values.end(), 0.0);
  if(mCenterValue)
  {
    mCenterValue.SetText(Dali::String((FormatNumber(total, 0) + mSpec.unit).c_str()));
  }
  for(size_t index = 0u; index < mLegendValues.size(); ++index)
  {
    const auto text = mSpec.labels[index] + (mWidth < 400.0f ? "\n" : " ") + FormatNumber(values[index], 0) + " · " + FormatNumber(total > 0 ? values[index] / total * 100.0 : 0.0, 0) + "%";
    mLegendValues[index].SetText(Dali::String(text.c_str()));
    auto button = View::DownCast(mLegendValues[index].GetParent().GetParent());
    button.SetBackgroundColor(UiColor(Color(mState.hasSelection && mState.selected == index ? mDark ? 0x354E7A : 0xDCE8FF : mDark ? 0x383B44
                                                                                                                                  : 0xECF1FC)));
  }
}
} // namespace ChartStandard
