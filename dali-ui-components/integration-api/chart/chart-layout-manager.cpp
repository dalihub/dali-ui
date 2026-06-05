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

// CLASS HEADER
#include <dali-ui-components/integration-api/chart/chart-layout-manager.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/pie-series.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

float ChartLayoutManager::EstimateTextHeight(float pointSize)
{
  return pointSize * 1.333f * 1.2f;
}

float ChartLayoutManager::EstimateTextWidth(const Dali::String& text, float pointSize)
{
  return static_cast<float>(text.Size()) * pointSize * 1.333f * 0.6f;
}

Dali::String ChartLayoutManager::FormatTickValue(float value)
{
  if(value == std::floor(value) && std::abs(value) < 1e6f)
  {
    return Dali::String(std::to_string(static_cast<int>(value)).c_str());
  }
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << value;
  std::string s   = ss.str();
  auto        dot = s.find('.');
  if(dot != std::string::npos)
  {
    s.erase(s.find_last_not_of('0') + 1);
    if(s.back() == '.')
    {
      s.pop_back();
    }
  }
  return Dali::String(s.c_str());
}

ChartLayoutManager::LayoutResult ChartLayoutManager::ComputeLayout(
  const Vector2&    totalSize,
  const ChartModel& model,
  ScaleEngine&      scale)
{
  LayoutResult result;
  result.totalSize = totalSize;

  const StyleConfig& style = model.mStyle;
  const float        mr    = style.render.markerRadius;

  float left   = PADDING;
  float top    = PADDING + mr;
  float right  = totalSize.width - PADDING - mr;
  float bottom = totalSize.height - PADDING;

  // Title (top)
  if(!model.mTitle.Empty())
  {
    float titleH    = EstimateTextHeight(style.layout.titleSize) + PADDING;
    result.titlePos = Vector2(totalSize.width * 0.5f, top);
    result.hasTitle = true;
    top += titleH;
  }

  // Legend (bottom)
  bool legendAtBottom = style.visibility.showLegend &&
                        style.layout.legendPosition == 2 /* BOTTOM */ &&
                        !model.mSeriesList.empty();
  if(legendAtBottom)
  {
    float legendH = EstimateTextHeight(style.layout.axisLabelSize) + PADDING;
    bottom -= legendH;
  }

  // GAUGE — no axes
  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::GAUGE))
  {
    result.plotArea = Rect<float>(PADDING, top, totalSize.width - 2.0f * PADDING, bottom - top);
    if(result.plotArea.width < 1.0f) result.plotArea.width = 1.0f;
    if(result.plotArea.height < 1.0f) result.plotArea.height = 1.0f;
    scale.SetPlotArea(result.plotArea);
    return result;
  }

  // PIE — no axes
  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    result.plotArea = Rect<float>(PADDING, top, totalSize.width - 2.0f * PADDING, bottom - top);
    if(result.plotArea.width < 1.0f) result.plotArea.width = 1.0f;
    if(result.plotArea.height < 1.0f) result.plotArea.height = 1.0f;
    scale.SetPlotArea(result.plotArea);

    if(legendAtBottom)
    {
      for(const auto& s : model.mSeriesList)
      {
        auto ps = Ui::PieSeries::DownCast(const_cast<Ui::ChartSeries&>(s));
        if(!ps) continue;
        const float legendY = bottom + PADDING;
        float       x       = PADDING;
        for(const auto& sl : GetImplementation(ps).GetSlices())
        {
          LegendItem li;
          li.iconCenter = Vector2(x + LEGEND_SWATCH * 0.5f, legendY + LEGEND_SWATCH * 0.5f);
          li.textPos    = Vector2(x + LEGEND_SWATCH + LEGEND_GAP, legendY);
          li.name       = sl.label;
          x += EstimateTextWidth(sl.label, style.layout.axisLabelSize) + LEGEND_SWATCH + LEGEND_GAP + PADDING;
          result.legendItems.push_back(std::move(li));
        }
        result.hasLegend = true;
        break;
      }
    }
    return result;
  }

  auto& xAxisImpl = GetImplementation(model.mXAxis);
  auto& yAxisImpl = GetImplementation(model.mYAxis);

  // X-axis title (bottom)
  if(!xAxisImpl.GetTitle().Empty())
  {
    float h              = EstimateTextHeight(style.layout.axisLabelSize) + PADDING * 0.5f;
    result.xAxisTitlePos = Vector2(totalSize.width * 0.5f, bottom - h);
    result.hasXAxisTitle = true;
    bottom -= h;
  }

  // X tick label height
  float xTickLabelH = EstimateTextHeight(style.layout.axisLabelSize) + TICK_LENGTH + TICK_LABEL_GAP;
  bottom -= xTickLabelH;

  // Y-axis title (left, rotated 90°)
  if(!yAxisImpl.GetTitle().Empty())
  {
    float w              = EstimateTextHeight(style.layout.axisLabelSize);
    result.yAxisTitlePos = Vector2(left + w * 0.5f, (top + bottom) * 0.5f);
    result.hasYAxisTitle = true;
    left += w + PADDING * 0.5f;
  }

  // Y tick labels (left margin)
  std::vector<float> yTicks = ScaleEngine::ComputeNiceTicks(
    yAxisImpl.GetMinLimit(), yAxisImpl.GetMaxLimit(), 5, yAxisImpl.GetMinStep());

  float maxYLabelW = 0.0f;
  for(auto v : yTicks)
  {
    float w = EstimateTextWidth(FormatTickValue(v), style.layout.axisLabelSize);
    if(w > maxYLabelW) maxYLabelW = w;
  }
  left += maxYLabelW + TICK_LENGTH + TICK_LABEL_GAP;

  // Plot area
  result.plotArea = Rect<float>(left, top, right - left, bottom - top);
  if(result.plotArea.width < 1.0f) result.plotArea.width = 1.0f;
  if(result.plotArea.height < 1.0f) result.plotArea.height = 1.0f;
  scale.SetPlotArea(result.plotArea);

  // Y tick label positions
  result.yTicks = yTicks;
  for(auto v : yTicks)
  {
    float cy = scale.ToCanvasY(v);
    if(cy < result.plotArea.y - 1.0f || cy > result.plotArea.y + result.plotArea.height + 1.0f)
      continue;
    TickLabel tl;
    tl.text     = FormatTickValue(v);
    tl.position = Vector2(result.plotArea.x - TICK_LENGTH - TICK_LABEL_GAP, cy);
    result.yTickLabels.push_back(std::move(tl));
  }

  // X tick label positions
  const float xRotation      = xAxisImpl.GetLabelsRotation();
  const float xTickBaselineY = result.plotArea.y + result.plotArea.height + TICK_LENGTH + TICK_LABEL_GAP;
  const auto& xLabels        = xAxisImpl.GetLabels();

  if(!xLabels.empty())
  {
    int n             = static_cast<int>(xLabels.size());
    result.xTickCount = n;
    for(int i = 0; i < n; ++i)
    {
      float cx = scale.ToCanvasX(static_cast<float>(i));
      if(cx < result.plotArea.x - 1.0f || cx > result.plotArea.x + result.plotArea.width + 1.0f)
        continue;
      TickLabel tl;
      tl.text     = xLabels[static_cast<size_t>(i)];
      tl.position = Vector2(cx, xTickBaselineY);
      tl.angle    = xRotation;
      result.xTickLabels.push_back(std::move(tl));
    }
  }
  else
  {
    std::vector<float> xTicks = ScaleEngine::ComputeNiceTicks(
      xAxisImpl.GetMinLimit(), xAxisImpl.GetMaxLimit(), 5, xAxisImpl.GetMinStep());
    result.xTickCount = static_cast<int>(xTicks.size());
    for(auto v : xTicks)
    {
      float cx = scale.ToCanvasX(v);
      if(cx < result.plotArea.x - 1.0f || cx > result.plotArea.x + result.plotArea.width + 1.0f)
        continue;
      TickLabel tl;
      tl.text     = FormatTickValue(v);
      tl.position = Vector2(cx, xTickBaselineY);
      tl.angle    = xRotation;
      result.xTickLabels.push_back(std::move(tl));
    }
  }

  // Legend items (bottom)
  if(legendAtBottom && !model.mSeriesList.empty())
  {
    result.hasLegend = true;
    float legendY    = bottom + PADDING;
    float x          = PADDING;
    for(auto& s : model.mSeriesList)
    {
      LegendItem li;
      li.iconCenter = Vector2(x + LEGEND_SWATCH * 0.5f, legendY + LEGEND_SWATCH * 0.5f);
      li.textPos    = Vector2(x + LEGEND_SWATCH + LEGEND_GAP, legendY);
      li.name       = GetImplementation(s).GetName();
      x += EstimateTextWidth(li.name, style.layout.axisLabelSize) + LEGEND_SWATCH + LEGEND_GAP + PADDING;
      result.legendItems.push_back(std::move(li));
    }
  }

  return result;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
