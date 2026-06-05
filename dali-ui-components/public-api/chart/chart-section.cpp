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

ChartSection& ChartSection::SetXMin(float v)
{
  GetImplementation(*this).SetXMin(v);
  return *this;
}
float ChartSection::GetXMin() const
{
  return GetImplementation(*this).GetXMin();
}
ChartSection& ChartSection::SetXMax(float v)
{
  GetImplementation(*this).SetXMax(v);
  return *this;
}
float ChartSection::GetXMax() const
{
  return GetImplementation(*this).GetXMax();
}
ChartSection& ChartSection::SetYMin(float v)
{
  GetImplementation(*this).SetYMin(v);
  return *this;
}
float ChartSection::GetYMin() const
{
  return GetImplementation(*this).GetYMin();
}
ChartSection& ChartSection::SetYMax(float v)
{
  GetImplementation(*this).SetYMax(v);
  return *this;
}
float ChartSection::GetYMax() const
{
  return GetImplementation(*this).GetYMax();
}

ChartSection& ChartSection::SetFillColor(const Vector4& c)
{
  GetImplementation(*this).SetFillColor(c);
  return *this;
}
Vector4 ChartSection::GetFillColor() const
{
  return GetImplementation(*this).GetFillColor();
}
ChartSection& ChartSection::SetStrokeColor(const Vector4& c)
{
  GetImplementation(*this).SetStrokeColor(c);
  return *this;
}
Vector4 ChartSection::GetStrokeColor() const
{
  return GetImplementation(*this).GetStrokeColor();
}
ChartSection& ChartSection::SetStrokeWidth(float w)
{
  GetImplementation(*this).SetStrokeWidth(w);
  return *this;
}
float ChartSection::GetStrokeWidth() const
{
  return GetImplementation(*this).GetStrokeWidth();
}

} // namespace Ui
} // namespace Dali
