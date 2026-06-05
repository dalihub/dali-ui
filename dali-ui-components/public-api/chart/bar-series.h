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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class BarSeries;
}

/**
 * @brief Vertical bar chart data series.
 */
class DALI_UI_API BarSeries : public ChartSeries
{
public:
  static BarSeries New();
  static BarSeries DownCast(BaseHandle handle);

  BarSeries();
  ~BarSeries();
  BarSeries(const BarSeries&)            = default;
  BarSeries& operator=(const BarSeries&) = default;

  BarSeries& SetColor(const Vector4& color);
  Vector4    GetColor() const;

  BarSeries& SetBarGroupWidth(float ratio);
  float      GetBarGroupWidth() const;

  BarSeries& SetStacked(bool stacked);
  bool       IsStacked() const;

  BarSeries& SetDataLabelsVisible(bool visible);
  bool       IsDataLabelsVisible() const;

  BarSeries&   SetDataLabelFormat(const Dali::String& fmt);
  Dali::String GetDataLabelFormat() const;

  BarSeries& SetDataLabelColor(const Vector4& color);
  Vector4    GetDataLabelColor() const;

  BarSeries& SetDataLabelSize(float pointSize);
  float      GetDataLabelSize() const;

  DALI_UI_CHAIN_CHARTSERIES_METHODS_BASE(BarSeries)

public:
  explicit DALI_UI_API BarSeries(Integration::BarSeries* impl);
};

} // namespace Ui
} // namespace Dali
