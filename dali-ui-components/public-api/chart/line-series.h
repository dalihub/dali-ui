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
#include <functional>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class LineSeries;
}

/**
 * @brief Line chart data series.
 */
class DALI_UI_API LineSeries : public ChartSeries
{
public:
  enum class MarkerShape
  {
    CIRCLE,
    SQUARE,
    TRIANGLE,
    DIAMOND
  };

  LineSeries();
  ~LineSeries();
  LineSeries(const LineSeries& rhs);
  LineSeries(LineSeries&& rhs);
  LineSeries& operator=(const LineSeries& rhs);
  LineSeries& operator=(LineSeries&& rhs);

  static LineSeries New();
  static LineSeries DownCast(BaseHandle handle);

  LineSeries& SetLineWidth(float width);
  float       GetLineWidth() const;

  LineSeries& SetSmoothness(float value);
  float       GetSmoothness() const;

  LineSeries& SetColor(const Vector4& color);
  Vector4     GetColor() const;

  LineSeries& SetMarkersVisible(bool visible);
  bool        IsMarkersVisible() const;

  LineSeries& SetMarkerRadius(float radius);
  float       GetMarkerRadius() const;

  LineSeries& SetMarkerShape(MarkerShape shape);
  MarkerShape GetMarkerShape() const;

  LineSeries& SetMarkerColor(const Vector4& color);
  Vector4     GetMarkerColor() const;

  LineSeries& SetMarkerBorderColor(const Vector4& color);
  Vector4     GetMarkerBorderColor() const;

  LineSeries& SetMarkerBorderWidth(float width);
  float       GetMarkerBorderWidth() const;

  LineSeries& SetDataLabelsVisible(bool visible);
  bool        IsDataLabelsVisible() const;

  LineSeries&  SetDataLabelFormat(const Dali::String& format);
  Dali::String GetDataLabelFormat() const;

  LineSeries& SetDataLabelColor(const Vector4& color);
  Vector4     GetDataLabelColor() const;

  LineSeries& SetDataLabelSize(float pointSize);
  float       GetDataLabelSize() const;

  LineSeries& SetFillEnabled(bool enabled);
  bool        GetFillEnabled() const;

  LineSeries& SetFillColor(const Vector4& color);
  Vector4     GetFillColor() const;

  using DataLabelFormatterType = std::function<Dali::String(float value, int pointIndex)>;
  LineSeries&                   SetDataLabelFormatter(DataLabelFormatterType formatter);
  const DataLabelFormatterType& GetDataLabelFormatter() const;

  DALI_UI_CHAIN_CHARTSERIES_METHODS_BASE(LineSeries)

public:
  explicit DALI_UI_API LineSeries(Integration::LineSeries* impl);
};

} // namespace Ui
} // namespace Dali
