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

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/bar-series-impl.h>

namespace DALI_NAMESPACE
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
  GetImplementation(*this).NotifyChanged();
}

Vector4 BarSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

void BarSeries::SetBarGroupWidth(float r)
{
  GetImplementation(*this).SetBarGroupWidth(r);
  GetImplementation(*this).NotifyChanged();
}

float BarSeries::GetBarGroupWidth() const
{
  return GetImplementation(*this).GetBarGroupWidth();
}

void BarSeries::SetStacked(bool s)
{
  DALI_ASSERT_ALWAYS(!s || !GetImplementation(*this).HasRangeValues());
  GetImplementation(*this).SetStacked(s);
  GetImplementation(*this).NotifyChanged();
}

bool BarSeries::IsStacked() const
{
  return GetImplementation(*this).IsStacked();
}

void BarSeries::SetDataLabelsVisible(bool v)
{
  GetImplementation(*this).SetDataLabelsVisible(v);
  GetImplementation(*this).NotifyChanged();
}

bool BarSeries::IsDataLabelsVisible() const
{
  return GetImplementation(*this).IsDataLabelsVisible();
}

void BarSeries::SetDataLabelFormat(const Dali::String& f)
{
  GetImplementation(*this).SetDataLabelFormat(f);
  GetImplementation(*this).NotifyChanged();
}

Dali::String BarSeries::GetDataLabelFormat() const
{
  return GetImplementation(*this).GetDataLabelFormat();
}

void BarSeries::SetDataLabelColor(const Vector4& c)
{
  GetImplementation(*this).SetDataLabelColor(c);
  GetImplementation(*this).NotifyChanged();
}

Vector4 BarSeries::GetDataLabelColor() const
{
  return GetImplementation(*this).GetDataLabelColor();
}

void BarSeries::SetDataLabelSize(float s)
{
  GetImplementation(*this).SetDataLabelSize(s);
  GetImplementation(*this).NotifyChanged();
}

float BarSeries::GetDataLabelSize() const
{
  return GetImplementation(*this).GetDataLabelSize();
}

void BarSeries::SetCornerRadius(float radius)
{
  DALI_ASSERT_ALWAYS(std::isfinite(radius) && radius >= 0.0f);
  auto& impl         = GetImplementation(*this);
  impl.mCornerRadius = radius;
  impl.NotifyChanged();
}

float BarSeries::GetCornerRadius() const
{
  return GetImplementation(*this).mCornerRadius;
}

void BarSeries::SetFillGradient(const Gradient::Linear& gradient)
{
  auto& impl             = GetImplementation(*this);
  impl.mFillGradient     = gradient;
  impl.mHasFillGradient  = gradient.GetType() == Gradient::Type::LINEAR;
  impl.mHasFillThreshold = false;
  impl.NotifyChanged();
}

void BarSeries::ClearFillGradient()
{
  auto& impl             = GetImplementation(*this);
  impl.mHasFillGradient  = false;
  impl.mHasFillThreshold = false;
  impl.NotifyChanged();
}

void BarSeries::SetRangeValues(const Dali::Vector<Vector3>& values)
{
  auto& impl = GetImplementation(*this);
  DALI_ASSERT_ALWAYS(!impl.IsStacked());
  for(const auto& value : values)
  {
    DALI_ASSERT_ALWAYS(!std::isfinite(value.y) || !std::isfinite(value.z) || value.y <= value.z);
  }
  impl.SetRangeValues(std::vector<Vector3>(values.Begin(), values.End()));
}

void BarSeries::SetBorder(float width, const UiColor& color)
{
  DALI_ASSERT_ALWAYS(std::isfinite(width) && width >= 0.0f);
  auto& impl        = GetImplementation(*this);
  impl.mBorderWidth = width;
  impl.mBorderColor = color;
  impl.NotifyChanged();
}

void BarSeries::SetBorderDash(float dash, float gap)
{
  DALI_ASSERT_ALWAYS(std::isfinite(dash) && std::isfinite(gap) && dash >= 0.0f && gap >= 0.0f);
  auto& impl       = GetImplementation(*this);
  impl.mBorderDash = Vector2(dash, gap);
  impl.NotifyChanged();
}

Vector2 BarSeries::GetBorderDash() const
{
  return GetImplementation(*this).mBorderDash;
}

void BarSeries::SetFillThreshold(float value, const UiColor& lowerColor, const UiColor& upperColor)
{
  DALI_ASSERT_ALWAYS(std::isfinite(value));
  auto& impl             = GetImplementation(*this);
  impl.mHasFillThreshold = true;
  impl.mHasFillGradient  = false;
  impl.mFillThreshold    = value;
  impl.mLowerFillColor   = lowerColor;
  impl.mUpperFillColor   = upperColor;
  impl.NotifyChanged();
}

void BarSeries::ClearFillThreshold()
{
  auto& impl             = GetImplementation(*this);
  impl.mHasFillThreshold = false;
  impl.NotifyChanged();
}

void BarSeries::SetPointBorder(uint32_t pointIndex, float width, const UiColor& color, float dash, float gap)
{
  DALI_ASSERT_ALWAYS(std::isfinite(width) && width >= 0.0f);
  DALI_ASSERT_ALWAYS(std::isfinite(dash) && std::isfinite(gap) && dash >= 0.0f && gap >= 0.0f);
  auto& impl                     = GetImplementation(*this);
  impl.mPointBorders[pointIndex] = {color, Vector2(dash, gap), width};
  impl.NotifyChanged();
}

void BarSeries::ClearPointBorder(uint32_t pointIndex)
{
  auto& impl = GetImplementation(*this);
  if(impl.mPointBorders.erase(pointIndex) > 0u)
  {
    impl.NotifyChanged();
  }
}

void BarSeries::ClearPointBorders()
{
  auto& impl = GetImplementation(*this);
  if(!impl.mPointBorders.empty())
  {
    impl.mPointBorders.clear();
    impl.NotifyChanged();
  }
}

} // namespace Ui
} //namespace DALI_NAMESPACE
