#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector2.h>
#include <cmath>
#include <vector>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ScaleEngine
{
public:
  void SetPlotArea(const Rect<float>& area)
  {
    mPlotArea = area;
  }

  void SetDataRange(float xMin, float xMax, float yMin, float yMax)
  {
    mXMin = xMin;
    mXMax = xMax;
    mYMin = yMin;
    mYMax = yMax;
  }

  float ToCanvasX(float dataX) const
  {
    float range = mXMax - mXMin;
    if(std::abs(range) < 1e-6f)
    {
      return mPlotArea.x + mPlotArea.width * 0.5f;
    }
    return mPlotArea.x + (dataX - mXMin) / range * mPlotArea.width;
  }

  float ToCanvasY(float dataY) const
  {
    float range = mYMax - mYMin;
    if(std::abs(range) < 1e-6f)
    {
      return mPlotArea.y + mPlotArea.height * 0.5f;
    }
    return mPlotArea.y + mPlotArea.height - (dataY - mYMin) / range * mPlotArea.height;
  }

  Vector2 ToCanvas(float dataX, float dataY) const
  {
    return Vector2(ToCanvasX(dataX), ToCanvasY(dataY));
  }

  float ToDataX(float canvasX) const
  {
    float range = mXMax - mXMin;
    if(mPlotArea.width < 1e-6f)
    {
      return mXMin;
    }
    return mXMin + (canvasX - mPlotArea.x) / mPlotArea.width * range;
  }

  float ToDataY(float canvasY) const
  {
    float range = mYMax - mYMin;
    if(mPlotArea.height < 1e-6f)
    {
      return mYMin;
    }
    return mYMin + (mPlotArea.y + mPlotArea.height - canvasY) / mPlotArea.height * range;
  }

  static std::vector<float> ComputeNiceTicks(float min, float max, int targetCount,
                                             float minStep = 0.0f)
  {
    std::vector<float> ticks;

    if(targetCount <= 0 || max <= min)
    {
      ticks.push_back(min);
      ticks.push_back(max);
      return ticks;
    }

    float range     = max - min;
    float roughStep = range / static_cast<float>(targetCount);

    float magnitude = std::pow(10.0f, std::floor(std::log10(roughStep)));
    float fraction  = roughStep / magnitude;

    float niceStep;
    if(fraction <= 1.0f)
      niceStep = magnitude;
    else if(fraction <= 2.0f)
      niceStep = 2.0f * magnitude;
    else if(fraction <= 5.0f)
      niceStep = 5.0f * magnitude;
    else
      niceStep = 10.0f * magnitude;

    if(minStep > 0.0f && niceStep < minStep)
      niceStep = minStep;

    float niceMin = std::floor(min / niceStep) * niceStep;
    float niceMax = std::ceil(max / niceStep) * niceStep;

    for(float v = niceMin; v <= niceMax + niceStep * 0.5f; v += niceStep)
      ticks.push_back(v);

    return ticks;
  }

  const Rect<float>& GetPlotArea() const
  {
    return mPlotArea;
  }

  float GetXMin() const
  {
    return mXMin;
  }
  float GetXMax() const
  {
    return mXMax;
  }
  float GetYMin() const
  {
    return mYMin;
  }
  float GetYMax() const
  {
    return mYMax;
  }

private:
  Rect<float> mPlotArea{0.0f, 0.0f, 100.0f, 100.0f};
  float       mXMin{0.0f};
  float       mXMax{1.0f};
  float       mYMin{0.0f};
  float       mYMax{1.0f};
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
