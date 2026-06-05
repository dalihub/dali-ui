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

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/public-api/chart/bar-series.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class BarSeries : public ChartSeries
{
public:
  BarSeries();

  Vector4 GetColor() const
  {
    return mColor;
  }
  void SetColor(const Vector4& c)
  {
    mColor    = c;
    mColorSet = true;
  }
  bool IsColorSet() const
  {
    return mColorSet;
  }

  float GetBarGroupWidth() const
  {
    return mBarGroupWidth;
  }
  void SetBarGroupWidth(float r)
  {
    mBarGroupWidth = std::clamp(r, 0.01f, 1.0f);
  }

  bool IsDataLabelsVisible() const
  {
    return mShowDataLabels;
  }
  void SetDataLabelsVisible(bool v)
  {
    mShowDataLabels = v;
  }
  const Dali::String& GetDataLabelFormat() const
  {
    return mDataLabelFormat;
  }
  void SetDataLabelFormat(const Dali::String& f)
  {
    mDataLabelFormat = f;
  }
  Vector4 GetDataLabelColor() const
  {
    return mDataLabelColor;
  }
  void SetDataLabelColor(const Vector4& c)
  {
    mDataLabelColor = c;
  }
  float GetDataLabelSize() const
  {
    return mDataLabelSize;
  }
  void SetDataLabelSize(float s)
  {
    mDataLabelSize = s;
  }

  bool IsStacked() const
  {
    return mStacked;
  }
  void SetStacked(bool s)
  {
    mStacked = s;
  }

protected:
  ~BarSeries() override = default;

private:
  Dali::String mDataLabelFormat{"%.0f"};
  Vector4      mColor{0.3f, 0.6f, 1.0f, 1.0f};
  Vector4      mDataLabelColor{0.2f, 0.2f, 0.2f, 1.0f};
  float        mBarGroupWidth{0.8f};
  float        mDataLabelSize{9.0f};
  bool         mColorSet{false};
  bool         mShowDataLabels{false};
  bool         mStacked{false};
};

} // namespace Integration

inline Integration::BarSeries& GetImplementation(Ui::BarSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "BarSeries handle is empty");
  return static_cast<Integration::BarSeries&>(handle.GetBaseObject());
}

inline const Integration::BarSeries& GetImplementation(const Ui::BarSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "BarSeries handle is empty");
  return static_cast<const Integration::BarSeries&>(handle.GetBaseObject());
}

} // namespace Ui
} // namespace Dali
