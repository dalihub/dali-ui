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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class BarSeries;
}

/**
 * @brief Vertical bar chart data series.
 *
 * Multiple BarSeries added to the same BAR chart are grouped side by side
 * per X position by default; call SetStacked(true) to stack them instead.
 *
 * @code
 *   BarSeries series = BarSeries::New();
 *   series.SetName("Revenue");
 *   series.SetValues({120.0f, 190.0f, 150.0f, 250.0f});
 *   chart.AddSeries(series);
 * @endcode
 */
class DALI_UI_COMPONENTS_API BarSeries : public ChartSeries
{
public:
  /**
   * @brief Creates a new BarSeries.
   *
   * @return A handle to the new series
   */
  static BarSeries New();

  /**
   * @brief Downcasts a handle to a BarSeries handle.
   *
   * @param[in] handle The handle to downcast
   * @return A BarSeries handle, or an uninitialized handle if the cast fails
   */
  static BarSeries DownCast(BaseHandle handle);

  /**
   * @brief Creates an uninitialized BarSeries handle.
   */
  BarSeries();

  /**
   * @brief Destructor.
   */
  ~BarSeries();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  BarSeries(const BarSeries&) = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  BarSeries& operator=(const BarSeries&) = default;

  /**
   * @brief Sets the color of the bars.
   *
   * Until this is called, bars are colored from the chart's color palette
   * instead, cycling by series index.
   *
   * @param[in] color The color of the bars
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the bars.
   *
   * @return The color of the bars, either the color set by SetColor() or a
   *         palette color if it was not called
   */
  Vector4 GetColor() const;

  /**
   * @brief Sets the width of each bar group, as a fraction of the space available per X position.
   *
   * Clamped to [0.01, 1.0]. The default is 0.8.
   *
   * @param[in] ratio The bar group width, as a fraction of the available space
   */
  void SetBarGroupWidth(float ratio);

  /**
   * @brief Retrieves the width of each bar group, as a fraction of the space available per X position.
   *
   * @return The bar group width, as a fraction of the available space
   */
  float GetBarGroupWidth() const;

  /**
   * @brief Sets whether this series stacks on top of other stacked BarSeries in the same chart, instead of grouping side by side.
   *
   * The default is false.
   *
   * @param[in] stacked True to stack this series
   */
  void SetStacked(bool stacked);

  /**
   * @brief Retrieves whether this series stacks on top of other stacked BarSeries in the same chart.
   *
   * @return True if this series stacks
   */
  bool IsStacked() const;

  /**
   * @brief Sets whether a text label is drawn on each bar.
   *
   * The default is false.
   *
   * @param[in] visible True to draw data labels
   */
  void SetDataLabelsVisible(bool visible);

  /**
   * @brief Retrieves whether a text label is drawn on each bar.
   *
   * @return True if data labels are drawn
   */
  bool IsDataLabelsVisible() const;

  /**
   * @brief Sets the printf-style format string used to render each bar's data label.
   *
   * The default is "%.0f".
   *
   * @param[in] fmt The printf-style format string
   */
  void SetDataLabelFormat(const Dali::String& fmt);

  /**
   * @brief Retrieves the printf-style format string used to render each bar's data label.
   *
   * @return The printf-style format string
   */
  Dali::String GetDataLabelFormat() const;

  /**
   * @brief Sets the color of the data label text.
   *
   * The default is (0.2, 0.2, 0.2, 1.0).
   *
   * @param[in] color The color of the data label text
   */
  void SetDataLabelColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the data label text.
   *
   * @return The color of the data label text
   */
  Vector4 GetDataLabelColor() const;

  /**
   * @brief Sets the point size of the data label text.
   *
   * The default is 9.0.
   *
   * @param[in] pointSize The point size of the data label text
   */
  void SetDataLabelSize(float pointSize);

  /**
   * @brief Retrieves the point size of the data label text.
   *
   * @return The point size of the data label text
   */
  float GetDataLabelSize() const;

public:
  explicit BarSeries(Integration::BarSeries* impl);
};

} // namespace Ui
} // namespace Dali
