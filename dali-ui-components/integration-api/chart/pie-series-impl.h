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
#include <algorithm>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/public-api/chart/pie-series.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

struct SliceInfo
{
  Dali::String label;
  float        value{0.0f};
  Vector4      color{1.0f, 1.0f, 1.0f, 1.0f};
};

class PieSeries : public ChartSeries
{
public:
  PieSeries();

  void AddSlice(const Dali::String& label, float value, const Vector4& color);
  void ClearSlices();
  int  GetSliceCount() const
  {
    return static_cast<int>(mSlices.size());
  }

  const std::vector<SliceInfo>& GetSlices() const
  {
    return mSlices;
  }

  float GetInnerRadiusRatio() const
  {
    return mInnerRadiusRatio;
  }
  void SetInnerRadiusRatio(float r)
  {
    mInnerRadiusRatio = std::clamp(r, 0.0f, 0.95f);
    EmitDataChangedSignal();
  }

  const Dali::String& GetCenterLabel() const
  {
    return mCenterLabel;
  }
  void SetCenterLabel(const Dali::String& t)
  {
    mCenterLabel = t;
    EmitDataChangedSignal();
  }

  float GetSliceGap() const
  {
    return mSliceGap;
  }
  void SetSliceGap(float d)
  {
    mSliceGap = std::clamp(d, 0.0f, 10.0f);
    EmitDataChangedSignal();
  }

  bool IsDataLabelsVisible() const
  {
    return mShowDataLabels;
  }
  void SetDataLabelsVisible(bool v)
  {
    mShowDataLabels = v;
    EmitDataChangedSignal();
  }

  float GetDataLabelMinAngle() const
  {
    return mDataLabelMinAngle;
  }
  void SetDataLabelMinAngle(float d)
  {
    mDataLabelMinAngle = std::max(0.0f, d);
    EmitDataChangedSignal();
  }

protected:
  ~PieSeries() override = default;

private:
  std::vector<SliceInfo> mSlices;
  Dali::String           mCenterLabel;
  float                  mInnerRadiusRatio{0.0f};
  float                  mSliceGap{1.0f};
  float                  mDataLabelMinAngle{15.0f};
  bool                   mShowDataLabels{false};
};

} // namespace Integration

inline Integration::PieSeries& GetImplementation(Ui::PieSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "PieSeries handle is empty");
  return static_cast<Integration::PieSeries&>(handle.GetBaseObject());
}

inline const Integration::PieSeries& GetImplementation(const Ui::PieSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "PieSeries handle is empty");
  return static_cast<const Integration::PieSeries&>(handle.GetBaseObject());
}

} // namespace Ui
} // namespace Dali
