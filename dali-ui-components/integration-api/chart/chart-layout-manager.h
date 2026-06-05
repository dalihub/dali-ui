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
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-model.h>
#include <dali-ui-components/integration-api/chart/chart-scale-engine.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ChartLayoutManager
{
public:
  struct TickLabel
  {
    Vector2      position;
    Dali::String text;
    float        angle{0.0f};
  };

  struct LegendItem
  {
    Vector2      iconCenter;
    Vector2      textPos;
    Dali::String name;
  };

  struct LayoutResult
  {
    std::vector<TickLabel>  xTickLabels;
    std::vector<TickLabel>  yTickLabels;
    std::vector<LegendItem> legendItems;
    std::vector<float>      yTicks;
    Vector2                 totalSize;
    Rect<float>             plotArea;
    Vector2                 titlePos;
    Vector2                 xAxisTitlePos;
    Vector2                 yAxisTitlePos;
    int                     xTickCount{0};
    bool                    hasTitle{false};
    bool                    hasXAxisTitle{false};
    bool                    hasYAxisTitle{false};
    bool                    hasLegend{false};
  };

  static constexpr float PADDING        = 10.0f;
  static constexpr float TICK_LENGTH    = 5.0f;
  static constexpr float TICK_LABEL_GAP = 4.0f;
  static constexpr float LEGEND_SWATCH  = 12.0f;
  static constexpr float LEGEND_GAP     = 6.0f;

  LayoutResult ComputeLayout(const Vector2&    totalSize,
                             const ChartModel& model,
                             ScaleEngine&      scale);

private:
  static float        EstimateTextWidth(const Dali::String& text, float pointSize);
  static float        EstimateTextHeight(float pointSize);
  static Dali::String FormatTickValue(float value);
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
