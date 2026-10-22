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
#include <dali-ui-components/integration-api/chart/chart-hit-tester.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-color-palette.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/integration-api/chart/line-series-impl.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-components/public-api/chart/line-series.h>

#include <dali-ui-components/integration-api/chart/chart-bar-geometry.h>
#include <dali-ui-components/integration-api/chart/chart-pie-geometry.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <string>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

bool ChartHitTester::IsInsidePlotArea(const Vector2& pos, const ScaleEngine& scale) const
{
  const Rect<float>& pa = scale.GetPlotArea();
  return pos.x >= pa.x && pos.x <= pa.x + pa.width &&
         pos.y >= pa.y && pos.y <= pa.y + pa.height;
}

HitResult ChartHitTester::FindNearest(const Vector2&     touchPos,
                                      const ChartModel&  model,
                                      const ScaleEngine& scale,
                                      float              threshold) const
{
  HitResult best;
  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    for(const auto& slice : BuildChartPieSlices(model, scale.GetPlotArea()))
    {
      if(slice.Contains(touchPos))
      {
        return slice.hit;
      }
    }
    return best;
  }
  const auto bars = BuildChartBars(model, scale);
  for(auto iter = bars.rbegin(); iter != bars.rend(); ++iter)
  {
    if(iter->Contains(touchPos))
    {
      const auto& impl  = GetImplementation(model.mSeriesList[iter->seriesIndex]);
      best.isValid      = true;
      best.seriesIndex  = static_cast<int>(iter->seriesIndex);
      best.pointIndex   = static_cast<int>(iter->pointIndex);
      best.dataX        = iter->dataX;
      best.dataY        = iter->dataY;
      best.canvasPos    = Vector2(iter->bounds.x + iter->bounds.width * 0.5f, std::clamp(scale.ToCanvasY(iter->dataY), iter->bounds.y, iter->bounds.y + iter->bounds.height));
      best.seriesName   = impl.GetName();
      best.seriesColor  = iter->color;
      const auto labels = GetImplementation(model.mXAxis).GetLabels();
      const int  x      = static_cast<int>(std::round(iter->dataX));
      best.xLabel       = x >= 0 && x < static_cast<int>(labels.size()) ? labels[x] : Dali::String(std::to_string(iter->dataX).c_str());
      return best;
    }
  }
  float bestDist2 = threshold * threshold;

  const std::vector<Dali::String>& xLabels =
    GetImplementation(const_cast<Ui::ChartAxis&>(model.mXAxis)).GetLabels();

  const int seriesCount = model.GetSeriesCount();
  for(int si = 0; si < seriesCount; ++si)
  {
    Ui::ChartSeries& series = const_cast<Ui::ChartSeries&>(model.mSeriesList[static_cast<size_t>(si)]);

    if(!GetImplementation(series).IsVisible())
    {
      continue;
    }

    const std::vector<std::pair<float, float>>& pts     = GetImplementation(series).GetValues();
    const int                                   ptCount = static_cast<int>(pts.size());

    for(int pi = 0; pi < ptCount; ++pi)
    {
      if(!scale.IsValid(pts[static_cast<size_t>(pi)].first, pts[static_cast<size_t>(pi)].second))
      {
        continue;
      }

      Vector2 cp = scale.ToCanvas(pts[static_cast<size_t>(pi)].first, pts[static_cast<size_t>(pi)].second);
      float   dx = cp.x - touchPos.x;
      float   dy = cp.y - touchPos.y;
      float   d2 = dx * dx + dy * dy;

      if(d2 < bestDist2)
      {
        bestDist2        = d2;
        best.isValid     = true;
        best.seriesIndex = si;
        best.pointIndex  = pi;
        best.dataX       = pts[static_cast<size_t>(pi)].first;
        best.dataY       = pts[static_cast<size_t>(pi)].second;
        best.canvasPos   = cp;
        best.seriesName  = GetImplementation(series).GetName();

        int xi = static_cast<int>(std::round(pts[static_cast<size_t>(pi)].first));
        if(xi >= 0 && xi < static_cast<int>(xLabels.size()))
        {
          best.xLabel = xLabels[static_cast<size_t>(xi)];
        }
        else
        {
          best.xLabel = Dali::String(std::to_string(static_cast<int>(pts[static_cast<size_t>(pi)].first)).c_str());
        }

        Ui::LineSeries ls = Ui::LineSeries::DownCast(series);
        best.seriesColor  = GetImplementation(series).PointColor(static_cast<uint32_t>(pi));
      }
    }
  }

  return best;
}

std::vector<HitResult> ChartHitTester::FindBySameX(const Vector2&     touchPos,
                                                   const ChartModel&  model,
                                                   const ScaleEngine& scale,
                                                   float              threshold,
                                                   bool               nearestY) const
{
  std::vector<HitResult> results;
  if(model.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    const auto hit = FindNearest(touchPos, model, scale, threshold);
    if(hit.isValid)
    {
      results.push_back(hit);
    }
    return results;
  }

  const std::vector<Dali::String>& xLabels =
    GetImplementation(const_cast<Ui::ChartAxis&>(model.mXAxis)).GetLabels();

  const int seriesCount = model.GetSeriesCount();
  for(int si = 0; si < seriesCount; ++si)
  {
    Ui::ChartSeries& series = const_cast<Ui::ChartSeries&>(model.mSeriesList[static_cast<size_t>(si)]);
    if(!GetImplementation(series).IsVisible())
    {
      continue;
    }

    const auto& pts     = GetImplementation(series).GetValues();
    const int   ptCount = static_cast<int>(pts.size());
    if(ptCount == 0)
    {
      continue;
    }

    float     bestDx2 = threshold * threshold;
    HitResult best;
    for(int pi = 0; pi < ptCount; ++pi)
    {
      if(!scale.IsValid(pts[static_cast<size_t>(pi)].first, pts[static_cast<size_t>(pi)].second))
      {
        continue;
      }

      float dx = scale.ToCanvasX(pts[static_cast<size_t>(pi)].first) - touchPos.x;
      if(dx * dx < bestDx2)
      {
        bestDx2          = dx * dx;
        best.isValid     = true;
        best.seriesIndex = si;
        best.pointIndex  = pi;
        best.dataX       = pts[static_cast<size_t>(pi)].first;
        best.dataY       = pts[static_cast<size_t>(pi)].second;
        best.canvasPos   = scale.ToCanvas(pts[static_cast<size_t>(pi)].first, pts[static_cast<size_t>(pi)].second);
        best.seriesName  = GetImplementation(series).GetName();

        int xi = static_cast<int>(std::round(pts[static_cast<size_t>(pi)].first));
        if(xi >= 0 && xi < static_cast<int>(xLabels.size()))
        {
          best.xLabel = xLabels[static_cast<size_t>(xi)];
        }
        else
        {
          best.xLabel = Dali::String(std::to_string(static_cast<int>(pts[static_cast<size_t>(pi)].first)).c_str());
        }

        Ui::LineSeries ls = Ui::LineSeries::DownCast(series);
        best.seriesColor  = GetImplementation(series).PointColor(static_cast<uint32_t>(pi));
      }
    }
    if(best.isValid)
    {
      results.push_back(best);
    }
  }

  if(nearestY && !results.empty())
  {
    auto      it      = std::min_element(results.begin(), results.end(),
                                         [&](const HitResult& a, const HitResult& b)
              {
      float da = std::abs(a.canvasPos.y - touchPos.y);
      float db = std::abs(b.canvasPos.y - touchPos.y);
      return da < db;
    });
    HitResult nearest = *it;
    results.clear();
    results.push_back(nearest);
  }

  return results;
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
