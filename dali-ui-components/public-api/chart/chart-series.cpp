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

// EXTERNAL INCLUDES
#include <utility>
#include <vector>

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

void ChartSeries::SetName(const Dali::String& name)
{
  GetImplementation(*this).SetName(name);
}

Dali::String ChartSeries::GetName() const
{
  return GetImplementation(*this).GetName();
}

void ChartSeries::SetColor(const Vector4& color)
{
  GetImplementation(*this).SetColor(color);
}

Vector4 ChartSeries::GetColor() const
{
  return GetImplementation(*this).GetColor();
}

void ChartSeries::SetValues(const Dali::Vector<float>& values)
{
  GetImplementation(*this).SetValues(std::vector<float>(values.Begin(), values.End()));
}

void ChartSeries::SetValues(const Dali::Vector<Vector2>& xyValues)
{
  std::vector<std::pair<float, float>> points;
  points.reserve(xyValues.Count());
  for(const auto& xyValue : xyValues)
  {
    points.emplace_back(xyValue.x, xyValue.y);
  }
  GetImplementation(*this).SetValues(points);
}

void ChartSeries::SetVisible(bool visible)
{
  GetImplementation(*this).SetVisible(visible);
}

bool ChartSeries::IsVisible() const
{
  return GetImplementation(*this).IsVisible();
}

void ChartSeries::AppendValue(float y)
{
  GetImplementation(*this).AppendValue(y);
}

void ChartSeries::AppendValue(float x, float y)
{
  GetImplementation(*this).AppendValue(x, y);
}

void ChartSeries::AppendValues(const Dali::Vector<float>& values)
{
  GetImplementation(*this).AppendValues(std::vector<float>(values.Begin(), values.End()));
}

void ChartSeries::SetMaximumDataPoints(int maximumCount)
{
  GetImplementation(*this).SetMaxDataPoints(maximumCount);
}

int ChartSeries::GetMaximumDataPoints() const
{
  return GetImplementation(*this).GetMaxDataPoints();
}

void ChartSeries::SetZIndex(int zIndex)
{
  GetImplementation(*this).SetZIndex(zIndex);
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
