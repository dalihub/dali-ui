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

void ChartAxis::SetLabels(const std::vector<Dali::String>& labels)
{
  GetImplementation(*this).SetLabels(labels);
}

std::vector<Dali::String> ChartAxis::GetLabels() const
{
  return GetImplementation(*this).GetLabels();
}

void ChartAxis::SetMinLimit(float min)
{
  GetImplementation(*this).SetMinLimit(min);
}

float ChartAxis::GetMinLimit() const
{
  return GetImplementation(*this).GetMinLimit();
}

void ChartAxis::SetMaxLimit(float max)
{
  GetImplementation(*this).SetMaxLimit(max);
}

float ChartAxis::GetMaxLimit() const
{
  return GetImplementation(*this).GetMaxLimit();
}

void ChartAxis::SetAutoRange(bool autoRange)
{
  GetImplementation(*this).SetAutoRange(autoRange);
}

bool ChartAxis::IsAutoRange() const
{
  return GetImplementation(*this).GetAutoRange();
}

void ChartAxis::SetShowGridLines(bool show)
{
  GetImplementation(*this).SetShowGridLines(show);
}

bool ChartAxis::GetShowGridLines() const
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

void ChartAxis::SetMinStep(float step)
{
  GetImplementation(*this).SetMinStep(step);
}

float ChartAxis::GetMinStep() const
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
