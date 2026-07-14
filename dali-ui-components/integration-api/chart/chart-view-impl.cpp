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

// CLASS HEADER
#include <dali-ui-components/integration-api/chart/chart-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/events/hover-event.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>
#include <map>

// INTERNAL INCLUDES
#include <dali-ui-components/integration-api/chart/bar-series-impl.h>
#include <dali-ui-components/integration-api/chart/chart-axis-impl.h>
#include <dali-ui-components/integration-api/chart/chart-series-impl.h>
#include <dali-ui-components/integration-api/chart/pie-series-impl.h>
#include <dali-ui-components/public-api/chart/bar-series.h>
#include <dali-ui-components/public-api/chart/pie-series.h>
#include <dali-ui-foundation/extension-api/property-registration-helper.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace
{

BaseHandle Create()
{
  return BaseHandle();
}

// clang-format off
#define CHART_VIEW_PROPERTY_REGISTRATION(text, valueType, enumIndex) \
  DALI_PROPERTY_REGISTRATION_EXTERNAL(Ui, ChartViewPropertyIndex, Ui::Integration, ChartViewImpl, text, valueType, enumIndex)

DALI_TYPE_REGISTRATION_BEGIN(ChartViewImpl, ViewImpl, Create)
CHART_VIEW_PROPERTY_REGISTRATION("showGrid",           BOOLEAN, SHOW_GRID)
CHART_VIEW_PROPERTY_REGISTRATION("showLegend",         BOOLEAN, SHOW_LEGEND)
CHART_VIEW_PROPERTY_REGISTRATION("showTooltip",        BOOLEAN, SHOW_TOOLTIP)
CHART_VIEW_PROPERTY_REGISTRATION("backgroundColor",    VECTOR4, BACKGROUND_COLOR)
CHART_VIEW_PROPERTY_REGISTRATION("gridColor",          VECTOR4, GRID_COLOR)
CHART_VIEW_PROPERTY_REGISTRATION("animationDuration",  FLOAT,   ANIMATION_DURATION)
CHART_VIEW_PROPERTY_REGISTRATION("yAxisAutoRange",     BOOLEAN, Y_AXIS_AUTO_RANGE)
CHART_VIEW_PROPERTY_REGISTRATION("legendPosition",     INTEGER, LEGEND_POSITION)
CHART_VIEW_PROPERTY_REGISTRATION("axisLabelSize",      FLOAT,   AXIS_LABEL_SIZE)
CHART_VIEW_PROPERTY_REGISTRATION("titleSize",          FLOAT,   TITLE_SIZE)
CHART_VIEW_PROPERTY_REGISTRATION("lineWidth",          FLOAT,   LINE_WIDTH)
CHART_VIEW_PROPERTY_REGISTRATION("showMarkers",        BOOLEAN, SHOW_MARKERS)
CHART_VIEW_PROPERTY_REGISTRATION("markerRadius",       FLOAT,   MARKER_RADIUS)
CHART_VIEW_PROPERTY_REGISTRATION("hoverEnabled",       BOOLEAN, HOVER_ENABLED)
CHART_VIEW_PROPERTY_REGISTRATION("touchEnabled",       BOOLEAN, TOUCH_ENABLED)
DALI_TYPE_REGISTRATION_END()
#undef CHART_VIEW_PROPERTY_REGISTRATION
// clang-format on

} // anonymous namespace

// =============================================================================
// Construction
// =============================================================================

ChartViewImpl::ChartViewImpl(Ui::ChartView::Type type, const Vector2& size)
: ViewImpl(),
  mType(type),
  mSize(size)
{
  mModel.mXAxis = Ui::ChartAxis::New();
  mModel.mYAxis = Ui::ChartAxis::New();
}

ChartViewImpl::~ChartViewImpl()
{
  // Stop timers first so no callbacks fire after destruction begins.
  if(mUpdateThrottleTimer)
  {
    mUpdateThrottleTimer.Stop();
  }
  if(mAnimTimer)
  {
    mAnimTimer.Stop();
  }

  // Explicitly disconnect signals on external objects (series, axes).
  // mModel members are still valid here (member destructors run after this body).
  // ViewImpl inherits ConnectionTrackerInterface, not ConnectionTracker, so
  // auto-disconnect on destruction is not guaranteed for externally-owned signals.
  for(auto& s : mModel.mSeriesList)
  {
    GetImplementation(s).DataChangedSignal().Disconnect(this, &ChartViewImpl::OnSeriesDataChanged);
  }
  if(mModel.mXAxis)
  {
    GetImplementation(mModel.mXAxis).ConfigChangedSignal().Disconnect(this, &ChartViewImpl::OnAxisConfigChanged);
  }
  if(mModel.mYAxis)
  {
    GetImplementation(mModel.mYAxis).ConfigChangedSignal().Disconnect(this, &ChartViewImpl::OnAxisConfigChanged);
  }

  // Release Handle references so refcounts drop.
  // Do NOT call Self() here — the CustomActorImpl base is already in
  // teardown; Self() triggers DALI_ASSERT_ALWAYS in that state.
  // The Actor tree cleanup (children removed) is handled automatically
  // by the Actor's own destructor after this runs.
  mBackgroundCanvas.Reset();
  mDataCanvas.Reset();
  mOverlayCanvas.Reset();

  mTitleLabel.Reset();
  mXAxisTitleLabel.Reset();
  mYAxisTitleLabel.Reset();
  mTooltipLabel.Reset();

  mXTickLabels.clear();
  mYTickLabels.clear();
  mLegendLabels.clear();
  mDataLabels.clear();
}

Ui::ChartView ChartViewImpl::New(Ui::ChartView::Type type, const Vector2& size)
{
  ChartViewImpl* impl = new ChartViewImpl(type, size);
  Ui::ChartView  handle(*impl);
  impl->Initialize();
  return handle;
}

// =============================================================================
// ViewImpl overrides
// =============================================================================

void ChartViewImpl::OnInitialize()
{
  // ViewImpl::OnInitialize() sets PIVOT=TOP_LEFT, PARENT_ORIGIN=TOP_LEFT,
  // POSITION_USES_PIVOT=false.  With PIVOT=TOP_LEFT, POSITION_USES_PIVOT=false
  // and =true are mathematically identical, so no override is needed.
  ViewImpl::OnInitialize();

  // Register the constructor-specified canvas size as the natural (requested)
  // size so the layout system can measure this view correctly even without
  // explicit AbsoluteLayoutParams (e.g. direct window.Add usage).
  // Must be called here, not in the constructor, because SetRequestedWidth/Height
  // require the Dali Handle to be initialised first.
  SetRequestedWidth(mSize.x);
  SetRequestedHeight(mSize.y);

  Actor self = Self();
  self.SetProperty(Actor::Property::SIZE, mSize);

  mBackgroundCanvas = Ui::CanvasView::New(mSize);
  mDataCanvas       = Ui::CanvasView::New(mSize);
  mOverlayCanvas    = Ui::CanvasView::New(mSize);

  auto configureLayers = [](Ui::CanvasView& canvas, const Vector2& size)
  {
    canvas.SetProperty(Ui::CanvasView::Property::SYNCHRONOUS_LOADING, true);
    canvas.SetProperty(Ui::CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY, true);
    canvas.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    canvas.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    canvas.SetProperty(Actor::Property::SIZE, size);
    // Request MATCH_PARENT so the layout system assigns the chart's full bounds
    // if OnArrange is ever restored to call ViewImpl::OnArrange.
    canvas.SetRequestedWidth(MATCH_PARENT);
    canvas.SetRequestedHeight(MATCH_PARENT);
  };

  configureLayers(mBackgroundCanvas, mSize);
  configureLayers(mDataCanvas, mSize);
  configureLayers(mOverlayCanvas, mSize);

  mBackgroundCanvas.SetProperty(Actor::Property::SENSITIVE, false);
  mDataCanvas.SetProperty(Actor::Property::SENSITIVE, false);
  mOverlayCanvas.SetProperty(Actor::Property::SENSITIVE, false);

  self.Add(mBackgroundCanvas);
  self.Add(mDataCanvas);
  self.Add(mOverlayCanvas);

  mScale.SetPlotArea(Rect<float>(0.0f, 0.0f, mSize.width, mSize.height));

  mTooltipLabel = Ui::Label::New();
  mTooltipLabel.SetProperty(Actor::Property::VISIBLE, false);
  mTooltipLabel.SetProperty(Actor::Property::SENSITIVE, false);
  mTooltipLabel.SetProperty(Ui::Label::Property::MULTI_LINE, true);
  mTooltipLabel.SetProperty(Ui::Label::Property::TEXT_COLOR, Vector4(0.1f, 0.1f, 0.1f, 1.0f));
  mTooltipLabel.SetProperty(Ui::Label::Property::FONT_SIZE, 10.0f);
  mTooltipLabel.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
  mTooltipLabel.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  mTooltipLabel.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  self.Add(mTooltipLabel);

  // Throttle timer (single-shot)
  mUpdateThrottleTimer = Timer::New(16);
  mUpdateThrottleTimer.TickSignal().Connect(this, &ChartViewImpl::OnUpdateThrottleTimer);

  // Animation timer (repeating, started on demand)
  mAnimTimer = Timer::New(16);
  mAnimTimer.TickSignal().Connect(this, &ChartViewImpl::OnAnimTimer);

  // Touch and hover
  self.TouchEventSignal().Connect(this, &ChartViewImpl::OnTouch);
  self.HoverEventSignal().Connect(this, &ChartViewImpl::OnHover);

  DALI_LOG_DEBUG_INFO("ChartViewImpl [%p] initialized: type=%d, size=%.0fx%.0f\n",
                      this, static_cast<int>(mType), mSize.width, mSize.height);
}

