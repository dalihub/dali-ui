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
#include <dali-ui-components/public-api/chart/scatter-series.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/scatter-series-impl.h>

namespace Dali
{
namespace Ui
{

ScatterSeries ScatterSeries::New()
{
  Integration::ScatterSeries* impl = new Integration::ScatterSeries();
  return ScatterSeries(impl);
}

ScatterSeries::ScatterSeries()  = default;
ScatterSeries::~ScatterSeries() = default;

ScatterSeries::ScatterSeries(Integration::ScatterSeries* impl)
: ChartSeries(impl)
{
}

ScatterSeries ScatterSeries::DownCast(BaseHandle handle)
{
  return ScatterSeries(dynamic_cast<Integration::ScatterSeries*>(handle.GetObjectPtr()));
}

void ScatterSeries::SetColor(const Vector4& c)
{
  GetImplementation(*this).SetColor(c);
}
Vector4 ScatterSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

void ScatterSeries::SetMarkerRadius(float r)
{
  GetImplementation(*this).SetMarkerRadius(r);
}
float ScatterSeries::GetMarkerRadius() const
{
  return GetImplementation(*this).GetMarkerRadius();
}

void ScatterSeries::SetMarkerShape(MarkerShape s)
{
  GetImplementation(*this).SetMarkerShape(s);
}
ScatterSeries::MarkerShape ScatterSeries::GetMarkerShape() const
{
  return GetImplementation(*this).GetMarkerShape();
}

} // namespace Ui
} // namespace Dali
