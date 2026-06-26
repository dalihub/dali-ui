[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/ChartView-Architecture-(kr).md)

# DALi UI Components — ChartView Architecture

> This document describes the internal design of `ChartView` for component contributors and developers who need to understand the implementation.
> For usage information see [ChartView User Guide](ChartView.md).

---

## Table of Contents

1. [Overview](#1-overview)
2. [Package Structure](#2-package-structure)
3. [Layer Architecture](#3-layer-architecture)
4. [Public API Layer](#4-public-api-layer)
   - 4.1 [Handle Pattern](#41-handle-pattern)
   - 4.2 [GetImplementation — ADL Pattern](#42-getimplementation--adl-pattern)
5. [Integration API Layer](#5-integration-api-layer)
   - 5.1 [ChartViewImpl](#51-chartviewimpl)
   - 5.2 [Data Flow](#52-data-flow)
6. [Rendering Layer](#6-rendering-layer)
   - 6.1 [3-Canvas Split and Rationale](#61-3-canvas-split-and-rationale)
   - 6.2 [ChartRenderer](#62-chartrenderer)
   - 6.3 [ChartLayoutManager](#63-chartlayoutmanager)
7. [Data Model](#7-data-model)
8. [ScaleEngine](#8-scaleengine)
9. [HitTester](#9-hittester)
10. [Gesture Handling](#10-gesture-handling)
11. [Animation System](#11-animation-system)

---

## 1. Overview

`ChartView` renders Line, Bar, Pie, Area, Scatter, and Gauge charts on top of DALi UI Foundation. It uses multiple `CanvasView` layers backed by ThorVG for vector graphics and `Ui::Label` actors for all text (axis labels, titles, legend, tooltip).

Key design decisions:

- **3-canvas split** — separates static background, dynamic data, and interactive overlay into independent rasterization targets, so touch events only re-rasterize the thin overlay layer.
- **Throttle timer** — coalesces rapid `series.SetValues()` calls before triggering a layout/render cycle.
- **Direct `Rebuild*()` bypass** — after the initial layout, data updates call `RebuildData()` directly and invoke `RequestRasterization()` rather than going through `RelayoutRequest()`, because `RelayoutRequest()` does not wake the Adaptor.
- **Explicit gesture detectors** — `ViewImpl` does not expose `EnableGestureDetection`, so `ChartViewImpl` manages `PanGestureDetector` and `PinchGestureDetector` members directly.

---

## 2. Package Structure

```
dali-ui-components/
├── public-api/chart/
│   ├── chart-view.h          ← Public handle + enums + signals
│   ├── chart-series.h        ← Base series handle (BaseHandle-derived)
│   ├── line-series.h
│   ├── bar-series.h
│   ├── pie-series.h
│   ├── scatter-series.h
│   ├── chart-axis.h
│   └── chart-section.h
└── integration-api/chart/
    ├── chart-view-impl.h/.cpp   ← ViewImpl-derived implementation
    ├── chart-model.h            ← Data container + StyleConfig
    ├── chart-renderer.h/.cpp    ← BackgroundRenderer / DataRenderer / OverlayRenderer
    ├── chart-layout-manager.h/.cpp
    ├── chart-scale-engine.h     ← Data ↔ canvas coordinate mapping
    ├── chart-hit-tester.h/.cpp
    ├── chart-color-palette.h/.cpp ← Default series color sequence
    ├── chart-series-impl.h/.cpp  ← Base series impl (BaseObject-derived)
    ├── chart-axis-impl.h/.cpp
    ├── chart-section-impl.h/.cpp
    ├── line-series-impl.h/.cpp
    ├── bar-series-impl.h/.cpp
    ├── pie-series-impl.h/.cpp
    └── scatter-series-impl.h/.cpp
```

---

## 3. Layer Architecture

```
┌──────────────────────────────────────────────┐
│           Application Code                   │
│  ChartView  LineSeries  ChartAxis  ...        │
└──────────────────────┬───────────────────────┘
                       │  public-api/chart/
┌──────────────────────▼───────────────────────┐
│             Public API Layer                  │
│  Handle classes (ChartView : View,            │
│  ChartSeries : BaseHandle, ...)               │
└──────────────────────┬───────────────────────┘
                       │  integration-api/chart/
┌──────────────────────▼───────────────────────┐
│          Integration API Layer                │
│  ChartViewImpl  ChartSeriesImpl               │
│  ChartModel  StyleConfig                      │
└────────────┬──────────────┬──────────────────┘
             │              │
    ┌────────▼───┐   ┌──────▼──────────────────┐
    │ ScaleEngine│   │   Rendering Layer        │
    │ HitTester  │   │  BackgroundRenderer      │
    └────────────┘   │  DataRenderer            │
                     │  OverlayRenderer         │
                     │  ChartLayoutManager      │
                     └─────────────────────────┘
```

| Layer | Location | Visibility |
|-------|----------|------------|
| Public API | `public-api/chart/` | App developers |
| Provider API | `provider-api/` | DALi UI module providers |
| Integration API | `integration-api/chart/` | Component contributors |
| Rendering helpers | `integration-api/chart/` | Internal to `ChartViewImpl` |
| DALi UI Foundation | `dali-ui-foundation/` | Shared infrastructure |

---

## 4. Public API Layer

### 4.1 Handle Pattern

`ChartView` follows the same p-impl handle pattern as all dali-ui views (see [View Architecture](View.md)).

```cpp
// public-api/chart/chart-view.h
class ChartView : public View          // handle — holds only a pointer
{
public:
  static ChartView New(Type type, const Vector2& size);
  ChartView& AddSeries(Ui::ChartSeries series);
  // ...
};

// integration-api/chart/chart-view-impl.h
class ChartViewImpl : public ViewImpl  // impl — owns all state and logic
{
  ChartModel   mModel;
  ScaleEngine  mScale;
  CanvasView   mBackgroundCanvas;
  CanvasView   mDataCanvas;
  CanvasView   mOverlayCanvas;
  // ...
};
```

Copying a `ChartView` handle produces a second handle pointing to the same `ChartViewImpl`; the impl is destroyed when all handles are gone.

Series handles (`ChartSeries`, `LineSeries`, …) inherit from `Dali::BaseHandle` rather than `View` because series are not scene actors. Their impls (`Integration::ChartSeries`, …) inherit from `Dali::BaseObject`.

Rule summary:

| Item | Rule |
|------|------|
| Handle destructor | Must be **non-virtual** |
| Handle data members | None — all state lives in impl |
| Logic location | Impl only |
| Type registration | `DALI_TYPE_REGISTRATION_BEGIN` in impl `.cpp` |

<br/>

### 4.2 GetImplementation — ADL Pattern

`GetImplementation` helpers for series are defined inside `namespace Dali::Ui` (not inside `namespace Dali::Ui::Integration`). This placement makes them reachable via **Argument-Dependent Lookup** (ADL) without a namespace prefix.

```cpp
// integration-api/chart/chart-series-impl.h
// Defined at namespace Dali::Ui scope — outside Integration

inline Integration::ChartSeries& GetImplementation(Ui::ChartSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartSeries handle is empty");
  return static_cast<Integration::ChartSeries&>(handle.GetBaseObject());
}
```

Usage inside impl code:

```cpp
// ADL resolves the call — no prefix needed
GetImplementation(series).DataChangedSignal().Connect(...);   // correct

// Integration::GetImplementation(series)...                  // would fail — wrong namespace
```

> **Note:** `ChartView` itself uses `GetImpl()` (provided by `ViewImpl`) to obtain `ChartViewImpl&` from a `ChartView` handle, matching the standard dali-ui pattern.

---

## 5. Integration API Layer

### 5.1 ChartViewImpl

`ChartViewImpl` inherits `ViewImpl` and overrides two lifecycle methods.

**`OnInitialize()`** — called once after the DALi handle is constructed:

- Calls `ViewImpl::OnInitialize()` which sets `PIVOT=TOP_LEFT`, `PARENT_ORIGIN=TOP_LEFT`.
- Registers the constructor-specified size as `SetRequestedWidth/Height` so the layout system can measure the view even without `AbsoluteLayoutParams`.
- Creates three `CanvasView` layers with `SYNCHRONOUS_LOADING=true` and `RASTERIZATION_REQUEST_MANUALLY=true`, and adds them to `Self()`.
- Creates the throttle timer (16 ms, single-shot) and animation timer (16 ms, repeating).
- Connects `TouchedSignal` and `HoveredSignal` on `Self()`.

**`OnArrange(bounds)`** — called by the layout system each frame when the view needs repositioning:

```
ViewImpl::OnArrange is intentionally NOT called.

Reason: it iterates all child Views and calls Arrange(0,0,w,h) on
those without AbsoluteLayoutParams.  Canvas layers declare
MATCH_PARENT requested size, but the inherited ViewImpl layout does
not propagate that correctly for non-Layout parents, so their SIZE
is reset to 0×0 (WRAP_CONTENT natural size = 0).

Instead, OnArrange applies bounds directly to Self() and calls
Rebuild*() functions directly.  With PIVOT=TOP_LEFT (set by
ViewImpl::OnInitialize), POSITION maps to the top-left corner
without any pivot math.

Text label positions (axis ticks, legend, title) are set via
Actor::Property::POSITION in PlaceTextLabels().  Because
ViewImpl::OnArrange is skipped, the layout system never resets those
positions.  Labels also set POSITION_USES_PIVOT=true so that
non-TOP_LEFT pivots (e.g. CENTER_RIGHT for Y-tick labels,
TOP_CENTER for X-tick labels) are honoured when computing the final
screen position.
```

```cpp
MeasuredSize ChartViewImpl::OnArrange(const LayoutRect& bounds)
{
  // Intentionally skip ViewImpl::OnArrange — see comment above.
  Actor             self    = Self();
  const Vector2     newSize(bounds.width, bounds.height);

  self.SetProperty(Actor::Property::POSITION_X, bounds.x);
  self.SetProperty(Actor::Property::POSITION_Y, bounds.y);
  self.SetProperty(Actor::Property::SIZE_WIDTH,  bounds.width);
  self.SetProperty(Actor::Property::SIZE_HEIGHT, bounds.height);

  if(newSize != mSize && newSize.width > 0.0f && newSize.height > 0.0f)
  {
    mSize = newSize;
    mNeedsBackgroundUpdate = mNeedsDataUpdate = true;
  }
  if(mNeedsBackgroundUpdate) { RebuildBackground(); mNeedsBackgroundUpdate = false; }
  if(mNeedsDataUpdate)       { RebuildData();       mNeedsDataUpdate       = false; }

  return MeasuredSize(bounds.width, bounds.height);
}
```

<br/>

### 5.2 Data Flow

**Steady-state update triggered by `series.SetValues()`:**

```
series.SetValues(newData)
  │
  │  Integration::ChartSeries::EmitDataChangedSignal()
  ▼
ChartViewImpl::OnSeriesDataChanged()
  │  mPendingUpdate = true
  │  throttleMs < 1 → call directly, else mUpdateThrottleTimer.Start()
  ▼
ChartViewImpl::OnUpdateThrottleTimer()
  │  mModel.ComputeAutoRange()
  │  UpdateScale()
  │
  ├─ animation.duration > 0 ─► start mAnimTimer (16 ms repeating)
  │
  └─ no animation, mLastLayout valid ─► RebuildBackground() + RebuildData()
       │                                  RequestRasterization() on each canvas
       │                                  (wakes Adaptor immediately)
       └─ mLastLayout not valid ──────► mNeedsBackgroundUpdate = mNeedsDataUpdate = true
                                         RelayoutRequest()
                                         (deferred to next OnArrange frame)
```

> **Why bypass RelayoutRequest() for data updates?**
> `RelayoutRequest()` only registers the view with the `LayoutController`. It does not signal the Adaptor to schedule a new render pass. If a timer-driven animation calls `RelayoutRequest()` with no other events pending, the Adaptor stays idle and the chart freezes. Calling `RebuildData()` + `RequestRasterization()` directly wakes the Adaptor on every timer tick.

---

## 6. Rendering Layer

### 6.1 3-Canvas Split and Rationale

`ChartViewImpl` creates three `CanvasView` actors stacked at the same position and size:

```
Self() (ChartView actor)
 ├── mBackgroundCanvas  (z-order 0)
 ├── mDataCanvas        (z-order 1)
 └── mOverlayCanvas     (z-order 2)
```

| Canvas | Content | Re-rasterized when |
|--------|---------|-------------------|
| `mBackgroundCanvas` | Chart background fill, grid lines, axis lines, tick marks, legend swatches | Size changes, axis config changes, style changes |
| `mDataCanvas` | Series geometry — lines, fill areas, bars, markers, pie slices, gauge arcs, data labels | Data changes |
| `mOverlayCanvas` | Crosshair line, hit-point highlight circle | Touch / hover events |

This separation means an interactive hover event only re-rasterizes `mOverlayCanvas` (typically a thin crosshair), leaving the heavier `mBackgroundCanvas` and `mDataCanvas` untouched. The `mNeedsBackgroundUpdate` / `mNeedsDataUpdate` flags in `ChartViewImpl` enforce this separation.

All three canvases are configured with:
- `SYNCHRONOUS_LOADING = true` — rasterization happens on the render thread synchronously.
- `RASTERIZATION_REQUEST_MANUALLY = true` — the canvas does not auto-rasterize; `RequestRasterization()` must be called explicitly.
- `SENSITIVE = false` — touch events pass through to the `ChartView` actor.

<br/>

### 6.2 ChartRenderer

Three renderer classes, each taking a `Ui::CanvasView&` reference:

**`BackgroundRenderer`** renders static elements:

```
Render(canvas, model, scale, layout)
  ├── RenderBackground()    — background rect fill
  ├── RenderSections()      — ChartSection bands / threshold lines
  ├── RenderGrid()          — horizontal / vertical grid lines (dashed optional)
  ├── RenderAxes()          — X and Y axis lines
  ├── RenderTickMarks()     — short tick marks at each grid position
  └── RenderLegendSwatches()— colored square swatches next to legend text
```

**`DataRenderer`** renders dynamic series data and returns `DataLabelInfo` for text overlay:

```
Render(canvas, model, scale, layout, pOldYValues, animProgress)
  ├── RenderFillAreas()   — LineSeries fill polygons (AREA type / FillEnabled)
  ├── RenderLines()       — LineSeries polylines (smooth cubic Bézier optional)
  ├── RenderMarkers()     — per-point marker shapes (circle/square/triangle/diamond)
  ├── RenderScatters()    — ScatterSeries point cloud
  ├── RenderBars()        — BarSeries individual bars
  ├── RenderStackedBars() — BarSeries stacked bars
  ├── RenderPie()         — PieSeries arc slices
  ├── RenderGauge()       — Gauge arc track + progress arc
  └── RenderDataLabels()  → returns vector<DataLabelInfo>
```

`DataRenderer::Render()` returns `std::vector<DataLabelInfo>`. `ChartViewImpl` applies these to a `mDataLabels` pool of `Ui::Label` actors, creating new labels as needed and hiding excess ones.

**`OverlayRenderer`** renders interactive decorations:

```cpp
void RenderCrosshair(canvas, hitPos, scale, style);
void RenderHighlight(canvas, hitPos, color, radius);
void Clear(canvas);
```

<br/>

### 6.3 ChartLayoutManager

`ChartLayoutManager::ComputeLayout()` converts a `totalSize` + `ChartModel` into a `LayoutResult`:

```
ComputeLayout(totalSize, model, scale)
  │
  ├── Measure axis title text widths / heights
  ├── Measure Y tick label widths to determine left margin
  ├── Measure X tick label heights to determine bottom margin
  ├── Compute plotArea (Rect<float>) = totalSize minus all margins
  ├── Call ScaleEngine::ComputeNiceTicks() for Y and X axes
  ├── Build xTickLabels / yTickLabels (position + text + angle)
  ├── Build legendItems (icon center + text position + name)
  └── Return LayoutResult
```

`LayoutResult` is cached in `ChartViewImpl::mLastLayout` and reused by all three renderers and the `ChartHitTester`.

Key constants:

| Constant | Value | Purpose |
|----------|-------|---------|
| `PADDING` | 10 px | Outer chart padding |
| `TICK_LENGTH` | 5 px | Tick mark length |
| `TICK_LABEL_GAP` | 4 px | Gap between tick and label |
| `LEGEND_SWATCH` | 12 px | Legend color swatch size |
| `LEGEND_GAP` | 6 px | Gap between swatch and label text |

---

## 7. Data Model

`ChartModel` is a plain data container owned exclusively by `ChartViewImpl`. It holds:

```cpp
class ChartModel
{
  std::vector<Ui::ChartSeries>  mSeriesList;
  std::vector<Ui::ChartSection> mSections;
  Ui::ChartAxis                 mXAxis;
  Ui::ChartAxis                 mYAxis;
  StyleConfig                   mStyle;
  Dali::String                  mTitle;
  int                           mChartType{0};  // mirrors ChartView::Type

  void ComputeAutoRange();  // scans mSeriesList to set axis min/max limits
};
```

`ComputeAutoRange()` is called after every data or axis change. It handles:
- NaN point exclusion
- BarSeries baseline at zero
- Stacked bar cumulative sums
- Padding fractions (`SetDataPadding`) on both axes
- Bar series X-axis ±0.5 bar-width padding

`StyleConfig` aggregates all visual and behavioral settings into six nested structs:

| Sub-struct | Fields |
|------------|--------|
| `Visibility` | `showGrid`, `showLegend`, `showTooltip`, `showMarkers`, `legendToggleEnabled` |
| `RenderStyle` | `backgroundColor`, `gridColor`, `axisColor`, `titleColor`, `lineWidth`, `markerRadius` |
| `LayoutStyle` | `axisLabelSize`, `titleSize`, `legendPosition`, `titlePosition` |
| `Interaction` | `touchEnabled`, `hoverEnabled`, `hitThreshold`, `findingStrategy`, `zoomModeFlags`, `zoomClampEnabled`, `autoFitY` |
| `AnimationConfig` | `duration`, `easing` |
| `GaugeConfig` | `minValue`, `maxValue`, `value`, `arcSpanDegrees`, `startAngleDegrees`, `arcWidthRatio`, `trackColor`, `progressColor`, `centerLabel`, `ranges` |

---

## 8. ScaleEngine

`ScaleEngine` maps between **data coordinates** and **canvas pixel coordinates**. It is updated by `ChartViewImpl::UpdateScale()` after every `ComputeAutoRange()` call (or after zoom/pan changes the viewport).

```cpp
class ScaleEngine
{
  void SetPlotArea(const Rect<float>& area);
  void SetDataRange(float xMin, float xMax, float yMin, float yMax);

  float   ToCanvasX(float dataX) const;
  float   ToCanvasY(float dataY) const;  // Y-axis is inverted: data-up = canvas-up
  Vector2 ToCanvas(float dataX, float dataY) const;

  float ToDataX(float canvasX) const;
  float ToDataY(float canvasY) const;

  static std::vector<float> ComputeNiceTicks(float min, float max,
                                             int targetCount,
                                             float minStep = 0.0f);
};
```

Coordinate convention: `ToCanvasY` inverts the Y axis so that larger data values appear higher on screen.

`ComputeNiceTicks()` computes "nice" rounded tick values using the standard magnitude-fraction algorithm (steps at 1×, 2×, 5×, 10× powers of ten), optionally clamped to a `minStep` set via `ChartAxis::SetMinStep()`.

When zoom/pan is active (`mViewportActive == true`), `UpdateScale()` uses the viewport's data-space bounds rather than the full auto-range, so all rendering and hit-testing automatically operate within the zoomed window.

---

## 9. HitTester

`ChartHitTester` finds data points closest to a touch position. It operates in **actor-local coordinates**, which are identical to canvas pixel coordinates because all three `CanvasView` layers share the same origin and size as `Self()`.

```cpp
class ChartHitTester
{
  // Single nearest point across all visible series
  HitResult FindNearest(touchPos, model, scale, threshold) const;

  // One result per visible series at the nearest X position
  std::vector<HitResult> FindBySameX(touchPos, model, scale,
                                     threshold, nearestY) const;

  bool IsInsidePlotArea(touchPos, scale) const;
};
```

Strategy selection is controlled by `ChartView::SetFindingStrategy()`:

| `FindingStrategy` | Method used | Use case |
|-------------------|-------------|----------|
| `NEAREST` | `FindNearest` | Scatter / single series |
| `SAME_X` | `FindBySameX(nearestY=false)` | Multi-series tooltip |
| `SAME_X_NEAREST_Y` | `FindBySameX(nearestY=true)` | Multi-series, pick closest Y |

NaN data points are skipped during iteration. Invisible series (`SetVisible(false)`) are excluded from the search.

`HitResult` fields returned to `ChartViewImpl`:

| Field | Description |
|-------|-------------|
| `isValid` | `false` if no point was within threshold |
| `seriesIndex` / `pointIndex` | Location in model |
| `dataX` / `dataY` | Data-space coordinates |
| `canvasPos` | Canvas-space position for crosshair placement |
| `seriesColor` | Used to color the highlight circle |
| `xLabel` | X-axis label string or stringified `dataX` |

---

## 10. Gesture Handling

`ViewImpl` does not expose `EnableGestureDetection`, so `ChartViewImpl` owns its gesture detectors as explicit members:

```cpp
Dali::PanGestureDetector   mPanDetector;
Dali::PinchGestureDetector mPinchDetector;
```

Detectors are created and attached only when `SetZoomMode()` enables pan or zoom:

```cpp
void ChartViewImpl::SetZoomMode(int flags)
{
  // Attach on first activation
  if(isActive && !wasActive)
  {
    mPanDetector   = Dali::PanGestureDetector::New();
    mPinchDetector = Dali::PinchGestureDetector::New();
    mPanDetector.Attach(Self());
    mPinchDetector.Attach(Self());
    mPanDetector.DetectedSignal().Connect(this, &ChartViewImpl::OnPanGesture);
    mPinchDetector.DetectedSignal().Connect(this, &ChartViewImpl::OnPinchGesture);
  }
  // Detach when deactivated
  else if(!isActive && wasActive)
  {
    mPanDetector.Detach(Self());
    mPinchDetector.Detach(Self());
    mPanDetector.Reset();
    mPinchDetector.Reset();
  }
}
```

Gesture callbacks receive arguments by value, matching the DALi signal signature `Signal<void(Actor, PanGesture)>`:

```cpp
void OnPanGesture(Actor actor, Dali::PanGesture pan);
void OnPinchGesture(Actor actor, Dali::PinchGesture pinch);
```

**Pan** translates the viewport in data space. **Pinch** zooms around the data-space center of the pinch start. Both are clamped to the full data range when `SetZoomClampEnabled(true)` (default). After each gesture update, `ApplyViewportToScale()` pushes the new viewport bounds into `ScaleEngine` and triggers `RebuildBackground()` + `RebuildData()` directly (bypassing the layout system, for the same reason as the throttle timer path).

Zoom mode flags are bitfield-combinable:

```cpp
enum class ZoomMode : int
{
  NONE   = 0,
  PAN_X  = 1 << 0,
  PAN_Y  = 1 << 1,
  ZOOM_X = 1 << 2,
  ZOOM_Y = 1 << 3,
};
// Example: horizontal pan + horizontal zoom
chart.SetZoomMode(int(ZoomMode::PAN_X) | int(ZoomMode::ZOOM_X));
```

---

## 11. Animation System

When `SetAnimationDuration(ms)` is non-zero and data changes, `ChartViewImpl` snapshots the current canvas-Y positions before applying new data, then interpolates between old and new positions over the configured duration.

**Animation timer loop (16 ms tick):**

```
mAnimOldCanvasY = CaptureCanvasY()   ← snapshot before data changes
  │
series.SetValues(newData)
  │
OnSeriesDataChanged()
  │  mAnimStartTime = now
  │  mAnimTimer.Start()
  ▼
OnAnimTimer()  [fires every 16 ms]
  │  elapsed = now - mAnimStartTime
  │  rawT    = min(elapsed / duration, 1.0)
  │  t       = ApplyEasing(rawT, easing)
  │
  ├── RebuildDataAnimated(mAnimOldCanvasY, t)
  │     DataRenderer.Render(canvas, model, scale, layout,
  │                         &oldY, t)   ← lerp old→new Y
  │     mDataCanvas.RequestRasterization()
  │
  └── rawT >= 1.0 → stop timer, clear mAnimOldCanvasY
```

`CaptureCanvasY()` converts each series data point through `ScaleEngine::ToCanvasY()` into a flat `vector<float>`, preserving NaN gaps. During rendering, the `DataRenderer` linearly interpolates each canvas-Y:

```
canvasY = oldY + (newY - oldY) * t
```

NaN points in either old or new values skip interpolation (the gap is shown immediately).

**Easing functions:**

| `EasingType` | Formula | Effect |
|--------------|---------|--------|
| `LINEAR` (0) | `t` | Constant speed |
| `EASE_OUT` (1) | `1 - (1-t)²` | Fast start, slow finish (default) |
| `EASE_IN_OUT` (2) | `t<0.5 ? 2t² : 1-2(1-t)²` | Slow start and finish |

`mLastRenderedCanvasY` is updated at animation completion (`t == 1.0`) to serve as `mAnimOldCanvasY` for the next transition.

---

[← Back to Components](Components.md) | [ChartView User Guide](ChartView.md)
