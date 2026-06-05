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
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/public-api/chart/scatter-series.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ScatterSeries : public ChartSeries
{
public:
  ScatterSeries();

  Vector4 GetColor() const
  {
    return mColor;
  }
  void SetColor(const Vector4& c)
  {
    mColor = c;
  }

  float GetMarkerRadius() const
  {
    return mMarkerRadius;
  }
  void SetMarkerRadius(float r)
  {
    mMarkerRadius = std::max(1.0f, r);
  }

  Ui::ScatterSeries::MarkerShape GetMarkerShape() const
  {
    return mShape;
  }
  void SetMarkerShape(Ui::ScatterSeries::MarkerShape s)
  {
    mShape = s;
  }

protected:
  ~ScatterSeries() override = default;

private:
  Vector4                        mColor{0.8f, 0.3f, 0.3f, 1.0f};
  float                          mMarkerRadius{6.0f};
  Ui::ScatterSeries::MarkerShape mShape{Ui::ScatterSeries::MarkerShape::CIRCLE};
};

} // namespace Integration

inline Integration::ScatterSeries& GetImplementation(Ui::ScatterSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ScatterSeries handle is empty");
  return static_cast<Integration::ScatterSeries&>(handle.GetBaseObject());
}

inline const Integration::ScatterSeries& GetImplementation(const Ui::ScatterSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ScatterSeries handle is empty");
  return static_cast<const Integration::ScatterSeries&>(handle.GetBaseObject());
}

} // namespace Ui
} // namespace Dali
