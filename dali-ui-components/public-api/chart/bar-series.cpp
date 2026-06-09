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
#include <dali-ui-components/public-api/chart/bar-series.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/bar-series-impl.h>

namespace Dali
{
namespace Ui
{

BarSeries BarSeries::New()
{
  Integration::BarSeries* impl = new Integration::BarSeries();
  return BarSeries(impl);
}

BarSeries::BarSeries()  = default;
BarSeries::~BarSeries() = default;

BarSeries::BarSeries(Integration::BarSeries* impl)
: ChartSeries(impl)
{
}

BarSeries BarSeries::DownCast(BaseHandle handle)
{
  return BarSeries(dynamic_cast<Integration::BarSeries*>(handle.GetObjectPtr()));
}

void BarSeries::SetColor(const Vector4& c)
{
  GetImplementation(*this).SetColor(c);
}
Vector4 BarSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

void BarSeries::SetBarGroupWidth(float r)
{
  GetImplementation(*this).SetBarGroupWidth(r);
}
float BarSeries::GetBarGroupWidth() const
{
  return GetImplementation(*this).GetBarGroupWidth();
}

void BarSeries::SetStacked(bool s)
{
  GetImplementation(*this).SetStacked(s);
}
bool BarSeries::IsStacked() const
{
  return GetImplementation(*this).IsStacked();
}

void BarSeries::SetDataLabelsVisible(bool v)
{
  GetImplementation(*this).SetDataLabelsVisible(v);
}
bool BarSeries::IsDataLabelsVisible() const
{
  return GetImplementation(*this).IsDataLabelsVisible();
}

void BarSeries::SetDataLabelFormat(const Dali::String& f)
{
  GetImplementation(*this).SetDataLabelFormat(f);
}
Dali::String BarSeries::GetDataLabelFormat() const
{
  return GetImplementation(*this).GetDataLabelFormat();
}

void BarSeries::SetDataLabelColor(const Vector4& c)
{
  GetImplementation(*this).SetDataLabelColor(c);
}
Vector4 BarSeries::GetDataLabelColor() const
{
  return GetImplementation(*this).GetDataLabelColor();
}

void BarSeries::SetDataLabelSize(float s)
{
  GetImplementation(*this).SetDataLabelSize(s);
}
float BarSeries::GetDataLabelSize() const
{
  return GetImplementation(*this).GetDataLabelSize();
}

} // namespace Ui
} // namespace Dali
