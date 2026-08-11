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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
#include <initializer_list>

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
 *
 * Assign an axis to a chart with ChartView::SetXAxis() or ChartView::SetYAxis().
 * By default an axis auto-ranges to fit the data; calling SetMinimumLimit() or
 * SetMaximumLimit() switches it to a fixed range (see IsAutoRangeEnabled()).
 *
 * @code
 *   ChartAxis xAxis = ChartAxis::New();
 *   xAxis.SetLabels({"Jan", "Feb", "Mar", "Apr"});
 *   chart.SetXAxis(xAxis);
 * @endcode
 */
class DALI_UI_COMPONENTS_API ChartAxis : public Dali::BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized ChartAxis handle.
   */
  ChartAxis();

  /**
   * @brief Destructor.
   */
  ~ChartAxis();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  ChartAxis(const ChartAxis& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The handle to move
   */
  ChartAxis(ChartAxis&& rhs);

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  ChartAxis& operator=(const ChartAxis& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  ChartAxis& operator=(ChartAxis&& rhs);

  /**
   * @brief Creates a new ChartAxis.
   *
   * @return A handle to the new axis
   */
  static ChartAxis New();

  /**
   * @brief Downcasts a handle to a ChartAxis handle.
   *
   * @param[in] handle The handle to downcast
   * @return A ChartAxis handle, or an uninitialized handle if the cast fails
   */
  static ChartAxis DownCast(BaseHandle handle);

  /**
   * @brief Sets the category labels drawn along the axis.
   *
   * Replaces any existing labels.
   *
   * @param[in] labels The labels, in axis order
   */
  void SetLabels(const Dali::Vector<Dali::String>& labels);

  /**
   * @brief Sets the category labels drawn along the axis.
   *
   * @param[in] labels The labels, in axis order
   */
  void SetLabels(std::initializer_list<Dali::String> labels)
  {
    Dali::Vector<Dali::String> list;
    list.Reserve(labels.size());
    for(auto&& label : labels)
    {
      list.PushBack(label);
    }
    SetLabels(list);
  }

  /**
   * @brief Retrieves the category labels drawn along the axis.
   *
   * @return The labels, in axis order
   */
  Dali::Vector<Dali::String> GetLabels() const;

  /**
   * @brief Sets the fixed minimum value of the axis, in data coordinates.
   *
   * Also switches the axis out of auto-range mode (see IsAutoRangeEnabled()).
   * The default is 0.0.
   *
   * @param[in] min The minimum value of the axis
   */
  void SetMinimumLimit(float min);

  /**
   * @brief Retrieves the minimum value of the axis, in data coordinates.
   *
   * @return The minimum value of the axis
   */
  float GetMinimumLimit() const;

  /**
   * @brief Sets the fixed maximum value of the axis, in data coordinates.
   *
   * Also switches the axis out of auto-range mode (see IsAutoRangeEnabled()).
   * The default is 1.0.
   *
   * @param[in] max The maximum value of the axis
   */
  void SetMaximumLimit(float max);

  /**
   * @brief Retrieves the maximum value of the axis, in data coordinates.
   *
   * @return The maximum value of the axis
   */
  float GetMaximumLimit() const;

  /**
   * @brief Sets whether the axis automatically fits its range to the series data.
   *
   * While enabled, the min/max limits are recomputed whenever the series
   * data changes. Calling SetMinimumLimit() or SetMaximumLimit() sets this to false.
   * The default is true.
   *
   * @param[in] enabled True to fit the range to the series data automatically
   */
  void SetAutoRangeEnabled(bool enabled);

  /**
   * @brief Retrieves whether the axis automatically fits its range to the series data.
   *
   * @return True if the range is fit to the series data automatically
   */
  bool IsAutoRangeEnabled() const;

  /**
   * @brief Sets whether grid lines are drawn at this axis' tick positions.
   *
   * The default is true.
   *
   * @param[in] enabled True to draw grid lines
   */
  void SetShowGridLinesEnabled(bool enabled);

  /**
   * @brief Retrieves whether grid lines are drawn at this axis' tick positions.
   *
   * @return True if grid lines are drawn
   */
  bool IsShowGridLinesEnabled() const;

  /**
   * @brief Sets the point size of the tick labels.
   *
   * The default is 11.0.
   *
   * @param[in] size The point size of the tick labels
   */
  void SetLabelSize(float size);

  /**
   * @brief Retrieves the point size of the tick labels.
   *
   * @return The point size of the tick labels
   */
  float GetLabelSize() const;

  /**
   * @brief Sets the axis title text.
   *
   * The title is drawn alongside the axis. An empty title (the default)
   * draws nothing.
   *
   * @param[in] title The axis title
   */
  void SetTitle(const Dali::String& title);

  /**
   * @brief Retrieves the axis title text.
   *
   * @return The axis title
   */
  Dali::String GetTitle() const;

  /**
   * @brief Sets the color of the axis line.
   *
   * The default is (0.2, 0.2, 0.2, 1.0).
   *
   * @param[in] color The color of the axis line
   */
  void SetAxisLineColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the axis line.
   *
   * @return The color of the axis line
   */
  Vector4 GetAxisLineColor() const;

  /**
   * @brief Sets the width of the axis line, in pixels.
   *
   * The default is 2.0.
   *
   * @param[in] width The width of the axis line, in pixels
   */
  void SetAxisLineWidth(float width);

  /**
   * @brief Retrieves the width of the axis line, in pixels.
   *
   * @return The width of the axis line, in pixels
   */
  float GetAxisLineWidth() const;

  /**
   * @brief Sets the dash pattern of this axis' grid lines.
   *
   * A gapLength of 0 draws a solid line. Defaults are dashLength 5.0,
   * gapLength 4.0.
   *
   * @param[in] dashLength The length of each dash, in pixels
   * @param[in] gapLength The length of each gap between dashes, in pixels
   */
  void SetGridDash(float dashLength, float gapLength);

  /**
   * @brief Retrieves the dash pattern of this axis' grid lines.
   *
   * @param[out] dashLength The length of each dash, in pixels
   * @param[out] gapLength The length of each gap between dashes, in pixels
   */
  void GetGridDash(float& dashLength, float& gapLength) const;

  /**
   * @brief Sets the color of this axis' grid lines.
   *
   * The default is (0.8, 0.8, 0.8, 1.0).
   *
   * @param[in] color The color of the grid lines
   */
  void SetGridColor(const Vector4& color);

  /**
   * @brief Retrieves the color of this axis' grid lines.
   *
   * @return The color of the grid lines
   */
  Vector4 GetGridColor() const;

  /**
   * @brief Sets the minimum spacing between ticks, in data units.
   *
   * Negative values are clamped to 0. The default is 0.0, meaning the tick
   * spacing is chosen automatically with no minimum.
   *
   * @param[in] step The minimum spacing between ticks, in data units
   */
  void SetMinimumStep(float step);

  /**
   * @brief Retrieves the minimum spacing between ticks, in data units.
   *
   * @return The minimum spacing between ticks, in data units
   */
  float GetMinimumStep() const;

  /**
   * @brief Sets extra padding added to the auto-computed data range, as a fraction of it.
   *
   * For example 0.1 adds 10% of the data range as padding on top of the
   * range before axis limits are computed. Only applies while IsAutoRangeEnabled()
   * is true. Negative values are clamped to 0. The default is 0.0.
   *
   * @param[in] fraction The padding, as a fraction of the data range
   */
  void SetDataPadding(float fraction);

  /**
   * @brief Retrieves the extra padding added to the auto-computed data range.
   *
   * @return The padding, as a fraction of the data range
   */
  float GetDataPadding() const;

  /**
   * @brief Sets the rotation applied to the tick labels.
   *
   * The default is 0.0.
   *
   * @param[in] degrees The rotation angle, in degrees
   */
  void SetLabelsRotation(float degrees);

  /**
   * @brief Retrieves the rotation applied to the tick labels.
   *
   * @return The rotation angle, in degrees
   */
  float GetLabelsRotation() const;

public:
  explicit ChartAxis(Integration::ChartAxis* impl);
};

} // namespace Ui
} // namespace Dali
