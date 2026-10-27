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
#include <dali-ui-foundation/public-api/configuration/ui-color-manager.h>
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
#include <dali-ui-foundation/public-api/views/canvas/canvas-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace DALI_NAMESPACE
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
class DALI_UI_COMPONENTS_API ChartViewImpl : public ViewImpl
{
public:
  static Ui::ChartView New(Ui::ChartView::Type type, const Vector2& size);

  // =========================================================================
  // Public API implementation
  // =========================================================================

  void    AddSeries(Ui::ChartSeries series);
  void    ApplyStyle(const Ui::ChartViewStyle& style);
  void    SetViewport(const Vector4& range);
  Vector4 GetViewport() const;
  bool    SelectPoint(uint32_t seriesIndex, uint32_t pointIndex);
  bool    RemoveSeries(const Dali::String& name);
  void    RemoveAllSeries();

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

  void SetUpdateThrottle(float ms);

  float GetUpdateThrottle() const
  {
    return mThrottleMs;
  }

  // Gauge
  void SetGaugeValue(float v);

  float GetGaugeValue() const
  {
    return mModel.mStyle.gauge.value;
  }

  void SetGaugeMinValue(float v);

  float GetGaugeMinValue() const
  {
    return mModel.mStyle.gauge.minValue;
  }

  void SetGaugeMaxValue(float v);

  float GetGaugeMaxValue() const
  {
    return mModel.mStyle.gauge.maxValue;
  }

  void SetGaugeArcSpan(float degrees);

  float GetGaugeArcSpan() const
  {
    return mModel.mStyle.gauge.arcSpanDegrees;
  }

  void SetGaugeStartAngle(float degrees);

  float GetGaugeStartAngle() const
  {
    return mModel.mStyle.gauge.startAngleDegrees;
  }

  void SetGaugeArcWidth(float ratio);

  float GetGaugeArcWidth() const
  {
    return mModel.mStyle.gauge.arcWidthRatio;
  }

  void SetGaugeTrackColor(const Vector4& color);

  Vector4 GetGaugeTrackColor() const
  {
    return mModel.mStyle.gauge.trackColor;
  }

  void SetGaugeProgressColor(const Vector4& color);

  Vector4 GetGaugeProgressColor() const
  {
    return mModel.mStyle.gauge.progressColor;
  }

  void SetGaugeCenterLabel(const Dali::String& text);

  Dali::String GetGaugeCenterLabel() const
  {
    return mModel.mStyle.gauge.centerLabel;
  }

  void AddGaugeRange(float fromValue, float toValue, const Vector4& color);
  void ClearGaugeRanges();

  // Zoom / Pan
  void SetZoomMode(int flags);

  int GetZoomMode() const
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
  void AddAnnotation(Ui::ChartAnnotation annotation);
  void RemoveAnnotation(Ui::ChartAnnotation annotation);
  void ClearAnnotations();
  void RebuildAnnotations();
  void SetTooltipContent(Ui::View content);
  /**
   * @brief Updates the shared tooltip content while preserving single-point text formatting.
   * @param[in] hits Selected points whose tooltip content is refreshed
   * @param[in] singlePoint True for the single-point overlay, which includes the X label
   */
  void UpdateTooltip(const std::vector<HitResult>& hits, bool singlePoint = false);

  Ui::ChartView::TooltipUpdatedSignalType& TooltipUpdatedSignal()
  {
    return mTooltipUpdatedSignal;
  }

  Ui::ChartView::ContentUpdatedSignalType& ContentUpdatedSignal()
  {
    return mContentUpdatedSignal;
  }

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
  void QueueRedraw();
  void FlushRedraw();
  void EmitContentUpdated();

