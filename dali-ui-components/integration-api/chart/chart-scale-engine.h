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
#include <limits>
#include <vector>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

class ScaleEngine
{
public:
  void SetLogarithmic(bool x, bool y)
  {
    mLogX = x;
    mLogY = y;
  }

  bool IsValid(float x, float y) const
  {
    return std::isfinite(x) && std::isfinite(y) && (!mLogX || x > 0.0f) && (!mLogY || y > 0.0f);
  }

  bool IsLogY() const
  {
    return mLogY;
  }

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
    float range = Map(mXMax, mLogX) - Map(mXMin, mLogX);
    if(std::abs(range) < 1e-6f)
    {
      return mPlotArea.x + mPlotArea.width * 0.5f;
    }
    return mPlotArea.x + (Map(dataX, mLogX) - Map(mXMin, mLogX)) / range * mPlotArea.width;
  }

  float ToCanvasY(float dataY) const
  {
    float range = Map(mYMax, mLogY) - Map(mYMin, mLogY);
    if(std::abs(range) < 1e-6f)
    {
      return mPlotArea.y + mPlotArea.height * 0.5f;
    }
    return mPlotArea.y + mPlotArea.height - (Map(dataY, mLogY) - Map(mYMin, mLogY)) / range * mPlotArea.height;
  }

  Vector2 ToCanvas(float dataX, float dataY) const
  {
    return Vector2(ToCanvasX(dataX), ToCanvasY(dataY));
  }

  float ToDataX(float canvasX) const
  {
    float range = Map(mXMax, mLogX) - Map(mXMin, mLogX);
    if(mPlotArea.width < 1e-6f)
    {
      return mXMin;
    }
    return Unmap(Map(mXMin, mLogX) + (canvasX - mPlotArea.x) / mPlotArea.width * range, mLogX);
  }

  float ToDataY(float canvasY) const
  {
    float range = Map(mYMax, mLogY) - Map(mYMin, mLogY);
    if(mPlotArea.height < 1e-6f)
    {
      return mYMin;
    }
    return Unmap(Map(mYMin, mLogY) + (mPlotArea.y + mPlotArea.height - canvasY) / mPlotArea.height * range, mLogY);
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
    {
      niceStep = magnitude;
    }
    else if(fraction <= 2.0f)
    {
      niceStep = 2.0f * magnitude;
    }
    else if(fraction <= 5.0f)
    {
      niceStep = 5.0f * magnitude;
    }
    else
    {
      niceStep = 10.0f * magnitude;
    }

    if(minStep > 0.0f && niceStep < minStep)
    {
      niceStep = minStep;
    }

    float niceMin = std::floor(min / niceStep) * niceStep;
    float niceMax = std::ceil(max / niceStep) * niceStep;

    for(float v = niceMin; v <= niceMax + niceStep * 0.5f; v += niceStep)
    {
      ticks.push_back(v);
    }

    return ticks;
  }

  static std::vector<float> ComputeLogTicks(float min, float max, int count = 5)
  {
    std::vector<float> ticks;
    if(min <= 0.0f || max <= min || count < 1)
    {
      return ticks;
    }
    const float from = std::log10(min);
    const float to   = std::log10(max);
    for(int i = 0; i <= count; ++i)
    {
      ticks.push_back(std::pow(10.0f, from + (to - from) * static_cast<float>(i) / count));
    }
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
  static float Map(float value, bool logarithmic)
  {
    return logarithmic ? std::log10(std::max(value, std::numeric_limits<float>::min())) : value;
  }

  static float Unmap(float value, bool logarithmic)
  {
    return logarithmic ? std::pow(10.0f, value) : value;
  }

  Rect<float> mPlotArea{0.0f, 0.0f, 100.0f, 100.0f};
  float       mXMin{0.0f};
  float       mXMax{1.0f};
  float       mYMin{0.0f};
  float       mYMax{1.0f};
  bool        mLogX{false};
  bool        mLogY{false};
};

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
