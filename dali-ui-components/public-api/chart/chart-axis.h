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
#include <dali/public-api/object/base-handle.h>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ChartAxis;
}

/**
 * @brief Represents the X or Y axis of a ChartView.
 */
class DALI_UI_API ChartAxis : public Dali::BaseHandle
{
public:
  ChartAxis();
  ~ChartAxis();
  ChartAxis(const ChartAxis& rhs);
  ChartAxis(ChartAxis&& rhs);
  ChartAxis& operator=(const ChartAxis& rhs);
  ChartAxis& operator=(ChartAxis&& rhs);

  static ChartAxis New();
  static ChartAxis DownCast(BaseHandle handle);

  void                      SetLabels(const std::vector<Dali::String>& labels);
  std::vector<Dali::String> GetLabels() const;

  void  SetMinLimit(float min);
  float GetMinLimit() const;

  void  SetMaxLimit(float max);
  float GetMaxLimit() const;

  void SetAutoRange(bool autoRange);
  bool IsAutoRange() const;

  void SetShowGridLines(bool show);
  bool GetShowGridLines() const;

  void  SetLabelSize(float size);
  float GetLabelSize() const;

  void         SetTitle(const Dali::String& title);
  Dali::String GetTitle() const;

  void    SetAxisLineColor(const Vector4& color);
  Vector4 GetAxisLineColor() const;

  void  SetAxisLineWidth(float width);
  float GetAxisLineWidth() const;

  void SetGridDash(float dashLength, float gapLength);
  void GetGridDash(float& dashLength, float& gapLength) const;

  void    SetGridColor(const Vector4& color);
  Vector4 GetGridColor() const;

  void  SetMinStep(float step);
  float GetMinStep() const;

  void  SetDataPadding(float fraction);
  float GetDataPadding() const;

  void  SetLabelsRotation(float degrees);
  float GetLabelsRotation() const;

public:
  explicit DALI_UI_API ChartAxis(Integration::ChartAxis* impl);
};

} // namespace Ui
} // namespace Dali
