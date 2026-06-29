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
#include <dali-ui-components/public-api/chart/line-series.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/line-series-impl.h>

namespace Dali
{
namespace Ui
{

LineSeries::LineSeries()
{
}

LineSeries::~LineSeries()
{
}

LineSeries LineSeries::New()
{
  Integration::LineSeries* impl = new Integration::LineSeries();
  return LineSeries(impl);
}

LineSeries::LineSeries(const LineSeries& rhs)
: ChartSeries(rhs)
{
}

LineSeries::LineSeries(LineSeries&& rhs)
: ChartSeries(std::move(rhs))
{
}

LineSeries& LineSeries::operator=(const LineSeries& rhs)
{
  if(this != &rhs)
  {
    ChartSeries::operator=(rhs);
  }
  return *this;
}

LineSeries& LineSeries::operator=(LineSeries&& rhs)
{
  if(this != &rhs)
  {
    ChartSeries::operator=(std::move(rhs));
  }
  return *this;
}

LineSeries LineSeries::DownCast(BaseHandle handle)
{
  return LineSeries(dynamic_cast<Integration::LineSeries*>(handle.GetObjectPtr()));
}

void LineSeries::SetLineWidth(float width)
{
  GetImplementation(*this).SetLineWidth(width);
}
float LineSeries::GetLineWidth() const
{
  return GetImplementation(*this).GetLineWidth();
}

void LineSeries::SetSmoothness(float value)
{
  GetImplementation(*this).SetSmoothness(value);
}
float LineSeries::GetSmoothness() const
{
  return GetImplementation(*this).GetSmoothness();
}

void LineSeries::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetColor(color);
}
Vector4 LineSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

void LineSeries::SetMarkersVisible(bool visible)
{
  GetImplementation(*this).SetMarkersVisible(visible);
}
bool LineSeries::IsMarkersVisible() const
{
  return GetImplementation(*this).IsMarkersVisible();
}

void LineSeries::SetMarkerRadius(float radius)
{
  GetImplementation(*this).SetMarkerRadius(radius);
}
float LineSeries::GetMarkerRadius() const
{
  return GetImplementation(*this).GetMarkerRadius();
}

void LineSeries::SetMarkerShape(MarkerShape shape)
{
  GetImplementation(*this).SetMarkerShape(shape);
}
LineSeries::MarkerShape LineSeries::GetMarkerShape() const
{
  return GetImplementation(*this).GetMarkerShape();
}

void LineSeries::SetMarkerColor(const Vector4& color)
{
  GetImplementation(*this).SetMarkerColor(color);
}
Vector4 LineSeries::GetMarkerColor() const
{
  return GetImplementation(*this).GetMarkerColor();
}

void LineSeries::SetMarkerBorderColor(const Vector4& color)
{
  GetImplementation(*this).SetMarkerBorderColor(color);
}
Vector4 LineSeries::GetMarkerBorderColor() const
{
  return GetImplementation(*this).GetMarkerBorderColor();
}

void LineSeries::SetMarkerBorderWidth(float width)
{
  GetImplementation(*this).SetMarkerBorderWidth(width);
}
float LineSeries::GetMarkerBorderWidth() const
{
  return GetImplementation(*this).GetMarkerBorderWidth();
}

void LineSeries::SetDataLabelsVisible(bool visible)
{
  GetImplementation(*this).SetDataLabelsVisible(visible);
}
bool LineSeries::IsDataLabelsVisible() const
{
  return GetImplementation(*this).IsDataLabelsVisible();
}

void LineSeries::SetDataLabelFormat(const Dali::String& format)
{
  GetImplementation(*this).SetDataLabelFormat(format);
}
Dali::String LineSeries::GetDataLabelFormat() const
{
  return GetImplementation(*this).GetDataLabelFormat();
}

void LineSeries::SetDataLabelColor(const Vector4& color)
{
  GetImplementation(*this).SetDataLabelColor(color);
}
Vector4 LineSeries::GetDataLabelColor() const
{
  return GetImplementation(*this).GetDataLabelColor();
}

void LineSeries::SetDataLabelSize(float pointSize)
{
  GetImplementation(*this).SetDataLabelSize(pointSize);
}
float LineSeries::GetDataLabelSize() const
{
  return GetImplementation(*this).GetDataLabelSize();
}

void LineSeries::SetFillEnabled(bool enabled)
{
  GetImplementation(*this).SetFillEnabled(enabled);
}
bool LineSeries::IsFillEnabled() const
{
  return GetImplementation(*this).IsFillEnabled();
}

void LineSeries::SetFillColor(const Vector4& color)
{
  GetImplementation(*this).SetFillColor(color);
}
Vector4 LineSeries::GetFillColor() const
{
  return GetImplementation(*this).GetFillColor();
}

void LineSeries::SetDataLabelFormatter(DataLabelFormatterType formatter)
{
  GetImplementation(*this).SetDataLabelFormatter(std::move(formatter));
}

const LineSeries::DataLabelFormatterType& LineSeries::GetDataLabelFormatter() const
{
  return GetImplementation(*this).GetDataLabelFormatter();
}

LineSeries::LineSeries(Integration::LineSeries* impl)
: ChartSeries(impl)
{
}

} // namespace Ui
} // namespace Dali
