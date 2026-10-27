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
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace DALI_NAMESPACE
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

  /**
   * @brief Sets a finite non-negative corner radius in pixels, clamped to half the bar dimensions.
   * @param[in] radius The radius; zero preserves rectangular bars
   */
  void SetCornerRadius(float radius);

  /**
   * @brief Returns the authored corner radius.
   * @return The radius in pixels
   */
  float GetCornerRadius() const;

  /**
   * @brief Sets a linear fill gradient in bar-local coordinates.
   * @param[in] gradient The gradient
   */
  void SetFillGradient(const Gradient::Linear& gradient);

  /**
   * @brief Restores solid fill colors.
   */
  void ClearFillGradient();

  /**
   * @brief Replaces data with X/minimum/maximum triples. Range bars cannot be stacked.
   * @param[in] values Triples (X, minimum Y, maximum Y); minimum must not exceed maximum and NaN marks a gap
   */
  void SetRangeValues(const Dali::Vector<Vector3>& values);

  /**
   * @brief Sets the finite non-negative bar border width and color.
   * @param[in] width The border width
   * @param[in] color The border color; theme identifiers are retained
   */
  void SetBorder(float width, const UiColor& color);

  /**
   * @brief Sets finite non-negative border dash and gap lengths in pixels.
   *
   * Both lengths must be positive to draw a dashed border. Otherwise it is solid.
   * @param[in] dash The dash length
   * @param[in] gap The gap length
   */
  void SetBorderDash(float dash, float gap);

  /**
   * @brief Returns the border dash and gap lengths, initially zero.
   * @return The dash and gap in pixels
   */
  Vector2 GetBorderDash() const;

  /**
   * @brief Colors bars below and above a finite data value.
   *
   * The boundary uses the chart's Y scale and stays aligned with sections at the
   * same value. Replaces any fill gradient; point color overrides take priority.
   * @param[in] value The threshold in data coordinates
   * @param[in] lowerColor The color below the threshold
   * @param[in] upperColor The color above the threshold
   */
  void SetFillThreshold(float value, const UiColor& lowerColor, const UiColor& upperColor);

  /**
   * @brief Restores solid fill after a threshold fill.
   */
  void ClearFillThreshold();

  /**
   * @brief Overrides the border of one bar, independently of its fill color.
   *
   * The override replaces the series border width, color and dash pattern.
   * Width zero hides the border; both dash and gap must be positive to dash it.
   * Overrides stay at their indices after data replacement until cleared.
   * Indices beyond the current data can be configured for future points.
   * @param[in] pointIndex The zero-based data index
   * @param[in] width The finite non-negative border width in pixels
   * @param[in] color The border color; theme identifiers are retained
   * @param[in] dash The finite non-negative dash length in pixels
   * @param[in] gap The finite non-negative gap length in pixels
   */
  void SetPointBorder(uint32_t pointIndex, float width, const UiColor& color, float dash = 0.0f, float gap = 0.0f);

  /**
   * @brief Restores the current series border for one data index.
   * @param[in] pointIndex The zero-based data index; an unset index is ignored
   */
  void ClearPointBorder(uint32_t pointIndex);

  /**
   * @brief Restores the current series border for all data indices.
   */
  void ClearPointBorders();

public:
  explicit BarSeries(Integration::BarSeries* impl);
};

} // namespace Ui
} //namespace DALI_NAMESPACE
