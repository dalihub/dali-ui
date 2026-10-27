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

#include <dali-ui-components/integration-api/chart/chart-color-palette.h>
#include <dali-ui-components/integration-api/chart/chart-model.h>
#include <dali-ui-components/integration-api/chart/chart-scale-engine.h>
#include <map>
#include <numeric>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
/**
 * @brief Describes one rounded bar body in canvas pixels for rendering, animation and selection.
 */
struct ChartBarGeometry
{
  Rect<float> bounds;
  Vector4     color;
  size_t      seriesIndex;
  size_t      pointIndex;
  float       radius;
  float       dataX;
  float       dataY;

  /**
   * @brief Tests the visible rounded body rather than its enclosing rectangle.
   * @param[in] point The canvas position in pixels
   * @return True when the point lies inside the rounded body
   */
  bool Contains(const Vector2& point) const
  {
    if(point.x < bounds.x || point.x > bounds.x + bounds.width ||
       point.y < bounds.y || point.y > bounds.y + bounds.height)
    {
      return false;
    }
    const float   x     = std::clamp(point.x, bounds.x + radius, bounds.x + bounds.width - radius);
    const float   y     = std::clamp(point.y, bounds.y + radius, bounds.y + bounds.height - radius);
    const Vector2 delta = point - Vector2(x, y);
    return delta.LengthSquared() <= radius * radius + 0.001f;
  }
};

/**
 * @brief Interpolates bar bounds by stable series and data indices, collapsing added or removed bars.
 * @param[in] before The last displayed geometry
 * @param[in] after The destination geometry
 * @param[in] progress The normalized transition progress, clamped to zero through one
 * @return The geometry displayed at this progress
 */
inline std::vector<ChartBarGeometry> InterpolateChartBars(const std::vector<ChartBarGeometry>& before,
                                                          const std::vector<ChartBarGeometry>& after, float progress)
{
  const float t = std::clamp(progress, 0.0f, 1.0f);
  if(t >= 1.0f)
  {
    return after;
  }
  auto                                                         result = after;
  std::map<std::pair<size_t, size_t>, const ChartBarGeometry*> previous;
  for(const auto& old : before)
  {
    previous.emplace(std::make_pair(old.seriesIndex, old.pointIndex), &old);
  }
  auto collapse = [](ChartBarGeometry bar)
  {
    bar.bounds.y += bar.dataY >= 0.0f ? bar.bounds.height : 0.0f;
    bar.bounds.height = 0.0f;
    bar.radius        = 0.0f;
    return bar;
  };
  auto blend = [t](ChartBarGeometry& target, const ChartBarGeometry& start)
  {
    target.bounds.x      = start.bounds.x + (target.bounds.x - start.bounds.x) * t;
    target.bounds.y      = start.bounds.y + (target.bounds.y - start.bounds.y) * t;
    target.bounds.width  = start.bounds.width + (target.bounds.width - start.bounds.width) * t;
    target.bounds.height = start.bounds.height + (target.bounds.height - start.bounds.height) * t;
    target.radius        = std::min({start.radius + (target.radius - start.radius) * t,
                                     target.bounds.width * .5f, target.bounds.height * .5f});
  };
  for(auto& target : result)
  {
    const auto found = previous.find(std::make_pair(target.seriesIndex, target.pointIndex));
    blend(target, found == previous.end() ? collapse(target) : *found->second);
    if(found != previous.end())
    {
      previous.erase(found);
    }
  }
  for(const auto& remaining : previous)
  {
    const auto& old     = *remaining.second;
    auto        removed = collapse(old);
    blend(removed, old);
    result.push_back(removed);
  }
  return result;
}

/**
 * @brief Builds visible bar bodies using the current axes, grouping, stacking and point colors.
 * @param[in] model The chart data and appearance
 * @param[in] scale The data-to-canvas coordinate mapping
 * @return Bar geometry clipped to the plot area, omitting invalid data
 */