  /**
   * @brief Refreshes resolved colors after a theme color-table update.
   */
  void OnColorsChanged();
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
  void ClampViewport(bool clampX, bool clampY);
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
  // Ordered storage
  ChartModel                                  mModel;
  ChartLayoutManager::LayoutResult            mLastLayout;
  HitResult                                   mLastHit;
  ChartLayoutManager                          mLayoutManager;
  ScaleEngine                                 mScale;
  DataRenderer                                mDataRenderer;
  std::vector<Ui::ChartAnnotation>            mAnnotations;
  std::vector<Ui::View>                       mAnnotationViews;
  std::vector<std::vector<float>>             mLastRenderedCanvasY;
  std::vector<std::vector<float>>             mAnimOldCanvasY;
  std::vector<ChartBarGeometry>               mLastRenderedBars;
  std::vector<ChartBarGeometry>               mAnimOldBars;
  std::vector<Ui::Label>                      mXTickLabels;
  std::vector<Ui::Label>                      mYTickLabels;
  std::vector<Ui::Label>                      mLegendLabels;
  std::vector<Ui::Label>                      mDataLabels;
  Ui::UiColorManager                          mColorManager;
  Ui::CanvasView                              mBackgroundCanvas;
  Ui::CanvasView                              mDataCanvas;
  Ui::CanvasView                              mOverlayCanvas;
  CallbackBase*                               mRedrawCallback{nullptr};
  Ui::Label                                   mTooltipLabel;
  Ui::View                                    mTooltipContent;
  Ui::ChartView::TooltipUpdatedSignalType     mTooltipUpdatedSignal;
  Ui::ChartView::ContentUpdatedSignalType     mContentUpdatedSignal;
  Timer                                       mUpdateThrottleTimer;
  Timer                                       mAnimTimer;
  std::chrono::steady_clock::time_point       mAnimStartTime;
  Ui::Label                                   mTitleLabel;
  Ui::Label                                   mXAxisTitleLabel;
  Ui::Label                                   mYAxisTitleLabel;
  Ui::ChartView::TooltipFormatterType         mTooltipFormatter{nullptr};
  Ui::ChartView::DataPointSelectedSignalType  mDataPointSelectedSignal;
  Ui::ChartView::LegendItemTappedSignalType   mLegendItemTappedSignal;
  Ui::ChartView::MultiPointSelectedSignalType mMultiPointSelectedSignal;
  Ui::ChartView::ZoomedSignalType             mZoomedSignal;
  std::chrono::steady_clock::time_point       mLastTapTimePoint;
  Dali::PanGestureDetector                    mPanDetector;
  Dali::PinchGestureDetector                  mPinchDetector;
  Vector2                                     mPinchStartDataCenter;
  Vector2                                     mPanStartLocalPos;
  Vector2                                     mSize;
  Ui::ChartView::Type                         mType;
  float                                       mThrottleMs{16.0f};
  float                                       mAnimProgress{0.0f};
  float                                       mViewportXMin{0.0f};
  float                                       mViewportXMax{0.0f};
  float                                       mViewportYMin{0.0f};
  float                                       mViewportYMax{0.0f};
  float                                       mDataXMin{0.0f};
  float                                       mDataXMax{1.0f};
  float                                       mDataYMin{0.0f};
  float                                       mDataYMax{1.0f};
  float                                       mPinchStartXMin{0.0f};
  float                                       mPinchStartXMax{0.0f};
  float                                       mPinchStartYMin{0.0f};
  float                                       mPinchStartYMax{0.0f};
  float                                       mPanStartXMin{0.0f};
  float                                       mPanStartYMin{0.0f};
  int                                         mHoveredLegendIndex{-1};
  BackgroundRenderer                          mBackgroundRenderer;
  ChartHitTester                              mHitTester;
  OverlayRenderer                             mOverlayRenderer;
  bool                                        mAnimationActive{false};
  bool                                        mPendingUpdate{false};
  bool                                        mViewportActive{false};
  bool                                        mTouchActive{false};
  bool                                        mPanActive{false};
  bool                                        mNeedsBackgroundUpdate{true};
  bool                                        mNeedsDataUpdate{true};

  static constexpr uint32_t DOUBLE_TAP_MS = 300;
};

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
