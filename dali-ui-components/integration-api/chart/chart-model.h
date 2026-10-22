#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/bar-series-impl.h>
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-section-impl.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/public-api/chart/bar-series.h>
#include <dali-ui-components/public-api/chart/chart-axis.h>
#include <dali-ui-components/public-api/chart/chart-section.h>
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

struct StyleConfig
{
  struct GaugeRange
  {
    Vector4 color{1.0f, 1.0f, 1.0f, 1.0f};
    float   fromValue{0.0f};
    float   toValue{100.0f};
  };

  struct RenderStyle
  {
    UiColor backgroundColor{1.0f, 1.0f, 1.0f, 1.0f};
    UiColor labelColor{0.0f, 0.0f, 0.0f, 1.0f};
    UiColor legendTextColor{0.0f, 0.0f, 0.0f, 1.0f};
    UiColor tooltipBackgroundColor{1.0f, 1.0f, 1.0f, 1.0f};
    UiColor tooltipTextColor{0.1f, 0.1f, 0.1f, 1.0f};
    Vector4 gridColor{0.9f, 0.9f, 0.9f, 1.0f};
    Vector4 axisColor{0.3f, 0.3f, 0.3f, 1.0f};
    Vector4 titleColor{0.1f, 0.1f, 0.1f, 1.0f};
    float   padding{10.0f};
    float   tooltipPadding{8.0f};
    float   lineWidth{2.5f};
    float   markerRadius{4.0f};
  } render;

  struct GaugeConfig
  {
    Dali::String            centerLabel;
    std::vector<GaugeRange> ranges;
    Vector4                 trackColor{0.85f, 0.85f, 0.85f, 1.0f};
    Vector4                 progressColor{0.27f, 0.51f, 0.71f, 1.0f};
    float                   minValue{0.0f};
    float                   maxValue{100.0f};
    float                   value{0.0f};
    float                   arcSpanDegrees{270.0f};
    float                   startAngleDegrees{135.0f};
    float                   arcWidthRatio{0.18f};
  } gauge;

  struct LayoutStyle
  {
    float axisLabelSize{11.0f};
    float titleSize{16.0f};
    int   legendPosition{1};
    int   titlePosition{0};
  } layout;

  struct Interaction
  {
    float hitThreshold{30.0f};
    int   findingStrategy{0};
    int   zoomModeFlags{0};
    bool  touchEnabled{true};
    bool  hoverEnabled{true};
    bool  zoomClampEnabled{true};
    bool  autoFitY{false};
  } interaction;

  struct AnimationConfig
  {
    float duration{0.0f};
    int   easing{1};
  } animation;

  struct Visibility
  {
    bool showGrid{true};
    bool showLegend{true};
    bool showTooltip{true};
    bool showMarkers{true};
    bool legendToggleEnabled{true};
  } visibility;
};

class ChartModel
{
public:
  void AddSeries(Ui::ChartSeries series)
  {
    mSeriesList.push_back(series);
  }

  void RemoveAllSeries()
  {
    mSeriesList.clear();
  }

  int GetSeriesCount() const
  {
    return static_cast<int>(mSeriesList.size());
  }

