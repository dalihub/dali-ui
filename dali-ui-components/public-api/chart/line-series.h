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
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-series.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <cstdint>

namespace DALI_NAMESPACE
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
 * @SINCE_3_0.0
 */
class DALI_UI_COMPONENTS_API LineSeries : public ChartSeries
{
public:
  /**
   * @brief The shape drawn at each data point when markers are visible.
   *
   * @SINCE_3_0.0
   * @see SetMarkersVisible(), SetMarkerShape()
   */
  enum class MarkerShape
  {
    CIRCLE,   ///< Circular marker. This is the default. @SINCE_3_0.0
    SQUARE,   ///< Square marker. @SINCE_3_0.0
    TRIANGLE, ///< Triangular marker. @SINCE_3_0.0
    DIAMOND   ///< Diamond-shaped marker. @SINCE_3_0.0
  };

  /**
   * @brief Creates an uninitialized LineSeries handle.
   * @SINCE_3_0.0
   */
  LineSeries();

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~LineSeries();

  /**
   * @brief Copy constructor.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to copy
   */
  LineSeries(const LineSeries& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to move
   */
  LineSeries(LineSeries&& rhs);

  /**
   * @brief Copy assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  LineSeries& operator=(const LineSeries& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  LineSeries& operator=(LineSeries&& rhs);

  /**
   * @brief Creates a new LineSeries.
   *
   * @SINCE_3_0.0
   * @return A handle to the new series
   */
  static LineSeries New();

  /**
   * @brief Downcasts a handle to a LineSeries handle.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to downcast
   * @return A LineSeries handle, or an uninitialized handle if the cast fails
   */
  static LineSeries DownCast(BaseHandle handle);

  /**
   * @brief Sets the width of the line, in pixels.
   *
   * The default is 2.5.
   *
   * @SINCE_3_0.0
   * @param[in] width The width of the line, in pixels
   */
  void SetLineWidth(float width);

  /**
   * @brief Retrieves the width of the line, in pixels.
   *
   * @SINCE_3_0.0
   * @return The width of the line, in pixels
   */
  float GetLineWidth() const;

  /**
   * @brief Sets how much the line curves through its data points, instead of using straight segments.
   *
   * Clamped to [0, 1], where 0 draws straight segments and 1 is maximally
   * smoothed. The default is 0.0.
   *
   * @SINCE_3_0.0
   * @param[in] value The smoothing amount
   */
  void SetSmoothness(float value);

  /**
   * @brief Retrieves how much the line curves through its data points.
   *
   * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   * @param[in] color The color of the line
   */
  void SetColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the line.
   *
   * @SINCE_3_0.0
   * @return The color of the line
   */
  Vector4 GetColor() const;

  /**
   * @brief Sets whether a marker is drawn at each data point.
   *
   * The default is true.
   *
   * @SINCE_3_0.0
   * @param[in] visible True to draw markers
   */
  void SetMarkersVisible(bool visible);

  /**
   * @brief Retrieves whether a marker is drawn at each data point.
   *
   * @SINCE_3_0.0
   * @return True if markers are drawn
   */
  bool IsMarkersVisible() const;

  /**
   * @brief Sets the radius of each marker, in pixels.
   *
   * The default is 4.0.
   *
   * @SINCE_3_0.0
   * @param[in] radius The marker radius, in pixels
   */
  void SetMarkerRadius(float radius);

  /**
   * @brief Retrieves the radius of each marker, in pixels.
   *
   * @SINCE_3_0.0
   * @return The marker radius, in pixels
   */
  float GetMarkerRadius() const;

  /**
   * @brief Sets the shape drawn for each marker.
   *
   * The default is MarkerShape::CIRCLE.
   *
   * @SINCE_3_0.0
   * @param[in] shape The marker shape
   */
  void SetMarkerShape(MarkerShape shape);

  /**
   * @brief Retrieves the shape drawn for each marker.
   *
   * @SINCE_3_0.0
   * @return The marker shape
   */
  MarkerShape GetMarkerShape() const;

  /**
   * @brief Sets the fill color of each marker.
   *
   * Until this is called, markers are filled with the line color set by
   * SetColor().
   *
   * @SINCE_3_0.0
   * @param[in] color The marker fill color
   */
  void SetMarkerColor(const Vector4& color);

  /**
   * @brief Retrieves the fill color of each marker.
   *
   * @SINCE_3_0.0
   * @return The marker fill color, either the color set by SetMarkerColor()
   *         or the line color if it was not called
   */
  Vector4 GetMarkerColor() const;

  /**
   * @brief Sets the border color of each marker.
   *
   * The default is fully transparent, meaning no border is visible.
   *
   * @SINCE_3_0.0
   * @param[in] color The marker border color
   */
  void SetMarkerBorderColor(const Vector4& color);

  /**
   * @brief Retrieves the border color of each marker.
   *
   * @SINCE_3_0.0
   * @return The marker border color
   */
  Vector4 GetMarkerBorderColor() const;

  /**
   * @brief Sets the border width of each marker, in pixels.
   *
   * The default is 0.0.
   *
   * @SINCE_3_0.0
   * @param[in] width The marker border width, in pixels
   */
  void SetMarkerBorderWidth(float width);

  /**
   * @brief Retrieves the border width of each marker, in pixels.
   *
   * @SINCE_3_0.0
   * @return The marker border width, in pixels
   */
  float GetMarkerBorderWidth() const;

  /**
   * @brief Sets whether a text label is drawn at each data point.
   *
   * The default is false.
   *
   * @SINCE_3_0.0
   * @param[in] visible True to draw data labels
   */
  void SetDataLabelsVisible(bool visible);

  /**
   * @brief Retrieves whether a text label is drawn at each data point.
   *
   * @SINCE_3_0.0
   * @return True if data labels are drawn
   */
  bool IsDataLabelsVisible() const;

  /**
   * @brief Sets the printf-style format string used to render each data label's Y value.
   *
   * Ignored while a formatter set with SetDataLabelFormatter() is active.
   * The default is "%.1f".
   *
   * @SINCE_3_0.0
   * @param[in] format The printf-style format string
   */
  void SetDataLabelFormat(const Dali::String& format);

  /**
   * @brief Retrieves the printf-style format string used to render each data label's Y value.
   *
   * @SINCE_3_0.0
   * @return The printf-style format string
   */
  Dali::String GetDataLabelFormat() const;

  /**
   * @brief Sets the color of the data label text.
   *
   * The default is (0.2, 0.2, 0.2, 1.0).
   *
   * @SINCE_3_0.0
   * @param[in] color The color of the data label text
   */
  void SetDataLabelColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the data label text.
   *
   * @SINCE_3_0.0
   * @return The color of the data label text
   */
  Vector4 GetDataLabelColor() const;

  /**
   * @brief Sets the point size of the data label text.
   *
   * The default is 8.0.
   *
   * @SINCE_3_0.0
   * @param[in] pointSize The point size of the data label text
   */
  void SetDataLabelSize(float pointSize);

  /**
   * @brief Retrieves the point size of the data label text.
   *
   * @SINCE_3_0.0
   * @return The point size of the data label text
   */
  float GetDataLabelSize() const;

  /**
   * @brief Sets whether the area under the line is filled.
   *
   * Used to build AREA charts from a LINE-family series. The default is false.
   *
   * @SINCE_3_0.0
   * @param[in] enabled True to fill the area under the line
   */
  void SetFillEnabled(bool enabled);

  /**
   * @brief Retrieves whether the area under the line is filled.
   *
   * @SINCE_3_0.0
   * @return True if the area under the line is filled
   */
  bool IsFillEnabled() const;

  /**
   * @brief Sets the fill color of the area under the line.
   *
   * Until this is called, the fill uses the line color set by SetColor()
   * at 20% opacity.
   *
   * @SINCE_3_0.0
   * @param[in] color The fill color
   */
  void SetFillColor(const Vector4& color);

  /**
   * @brief Retrieves the fill color of the area under the line.
   *
   * @SINCE_3_0.0
   * @return The fill color, either the color set by SetFillColor() or the
   *         line color at 20% opacity if it was not called
   */
  Vector4 GetFillColor() const;

  /**
   * @brief Function pointer type for building the text of a data label.
   *
   * @SINCE_3_0.0
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
   * @SINCE_3_0.0
   * @param[in] formatter The formatter, or nullptr to use the data label format
   */
  void SetDataLabelFormatter(DataLabelFormatterType formatter);

  /**
   * @brief Retrieves the formatter used to build the text of each data label.
   *
   * @SINCE_3_0.0
   * @return The formatter, or nullptr if the data label format is used
   */
  DataLabelFormatterType GetDataLabelFormatter() const;

  /**
   * @brief Sets a linear gradient for the filled area.
   * @SINCE_3_0.0
   * @param[in] gradient The gradient in plot coordinates
   */
  void SetFillGradient(const Gradient::Linear& gradient);

  /**
   * @brief Restores the solid area fill.
   * @SINCE_3_0.0
   */
  void ClearFillGradient();

  /**
   * @brief Sets a linear gradient for the line stroke.
   * @SINCE_3_0.0
   * @param[in] gradient The gradient in plot coordinates
   */
  void SetStrokeGradient(const Gradient::Linear& gradient);

  /**
   * @brief Restores the solid line stroke.
   * @SINCE_3_0.0
   */
  void ClearStrokeGradient();

  /**
   * @brief Sets the dash pattern; a zero gap produces a solid stroke.
   * @SINCE_3_0.0
   * @param[in] dashLength Finite positive dash length in pixels
   * @param[in] gapLength Finite non-negative gap length in pixels
   */
  void SetLineDash(float dashLength, float gapLength);

  /**
   * @brief Overrides edges [firstPoint, lastPoint) using original point indices.
   *
   * Later overlapping overrides win. NaN gaps remain disconnected.
   * @SINCE_3_0.0
   * @param[in] firstPoint The first point index
   * @param[in] lastPoint The final point index
   * @param[in] color The stroke color; theme identifiers are retained
   * @param[in] dashLength The dash length in pixels
   * @param[in] gapLength The gap length, or zero for solid
   */
  void SetSegmentStyle(uint32_t firstPoint, uint32_t lastPoint, const UiColor& color, float dashLength = 5.0f, float gapLength = 0.0f);

  /**
   * @brief Removes all per-segment overrides.
   * @SINCE_3_0.0
   */
  void ClearSegmentStyles();

public:
  explicit LineSeries(Integration::LineSeries* impl);
};

} // namespace Ui
} //namespace DALI_NAMESPACE
