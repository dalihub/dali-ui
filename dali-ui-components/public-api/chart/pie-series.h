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
class PieSeries;
}

/**
 * @brief Pie/Donut chart data series.
 *
 * Unlike other series types, a PieSeries does not use SetValues(); its data
 * points are individual slices added with AddSlice(). Set
 * SetInnerRadiusRatio() above 0 to draw a donut instead of a full pie.
 *
 * @code
 *   PieSeries series = PieSeries::New();
 *   series.AddSlice("Chrome", 65.0f, Vector4(0.26f, 0.52f, 0.96f, 1.0f));
 *   series.AddSlice("Safari", 20.0f, Vector4(0.0f, 0.48f, 1.0f, 1.0f));
 *   series.AddSlice("Other", 15.0f, Vector4(0.6f, 0.6f, 0.6f, 1.0f));
 *   chart.AddSeries(series);
 * @endcode
 */
class DALI_UI_COMPONENTS_API PieSeries : public ChartSeries
{
public:
  /**
   * @brief Creates an uninitialized PieSeries handle.
   */
  PieSeries();

  /**
   * @brief Destructor.
   */
  ~PieSeries();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  PieSeries(const PieSeries& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The handle to move
   */
  PieSeries(PieSeries&& rhs);

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  PieSeries& operator=(const PieSeries& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  PieSeries& operator=(PieSeries&& rhs);

  /**
   * @brief Creates a new PieSeries.
   *
   * @return A handle to the new series
   */
  static PieSeries New();

  /**
   * @brief Downcasts a handle to a PieSeries handle.
   *
   * @param[in] handle The handle to downcast
   * @return A PieSeries handle, or an uninitialized handle if the cast fails
   */
  static PieSeries DownCast(BaseHandle handle);

  /**
   * @brief Adds a slice to the pie.
   *
   * Slices with a value of 0 or less are ignored. Slices are drawn in the
   * order they were added.
   *
   * @param[in] label The slice label, shown in the legend and tooltip
   * @param[in] value The slice value; each slice's share of the pie is its
   *                   value divided by the sum of all slice values
   * @param[in] color The slice color
   */
  void AddSlice(const Dali::String& label, float value, const Vector4& color);

  /**
   * @brief Removes all slices from the series.
   */
  void ClearSlices();

  /**
   * @brief Retrieves the number of slices in the series.
   *
   * @return The number of slices
   */
  int GetSliceCount() const;

  /**
   * @brief Sets the inner radius of the pie, as a fraction of its outer radius.
   *
   * A value above 0 draws a donut chart with a hole of that relative size.
   * Clamped to [0, 0.95]. The default is 0.0.
   *
   * @param[in] ratio The inner radius, as a fraction of the outer radius
   */
  void SetInnerRadiusRatio(float ratio);

  /**
   * @brief Retrieves the inner radius of the pie, as a fraction of its outer radius.
   *
   * @return The inner radius, as a fraction of the outer radius
   */
  float GetInnerRadiusRatio() const;

  /**
   * @brief Sets the text shown at the center of a donut (SetInnerRadiusRatio() above 0).
   *
   * An empty label (the default) draws nothing.
   *
   * @param[in] text The center label text
   */
  void SetCenterLabel(const Dali::String& text);

  /**
   * @brief Retrieves the text shown at the center of a donut.
   *
   * @return The center label text
   */
  Dali::String GetCenterLabel() const;

  /**
   * @brief Sets the angular gap drawn between adjacent slices.
   *
   * Clamped to [0, 10]. The default is 1.0.
   *
   * @param[in] degrees The gap between slices, in degrees
   */
  void SetSliceGap(float degrees);

  /**
   * @brief Retrieves the angular gap drawn between adjacent slices.
   *
   * @return The gap between slices, in degrees
   */
  float GetSliceGap() const;

  /**
   * @brief Sets whether a text label is drawn on each slice.
   *
   * The default is false.
   *
   * @param[in] visible True to draw data labels
   */
  void SetDataLabelsVisible(bool visible);

  /**
   * @brief Retrieves whether a text label is drawn on each slice.
   *
   * @return True if data labels are drawn
   */
  bool IsDataLabelsVisible() const;

  /**
   * @brief Sets the minimum angular span a slice must have to draw its data label.
   *
   * Slices narrower than this are skipped to avoid overlapping labels.
   * Negative values are clamped to 0. The default is 15.0.
   *
   * @param[in] degrees The minimum slice angle, in degrees
   */
  void SetDataLabelMinAngle(float degrees);

  /**
   * @brief Retrieves the minimum angular span a slice must have to draw its data label.
   *
   * @return The minimum slice angle, in degrees
   */
  float GetDataLabelMinAngle() const;

public:
  explicit PieSeries(Integration::PieSeries* impl);
};

} // namespace Ui
} // namespace Dali
