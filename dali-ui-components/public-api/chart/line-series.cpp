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

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/line-series-impl.h>

namespace DALI_NAMESPACE
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
  GetImplementation(*this).NotifyChanged();
}

float LineSeries::GetLineWidth() const
{
  return GetImplementation(*this).GetLineWidth();
}

void LineSeries::SetSmoothness(float value)
{
  GetImplementation(*this).SetSmoothness(value);
  GetImplementation(*this).NotifyChanged();
}

float LineSeries::GetSmoothness() const
{
  return GetImplementation(*this).GetSmoothness();
}

void LineSeries::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetColor(color);
  GetImplementation(*this).NotifyChanged();
}

Vector4 LineSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

void LineSeries::SetMarkersVisible(bool visible)
{
  GetImplementation(*this).SetMarkersVisible(visible);
  GetImplementation(*this).NotifyChanged();
}

bool LineSeries::IsMarkersVisible() const
{
  return GetImplementation(*this).IsMarkersVisible();
}

void LineSeries::SetMarkerRadius(float radius)
{
  GetImplementation(*this).SetMarkerRadius(radius);
  GetImplementation(*this).NotifyChanged();
}

float LineSeries::GetMarkerRadius() const
{
  return GetImplementation(*this).GetMarkerRadius();
}

void LineSeries::SetMarkerShape(MarkerShape shape)
{
  GetImplementation(*this).SetMarkerShape(shape);
  GetImplementation(*this).NotifyChanged();
}

LineSeries::MarkerShape LineSeries::GetMarkerShape() const
{
  return GetImplementation(*this).GetMarkerShape();
}

void LineSeries::SetMarkerColor(const Vector4& color)
{
  GetImplementation(*this).SetMarkerColor(color);
  GetImplementation(*this).NotifyChanged();
}

Vector4 LineSeries::GetMarkerColor() const
{
  return GetImplementation(*this).GetMarkerColor();
}

void LineSeries::SetMarkerBorderColor(const Vector4& color)
{
  GetImplementation(*this).SetMarkerBorderColor(color);
  GetImplementation(*this).NotifyChanged();
}

Vector4 LineSeries::GetMarkerBorderColor() const
{
  return GetImplementation(*this).GetMarkerBorderColor();
}

void LineSeries::SetMarkerBorderWidth(float width)
{
  GetImplementation(*this).SetMarkerBorderWidth(width);
  GetImplementation(*this).NotifyChanged();
}

float LineSeries::GetMarkerBorderWidth() const
{
  return GetImplementation(*this).GetMarkerBorderWidth();
}

void LineSeries::SetDataLabelsVisible(bool visible)
{
  GetImplementation(*this).SetDataLabelsVisible(visible);
  GetImplementation(*this).NotifyChanged();
}

bool LineSeries::IsDataLabelsVisible() const
{
  return GetImplementation(*this).IsDataLabelsVisible();
}

void LineSeries::SetDataLabelFormat(const Dali::String& format)
{
  GetImplementation(*this).SetDataLabelFormat(format);
  GetImplementation(*this).NotifyChanged();
}

Dali::String LineSeries::GetDataLabelFormat() const
{
  return GetImplementation(*this).GetDataLabelFormat();
}

void LineSeries::SetDataLabelColor(const Vector4& color)
{
  GetImplementation(*this).SetDataLabelColor(color);
  GetImplementation(*this).NotifyChanged();
}

Vector4 LineSeries::GetDataLabelColor() const
{
  return GetImplementation(*this).GetDataLabelColor();
}

void LineSeries::SetDataLabelSize(float pointSize)
{
  GetImplementation(*this).SetDataLabelSize(pointSize);
  GetImplementation(*this).NotifyChanged();
}

float LineSeries::GetDataLabelSize() const
{
  return GetImplementation(*this).GetDataLabelSize();
}

void LineSeries::SetFillEnabled(bool enabled)
{
  GetImplementation(*this).SetFillEnabled(enabled);
  GetImplementation(*this).NotifyChanged();
}

bool LineSeries::IsFillEnabled() const
{
  return GetImplementation(*this).IsFillEnabled();
}

void LineSeries::SetFillColor(const Vector4& color)
{
  GetImplementation(*this).SetFillColor(color);
  GetImplementation(*this).NotifyChanged();
}

Vector4 LineSeries::GetFillColor() const
{
  return GetImplementation(*this).GetFillColor();
}

void LineSeries::SetDataLabelFormatter(DataLabelFormatterType formatter)
{
  GetImplementation(*this).SetDataLabelFormatter(formatter);
  GetImplementation(*this).NotifyChanged();
}

LineSeries::DataLabelFormatterType LineSeries::GetDataLabelFormatter() const
{
  return GetImplementation(*this).GetDataLabelFormatter();
}

LineSeries::LineSeries(Integration::LineSeries* impl)
: ChartSeries(impl)
{
}

void LineSeries::SetFillGradient(const Gradient::Linear& gradient)
{
  auto& impl            = GetImplementation(*this);
  impl.mFillGradient    = gradient;
  impl.mHasFillGradient = gradient.GetType() == Gradient::Type::LINEAR;
  impl.NotifyChanged();
}

void LineSeries::ClearFillGradient()
{
  auto& impl            = GetImplementation(*this);
  impl.mHasFillGradient = false;
  impl.NotifyChanged();
}

void LineSeries::SetStrokeGradient(const Gradient::Linear& gradient)
{
  auto& impl              = GetImplementation(*this);
  impl.mStrokeGradient    = gradient;
  impl.mHasStrokeGradient = gradient.GetType() == Gradient::Type::LINEAR;
  impl.NotifyChanged();
}

void LineSeries::ClearStrokeGradient()
{
  auto& impl              = GetImplementation(*this);
  impl.mHasStrokeGradient = false;
  impl.NotifyChanged();
}

void LineSeries::SetLineDash(float dashLength, float gapLength)
{
  DALI_ASSERT_ALWAYS(std::isfinite(dashLength) && dashLength > 0.0f && std::isfinite(gapLength) && gapLength >= 0.0f);
  auto& impl       = GetImplementation(*this);
  impl.mDashLength = dashLength;
  impl.mGapLength  = gapLength;
  impl.NotifyChanged();
}

void LineSeries::SetSegmentStyle(uint32_t firstPoint, uint32_t lastPoint, const UiColor& color, float dashLength, float gapLength)
{
  DALI_ASSERT_ALWAYS(firstPoint < lastPoint && std::isfinite(dashLength) && dashLength > 0.0f && std::isfinite(gapLength) && gapLength >= 0.0f);
  auto& impl = GetImplementation(*this);
  impl.mSegments.push_back({color, firstPoint, lastPoint, dashLength, gapLength});
  impl.NotifyChanged();
}

void LineSeries::ClearSegmentStyles()
{
  auto& impl = GetImplementation(*this);
  impl.mSegments.clear();
  impl.NotifyChanged();
}

} // namespace Ui
} //namespace DALI_NAMESPACE
