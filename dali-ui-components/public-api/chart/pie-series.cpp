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
#include <dali-ui-components/public-api/chart/pie-series.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>

namespace Dali
{
namespace Ui
{

PieSeries::PieSeries()                                = default;
PieSeries::~PieSeries()                               = default;
PieSeries::PieSeries(const PieSeries& rhs)            = default;
PieSeries::PieSeries(PieSeries&& rhs)                 = default;
PieSeries& PieSeries::operator=(const PieSeries& rhs) = default;
PieSeries& PieSeries::operator=(PieSeries&& rhs)      = default;

PieSeries PieSeries::New()
{
  Integration::PieSeries* impl = new Integration::PieSeries();
  return PieSeries(impl);
}

PieSeries PieSeries::DownCast(BaseHandle handle)
{
  return PieSeries(dynamic_cast<Integration::PieSeries*>(handle.GetObjectPtr()));
}

PieSeries::PieSeries(Integration::PieSeries* impl)
: ChartSeries(impl)
{
}

void PieSeries::AddSlice(const Dali::String& label, float value, const Vector4& color)
{
  GetImplementation(*this).AddSlice(label, value, color);
}

void PieSeries::ClearSlices()
{
  GetImplementation(*this).ClearSlices();
}

int PieSeries::GetSliceCount() const
{
  return GetImplementation(*this).GetSliceCount();
}

void PieSeries::SetInnerRadiusRatio(float r)
{
  GetImplementation(*this).SetInnerRadiusRatio(r);
}
float PieSeries::GetInnerRadiusRatio() const
{
  return GetImplementation(*this).GetInnerRadiusRatio();
}

void PieSeries::SetCenterLabel(const Dali::String& t)
{
  GetImplementation(*this).SetCenterLabel(t);
}
Dali::String PieSeries::GetCenterLabel() const
{
  return GetImplementation(*this).GetCenterLabel();
}

void PieSeries::SetSliceGap(float d)
{
  GetImplementation(*this).SetSliceGap(d);
}
float PieSeries::GetSliceGap() const
{
  return GetImplementation(*this).GetSliceGap();
}

void PieSeries::SetDataLabelsVisible(bool v)
{
  GetImplementation(*this).SetDataLabelsVisible(v);
}
bool PieSeries::IsDataLabelsVisible() const
{
  return GetImplementation(*this).IsDataLabelsVisible();
}

void PieSeries::SetDataLabelMinAngle(float d)
{
  GetImplementation(*this).SetDataLabelMinAngle(d);
}
float PieSeries::GetDataLabelMinAngle() const
{
  return GetImplementation(*this).GetDataLabelMinAngle();
}

} // namespace Ui
} // namespace Dali
