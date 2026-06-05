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
#include <dali/public-api/object/base-handle.h>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ChartSeries;
}

#include "chart-series.autogen.h"

/**
 * @brief Base class for all chart series (Line, Bar, Pie, Scatter).
 */
class DALI_UI_API ChartSeries : public Dali::BaseHandle
{
public:
  ChartSeries();
  ~ChartSeries();
  ChartSeries(const ChartSeries& rhs);
  ChartSeries(ChartSeries&& rhs);
  ChartSeries& operator=(const ChartSeries& rhs);
  ChartSeries& operator=(ChartSeries&& rhs);

  static ChartSeries DownCast(BaseHandle handle);

  ChartSeries& SetName(const Dali::String& name);
  Dali::String GetName() const;

  // Common color shared by all concrete series types (line/marker/bar/scatter color).
  // For PieSeries this sets the default color applied to new slices.
  ChartSeries& SetColor(const Vector4& color);
  Vector4      GetColor() const;

  ChartSeries& SetValues(const std::vector<float>& values);
  ChartSeries& SetValues(const std::vector<std::pair<float, float>>& xyValues);

  ChartSeries& SetVisible(bool visible);
  bool         IsVisible() const;

  ChartSeries& AppendValue(float y);
  ChartSeries& AppendValue(float x, float y);
  ChartSeries& AppendValues(const std::vector<float>& values);

  ChartSeries& SetMaxDataPoints(int maxCount);
  int          GetMaxDataPoints() const;

  ChartSeries& SetZIndex(int zIndex);
  int          GetZIndex() const;

public:
  explicit DALI_UI_API ChartSeries(Integration::ChartSeries* impl);
};

} // namespace Ui
} // namespace Dali
