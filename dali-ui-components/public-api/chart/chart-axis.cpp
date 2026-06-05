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

ChartAxis& ChartAxis::SetLabels(const std::vector<Dali::String>& labels)
{
  GetImplementation(*this).SetLabels(labels);
  return *this;
}

std::vector<Dali::String> ChartAxis::GetLabels() const
{
  return GetImplementation(*this).GetLabels();
}

ChartAxis& ChartAxis::SetMinLimit(float min)
{
  GetImplementation(*this).SetMinLimit(min);
  return *this;
}

float ChartAxis::GetMinLimit() const
{
  return GetImplementation(*this).GetMinLimit();
}

ChartAxis& ChartAxis::SetMaxLimit(float max)
{
  GetImplementation(*this).SetMaxLimit(max);
  return *this;
}

float ChartAxis::GetMaxLimit() const
{
  return GetImplementation(*this).GetMaxLimit();
}

ChartAxis& ChartAxis::SetAutoRange(bool autoRange)
{
  GetImplementation(*this).SetAutoRange(autoRange);
  return *this;
}

bool ChartAxis::IsAutoRange() const
{
  return GetImplementation(*this).GetAutoRange();
}

ChartAxis& ChartAxis::SetShowGridLines(bool show)
{
  GetImplementation(*this).SetShowGridLines(show);
  return *this;
}

bool ChartAxis::GetShowGridLines() const
{
  return GetImplementation(*this).GetShowGridLines();
}

ChartAxis& ChartAxis::SetLabelSize(float size)
{
  GetImplementation(*this).SetLabelSize(size);
  return *this;
}

float ChartAxis::GetLabelSize() const
{
  return GetImplementation(*this).GetLabelSize();
}

ChartAxis& ChartAxis::SetTitle(const Dali::String& title)
{
  GetImplementation(*this).SetTitle(title);
  return *this;
}

Dali::String ChartAxis::GetTitle() const
{
  return GetImplementation(*this).GetTitle();
}

ChartAxis& ChartAxis::SetAxisLineColor(const Vector4& color)
{
  GetImplementation(*this).SetAxisLineColor(color);
  return *this;
}

Vector4 ChartAxis::GetAxisLineColor() const
{
  return GetImplementation(*this).GetAxisLineColor();
}

ChartAxis& ChartAxis::SetAxisLineWidth(float width)
{
  GetImplementation(*this).SetAxisLineWidth(width);
  return *this;
}

float ChartAxis::GetAxisLineWidth() const
{
  return GetImplementation(*this).GetAxisLineWidth();
}

ChartAxis& ChartAxis::SetGridDash(float dashLength, float gapLength)
{
  GetImplementation(*this).SetGridDash(dashLength, gapLength);
  return *this;
}

void ChartAxis::GetGridDash(float& dashLength, float& gapLength) const
{
  GetImplementation(*this).GetGridDash(dashLength, gapLength);
}

ChartAxis& ChartAxis::SetGridColor(const Vector4& color)
{
  GetImplementation(*this).SetGridColor(color);
  return *this;
}

Vector4 ChartAxis::GetGridColor() const
{
  return GetImplementation(*this).GetGridColor();
}

ChartAxis& ChartAxis::SetMinStep(float step)
{
  GetImplementation(*this).SetMinStep(step);
  return *this;
}

float ChartAxis::GetMinStep() const
{
  return GetImplementation(*this).GetMinStep();
}

ChartAxis& ChartAxis::SetDataPadding(float fraction)
{
  GetImplementation(*this).SetDataPadding(fraction);
  return *this;
}

float ChartAxis::GetDataPadding() const
{
  return GetImplementation(*this).GetDataPadding();
}

ChartAxis& ChartAxis::SetLabelsRotation(float degrees)
{
  GetImplementation(*this).SetLabelsRotation(degrees);
  return *this;
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
