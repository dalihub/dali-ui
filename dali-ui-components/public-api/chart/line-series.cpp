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

LineSeries& LineSeries::SetLineWidth(float width)
{
  GetImplementation(*this).SetLineWidth(width);
  return *this;
}
float LineSeries::GetLineWidth() const
{
  return GetImplementation(*this).GetLineWidth();
}

LineSeries& LineSeries::SetSmoothness(float value)
{
  GetImplementation(*this).SetSmoothness(value);
  return *this;
}
float LineSeries::GetSmoothness() const
{
  return GetImplementation(*this).GetSmoothness();
}

LineSeries& LineSeries::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetColor(color);
  return *this;
}
Vector4 LineSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

LineSeries& LineSeries::SetMarkersVisible(bool visible)
{
  GetImplementation(*this).SetMarkersVisible(visible);
  return *this;
}
bool LineSeries::IsMarkersVisible() const
{
  return GetImplementation(*this).IsMarkersVisible();
}

LineSeries& LineSeries::SetMarkerRadius(float radius)
{
  GetImplementation(*this).SetMarkerRadius(radius);
  return *this;
}
float LineSeries::GetMarkerRadius() const
{
  return GetImplementation(*this).GetMarkerRadius();
}

LineSeries& LineSeries::SetMarkerShape(MarkerShape shape)
{
  GetImplementation(*this).SetMarkerShape(shape);
  return *this;
}
LineSeries::MarkerShape LineSeries::GetMarkerShape() const
{
  return GetImplementation(*this).GetMarkerShape();
}

LineSeries& LineSeries::SetMarkerColor(const Vector4& color)
{
  GetImplementation(*this).SetMarkerColor(color);
  return *this;
}
Vector4 LineSeries::GetMarkerColor() const
{
  return GetImplementation(*this).GetMarkerColor();
}

LineSeries& LineSeries::SetMarkerBorderColor(const Vector4& color)
{
  GetImplementation(*this).SetMarkerBorderColor(color);
  return *this;
}
Vector4 LineSeries::GetMarkerBorderColor() const
{
  return GetImplementation(*this).GetMarkerBorderColor();
}

LineSeries& LineSeries::SetMarkerBorderWidth(float width)
{
  GetImplementation(*this).SetMarkerBorderWidth(width);
  return *this;
}
float LineSeries::GetMarkerBorderWidth() const
{
  return GetImplementation(*this).GetMarkerBorderWidth();
}

LineSeries& LineSeries::SetDataLabelsVisible(bool visible)
{
  GetImplementation(*this).SetDataLabelsVisible(visible);
  return *this;
}
bool LineSeries::IsDataLabelsVisible() const
{
  return GetImplementation(*this).IsDataLabelsVisible();
}

LineSeries& LineSeries::SetDataLabelFormat(const Dali::String& format)
{
  GetImplementation(*this).SetDataLabelFormat(format);
  return *this;
}
Dali::String LineSeries::GetDataLabelFormat() const
{
  return GetImplementation(*this).GetDataLabelFormat();
}

LineSeries& LineSeries::SetDataLabelColor(const Vector4& color)
{
  GetImplementation(*this).SetDataLabelColor(color);
  return *this;
}
Vector4 LineSeries::GetDataLabelColor() const
{
  return GetImplementation(*this).GetDataLabelColor();
}

LineSeries& LineSeries::SetDataLabelSize(float pointSize)
{
  GetImplementation(*this).SetDataLabelSize(pointSize);
  return *this;
}
float LineSeries::GetDataLabelSize() const
{
  return GetImplementation(*this).GetDataLabelSize();
}

LineSeries& LineSeries::SetFillEnabled(bool enabled)
{
  GetImplementation(*this).SetFillEnabled(enabled);
  return *this;
}
bool LineSeries::GetFillEnabled() const
{
  return GetImplementation(*this).GetFillEnabled();
}

LineSeries& LineSeries::SetFillColor(const Vector4& color)
{
  GetImplementation(*this).SetFillColor(color);
  return *this;
}
Vector4 LineSeries::GetFillColor() const
{
  return GetImplementation(*this).GetFillColor();
}

LineSeries& LineSeries::SetDataLabelFormatter(DataLabelFormatterType formatter)
{
  GetImplementation(*this).SetDataLabelFormatter(std::move(formatter));
  return *this;
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