  void ComputeAutoRange()
  {
    auto& xAxisImpl = GetImplementation(mXAxis);
    auto& yAxisImpl = GetImplementation(mYAxis);

    if(mSeriesList.empty())
    {
      if(xAxisImpl.GetAutoRange())
      {
        xAxisImpl.SetComputedMinimum(xAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC ? 0.1f : 0.0f);
        float maxX = xAxisImpl.GetLabels().empty() ? 1.0f : static_cast<float>(xAxisImpl.GetLabels().size() - 1);
        if(maxX < 1e-6f)
        {
          maxX = 1.0f;
        }
        xAxisImpl.SetComputedMaximum(maxX);
      }
      if(yAxisImpl.GetAutoRange())
      {
        yAxisImpl.SetComputedMinimum(yAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC ? 0.1f : 0.0f);
        yAxisImpl.SetComputedMaximum(1.0f);
      }
      return;
    }

    float xMin = std::numeric_limits<float>::max();
    float xMax = std::numeric_limits<float>::lowest();
    float yMin = std::numeric_limits<float>::max();
    float yMax = std::numeric_limits<float>::lowest();

    for(auto& s : mSeriesList)
    {
      auto& impl = GetImplementation(s);
      if(!impl.IsVisible())
      {
        continue;
      }
      for(auto& p : impl.GetValues())
      {
        if(!std::isfinite(p.first) || !std::isfinite(p.second) ||
           (xAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC && p.first <= 0.0f) ||
           (yAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC && p.second <= 0.0f))
        {
          continue;
        }
        const size_t index = &p - impl.GetValues().data();
        const float  lower = impl.LowerValue(index);
        if(impl.HasRangeValues() && (!std::isfinite(lower) ||
                                     (yAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC && lower <= 0.0f)))
        {
          continue;
        }
        xMin = std::min(xMin, p.first);
        xMax = std::max(xMax, p.first);
        yMin = std::min(yMin, p.second);
        yMax = std::max(yMax, p.second);
        if(impl.HasRangeValues() && std::isfinite(lower) &&
           (yAxisImpl.mScaleType != Ui::ChartAxis::ScaleType::LOGARITHMIC || lower > 0.0f))
        {
          yMin = std::min(yMin, lower);
          yMax = std::max(yMax, lower);
        }
      }
    }

    if(xMin > xMax)
    {
      xMin = 0.0f;
      xMax = 1.0f;
      yMin = 0.0f;
      yMax = 1.0f;
    }

    for(auto& s : mSeriesList)
    {
      if(Ui::BarSeries::DownCast(s) && GetImplementation(s).IsVisible() && !GetImplementation(s).HasRangeValues() && yAxisImpl.mScaleType != Ui::ChartAxis::ScaleType::LOGARITHMIC)
      {
        yMin = std::min(yMin, 0.0f);
        yMax = std::max(yMax, 0.0f);
        break;
      }
    }

    {
      std::map<float, float> stackedPos;
      std::map<float, float> stackedNeg;
      bool                   hasStacked = false;

      for(auto& s : mSeriesList)
      {
        auto bs = Ui::BarSeries::DownCast(s);
        if(!bs)
        {
          continue;
        }
        const auto& bsImpl = GetImplementation(bs);
        if(!bsImpl.IsVisible() || !bsImpl.IsStacked())
        {
          continue;
        }

        hasStacked = true;
        for(const auto& pt : GetImplementation(s).GetValues())
        {
          if(std::isnan(pt.second))
          {
            continue;
          }
          const float xKey = pt.first;
          if(pt.second >= 0.0f)
          {
            stackedPos[xKey] += pt.second;
          }
          else
          {
            stackedNeg[xKey] += pt.second;
          }
        }
      }

      if(hasStacked)
      {
        for(auto& [k, v] : stackedPos)
        {
          yMax = std::max(yMax, v);
        }
        for(auto& [k, v] : stackedNeg)
        {
          yMin = std::min(yMin, v);
        }
      }
    }

    if(xAxisImpl.GetAutoRange())
    {
      if(xAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC)
      {
        xMin = xMin > 0.0f ? xMin : 1.0f;
        xMax = xMax > xMin ? xMax : xMin * 10.0f;
      }
      xAxisImpl.SetComputedMinimum(xMin);
      float finalMaxX = xMax;
      if(std::abs(finalMaxX - xMin) < 1e-6f)
      {
        finalMaxX = xMin + 1.0f;
      }
      xAxisImpl.SetComputedMaximum(finalMaxX);
    }

    if(xAxisImpl.GetAutoRange() && xAxisImpl.GetDataPadding() > 0.0f && xAxisImpl.mScaleType != Ui::ChartAxis::ScaleType::LOGARITHMIC)
    {
      float xRange   = xAxisImpl.GetMaxLimit() - xAxisImpl.GetMinLimit();
      float xPadding = (xRange > 0.0f) ? xRange * xAxisImpl.GetDataPadding() : 0.5f;
      xAxisImpl.SetComputedMinimum(xAxisImpl.GetMinLimit() - xPadding);
      xAxisImpl.SetComputedMaximum(xAxisImpl.GetMaxLimit() + xPadding);
    }

    if(!xAxisImpl.GetLabels().empty() && xAxisImpl.GetAutoRange())
    {
      for(const auto& s : mSeriesList)
      {
        if(Ui::BarSeries::DownCast(s) && GetImplementation(s).IsVisible() && !GetImplementation(s).HasRangeValues() && xAxisImpl.mScaleType != Ui::ChartAxis::ScaleType::LOGARITHMIC)
        {
          xAxisImpl.SetComputedMinimum(xAxisImpl.GetMinLimit() - 0.5f);
          xAxisImpl.SetComputedMaximum(xAxisImpl.GetMaxLimit() + 0.5f);

          break;
        }
      }
    }

    if(yAxisImpl.GetAutoRange())
    {
      float yPaddingFrac = yAxisImpl.GetDataPadding();
      if(yPaddingFrac == 0.0f)
      {
        yPaddingFrac = 0.05f;
      }
      float yRange    = yMax - yMin;
      float yPadding  = (yRange > 0.0f) ? yRange * yPaddingFrac : 0.5f;
      float finalYMin = yMin - yPadding;
      float finalYMax = yMax + yPadding;

      if(yMin >= 0.0f && finalYMin < 0.0f)
      {
        finalYMin = 0.0f;
      }

      if(yAxisImpl.mScaleType == Ui::ChartAxis::ScaleType::LOGARITHMIC)
      {
        finalYMin = yMin > 0.0f ? yMin : 1.0f;
        finalYMax = yMax > finalYMin ? yMax : finalYMin * 10.0f;
      }
      yAxisImpl.SetComputedMinimum(finalYMin);
      yAxisImpl.SetComputedMaximum(finalYMax);
    }
  }

public:
  // Ordered storage
  StyleConfig                   mStyle;
  Dali::String                  mTitle;
  std::vector<Ui::ChartSeries>  mSeriesList;
  std::vector<Ui::ChartSection> mSections;
  Ui::ChartAxis                 mXAxis;
  Ui::ChartAxis                 mYAxis;
  int                           mChartType{0};
};

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
