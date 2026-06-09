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

  void  SetLineWidth(float width);
  float GetLineWidth() const;

  void  SetSmoothness(float value);
  float GetSmoothness() const;

  void    SetColor(const Vector4& color);
  Vector4 GetColor() const;

  void SetMarkersVisible(bool visible);
  bool IsMarkersVisible() const;

  void  SetMarkerRadius(float radius);
  float GetMarkerRadius() const;

  void        SetMarkerShape(MarkerShape shape);
  MarkerShape GetMarkerShape() const;

  void    SetMarkerColor(const Vector4& color);
  Vector4 GetMarkerColor() const;

  void    SetMarkerBorderColor(const Vector4& color);
  Vector4 GetMarkerBorderColor() const;

  void  SetMarkerBorderWidth(float width);
  float GetMarkerBorderWidth() const;

  void SetDataLabelsVisible(bool visible);
  bool IsDataLabelsVisible() const;

  void         SetDataLabelFormat(const Dali::String& format);
  Dali::String GetDataLabelFormat() const;

  void    SetDataLabelColor(const Vector4& color);
  Vector4 GetDataLabelColor() const;

  void  SetDataLabelSize(float pointSize);
  float GetDataLabelSize() const;

  void SetFillEnabled(bool enabled);
  bool GetFillEnabled() const;

  void    SetFillColor(const Vector4& color);
  Vector4 GetFillColor() const;

  using DataLabelFormatterType = std::function<Dali::String(float value, int pointIndex)>;
  void                          SetDataLabelFormatter(DataLabelFormatterType formatter);
  const DataLabelFormatterType& GetDataLabelFormatter() const;

public:
  explicit DALI_UI_API LineSeries(Integration::LineSeries* impl);
};

} // namespace Ui
} // namespace Dali