LayoutRect ChartViewImpl::OnArrange(const LayoutRect& bounds)
{
  // ViewImpl::OnArrange is intentionally not called.  It would iterate child
  // Views and call Arrange(0,0,w,h) on those without AbsoluteLayoutParams,
  // resetting canvas layer sizes to 0×0.  Canvas layers declare MATCH_PARENT
  // requested size but the inherited ViewImpl layout does not propagate that
  // correctly for non-Layout parents, so we manage position and size here.
  // Text labels are positioned via Actor::Property::POSITION set in
  // PlaceTextLabels.  Because ViewImpl::OnArrange is skipped, the layout
  // system never resets those positions, so no save/restore is needed.

  // Self geometry is applied centrally by ViewImpl::Arrange (ApplySelfBoundsIfChanged).
  const Vector2 newSize(bounds.width, bounds.height);
  if(newSize != mSize && newSize.width > 0.0f && newSize.height > 0.0f)
  {
    mSize                  = newSize;
    mNeedsBackgroundUpdate = true;
    mNeedsDataUpdate       = true;
  }

  // SyncLayerSizes is called inside RebuildBackground when mSize changes.
  // No need to call it unconditionally here: since ViewImpl::OnArrange is
  // skipped, canvas sizes are never reset between layout passes.
  if(mNeedsBackgroundUpdate)
  {
    RebuildBackground();
    mNeedsBackgroundUpdate = false;
  }

  if(mNeedsDataUpdate)
  {
    RebuildData();
    mNeedsDataUpdate = false;
  }

  // Echo the layout-assigned bounds as this view's final self bounds.
  return bounds;
}

// =============================================================================
// Public API implementation
// =============================================================================

void ChartViewImpl::AddSeries(Ui::ChartSeries series)
{
  mModel.AddSeries(series);
  GetImplementation(series).DataChangedSignal().Connect(this, &ChartViewImpl::OnSeriesDataChanged);

  mModel.ComputeAutoRange();
  UpdateScale();
  mNeedsBackgroundUpdate = true;
  mNeedsDataUpdate       = true;
  RelayoutRequest();
}

void ChartViewImpl::SetXAxis(Ui::ChartAxis axis)
{
  mModel.mXAxis = axis;
  GetImplementation(axis).ConfigChangedSignal().Connect(this, &ChartViewImpl::OnAxisConfigChanged);

  mModel.ComputeAutoRange();
  UpdateScale();
  mNeedsBackgroundUpdate = true;
  mNeedsDataUpdate       = true;
  RelayoutRequest();
}

void ChartViewImpl::SetYAxis(Ui::ChartAxis axis)
{
  mModel.mYAxis = axis;
  GetImplementation(axis).ConfigChangedSignal().Connect(this, &ChartViewImpl::OnAxisConfigChanged);

  mModel.ComputeAutoRange();
  UpdateScale();
  mNeedsBackgroundUpdate = true;
  mNeedsDataUpdate       = true;
  RelayoutRequest();
}

void ChartViewImpl::OnSeriesDataChanged()
{
  if(mPendingUpdate) return;

  if(mModel.mStyle.animation.duration > 0.0f && !mAnimTimer.IsRunning())
    mAnimOldCanvasY = mLastRenderedCanvasY;

  mPendingUpdate = true;
  if(mThrottleMs < 1.0f)
  {
    OnUpdateThrottleTimer();
  }
  else
  {
    mUpdateThrottleTimer.Start();
  }
}

void ChartViewImpl::SetUpdateThrottle(float ms)
{
  mThrottleMs = std::max(0.0f, ms);

  const bool pendingFire = mPendingUpdate && mUpdateThrottleTimer.IsRunning();
  mUpdateThrottleTimer.Stop();

  if(mThrottleMs >= 1.0f)
  {
    const uint32_t interval = static_cast<uint32_t>(std::round(mThrottleMs));
    mUpdateThrottleTimer    = Timer::New(interval);
    mUpdateThrottleTimer.TickSignal().Connect(this, &ChartViewImpl::OnUpdateThrottleTimer);
    if(pendingFire) mUpdateThrottleTimer.Start();
  }
  else if(pendingFire)
  {
    OnUpdateThrottleTimer();
  }
}

bool ChartViewImpl::OnUpdateThrottleTimer()
{
  mPendingUpdate = false;
  mModel.ComputeAutoRange();
  UpdateScale();

  const float dur = mModel.mStyle.animation.duration;
  if(dur > 0.0f && !mAnimOldCanvasY.empty())
  {
    mAnimStartTime = std::chrono::steady_clock::now();
    if(!mAnimTimer.IsRunning()) mAnimTimer.Start();
  }
  else
  {
    mAnimOldCanvasY.clear();
    if(!mLastLayout.plotArea.IsEmpty() && mSize.width > 0.0f)
    {
      // Bypass the layout system for data-only updates.
      //
      // RelayoutRequest() only registers with the dali-ui LayoutController;
      // it does NOT signal the Adaptor to run another render cycle.  The
      // layout (and therefore RequestRasterization) would only execute when
      // the Adaptor wakes up for another reason (e.g. a hover event), causing
      // animated charts to freeze until mouse movement.
      //
      // Calling Rebuild*() directly triggers RequestRasterization() which
      // signals the Adaptor immediately, keeping timer-driven animations smooth.
      RebuildBackground();
      RebuildData();
    }
    else
    {
      // Not yet laid out — defer until OnArrange provides valid bounds.
      mNeedsBackgroundUpdate = true;
      mNeedsDataUpdate       = true;
      RelayoutRequest();
    }
  }
  return false;
}

// ── Animation helpers ─────────────────────────────────────────────────────────

std::vector<std::vector<float>> ChartViewImpl::CaptureCanvasY() const
{
  std::vector<std::vector<float>> snap;
  for(const auto& series : mModel.mSeriesList)
  {
    const auto&        pts = GetImplementation(const_cast<Ui::ChartSeries&>(series)).GetValues();
    std::vector<float> canvasYs;
    canvasYs.reserve(pts.size());
    for(const auto& p : pts)
    {
      canvasYs.push_back(std::isnan(p.second) ? std::numeric_limits<float>::quiet_NaN()
                                              : mScale.ToCanvasY(p.second));
    }
    snap.push_back(std::move(canvasYs));
  }
  return snap;
}

float ChartViewImpl::ApplyEasing(float t, int easingType)
{
  t = std::min(std::max(t, 0.0f), 1.0f);
  switch(easingType)
  {
    case 1:
      return 1.0f - (1.0f - t) * (1.0f - t);
    case 2:
      return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    default:
      return t;
  }
}

bool ChartViewImpl::OnAnimTimer()
{
  const float dur = mModel.mStyle.animation.duration;
  if(dur <= 0.0f)
  {
    // Same direct-rebuild pattern as OnUpdateThrottleTimer: bypass the layout
    // system so RequestRasterization() wakes the Adaptor immediately.
    if(!mLastLayout.plotArea.IsEmpty() && mSize.width > 0.0f)
    {
      RebuildData();
    }
    else
    {
      mNeedsDataUpdate = true;
      RelayoutRequest();
    }
    return false;
  }

  auto  now     = std::chrono::steady_clock::now();
  float elapsed = static_cast<float>(
    std::chrono::duration_cast<std::chrono::milliseconds>(now - mAnimStartTime).count());
  float rawT = std::min(elapsed / dur, 1.0f);
  float t    = ApplyEasing(rawT, mModel.mStyle.animation.easing);

  if(mAnimOldCanvasY.size() != mModel.mSeriesList.size()) t = 1.0f;

  RebuildDataAnimated(mAnimOldCanvasY, t);

  if(rawT >= 1.0f)
  {
    mAnimOldCanvasY.clear();
    return false;
  }
  return true;
}

