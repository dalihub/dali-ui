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
#include <dali/public-api/adaptor-framework/timer.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/events/pan-gesture-detector.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/public-api/events/pinch-gesture-detector.h>
#include <dali/public-api/events/pinch-gesture.h>
#include <dali/public-api/events/wheel-event.h>
#include <chrono>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/chart-hit-tester.h>
#include <dali-ui-components/integration-api/chart/chart-layout-manager.h>
#include <dali-ui-components/integration-api/chart/chart-model.h>
#include <dali-ui-components/integration-api/chart/chart-renderer.h>
#include <dali-ui-components/integration-api/chart/chart-scale-engine.h>
#include <dali-ui-components/public-api/chart/chart-view.h>
#include <dali-ui-foundation/integration-api/size-negotiated-view-impl.h>
#include <dali-ui-foundation/public-api/views/canvas/canvas-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

class ChartViewImpl;
using ChartViewImplPtr = IntrusivePtr<ChartViewImpl>;

/**
 * @brief Implementation class for Ui::ChartView.
 */
class DALI_UI_COMPONENTS_API ChartViewImpl : public SizeNegotiatedViewImpl
{
public:
  static Ui::ChartView New(Ui::ChartView::Type type, const Vector2& size);

  // =========================================================================
  // Public API implementation
  // =========================================================================

  void AddSeries(Ui::ChartSeries series);
  bool RemoveSeries(const Dali::String& name);
  void RemoveAllSeries();

  void SetXAxis(Ui::ChartAxis axis);
  void SetYAxis(Ui::ChartAxis axis);

  void                         SetTitle(const Dali::String& title);
  Dali::String                 GetTitle() const;
  void                         SetTitlePosition(Ui::ChartView::TitlePosition position);
  Ui::ChartView::TitlePosition GetTitlePosition() const;
  void                         SetTitleColor(const Vector4& color);
  Vector4                      GetTitleColor() const;

  Ui::ChartView::DataPointSelectedSignalType& DataPointSelectedSignal()
  {
    return mDataPointSelectedSignal;
  }
  Ui::ChartView::LegendItemTappedSignalType& LegendItemTappedSignal()
  {
    return mLegendItemTappedSignal;
  }
  Ui::ChartView::MultiPointSelectedSignalType& MultiPointSelectedSignal()
  {
    return mMultiPointSelectedSignal;
  }
  Ui::ChartView::ZoomedSignalType& ZoomedSignal()
  {
    return mZoomedSignal;
  }

  void SetLegendToggleEnabled(bool enabled)
  {
    mModel.mStyle.visibility.legendToggleEnabled = enabled;
  }
  bool IsLegendToggleEnabled() const
  {
    return mModel.mStyle.visibility.legendToggleEnabled;
  }

  void SetHitThreshold(float pixels)
  {
    mModel.mStyle.interaction.hitThreshold = pixels;
  }
  float GetHitThreshold() const
  {
    return mModel.mStyle.interaction.hitThreshold;
  }

  void SetFindingStrategy(Ui::ChartView::FindingStrategy s)
  {
    mModel.mStyle.interaction.findingStrategy = static_cast<int>(s);
  }
  Ui::ChartView::FindingStrategy GetFindingStrategy() const
  {
    return static_cast<Ui::ChartView::FindingStrategy>(mModel.mStyle.interaction.findingStrategy);
  }

  void SetTooltipFormatter(Ui::ChartView::TooltipFormatterType formatter)
  {
    mTooltipFormatter = std::move(formatter);
  }

  void SetAnimationDuration(float ms)
  {
    mModel.mStyle.animation.duration = std::max(0.0f, ms);
  }
  float GetAnimationDuration() const
  {
    return mModel.mStyle.animation.duration;
  }

  void SetAnimationEasing(Ui::ChartView::EasingType easing)
  {
    mModel.mStyle.animation.easing = static_cast<int>(easing);
  }
  Ui::ChartView::EasingType GetAnimationEasing() const
  {
    return static_cast<Ui::ChartView::EasingType>(mModel.mStyle.animation.easing);
  }

  void  SetUpdateThrottle(float ms);
  float GetUpdateThrottle() const
  {
    return mThrottleMs;
  }

