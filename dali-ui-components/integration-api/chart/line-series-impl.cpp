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

// CLASS HEADER
#include <dali-ui-components/integration-api/chart/line-series-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/vector-wrapper.h>
#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Integration
{

LineSeries::LineSeries()
: mColor(Color::BLACK),
  mLineWidth(2.5f),
  mMarkerRadius(4.0f),
  mShowMarkers(true)
{
}

LineSeries::~LineSeries()
{
}

void LineSeries::SetLineWidth(float width)
{
  if(mLineWidth != width)
  {
    mLineWidth = width;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetColor(const Vector4& color)
{
  if(mColor != color)
  {
    mColor = color;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetMarkersVisible(bool visible)
{
  if(mShowMarkers != visible)
  {
    mShowMarkers = visible;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetMarkerRadius(float radius)
{
  if(mMarkerRadius != radius)
  {
    mMarkerRadius = radius;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetMarkerShape(Ui::LineSeries::MarkerShape shape)
{
  if(mMarkerShape != shape)
  {
    mMarkerShape = shape;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetMarkerColor(const Vector4& color)
{
  if(mMarkerColor != color || !mMarkerColorOverride)
  {
    mMarkerColor         = color;
    mMarkerColorOverride = true;
    EmitDataChangedSignal();
  }
}

Vector4 LineSeries::GetMarkerColor() const
{
  return mMarkerColorOverride ? mMarkerColor : mColor;
}

void LineSeries::SetMarkerBorderColor(const Vector4& color)
{
  if(mMarkerBorderColor != color)
  {
    mMarkerBorderColor = color;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetMarkerBorderWidth(float width)
{
  if(mMarkerBorderWidth != width)
  {
    mMarkerBorderWidth = width;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetDataLabelsVisible(bool visible)
{
  if(mShowDataLabels != visible)
  {
    mShowDataLabels = visible;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetDataLabelFormat(const Dali::String& format)
{
  if(mDataLabelFormat != format)
  {
    mDataLabelFormat = format;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetDataLabelColor(const Vector4& color)
{
  if(mDataLabelColor != color)
  {
    mDataLabelColor = color;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetDataLabelSize(float pointSize)
{
  if(mDataLabelSize != pointSize)
  {
    mDataLabelSize = pointSize;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetFillEnabled(bool enabled)
{
  if(mFillEnabled != enabled)
  {
    mFillEnabled = enabled;
    EmitDataChangedSignal();
  }
}

void LineSeries::SetFillColor(const Vector4& color)
{
  if(mFillColor != color || !mFillColorOverride)
  {
    mFillColor         = color;
    mFillColorOverride = true;
    EmitDataChangedSignal();
  }
}

Vector4 LineSeries::GetFillColor() const
{
  if(mFillColorOverride)
  {
    return mFillColor;
  }
  return Vector4(mColor.r, mColor.g, mColor.b, 0.2f);
}

void LineSeries::SetDataLabelFormatter(DataLabelFormatterType formatter)
{
  mDataLabelFormatter = std::move(formatter);
  EmitDataChangedSignal();
}

void LineSeries::SetSmoothness(float value)
{
  mSmoothness = std::min(std::max(value, 0.0f), 1.0f);
  EmitDataChangedSignal();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