void ChartViewImpl::RebuildDataAnimated(const std::vector<std::vector<float>>& oldY, float t)
{
  if(!mDataCanvas || mSize.width < 1.0f) return;

  auto labels = mDataRenderer.Render(mDataCanvas, mModel, mScale, mLastLayout, &oldY, t);
  mDataCanvas.RequestRasterization();

  if(t >= 1.0f)
  {
    size_t used = 0;
    for(const auto& info : labels)
    {
      auto label = GetOrCreateLabel(mDataLabels, used);
      label.SetProperty(Ui::Label::Property::TEXT, info.text);
      label.SetProperty(Ui::Label::Property::TEXT_COLOR, info.color);
      label.SetProperty(Ui::Label::Property::FONT_SIZE, info.size);
      label.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      label.SetProperty(Actor::Property::PIVOT, Pivot::BOTTOM_CENTER);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      label.SetProperty(Actor::Property::POSITION, info.position);
      label.SetProperty(Actor::Property::VISIBLE, true);
      ++used;
    }
    HideExcessLabels(mDataLabels, used);
    mLastRenderedCanvasY = CaptureCanvasY();
  }
}

void ChartViewImpl::OnAxisConfigChanged()
{
  if(mViewportActive) return;

  mModel.ComputeAutoRange();
  UpdateScale();
  mNeedsBackgroundUpdate = true;
  mNeedsDataUpdate       = true;
  RelayoutRequest();
}

// =============================================================================
// Phase 5-C: Zoom / Pan
// =============================================================================

void ChartViewImpl::SetZoomMode(int flags)
{
  const int  oldFlags  = mModel.mStyle.interaction.zoomModeFlags;
  const bool wasActive = oldFlags != 0;
  const bool wasZoom   = oldFlags & (static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_X) |
                                   static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_Y));

  mModel.mStyle.interaction.zoomModeFlags = flags;

  const bool isActive = flags != 0;
  const bool isZoom   = flags & (static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_X) |
                               static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_Y));

  // ViewImpl has no EnableGestureDetection — manage detectors manually
  if(isActive && !wasActive)
  {
    mPanDetector   = Dali::PanGestureDetector::New();
    mPinchDetector = Dali::PinchGestureDetector::New();
    mPanDetector.Attach(Self());
    mPinchDetector.Attach(Self());
    mPanDetector.DetectedSignal().Connect(this, &ChartViewImpl::OnPanGesture);
    mPinchDetector.DetectedSignal().Connect(this, &ChartViewImpl::OnPinchGesture);
  }
  else if(!isActive && wasActive)
  {
    if(mPanDetector)
    {
      mPanDetector.Detach(Self());
      mPanDetector.Reset();
    }
    if(mPinchDetector)
    {
      mPinchDetector.Detach(Self());
      mPinchDetector.Reset();
    }
    if(mViewportActive) ResetZoom();
  }

  if(wasZoom)
    Self().WheelEventSignal().Disconnect(this, &ChartViewImpl::OnWheel);
  if(isZoom)
    Self().WheelEventSignal().Connect(this, &ChartViewImpl::OnWheel);
}

void ChartViewImpl::ResetZoom()
{
  mViewportActive = false;
  GetImplementation(mModel.mXAxis).SetAutoRange(true);
  GetImplementation(mModel.mYAxis).SetAutoRange(true);
  mModel.ComputeAutoRange();
  UpdateScale();
  mViewportXMin          = GetImplementation(mModel.mXAxis).GetMinLimit();
  mViewportXMax          = GetImplementation(mModel.mXAxis).GetMaxLimit();
  mViewportYMin          = GetImplementation(mModel.mYAxis).GetMinLimit();
  mViewportYMax          = GetImplementation(mModel.mYAxis).GetMaxLimit();
  mNeedsBackgroundUpdate = true;
  mNeedsDataUpdate       = true;
  RelayoutRequest();
  EmitZoomedSignal();
}

void ChartViewImpl::InitViewportFromData()
{
  auto& xImpl   = GetImplementation(mModel.mXAxis);
  auto& yImpl   = GetImplementation(mModel.mYAxis);
  mViewportXMin = mDataXMin = xImpl.GetMinLimit();
  mViewportXMax = mDataXMax = xImpl.GetMaxLimit();
  mViewportYMin = mDataYMin = yImpl.GetMinLimit();
  mViewportYMax = mDataYMax = yImpl.GetMaxLimit();
}

void ChartViewImpl::ApplyViewportToScale()
{
  auto& xImpl = GetImplementation(mModel.mXAxis);
  auto& yImpl = GetImplementation(mModel.mYAxis);
  if(mViewportXMax - mViewportXMin < 1e-6f) mViewportXMax = mViewportXMin + 1e-6f;
  if(mViewportYMax - mViewportYMin < 1e-6f) mViewportYMax = mViewportYMin + 1e-6f;
  xImpl.SetMinLimit(mViewportXMin);
  xImpl.SetMaxLimit(mViewportXMax);
  xImpl.SetAutoRange(false);
  yImpl.SetMinLimit(mViewportYMin);
  yImpl.SetMaxLimit(mViewportYMax);
  yImpl.SetAutoRange(false);
  UpdateScale();
  EmitZoomedSignal();
}

void ChartViewImpl::ClampViewport()
{
  const float xViewW = mViewportXMax - mViewportXMin;
  if(mViewportXMin < mDataXMin)
  {
    mViewportXMin = mDataXMin;
    mViewportXMax = mViewportXMin + xViewW;
  }
  if(mViewportXMax > mDataXMax)
  {
    mViewportXMax = mDataXMax;
    mViewportXMin = mViewportXMax - xViewW;
  }
  mViewportXMin = std::max(mViewportXMin, mDataXMin);
  mViewportXMax = std::min(mViewportXMax, mDataXMax);

  const float yViewH = mViewportYMax - mViewportYMin;
  if(mViewportYMin < mDataYMin)
  {
    mViewportYMin = mDataYMin;
    mViewportYMax = mViewportYMin + yViewH;
  }
  if(mViewportYMax > mDataYMax)
  {
    mViewportYMax = mDataYMax;
    mViewportYMin = mViewportYMax - yViewH;
  }
  mViewportYMin = std::max(mViewportYMin, mDataYMin);
  mViewportYMax = std::min(mViewportYMax, mDataYMax);
}

void ChartViewImpl::FitYToViewport()
{
  float yMin = std::numeric_limits<float>::max();
  float yMax = std::numeric_limits<float>::lowest();

  // Pass 1: stacked BarSeries
  {
    std::map<int, float> stackedPos;
    std::map<int, float> stackedNeg;
    bool                 hasStacked = false;

    for(const auto& series : mModel.mSeriesList)
    {
      auto bs = Ui::BarSeries::DownCast(const_cast<Ui::ChartSeries&>(series));
      if(!bs) continue;
      const auto& bsImpl = GetImplementation(bs);
      if(!bsImpl.IsVisible() || !bsImpl.IsStacked()) continue;

      hasStacked = true;
      for(const auto& pt : bsImpl.GetValues())
      {
        if(std::isnan(pt.second)) continue;
        if(pt.first < mViewportXMin || pt.first > mViewportXMax) continue;
        const int xKey = static_cast<int>(std::round(pt.first));
        if(pt.second >= 0.0f)
          stackedPos[xKey] += pt.second;
        else
          stackedNeg[xKey] += pt.second;
      }
    }

    if(hasStacked)
    {
      for(auto& [k, v] : stackedPos) yMax = std::max(yMax, v);
      for(auto& [k, v] : stackedNeg) yMin = std::min(yMin, v);
    }
  }

  // Pass 2: non-stacked series
  for(const auto& series : mModel.mSeriesList)
  {
    {
      auto bs = Ui::BarSeries::DownCast(const_cast<Ui::ChartSeries&>(series));
      if(bs && GetImplementation(bs).IsStacked()) continue;
    }

    const auto& impl = GetImplementation(const_cast<Ui::ChartSeries&>(series));
    if(!impl.IsVisible()) continue;

    const auto& pts = impl.GetValues();
    for(size_t i = 0; i < pts.size(); ++i)
    {
      const float x0 = pts[i].first;
      const float y0 = pts[i].second;
      if(std::isnan(y0)) continue;

      if(x0 >= mViewportXMin && x0 <= mViewportXMax)
      {
        yMin = std::min(yMin, y0);
        yMax = std::max(yMax, y0);
      }

      if(i + 1 >= pts.size()) continue;
      const float x1 = pts[i + 1].first;
      const float y1 = pts[i + 1].second;
      if(std::isnan(y1)) continue;

      const float dx = x1 - x0;
      if(std::abs(dx) < 1e-9f) continue;

      if((x0 < mViewportXMin) != (x1 < mViewportXMin))
      {
        const float iy = y0 + (y1 - y0) * (mViewportXMin - x0) / dx;
        yMin           = std::min(yMin, iy);
        yMax           = std::max(yMax, iy);
      }

      if((x0 > mViewportXMax) != (x1 > mViewportXMax))
      {
        const float iy = y0 + (y1 - y0) * (mViewportXMax - x0) / dx;
        yMin           = std::min(yMin, iy);
        yMax           = std::max(yMax, iy);
      }
    }
  }

  if(yMin > yMax) return;

  const float range   = yMax - yMin;
  const float padding = (range > 1e-6f) ? range * 0.05f
                                        : (std::abs(yMax) > 1e-6f ? std::abs(yMax) * 0.1f : 1.0f);
  mViewportYMin       = yMin - padding;
  mViewportYMax       = yMax + padding;
}

