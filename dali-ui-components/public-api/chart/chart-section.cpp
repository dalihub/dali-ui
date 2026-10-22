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

#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-section-impl.h>

namespace DALI_NAMESPACE
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

void ChartSection::SetMinimumX(float v)
{
  GetImplementation(*this).SetXMin(v);
  GetImplementation(*this).mChanged.Emit();
}

float ChartSection::GetMinimumX() const
{
  return GetImplementation(*this).GetXMin();
}

void ChartSection::SetMaximumX(float v)
{
  GetImplementation(*this).SetXMax(v);
  GetImplementation(*this).mChanged.Emit();
}

float ChartSection::GetMaximumX() const
{
  return GetImplementation(*this).GetXMax();
}

void ChartSection::SetMinimumY(float v)
{
  GetImplementation(*this).SetYMin(v);
  GetImplementation(*this).mChanged.Emit();
}

float ChartSection::GetMinimumY() const
{
  return GetImplementation(*this).GetYMin();
}

void ChartSection::SetMaximumY(float v)
{
  GetImplementation(*this).SetYMax(v);
  GetImplementation(*this).mChanged.Emit();
}

float ChartSection::GetMaximumY() const
{
  return GetImplementation(*this).GetYMax();
}

void ChartSection::SetFillColor(const Vector4& c)
{
  GetImplementation(*this).SetFillColor(c);
  GetImplementation(*this).mChanged.Emit();
}

Vector4 ChartSection::GetFillColor() const
{
  return GetImplementation(*this).GetFillColor();
}

void ChartSection::SetStrokeColor(const Vector4& c)
{
  GetImplementation(*this).SetStrokeColor(c);
  GetImplementation(*this).mChanged.Emit();
}

Vector4 ChartSection::GetStrokeColor() const
{
  return GetImplementation(*this).GetStrokeColor();
}

void ChartSection::SetStrokeWidth(float w)
{
  GetImplementation(*this).SetStrokeWidth(w);
  GetImplementation(*this).mChanged.Emit();
}

float ChartSection::GetStrokeWidth() const
{
  return GetImplementation(*this).GetStrokeWidth();
}

void ChartSection::SetCornerRadius(float radius)
{
  DALI_ASSERT_ALWAYS(std::isfinite(radius) && radius >= 0.0f);
  auto& impl        = GetImplementation(*this);
  impl.cornerRadius = radius;
  impl.mChanged.Emit();
}

float ChartSection::GetCornerRadius() const
{
  return GetImplementation(*this).cornerRadius;
}

void ChartSection::SetPatternEnabled(bool enabled)
{
  auto& impl          = GetImplementation(*this);
  impl.patternEnabled = enabled;
  impl.mChanged.Emit();
}

bool ChartSection::IsPatternEnabled() const
{
  return GetImplementation(*this).patternEnabled;
}

} // namespace Ui
} //namespace DALI_NAMESPACE