inline std::vector<ChartBarGeometry> BuildChartBars(const ChartModel& model, const ScaleEngine& scale)
{
  std::vector<size_t> order(model.mSeriesList.size());
  std::iota(order.begin(), order.end(), 0u);
  std::stable_sort(order.begin(), order.end(), [&](size_t a, size_t b)
  {
    return GetImplementation(model.mSeriesList[a]).GetZIndex() < GetImplementation(model.mSeriesList[b]).GetZIndex();
  });
  std::vector<size_t> grouped;
  for(size_t index : order)
  {
    auto bar = Ui::BarSeries::DownCast(model.mSeriesList[index]);
    if(bar && bar.IsVisible() && !bar.IsStacked())
    {
      grouped.push_back(index);
    }
  }
  const auto&                   area = scale.GetPlotArea();
  std::map<float, float>        positive, negative;
  std::vector<ChartBarGeometry> result;
  for(size_t index : order)
  {
    auto bar = Ui::BarSeries::DownCast(model.mSeriesList[index]);
    if(!bar || !bar.IsVisible())
    {
      continue;
    }
    const auto& impl   = GetImplementation(bar);
    const auto& points = impl.GetValues();
    float       slot   = std::abs(scale.ToCanvasX(1.0f) - scale.ToCanvasX(0.0f));
    if(GetImplementation(model.mXAxis).GetLabels().empty())
    {
      slot = area.width;
      for(size_t i = 1; i < points.size(); ++i)
      {
        const float gap = std::abs(scale.ToCanvasX(points[i].first) - scale.ToCanvasX(points[i - 1].first));
        if(std::isfinite(gap) && gap > 0.0f)
        {
          slot = std::min(slot, gap);
        }
      }
    }
    const float  groupWidth = slot * impl.GetBarGroupWidth();
    const float  width      = impl.IsStacked() ? groupWidth : groupWidth / std::max(size_t(1), grouped.size());
    const size_t column     = std::distance(grouped.begin(), std::find(grouped.begin(), grouped.end(), index));
    for(size_t i = 0; i < points.size(); ++i)
    {
      const auto& point = points[i];
      if(!scale.IsValid(point.first, point.second))
      {
        continue;
      }
      float lower = impl.LowerValue(i);
      float upper = point.second;
      if(impl.IsStacked())
      {
        float& sum = upper >= 0.0f ? positive[point.first] : negative[point.first];
        lower      = sum;
        sum += upper;
        upper = sum;
      }
      if(impl.HasRangeValues() && (!std::isfinite(lower) || (scale.IsLogY() && lower <= 0.0f)))
      {
        continue;
      }
      if(scale.IsLogY() && lower <= 0.0f)
      {
        lower = scale.GetYMin();
      }
      const float a      = std::clamp(scale.ToCanvasY(lower), area.y, area.y + area.height);
      const float b      = std::clamp(scale.ToCanvasY(upper), area.y, area.y + area.height);
      const float left   = scale.ToCanvasX(point.first) - groupWidth * 0.5f + (impl.IsStacked() ? 0.0f : column * width);
      const float x      = std::max(area.x, left);
      const float right  = std::min(area.x + area.width, left + width);
      const float height = std::abs(a - b);
      if(right <= x || height < 0.5f)
      {
        continue;
      }
      const auto    override = impl.mPointColors.find(static_cast<uint32_t>(i));
      const Vector4 color    = override != impl.mPointColors.end() ? override->second.GetRgba() : (impl.IsColorSet() ? impl.GetColor() : ChartColorPalette::Get(static_cast<int>(index)));
      result.push_back({Rect<float>(x, std::min(a, b), right - x, height), color, index, i,
                        std::min({impl.mCornerRadius, (right - x) * 0.5f, height * 0.5f}), point.first, point.second});
    }
  }
  return result;
}
} // namespace Integration
} // namespace Ui
} // namespace DALI_NAMESPACE