void ChartViewImpl::EmitZoomedSignal()
{
  Ui::ChartViewportArgs args;
  args.xMin = mViewportXMin;
  args.xMax = mViewportXMax;
  args.yMin = mViewportYMin;
  args.yMax = mViewportYMax;
  mZoomedSignal.Emit(args);
}

void ChartViewImpl::OnPanGesture(Actor /*actor*/, Dali::PanGesture pan)
{
  const int  flags = mModel.mStyle.interaction.zoomModeFlags;
  const bool panX  = flags & static_cast<int>(Ui::ChartView::ZoomMode::PAN_X);
  const bool panY  = flags & static_cast<int>(Ui::ChartView::ZoomMode::PAN_Y);
  if(!panX && !panY) return;

  if(pan.GetState() == Dali::GestureState::STARTED)
  {
    if(!mViewportActive) InitViewportFromData();
    mPanStartXMin     = mViewportXMin;
    mPanStartYMin     = mViewportYMin;
    mPanStartLocalPos = pan.GetPosition();
    mPanActive        = true;
    HideOverlay();
    return;
  }
  if(pan.GetState() == Dali::GestureState::FINISHED ||
     pan.GetState() == Dali::GestureState::CANCELLED)
  {
    mPanActive = false;
    return;
  }
  if(pan.GetState() != Dali::GestureState::CONTINUING) return;

  const Vector2      localDelta = pan.GetPosition() - mPanStartLocalPos;
  const Rect<float>& pa         = mLastLayout.plotArea;

  if(panX)
  {
    const float viewW = mViewportXMax - mViewportXMin;
    const float pixW  = std::max(1.0f, pa.width);
    const float shift = -(localDelta.x / pixW) * viewW;
    mViewportXMin     = mPanStartXMin + shift;
    mViewportXMax     = mViewportXMin + viewW;
  }

  if(panY)
  {
    const float viewH = mViewportYMax - mViewportYMin;
    const float pixH  = std::max(1.0f, pa.height);
    const float shift = -(localDelta.y / pixH) * viewH;
    mViewportYMin     = mPanStartYMin + shift;
    mViewportYMax     = mViewportYMin + viewH;
  }

  if(mModel.mStyle.interaction.zoomClampEnabled) ClampViewport();
  if(mModel.mStyle.interaction.autoFitY && panX) FitYToViewport();
  mViewportActive = true;
  ApplyViewportToScale();
  RebuildBackground();
  RebuildData();
}

void ChartViewImpl::OnPinchGesture(Actor /*actor*/, Dali::PinchGesture gesture)
{
  const int  flags = mModel.mStyle.interaction.zoomModeFlags;
  const bool zoomX = flags & static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_X);
  const bool zoomY = flags & static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_Y);
  if(!zoomX && !zoomY) return;

  if(gesture.GetState() == Dali::GestureState::STARTED)
  {
    if(!mViewportActive) InitViewportFromData();
    mPinchStartXMin             = mViewportXMin;
    mPinchStartXMax             = mViewportXMax;
    mPinchStartYMin             = mViewportYMin;
    mPinchStartYMax             = mViewportYMax;
    const Vector2& screenCenter = gesture.GetScreenCenterPoint();
    Vector2        local;
    Self().ScreenToLocal(local.x, local.y, screenCenter.x, screenCenter.y);
    mPinchStartDataCenter = Vector2(mScale.ToDataX(local.x), mScale.ToDataY(local.y));
    HideOverlay();
    return;
  }
  if(gesture.GetState() != Dali::GestureState::CONTINUING) return;

  const float scale = std::max(0.05f, gesture.GetScale());

  if(zoomX)
  {
    const float origW = mPinchStartXMax - mPinchStartXMin;
    const float newW  = origW / scale;
    const float ratio = (mPinchStartDataCenter.x - mPinchStartXMin) / std::max(1e-6f, origW);
    mViewportXMin     = mPinchStartDataCenter.x - ratio * newW;
    mViewportXMax     = mViewportXMin + newW;
  }

  if(zoomY)
  {
    const float origH = mPinchStartYMax - mPinchStartYMin;
    const float newH  = origH / scale;
    const float ratio = (mPinchStartDataCenter.y - mPinchStartYMin) / std::max(1e-6f, origH);
    mViewportYMin     = mPinchStartDataCenter.y - ratio * newH;
    mViewportYMax     = mViewportYMin + newH;
  }

  if(mModel.mStyle.interaction.zoomClampEnabled) ClampViewport();
  if(mModel.mStyle.interaction.autoFitY && zoomX) FitYToViewport();
  mViewportActive = true;
  ApplyViewportToScale();
  RebuildBackground();
  RebuildData();
}

bool ChartViewImpl::OnWheel(Actor /*actor*/, Dali::WheelEvent event)
{
  if(event.GetType() != Dali::WheelEvent::MOUSE_WHEEL) return false;

  const int  flags = mModel.mStyle.interaction.zoomModeFlags;
  const bool zoomX = flags & static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_X);
  const bool zoomY = flags & static_cast<int>(Ui::ChartView::ZoomMode::ZOOM_Y);
  if(!zoomX && !zoomY) return false;

  if(!mViewportActive) InitViewportFromData();

  constexpr float FACTOR_PER_NOTCH = 1.15f;
  const float     factor           = (event.GetDelta() > 0) ? (1.0f / FACTOR_PER_NOTCH) : FACTOR_PER_NOTCH;

  const Vector2& screenPt = event.GetPoint();
  Vector2        local;
  Self().ScreenToLocal(local.x, local.y, screenPt.x, screenPt.y);

  const Rect<float>& pa = mLastLayout.plotArea;
  if(local.x < pa.x || local.x > pa.x + pa.width ||
     local.y < pa.y || local.y > pa.y + pa.height)
    return false;

  const float cursorDataX = mScale.ToDataX(local.x);
  const float cursorDataY = mScale.ToDataY(local.y);

  if(zoomX)
  {
    const float viewW = mViewportXMax - mViewportXMin;
    const float newW  = viewW * factor;
    const float ratio = (cursorDataX - mViewportXMin) / std::max(1e-6f, viewW);
    mViewportXMin     = cursorDataX - ratio * newW;
    mViewportXMax     = mViewportXMin + newW;
  }

  if(zoomY)
  {
    const float viewH = mViewportYMax - mViewportYMin;
    const float newH  = viewH * factor;
    const float ratio = (cursorDataY - mViewportYMin) / std::max(1e-6f, viewH);
    mViewportYMin     = cursorDataY - ratio * newH;
    mViewportYMax     = mViewportYMin + newH;
  }

  if(mModel.mStyle.interaction.zoomClampEnabled) ClampViewport();
  if(mModel.mStyle.interaction.autoFitY && zoomX) FitYToViewport();
  mViewportActive = true;
  ApplyViewportToScale();
  RebuildBackground();
  RebuildData();
  return true;
}

bool ChartViewImpl::RemoveSeries(const Dali::String& name)
{
  int idx = -1;
  for(int i = 0; i < mModel.GetSeriesCount(); ++i)
  {
    if(mModel.mSeriesList[i].GetName() == name)
    {
      idx = i;
      break;
    }
  }

  if(idx >= 0)
  {
    GetImplementation(mModel.mSeriesList[idx])
      .DataChangedSignal()
      .Disconnect(this, &ChartViewImpl::OnSeriesDataChanged);
    mModel.mSeriesList.erase(mModel.mSeriesList.begin() + idx);
    mModel.ComputeAutoRange();
    UpdateScale();
    mNeedsDataUpdate       = true;
    mNeedsBackgroundUpdate = true;
    RelayoutRequest();
    return true;
  }
  return false;
}

void ChartViewImpl::RemoveAllSeries()
{
  for(auto& s : mModel.mSeriesList)
  {
    GetImplementation(s).DataChangedSignal().Disconnect(this, &ChartViewImpl::OnSeriesDataChanged);
  }
  mModel.RemoveAllSeries();
  mModel.ComputeAutoRange();
  UpdateScale();
  mNeedsDataUpdate       = true;
  mNeedsBackgroundUpdate = true;
  RelayoutRequest();
}

// =============================================================================
// Sections
// =============================================================================

