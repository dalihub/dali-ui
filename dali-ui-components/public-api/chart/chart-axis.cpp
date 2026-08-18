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
#include <dali-ui-components/public-api/chart/chart-axis.h>

// EXTERNAL INCLUDES
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>

namespace Dali
{
namespace Ui
{

ChartAxis::ChartAxis()
{
}

ChartAxis::~ChartAxis()
{
}

ChartAxis ChartAxis::New()
{
  Integration::ChartAxis* impl = new Integration::ChartAxis();
  return ChartAxis(impl);
}

ChartAxis::ChartAxis(const ChartAxis& rhs)
: BaseHandle(rhs)
{
}

ChartAxis::ChartAxis(ChartAxis&& rhs)
: BaseHandle(std::move(rhs))
{
}

ChartAxis& ChartAxis::operator=(const ChartAxis& rhs)
{
  if(this != &rhs)
  {
    BaseHandle::operator=(rhs);
  }
  return *this;
}

ChartAxis& ChartAxis::operator=(ChartAxis&& rhs)
{
  if(this != &rhs)
  {
    BaseHandle::operator=(std::move(rhs));
  }
  return *this;
}

ChartAxis ChartAxis::DownCast(BaseHandle handle)
{
  return ChartAxis(dynamic_cast<Integration::ChartAxis*>(handle.GetObjectPtr()));
}

void ChartAxis::SetLabels(const Dali::Vector<Dali::String>& labels)
{
  GetImplementation(*this).SetLabels(std::vector<Dali::String>(labels.Begin(), labels.End()));
}

Dali::Vector<Dali::String> ChartAxis::GetLabels() const
{
  const std::vector<Dali::String> labels = GetImplementation(*this).GetLabels();

  Dali::Vector<Dali::String> result;
  result.Reserve(labels.size());
  for(const auto& label : labels)
  {
    result.PushBack(label);
  }
  return result;
}

void ChartAxis::SetMinimumLimit(float min)
{
  GetImplementation(*this).SetMinLimit(min);
}

float ChartAxis::GetMinimumLimit() const
{
  return GetImplementation(*this).GetMinLimit();
}

void ChartAxis::SetMaximumLimit(float max)
{
  GetImplementation(*this).SetMaxLimit(max);
}

float ChartAxis::GetMaximumLimit() const
{
  return GetImplementation(*this).GetMaxLimit();
}

void ChartAxis::SetAutoRangeEnabled(bool enabled)
{
  GetImplementation(*this).SetAutoRange(enabled);
}

bool ChartAxis::IsAutoRangeEnabled() const
{
  return GetImplementation(*this).GetAutoRange();
}

void ChartAxis::SetShowGridLinesEnabled(bool enabled)
{
  GetImplementation(*this).SetShowGridLines(enabled);
}

bool ChartAxis::IsShowGridLinesEnabled() const
{
  return GetImplementation(*this).GetShowGridLines();
}

void ChartAxis::SetLabelSize(float size)
{
  GetImplementation(*this).SetLabelSize(size);
}

float ChartAxis::GetLabelSize() const
{
  return GetImplementation(*this).GetLabelSize();
}

void ChartAxis::SetTitle(const Dali::String& title)
{
  GetImplementation(*this).SetTitle(title);
}

Dali::String ChartAxis::GetTitle() const
{
  return GetImplementation(*this).GetTitle();
}

void ChartAxis::SetAxisLineColor(const Vector4& color)
{
  GetImplementation(*this).SetAxisLineColor(color);
}

Vector4 ChartAxis::GetAxisLineColor() const
{
  return GetImplementation(*this).GetAxisLineColor();
}

void ChartAxis::SetAxisLineWidth(float width)
{
  GetImplementation(*this).SetAxisLineWidth(width);
}

float ChartAxis::GetAxisLineWidth() const
{
  return GetImplementation(*this).GetAxisLineWidth();
}

void ChartAxis::SetGridDash(float dashLength, float gapLength)
{
  GetImplementation(*this).SetGridDash(dashLength, gapLength);
}

void ChartAxis::GetGridDash(float& dashLength, float& gapLength) const
{
  GetImplementation(*this).GetGridDash(dashLength, gapLength);
}

void ChartAxis::SetGridColor(const Vector4& color)
{
  GetImplementation(*this).SetGridColor(color);
}

Vector4 ChartAxis::GetGridColor() const
{
  return GetImplementation(*this).GetGridColor();
}

void ChartAxis::SetMinimumStep(float step)
{
  GetImplementation(*this).SetMinStep(step);
}

float ChartAxis::GetMinimumStep() const
{
  return GetImplementation(*this).GetMinStep();
}

void ChartAxis::SetDataPadding(float fraction)
{
  GetImplementation(*this).SetDataPadding(fraction);
}

float ChartAxis::GetDataPadding() const
{
  return GetImplementation(*this).GetDataPadding();
}

void ChartAxis::SetLabelsRotation(float degrees)
{
  GetImplementation(*this).SetLabelsRotation(degrees);
}

float ChartAxis::GetLabelsRotation() const
{
  return GetImplementation(*this).GetLabelsRotation();
}

ChartAxis::ChartAxis(Integration::ChartAxis* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace Dali