  // Gauge
  void  SetGaugeValue(float v);
  float GetGaugeValue() const
  {
    return mModel.mStyle.gauge.value;
  }
  void  SetGaugeMinValue(float v);
  float GetGaugeMinValue() const
  {
    return mModel.mStyle.gauge.minValue;
  }
  void  SetGaugeMaxValue(float v);
  float GetGaugeMaxValue() const
  {
    return mModel.mStyle.gauge.maxValue;
  }
  void  SetGaugeArcSpan(float degrees);
  float GetGaugeArcSpan() const
  {
    return mModel.mStyle.gauge.arcSpanDegrees;
  }
  void  SetGaugeStartAngle(float degrees);
  float GetGaugeStartAngle() const
  {
    return mModel.mStyle.gauge.startAngleDegrees;
  }
  void  SetGaugeArcWidth(float ratio);
  float GetGaugeArcWidth() const
  {
    return mModel.mStyle.gauge.arcWidthRatio;
  }
  void    SetGaugeTrackColor(const Vector4& color);
  Vector4 GetGaugeTrackColor() const
  {
    return mModel.mStyle.gauge.trackColor;
  }
  void    SetGaugeProgressColor(const Vector4& color);
  Vector4 GetGaugeProgressColor() const
  {
    return mModel.mStyle.gauge.progressColor;
  }
  void         SetGaugeCenterLabel(const Dali::String& text);
  Dali::String GetGaugeCenterLabel() const
  {
    return mModel.mStyle.gauge.centerLabel;
  }
  void AddGaugeRange(float fromValue, float toValue, const Vector4& color);
  void ClearGaugeRanges();

  // Zoom / Pan
  void SetZoomMode(int flags);
  int  GetZoomMode() const
  {
    return mModel.mStyle.interaction.zoomModeFlags;
  }
  void ResetZoom();
  void SetZoomClampEnabled(bool enabled)
  {
    mModel.mStyle.interaction.zoomClampEnabled = enabled;
  }
  bool IsZoomClampEnabled() const
  {
    return mModel.mStyle.interaction.zoomClampEnabled;
  }
  void SetAutoFitYOnPan(bool enabled)
  {
    mModel.mStyle.interaction.autoFitY = enabled;
  }
  bool IsAutoFitYOnPan() const
  {
    return mModel.mStyle.interaction.autoFitY;
  }

  // Sections
  void AddSection(Ui::ChartSection section);
  void RemoveSection(Ui::ChartSection section);
  void ClearSections();

  // =========================================================================
  // Property
  // =========================================================================

  static void            SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);
  static Property::Value GetProperty(BaseObject* object, Property::Index index);

protected:
  ChartViewImpl(Ui::ChartView::Type type, const Vector2& size);
  ~ChartViewImpl() override;

private:
  // ViewImpl overrides
  void       OnInitialize() override;
  LayoutRect OnArrange(const LayoutRect& bounds) override;

  // Helpers
  void SyncLayerSizes();
  void UpdateScale();
  void RebuildBackground();
  void RebuildData();

  void OnSeriesDataChanged();
  void OnAxisConfigChanged();

  bool OnTouch(Actor actor, TouchEvent event);
  bool OnHover(Actor actor, HoverEvent event);

  void        UpdateOverlay(const HitResult& hit);
  void        UpdateOverlayMulti(const std::vector<HitResult>& hits);
  void        PerformHitAtPos(const Vector2& localPos, bool emitSignal);
  int         HitTestPie(const Vector2& local, int& outSeriesIdx) const;
  std::string BuildMultiTooltipText(const std::vector<HitResult>& hits) const;
  void        HideOverlay();
  Vector2     ComputeTooltipPosition(const Vector2& hitCanvasPos, const Vector2& tooltipSize) const;

  int  FindLegendItemAt(const Vector2& pos) const;
  bool HandleLegendTap(const Vector2& tapPos);
  void HighlightLegendItem(int index);
  void ClearLegendHighlight();

  struct TickLabelStyle
  {
    Dali::String horizontalAlign;
    Dali::String verticalAlign;
    Vector3      pivot;
    Vector3      parentOrigin;
    float        fontSize{11.0f};
  };

  void      PlaceTextLabels(const ChartLayoutManager::LayoutResult& layout);
  void      PlaceTickLabels(std::vector<Ui::Label>&                           pool,
                            const std::vector<ChartLayoutManager::TickLabel>& ticks,
                            const TickLabelStyle&                             labelStyle);
  Ui::Label GetOrCreateLabel(std::vector<Ui::Label>& pool, size_t index);
  void      HideExcessLabels(std::vector<Ui::Label>& pool, size_t usedCount);
  void      ClearLabelPool(std::vector<Ui::Label>& pool);

  // Throttle timer
  bool OnUpdateThrottleTimer();

  // Animation
  bool                            OnAnimTimer();
  std::vector<std::vector<float>> CaptureCanvasY() const;
  void                            RebuildDataAnimated(const std::vector<std::vector<float>>& oldY, float t);
  static float                    ApplyEasing(float t, int easingType);

  // Zoom helpers
  void InitViewportFromData();
  void ApplyViewportToScale();
  void ClampViewport();
  void FitYToViewport();
  void EmitZoomedSignal();

  // Gesture callbacks
  void OnPanGesture(Actor actor, Dali::PanGesture pan);
  void OnPinchGesture(Actor actor, Dali::PinchGesture pinch);
  bool OnWheel(Actor actor, Dali::WheelEvent event);

  // Non-copyable
  ChartViewImpl(const ChartViewImpl&)            = delete;
  ChartViewImpl& operator=(const ChartViewImpl&) = delete;