void ChartViewImpl::AddSection(Ui::ChartSection section)
{
  mModel.mSections.push_back(section);
  mNeedsBackgroundUpdate = true;
  RelayoutRequest();
}

void ChartViewImpl::RemoveSection(Ui::ChartSection section)
{
  auto& v = mModel.mSections;
  v.erase(std::remove(v.begin(), v.end(), section), v.end());
  mNeedsBackgroundUpdate = true;
  RelayoutRequest();
}

void ChartViewImpl::ClearSections()
{
  mModel.mSections.clear();
  mNeedsBackgroundUpdate = true;
  RelayoutRequest();
}

void ChartViewImpl::SetTitle(const Dali::String& title)
{
  mModel.mTitle          = title;
  mNeedsBackgroundUpdate = true;
  RelayoutRequest();
}

Dali::String ChartViewImpl::GetTitle() const
{
  return mModel.mTitle;
}

void ChartViewImpl::SetTitlePosition(Ui::ChartView::TitlePosition position)
{
  mModel.mStyle.layout.titlePosition = static_cast<int>(position);
  mNeedsBackgroundUpdate             = true;
  RelayoutRequest();
}

Ui::ChartView::TitlePosition ChartViewImpl::GetTitlePosition() const
{
  return static_cast<Ui::ChartView::TitlePosition>(mModel.mStyle.layout.titlePosition);
}

void ChartViewImpl::SetTitleColor(const Vector4& color)
{
  mModel.mStyle.render.titleColor = color;
  mNeedsBackgroundUpdate          = true;
  RelayoutRequest();
}

Vector4 ChartViewImpl::GetTitleColor() const
{
  return mModel.mStyle.render.titleColor;
}

// =============================================================================
// Gauge
// =============================================================================

