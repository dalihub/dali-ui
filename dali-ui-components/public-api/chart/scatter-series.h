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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class ScatterSeries;
}

/**
 * @brief Scatter plot data series.
 *
 * Set XY data points with ChartSeries::SetValues(const Dali::Vector<Vector2>&),
 * unlike LineSeries/BarSeries which are typically indexed by position.
 *
 * @code
 *   ScatterSeries series = ScatterSeries::New();
 *   series.SetName("Samples");
 *   series.SetValues({Vector2(0.5f, 30.0f), Vector2(1.8f, 55.0f), Vector2(3.2f, 45.0f)});
 *   chart.AddSeries(series);
 * @endcode
 */
class DALI_UI_COMPONENTS_API ScatterSeries : public ChartSeries
{
public:
  /**
   * @brief The shape drawn for each data point marker.
   */
  enum class MarkerShape
  {
    CIRCLE,   ///< Circular marker. This is the default.
    SQUARE,   ///< Square marker.
    TRIANGLE, ///< Triangular marker.
    DIAMOND   ///< Diamond-shaped marker.
  };

  /**
   * @brief Creates a new ScatterSeries.
   *
   * @return A handle to the new series
   */
  static ScatterSeries New();

  /**
   * @brief Downcasts a handle to a ScatterSeries handle.
   *
   * @param[in] handle The handle to downcast
   * @return A ScatterSeries handle, or an uninitialized handle if the cast fails
   */
  static ScatterSeries DownCast(BaseHandle handle);

  /**
   * @brief Creates an uninitialized ScatterSeries handle.
   */
  ScatterSeries();

  /**
   * @brief Destructor.
   */
  ~ScatterSeries();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  ScatterSeries(const ScatterSeries&) = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  ScatterSeries& operator=(const ScatterSeries&) = default;

  /**
   * @brief Sets the color of the markers.
   *
   * The default is (0.8, 0.3, 0.3, 1.0).
   *
   * @param[in] color The color of the markers
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the markers.
   *
   * @return The color of the markers
   */
  Vector4 GetColor() const;

  /**
   * @brief Sets the radius of each marker, in pixels.
   *
   * Clamped to a minimum of 1.0. The default is 6.0.
   *
   * @param[in] radius The marker radius, in pixels
   */
  void SetMarkerRadius(float radius);

  /**
   * @brief Retrieves the radius of each marker, in pixels.
   *
   * @return The marker radius, in pixels
   */
  float GetMarkerRadius() const;

  /**
   * @brief Sets the shape drawn for each marker.
   *
   * The default is MarkerShape::CIRCLE.
   *
   * @param[in] shape The marker shape
   */
  void SetMarkerShape(MarkerShape shape);

  /**
   * @brief Retrieves the shape drawn for each marker.
   *
   * @return The marker shape
   */
  MarkerShape GetMarkerShape() const;

public:
  explicit ScatterSeries(Integration::ScatterSeries* impl);
};

} // namespace Ui
} // namespace Dali
