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
class DALI_UI_COMPONENTS_API PieSeries : public ChartSeries
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

  void AddSlice(const Dali::String& label, float value, const Vector4& color);
  void ClearSlices();
  int  GetSliceCount() const;

  void  SetInnerRadiusRatio(float ratio);
  float GetInnerRadiusRatio() const;

  void         SetCenterLabel(const Dali::String& text);
  Dali::String GetCenterLabel() const;

  void  SetSliceGap(float degrees);
  float GetSliceGap() const;

  void SetDataLabelsVisible(bool visible);
  bool IsDataLabelsVisible() const;

  void  SetDataLabelMinAngle(float degrees);
  float GetDataLabelMinAngle() const;

public:
  explicit PieSeries(Integration::PieSeries* impl);
};

} // namespace Ui
} // namespace Dali