void ChartViewImpl::SetGaugeValue(float v)
{
  mModel.mStyle.gauge.value = v;
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeMinValue(float v)
{
  mModel.mStyle.gauge.minValue = v;
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeMaxValue(float v)
{
  mModel.mStyle.gauge.maxValue = v;
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeArcSpan(float degrees)
{
  mModel.mStyle.gauge.arcSpanDegrees = std::clamp(degrees, 0.1f, 360.0f);
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeStartAngle(float degrees)
{
  mModel.mStyle.gauge.startAngleDegrees = degrees;
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeArcWidth(float ratio)
{
  mModel.mStyle.gauge.arcWidthRatio = std::clamp(ratio, 0.01f, 0.5f);
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeTrackColor(const Vector4& color)
{
  mModel.mStyle.gauge.trackColor = color;
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeProgressColor(const Vector4& color)
{
  mModel.mStyle.gauge.progressColor = color;
  OnSeriesDataChanged();
}

void ChartViewImpl::SetGaugeCenterLabel(const Dali::String& text)
{
  mModel.mStyle.gauge.centerLabel = text;
  OnSeriesDataChanged();
}

void ChartViewImpl::AddGaugeRange(float fromValue, float toValue, const Vector4& color)
{
  StyleConfig::GaugeRange r;
  r.fromValue = fromValue;
  r.toValue   = toValue;
  r.color     = color;
  mModel.mStyle.gauge.ranges.push_back(r);
  OnSeriesDataChanged();
}

void ChartViewImpl::ClearGaugeRanges()
{
  mModel.mStyle.gauge.ranges.clear();
  OnSeriesDataChanged();
}

// =============================================================================
// Property
// =============================================================================

void ChartViewImpl::SetProperty(BaseObject* object, Property::Index propertyIndex, const Property::Value& value)
{
  Ui::View view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    ChartViewImpl& impl = static_cast<ChartViewImpl&>(GetImpl(view));
    switch(propertyIndex)
    {
      case Ui::ChartView::Property::SHOW_GRID:
        value.Get(impl.mModel.mStyle.visibility.showGrid);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::SHOW_LEGEND:
        value.Get(impl.mModel.mStyle.visibility.showLegend);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::SHOW_TOOLTIP:
        value.Get(impl.mModel.mStyle.visibility.showTooltip);
        break;
      case Ui::ChartView::Property::BACKGROUND_COLOR:
        value.Get(impl.mModel.mStyle.render.backgroundColor);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::GRID_COLOR:
        value.Get(impl.mModel.mStyle.render.gridColor);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::ANIMATION_DURATION:
        value.Get(impl.mModel.mStyle.animation.duration);
        break;
      case Ui::ChartView::Property::Y_AXIS_AUTO_RANGE:
      {
        bool autoRange = true;
        if(value.Get(autoRange))
        {
          auto& yAxisImpl = GetImplementation(impl.mModel.mYAxis);
          yAxisImpl.SetAutoRange(autoRange);
          if(autoRange)
          {
            impl.mModel.ComputeAutoRange();
            impl.UpdateScale();
          }
        }
        break;
      }
      case Ui::ChartView::Property::LEGEND_POSITION:
        value.Get(impl.mModel.mStyle.layout.legendPosition);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::AXIS_LABEL_SIZE:
        value.Get(impl.mModel.mStyle.layout.axisLabelSize);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::TITLE_SIZE:
        value.Get(impl.mModel.mStyle.layout.titleSize);
        impl.mNeedsBackgroundUpdate = true;
        break;
      case Ui::ChartView::Property::LINE_WIDTH:
        value.Get(impl.mModel.mStyle.render.lineWidth);
        impl.mNeedsDataUpdate = true;
        break;
      case Ui::ChartView::Property::SHOW_MARKERS:
        value.Get(impl.mModel.mStyle.visibility.showMarkers);
        impl.mNeedsDataUpdate = true;
        break;
      case Ui::ChartView::Property::MARKER_RADIUS:
        value.Get(impl.mModel.mStyle.render.markerRadius);
        impl.mNeedsDataUpdate = true;
        break;
      case Ui::ChartView::Property::HOVER_ENABLED:
        value.Get(impl.mModel.mStyle.interaction.hoverEnabled);
        if(!impl.mModel.mStyle.interaction.hoverEnabled)
        {
          impl.HideOverlay();
          impl.ClearLegendHighlight();
        }
        break;
      case Ui::ChartView::Property::TOUCH_ENABLED:
        value.Get(impl.mModel.mStyle.interaction.touchEnabled);
        if(!impl.mModel.mStyle.interaction.touchEnabled)
        {
          impl.mTouchActive = false;
          impl.HideOverlay();
        }
        break;
    }
    impl.RelayoutRequest();
  }
}

Property::Value ChartViewImpl::GetProperty(BaseObject* object, Property::Index propertyIndex)
{
  Property::Value value;
  Ui::View        view = Ui::View::DownCast(Dali::BaseHandle(object));
  if(view)
  {
    const ChartViewImpl& impl = static_cast<const ChartViewImpl&>(GetImpl(view));
    switch(propertyIndex)
    {
      case Ui::ChartView::Property::SHOW_GRID:
        value = impl.mModel.mStyle.visibility.showGrid;
        break;
      case Ui::ChartView::Property::SHOW_LEGEND:
        value = impl.mModel.mStyle.visibility.showLegend;
        break;
      case Ui::ChartView::Property::SHOW_TOOLTIP:
        value = impl.mModel.mStyle.visibility.showTooltip;
        break;
      case Ui::ChartView::Property::BACKGROUND_COLOR:
        value = impl.mModel.mStyle.render.backgroundColor;
        break;
      case Ui::ChartView::Property::GRID_COLOR:
        value = impl.mModel.mStyle.render.gridColor;
        break;
      case Ui::ChartView::Property::ANIMATION_DURATION:
        value = impl.mModel.mStyle.animation.duration;
        break;
      case Ui::ChartView::Property::Y_AXIS_AUTO_RANGE:
        value = GetImplementation(impl.mModel.mYAxis).GetAutoRange();
        break;
      case Ui::ChartView::Property::LEGEND_POSITION:
        value = impl.mModel.mStyle.layout.legendPosition;
        break;
      case Ui::ChartView::Property::AXIS_LABEL_SIZE:
        value = impl.mModel.mStyle.layout.axisLabelSize;
        break;
      case Ui::ChartView::Property::TITLE_SIZE:
        value = impl.mModel.mStyle.layout.titleSize;
        break;
      case Ui::ChartView::Property::LINE_WIDTH:
        value = impl.mModel.mStyle.render.lineWidth;
        break;
      case Ui::ChartView::Property::SHOW_MARKERS:
        value = impl.mModel.mStyle.visibility.showMarkers;
        break;
      case Ui::ChartView::Property::MARKER_RADIUS:
        value = impl.mModel.mStyle.render.markerRadius;
        break;
      case Ui::ChartView::Property::HOVER_ENABLED:
        value = impl.mModel.mStyle.interaction.hoverEnabled;
        break;
      case Ui::ChartView::Property::TOUCH_ENABLED:
        value = impl.mModel.mStyle.interaction.touchEnabled;
        break;
    }
  }
  return value;
}

// =============================================================================
// Internal helpers
// =============================================================================

void ChartViewImpl::SyncLayerSizes()
{
  if(mBackgroundCanvas)
  {
    mBackgroundCanvas.SetProperty(Actor::Property::SIZE, mSize);
    mBackgroundCanvas.SetProperty(Ui::CanvasView::Property::VIEW_BOX, mSize);
  }
  if(mDataCanvas)
  {
    mDataCanvas.SetProperty(Actor::Property::SIZE, mSize);
    mDataCanvas.SetProperty(Ui::CanvasView::Property::VIEW_BOX, mSize);
  }
  if(mOverlayCanvas)
  {
    mOverlayCanvas.SetProperty(Actor::Property::SIZE, mSize);
    mOverlayCanvas.SetProperty(Ui::CanvasView::Property::VIEW_BOX, mSize);
  }
}

void ChartViewImpl::UpdateScale()
{
  auto& xAxisImpl = GetImplementation(mModel.mXAxis);
  auto& yAxisImpl = GetImplementation(mModel.mYAxis);
  mScale.SetDataRange(xAxisImpl.GetMinLimit(), xAxisImpl.GetMaxLimit(),
                      yAxisImpl.GetMinLimit(), yAxisImpl.GetMaxLimit());
}

// =============================================================================
// RebuildBackground
// =============================================================================

void ChartViewImpl::RebuildBackground()
{
  if(!mBackgroundCanvas || mSize.width < 1.0f || mSize.height < 1.0f) return;

  mModel.mChartType = static_cast<int>(mType);

  SyncLayerSizes();
  UpdateScale();

  mLastLayout = mLayoutManager.ComputeLayout(mSize, mModel, mScale);
  mBackgroundRenderer.Render(mBackgroundCanvas, mModel, mScale, mLastLayout);
  mBackgroundCanvas.RequestRasterization();

  PlaceTextLabels(mLastLayout);

  DALI_LOG_DEBUG_INFO("ChartViewImpl RebuildBackground DONE: size=%.0fx%.0f\n",
                      mSize.width, mSize.height);
}

// =============================================================================
// RebuildData
// =============================================================================

void ChartViewImpl::RebuildData()
{
  if(!mDataCanvas || mSize.width < 1.0f || mSize.height < 1.0f) return;

  auto labelInfos = mDataRenderer.Render(mDataCanvas, mModel, mScale, mLastLayout);
  mDataCanvas.RequestRasterization();

  size_t used = 0;
  for(const auto& info : labelInfos)
  {
    Ui::Label label = GetOrCreateLabel(mDataLabels, used);
    label.SetProperty(Ui::Label::Property::TEXT, info.text);
    label.SetProperty(Ui::Label::Property::TEXT_COLOR, info.color);
    label.SetProperty(Ui::Label::Property::FONT_SIZE, info.size);
    label.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    label.SetProperty(Actor::Property::PIVOT, info.pivot);
    label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    label.SetProperty(Actor::Property::POSITION, info.position);
    label.SetProperty(Actor::Property::VISIBLE, true);
    ++used;
  }
  HideExcessLabels(mDataLabels, used);

  mLastRenderedCanvasY = CaptureCanvasY();
}

// =============================================================================
// PlaceTextLabels helpers
// =============================================================================

void ChartViewImpl::PlaceTickLabels(std::vector<Ui::Label>&                           pool,
                                    const std::vector<ChartLayoutManager::TickLabel>& ticks,
                                    const TickLabelStyle&                             ls)
{
  for(size_t i = 0; i < ticks.size(); ++i)
  {
    Ui::Label label = GetOrCreateLabel(pool, i);
    label.SetProperty(Ui::Label::Property::TEXT, ticks[i].text.CStr());
    label.SetProperty(Ui::Label::Property::TEXT_COLOR, Color::BLACK);
    label.SetProperty(Ui::Label::Property::FONT_SIZE, ls.fontSize);
    label.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, ls.horizontalAlign.CStr());
    if(!ls.verticalAlign.Empty())
      label.SetProperty(Ui::Label::Property::VERTICAL_ALIGNMENT, ls.verticalAlign.CStr());
    label.SetProperty(Actor::Property::PIVOT, ls.pivot);
    label.SetProperty(Actor::Property::PARENT_ORIGIN, ls.parentOrigin);
    label.SetProperty(Actor::Property::POSITION, Vector3(ticks[i].position.x, ticks[i].position.y, 0.0f));
    if(std::abs(ticks[i].angle) > 0.01f)
      label.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(ticks[i].angle), Vector3::ZAXIS));
    label.SetProperty(Actor::Property::VISIBLE, true);
  }
  HideExcessLabels(pool, ticks.size());
}

void ChartViewImpl::PlaceTextLabels(const ChartLayoutManager::LayoutResult& layout)
{
  const StyleConfig& style = mModel.mStyle;

  auto placeOrHide = [&](Ui::Label& label, bool visible,
                         const std::function<void(Ui::Label&)>& configure)
  {
    if(visible)
    {
      if(!label)
      {
        label = Ui::Label::New();
        label.SetProperty(Actor::Property::SENSITIVE, false);
        // ViewImpl::OnArrange is not called, so the label position set via
        // Actor::Property::POSITION is never overridden by the layout system.
        // POSITION_USES_PIVOT=true is required so that non-TOP_LEFT pivots
        // (e.g. TOP_CENTER for titles, CENTER_RIGHT for Y-tick labels) are
        // honoured when computing the final screen position.
        label.SetProperty(Actor::Property::POSITION_USES_PIVOT, true);
        Self().Add(label);
      }
      configure(label);
      label.SetProperty(Actor::Property::VISIBLE, true);
    }
    else if(label)
    {
      label.SetProperty(Actor::Property::VISIBLE, false);
    }
  };

  // Title
  placeOrHide(mTitleLabel, layout.hasTitle, [&](Ui::Label& l)
  {
    Vector3 pivot  = Pivot::TOP_CENTER;
    Vector3 origin = ParentOrigin::TOP_LEFT;
    Vector2 pos    = layout.titlePos;
    switch(style.layout.titlePosition)
    {
      case 1:
        pivot  = Pivot::TOP_LEFT;
        origin = ParentOrigin::TOP_LEFT;
        pos.x  = ChartLayoutManager::PADDING;
        break;
      case 2:
        pivot  = Pivot::TOP_RIGHT;
        origin = ParentOrigin::TOP_RIGHT;
        pos.x  = -ChartLayoutManager::PADDING;
        break;
      case 3:
        pivot  = Pivot::BOTTOM_CENTER;
        origin = ParentOrigin::BOTTOM_CENTER;
        pos.x  = 0.0f;
        pos.y  = -ChartLayoutManager::PADDING;
        break;
      default:
        break;
    }
    l.SetProperty(Ui::Label::Property::TEXT, mModel.mTitle.CStr());
    l.SetProperty(Ui::Label::Property::TEXT_COLOR, style.render.titleColor);
    l.SetProperty(Ui::Label::Property::FONT_SIZE, style.layout.titleSize);
    l.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    l.SetProperty(Actor::Property::PIVOT, pivot);
    l.SetProperty(Actor::Property::PARENT_ORIGIN, origin);
    l.SetProperty(Actor::Property::POSITION, pos);
  });

  // X-axis title
  placeOrHide(mXAxisTitleLabel, layout.hasXAxisTitle, [&](Ui::Label& l)
  {
    l.SetProperty(Actor::Property::PIVOT, Pivot::TOP_CENTER);
    l.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    l.SetProperty(Ui::Label::Property::TEXT, GetImplementation(mModel.mXAxis).GetTitle().CStr());
    l.SetProperty(Ui::Label::Property::TEXT_COLOR, Color::BLACK);
    l.SetProperty(Ui::Label::Property::FONT_SIZE, style.layout.axisLabelSize);
    l.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    l.SetProperty(Actor::Property::POSITION, layout.xAxisTitlePos);
  });

  // Y-axis title (rotated 90 degrees counter-clockwise)
  placeOrHide(mYAxisTitleLabel, layout.hasYAxisTitle, [&](Ui::Label& l)
  {
    l.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
    l.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    l.SetProperty(Actor::Property::ORIENTATION, Quaternion(Degree(-90.0f), Vector3::ZAXIS));
    l.SetProperty(Ui::Label::Property::TEXT, GetImplementation(mModel.mYAxis).GetTitle().CStr());
    l.SetProperty(Ui::Label::Property::TEXT_COLOR, Color::BLACK);
    l.SetProperty(Ui::Label::Property::FONT_SIZE, style.layout.axisLabelSize);
    l.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    l.SetProperty(Actor::Property::POSITION, layout.yAxisTitlePos);
  });

  const float fs = style.layout.axisLabelSize;

  PlaceTickLabels(mYTickLabels, layout.yTickLabels,
                  {"END", "CENTER", Pivot::CENTER_RIGHT, ParentOrigin::TOP_LEFT, fs});

  PlaceTickLabels(mXTickLabels, layout.xTickLabels,
                  {"CENTER", "", Pivot::TOP_CENTER, ParentOrigin::TOP_LEFT, fs});

  // Legend text labels
  if(layout.hasLegend && style.visibility.showLegend)
  {
    for(size_t i = 0; i < layout.legendItems.size(); ++i)
    {
      const ChartLayoutManager::LegendItem& item  = layout.legendItems[i];
      Ui::Label                             label = GetOrCreateLabel(mLegendLabels, i);
      label.SetProperty(Ui::Label::Property::TEXT, item.name.CStr());
      label.SetProperty(Ui::Label::Property::TEXT_COLOR, Color::BLACK);
      label.SetProperty(Ui::Label::Property::FONT_SIZE, fs);
      label.SetProperty(Ui::Label::Property::HORIZONTAL_ALIGNMENT, "BEGIN");
      label.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
      label.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
      label.SetProperty(Actor::Property::POSITION, Vector3(item.textPos.x, item.textPos.y, 0.0f));
      label.SetProperty(Actor::Property::VISIBLE, true);
    }
    HideExcessLabels(mLegendLabels, layout.legendItems.size());
  }
  else
  {
    HideExcessLabels(mLegendLabels, 0);
  }
}

// =============================================================================
// TextLabel pool helpers
// =============================================================================

Ui::Label ChartViewImpl::GetOrCreateLabel(std::vector<Ui::Label>& pool, size_t index)
{
  if(index < pool.size())
  {
    return pool[index];
  }
  Ui::Label label = Ui::Label::New();
  label.SetProperty(Actor::Property::SENSITIVE, false);
  // ViewImpl::OnArrange is not called, so Actor::Property::POSITION set by
  // PlaceTextLabels is never reset by the layout system.
  // POSITION_USES_PIVOT=true enables pivot-based anchor alignment used by
  // tick labels (TOP_CENTER, CENTER_RIGHT) and other text elements.
  label.SetProperty(Actor::Property::POSITION_USES_PIVOT, true);
  Self().Add(label);
  pool.push_back(label);
  return label;
}

void ChartViewImpl::HideExcessLabels(std::vector<Ui::Label>& pool, size_t usedCount)
{
  if(usedCount >= pool.size()) return;

  Actor self = Self();
  for(size_t i = usedCount; i < pool.size(); ++i)
  {
    if(pool[i])
    {
      self.Remove(pool[i]);
    }
  }
  pool.erase(pool.begin() + static_cast<ptrdiff_t>(usedCount), pool.end());
}

void ChartViewImpl::ClearLabelPool(std::vector<Ui::Label>& pool)
{
  Actor self = Self();
  for(auto& label : pool)
  {
    if(label)
    {
      self.Remove(label);
    }
  }
  pool.clear();
}

// =============================================================================
// Touch interaction
// =============================================================================

bool ChartViewImpl::OnTouch(Actor actor, TouchEvent event)
{
  if(!mModel.mStyle.interaction.touchEnabled) return false;

  const PointState::Type state = event.GetState(0);
  const Vector2          local = event.GetLocalPosition(0u);

  // PIE: angular slice hit test
  if(mModel.mChartType == static_cast<int>(Ui::ChartView::Type::PIE))
  {
    if(state == PointState::STARTED)
    {
      int       seriesIdx = -1;
      const int sliceIdx  = HitTestPie(local, seriesIdx);
      if(sliceIdx >= 0 && seriesIdx >= 0 && !mDataPointSelectedSignal.Empty())
      {
        auto ps = Ui::PieSeries::DownCast(
          const_cast<Ui::ChartSeries&>(mModel.mSeriesList[static_cast<size_t>(seriesIdx)]));
        const auto& slice = GetImplementation(ps).GetSlices()[static_cast<size_t>(sliceIdx)];

        Ui::ChartPointEventArgs args;
        args.seriesIndex = seriesIdx;
        args.pointIndex  = sliceIdx;
        args.dataX       = static_cast<float>(sliceIdx);
        args.dataY       = slice.value;
        args.seriesName  = GetImplementation(ps).GetName();
        args.xLabel      = slice.label;
        mDataPointSelectedSignal.Emit(args);
      }
    }
    return true;
  }

  if(state == PointState::STARTED)
  {
    auto now          = std::chrono::steady_clock::now();
    auto elapsedMs    = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastTapTimePoint).count();
    mLastTapTimePoint = now;

    if(elapsedMs < DOUBLE_TAP_MS && mViewportActive)
    {
      ResetZoom();
      return true;
    }

    if(mModel.mStyle.interaction.zoomModeFlags != 0)
    {
      HandleLegendTap(local);
      return true;
    }
  }

  if(state == PointState::STARTED || state == PointState::MOTION)
  {
    if(state == PointState::STARTED && HandleLegendTap(local)) return true;

    if(!mHitTester.IsInsidePlotArea(local, mScale))
    {
      HideOverlay();
      return true;
    }

    const bool emitSignal = (state == PointState::STARTED);
    mTouchActive          = true;
    PerformHitAtPos(local, emitSignal);
  }
  else if(state == PointState::FINISHED || state == PointState::INTERRUPTED)
  {
    mTouchActive = false;
    HideOverlay();
  }

  return true;
}

int ChartViewImpl::HitTestPie(const Vector2& local, int& outSeriesIdx) const
{
  outSeriesIdx = -1;

  const Integration::PieSeries* pieImpl = nullptr;
  for(int i = 0; i < static_cast<int>(mModel.mSeriesList.size()); ++i)
  {
    auto ps = Ui::PieSeries::DownCast(
      const_cast<Ui::ChartSeries&>(mModel.mSeriesList[static_cast<size_t>(i)]));
    if(ps && GetImplementation(ps).IsVisible())
    {
      pieImpl      = &GetImplementation(ps);
      outSeriesIdx = i;
      break;
    }
  }
  if(!pieImpl || pieImpl->GetSliceCount() == 0) return -1;

  const Rect<float>& pa     = mLastLayout.plotArea;
  const float        side   = std::min(pa.width, pa.height);
  const float        outerR = side * 0.5f;
  const Vector2      center(pa.x + pa.width * 0.5f, pa.y + pa.height * 0.5f);
  const float        innerR = outerR * pieImpl->GetInnerRadiusRatio();

  const float dx   = local.x - center.x;
  const float dy   = local.y - center.y;
  const float dist = std::sqrt(dx * dx + dy * dy);
  if(dist > outerR) return -1;
  if(pieImpl->GetInnerRadiusRatio() > 0.0f && dist < innerR) return -1;

  float angle = std::atan2(dy, dx) * 180.0f / static_cast<float>(M_PI);
  if(angle < 0.0f) angle += 360.0f;
  angle = std::fmod(angle - 270.0f + 360.0f, 360.0f);

  const auto& slices = pieImpl->GetSlices();
  float       total  = 0.0f;
  for(const auto& sl : slices) total += sl.value;
  if(total <= 0.0f) return -1;

  const float gap      = pieImpl->GetSliceGap();
  float       cumAngle = 0.0f;
  for(int i = 0; i < static_cast<int>(slices.size()); ++i)
  {
    const float fullSweep   = (slices[static_cast<size_t>(i)].value / total) * 360.0f;
    const float renderStart = cumAngle + gap * 0.5f;
    const float renderEnd   = cumAngle + fullSweep - gap * 0.5f;
    if(angle >= renderStart && angle < renderEnd) return i;
    cumAngle += fullSweep;
  }
  return -1;
}

void ChartViewImpl::PerformHitAtPos(const Vector2& localPos, bool emitSignal)
{
  const float threshold = mModel.mStyle.interaction.hitThreshold;
  const int   strategy  = mModel.mStyle.interaction.findingStrategy;

  if(strategy == 0) // NEAREST
  {
    HitResult hit = mHitTester.FindNearest(localPos, mModel, mScale, threshold);
    if(hit.isValid)
    {
      mLastHit = hit;
      UpdateOverlay(hit);
      if(emitSignal && !mDataPointSelectedSignal.Empty())
      {
        Ui::ChartPointEventArgs args;
        args.seriesIndex = hit.seriesIndex;
        args.pointIndex  = hit.pointIndex;
        args.dataX       = hit.dataX;
        args.dataY       = hit.dataY;
        args.seriesName  = hit.seriesName;
        args.xLabel      = hit.xLabel;
        mDataPointSelectedSignal.Emit(args);
      }
    }
    else
    {
      HideOverlay();
    }
  }
  else // SAME_X or SAME_X_NEAREST_Y
  {
    bool nearestY = (strategy == 2);
    auto hits     = mHitTester.FindBySameX(localPos, mModel, mScale, threshold, nearestY);
    if(!hits.empty())
    {
      mLastHit = hits[0];
      UpdateOverlayMulti(hits);
      if(emitSignal && !mMultiPointSelectedSignal.Empty())
      {
        for(const auto& h : hits)
        {
          Ui::ChartPointEventArgs args;
          args.seriesIndex = h.seriesIndex;
          args.pointIndex  = h.pointIndex;
          args.dataX       = h.dataX;
          args.dataY       = h.dataY;
          args.seriesName  = h.seriesName;
          args.xLabel      = h.xLabel;
          mMultiPointSelectedSignal.Emit(args);
        }
      }
    }
    else
    {
      HideOverlay();
    }
  }
}

std::string ChartViewImpl::BuildMultiTooltipText(const std::vector<HitResult>& hits) const
{
  std::string text;
  for(size_t i = 0; i < hits.size(); ++i)
  {
    const auto& h = hits[i];
    if(i > 0) text += "\n";
    if(mTooltipFormatter)
    {
      text += mTooltipFormatter(h.seriesName, h.xLabel, h.dataY).CStr();
    }
    else
    {
      char buf[64];
      std::snprintf(buf, sizeof(buf), "%.4g", h.dataY);
      text += std::string(h.seriesName.CStr()) + ": " + buf;
    }
  }
  return text;
}

void ChartViewImpl::UpdateOverlayMulti(const std::vector<HitResult>& hits)
{
  if(hits.empty()) return;

  mOverlayCanvas.RemoveAllDrawables();
  mOverlayRenderer.RenderCrosshair(mOverlayCanvas, hits[0].canvasPos, mScale, mModel.mStyle);
  for(const auto& h : hits)
    mOverlayRenderer.RenderHighlight(mOverlayCanvas, h.canvasPos,
                                     h.seriesColor, mModel.mStyle.render.markerRadius * 1.8f);
  mOverlayCanvas.RequestRasterization();

  const std::string text = BuildMultiTooltipText(hits);
  const Vector2     tooltipSize(150.0f, 20.0f + static_cast<float>(hits.size()) * 18.0f);
  const Vector2     pos = ComputeTooltipPosition(hits[0].canvasPos, tooltipSize);

  if(mModel.mStyle.visibility.showTooltip)
  {
    mTooltipLabel.SetProperty(Ui::Label::Property::TEXT, text.c_str());
    mTooltipLabel.SetProperty(Actor::Property::POSITION, Vector3(pos.x, pos.y, 1.0f));
    mTooltipLabel.SetProperty(Actor::Property::SIZE, Vector3(tooltipSize.x, tooltipSize.y, 0.0f));
    mTooltipLabel.SetProperty(Actor::Property::VISIBLE, true);
  }
}

void ChartViewImpl::UpdateOverlay(const HitResult& hit)
{
  mOverlayCanvas.RemoveAllDrawables();
  mOverlayRenderer.RenderCrosshair(mOverlayCanvas, hit.canvasPos, mScale, mModel.mStyle);
  mOverlayRenderer.RenderHighlight(mOverlayCanvas, hit.canvasPos,
                                   hit.seriesColor, mModel.mStyle.render.markerRadius * 1.8f);
  mOverlayCanvas.RequestRasterization();

  std::string text;
  char        buf[64];
  if(mTooltipFormatter)
  {
    text = mTooltipFormatter(hit.seriesName, hit.xLabel, hit.dataY).CStr();
  }
  else
  {
    std::snprintf(buf, sizeof(buf), "%.4g", hit.dataY);
    text = std::string(hit.seriesName.CStr()) + "\n" + hit.xLabel.CStr() + " : " + buf;
  }

  const Vector2 tooltipSize(130.0f, 52.0f);
  const Vector2 pos = ComputeTooltipPosition(hit.canvasPos, tooltipSize);

  if(mModel.mStyle.visibility.showTooltip)
  {
    mTooltipLabel.SetProperty(Ui::Label::Property::TEXT, text.c_str());
    mTooltipLabel.SetProperty(Actor::Property::POSITION, Vector3(pos.x, pos.y, 1.0f));
    mTooltipLabel.SetProperty(Actor::Property::SIZE, Vector3(tooltipSize.x, tooltipSize.y, 0.0f));
    mTooltipLabel.SetProperty(Actor::Property::VISIBLE, true);
  }
}

void ChartViewImpl::HideOverlay()
{
  mOverlayRenderer.Clear(mOverlayCanvas);
  if(mTooltipLabel)
  {
    mTooltipLabel.SetProperty(Actor::Property::VISIBLE, false);
  }
}

Vector2 ChartViewImpl::ComputeTooltipPosition(const Vector2& hitPos, const Vector2& tooltipSize) const
{
  constexpr float MARGIN = 10.0f;

  float x = hitPos.x - tooltipSize.x * 0.5f;
  float y = hitPos.y - tooltipSize.y - MARGIN;

  if(y < MARGIN) y = hitPos.y + MARGIN;

  x = std::max(MARGIN, std::min(x, mSize.x - tooltipSize.x - MARGIN));
  return Vector2(x, y);
}

int ChartViewImpl::FindLegendItemAt(const Vector2& pos) const
{
  if(!mLastLayout.hasLegend || !mModel.mStyle.visibility.showLegend) return -1;

  constexpr float ITEM_HEIGHT = 20.0f;
  constexpr float HIT_PADDING = 8.0f;

  const size_t count = std::min(mLastLayout.legendItems.size(), mModel.mSeriesList.size());
  for(size_t i = 0; i < count; ++i)
  {
    const ChartLayoutManager::LegendItem& item = mLastLayout.legendItems[i];
    const bool                            yHit = pos.y >= item.textPos.y - HIT_PADDING &&
                      pos.y <= item.textPos.y + ITEM_HEIGHT + HIT_PADDING;
    const bool xHit = pos.x >= item.iconCenter.x - ChartLayoutManager::LEGEND_SWATCH;
    if(yHit && xHit) return static_cast<int>(i);
  }
  return -1;
}

bool ChartViewImpl::HandleLegendTap(const Vector2& tapPos)
{
  const int idx = FindLegendItemAt(tapPos);
  if(idx < 0) return false;

  auto& series      = mModel.mSeriesList[static_cast<size_t>(idx)];
  bool  nextVisible = !GetImplementation(series).IsVisible();

  if(mModel.mStyle.visibility.legendToggleEnabled)
  {
    GetImplementation(series).SetVisible(nextVisible);
  }

  if(!mLegendItemTappedSignal.Empty())
  {
    mLegendItemTappedSignal.Emit(idx, nextVisible);
  }
  return true;
}

void ChartViewImpl::HighlightLegendItem(int index)
{
  if(mHoveredLegendIndex == index) return;
  ClearLegendHighlight();

  if(index >= 0 && index < static_cast<int>(mLegendLabels.size()) && mLegendLabels[static_cast<size_t>(index)])
  {
    mLegendLabels[static_cast<size_t>(index)].SetProperty(Ui::Label::Property::TEXT_COLOR,
                                                          Vector4(0.1f, 0.35f, 0.85f, 1.0f));
    mHoveredLegendIndex = index;
  }
}

void ChartViewImpl::ClearLegendHighlight()
{
  if(mHoveredLegendIndex >= 0 &&
     mHoveredLegendIndex < static_cast<int>(mLegendLabels.size()) &&
     mLegendLabels[static_cast<size_t>(mHoveredLegendIndex)])
  {
    mLegendLabels[static_cast<size_t>(mHoveredLegendIndex)].SetProperty(Ui::Label::Property::TEXT_COLOR,
                                                                        Color::BLACK);
  }
  mHoveredLegendIndex = -1;
}

bool ChartViewImpl::OnHover(Actor actor, HoverEvent event)
{
  if(!mModel.mStyle.interaction.hoverEnabled) return false;

  if(mModel.mChartType == static_cast<int>(Ui::ChartView::Type::PIE)) return true;

  if(mTouchActive || mPanActive) return true;

  const PointState::Type state = event.GetState(0);
  const Vector2          local = event.GetLocalPosition(0u);

  if(state == PointState::STARTED || state == PointState::MOTION)
  {
    const int legendIdx = FindLegendItemAt(local);
    if(legendIdx >= 0)
    {
      HideOverlay();
      HighlightLegendItem(legendIdx);
      return true;
    }

    ClearLegendHighlight();

    if(mHitTester.IsInsidePlotArea(local, mScale))
    {
      PerformHitAtPos(local, false);
    }
    else
    {
      HideOverlay();
    }
  }
  else if(state == PointState::FINISHED || state == PointState::INTERRUPTED)
  {
    HideOverlay();
    ClearLegendHighlight();
  }

  return true;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
