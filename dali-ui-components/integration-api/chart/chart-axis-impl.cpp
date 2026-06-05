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
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

ChartAxis::ChartAxis()
: mTitle(),
  mMinLimit(0.0f),
  mMaxLimit(1.0f),
  mLabelSize(11.0f),
  mAutoRange(true),
  mShowGridLines(true),
  mAxisLineColor(0.2f, 0.2f, 0.2f, 1.0f),
  mAxisLineWidth(2.0f),
  mGridDashLength(5.0f),
  mGridGapLength(4.0f),
  mGridColor(0.8f, 0.8f, 0.8f, 1.0f)
{
}

ChartAxis::~ChartAxis()
{
}

void ChartAxis::SetLabels(const std::vector<Dali::String>& labels)
{
  mLabels = labels;
  EmitConfigChangedSignal();
}

std::vector<Dali::String> ChartAxis::GetLabels() const
{
  return mLabels;
}

void ChartAxis::SetMinLimit(float min)
{
  if(mMinLimit != min)
  {
    mMinLimit  = min;
    mAutoRange = false;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetMaxLimit(float max)
{
  if(mMaxLimit != max)
  {
    mMaxLimit  = max;
    mAutoRange = false;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetAutoRange(bool autoRange)
{
  if(mAutoRange != autoRange)
  {
    mAutoRange = autoRange;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetShowGridLines(bool show)
{
  if(mShowGridLines != show)
  {
    mShowGridLines = show;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetLabelSize(float size)
{
  if(mLabelSize != size)
  {
    mLabelSize = size;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetTitle(const Dali::String& title)
{
  if(mTitle != title)
  {
    mTitle = title;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetAxisLineColor(const Vector4& color)
{
  if(mAxisLineColor != color)
  {
    mAxisLineColor = color;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetAxisLineWidth(float width)
{
  if(mAxisLineWidth != width)
  {
    mAxisLineWidth = width;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetGridDash(float dashLength, float gapLength)
{
  if(mGridDashLength != dashLength || mGridGapLength != gapLength)
  {
    mGridDashLength = dashLength;
    mGridGapLength  = gapLength;
    EmitConfigChangedSignal();
  }
}

void ChartAxis::SetGridColor(const Vector4& color)
{
  if(mGridColor != color)
  {
    mGridColor = color;
    EmitConfigChangedSignal();
  }
}

ChartAxis::ConfigChangedSignalType& ChartAxis::ConfigChangedSignal()
{
  return mConfigChangedSignal;
}

void ChartAxis::EmitConfigChangedSignal()
{
  if(!mConfigChangedSignal.Empty())
  {
    mConfigChangedSignal.Emit();
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
