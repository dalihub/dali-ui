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
 */

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-hit-tester.h>
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
/**
 * @brief Shared rendered sector and selection geometry for a pie or donut.
 */
struct ChartPieSliceGeometry
{
  HitResult hit;
  Vector2   center;
  float     innerRadius;
  float     outerRadius;
  float     startAngle;
  float     sweepAngle;

  /**
   * @brief Tests the rendered sector, excluding the donut hole and slice gaps.
   * @param[in] position The canvas position in pixels
   * @return True when the position is inside this sector
   */
  bool Contains(const Vector2& position) const
  {
    const Vector2 offset   = position - center;
    const float   distance = offset.Length();
    if(distance < innerRadius || distance > outerRadius || sweepAngle <= 0.0f)
    {
      return false;
    }
    const float angle    = std::atan2(offset.y, offset.x) * 180.0f / Math::PI;
    float       relative = std::fmod(angle - startAngle, 360.0f);
    if(relative < 0.0f)
    {
      relative += 360.0f;
    }
    return relative <= sweepAngle;
  }
};

/**
 * @brief Builds shared rendering and selection geometry from finite positive slices.
 * @param[in] model The chart data and appearance
 * @param[in] plotArea The canvas area in pixels
 * @return The visible pie or donut sectors
 */
inline std::vector<ChartPieSliceGeometry> BuildChartPieSlices(const ChartModel& model, const Rect<float>& plotArea)
{
  std::vector<ChartPieSliceGeometry> result;
  if(plotArea.width <= 0.0f || plotArea.height <= 0.0f)
  {
    return result;
  }
  for(size_t seriesIndex = 0u; seriesIndex < model.mSeriesList.size(); ++seriesIndex)
  {
    const auto pie = Ui::PieSeries::DownCast(model.mSeriesList[seriesIndex]);
    if(!pie || !GetImplementation(pie).IsVisible())
    {
      continue;
    }
    const auto& impl  = GetImplementation(pie);
    float       total = 0.0f;
    for(const auto& slice : impl.GetSlices())
    {
      if(std::isfinite(slice.value) && slice.value > 0.0f)
      {
        total += slice.value;
      }
    }
    if(!std::isfinite(total) || total <= 0.0f)
    {
      return result;
    }
    const float   outerRadius = std::min(plotArea.width, plotArea.height) * 0.5f;
    const float   innerRadius = outerRadius * impl.GetInnerRadiusRatio();
    const Vector2 center(plotArea.x + plotArea.width * 0.5f, plotArea.y + plotArea.height * 0.5f);
    float         start  = 270.0f;
    const auto&   slices = impl.GetSlices();
    for(size_t index = 0u; index < slices.size(); ++index)
    {
      const auto& slice = slices[index];
      if(!std::isfinite(slice.value) || slice.value <= 0.0f)
      {
        continue;
      }
      const float sweep       = slice.value / total * 360.0f;
      const float renderStart = start + impl.GetSliceGap() * 0.5f;
      const float renderSweep = sweep - impl.GetSliceGap();
      start += sweep;
      if(renderSweep <= 0.0f)
      {
        continue;
      }
      const float middle = (renderStart + renderSweep * 0.5f) * Math::PI / 180.0f;
      HitResult   hit;
      hit.isValid     = true;
      hit.seriesIndex = static_cast<int>(seriesIndex);
      hit.pointIndex  = static_cast<int>(index);
      hit.dataX       = static_cast<float>(index);
      hit.dataY       = slice.value;
      hit.seriesName  = impl.GetName();
      hit.xLabel      = slice.label;
      hit.seriesColor = slice.color;
      hit.canvasPos   = center + Vector2(std::cos(middle), std::sin(middle)) * ((innerRadius + outerRadius) * 0.5f);
      result.push_back({hit, center, innerRadius, outerRadius, renderStart, renderSweep});
    }
    break;
  }
  return result;
}
} // namespace Integration
} // namespace Ui
} // namespace DALI_NAMESPACE
