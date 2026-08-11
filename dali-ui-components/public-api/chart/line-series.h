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
class LineSeries;
}

/**
 * @brief Line chart data series.
 *
 * Used directly for LINE and SCATTER-like line charts, and for AREA charts
 * by additionally calling SetFillEnabled(true).
 *
 * @code
 *   LineSeries series = LineSeries::New();
 *   series.SetName("CPU");
 *   series.SetColor(Vector4(0.0f, 0.6f, 0.9f, 1.0f));
 *   series.SetValues({12.0f, 48.0f, 31.0f, 66.0f});
 *   chart.AddSeries(series);
 * @endcode
 */
class DALI_UI_COMPONENTS_API LineSeries : public ChartSeries
{
public:
  /**
   * @brief The shape drawn at each data point when markers are visible.
   *
   * @see SetMarkersVisible(), SetMarkerShape()
   */
  enum class MarkerShape
  {
    CIRCLE,   ///< Circular marker. This is the default.
    SQUARE,   ///< Square marker.
    TRIANGLE, ///< Triangular marker.
    DIAMOND   ///< Diamond-shaped marker.
  };

  /**
   * @brief Creates an uninitialized LineSeries handle.
   */
  LineSeries();

  /**
   * @brief Destructor.
   */
  ~LineSeries();

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  LineSeries(const LineSeries& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The handle to move
   */
  LineSeries(LineSeries&& rhs);

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  LineSeries& operator=(const LineSeries& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  LineSeries& operator=(LineSeries&& rhs);

  /**
   * @brief Creates a new LineSeries.
   *
   * @return A handle to the new series
   */
  static LineSeries New();

  /**
   * @brief Downcasts a handle to a LineSeries handle.
   *
   * @param[in] handle The handle to downcast
   * @return A LineSeries handle, or an uninitialized handle if the cast fails
   */
  static LineSeries DownCast(BaseHandle handle);

  /**
   * @brief Sets the width of the line, in pixels.
   *
   * The default is 2.5.
   *
   * @param[in] width The width of the line, in pixels
   */
  void SetLineWidth(float width);

  /**
   * @brief Retrieves the width of the line, in pixels.
   *
   * @return The width of the line, in pixels
   */
  float GetLineWidth() const;

  /**
   * @brief Sets how much the line curves through its data points, instead of using straight segments.
   *
   * Clamped to [0, 1], where 0 draws straight segments and 1 is maximally
   * smoothed. The default is 0.0.
   *
   * @param[in] value The smoothing amount
   */
  void SetSmoothness(float value);

  /**
   * @brief Retrieves how much the line curves through its data points.
   *
   * @return The smoothing amount
   */
  float GetSmoothness() const;

  /**
   * @brief Sets the color of the line.
   *
   * Also the base color used to derive the default marker color
   * (see SetMarkerColor()) and default fill color (see SetFillColor())
   * unless those are overridden explicitly. The default is black.
   *
   * @param[in] color The color of the line
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the line.
   *
   * @return The color of the line
   */
  Vector4 GetColor() const;

  /**
   * @brief Sets whether a marker is drawn at each data point.
   *
   * The default is true.
   *
   * @param[in] visible True to draw markers
   */
  void SetMarkersVisible(bool visible);

  /**
   * @brief Retrieves whether a marker is drawn at each data point.
   *
   * @return True if markers are drawn
   */
  bool IsMarkersVisible() const;

  /**
   * @brief Sets the radius of each marker, in pixels.
   *
   * The default is 4.0.
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

  /**
   * @brief Sets the fill color of each marker.
   *
   * Until this is called, markers are filled with the line color set by
   * SetColor().
   *
   * @param[in] color The marker fill color
   */
  void SetMarkerColor(const Vector4& color);

  /**
   * @brief Retrieves the fill color of each marker.
   *
   * @return The marker fill color, either the color set by SetMarkerColor()
   *         or the line color if it was not called
   */
  Vector4 GetMarkerColor() const;

  /**
   * @brief Sets the border color of each marker.
   *
   * The default is fully transparent, meaning no border is visible.
   *
   * @param[in] color The marker border color
   */
  void SetMarkerBorderColor(const Vector4& color);

  /**
   * @brief Retrieves the border color of each marker.
   *
   * @return The marker border color
   */
  Vector4 GetMarkerBorderColor() const;

  /**
   * @brief Sets the border width of each marker, in pixels.
   *
   * The default is 0.0.
   *
   * @param[in] width The marker border width, in pixels
   */
  void SetMarkerBorderWidth(float width);

  /**
   * @brief Retrieves the border width of each marker, in pixels.
   *
   * @return The marker border width, in pixels
   */
  float GetMarkerBorderWidth() const;

  /**
   * @brief Sets whether a text label is drawn at each data point.
   *
   * The default is false.
   *
   * @param[in] visible True to draw data labels
   */
  void SetDataLabelsVisible(bool visible);

  /**
   * @brief Retrieves whether a text label is drawn at each data point.
   *
   * @return True if data labels are drawn
   */
  bool IsDataLabelsVisible() const;

  /**
   * @brief Sets the printf-style format string used to render each data label's Y value.
   *
   * Ignored while a formatter set with SetDataLabelFormatter() is active.
   * The default is "%.1f".
   *
   * @param[in] format The printf-style format string
   */
  void SetDataLabelFormat(const Dali::String& format);

  /**
   * @brief Retrieves the printf-style format string used to render each data label's Y value.
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
   * The default is 8.0.
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
   * @brief Sets whether the area under the line is filled.
   *
   * Used to build AREA charts from a LINE-family series. The default is false.
   *
   * @param[in] enabled True to fill the area under the line
   */
  void SetFillEnabled(bool enabled);

  /**
   * @brief Retrieves whether the area under the line is filled.
   *
   * @return True if the area under the line is filled
   */
  bool IsFillEnabled() const;

  /**
   * @brief Sets the fill color of the area under the line.
   *
   * Until this is called, the fill uses the line color set by SetColor()
   * at 20% opacity.
   *
   * @param[in] color The fill color
   */
  void SetFillColor(const Vector4& color);

  /**
   * @brief Retrieves the fill color of the area under the line.
   *
   * @return The fill color, either the color set by SetFillColor() or the
   *         line color at 20% opacity if it was not called
   */
  Vector4 GetFillColor() const;

  /**
   * @brief Function pointer type for building the text of a data label.
   *
   * @note This is a plain function pointer, not std::function.
   *       Capturing lambdas and non-static member functions are not accepted.
   *       Use a free function, a static function, or a captureless lambda.
   *       If state is required, route through an application singleton.
   *
   * @param[in] value The Y value of the point in data coordinates
   * @param[in] pointIndex The index of the point within the series
   * @return The data label text to display
   */
  using DataLabelFormatterType = Dali::String (*)(float value, int pointIndex);

  /**
   * @brief Sets the formatter used to build the text of each data label.
   *
   * @param[in] formatter The formatter, or nullptr to use the data label format
   */
  void SetDataLabelFormatter(DataLabelFormatterType formatter);

  /**
   * @brief Retrieves the formatter used to build the text of each data label.
   *
   * @return The formatter, or nullptr if the data label format is used
   */
  DataLabelFormatterType GetDataLabelFormatter() const;

public:
  explicit LineSeries(Integration::LineSeries* impl);
};

} // namespace Ui
} // namespace Dali
