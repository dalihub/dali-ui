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

  ChartAxis&                SetLabels(const std::vector<Dali::String>& labels);
  std::vector<Dali::String> GetLabels() const;

  ChartAxis& SetMinLimit(float min);
  float      GetMinLimit() const;

  ChartAxis& SetMaxLimit(float max);
  float      GetMaxLimit() const;

  ChartAxis& SetAutoRange(bool autoRange);
  bool       IsAutoRange() const;

  ChartAxis& SetShowGridLines(bool show);
  bool       GetShowGridLines() const;

  ChartAxis& SetLabelSize(float size);
  float      GetLabelSize() const;

  ChartAxis&   SetTitle(const Dali::String& title);
  Dali::String GetTitle() const;

  ChartAxis& SetAxisLineColor(const Vector4& color);
  Vector4    GetAxisLineColor() const;

  ChartAxis& SetAxisLineWidth(float width);
  float      GetAxisLineWidth() const;

  ChartAxis& SetGridDash(float dashLength, float gapLength);
  void       GetGridDash(float& dashLength, float& gapLength) const;

  ChartAxis& SetGridColor(const Vector4& color);
  Vector4    GetGridColor() const;

  ChartAxis& SetMinStep(float step);
  float      GetMinStep() const;

  ChartAxis& SetDataPadding(float fraction);
  float      GetDataPadding() const;

  ChartAxis& SetLabelsRotation(float degrees);
  float      GetLabelsRotation() const;

public:
  explicit DALI_UI_API ChartAxis(Integration::ChartAxis* impl);
};

} // namespace Ui
} // namespace Dali
