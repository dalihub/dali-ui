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
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/pie-series.h>

namespace DALI_NAMESPACE
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
  const auto key   = std::make_pair(std::string(text.CStr()), pointSize);
  const auto found = mTextWidths.find(key);
  if(found != mTextWidths.end())
  {
    return found->second;
  }
  if(!mMeasureLabel)
  {
    mMeasureLabel = Ui::Label::New();
  }
  mMeasureLabel.SetProperty(Ui::Label::Property::MULTI_LINE, true);
  mMeasureLabel.SetText(text);
  mMeasureLabel.SetFontSize(pointSize);
  const float width = mMeasureLabel.Measure(FLT_MAX, FLT_MAX).width;
  if(mTextWidths.size() >= 256u)
  {
    mTextWidths.clear();
  }
  mTextWidths.emplace(key, width);
  return width;
}

Dali::String ChartLayoutManager::FormatTickValue(float value)
{
  if(value == std::floor(value) && std::abs(value) < 1e6f)
  {
    return Dali::String(std::to_string(static_cast<int>(value)).c_str());
  }
  std::ostringstream ss;
  ss.imbue(std::locale::classic());
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

  float left   = style.render.padding;
  float top    = style.render.padding + mr;
  float right  = totalSize.width - style.render.padding - mr;
  float bottom = totalSize.height - style.render.padding;

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
  float legendTop = bottom;
  if(legendAtBottom)
  {
    float lineWidth = 0.0f;
    int   rows      = 1;
    for(const auto& series : model.mSeriesList)
    {
      const float width = EstimateTextWidth(GetImplementation(series).GetName(), style.layout.axisLabelSize) + LEGEND_SWATCH + LEGEND_GAP + PADDING;
      if(lineWidth > 0.0f && lineWidth + width > totalSize.width - 2.0f * PADDING)
      {
        ++rows;
        lineWidth = 0.0f;
      }
      lineWidth += width;
    }
    const float legendH = rows * (EstimateTextHeight(style.layout.axisLabelSize) + LEGEND_GAP) + PADDING;
    bottom -= legendH;
    legendTop = bottom;
  }

  // GAUGE — no axes
  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::GAUGE))
  {
    result.plotArea = Rect<float>(PADDING, top, totalSize.width - 2.0f * PADDING, bottom - top);
    if(result.plotArea.width < 1.0f)
    {
      result.plotArea.width = 1.0f;
    }
    if(result.plotArea.height < 1.0f)
    {
      result.plotArea.height = 1.0f;
    }
    scale.SetPlotArea(result.plotArea);
    return result;
  }

  // PIE — no axes
  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    result.plotArea = Rect<float>(PADDING, top, totalSize.width - 2.0f * PADDING, bottom - top);
    if(result.plotArea.width < 1.0f)
    {
      result.plotArea.width = 1.0f;
    }
    if(result.plotArea.height < 1.0f)
    {
      result.plotArea.height = 1.0f;
    }
    scale.SetPlotArea(result.plotArea);

    if(legendAtBottom)
    {
      for(const auto& s : model.mSeriesList)
      {
        auto ps = Ui::PieSeries::DownCast(const_cast<Ui::ChartSeries&>(s));
        if(!ps)
        {
          continue;
        }
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

  const auto& xAxisImpl = GetImplementation(model.mXAxis);
  const auto& yAxisImpl = GetImplementation(model.mYAxis);
  const bool  farX      = xAxisImpl.mPosition == Ui::ChartAxis::Position::FAR;
  const bool  farY      = yAxisImpl.mPosition == Ui::ChartAxis::Position::FAR;
  const float fontSize  = style.layout.axisLabelSize;
  auto        ticks     = [](const auto& axis)
  {
    std::vector<float> values;
    if(!axis.mTickLabels.empty())
    {
      for(const auto& tick : axis.mTickLabels)
      {
        values.push_back(tick.first);
      }
    }
    else if(axis.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC)
    {
      values = ScaleEngine::ComputeLogTicks(axis.GetMinLimit(), axis.GetMaxLimit());
    }
    else
    {
      values = ScaleEngine::ComputeNiceTicks(axis.GetMinLimit(), axis.GetMaxLimit(), 5, axis.GetMinStep());
    }
    return values;
  };
  auto textFor = [](const auto& axis, float value)
  {
    const auto found = axis.mTickLabels.find(value);
    return found == axis.mTickLabels.end() ? FormatTickValue(value) : found->second.first;
  };
  auto colorFor = [](const auto& axis, float value)
  {
    const auto found = axis.mTickLabels.find(value);
    return found == axis.mTickLabels.end() ? axis.mLabelColor : found->second.second;
  };
  const auto yTicks      = ticks(yAxisImpl);
  float      yLabelWidth = 0.0f;
  if(yAxisImpl.mLabelsVisible)
  {
    for(float value : yTicks)
    {
      yLabelWidth = std::max(yLabelWidth, EstimateTextWidth(textFor(yAxisImpl, value), fontSize));
    }
  }
  if(yLabelWidth > 0.0f)
  {
    const float width = yLabelWidth + TICK_LABEL_GAP + (yAxisImpl.mTickMarksVisible ? TICK_LENGTH : 0.0f);
    if(farY)
    {
      right -= width;
    }
    else
    {
      left += width;
    }
  }
  if(!yAxisImpl.GetTitle().Empty())
  {
    const float width    = EstimateTextHeight(fontSize) + PADDING * 0.5f;
    result.hasYAxisTitle = true;
    result.yAxisTitlePos = Vector2(farY ? right + yLabelWidth + width * 0.5f : left - yLabelWidth - width * 0.5f, (top + bottom) * 0.5f);
    if(farY)
    {
      right -= width;
    }
    else
    {
      left += width;
    }
  }
  std::vector<float> xTicks;
  const auto         labels = xAxisImpl.GetLabels();
  if(!xAxisImpl.mTickLabels.empty() || labels.empty())
  {
    xTicks = ticks(xAxisImpl);
  }
  else
  {
    for(size_t i = 0; i < labels.size(); ++i)
    {
      xTicks.push_back(static_cast<float>(i));
    }
  }
  auto xText = [&](float value)
  {
    const int index = static_cast<int>(std::round(value));
    if(xAxisImpl.mTickLabels.empty() && index >= 0 && index < static_cast<int>(labels.size()))
    {
      return labels[index];
    }
    return textFor(xAxisImpl, value);
  };
  const float rotation     = xAxisImpl.GetLabelsRotation();
  const float radians      = rotation * 0.01745329252f;
  float       xLabelHeight = xAxisImpl.mLabelsVisible ? EstimateTextHeight(fontSize) : 0.0f;
  if(xAxisImpl.mLabelsVisible && std::abs(rotation) > 0.01f)
  {
    for(float value : xTicks)
    {
      xLabelHeight = std::max(xLabelHeight, std::abs(std::sin(radians)) * EstimateTextWidth(xText(value), fontSize) + std::abs(std::cos(radians)) * EstimateTextHeight(fontSize));
    }
  }
  if(xAxisImpl.mLabelsVisible)
  {
    for(float value : xTicks)
    {
      const std::string text(xText(value).CStr());
      const auto        lines = 1u + static_cast<unsigned int>(std::count(text.begin(), text.end(), '\n'));
      xLabelHeight            = std::max(xLabelHeight, EstimateTextHeight(fontSize) * lines);
    }
  }
  const float xSpace = xLabelHeight + TICK_LABEL_GAP + (xAxisImpl.mTickMarksVisible ? TICK_LENGTH : 0.0f);
  if(farX)
  {
    top += xSpace;
  }
  else
  {
    bottom -= xSpace;
  }
  if(!xAxisImpl.GetTitle().Empty())
  {
    result.hasXAxisTitle = true;
    const float height   = EstimateTextHeight(fontSize) + PADDING * 0.5f;
    result.xAxisTitlePos = Vector2(totalSize.width * 0.5f, farX ? top - xSpace - height : bottom + xSpace);
    if(farX)
    {
      top += height;
    }
    else
    {
      bottom -= height;
    }
  }
  result.plotArea = Rect<float>(left, top, std::max(1.0f, right - left), std::max(1.0f, bottom - top));
  scale.SetPlotArea(result.plotArea);
  result.xTicks = xTicks;
  result.yTicks = yTicks;
  for(float value : yTicks)
  {
    const float position = scale.ToCanvasY(value);
    if(!std::isfinite(position) || position < top - 1.0f || position > bottom + 1.0f || !yAxisImpl.mLabelsVisible)
    {
      continue;
    }
    TickLabel label;
    label.text     = textFor(yAxisImpl, value);
    label.color    = colorFor(yAxisImpl, value);
    label.position = Vector2(farY ? right + TICK_LABEL_GAP : left - TICK_LENGTH - TICK_LABEL_GAP, position);
    result.yTickLabels.push_back(std::move(label));
  }
  float lastRight = -FLT_MAX;
  for(float value : xTicks)
  {
    const float position = scale.ToCanvasX(value);
    if(!std::isfinite(position) || position < left - 1.0f || position > right + 1.0f || !xAxisImpl.mLabelsVisible)
    {
      continue;
    }
    const auto  text  = xText(value);
    const float width = std::abs(std::cos(radians)) * EstimateTextWidth(text, fontSize);
    if(position - width * 0.5f < lastRight + TICK_LABEL_GAP)
    {
      continue;
    }
    TickLabel label;
    label.text     = text;
    label.color    = colorFor(xAxisImpl, value);
    label.angle    = rotation;
    label.position = Vector2(position, farX ? top - TICK_LENGTH - TICK_LABEL_GAP : bottom + TICK_LENGTH + TICK_LABEL_GAP);
    result.xTickLabels.push_back(std::move(label));
    lastRight = position + width * 0.5f;
  }
  result.xTickCount = static_cast<int>(xTicks.size());
  if(legendAtBottom && !model.mSeriesList.empty())
  {
    result.hasLegend = true;
    float x          = PADDING;
    float legendY    = legendTop + PADDING;
    for(const auto& series : model.mSeriesList)
    {
      LegendItem item;
      item.name         = GetImplementation(series).GetName();
      const float width = EstimateTextWidth(item.name, fontSize) + LEGEND_SWATCH + LEGEND_GAP + PADDING;
      if(x > PADDING && x + width > totalSize.width - PADDING)
      {
        x = PADDING;
        legendY += EstimateTextHeight(fontSize) + LEGEND_GAP;
      }
      item.iconCenter = Vector2(x + LEGEND_SWATCH * 0.5f, legendY + LEGEND_SWATCH * 0.5f);
      item.textPos    = Vector2(x + LEGEND_SWATCH + LEGEND_GAP, legendY);
      x += width;
      result.legendItems.push_back(std::move(item));
    }
  }

  return result;
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
