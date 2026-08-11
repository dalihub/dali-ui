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
#include <dali-ui-components/public-api/chart/chart-axis.h>
#include <dali-ui-components/public-api/chart/chart-section.h>
#include <dali-ui-components/public-api/chart/chart-series.h>
#include <dali-ui-components/public-api/chart/chart-view-properties.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
namespace Ui
{

namespace Integration DALI_INTERNAL
{
class ChartViewImpl;
}

/**
 * @brief Event data passed to chart point interaction signals.
 *
 * @code
 * chart.DataPointSelectedSignal().Connect(this, [](ChartView, const ChartPointEventArgs& e) {
 *   ... e.GetSeriesIndex(), e.GetDataY(), e.GetXLabel() ...
 * });
 * @endcode
 */
class DALI_UI_COMPONENTS_API ChartPointEventArgs
{
public:
  /**
   * @brief Creates empty event data.
   */
  ChartPointEventArgs();

  /**
   * @brief Creates event data describing a single chart point.
   *
   * @param[in] seriesIndex The index of the series the point belongs to
   * @param[in] pointIndex The index of the point within the series
   * @param[in] dataX The X value of the point in data coordinates
   * @param[in] dataY The Y value of the point in data coordinates
   * @param[in] seriesName The name of the series the point belongs to
   * @param[in] xLabel The X axis label matching the point
   */
  ChartPointEventArgs(int                 seriesIndex,
                      int                 pointIndex,
                      float               dataX,
                      float               dataY,
                      const Dali::String& seriesName,
                      const Dali::String& xLabel);

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The object to copy
   */
  ChartPointEventArgs(const ChartPointEventArgs& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The object to move
   */
  ChartPointEventArgs(ChartPointEventArgs&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The object to copy
   * @return A reference to this object
   */
  ChartPointEventArgs& operator=(const ChartPointEventArgs& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The object to move
   * @return A reference to this object
   */
  ChartPointEventArgs& operator=(ChartPointEventArgs&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~ChartPointEventArgs();

  /**
   * @brief Retrieves the index of the series the point belongs to.
   * @return The series index, or -1 if not set
   */
  int GetSeriesIndex() const;

  /**
   * @brief Retrieves the index of the point within the series.
   * @return The point index, or -1 if not set
   */
  int GetPointIndex() const;

  /**
   * @brief Retrieves the X value of the point in data coordinates.
   * @return The X value
   */
  float GetDataX() const;

  /**
   * @brief Retrieves the Y value of the point in data coordinates.
   * @return The Y value
   */
  float GetDataY() const;

  /**
   * @brief Retrieves the name of the series the point belongs to.
   * @return The series name
   */
  const Dali::String& GetSeriesName() const;

  /**
   * @brief Retrieves the X axis label matching the point.
   * @return The X axis label
   */
  const Dali::String& GetXLabel() const;

private:
  struct Impl;
  UniquePtr<Impl> mImpl;
};

/**
 * @brief ChartView displays line, bar, pie, area, scatter, and gauge charts.
 *
 * Uses multiple CanvasView layers (ThorVG) for rendering and Ui::Label
 * actors for text elements (axis labels, title, legend, tooltip).
 *
 * @code
 *   auto chart = ChartView::New(ChartView::Type::LINE, Vector2(600, 400));
 *   chart.AddSeries(lineSeries);
 *   parent.Add(chart);
 * @endcode
 */
class DALI_UI_COMPONENTS_API ChartView : public View
{
public:
  /**
   * @brief The kind of chart drawn, chosen when the ChartView is created.
   */
  enum class Type
  {
    LINE,    ///< Line chart, built from LineSeries.
    BAR,     ///< Bar chart, built from BarSeries.
    PIE,     ///< Pie or donut chart, built from PieSeries.
    AREA,    ///< Area chart — equivalent to LINE with FillEnabled on each LineSeries.
    SCATTER, ///< Scatter chart, built from ScatterSeries.
    GAUGE    ///< Gauge chart, configured with the SetGauge*() family of methods.
  };

  /**
   * @brief Where the legend is drawn relative to the plot area.
   */
  enum class LegendPosition
  {
    NONE   = 0, ///< No legend is drawn.
    TOP    = 1, ///< Legend is drawn above the plot area. This is the default.
    BOTTOM = 2, ///< Legend is drawn below the plot area.
    LEFT   = 3, ///< Legend is drawn to the left of the plot area.
    RIGHT  = 4  ///< Legend is drawn to the right of the plot area.
  };

  /**
   * @brief Where the title is drawn relative to the plot area.
   */
  enum class TitlePosition
  {
    TOP_CENTER    = 0, ///< Centered above the plot area. This is the default.
    TOP_LEFT      = 1, ///< Left-aligned above the plot area.
    TOP_RIGHT     = 2, ///< Right-aligned above the plot area.
    BOTTOM_CENTER = 3, ///< Centered below the plot area.
  };

  /**
   * @brief The easing curve used by data update animations.
   *
   * @see SetAnimationDuration()
   */
  enum class EasingType
  {
    LINEAR      = 0, ///< Constant rate of change.
    EASE_OUT    = 1, ///< Starts fast, decelerates towards the end. This is the default.
    EASE_IN_OUT = 2, ///< Accelerates at the start, decelerates towards the end.
  };

  /**
   * @brief How touch/hover input is matched to a data point.
   *
   * @see SetFindingStrategy()
   */
  enum class FindingStrategy
  {
    NEAREST,          ///< Selects the single nearest point across all series. This is the default.
    SAME_X,           ///< Selects one point per series that shares the nearest X value.
    SAME_X_NEAREST_Y, ///< Same as SAME_X, but only for series whose Y value is also close to the touch position.
  };

  /**
   * @brief Bit flags selecting which zoom/pan gestures are enabled.
   *
   * Combine with bitwise OR and pass to SetZoomMode(). For example
   * `SetZoomMode(static_cast<int>(ZoomMode::PAN_X) | static_cast<int>(ZoomMode::ZOOM_X))`
   * enables horizontal panning and zooming only.
   */
  enum class ZoomMode : int
  {
    NONE   = 0,      ///< No zoom or pan gestures are enabled. This is the default.
    PAN_X  = 1 << 0, ///< Enables horizontal panning by dragging.
    PAN_Y  = 1 << 1, ///< Enables vertical panning by dragging.
    ZOOM_X = 1 << 2, ///< Enables horizontal zooming by pinch or mouse wheel.
    ZOOM_Y = 1 << 3, ///< Enables vertical zooming by pinch or mouse wheel.
  };

  /**
   * @brief Animatable and queryable Dali::Property indices of ChartView.
   *
   * These mirror several of the SetXxx()/GetXxx() methods below as properties,
   * for use with Animation and property notifications.
   */
  struct Property
  {
    enum
    {
      SHOW_GRID          = ChartViewPropertyIndex::SHOW_GRID,
      SHOW_LEGEND        = ChartViewPropertyIndex::SHOW_LEGEND,
      SHOW_TOOLTIP       = ChartViewPropertyIndex::SHOW_TOOLTIP,
      BACKGROUND_COLOR   = ChartViewPropertyIndex::BACKGROUND_COLOR,
      GRID_COLOR         = ChartViewPropertyIndex::GRID_COLOR,
      ANIMATION_DURATION = ChartViewPropertyIndex::ANIMATION_DURATION,
      Y_AXIS_AUTO_RANGE  = ChartViewPropertyIndex::Y_AXIS_AUTO_RANGE,
      LEGEND_POSITION    = ChartViewPropertyIndex::LEGEND_POSITION,
      AXIS_LABEL_SIZE    = ChartViewPropertyIndex::AXIS_LABEL_SIZE,
      TITLE_SIZE         = ChartViewPropertyIndex::TITLE_SIZE,
      LINE_WIDTH         = ChartViewPropertyIndex::LINE_WIDTH,
      SHOW_MARKERS       = ChartViewPropertyIndex::SHOW_MARKERS,
      MARKER_RADIUS      = ChartViewPropertyIndex::MARKER_RADIUS,
      HOVER_ENABLED      = ChartViewPropertyIndex::HOVER_ENABLED,
      TOUCH_ENABLED      = ChartViewPropertyIndex::TOUCH_ENABLED,
    };
  };

  /**
   * @brief Creates an uninitialized ChartView handle.
   */
  ChartView();

  /**
   * @brief Destructor.
   */
  ~ChartView();

  /**
   * @brief Creates a new ChartView.
   *
   * @param[in] type The kind of chart to draw
   * @param[in] size The initial size of the chart, in pixels
   * @return A handle to the new ChartView
   */
  static ChartView New(Type type, const Vector2& size);

  /**
   * @brief Downcasts a handle to a ChartView handle.
   *
   * @param[in] handle The handle to downcast
   * @return A ChartView handle, or an uninitialized handle if the cast fails
   */
  static ChartView DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   *
   * @param[in] rhs The handle to copy
   */
  ChartView(const ChartView& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs The handle to move
   */
  ChartView(ChartView&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] rhs The handle to copy
   * @return A reference to this handle
   */
  ChartView& operator=(const ChartView& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  ChartView& operator=(ChartView&& rhs) noexcept;

  DALI_UI_VIEW_WITH(ChartView)

  /**
   * @brief Adds a series to the chart.
   *
   * The series is drawn using the type-specific style already configured on
   * it (color, markers, and so on). Adding a series recomputes the
   * auto-ranged axis limits if either axis has IsAutoRange() enabled.
   *
   * @param[in] series The series to add
   */
  void AddSeries(Ui::ChartSeries series);

  /**
   * @brief Assigns the X axis of the chart.
   *
   * Recomputes the auto-ranged axis limits if either axis has IsAutoRange()
   * enabled.
   *
   * @param[in] axis The axis to use as the X axis
   */
  void SetXAxis(Ui::ChartAxis axis);

  /**
   * @brief Assigns the Y axis of the chart.
   *
   * Recomputes the auto-ranged axis limits if either axis has IsAutoRange()
   * enabled.
   *
   * @param[in] axis The axis to use as the Y axis
   */
  void SetYAxis(Ui::ChartAxis axis);

  /**
   * @brief Removes the first series with a matching name.
   *
   * @param[in] name The name of the series to remove, as set with ChartSeries::SetName()
   * @return True if a series with a matching name was found and removed
   */
  bool RemoveSeries(const Dali::String& name);

  /**
   * @brief Removes all series from the chart.
   */
  void RemoveAllSeries();

  /**
   * @brief Sets the chart title text.
   *
   * An empty title (the default) draws nothing.
   *
   * @param[in] title The chart title
   */
  void SetTitle(const Dali::String& title);

  /**
   * @brief Retrieves the chart title text.
   *
   * @return The chart title
   */
  Dali::String GetTitle() const;

  /**
   * @brief Sets where the title is drawn relative to the plot area.
   *
   * The default is TitlePosition::TOP_CENTER.
   *
   * @param[in] position Where to draw the title
   */
  void SetTitlePosition(TitlePosition position);

  /**
   * @brief Retrieves where the title is drawn relative to the plot area.
   *
   * @return Where the title is drawn
   */
  TitlePosition GetTitlePosition() const;

  /**
   * @brief Sets the color of the title text.
   *
   * The default is (0.1, 0.1, 0.1, 1.0).
   *
   * @param[in] color The color of the title text
   */
  void SetTitleColor(const Vector4& color);

  /**
   * @brief Retrieves the color of the title text.
   *
   * @return The color of the title text
   */
  Vector4 GetTitleColor() const;

  /**
   * @brief Function pointer type for building tooltip text.
   *
   * @note This is a plain function pointer, not std::function.
   *       Capturing lambdas and non-static member functions are not accepted.
   *       Use a free function, a static function, or a captureless lambda.
   *       If state is required, route through an application singleton.
   *
   * @param[in] seriesName The name of the series under the touch point
   * @param[in] xLabel The X axis label matching the touch point
   * @param[in] dataY The Y value of the point in data coordinates
   * @return The tooltip text to display
   */
  using TooltipFormatterType = Dali::String (*)(const Dali::String& seriesName,
                                                const Dali::String& xLabel,
                                                float               dataY);

  /**
   * @brief Sets the formatter used to build tooltip text.
   *
   * @param[in] formatter The formatter, or nullptr to restore the default text
   */
  void SetTooltipFormatter(TooltipFormatterType formatter);

  /**
   * @brief Sets the duration of the animation played when series data changes.
   *
   * Negative values are clamped to 0. A duration of 0 (the default) disables
   * the animation; data updates apply instantly.
   *
   * @param[in] milliseconds The animation duration, in milliseconds
   */
  void SetAnimationDuration(float milliseconds);

  /**
   * @brief Retrieves the duration of the animation played when series data changes.
   *
   * @return The animation duration, in milliseconds
   */
  float GetAnimationDuration() const;

  /**
   * @brief Sets the easing curve used by data update animations.
   *
   * The default is EasingType::EASE_OUT.
   *
   * @param[in] easing The easing curve
   */
  void SetAnimationEasing(EasingType easing);

  /**
   * @brief Retrieves the easing curve used by data update animations.
   *
   * @return The easing curve
   */
  EasingType GetAnimationEasing() const;

  /**
   * @brief Sets the minimum time between layout/data rebuilds triggered by rapid updates.
   *
   * Negative values are clamped to 0. Values below 1.0 (the default, 0.0)
   * disable throttling, so every update rebuilds immediately.
   *
   * @param[in] milliseconds The minimum time between rebuilds, in milliseconds
   */
  void SetUpdateThrottle(float milliseconds);

  /**
   * @brief Retrieves the minimum time between layout/data rebuilds triggered by rapid updates.
   *
   * @return The minimum time between rebuilds, in milliseconds
   */
  float GetUpdateThrottle() const;

  /**
   * @brief Sets the maximum distance from a touch/hover position to a data point still counted as a hit.
   *
   * The default is 30.0.
   *
   * @param[in] pixels The maximum hit distance, in pixels
   */
  void SetHitThreshold(float pixels);

  /**
   * @brief Retrieves the maximum distance from a touch/hover position to a data point still counted as a hit.
   *
   * @return The maximum hit distance, in pixels
   */
  float GetHitThreshold() const;

  /**
   * @brief Sets how touch/hover input is matched to a data point.
   *
   * The default is FindingStrategy::NEAREST.
   *
   * @param[in] strategy The hit-testing strategy
   */
  void SetFindingStrategy(FindingStrategy strategy);

  /**
   * @brief Retrieves how touch/hover input is matched to a data point.
   *
   * @return The hit-testing strategy
   */
  FindingStrategy GetFindingStrategy() const;

  /**
   * @brief Sets whether tapping a legend item toggles that series' visibility.
   *
   * The default is true.
   *
   * @param[in] enabled True to toggle series visibility on legend tap
   */
  void SetLegendToggleEnabled(bool enabled);

  /**
   * @brief Retrieves whether tapping a legend item toggles that series' visibility.
   *
   * @return True if series visibility is toggled on legend tap
   */
  bool IsLegendToggleEnabled() const;

  /**
   * @brief Sets which zoom/pan gestures are enabled on the chart.
   *
   * Pass a bitwise OR combination of ZoomMode values, cast to int. The
   * default is ZoomMode::NONE (all gestures disabled).
   *
   * @param[in] zoomModeFlags The enabled gestures, as a combination of ZoomMode flags
   */
  void SetZoomMode(int zoomModeFlags);

  /**
   * @brief Retrieves which zoom/pan gestures are enabled on the chart.
   *
   * @return The enabled gestures, as a combination of ZoomMode flags
   */
  int GetZoomMode() const;

  /**
   * @brief Resets the viewport to the full auto-ranged data extents.
   *
   * Also re-enables auto-range on both axes.
   */
  void ResetZoom();

  /**
   * @brief Sets whether panning/zooming is clamped to the full data range.
   *
   * When enabled, the viewport cannot pan or zoom out past the extents of the
   * series data. The default is true.
   *
   * @param[in] enabled True to clamp the viewport to the data range
   */
  void SetZoomClampEnabled(bool enabled);

  /**
   * @brief Retrieves whether panning/zooming is clamped to the full data range.
   *
   * @return True if the viewport is clamped to the data range
   */
  bool IsZoomClampEnabled() const;

  /**
   * @brief Sets whether the Y axis range is refit to the visible data while panning/zooming on X.
   *
   * The default is false.
   *
   * @param[in] enabled True to refit the Y range to the visible data
   */
  void SetAutoFitYOnPanEnabled(bool enabled);

  /**
   * @brief Retrieves whether the Y axis range is refit to the visible data while panning/zooming on X.
   *
   * @return True if the Y range is refit to the visible data
   */
  bool IsAutoFitYOnPanEnabled() const;

  /**
   * @brief Adds a highlighted region to the chart.
   *
   * @param[in] section The section to add
   * @see ChartSection
   */
  void AddSection(ChartSection section);

  /**
   * @brief Removes a previously added section.
   *
   * @param[in] section The section to remove
   */
  void RemoveSection(ChartSection section);

  /**
   * @brief Removes all sections from the chart.
   */
  void ClearSections();

  using DataPointSelectedSignalType  = Signal<void(ChartView, const ChartPointEventArgs&)>;
  using LegendItemTappedSignalType   = Signal<void(ChartView, int, bool)>;
  using MultiPointSelectedSignalType = Signal<void(ChartView, const ChartPointEventArgs&)>;
  using ZoomedSignalType             = Signal<void(ChartView, const Dali::Bounds&)>;

  /**
   * @brief Signal emitted when a single data point is selected by touch/hover.
   *
   * Emitted only while FindingStrategy is NEAREST, or for a pie/donut slice tap.
   * The first argument is the chart that emitted the signal.
   *
   * @return The signal to connect to
   */
  DataPointSelectedSignalType& DataPointSelectedSignal();

  /**
   * @brief Signal emitted when a legend item is tapped.
   *
   * The first argument is the chart that emitted the signal; the second is
   * the index of the tapped series; the third is the series' new visibility
   * state.
   *
   * @return The signal to connect to
   */
  LegendItemTappedSignalType& LegendItemTappedSignal();

  /**
   * @brief Signal emitted once per series when multiple points are selected by touch/hover.
   *
   * Emitted only while FindingStrategy is SAME_X or SAME_X_NEAREST_Y. The
   * first argument is the chart that emitted the signal.
   *
   * @return The signal to connect to
   */
  MultiPointSelectedSignalType& MultiPointSelectedSignal();

  /**
   * @brief Signal emitted when the visible viewport changes from panning or zooming.
   *
   * The first argument is the chart that emitted the signal. The second is
   * the new visible viewport in data coordinates: Bounds::x is the minimum X
   * value, Bounds::y is the minimum Y value, and Bounds::Right()/Bounds::Bottom()
   * give the maximum X/Y values.
   *
   * @return The signal to connect to
   */
  ZoomedSignalType& ZoomedSignal();

  /**
   * @brief Sets the current value shown by a GAUGE chart.
   *
   * Not clamped to [GetGaugeMinimumValue(), GetGaugeMaximumValue()] by this call; the
   * progress arc rendering clamps it. The default is 0.0.
   *
   * @param[in] value The current gauge value
   */
  void SetGaugeValue(float value);

  /**
   * @brief Retrieves the current value shown by a GAUGE chart.
   *
   * @return The current gauge value
   */
  float GetGaugeValue() const;

  /**
   * @brief Sets the value at the start of a GAUGE chart's arc.
   *
   * The default is 0.0.
   *
   * @param[in] value The minimum gauge value
   */
  void SetGaugeMinimumValue(float value);

  /**
   * @brief Retrieves the value at the start of a GAUGE chart's arc.
   *
   * @return The minimum gauge value
   */
  float GetGaugeMinimumValue() const;

  /**
   * @brief Sets the value at the end of a GAUGE chart's arc.
   *
   * The default is 100.0.
   *
   * @param[in] value The maximum gauge value
   */
  void SetGaugeMaximumValue(float value);

  /**
   * @brief Retrieves the value at the end of a GAUGE chart's arc.
   *
   * @return The maximum gauge value
   */
  float GetGaugeMaximumValue() const;

  /**
   * @brief Sets the angular span of a GAUGE chart's arc.
   *
   * Clamped to (0.1, 360]. The default is 270.0.
   *
   * @param[in] degrees The arc span, in degrees
   */
  void SetGaugeArcSpan(float degrees);

  /**
   * @brief Retrieves the angular span of a GAUGE chart's arc.
   *
   * @return The arc span, in degrees
   */
  float GetGaugeArcSpan() const;

  /**
   * @brief Sets the starting angle of a GAUGE chart's arc.
   *
   * 0 degrees points right, increasing clockwise. The default is 135.0.
   *
   * @param[in] degrees The starting angle, in degrees
   */
  void SetGaugeStartAngle(float degrees);

  /**
   * @brief Retrieves the starting angle of a GAUGE chart's arc.
   *
   * @return The starting angle, in degrees
   */
  float GetGaugeStartAngle() const;

  /**
   * @brief Sets the thickness of a GAUGE chart's arc, as a fraction of its outer radius.
   *
   * Clamped to [0.01, 0.5]. The default is 0.18.
   *
   * @param[in] ratio The arc thickness, as a fraction of the outer radius
   */
  void SetGaugeArcWidth(float ratio);

  /**
   * @brief Retrieves the thickness of a GAUGE chart's arc, as a fraction of its outer radius.
   *
   * @return The arc thickness, as a fraction of the outer radius
   */
  float GetGaugeArcWidth() const;

  /**
   * @brief Sets the color of a GAUGE chart's background track.
   *
   * The default is (0.85, 0.85, 0.85, 1.0).
   *
   * @param[in] color The color of the background track
   */
  void SetGaugeTrackColor(const Vector4& color);

  /**
   * @brief Retrieves the color of a GAUGE chart's background track.
   *
   * @return The color of the background track
   */
  Vector4 GetGaugeTrackColor() const;

  /**
   * @brief Sets the color of a GAUGE chart's progress arc, drawn from the start angle up to the current value.
   *
   * The default is (0.27, 0.51, 0.71, 1.0).
   *
   * @param[in] color The color of the progress arc
   */
  void SetGaugeProgressColor(const Vector4& color);

  /**
   * @brief Retrieves the color of a GAUGE chart's progress arc.
   *
   * @return The color of the progress arc
   */
  Vector4 GetGaugeProgressColor() const;

  /**
   * @brief Sets the text shown at the center of a GAUGE chart.
   *
   * An empty label (the default) shows the current value formatted as a
   * percentage of the min/max range instead.
   *
   * @param[in] text The center label text
   */
  void SetGaugeCenterLabel(const Dali::String& text);

  /**
   * @brief Retrieves the text shown at the center of a GAUGE chart.
   *
   * @return The center label text
   */
  Dali::String GetGaugeCenterLabel() const;

  /**
   * @brief Adds a colored zone to a GAUGE chart's track, spanning a sub-range of the gauge value.
   *
   * fromValue/toValue are clamped to [GetGaugeMinimumValue(), GetGaugeMaximumValue()]
   * when drawn. Zones are drawn in the order they were added, so a later zone
   * overlaps an earlier one where their ranges intersect.
   *
   * @param[in] fromValue The start of the zone, in gauge value units
   * @param[in] toValue The end of the zone, in gauge value units
   * @param[in] color The color of the zone
   */
  void AddGaugeRange(float fromValue, float toValue, const Vector4& color);

  /**
   * @brief Removes all colored zones added with AddGaugeRange().
   */
  void ClearGaugeRanges();

public:
  /// @cond internal
  explicit ChartView(Integration::ChartViewImpl& implementation);
  explicit ChartView(Dali::Internal::CustomActor* internal);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
