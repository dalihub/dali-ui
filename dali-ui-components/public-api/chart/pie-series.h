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
class PieSeries;
}

/**
 * @brief Pie/Donut chart data series.
 */
class DALI_UI_API PieSeries : public ChartSeries
{
public:
  PieSeries();
  ~PieSeries();
  PieSeries(const PieSeries& rhs);
  PieSeries(PieSeries&& rhs);
  PieSeries& operator=(const PieSeries& rhs);
  PieSeries& operator=(PieSeries&& rhs);

  static PieSeries New();
  static PieSeries DownCast(BaseHandle handle);

  PieSeries& AddSlice(const Dali::String& label, float value, const Vector4& color);
  PieSeries& ClearSlices();
  int        GetSliceCount() const;

  PieSeries& SetInnerRadiusRatio(float ratio);
  float      GetInnerRadiusRatio() const;

  PieSeries&   SetCenterLabel(const Dali::String& text);
  Dali::String GetCenterLabel() const;

  PieSeries& SetSliceGap(float degrees);
  float      GetSliceGap() const;

  PieSeries& SetDataLabelsVisible(bool visible);
  bool       IsDataLabelsVisible() const;

  PieSeries& SetDataLabelMinAngle(float degrees);
  float      GetDataLabelMinAngle() const;

  DALI_UI_CHAIN_CHARTSERIES_METHODS(PieSeries)

public:
  explicit DALI_UI_API PieSeries(Integration::PieSeries* impl);
};

} // namespace Ui
} // namespace Dali
