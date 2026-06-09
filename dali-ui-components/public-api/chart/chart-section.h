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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ChartSection;
}

/**
 * @brief Defines a highlighted rectangular region on the chart in data coordinates.
 *
 * NaN boundary values extend the section to the corresponding plot area edge.
 * xMin == xMax draws a vertical threshold line; yMin == yMax draws a horizontal one.
 */
class DALI_UI_API ChartSection : public BaseHandle
{
public:
  static ChartSection New();

  ChartSection();
  ~ChartSection();
  ChartSection(const ChartSection&)            = default;
  ChartSection& operator=(const ChartSection&) = default;

  void  SetXMin(float value);
  float GetXMin() const;

  void  SetXMax(float value);
  float GetXMax() const;

  void  SetYMin(float value);
  float GetYMin() const;

  void  SetYMax(float value);
  float GetYMax() const;

  void    SetFillColor(const Vector4& color);
  Vector4 GetFillColor() const;

  void    SetStrokeColor(const Vector4& color);
  Vector4 GetStrokeColor() const;

  void  SetStrokeWidth(float width);
  float GetStrokeWidth() const;

public:
  explicit DALI_UI_API ChartSection(Integration::ChartSection* impl);
};

} // namespace Ui
} // namespace Dali
