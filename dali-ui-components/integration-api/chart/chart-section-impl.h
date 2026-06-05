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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-object.h>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-section.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ChartSection : public Dali::BaseObject
{
public:
  ChartSection();

  void SetXMin(float v)
  {
    xMin = v;
  }
  float GetXMin() const
  {
    return xMin;
  }
  void SetXMax(float v)
  {
    xMax = v;
  }
  float GetXMax() const
  {
    return xMax;
  }

  void SetYMin(float v)
  {
    yMin = v;
  }
  float GetYMin() const
  {
    return yMin;
  }
  void SetYMax(float v)
  {
    yMax = v;
  }
  float GetYMax() const
  {
    return yMax;
  }

  void SetFillColor(const Vector4& c)
  {
    fillColor = c;
  }
  Vector4 GetFillColor() const
  {
    return fillColor;
  }
  void SetStrokeColor(const Vector4& c)
  {
    strokeColor = c;
  }
  Vector4 GetStrokeColor() const
  {
    return strokeColor;
  }
  void SetStrokeWidth(float w)
  {
    strokeWidth = w;
  }
  float GetStrokeWidth() const
  {
    return strokeWidth;
  }

public:
  Vector4 fillColor{0.3f, 0.6f, 1.0f, 0.15f};
  Vector4 strokeColor{0.3f, 0.6f, 1.0f, 0.8f};
  float   xMin{std::numeric_limits<float>::quiet_NaN()};
  float   xMax{std::numeric_limits<float>::quiet_NaN()};
  float   yMin{std::numeric_limits<float>::quiet_NaN()};
  float   yMax{std::numeric_limits<float>::quiet_NaN()};
  float   strokeWidth{0.0f};
};

} // namespace Integration

inline Integration::ChartSection& GetImplementation(Ui::ChartSection& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartSection handle is empty");
  return static_cast<Integration::ChartSection&>(handle.GetBaseObject());
}

inline const Integration::ChartSection& GetImplementation(const Ui::ChartSection& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartSection handle is empty");
  return static_cast<const Integration::ChartSection&>(handle.GetBaseObject());
}

} // namespace Ui
} // namespace Dali
