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
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/public-api/chart/line-series.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class LineSeries : public ChartSeries
{
public:
  LineSeries();

  float GetLineWidth() const
  {
    return mLineWidth;
  }
  Vector4 GetColor() const
  {
    return mColor;
  }
  bool IsMarkersVisible() const
  {
    return mShowMarkers;
  }
  float GetMarkerRadius() const
  {
    return mMarkerRadius;
  }

  void  SetSmoothness(float value);
  float GetSmoothness() const
  {
    return mSmoothness;
  }

  void SetLineWidth(float width);
  void SetColor(const Vector4& color);
  void SetMarkersVisible(bool visible);
  void SetMarkerRadius(float radius);

  void                        SetMarkerShape(Ui::LineSeries::MarkerShape shape);
  Ui::LineSeries::MarkerShape GetMarkerShape() const
  {
    return mMarkerShape;
  }

  void    SetMarkerColor(const Vector4& color);
  Vector4 GetMarkerColor() const;

  void    SetMarkerBorderColor(const Vector4& color);
  Vector4 GetMarkerBorderColor() const
  {
    return mMarkerBorderColor;
  }

  void  SetMarkerBorderWidth(float width);
  float GetMarkerBorderWidth() const
  {
    return mMarkerBorderWidth;
  }

  void SetDataLabelsVisible(bool visible);
  bool IsDataLabelsVisible() const
  {
    return mShowDataLabels;
  }

  void                SetDataLabelFormat(const Dali::String& format);
  const Dali::String& GetDataLabelFormat() const
  {
    return mDataLabelFormat;
  }

  void    SetDataLabelColor(const Vector4& color);
  Vector4 GetDataLabelColor() const
  {
    return mDataLabelColor;
  }

  void  SetDataLabelSize(float pointSize);
  float GetDataLabelSize() const
  {
    return mDataLabelSize;
  }

  void SetFillEnabled(bool enabled);
  bool GetFillEnabled() const
  {
    return mFillEnabled;
  }

  void    SetFillColor(const Vector4& color);
  Vector4 GetFillColor() const;

  using DataLabelFormatterType = Ui::LineSeries::DataLabelFormatterType;
  void                          SetDataLabelFormatter(DataLabelFormatterType formatter);
  const DataLabelFormatterType& GetDataLabelFormatter() const
  {
    return mDataLabelFormatter;
  }

protected:
  ~LineSeries() override;

private:
  DataLabelFormatterType      mDataLabelFormatter{nullptr};
  Ui::LineSeries::MarkerShape mMarkerShape{Ui::LineSeries::MarkerShape::CIRCLE};
  Dali::String                mDataLabelFormat{"%.1f"};
  Vector4                     mColor;
  Vector4                     mMarkerColor;
  Vector4                     mMarkerBorderColor{0.0f, 0.0f, 0.0f, 0.0f};
  Vector4                     mDataLabelColor{0.2f, 0.2f, 0.2f, 1.0f};
  Vector4                     mFillColor;
  float                       mLineWidth;
  float                       mMarkerRadius;
  float                       mMarkerBorderWidth{0.0f};
  float                       mDataLabelSize{8.0f};
  float                       mSmoothness{0.0f};
  bool                        mShowMarkers;
  bool                        mMarkerColorOverride{false};
  bool                        mShowDataLabels{false};
  bool                        mFillEnabled{false};
  bool                        mFillColorOverride{false};
};

} // namespace Integration

inline Integration::LineSeries& GetImplementation(Ui::LineSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "LineSeries handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Integration::LineSeries&>(object);
}

inline const Integration::LineSeries& GetImplementation(const Ui::LineSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "LineSeries handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Integration::LineSeries&>(object);
}

} // namespace Ui
} // namespace Dali
