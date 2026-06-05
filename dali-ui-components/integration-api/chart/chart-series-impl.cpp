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
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

ChartSeries::ChartSeries()
: mName()
{
}

ChartSeries::~ChartSeries()
{
}

void ChartSeries::SetName(const Dali::String& name)
{
  if(mName != name)
  {
    mName = name;
    EmitDataChangedSignal();
  }
}

Dali::String ChartSeries::GetName() const
{
  return mName;
}

void ChartSeries::SetColor(const Vector4& color)
{
  mColor = color;
  EmitDataChangedSignal();
}

Vector4 ChartSeries::GetColor() const
{
  return mColor;
}

void ChartSeries::SetValues(const std::vector<float>& values)
{
  mValues.clear();
  mValues.reserve(values.size());
  for(size_t i = 0; i < values.size(); ++i)
  {
    mValues.push_back({static_cast<float>(i), values[i]});
  }
  EmitDataChangedSignal();
}

void ChartSeries::SetValues(const std::vector<std::pair<float, float>>& xyValues)
{
  mValues = xyValues;
  EmitDataChangedSignal();
}

const std::vector<std::pair<float, float>>& ChartSeries::GetValues() const
{
  return mValues;
}

void ChartSeries::SetVisible(bool visible)
{
  if(mVisible != visible)
  {
    mVisible = visible;
    EmitDataChangedSignal();
  }
}

void ChartSeries::AppendValue(float y)
{
  float nextX = mValues.empty() ? 0.0f : mValues.back().first + 1.0f;
  mValues.push_back({nextX, y});
  TrimToMaxDataPoints();
  EmitDataChangedSignal();
}

void ChartSeries::AppendValue(float x, float y)
{
  mValues.push_back({x, y});
  TrimToMaxDataPoints();
  EmitDataChangedSignal();
}

void ChartSeries::AppendValues(const std::vector<float>& values)
{
  float nextX = mValues.empty() ? 0.0f : mValues.back().first + 1.0f;
  mValues.reserve(mValues.size() + values.size());
  for(float v : values)
  {
    mValues.push_back({nextX++, v});
  }
  TrimToMaxDataPoints();
  EmitDataChangedSignal();
}

void ChartSeries::SetMaxDataPoints(int maxCount)
{
  mMaxDataPoints = (maxCount < 0) ? 0 : maxCount;
  TrimToMaxDataPoints();
  if(mMaxDataPoints > 0)
  {
    EmitDataChangedSignal();
  }
}

void ChartSeries::TrimToMaxDataPoints()
{
  if(mMaxDataPoints > 0 && static_cast<int>(mValues.size()) > mMaxDataPoints)
  {
    mValues.erase(mValues.begin(),
                  mValues.begin() + (static_cast<int>(mValues.size()) - mMaxDataPoints));
  }
}

ChartSeries::DataChangedSignalType& ChartSeries::DataChangedSignal()
{
  return mDataChangedSignal;
}

void ChartSeries::EmitDataChangedSignal()
{
  if(!mDataChangedSignal.Empty())
  {
    mDataChangedSignal.Emit();
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
