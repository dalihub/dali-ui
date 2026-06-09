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
#include <dali-ui-components/public-api/chart/chart-section.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-section-impl.h>

namespace Dali
{
namespace Ui
{

ChartSection ChartSection::New()
{
  Integration::ChartSection* impl = new Integration::ChartSection();
  return ChartSection(impl);
}

ChartSection::ChartSection()  = default;
ChartSection::~ChartSection() = default;

ChartSection::ChartSection(Integration::ChartSection* impl)
: BaseHandle(impl)
{
}

void ChartSection::SetXMin(float v)
{
  GetImplementation(*this).SetXMin(v);
}
float ChartSection::GetXMin() const
{
  return GetImplementation(*this).GetXMin();
}
void ChartSection::SetXMax(float v)
{
  GetImplementation(*this).SetXMax(v);
}
float ChartSection::GetXMax() const
{
  return GetImplementation(*this).GetXMax();
}
void ChartSection::SetYMin(float v)
{
  GetImplementation(*this).SetYMin(v);
}
float ChartSection::GetYMin() const
{
  return GetImplementation(*this).GetYMin();
}
void ChartSection::SetYMax(float v)
{
  GetImplementation(*this).SetYMax(v);
}
float ChartSection::GetYMax() const
{
  return GetImplementation(*this).GetYMax();
}

void ChartSection::SetFillColor(const Vector4& c)
{
  GetImplementation(*this).SetFillColor(c);
}
Vector4 ChartSection::GetFillColor() const
{
  return GetImplementation(*this).GetFillColor();
}
void ChartSection::SetStrokeColor(const Vector4& c)
{
  GetImplementation(*this).SetStrokeColor(c);
}
Vector4 ChartSection::GetStrokeColor() const
{
  return GetImplementation(*this).GetStrokeColor();
}
void ChartSection::SetStrokeWidth(float w)
{
  GetImplementation(*this).SetStrokeWidth(w);
}
float ChartSection::GetStrokeWidth() const
{
  return GetImplementation(*this).GetStrokeWidth();
}

} // namespace Ui
} // namespace Dali
