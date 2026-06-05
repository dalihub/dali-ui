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
#include <dali/public-api/signals/dali-signal.h>
#include <functional>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/chart/chart-axis.h>
#include <dali-ui-components/public-api/chart/chart-section.h>
#include <dali-ui-components/public-api/chart/chart-series.h>
#include <dali-ui-components/public-api/chart/chart-view-properties.h>
#include <dali-ui-foundation/public-api/view.h>

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
 */
struct ChartPointEventArgs
{
  int          seriesIndex{-1};
  int          pointIndex{-1};
  float        dataX{0.0f};
  float        dataY{0.0f};
  Dali::String seriesName;
  Dali::String xLabel;
};

/**
 * @brief Viewport extents emitted by ZoomedSignal (data coordinates).
 */
struct ChartViewportArgs
{
  float xMin{0.0f};
  float xMax{1.0f};
  float yMin{0.0f};
  float yMax{1.0f};
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
class DALI_UI_API ChartView : public View
{
public:
  enum class Type
  {
    LINE,
    BAR,
    PIE,
    AREA, ///< Area chart — equivalent to LINE with FillEnabled on each LineSeries.
    SCATTER,
    GAUGE
  };

  enum class LegendPosition
  {
    NONE   = 0,
    TOP    = 1,
    BOTTOM = 2,
    LEFT   = 3,
    RIGHT  = 4
  };

  enum class TitlePosition
  {
    TOP_CENTER    = 0,
    TOP_LEFT      = 1,
    TOP_RIGHT     = 2,
    BOTTOM_CENTER = 3,
  };

  enum class EasingType
  {
    LINEAR      = 0,
    EASE_OUT    = 1,
    EASE_IN_OUT = 2,
  };

  enum class FindingStrategy
  {
    NEAREST,
    SAME_X,
    SAME_X_NEAREST_Y,
  };

  enum class ZoomMode : int
  {
    NONE   = 0,
    PAN_X  = 1 << 0,
    PAN_Y  = 1 << 1,
    ZOOM_X = 1 << 2,
    ZOOM_Y = 1 << 3,
  };

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

  // =========================================================================
  // Construction
  // =========================================================================

  ChartView();
  ~ChartView();

  static ChartView New(Type type, const Vector2& size);
  static ChartView DownCast(BaseHandle handle);

  ChartView(const ChartView& rhs);
  ChartView(ChartView&& rhs) noexcept;
  ChartView& operator=(const ChartView& rhs);
  ChartView& operator=(ChartView&& rhs) noexcept;

  // =========================================================================
  // Series Management
  // =========================================================================

  ChartView& AddSeries(Ui::ChartSeries series);
  ChartView& SetXAxis(Ui::ChartAxis axis);
  ChartView& SetYAxis(Ui::ChartAxis axis);
  bool       RemoveSeries(const Dali::String& name);
  ChartView& RemoveAllSeries();

  // =========================================================================
  // Title
  // =========================================================================

  ChartView&   SetTitle(const Dali::String& title);
  Dali::String GetTitle() const;

  ChartView&    SetTitlePosition(TitlePosition position);
  TitlePosition GetTitlePosition() const;

  ChartView& SetTitleColor(const Vector4& color);
  Vector4    GetTitleColor() const;

  // =========================================================================
  // Tooltip formatter
  // =========================================================================

  using TooltipFormatterType = std::function<Dali::String(const Dali::String& seriesName,
                                                          const Dali::String& xLabel,
                                                          float               dataY)>;
  ChartView& SetTooltipFormatter(TooltipFormatterType formatter);

  // =========================================================================
  // Animation
  // =========================================================================

  ChartView& SetAnimationDuration(float milliseconds);
  float      GetAnimationDuration() const;

  ChartView& SetAnimationEasing(EasingType easing);
  EasingType GetAnimationEasing() const;

  ChartView& SetUpdateThrottle(float milliseconds);
  float      GetUpdateThrottle() const;

  // =========================================================================
  // Hit testing
  // =========================================================================

  ChartView& SetHitThreshold(float pixels);
  float      GetHitThreshold() const;

  ChartView&      SetFindingStrategy(FindingStrategy strategy);
  FindingStrategy GetFindingStrategy() const;

  // =========================================================================
  // Interaction control
  // =========================================================================

  ChartView& SetLegendToggleEnabled(bool enabled);
  bool       GetLegendToggleEnabled() const;

  // =========================================================================
  // Zoom / Pan
  // =========================================================================

  ChartView& SetZoomMode(int zoomModeFlags);
  int        GetZoomMode() const;

  ChartView& ResetZoom();

  ChartView& SetZoomClampEnabled(bool enabled);
  bool       IsZoomClampEnabled() const;

  ChartView& SetAutoFitYOnPan(bool enabled);
  bool       IsAutoFitYOnPan() const;

  // =========================================================================
  // Sections
  // =========================================================================

  ChartView& AddSection(ChartSection section);
  ChartView& RemoveSection(ChartSection section);
  ChartView& ClearSections();

  // =========================================================================
  // Signals
  // =========================================================================

  using DataPointSelectedSignalType  = Signal<void(const ChartPointEventArgs&)>;
  using LegendItemTappedSignalType   = Signal<void(int, bool)>;
  using MultiPointSelectedSignalType = Signal<void(const ChartPointEventArgs&)>;
  using ZoomedSignalType             = Signal<void(const ChartViewportArgs&)>;

  DataPointSelectedSignalType&  DataPointSelectedSignal();
  LegendItemTappedSignalType&   LegendItemTappedSignal();
  MultiPointSelectedSignalType& MultiPointSelectedSignal();
  ZoomedSignalType&             ZoomedSignal();

  // =========================================================================
  // Gauge
  // =========================================================================

  ChartView& SetGaugeValue(float value);
  float      GetGaugeValue() const;

  ChartView& SetGaugeMinValue(float value);
  float      GetGaugeMinValue() const;

  ChartView& SetGaugeMaxValue(float value);
  float      GetGaugeMaxValue() const;

  ChartView& SetGaugeArcSpan(float degrees);
  float      GetGaugeArcSpan() const;

  ChartView& SetGaugeStartAngle(float degrees);
  float      GetGaugeStartAngle() const;

  ChartView& SetGaugeArcWidth(float ratio);
  float      GetGaugeArcWidth() const;

  ChartView& SetGaugeTrackColor(const Vector4& color);
  Vector4    GetGaugeTrackColor() const;

  ChartView& SetGaugeProgressColor(const Vector4& color);
  Vector4    GetGaugeProgressColor() const;

  ChartView&   SetGaugeCenterLabel(const Dali::String& text);
  Dali::String GetGaugeCenterLabel() const;

  ChartView& AddGaugeRange(float fromValue, float toValue, const Vector4& color);
  ChartView& ClearGaugeRanges();

public:
  /// @cond internal
  explicit DALI_UI_API ChartView(Integration::ChartViewImpl& implementation);
  explicit DALI_UI_API ChartView(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  DALI_UI_CHAIN_VIEW_METHODS(ChartView)
};

} // namespace Ui
} // namespace Dali
