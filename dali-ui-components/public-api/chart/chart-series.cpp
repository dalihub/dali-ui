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
#include <dali-ui-components/public-api/chart/chart-series.h>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>

namespace Dali
{
namespace Ui
{

ChartSeries::ChartSeries()
{
}

ChartSeries::~ChartSeries()
{
}

ChartSeries::ChartSeries(const ChartSeries& rhs)
: BaseHandle(rhs)
{
}

ChartSeries::ChartSeries(ChartSeries&& rhs)
: BaseHandle(std::move(rhs))
{
}

ChartSeries& ChartSeries::operator=(const ChartSeries& rhs)
{
  if(this != &rhs)
  {
    BaseHandle::operator=(rhs);
  }
  return *this;
}

ChartSeries& ChartSeries::operator=(ChartSeries&& rhs)
{
  if(this != &rhs)
  {
    BaseHandle::operator=(std::move(rhs));
  }
  return *this;
}

ChartSeries ChartSeries::DownCast(BaseHandle handle)
{
  return ChartSeries(dynamic_cast<Integration::ChartSeries*>(handle.GetObjectPtr()));
}

ChartSeries& ChartSeries::SetName(const Dali::String& name)
{
  GetImplementation(*this).SetName(name);
  return *this;
}

Dali::String ChartSeries::GetName() const
{
  return GetImplementation(*this).GetName();
}

ChartSeries& ChartSeries::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetColor(color);
  return *this;
}

Vector4 ChartSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

ChartSeries& ChartSeries::SetValues(const std::vector<float>& values)
{
  GetImplementation(*this).SetValues(values);
  return *this;
}

ChartSeries& ChartSeries::SetValues(const std::vector<std::pair<float, float>>& xyValues)
{
  GetImplementation(*this).SetValues(xyValues);
  return *this;
}

ChartSeries& ChartSeries::SetVisible(bool visible)
{
  GetImplementation(*this).SetVisible(visible);
  return *this;
}

bool ChartSeries::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

ChartSeries& ChartSeries::AppendValue(float y)
{
  GetImplementation(*this).AppendValue(y);
  return *this;
}

ChartSeries& ChartSeries::AppendValue(float x, float y)
{
  GetImplementation(*this).AppendValue(x, y);
  return *this;
}

ChartSeries& ChartSeries::AppendValues(const std::vector<float>& values)
{
  GetImplementation(*this).AppendValues(values);
  return *this;
}

ChartSeries& ChartSeries::SetMaxDataPoints(int maxCount)
{
  GetImplementation(*this).SetMaxDataPoints(maxCount);
  return *this;
}

int ChartSeries::GetMaxDataPoints() const
{
  return GetImplementation(*this).GetMaxDataPoints();
}

ChartSeries& ChartSeries::SetZIndex(int zIndex)
{
  GetImplementation(*this).SetZIndex(zIndex);
  return *this;
}

int ChartSeries::GetZIndex() const
{
  return GetImplementation(*this).GetZIndex();
}

ChartSeries::ChartSeries(Integration::ChartSeries* impl)
: BaseHandle(impl)
{
}

} // namespace Ui
} // namespace Dali
