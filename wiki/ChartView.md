# DALi UI Components - ChartView

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/ChartView-(kr).md)

`ChartView` is a multi-type chart component for `dali-ui-components`. It renders line, bar, pie, area, scatter, and gauge charts using ThorVG-backed `CanvasView` layers, with full support for zoom/pan gestures, interactive tooltips, animated data transitions, and real-time data feeds.

![ChartView gallery overview](assets/chartview/gallery-overview.gif)

---

## Table of Contents

1. [Basic Setup](#1-basic-setup)
2. [Chart Types](#2-chart-types)
3. [LineSeries](#3-lineseries)
4. [BarSeries](#4-barseries)
5. [PieSeries](#5-pieseries)
6. [ScatterSeries](#6-scatterseries)
7. [ChartAxis](#7-chartaxis)
8. [ChartSection](#8-chartsection)
9. [Properties](#9-properties)
10. [Real-time Updates & Performance](#10-real-time-updates--performance)
11. [Interaction & Zoom/Pan](#11-interaction--zoompan)
12. [Signals](#12-signals)
13. [Animation](#13-animation)
14. [Gauge](#14-gauge)
15. [Layout Integration](#15-layout-integration)
16. [CMake Integration](#16-cmake-integration)
17. [Default Values](#17-default-values)
18. [Important Notes](#18-important-notes)

---

## 1. Basic Setup

> ⚠️ **Note**: `ChartView` is part of `dali-ui-components`. You must call `UiConfig::New().Apply()` before `MainLoop()`. See [Configuration](Configuration.md) for details.

### main.cpp

```cpp
#include <dali-ui-components/dali-ui-components.h>
using namespace Dali;
using namespace Dali::Ui;

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Apply UiConfig before MainLoop().
  UiConfig::New().Apply();

  MyController controller(application);
  application.MainLoop();
  return 0;
}
```

### Creating a ChartView

```cpp
#include <dali-ui-components/dali-ui-components.h>
using namespace Dali;
using namespace Dali::Ui;

// Create a LINE chart with explicit size
ChartView chart = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

// Configure chart
chart.SetTitle("Monthly Revenue");
ChartAxis xAxis = ChartAxis::New();
xAxis.SetTitle("Month");
xAxis.SetLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
chart.SetXAxis(xAxis);

ChartAxis yAxis = ChartAxis::New();
yAxis.SetTitle("Amount (USD)");
chart.SetYAxis(yAxis);

LineSeries series = LineSeries::New();
series.SetName("Revenue");
series.SetValues({120.0f, 190.0f, 150.0f, 250.0f, 210.0f, 280.0f});
chart.AddSeries(series);

// Enable grid and legend via properties
chart.SetProperty(ChartView::Property::SHOW_GRID, true);
chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
```

### Placing in a Layout

```cpp
// Method 1: AbsoluteLayout with AbsoluteLayoutParams
chart.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX(20.0f).SetY(60.0f)
    .SetWidth(480.0f).SetHeight(360.0f));

AbsoluteLayout root = AbsoluteLayout::New();
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);
root.Add(chart);
window.Add(root);
```

```cpp
// Method 2: SetRequestedPositionX/Y without a separate layout params object
chart.SetRequestedPositionX(20.0f);
chart.SetRequestedPositionY(60.0f);
window.Add(chart);
```

> 💡 **Tip**: `ChartView` is a standard `View` and participates in the dali-ui layout system. Place it inside an `AbsoluteLayout` when you need pixel-precise positioning.

<br/>

---

## 2. Chart Types

Pass the type to `ChartView::New()`. The type cannot be changed after creation.

![LINE and AREA](assets/chartview/chart-types-line-area.png)

![BAR and PIE](assets/chartview/chart-types-bar-pie.png)

![SCATTER and GAUGE](assets/chartview/chart-types-scatter-gauge.png)

| Type | Enum | Description |
|---|---|---|
| Line | `ChartView::Type::LINE` | Connected line with optional markers |
| Bar | `ChartView::Type::BAR` | Vertical grouped or stacked bars |
| Pie | `ChartView::Type::PIE` | Full-circle pie chart |
| Area | `ChartView::Type::AREA` | Filled area beneath a line (`LineSeries` with `SetFillEnabled(true)`) |
| Scatter | `ChartView::Type::SCATTER` | XY scatter plot with marker shapes |
| Gauge | `ChartView::Type::GAUGE` | Arc-based gauge (no series — uses dedicated Gauge API) |

```cpp
ChartView lineChart    = ChartView::New(ChartView::Type::LINE,    Vector2(480, 360));
ChartView barChart     = ChartView::New(ChartView::Type::BAR,     Vector2(480, 360));
ChartView pieChart     = ChartView::New(ChartView::Type::PIE,     Vector2(400, 400));
ChartView areaChart    = ChartView::New(ChartView::Type::AREA,    Vector2(480, 360));
ChartView scatterChart = ChartView::New(ChartView::Type::SCATTER, Vector2(480, 360));
ChartView gaugeChart   = ChartView::New(ChartView::Type::GAUGE,   Vector2(360, 360));
```

> ⚠️ **Note**: `AREA` is a convenience alias. It behaves identically to `LINE` — you still create `LineSeries` and call `series.SetFillEnabled(true)` to get the filled area appearance. The enum value causes no special internal behavior beyond documentation intent.

<br/>

---

## 3. LineSeries

`LineSeries` renders a connected polyline with optional markers, fill, and data labels.

![Line and Area charts](assets/chartview/line-chart.png)

### Creating and Adding Data

```cpp
chart.AddSeries(LineSeries::New()
    .SetName("Revenue")
    .SetColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f))
    .SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f}));
```

### Line Style

```cpp
series.SetLineWidth(3.0f);          // Line stroke width in pixels
series.SetSmoothness(0.8f);         // 0.0 = straight segments, 1.0 = fully smooth Bezier
```

### Markers

![Marker shapes example](assets/chartview/line-geometries.png)

```cpp
series.SetMarkersVisible(true);
series.SetMarkerShape(LineSeries::MarkerShape::CIRCLE);   // CIRCLE, SQUARE, TRIANGLE, DIAMOND
series.SetMarkerRadius(5.0f);
series.SetMarkerColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f));
series.SetMarkerBorderColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
series.SetMarkerBorderWidth(1.5f);
```

| `MarkerShape` | Description |
|---|---|
| `CIRCLE` | Filled circle (default) |
| `SQUARE` | Filled square |
| `TRIANGLE` | Upward-pointing triangle |
| `DIAMOND` | Diamond (rotated square) |

### Area (Fill)

![Area chart](assets/chartview/area-chart.png)

```cpp
// Area chart: enable fill below the line
series.SetFillEnabled(true);
series.SetFillColor(Vector4(0.2f, 0.4f, 0.8f, 0.3f));  // Semi-transparent
```

### Data Labels

```cpp
series.SetDataLabelsVisible(true);
series.SetDataLabelFormat("%.1f");      // printf-style format string
series.SetDataLabelColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
series.SetDataLabelSize(9.0f);          // Font size in points

// Or use a custom formatter function
series.SetDataLabelFormatter([](float value, int pointIndex) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "$%.0f", value);
    return Dali::String(buf);
});
```

### NaN Gap

![NaN gap example](assets/chartview/nan-gap.png)

Insert a `NaN` value to break the line (create a visible gap):

```cpp
#include <limits>
float NaN = std::numeric_limits<float>::quiet_NaN();

series.SetValues({100.0f, 150.0f, 120.0f,
                  NaN,              // gap here — line breaks between index 2 and 4
                  200.0f, 180.0f, 220.0f});
```

### Z-Index (Draw Order)

`SetZIndex` is available on all series types (`LineSeries`, `BarSeries`, `ScatterSeries`). Higher value = drawn on top.

![Z-Index example](assets/chartview/zindex.png)

```cpp
series.SetZIndex(10);   // Higher value = drawn on top of lower-z series
```

### Full Example

```cpp
LineSeries sales = LineSeries::New();
sales.SetName("Sales");
sales.SetColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f));
sales.SetValues({50.0f, 80.0f, 60.0f, 100.0f, 70.0f});
sales.SetLineWidth(2.5f);
sales.SetSmoothness(0.5f);
sales.SetMarkersVisible(true);
sales.SetMarkerShape(LineSeries::MarkerShape::DIAMOND);
sales.SetMarkerRadius(5.0f);
sales.SetFillEnabled(true);
sales.SetFillColor(Vector4(0.2f, 0.6f, 1.0f, 0.2f));
sales.SetDataLabelsVisible(true);
sales.SetDataLabelFormat("%.0f");
chart.AddSeries(sales);
```

<br/>

---

## 4. BarSeries

`BarSeries` renders vertical bars. Multiple `BarSeries` added to the same chart are grouped by default; set `SetStacked(true)` to stack them.

![Bar charts](assets/chartview/bar-chart.png)

### Basic Bar Chart

```cpp
BarSeries revenue = BarSeries::New();
revenue.SetColor(Vector4(0.39f, 0.58f, 0.93f, 1.0f));
revenue.SetDataLabelsVisible(true);
revenue.SetName("Revenue");
revenue.SetValues({320.0f, 410.0f, 380.0f, 450.0f});
chart.AddSeries(revenue);

BarSeries cost = BarSeries::New();
cost.SetColor(Vector4(1.0f, 0.39f, 0.28f, 1.0f));
cost.SetDataLabelsVisible(true);
cost.SetName("Cost");
cost.SetValues({210.0f, 250.0f, 230.0f, 280.0f});
chart.AddSeries(cost);
```

### Stacked Bars

```cpp
BarSeries base = BarSeries::New();
base.SetColor(Vector4(0.39f, 0.58f, 0.93f, 1.0f));
base.SetStacked(true);
base.SetName("Base");
base.SetValues({200.0f, 240.0f, 220.0f, 270.0f});
chart.AddSeries(base);

BarSeries addon = BarSeries::New();
addon.SetColor(Vector4(0.24f, 0.70f, 0.44f, 1.0f));
addon.SetStacked(true);
addon.SetName("Add-on");
addon.SetValues({80.0f, 110.0f, 90.0f, 130.0f});
chart.AddSeries(addon);
```

> 💡 **Tip**: Stacked and grouped bars can coexist in the same chart. Series with `SetStacked(true)` are accumulated; series with `SetStacked(false)` (the default) occupy their own grouped slot.

### Bar Group Width

```cpp
b1.SetBarGroupWidth(0.6f);   // 0.0–1.0 ratio of the available slot width; default 0.7
```

### Data Labels

```cpp
b1.SetDataLabelsVisible(true);
b1.SetDataLabelFormat("%.0f");
b1.SetDataLabelColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
b1.SetDataLabelSize(9.0f);
```

### Mixing BarSeries and LineSeries

`BarSeries` and `LineSeries` can be added to the same `ChartView::Type::LINE` chart. The line is drawn over the bars.

![Mixed bar and line chart](assets/chartview/mixed-bar-line.png)

```cpp
ChartView chart = ChartView::New(ChartView::Type::LINE, Vector2(480, 360));

BarSeries bars = BarSeries::New();
bars.SetValues({320.0f, 410.0f, 380.0f, 450.0f});
chart.AddSeries(bars);

LineSeries trend = LineSeries::New();
trend.SetName("Target");
trend.SetValues({380.0f, 400.0f, 400.0f, 420.0f});
trend.SetMarkersVisible(true);
chart.AddSeries(trend);
```

<br/>

---

## 5. PieSeries

`PieSeries` draws a pie chart. Set `SetInnerRadiusRatio()` greater than zero to make it a donut chart.

### Basic Pie Chart

![Basic pie chart with data labels](assets/chartview/pie-chart.png)

```cpp
ChartView chart = ChartView::New(ChartView::Type::PIE, Vector2(400.0f, 400.0f));
chart.SetTitle("Browser Market Share");
chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);

PieSeries pie = PieSeries::New();
pie.AddSlice("Chrome",  63.0f, Vector4(0.39f, 0.58f, 0.93f, 1.0f));
pie.AddSlice("Safari",  19.0f, Vector4(1.0f,  0.39f, 0.28f, 1.0f));
pie.AddSlice("Edge",    11.0f, Vector4(0.24f, 0.70f, 0.44f, 1.0f));
pie.AddSlice("Firefox",  4.0f, Vector4(0.58f, 0.44f, 0.86f, 1.0f));
pie.AddSlice("Other",    3.0f, Vector4(0.5f,  0.5f,  0.5f,  1.0f));
pie.SetDataLabelsVisible(true);
chart.AddSeries(pie);
```

### Donut Chart

![Donut chart with center label](assets/chartview/pie-donut.png)

```cpp
PieSeries pie = PieSeries::New();
pie.AddSlice("Asia",    42.0f, Vector4(0.39f, 0.58f, 0.93f, 1.0f));
pie.AddSlice("America", 28.0f, Vector4(1.0f,  0.39f, 0.28f, 1.0f));
pie.AddSlice("Europe",  20.0f, Vector4(0.24f, 0.70f, 0.44f, 1.0f));
pie.AddSlice("Others",  10.0f, Vector4(0.5f,  0.5f,  0.5f,  1.0f));

pie.SetInnerRadiusRatio(0.55f);    // 0.0 = full pie, >0.0 = donut hole
pie.SetCenterLabel("$4.2B");       // Text displayed in the donut hole
pie.SetSliceGap(2.0f);             // Gap between slices in degrees
pie.SetDataLabelsVisible(true);
pie.SetDataLabelMinAngle(15.0f);   // Hide label if slice angle is smaller than this
chart.AddSeries(pie);
```

### Slice Management

```cpp
pie.ClearSlices();
int count = pie.GetSliceCount();
```

<br/>

---

## 6. ScatterSeries

`ScatterSeries` plots XY coordinate pairs as individual markers with no connecting lines.

![Scatter chart](assets/chartview/scatter.png)

### XY Data

```cpp
ChartView chart = ChartView::New(ChartView::Type::SCATTER, Vector2(480.0f, 360.0f));
chart.SetProperty(ChartView::Property::SHOW_GRID, true);
chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);

// SetValues takes a vector of (x, y) pairs
ScatterSeries groupA = ScatterSeries::New();
groupA.SetName("Group A");
groupA.SetColor(Vector4(0.39f, 0.58f, 0.93f, 1.0f));
groupA.SetMarkerShape(ScatterSeries::MarkerShape::CIRCLE);
groupA.SetMarkerRadius(7.0f);
groupA.SetValues({{0.5f, 30.0f}, {1.8f, 55.0f}, {3.2f, 45.0f}, {4.1f, 80.0f}});
chart.AddSeries(groupA);

ScatterSeries groupB = ScatterSeries::New();
groupB.SetName("Group B");
groupB.SetColor(Vector4(1.0f, 0.39f, 0.28f, 1.0f));
groupB.SetMarkerShape(ScatterSeries::MarkerShape::DIAMOND);
groupB.SetMarkerRadius(10.0f);
groupB.SetValues({{1.0f, 70.0f}, {2.5f, 40.0f}, {3.8f, 90.0f}, {5.0f, 25.0f}});
chart.AddSeries(groupB);
```

| `MarkerShape` | Description |
|---|---|
| `CIRCLE` | Filled circle |
| `SQUARE` | Filled square |
| `TRIANGLE` | Upward-pointing triangle |
| `DIAMOND` | Diamond |

> 💡 **Tip**: Mix `ScatterSeries` with `LineSeries` in the same chart to overlay a trend line on scatter data.

<br/>

---

## 7. ChartAxis

![Axis labels example](assets/chartview/axis-labels.png)

`ChartAxis` controls labels, title, limits, grid styling, and tick step for the X or Y axis.

### Labels and Title

```cpp
ChartAxis xAxis = ChartAxis::New();
xAxis.SetTitle("Quarter");
xAxis.SetLabels({"Q1", "Q2", "Q3", "Q4"});
chart.SetXAxis(xAxis);

ChartAxis yAxis = ChartAxis::New();
yAxis.SetTitle("Revenue (USD)");
chart.SetYAxis(yAxis);
```

### Manual Range Limits

```cpp
yAxis.SetMinLimit(0.0f);
yAxis.SetMaxLimit(500.0f);
```

When limits are not set, the axis auto-ranges to fit the data (plus padding).

### Data Padding

Adds extra space beyond the data range as a fraction of the total range:

```cpp
yAxis.SetDataPadding(0.1f);   // 10% headroom above the maximum value
xAxis.SetDataPadding(0.0f);   // No padding on the X axis
```

### Minimum Tick Step

Forces tick marks to land on multiples of the given value:

```cpp
yAxis.SetMinStep(50.0f);   // Y ticks at 0, 50, 100, 150, ...
```

### Label Rotation

Rotate axis labels to prevent overlap with dense data:

```cpp
xAxis.SetLabelsRotation(45.0f);   // 45-degree rotation
```

### Grid and Axis Line Style

```cpp
yAxis.SetGridColor(Vector4(0.0f, 0.5f, 1.0f, 0.6f));
yAxis.SetGridDash(4.0f, 4.0f);             // Dashed grid: 4px dash, 4px gap
yAxis.SetAxisLineColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f));
yAxis.SetAxisLineWidth(2.0f);
```

### Full Example

```cpp
ChartAxis xAxis = ChartAxis::New();
xAxis.SetTitle("Month");
xAxis.SetLabels({"January", "February", "March", "April", "May", "June"});
xAxis.SetLabelsRotation(45.0f);
xAxis.SetDataPadding(0.0f);
chart.SetXAxis(xAxis);

ChartAxis yAxis = ChartAxis::New();
yAxis.SetTitle("Units Sold");
yAxis.SetMinStep(50.0f);
yAxis.SetDataPadding(0.1f);
chart.SetYAxis(yAxis);
```

<br/>

---

## 8. ChartSection

`ChartSection` overlays a highlighted region on the chart plot area. Coordinates are in data space (same units as axis values).

![Chart sections example](assets/chartview/sections.png)

### Section Types

The type is determined by which boundaries are set:

| Pattern | Result |
|---|---|
| `SetXMin` + `SetXMax` only | Vertical band spanning full Y range |
| `SetYMin` + `SetYMax` only | Horizontal band spanning full X range |
| `SetXMin == SetXMax` | Vertical threshold line |
| `SetYMin == SetYMax` | Horizontal threshold line |
| All four boundaries set | Rectangle region |

A boundary value of `NaN` means "extend to the plot area edge" on that side.

### Vertical Band (X range highlight)

```cpp
ChartSection xBand = ChartSection::New();
xBand.SetXMin(2.5f);
xBand.SetXMax(5.5f);
xBand.SetFillColor(Vector4(0.2f, 0.7f, 0.3f, 0.12f));
xBand.SetStrokeColor(Vector4(0.2f, 0.7f, 0.3f, 0.5f));
xBand.SetStrokeWidth(1.5f);
chart.AddSection(xBand);
```

### Horizontal Band (Y range highlight)

```cpp
ChartSection yBand = ChartSection::New();
yBand.SetYMin(200.0f);
yBand.SetYMax(280.0f);
yBand.SetFillColor(Vector4(1.0f, 0.85f, 0.2f, 0.10f));
chart.AddSection(yBand);
```

### Horizontal Threshold Line

```cpp
ChartSection hLine = ChartSection::New();
hLine.SetYMin(250.0f);
hLine.SetYMax(250.0f);   // same value = line
hLine.SetStrokeColor(Vector4(1.0f, 0.3f, 0.3f, 0.9f));
hLine.SetStrokeWidth(2.0f);
chart.AddSection(hLine);
```

### Vertical Reference Line

```cpp
ChartSection vLine = ChartSection::New();
vLine.SetXMin(6.0f);
vLine.SetXMax(6.0f);   // same value = line
vLine.SetStrokeColor(Vector4(0.4f, 0.4f, 1.0f, 0.8f));
vLine.SetStrokeWidth(2.0f);
chart.AddSection(vLine);
```

### Rectangle Region

```cpp
ChartSection rect = ChartSection::New();
rect.SetXMin(8.5f);
rect.SetXMax(11.5f);
rect.SetYMin(270.0f);
rect.SetYMax(320.0f);
rect.SetFillColor(Vector4(0.6f, 0.2f, 1.0f, 0.12f));
rect.SetStrokeColor(Vector4(0.6f, 0.2f, 1.0f, 0.6f));
rect.SetStrokeWidth(1.5f);
chart.AddSection(rect);
```

### Section Management

```cpp
chart.RemoveSection(xBand);
chart.ClearSections();
```

<br/>

---

## 9. Properties

Use `chart.SetProperty(ChartView::Property::PROPERTY_NAME, value)` to configure chart-wide appearance and behavior.

| Property | Type | Default | Description |
|---|---|---|---|
| `SHOW_GRID` | `bool` | `true` | Show grid lines |
| `SHOW_LEGEND` | `bool` | `true` | Show series legend |
| `SHOW_TOOLTIP` | `bool` | `true` | Show data-point tooltip on hover/touch |
| `BACKGROUND_COLOR` | `Vector4` | white `(1,1,1,1)` | Chart background color |
| `GRID_COLOR` | `Vector4` | `(0.9,0.9,0.9,1)` | Grid line color |
| `ANIMATION_DURATION` | `float` (ms) | `0.0` | Data-change animation duration (0 = disabled) |
| `Y_AXIS_AUTO_RANGE` | `bool` | `true` | Auto-fit Y axis to data range |
| `LEGEND_POSITION` | `int` | `1` (TOP) | Legend position: 0=NONE, 1=TOP, 2=BOTTOM, 3=LEFT, 4=RIGHT |
| `AXIS_LABEL_SIZE` | `float` | `11.0` | Axis tick label font size in points |
| `TITLE_SIZE` | `float` | `16.0` | Chart title font size in points |
| `LINE_WIDTH` | `float` | `2.5` | Default line width for all series (can be overridden per series) |
| `SHOW_MARKERS` | `bool` | `true` | Global toggle for marker visibility |
| `MARKER_RADIUS` | `float` | `4.0` | Default marker radius (can be overridden per series) |
| `HOVER_ENABLED` | `bool` | `true` | Enable hover (pointer/stylus) interaction |
| `TOUCH_ENABLED` | `bool` | `true` | Enable touch (finger) interaction |

```cpp
chart.SetProperty(ChartView::Property::SHOW_GRID,        true);
chart.SetProperty(ChartView::Property::SHOW_LEGEND,      true);
chart.SetProperty(ChartView::Property::SHOW_TOOLTIP,     true);
chart.SetProperty(ChartView::Property::BACKGROUND_COLOR, Vector4(0.98f, 0.98f, 0.98f, 1.0f));
chart.SetProperty(ChartView::Property::GRID_COLOR,       Vector4(0.85f, 0.85f, 0.85f, 1.0f));
chart.SetProperty(ChartView::Property::ANIMATION_DURATION, 300.0f);
chart.SetProperty(ChartView::Property::AXIS_LABEL_SIZE,  10.0f);
chart.SetProperty(ChartView::Property::TITLE_SIZE,       14.0f);
chart.SetProperty(ChartView::Property::TOUCH_ENABLED,    true);
chart.SetProperty(ChartView::Property::HOVER_ENABLED,    false);
```

### Title Position

```cpp
chart.SetTitle("Sales Dashboard");
chart.SetTitlePosition(ChartView::TitlePosition::TOP_LEFT);   // TOP_CENTER (default), TOP_LEFT, TOP_RIGHT, BOTTOM_CENTER
chart.SetTitleColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
```

### Series Management

```cpp
chart.AddSeries(series);
chart.RemoveSeries("Revenue");   // Remove by name
chart.RemoveAllSeries();
```

<br/>

---

## 10. Real-time Updates & Performance

![Auto Updates and Real Time charts](assets/chartview/real-time.png)

### Sliding Window with AppendValue

For real-time data feeds (sensors, streaming), use `AppendValue()` together with `SetMaxDataPoints()` to maintain a fixed-length sliding window:

```cpp
LineSeries series = LineSeries::New();
series.SetName("CPU Usage");
series.SetMaxDataPoints(60);   // Keep at most 60 points; oldest discarded on overflow

chart.AddSeries(series);

// In a timer callback or event handler:
series.AppendValue(newY);          // Appends one Y value; auto-increments X index
```

### Batch Append

```cpp
// Seed with initial values
series.AppendValues({30.0f, 55.0f, 40.0f, 80.0f, 60.0f});
```

### Update Throttle

`SetUpdateThrottle()` limits how often a data change triggers a redraw. The default is `16 ms` (~60 fps). For high-frequency data sources this prevents excessive rasterization:

```cpp
// Redraw at most once per 100 ms (10 fps) — useful for very fast sensors
chart.SetUpdateThrottle(100.0f);

// Disable throttling: redraw immediately on every AppendValue
chart.SetUpdateThrottle(0.0f);
```

### Full Real-time Example

```cpp
// Keep a reference to call AppendValue later
LineSeries liveSeries = LineSeries::New();
liveSeries.SetName("Temperature");
liveSeries.SetColor(Vector4(1.0f, 0.4f, 0.2f, 1.0f));
liveSeries.SetMaxDataPoints(120);
liveSeries.SetMarkersVisible(false);

chart.AddSeries(liveSeries);
chart.SetUpdateThrottle(50.0f);   // At most 20 redraws per second

// Every 50 ms in your application loop:
liveSeries.AppendValue(readSensorTemperature());
```

<br/>

---

## 11. Interaction & Zoom/Pan

### Zoom Mode

![Zoom & Pan interaction](assets/chartview/zoom-pan.png)

`SetZoomMode()` accepts a bitmask of `ChartView::ZoomMode` flags:

| Flag | Value | Description |
|---|---|---|
| `NONE` | `0` | No zoom or pan |
| `PAN_X` | `1 << 0` | Pan along the X axis |
| `PAN_Y` | `1 << 1` | Pan along the Y axis |
| `ZOOM_X` | `1 << 2` | Pinch-zoom on the X axis |
| `ZOOM_Y` | `1 << 3` | Pinch-zoom on the Y axis |

```cpp
// Enable X pan and X zoom (common stock-chart pattern)
chart.SetZoomMode(
    static_cast<int>(ChartView::ZoomMode::PAN_X) |
    static_cast<int>(ChartView::ZoomMode::ZOOM_X));

// Enable full 2D pan and zoom
chart.SetZoomMode(
    static_cast<int>(ChartView::ZoomMode::PAN_X)  |
    static_cast<int>(ChartView::ZoomMode::PAN_Y)  |
    static_cast<int>(ChartView::ZoomMode::ZOOM_X) |
    static_cast<int>(ChartView::ZoomMode::ZOOM_Y));

// Disable all interaction
chart.SetZoomMode(static_cast<int>(ChartView::ZoomMode::NONE));
```

### Zoom Helpers

```cpp
chart.SetZoomClampEnabled(true);    // Prevent panning beyond the data range
chart.SetAutoFitYOnPan(true);       // Auto-scale Y axis to the visible X window while panning
chart.ResetZoom();                  // Return to the full data view
```

### Tooltip Finding Strategy

Controls which data points are highlighted when the user hovers or taps:

| Strategy | Description |
|---|---|
| `NEAREST` | Nearest single point across all series (default) |
| `SAME_X` | All points sharing the same X index across all series |
| `SAME_X_NEAREST_Y` | All points at the same X with the nearest Y |

```cpp
chart.SetFindingStrategy(ChartView::FindingStrategy::SAME_X);
```

### Custom Tooltip Formatter

```cpp
chart.SetTooltipFormatter([](const Dali::String& seriesName,
                              const Dali::String& xLabel,
                              float               dataY) {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "[%s] %s: $%.0f",
                  seriesName.CStr(), xLabel.CStr(), dataY);
    return Dali::String(buf);
});
```

### Legend Toggle

![Dynamic visibility via legend toggle](assets/chartview/dynamic-visibility.png)

When enabled, tapping a legend entry hides or shows the corresponding series:

```cpp
chart.SetLegendToggleEnabled(true);    // Default: true
chart.SetLegendToggleEnabled(false);   // Signal-only mode: LegendItemTappedSignal fires but visibility does not change
```

### Hit Threshold

```cpp
chart.SetHitThreshold(30.0f);   // Distance in pixels within which a tap is considered a hit (default 30)
```

<br/>

---

## 12. Signals

![Custom tooltip formatter](assets/chartview/formatter.png)

### DataPointSelectedSignal

Emitted when the user taps or hovers over a data point.

```cpp
class MyHandler : public Dali::ConnectionTracker
{
public:
    void OnPointSelected(const ChartPointEventArgs& e)
    {
        // e.seriesIndex  — zero-based series index
        // e.pointIndex   — zero-based point index within the series
        // e.dataX        — X value of the selected point
        // e.dataY        — Y value of the selected point
        // e.seriesName   — name set via SetName()
        // e.xLabel       — label string from ChartAxis::SetLabels()
        DALI_LOG_RELEASE_INFO("Hit series %d, point %d: [%s] = %.1f\n",
                              e.seriesIndex, e.pointIndex,
                              e.xLabel.CStr(), e.dataY);
    }
};

MyHandler handler;
chart.DataPointSelectedSignal().Connect(&handler, &MyHandler::OnPointSelected);
```

### LegendItemTappedSignal

Emitted when the user taps a legend entry.

```cpp
void OnLegendTapped(int seriesIndex, bool isNowVisible)
{
    DALI_LOG_RELEASE_INFO("Legend series %d toggled -> %s\n",
                          seriesIndex, isNowVisible ? "visible" : "hidden");
}

chart.LegendItemTappedSignal().Connect(&handler, &MyHandler::OnLegendTapped);
```

### MultiPointSelectedSignal

Emitted for each matching point when `FindingStrategy::SAME_X` or `SAME_X_NEAREST_Y` is active. The signal fires once per matching point with the same `ChartPointEventArgs` layout as `DataPointSelectedSignal`.

```cpp
chart.SetFindingStrategy(ChartView::FindingStrategy::SAME_X);
chart.MultiPointSelectedSignal().Connect(&handler, &MyHandler::OnMultiPoint);
```

### ZoomedSignal

Emitted after a zoom or pan gesture completes, reporting the new viewport in data coordinates.

```cpp
void OnZoomed(const ChartViewportArgs& vp)
{
    // vp.xMin, vp.xMax — visible X range in data coordinates
    // vp.yMin, vp.yMax — visible Y range in data coordinates
    DALI_LOG_RELEASE_INFO("Viewport X[%.1f ~ %.1f] Y[%.1f ~ %.1f]\n",
                          vp.xMin, vp.xMax, vp.yMin, vp.yMax);
}

chart.ZoomedSignal().Connect(&handler, &MyHandler::OnZoomed);
```

### Signal Summary

| Signal | Callback Signature | Emitted When |
|---|---|---|
| `DataPointSelectedSignal` | `void(const ChartPointEventArgs&)` | Single point tapped or hovered |
| `LegendItemTappedSignal` | `void(int index, bool isVisible)` | Legend entry tapped |
| `MultiPointSelectedSignal` | `void(const ChartPointEventArgs&)` | Each matching point under SAME_X strategy |
| `ZoomedSignal` | `void(const ChartViewportArgs&)` | Zoom or pan gesture ends |

<br/>

---

## 13. Animation

![Animated data transition](assets/chartview/animation.png)

`ChartView` can animate transitions when data is updated via `SetValues()`.

### Enabling Animation

```cpp
chart.SetAnimationDuration(300.0f);                              // 300 ms
chart.SetAnimationEasing(ChartView::EasingType::EASE_OUT);       // Easing function
```

| `EasingType` | Description |
|---|---|
| `LINEAR` | Constant speed throughout |
| `EASE_OUT` | Fast start, decelerates at the end (natural-feeling) |
| `EASE_IN_OUT` | Slow start, fast middle, slow end (smooth) |

### Animation Example

```cpp
chart.SetAnimationDuration(300.0f);
chart.SetAnimationEasing(ChartView::EasingType::EASE_OUT);

// The next SetValues() call will animate the transition
series.SetValues({60.0f, 40.0f, 280.0f, 100.0f, 310.0f, 180.0f});
```

> ⚠️ **Note**: `ANIMATION_DURATION` property and `SetAnimationDuration()` control the same setting. Setting `ANIMATION_DURATION` to `0.0` (the default) disables animation.

<br/>

---

## 14. Gauge

The `GAUGE` chart type renders a circular arc gauge. It does **not** use `AddSeries()` — all configuration is done through dedicated gauge methods on `ChartView`.

![Gauge chart](assets/chartview/gauge.png)

### Basic Gauge

```cpp
ChartView gauge = ChartView::New(ChartView::Type::GAUGE, Vector2(360.0f, 360.0f));
gauge.SetTitle("CPU Usage");

gauge.SetGaugeMinValue(0.0f);
gauge.SetGaugeMaxValue(100.0f);
gauge.SetGaugeValue(72.0f);        // Current needle position

// Color zones
gauge.AddGaugeRange( 0.0f,  60.0f, Vector4(0.24f, 0.70f, 0.44f, 1.0f));  // green
gauge.AddGaugeRange(60.0f,  80.0f, Vector4(1.0f,  0.7f,  0.0f,  1.0f));  // yellow
gauge.AddGaugeRange(80.0f, 100.0f, Vector4(1.0f,  0.39f, 0.28f, 1.0f));  // red
```

### Arc Shape

```cpp
gauge.SetGaugeArcSpan(270.0f);         // Total arc angle in degrees (default 270)
gauge.SetGaugeStartAngle(135.0f);      // Angle where arc begins (default 135, 6 o'clock = 270)
gauge.SetGaugeArcWidth(0.18f);         // Arc width as fraction of radius (default 0.18)
```

### Colors

```cpp
gauge.SetGaugeTrackColor(Vector4(0.85f, 0.85f, 0.85f, 1.0f));     // Background arc color
gauge.SetGaugeProgressColor(Vector4(0.27f, 0.51f, 0.71f, 1.0f));  // Progress arc color (overridden by ranges)
```

> 💡 **Tip**: `SetGaugeProgressColor()` sets the default progress color. When `AddGaugeRange()` entries are present, each range segment uses its own color and `SetGaugeProgressColor()` is ignored.

### Center Label

```cpp
gauge.SetGaugeCenterLabel("72%");
```

### Updating the Value

```cpp
// Update in response to events (e.g., a timer or sensor callback)
gauge.SetGaugeValue(newValue);
```

### Range Management

```cpp
gauge.ClearGaugeRanges();
gauge.AddGaugeRange(0.0f, 50.0f, Vector4(0.2f, 0.8f, 0.4f, 1.0f));
gauge.AddGaugeRange(50.0f, 100.0f, Vector4(1.0f, 0.3f, 0.3f, 1.0f));
```

### Gauge API Summary

| Method | Description |
|---|---|
| `SetGaugeValue(float)` | Set the current gauge position |
| `SetGaugeMinValue(float)` | Set the minimum range value |
| `SetGaugeMaxValue(float)` | Set the maximum range value |
| `SetGaugeArcSpan(float)` | Total arc sweep angle in degrees |
| `SetGaugeStartAngle(float)` | Starting angle of the arc |
| `SetGaugeArcWidth(float)` | Arc thickness as a fraction of the radius |
| `SetGaugeTrackColor(Vector4)` | Background track color |
| `SetGaugeProgressColor(Vector4)` | Default progress color |
| `SetGaugeCenterLabel(String)` | Text to display in the center |
| `AddGaugeRange(float, float, Vector4)` | Add a color-coded value range |
| `ClearGaugeRanges()` | Remove all color ranges |

<br/>

---

## 15. Layout Integration

`ChartView` inherits from `View` and integrates fully with the dali-ui layout system.

### Placing with AbsoluteLayout

```cpp
ChartView chart = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

// Position and size via AbsoluteLayoutParams
chart.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX(50.0f)
    .SetY(100.0f)
    .SetWidth(480.0f)
    .SetHeight(360.0f));

AbsoluteLayout root = AbsoluteLayout::New();
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);
root.Add(chart);
window.Add(root);
```

### Position Only (Size from New)

```cpp
// Size is fixed by ChartView::New(); only position needs setting
chart.SetRequestedPositionX(50.0f);
chart.SetRequestedPositionY(100.0f);
window.Add(chart);
```

### Centering in the Window

```cpp
float winW = window.GetSize().width;
float winH = window.GetSize().height;
float chartW = 480.0f, chartH = 360.0f;

chart.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX((winW - chartW) * 0.5f)
    .SetY((winH - chartH) * 0.5f)
    .SetWidth(chartW)
    .SetHeight(chartH));
```

### Dynamic Resize

Call `SetLayoutParams()` with a new size and re-add the chart to the layout to trigger `OnArrange`:

```cpp
chart.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX(x).SetY(y).SetWidth(newW).SetHeight(newH));
rootLayout.Remove(chart);
rootLayout.Add(chart);
```

<br/>

---

## 16. CMake Integration

Add the following to your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.8.2)
project(my-chart-app)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DALICORE           REQUIRED dali2-core)
pkg_check_modules(DALIADAPTOR        REQUIRED dali2-adaptor)
pkg_check_modules(DALI_UI_FOUNDATION REQUIRED dali2-ui-foundation)
pkg_check_modules(DALI_UI_COMP       REQUIRED dali2-ui-components)

add_executable(my-chart-app main.cpp)

target_include_directories(my-chart-app PRIVATE
    ${DALICORE_INCLUDE_DIRS}
    ${DALIADAPTOR_INCLUDE_DIRS}
    ${DALI_UI_FOUNDATION_INCLUDE_DIRS}
    ${DALI_UI_COMP_INCLUDE_DIRS}
)

target_link_libraries(my-chart-app
    ${DALICORE_LIBRARIES}
    ${DALIADAPTOR_LIBRARIES}
    ${DALI_UI_FOUNDATION_LIBRARIES}
    ${DALI_UI_COMP_LIBRARIES}
)

target_compile_options(my-chart-app PRIVATE
    ${DALI_UI_COMP_CFLAGS_OTHER}
)
```

> 💡 **Tip**: The sample `samples/chart-view/CMakeLists.txt` shows a complete working build configuration including debug/release flags and `HIDE_DALI_INTERNALS`.

<br/>

---

## 17. Default Values

### ChartView (StyleConfig defaults)

| Setting | Default |
|---|---|
| `SHOW_GRID` | `true` |
| `SHOW_LEGEND` | `true` |
| `SHOW_TOOLTIP` | `true` |
| `SHOW_MARKERS` | `true` |
| `BACKGROUND_COLOR` | white `(1.0, 1.0, 1.0, 1.0)` |
| `GRID_COLOR` | `(0.9, 0.9, 0.9, 1.0)` |
| `AXIS_LABEL_SIZE` | `11.0` pt |
| `TITLE_SIZE` | `16.0` pt |
| `LINE_WIDTH` | `2.5` px |
| `MARKER_RADIUS` | `4.0` px |
| `TOUCH_ENABLED` | `true` |
| `HOVER_ENABLED` | `true` |
| `ANIMATION_DURATION` | `0.0` ms (disabled) |
| `ANIMATION_EASING` | `EASE_OUT` |
| `LEGEND_POSITION` | `TOP` |
| `TITLE_POSITION` | `TOP_CENTER` |
| Legend toggle enabled | `true` |
| Hit threshold | `30.0` px |
| Finding strategy | `NEAREST` |
| Zoom mode | `NONE` |
| Zoom clamp enabled | `true` |
| Auto-fit Y on pan | `false` |
| Update throttle | `16.0` ms |

### Gauge Defaults

| Setting | Default |
|---|---|
| `GaugeMinValue` | `0.0` |
| `GaugeMaxValue` | `100.0` |
| `GaugeValue` | `0.0` |
| `GaugeArcSpan` | `270.0` degrees |
| `GaugeStartAngle` | `135.0` degrees |
| `GaugeArcWidth` | `0.18` (18% of radius) |
| `GaugeTrackColor` | `(0.85, 0.85, 0.85, 1.0)` |
| `GaugeProgressColor` | `(0.27, 0.51, 0.71, 1.0)` |

### Series Defaults

| Setting | Default |
|---|---|
| `LineSeries::LineWidth` | uses `ChartView::LINE_WIDTH` property |
| `LineSeries::Smoothness` | `0.0` (straight segments) |
| `LineSeries::FillEnabled` | `false` |
| `LineSeries::MarkerShape` | `CIRCLE` |
| `BarSeries::Stacked` | `false` |
| `BarSeries::BarGroupWidth` | `0.7` |
| `PieSeries::InnerRadiusRatio` | `0.0` (full pie, no donut hole) |
| `PieSeries::SliceGap` | `0.0` degrees |
| `ChartAxis::DataPadding` | `0.0` (Y axis auto-adds 5% internally) |
| `ChartAxis::MinStep` | `0.0` (auto-step) |

<br/>

---

## 18. Important Notes

- **`UiConfig` is required.** Call `UiConfig::New().Apply()` before `Application::MainLoop()` when using `dali-ui-components`. See [Configuration](Configuration.md).

- **Chart type is fixed at creation.** `ChartView::New(type, size)` determines the chart type for the lifetime of the object. There is no `SetType()` method — create a new `ChartView` if you need a different type.

- **`AREA` type uses `LineSeries` with fill.** `ChartView::Type::AREA` is a convenience alias. You must still create `LineSeries` and call `SetFillEnabled(true)` on each series to get the filled appearance. The type enum itself does not automatically enable fill.

- **NaN values create line gaps.** In `LineSeries`, inserting `std::numeric_limits<float>::quiet_NaN()` at any index breaks the polyline at that position. The gap applies to both the line and marker rendering. NaN points are skipped in hit testing.

- **`Gauge` type does not use `AddSeries()`.** `ChartView::Type::GAUGE` uses a dedicated set of `SetGauge*()` and `AddGaugeRange()` methods. Calling `AddSeries()` on a GAUGE chart has no effect.

- **`ChartSection` NaN boundaries.** Setting `SetXMin(NaN)` or `SetXMax(NaN)` extends the section to the left or right edge of the plot area respectively. The same applies to `SetYMin(NaN)` / `SetYMax(NaN)`.

- **Layout placement.** `ChartView` must be added to the dali-ui layout tree so that `OnArrange` is called with the correct bounds. For floating or overlay charts, use `AbsoluteLayout` with `AbsoluteLayoutParams` to control position and size precisely.

- **`SetUpdateThrottle(0.0f)` for immediate redraws.** The default throttle of `16 ms` batches rapid data changes into at most one redraw per frame. Use `0.0f` only when you need sub-frame immediacy; otherwise the default is appropriate for most use cases.

- **`SetFindingStrategy(SAME_X)` fires `MultiPointSelectedSignal`.** When using `SAME_X` or `SAME_X_NEAREST_Y`, the `MultiPointSelectedSignal` fires once for each matching point. `DataPointSelectedSignal` still fires for the closest single point independently.

- **`SetLegendToggleEnabled(false)` is signal-only mode.** The `LegendItemTappedSignal` still fires when this is `false`, but `ChartView` will not automatically hide or show the corresponding series. This lets you implement custom toggle logic in your callback.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
