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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ScatterSeries;
}

/**
 * @brief Scatter plot data series.
 */
class DALI_UI_COMPONENTS_API ScatterSeries : public ChartSeries
{
public:
  enum class MarkerShape
  {
    CIRCLE,
    SQUARE,
    TRIANGLE,
    DIAMOND
  };

  static ScatterSeries New();
  static ScatterSeries DownCast(BaseHandle handle);

  ScatterSeries();
  ~ScatterSeries();
  ScatterSeries(const ScatterSeries&)            = default;
  ScatterSeries& operator=(const ScatterSeries&) = default;

  void    SetColor(const Vector4& color);
  Vector4 GetColor() const;

  void  SetMarkerRadius(float radius);
  float GetMarkerRadius() const;

  void        SetMarkerShape(MarkerShape shape);
  MarkerShape GetMarkerShape() const;

public:
  explicit ScatterSeries(Integration::ScatterSeries* impl);
};

} // namespace Ui
} // namespace Dali
