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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ChartSeries : public Dali::BaseObject
{
public:
  using DataChangedSignalType = Signal<void()>;

  ChartSeries();

  void         SetName(const Dali::String& name);
  Dali::String GetName() const;

  // Uniform series color. Derived classes may use this for lines, bars, or
  // scatter markers.  PieSeries uses it as the default new-slice colour.
  virtual void    SetColor(const Vector4& color);
  virtual Vector4 GetColor() const;

  void SetValues(const std::vector<float>& values);
  void SetValues(const std::vector<std::pair<float, float>>& xyValues);

  const std::vector<std::pair<float, float>>& GetValues() const;

  void SetVisible(bool visible);
  bool IsVisible() const
  {
    return mVisible;
  }

  void AppendValue(float y);
  void AppendValue(float x, float y);
  void AppendValues(const std::vector<float>& values);

  void SetMaxDataPoints(int maxCount);
  int  GetMaxDataPoints() const
  {
    return mMaxDataPoints;
  }

  void SetZIndex(int zIndex)
  {
    mZIndex = zIndex;
  }
  int GetZIndex() const
  {
    return mZIndex;
  }

  DataChangedSignalType& DataChangedSignal();

protected:
  ~ChartSeries() override;

  void EmitDataChangedSignal();

private:
  void TrimToMaxDataPoints();

  DataChangedSignalType                mDataChangedSignal;
  std::vector<std::pair<float, float>> mValues;
  Dali::String                         mName;
  Vector4                              mColor{0.3f, 0.6f, 1.0f, 1.0f};
  int                                  mMaxDataPoints{0};
  int                                  mZIndex{0};
  bool                                 mVisible{true};
};

} // namespace Integration

inline Integration::ChartSeries& GetImplementation(Ui::ChartSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartSeries handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Integration::ChartSeries&>(object);
}

inline const Integration::ChartSeries& GetImplementation(const Ui::ChartSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartSeries handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Integration::ChartSeries&>(object);
}

} // namespace Ui
} // namespace Dali
