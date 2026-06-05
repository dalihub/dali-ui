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
#include <algorithm>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-axis.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ChartAxis : public Dali::BaseObject
{
public:
  using ConfigChangedSignalType = Signal<void()>;

  ChartAxis();

  void                      SetLabels(const std::vector<Dali::String>& labels);
  std::vector<Dali::String> GetLabels() const;

  void  SetMinLimit(float min);
  float GetMinLimit() const
  {
    return mMinLimit;
  }

  void  SetMaxLimit(float max);
  float GetMaxLimit() const
  {
    return mMaxLimit;
  }

  void SetAutoRange(bool autoRange);
  bool GetAutoRange() const
  {
    return mAutoRange;
  }

  bool GetShowGridLines() const
  {
    return mShowGridLines;
  }
  float GetLabelSize() const
  {
    return mLabelSize;
  }
  const Dali::String& GetTitle() const
  {
    return mTitle;
  }

  void SetShowGridLines(bool show);
  void SetLabelSize(float size);
  void SetTitle(const Dali::String& title);

  void    SetAxisLineColor(const Vector4& color);
  Vector4 GetAxisLineColor() const
  {
    return mAxisLineColor;
  }

  void  SetAxisLineWidth(float width);
  float GetAxisLineWidth() const
  {
    return mAxisLineWidth;
  }

  void SetGridDash(float dashLength, float gapLength);
  void GetGridDash(float& dashLength, float& gapLength) const
  {
    dashLength = mGridDashLength;
    gapLength  = mGridGapLength;
  }

  void    SetGridColor(const Vector4& color);
  Vector4 GetGridColor() const
  {
    return mGridColor;
  }

  void SetMinStep(float step)
  {
    mMinStep = std::max(0.0f, step);
    EmitConfigChangedSignal();
  }
  float GetMinStep() const
  {
    return mMinStep;
  }

  void SetDataPadding(float fraction)
  {
    mDataPadding = std::max(0.0f, fraction);
    EmitConfigChangedSignal();
  }
  float GetDataPadding() const
  {
    return mDataPadding;
  }

  void SetLabelsRotation(float degrees)
  {
    mLabelsRotation = degrees;
    EmitConfigChangedSignal();
  }
  float GetLabelsRotation() const
  {
    return mLabelsRotation;
  }

  ConfigChangedSignalType& ConfigChangedSignal();

protected:
  ~ChartAxis() override;

private:
  void EmitConfigChangedSignal();

  std::vector<Dali::String> mLabels;
  Dali::String              mTitle;
  float                     mMinLimit;
  float                     mMaxLimit;
  float                     mLabelSize;
  bool                      mAutoRange;
  bool                      mShowGridLines;

  Vector4 mAxisLineColor;
  float   mAxisLineWidth;
  float   mGridDashLength;
  float   mGridGapLength;
  Vector4 mGridColor;

  float mMinStep{0.0f};
  float mDataPadding{0.0f};
  float mLabelsRotation{0.0f};

  ConfigChangedSignalType mConfigChangedSignal;
};

} // namespace Integration

inline Integration::ChartAxis& GetImplementation(Ui::ChartAxis& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartAxis handle is empty");
  BaseObject& object = handle.GetBaseObject();
  return static_cast<Integration::ChartAxis&>(object);
}

inline const Integration::ChartAxis& GetImplementation(const Ui::ChartAxis& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartAxis handle is empty");
  const BaseObject& object = handle.GetBaseObject();
  return static_cast<const Integration::ChartAxis&>(object);
}

} // namespace Ui
} // namespace Dali