private:
  Ui::ChartView::Type mType;
  ChartModel          mModel;
  ScaleEngine         mScale;

  // Canvas layers
  Ui::CanvasView mBackgroundCanvas;
  Ui::CanvasView mDataCanvas;
  Ui::CanvasView mOverlayCanvas;

  // Layout / Rendering
  ChartLayoutManager               mLayoutManager;
  BackgroundRenderer               mBackgroundRenderer;
  DataRenderer                     mDataRenderer;
  ChartLayoutManager::LayoutResult mLastLayout;

  // Interaction
  ChartHitTester  mHitTester;
  OverlayRenderer mOverlayRenderer;

  // Tooltip
  Ui::Label mTooltipLabel;

  // Throttle timer
  Timer mUpdateThrottleTimer;
  float mThrottleMs{16.0f};
  bool  mPendingUpdate{false};

  // Animation timer
  Timer                                 mAnimTimer;
  std::vector<std::vector<float>>       mLastRenderedCanvasY;
  std::vector<std::vector<float>>       mAnimOldCanvasY;
  std::chrono::steady_clock::time_point mAnimStartTime;

  // Text label pools
  Ui::Label              mTitleLabel;
  Ui::Label              mXAxisTitleLabel;
  Ui::Label              mYAxisTitleLabel;
  std::vector<Ui::Label> mXTickLabels;
  std::vector<Ui::Label> mYTickLabels;
  std::vector<Ui::Label> mLegendLabels;
  std::vector<Ui::Label> mDataLabels;

  // Formatter
  Ui::ChartView::TooltipFormatterType mTooltipFormatter{nullptr};

  // Signals
  Ui::ChartView::DataPointSelectedSignalType  mDataPointSelectedSignal;
  Ui::ChartView::LegendItemTappedSignalType   mLegendItemTappedSignal;
  Ui::ChartView::MultiPointSelectedSignalType mMultiPointSelectedSignal;
  Ui::ChartView::ZoomedSignalType             mZoomedSignal;

  // Viewport state (data-space)
  float mViewportXMin{0.0f};
  float mViewportXMax{0.0f};
  float mViewportYMin{0.0f};
  float mViewportYMax{0.0f};
  bool  mViewportActive{false};

  // Full data range for clamping
  float mDataXMin{0.0f};
  float mDataXMax{1.0f};
  float mDataYMin{0.0f};
  float mDataYMax{1.0f};

  // Pinch state
  Vector2 mPinchStartDataCenter;
  float   mPinchStartXMin{0.0f};
  float   mPinchStartXMax{0.0f};
  float   mPinchStartYMin{0.0f};
  float   mPinchStartYMax{0.0f};

  // Pan state
  Vector2 mPanStartLocalPos;
  float   mPanStartXMin{0.0f};
  float   mPanStartYMin{0.0f};

  // Touch / hover state
  HitResult mLastHit;
  int       mHoveredLegendIndex{-1};
  bool      mTouchActive{false};
  bool      mPanActive{false};

  // Double-tap
  std::chrono::steady_clock::time_point mLastTapTimePoint;
  static constexpr uint32_t             DOUBLE_TAP_MS = 300;

  // Explicit gesture detectors (ViewImpl has no EnableGestureDetection)
  Dali::PanGestureDetector   mPanDetector;
  Dali::PinchGestureDetector mPinchDetector;

  Vector2 mSize;
  bool    mNeedsBackgroundUpdate{true};
  bool    mNeedsDataUpdate{true};
};

} // namespace Integration
} // namespace Ui
} // namespace Dali
