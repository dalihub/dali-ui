[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/ChartView.md)

# DALi UI Components - ChartView

`ChartView`는 `dali-ui-components`를 위한 멀티 타입 차트 컴포넌트입니다. ThorVG 기반의 `CanvasView` 레이어를 사용하여 라인, 바, 파이, 에어리어, 스캐터, 게이지 차트를 렌더링하며, 줌/팬 제스처, 인터랙티브 툴팁, 애니메이션 데이터 전환, 실시간 데이터 피드를 완벽하게 지원합니다.

![ChartView gallery overview](assets/chartview/gallery-overview.gif)

---

## 목차

1. [기본 설정](#1-기본-설정)
2. [차트 타입](#2-차트-타입)
3. [LineSeries](#3-lineseries)
4. [BarSeries](#4-barseries)
5. [PieSeries](#5-pieseries)
6. [ScatterSeries](#6-scatterseries)
7. [ChartAxis](#7-chartaxis)
8. [ChartSection](#8-chartsection)
9. [속성](#9-속성)
10. [실시간 업데이트 및 성능](#10-실시간-업데이트-및-성능)
11. [인터랙션 및 줌/팬](#11-인터랙션-및-줌팬)
12. [시그널](#12-시그널)
13. [애니메이션](#13-애니메이션)
14. [게이지](#14-게이지)
15. [레이아웃 통합](#15-레이아웃-통합)
16. [CMake 통합](#16-cmake-통합)
17. [기본값](#17-기본값)
18. [주요 참고사항](#18-주요-참고사항)
19. [주요 참고사항](#19-주요-참고사항)

---

## 1. 기본 설정

> ⚠️ **주의**: `ChartView`는 `dali-ui-components`의 일부입니다. `MainLoop()` 호출 전에 반드시 `Components::UiConfig::New().Apply()`를 호출해야 합니다. 자세한 내용은 [Configuration](Configuration.md)을 참조하세요.

### main.cpp

```cpp
#include <dali-ui-components/dali-ui-components.h>
using namespace Dali;
using namespace Dali::Ui;

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Components::UiConfig는 MainLoop() 전에 적용
  Components::UiConfig::New().Apply();

  MyController controller(application);
  application.MainLoop();
  return 0;
}
```

### ChartView 생성

```cpp
#include <dali-ui-components/dali-ui-components.h>
using namespace Dali;
using namespace Dali::Ui;

// 명시적 크기로 LINE 차트 생성
ChartView chart = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

// 차트 구성
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

// 속성으로 그리드 및 범례 활성화
chart.SetProperty(ChartView::Property::SHOW_GRID, true);
chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);
```

### 레이아웃에 배치

```cpp
// 방법 1: AbsoluteLayoutParams와 함께 AbsoluteLayout 사용
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
// 방법 2: 별도의 레이아웃 파라미터 객체 없이 SetRequestedX/Y 사용
chart.SetRequestedX(20.0f);
chart.SetRequestedY(60.0f);
window.Add(chart);
```

> 💡 **팁**: `ChartView`는 표준 `View`이며 dali-ui 레이아웃 시스템에 참여합니다. 픽셀 단위의 정밀한 위치 지정이 필요할 때는 `AbsoluteLayout` 안에 배치하세요.

<br/>

---

## 2. 차트 타입

`ChartView::New()`에 타입을 전달합니다. 타입은 생성 후 변경할 수 없습니다.

![LINE and AREA](assets/chartview/chart-types-line-area.png)

![BAR and PIE](assets/chartview/chart-types-bar-pie.png)

![SCATTER and GAUGE](assets/chartview/chart-types-scatter-gauge.png)

| 타입 | 열거형 | 설명 |
|---|---|---|
| 라인 | `ChartView::Type::LINE` | 선택적 마커가 있는 연결된 라인 |
| 바 | `ChartView::Type::BAR` | 세로형 그룹 또는 누적 바 |
| 파이 | `ChartView::Type::PIE` | 원형 파이 차트 |
| 에어리어 | `ChartView::Type::AREA` | 라인 아래 채워진 영역 (`LineSeries`에 `SetFillEnabled(true)` 적용) |
| 스캐터 | `ChartView::Type::SCATTER` | 마커 형태의 XY 산점도 |
| 게이지 | `ChartView::Type::GAUGE` | 호(arc) 기반 게이지 (시리즈 없음 — 전용 Gauge API 사용) |

```cpp
ChartView lineChart    = ChartView::New(ChartView::Type::LINE,    Vector2(480, 360));
ChartView barChart     = ChartView::New(ChartView::Type::BAR,     Vector2(480, 360));
ChartView pieChart     = ChartView::New(ChartView::Type::PIE,     Vector2(400, 400));
ChartView areaChart    = ChartView::New(ChartView::Type::AREA,    Vector2(480, 360));
ChartView scatterChart = ChartView::New(ChartView::Type::SCATTER, Vector2(480, 360));
ChartView gaugeChart   = ChartView::New(ChartView::Type::GAUGE,   Vector2(360, 360));
```

> ⚠️ **주의**: `AREA`는 편의용 별칭입니다. 내부 동작은 `LINE`과 동일하며, 채워진 에어리어 형태를 얻으려면 여전히 `LineSeries`를 생성하고 `series.SetFillEnabled(true)`를 호출해야 합니다. 이 열거형 값 자체는 문서상의 의도 이외에 특별한 내부 동작을 유발하지 않습니다.

<br/>

---

## 3. LineSeries

`LineSeries`는 선택적 마커, 채우기(fill), 데이터 레이블이 있는 연결된 폴리라인을 렌더링합니다.

![Line and Area charts](assets/chartview/line-chart.png)

### 데이터 생성 및 추가

```cpp
chart.AddSeries(LineSeries::New()
    .SetName("Revenue")
    .SetColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f))
    .SetValues({120.0f, 190.0f, 150.0f, 250.0f, 220.0f, 280.0f}));
```

### 라인 스타일

```cpp
series.SetLineWidth(3.0f);          // 라인 획 너비 (픽셀 단위)
series.SetSmoothness(0.8f);         // 0.0 = 직선 세그먼트, 1.0 = 완전히 부드러운 베지어 곡선
```

### 마커

![Marker shapes example](assets/chartview/line-geometries.png)

```cpp
series.SetMarkersVisible(true);
series.SetMarkerShape(LineSeries::MarkerShape::CIRCLE);   // CIRCLE, SQUARE, TRIANGLE, DIAMOND
series.SetMarkerRadius(5.0f);
series.SetMarkerColor(Vector4(0.2f, 0.6f, 1.0f, 1.0f));
series.SetMarkerBorderColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
series.SetMarkerBorderWidth(1.5f);
```

| `MarkerShape` | 설명 |
|---|---|
| `CIRCLE` | 채워진 원 (기본값) |
| `SQUARE` | 채워진 사각형 |
| `TRIANGLE` | 위를 향하는 삼각형 |
| `DIAMOND` | 다이아몬드 (회전된 사각형) |

### 에어리어 (채우기)

![Area chart](assets/chartview/area-chart.png)

```cpp
// 에어리어 차트: 라인 아래를 채움
series.SetFillEnabled(true);
series.SetFillColor(Vector4(0.2f, 0.4f, 0.8f, 0.3f));  // 반투명
```

### 데이터 레이블

```cpp
series.SetDataLabelsVisible(true);
series.SetDataLabelFormat("%.1f");      // printf 스타일 형식 문자열
series.SetDataLabelColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
series.SetDataLabelSize(9.0f);          // 폰트 크기 (포인트 단위)

// 또는 커스텀 포매터 함수 사용
series.SetDataLabelFormatter([](float value, int pointIndex) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "$%.0f", value);
    return Dali::String(buf);
});
```

### NaN 간격

![NaN gap example](assets/chartview/nan-gap.png)

`NaN` 값을 삽입하여 라인을 끊고 눈에 보이는 간격을 만듭니다:

```cpp
#include <limits>
float NaN = std::numeric_limits<float>::quiet_NaN();

series.SetValues({100.0f, 150.0f, 120.0f,
                  NaN,              // 여기서 간격 — 인덱스 2와 4 사이에서 라인이 끊김
                  200.0f, 180.0f, 220.0f});
```

### Z-Index (그리기 순서)

`SetZIndex`는 모든 시리즈 타입(`LineSeries`, `BarSeries`, `ScatterSeries`)에서 사용할 수 있습니다. 값이 높을수록 위에 그려집니다.

![Z-Index example](assets/chartview/zindex.png)

```cpp
series.SetZIndex(10);   // 값이 높을수록 Z값이 낮은 시리즈 위에 그려짐
```

### 전체 예제

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

`BarSeries`는 세로 바를 렌더링합니다. 동일한 차트에 추가된 여러 `BarSeries`는 기본적으로 그룹으로 묶이며, `SetStacked(true)`를 설정하면 누적됩니다.

![Bar charts](assets/chartview/bar-chart.png)

### 기본 바 차트

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

### 누적 바

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

> 💡 **팁**: 누적 바와 그룹 바는 동일한 차트 안에서 공존할 수 있습니다. `SetStacked(true)`인 시리즈는 누적되고, `SetStacked(false)`(기본값)인 시리즈는 자신만의 그룹 슬롯을 차지합니다.

### 바 그룹 너비

```cpp
b1.SetBarGroupWidth(0.6f);   // 사용 가능한 슬롯 너비에 대한 비율 0.0–1.0; 기본값 0.7
```

### 데이터 레이블

```cpp
b1.SetDataLabelsVisible(true);
b1.SetDataLabelFormat("%.0f");
b1.SetDataLabelColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
b1.SetDataLabelSize(9.0f);
```

### BarSeries와 LineSeries 혼합

`BarSeries`와 `LineSeries`는 동일한 `ChartView::Type::LINE` 차트에 함께 추가할 수 있습니다. 라인이 바 위에 그려집니다.

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

`PieSeries`는 파이 차트를 그립니다. `SetInnerRadiusRatio()`를 0보다 크게 설정하면 도넛 차트가 됩니다.

### 기본 파이 차트

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

### 도넛 차트

![Donut chart with center label](assets/chartview/pie-donut.png)

```cpp
PieSeries pie = PieSeries::New();
pie.AddSlice("Asia",    42.0f, Vector4(0.39f, 0.58f, 0.93f, 1.0f));
pie.AddSlice("America", 28.0f, Vector4(1.0f,  0.39f, 0.28f, 1.0f));
pie.AddSlice("Europe",  20.0f, Vector4(0.24f, 0.70f, 0.44f, 1.0f));
pie.AddSlice("Others",  10.0f, Vector4(0.5f,  0.5f,  0.5f,  1.0f));

pie.SetInnerRadiusRatio(0.55f);    // 0.0 = 완전한 파이, >0.0 = 도넛 구멍
pie.SetCenterLabel("$4.2B");       // 도넛 구멍 안에 표시할 텍스트
pie.SetSliceGap(2.0f);             // 슬라이스 간 간격 (도 단위)
pie.SetDataLabelsVisible(true);
pie.SetDataLabelMinAngle(15.0f);   // 슬라이스 각도가 이 값보다 작으면 레이블 숨김
chart.AddSeries(pie);
```

### 슬라이스 관리

```cpp
pie.ClearSlices();
int count = pie.GetSliceCount();
```

<br/>

---

## 6. ScatterSeries

`ScatterSeries`는 XY 좌표 쌍을 연결선 없이 개별 마커로 표시합니다.

![Scatter chart](assets/chartview/scatter.png)

### XY 데이터

```cpp
ChartView chart = ChartView::New(ChartView::Type::SCATTER, Vector2(480.0f, 360.0f));
chart.SetProperty(ChartView::Property::SHOW_GRID, true);
chart.SetProperty(ChartView::Property::SHOW_LEGEND, true);

// SetValues는 (x, y) 쌍의 벡터를 받음
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

| `MarkerShape` | 설명 |
|---|---|
| `CIRCLE` | 채워진 원 |
| `SQUARE` | 채워진 사각형 |
| `TRIANGLE` | 위를 향하는 삼각형 |
| `DIAMOND` | 다이아몬드 |

> 💡 **팁**: 동일한 차트에 `ScatterSeries`와 `LineSeries`를 혼합하면 산점도 위에 추세선을 오버레이할 수 있습니다.

<br/>

---

## 7. ChartAxis

![Axis labels example](assets/chartview/axis-labels.png)

`ChartAxis`는 X축 또는 Y축의 레이블, 제목, 범위 한계, 그리드 스타일, 틱 간격을 제어합니다.

### 레이블 및 제목

```cpp
ChartAxis xAxis = ChartAxis::New();
xAxis.SetTitle("Quarter");
xAxis.SetLabels({"Q1", "Q2", "Q3", "Q4"});
chart.SetXAxis(xAxis);

ChartAxis yAxis = ChartAxis::New();
yAxis.SetTitle("Revenue (USD)");
chart.SetYAxis(yAxis);
```

### 수동 범위 한계

```cpp
yAxis.SetMinLimit(0.0f);
yAxis.SetMaxLimit(500.0f);
```

한계를 설정하지 않으면 축이 데이터에 맞게 자동으로 범위를 조정합니다(여백 포함).

### 데이터 여백

데이터 범위를 초과하는 추가 공간을 전체 범위의 비율로 지정합니다:

```cpp
yAxis.SetDataPadding(0.1f);   // 최댓값 위로 10% 여백
xAxis.SetDataPadding(0.0f);   // X축 여백 없음
```

### 최소 틱 간격

틱 마크가 지정한 값의 배수에 위치하도록 강제합니다:

```cpp
yAxis.SetMinStep(50.0f);   // Y 틱: 0, 50, 100, 150, ...
```

### 레이블 회전

데이터가 밀집된 경우 축 레이블을 회전하여 겹침을 방지합니다:

```cpp
xAxis.SetLabelsRotation(45.0f);   // 45도 회전
```

### 그리드 및 축 라인 스타일

```cpp
yAxis.SetGridColor(Vector4(0.0f, 0.5f, 1.0f, 0.6f));
yAxis.SetGridDash(4.0f, 4.0f);             // 점선 그리드: 4px 대시, 4px 간격
yAxis.SetAxisLineColor(Vector4(0.2f, 0.2f, 0.2f, 1.0f));
yAxis.SetAxisLineWidth(2.0f);
```

### 전체 예제

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

`ChartSection`은 차트 플롯 영역 위에 강조 표시된 구역을 오버레이합니다. 좌표는 데이터 공간(축 값과 동일한 단위)으로 지정합니다.

![Chart sections example](assets/chartview/sections.png)

### 섹션 타입

타입은 어떤 경계가 설정됐는지에 따라 결정됩니다:

| 패턴 | 결과 |
|---|---|
| `SetXMin` + `SetXMax` 만 설정 | 전체 Y 범위에 걸친 세로 밴드 |
| `SetYMin` + `SetYMax` 만 설정 | 전체 X 범위에 걸친 가로 밴드 |
| `SetXMin == SetXMax` | 세로 임계선 |
| `SetYMin == SetYMax` | 가로 임계선 |
| 4개 경계 모두 설정 | 직사각형 영역 |

경계값이 `NaN`이면 해당 방향의 플롯 영역 끝까지 확장됩니다.

### 세로 밴드 (X 범위 강조)

```cpp
ChartSection xBand = ChartSection::New();
xBand.SetXMin(2.5f);
xBand.SetXMax(5.5f);
xBand.SetFillColor(Vector4(0.2f, 0.7f, 0.3f, 0.12f));
xBand.SetStrokeColor(Vector4(0.2f, 0.7f, 0.3f, 0.5f));
xBand.SetStrokeWidth(1.5f);
chart.AddSection(xBand);
```

### 가로 밴드 (Y 범위 강조)

```cpp
ChartSection yBand = ChartSection::New();
yBand.SetYMin(200.0f);
yBand.SetYMax(280.0f);
yBand.SetFillColor(Vector4(1.0f, 0.85f, 0.2f, 0.10f));
chart.AddSection(yBand);
```

### 가로 임계선

```cpp
ChartSection hLine = ChartSection::New();
hLine.SetYMin(250.0f);
hLine.SetYMax(250.0f);   // 같은 값 = 선
hLine.SetStrokeColor(Vector4(1.0f, 0.3f, 0.3f, 0.9f));
hLine.SetStrokeWidth(2.0f);
chart.AddSection(hLine);
```

### 세로 기준선

```cpp
ChartSection vLine = ChartSection::New();
vLine.SetXMin(6.0f);
vLine.SetXMax(6.0f);   // 같은 값 = 선
vLine.SetStrokeColor(Vector4(0.4f, 0.4f, 1.0f, 0.8f));
vLine.SetStrokeWidth(2.0f);
chart.AddSection(vLine);
```

### 직사각형 영역

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

### 섹션 관리

```cpp
chart.RemoveSection(xBand);
chart.ClearSections();
```

<br/>

---

## 9. 속성

`chart.SetProperty(ChartView::Property::PROPERTY_NAME, value)`를 사용하여 차트 전체의 외관 및 동작을 구성합니다.

| 속성 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `SHOW_GRID` | `bool` | `true` | 그리드 라인 표시 |
| `SHOW_LEGEND` | `bool` | `true` | 시리즈 범례 표시 |
| `SHOW_TOOLTIP` | `bool` | `true` | 호버/터치 시 데이터 포인트 툴팁 표시 |
| `BACKGROUND_COLOR` | `Vector4` | 흰색 `(1,1,1,1)` | 차트 배경 색상 |
| `GRID_COLOR` | `Vector4` | `(0.9,0.9,0.9,1)` | 그리드 라인 색상 |
| `ANIMATION_DURATION` | `float` (ms) | `0.0` | 데이터 변경 애니메이션 지속시간 (0 = 비활성화) |
| `Y_AXIS_AUTO_RANGE` | `bool` | `true` | 데이터 범위에 맞게 Y축 자동 조정 |
| `LEGEND_POSITION` | `int` | `1` (TOP) | 범례 위치: 0=NONE, 1=TOP, 2=BOTTOM, 3=LEFT, 4=RIGHT |
| `AXIS_LABEL_SIZE` | `float` | `11.0` | 축 틱 레이블 폰트 크기 (포인트 단위) |
| `TITLE_SIZE` | `float` | `16.0` | 차트 제목 폰트 크기 (포인트 단위) |
| `LINE_WIDTH` | `float` | `2.5` | 모든 시리즈의 기본 라인 너비 (시리즈별 재정의 가능) |
| `SHOW_MARKERS` | `bool` | `true` | 마커 가시성 전체 토글 |
| `MARKER_RADIUS` | `float` | `4.0` | 기본 마커 반지름 (시리즈별 재정의 가능) |
| `HOVER_ENABLED` | `bool` | `true` | 호버 (포인터/스타일러스) 인터랙션 활성화 |
| `TOUCH_ENABLED` | `bool` | `true` | 터치 (손가락) 인터랙션 활성화 |

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

### 제목 위치

```cpp
chart.SetTitle("Sales Dashboard");
chart.SetTitlePosition(ChartView::TitlePosition::TOP_LEFT);   // TOP_CENTER (기본값), TOP_LEFT, TOP_RIGHT, BOTTOM_CENTER
chart.SetTitleColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f));
```

### 시리즈 관리

```cpp
chart.AddSeries(series);
chart.RemoveSeries("Revenue");   // 이름으로 제거
chart.RemoveAllSeries();
```

<br/>

---

## 10. 실시간 업데이트 및 성능

![Auto Updates and Real Time charts](assets/chartview/real-time.png)

### AppendValue를 이용한 슬라이딩 윈도우

실시간 데이터 피드(센서, 스트리밍)에는 `AppendValue()`와 `SetMaxDataPoints()`를 함께 사용하여 고정 길이의 슬라이딩 윈도우를 유지합니다:

```cpp
LineSeries series = LineSeries::New();
series.SetName("CPU Usage");
series.SetMaxDataPoints(60);   // 최대 60개 포인트 유지; 초과 시 가장 오래된 포인트 제거

chart.AddSeries(series);

// 타이머 콜백 또는 이벤트 핸들러 내부:
series.AppendValue(newY);          // Y 값 하나를 추가; X 인덱스 자동 증가
```

### 일괄 추가

```cpp
// 초기 값으로 시드
series.AppendValues({30.0f, 55.0f, 40.0f, 80.0f, 60.0f});
```

### 업데이트 스로틀

`SetUpdateThrottle()`은 데이터 변경이 화면 갱신을 트리거하는 빈도를 제한합니다. 기본값은 `16 ms`(~60 fps)입니다. 고빈도 데이터 소스에서 과도한 래스터화를 방지합니다:

```cpp
// 최대 100ms에 한 번 갱신 (10 fps) — 매우 빠른 센서에 유용
chart.SetUpdateThrottle(100.0f);

// 스로틀 비활성화: AppendValue 호출마다 즉시 갱신
chart.SetUpdateThrottle(0.0f);
```

### 실시간 전체 예제

```cpp
// 나중에 AppendValue를 호출하기 위해 참조 보관
LineSeries liveSeries = LineSeries::New();
liveSeries.SetName("Temperature");
liveSeries.SetColor(Vector4(1.0f, 0.4f, 0.2f, 1.0f));
liveSeries.SetMaxDataPoints(120);
liveSeries.SetMarkersVisible(false);

chart.AddSeries(liveSeries);
chart.SetUpdateThrottle(50.0f);   // 초당 최대 20회 갱신

// 애플리케이션 루프에서 50ms마다:
liveSeries.AppendValue(readSensorTemperature());
```

<br/>

---

## 11. 인터랙션 및 줌/팬

### 줌 모드

![Zoom & Pan interaction](assets/chartview/zoom-pan.png)

`SetZoomMode()`는 `ChartView::ZoomMode` 플래그의 비트마스크를 받습니다:

| 플래그 | 값 | 설명 |
|---|---|---|
| `NONE` | `0` | 줌 또는 팬 없음 |
| `PAN_X` | `1 << 0` | X축 방향 팬 |
| `PAN_Y` | `1 << 1` | Y축 방향 팬 |
| `ZOOM_X` | `1 << 2` | X축 핀치 줌 |
| `ZOOM_Y` | `1 << 3` | Y축 핀치 줌 |

```cpp
// X 팬 및 X 줌 활성화 (주식 차트에서 일반적인 패턴)
chart.SetZoomMode(
    static_cast<int>(ChartView::ZoomMode::PAN_X) |
    static_cast<int>(ChartView::ZoomMode::ZOOM_X));

// 완전한 2D 팬 및 줌 활성화
chart.SetZoomMode(
    static_cast<int>(ChartView::ZoomMode::PAN_X)  |
    static_cast<int>(ChartView::ZoomMode::PAN_Y)  |
    static_cast<int>(ChartView::ZoomMode::ZOOM_X) |
    static_cast<int>(ChartView::ZoomMode::ZOOM_Y));

// 모든 인터랙션 비활성화
chart.SetZoomMode(static_cast<int>(ChartView::ZoomMode::NONE));
```

### 줌 헬퍼

```cpp
chart.SetZoomClampEnabled(true);    // 데이터 범위를 벗어난 팬 방지
chart.SetAutoFitYOnPan(true);       // 팬 중에 표시되는 X 윈도우에 맞게 Y축 자동 스케일
chart.ResetZoom();                  // 전체 데이터 뷰로 복귀
```

### 툴팁 탐색 전략

사용자가 호버하거나 탭할 때 강조 표시할 데이터 포인트를 제어합니다:

| 전략 | 설명 |
|---|---|
| `NEAREST` | 모든 시리즈에서 가장 가까운 단일 포인트 (기본값) |
| `SAME_X` | 모든 시리즈에서 동일한 X 인덱스를 공유하는 모든 포인트 |
| `SAME_X_NEAREST_Y` | 동일한 X에서 Y가 가장 가까운 모든 포인트 |

```cpp
chart.SetFindingStrategy(ChartView::FindingStrategy::SAME_X);
```

### 커스텀 툴팁 포매터

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

### 범례 토글

![Dynamic visibility via legend toggle](assets/chartview/dynamic-visibility.png)

활성화하면 범례 항목을 탭하여 해당 시리즈를 숨기거나 표시할 수 있습니다:

```cpp
chart.SetLegendToggleEnabled(true);    // 기본값: true
chart.SetLegendToggleEnabled(false);   // 시그널 전용 모드: LegendItemTappedSignal은 발생하지만 가시성은 변경되지 않음
```

### 히트 임계값

```cpp
chart.SetHitThreshold(30.0f);   // 탭이 히트로 인식되는 픽셀 거리 (기본값 30)
```

<br/>

---

## 12. 시그널

![Custom tooltip formatter](assets/chartview/formatter.png)

### DataPointSelectedSignal

사용자가 데이터 포인트를 탭하거나 호버할 때 발생합니다.

```cpp
class MyHandler : public Dali::ConnectionTracker
{
public:
    void OnPointSelected(const ChartPointEventArgs& e)
    {
        // e.seriesIndex  — 0부터 시작하는 시리즈 인덱스
        // e.pointIndex   — 시리즈 내 0부터 시작하는 포인트 인덱스
        // e.dataX        — 선택된 포인트의 X 값
        // e.dataY        — 선택된 포인트의 Y 값
        // e.seriesName   — SetName()으로 설정한 이름
        // e.xLabel       — ChartAxis::SetLabels()의 레이블 문자열
        DALI_LOG_RELEASE_INFO("Hit series %d, point %d: [%s] = %.1f\n",
                              e.seriesIndex, e.pointIndex,
                              e.xLabel.CStr(), e.dataY);
    }
};

MyHandler handler;
chart.DataPointSelectedSignal().Connect(&handler, &MyHandler::OnPointSelected);
```

### LegendItemTappedSignal

사용자가 범례 항목을 탭할 때 발생합니다.

```cpp
void OnLegendTapped(int seriesIndex, bool isNowVisible)
{
    DALI_LOG_RELEASE_INFO("Legend series %d toggled -> %s\n",
                          seriesIndex, isNowVisible ? "visible" : "hidden");
}

chart.LegendItemTappedSignal().Connect(&handler, &MyHandler::OnLegendTapped);
```

### MultiPointSelectedSignal

`FindingStrategy::SAME_X` 또는 `SAME_X_NEAREST_Y`가 활성화된 경우 매칭된 각 포인트마다 발생합니다. `DataPointSelectedSignal`과 동일한 `ChartPointEventArgs` 레이아웃으로 매칭되는 포인트마다 한 번씩 발생합니다.

```cpp
chart.SetFindingStrategy(ChartView::FindingStrategy::SAME_X);
chart.MultiPointSelectedSignal().Connect(&handler, &MyHandler::OnMultiPoint);
```

### ZoomedSignal

줌 또는 팬 제스처가 완료된 후 데이터 좌표의 새로운 뷰포트를 보고하며 발생합니다.

```cpp
void OnZoomed(const ChartViewportArgs& vp)
{
    // vp.xMin, vp.xMax — 데이터 좌표의 표시되는 X 범위
    // vp.yMin, vp.yMax — 데이터 좌표의 표시되는 Y 범위
    DALI_LOG_RELEASE_INFO("Viewport X[%.1f ~ %.1f] Y[%.1f ~ %.1f]\n",
                          vp.xMin, vp.xMax, vp.yMin, vp.yMax);
}

chart.ZoomedSignal().Connect(&handler, &MyHandler::OnZoomed);
```

### 시그널 요약

| 시그널 | 콜백 시그니처 | 발생 시점 |
|---|---|---|
| `DataPointSelectedSignal` | `void(const ChartPointEventArgs&)` | 단일 포인트를 탭하거나 호버할 때 |
| `LegendItemTappedSignal` | `void(int index, bool isVisible)` | 범례 항목을 탭할 때 |
| `MultiPointSelectedSignal` | `void(const ChartPointEventArgs&)` | SAME_X 전략에서 매칭되는 각 포인트마다 |
| `ZoomedSignal` | `void(const ChartViewportArgs&)` | 줌 또는 팬 제스처가 끝날 때 |

<br/>

---

## 13. 애니메이션

![Animated data transition](assets/chartview/animation.png)

`ChartView`는 `SetValues()`를 통해 데이터가 업데이트될 때 전환을 애니메이션으로 표현할 수 있습니다.

### 애니메이션 활성화

```cpp
chart.SetAnimationDuration(300.0f);                              // 300 ms
chart.SetAnimationEasing(ChartView::EasingType::EASE_OUT);       // 이징 함수
```

| `EasingType` | 설명 |
|---|---|
| `LINEAR` | 전체적으로 일정한 속도 |
| `EASE_OUT` | 빠른 시작, 끝에서 감속 (자연스러운 느낌) |
| `EASE_IN_OUT` | 느린 시작, 빠른 중간, 느린 끝 (부드러운 느낌) |

### 애니메이션 예제

```cpp
chart.SetAnimationDuration(300.0f);
chart.SetAnimationEasing(ChartView::EasingType::EASE_OUT);

// 다음 SetValues() 호출이 전환을 애니메이션으로 표현
series.SetValues({60.0f, 40.0f, 280.0f, 100.0f, 310.0f, 180.0f});
```

> ⚠️ **주의**: `ANIMATION_DURATION` 속성과 `SetAnimationDuration()`은 동일한 설정을 제어합니다. `ANIMATION_DURATION`을 `0.0`(기본값)으로 설정하면 애니메이션이 비활성화됩니다.

<br/>

---

## 14. 게이지

`GAUGE` 차트 타입은 원형 호 게이지를 렌더링합니다. `AddSeries()`를 사용하지 **않으며**, 모든 구성은 `ChartView`의 전용 게이지 메서드로 수행합니다.

![Gauge chart](assets/chartview/gauge.png)

### 기본 게이지

```cpp
ChartView gauge = ChartView::New(ChartView::Type::GAUGE, Vector2(360.0f, 360.0f));
gauge.SetTitle("CPU Usage");

gauge.SetGaugeMinValue(0.0f);
gauge.SetGaugeMaxValue(100.0f);
gauge.SetGaugeValue(72.0f);        // 현재 바늘 위치

// 색상 구역
gauge.AddGaugeRange( 0.0f,  60.0f, Vector4(0.24f, 0.70f, 0.44f, 1.0f));  // 초록
gauge.AddGaugeRange(60.0f,  80.0f, Vector4(1.0f,  0.7f,  0.0f,  1.0f));  // 노랑
gauge.AddGaugeRange(80.0f, 100.0f, Vector4(1.0f,  0.39f, 0.28f, 1.0f));  // 빨강
```

### 호 형태

```cpp
gauge.SetGaugeArcSpan(270.0f);         // 호의 전체 각도 (도 단위, 기본값 270)
gauge.SetGaugeStartAngle(135.0f);      // 호가 시작되는 각도 (기본값 135, 6시 방향 = 270)
gauge.SetGaugeArcWidth(0.18f);         // 호 너비 (반지름의 비율, 기본값 0.18)
```

### 색상

```cpp
gauge.SetGaugeTrackColor(Vector4(0.85f, 0.85f, 0.85f, 1.0f));     // 배경 호 색상
gauge.SetGaugeProgressColor(Vector4(0.27f, 0.51f, 0.71f, 1.0f));  // 진행 호 색상 (범위에 의해 재정의됨)
```

> 💡 **팁**: `SetGaugeProgressColor()`는 기본 진행 색상을 설정합니다. `AddGaugeRange()` 항목이 있으면 각 범위 세그먼트는 자체 색상을 사용하며 `SetGaugeProgressColor()`는 무시됩니다.

### 중앙 레이블

```cpp
gauge.SetGaugeCenterLabel("72%");
```

### 값 업데이트

```cpp
// 이벤트(예: 타이머 또는 센서 콜백)에 응답하여 업데이트
gauge.SetGaugeValue(newValue);
```

### 범위 관리

```cpp
gauge.ClearGaugeRanges();
gauge.AddGaugeRange(0.0f, 50.0f, Vector4(0.2f, 0.8f, 0.4f, 1.0f));
gauge.AddGaugeRange(50.0f, 100.0f, Vector4(1.0f, 0.3f, 0.3f, 1.0f));
```

### 게이지 API 요약

| 메서드 | 설명 |
|---|---|
| `SetGaugeValue(float)` | 현재 게이지 위치 설정 |
| `SetGaugeMinValue(float)` | 최솟값 설정 |
| `SetGaugeMaxValue(float)` | 최댓값 설정 |
| `SetGaugeArcSpan(float)` | 호의 전체 스윕 각도 (도 단위) |
| `SetGaugeStartAngle(float)` | 호의 시작 각도 |
| `SetGaugeArcWidth(float)` | 반지름 대비 호 두께 비율 |
| `SetGaugeTrackColor(Vector4)` | 배경 트랙 색상 |
| `SetGaugeProgressColor(Vector4)` | 기본 진행 색상 |
| `SetGaugeCenterLabel(String)` | 중앙에 표시할 텍스트 |
| `AddGaugeRange(float, float, Vector4)` | 색상 코드 값 범위 추가 |
| `ClearGaugeRanges()` | 모든 색상 범위 제거 |

<br/>

---

## 15. 레이아웃 통합

`ChartView`는 `View`를 상속하며 dali-ui 레이아웃 시스템과 완전히 통합됩니다.

### AbsoluteLayout으로 배치

```cpp
ChartView chart = ChartView::New(ChartView::Type::LINE, Vector2(480.0f, 360.0f));

// AbsoluteLayoutParams로 위치와 크기 설정
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

### 위치만 설정 (크기는 New에서 지정)

```cpp
// 크기는 ChartView::New()에서 고정; 위치만 설정
chart.SetRequestedX(50.0f);
chart.SetRequestedY(100.0f);
window.Add(chart);
```

### 윈도우 중앙 배치

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

### 동적 크기 조정

새 크기로 `SetLayoutParams()`를 호출하고 차트를 레이아웃에 다시 추가하여 `OnArrange`를 트리거합니다:

```cpp
chart.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX(x).SetY(y).SetWidth(newW).SetHeight(newH));
rootLayout.Remove(chart);
rootLayout.Add(chart);
```

<br/>

---

## 16. CMake 통합

`CMakeLists.txt`에 다음을 추가하세요:

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

> 💡 **팁**: 샘플 `samples/chart-view/CMakeLists.txt`에는 디버그/릴리즈 플래그 및 `HIDE_DALI_INTERNALS`를 포함한 완전한 빌드 구성 예시가 있습니다.

<br/>

---

## 17. 기본값

### ChartView (StyleConfig 기본값)

| 설정 | 기본값 |
|---|---|
| `SHOW_GRID` | `true` |
| `SHOW_LEGEND` | `true` |
| `SHOW_TOOLTIP` | `true` |
| `SHOW_MARKERS` | `true` |
| `BACKGROUND_COLOR` | 흰색 `(1.0, 1.0, 1.0, 1.0)` |
| `GRID_COLOR` | `(0.9, 0.9, 0.9, 1.0)` |
| `AXIS_LABEL_SIZE` | `11.0` pt |
| `TITLE_SIZE` | `16.0` pt |
| `LINE_WIDTH` | `2.5` px |
| `MARKER_RADIUS` | `4.0` px |
| `TOUCH_ENABLED` | `true` |
| `HOVER_ENABLED` | `true` |
| `ANIMATION_DURATION` | `0.0` ms (비활성화) |
| `ANIMATION_EASING` | `EASE_OUT` |
| `LEGEND_POSITION` | `TOP` |
| `TITLE_POSITION` | `TOP_CENTER` |
| 범례 토글 활성화 | `true` |
| 히트 임계값 | `30.0` px |
| 탐색 전략 | `NEAREST` |
| 줌 모드 | `NONE` |
| 줌 클램프 활성화 | `true` |
| 팬 시 Y 자동 맞춤 | `false` |
| 업데이트 스로틀 | `16.0` ms |

### 게이지 기본값

| 설정 | 기본값 |
|---|---|
| `GaugeMinValue` | `0.0` |
| `GaugeMaxValue` | `100.0` |
| `GaugeValue` | `0.0` |
| `GaugeArcSpan` | `270.0` 도 |
| `GaugeStartAngle` | `135.0` 도 |
| `GaugeArcWidth` | `0.18` (반지름의 18%) |
| `GaugeTrackColor` | `(0.85, 0.85, 0.85, 1.0)` |
| `GaugeProgressColor` | `(0.27, 0.51, 0.71, 1.0)` |

### 시리즈 기본값

| 설정 | 기본값 |
|---|---|
| `LineSeries::LineWidth` | `ChartView::LINE_WIDTH` 속성 사용 |
| `LineSeries::Smoothness` | `0.0` (직선 세그먼트) |
| `LineSeries::FillEnabled` | `false` |
| `LineSeries::MarkerShape` | `CIRCLE` |
| `BarSeries::Stacked` | `false` |
| `BarSeries::BarGroupWidth` | `0.7` |
| `PieSeries::InnerRadiusRatio` | `0.0` (완전한 파이, 도넛 구멍 없음) |
| `PieSeries::SliceGap` | `0.0` 도 |
| `ChartAxis::DataPadding` | `0.0` (Y축은 내부적으로 5% 자동 추가) |
| `ChartAxis::MinStep` | `0.0` (자동 간격) |

<br/>

---

## 18. 주요 참고사항

- **`Components::UiConfig`는 필수입니다.** `dali-ui-components`를 사용할 때는 `Application::MainLoop()` 전에 `Components::UiConfig::New().Apply()`를 호출해야 합니다. [Configuration](Configuration.md)을 참조하세요.

- **차트 타입은 생성 시 고정됩니다.** `ChartView::New(type, size)`는 객체의 생명주기 동안 차트 타입을 결정합니다. `SetType()` 메서드는 없으므로 다른 타입이 필요하면 새 `ChartView`를 생성해야 합니다.

- **`AREA` 타입은 채우기를 적용한 `LineSeries`를 사용합니다.** `ChartView::Type::AREA`는 편의용 별칭입니다. 채워진 형태를 얻으려면 여전히 `LineSeries`를 생성하고 각 시리즈에 `SetFillEnabled(true)`를 호출해야 합니다. 타입 열거형 자체는 채우기를 자동으로 활성화하지 않습니다.

- **NaN 값은 라인 간격을 만듭니다.** `LineSeries`에서 임의의 인덱스에 `std::numeric_limits<float>::quiet_NaN()`을 삽입하면 해당 위치에서 폴리라인이 끊깁니다. 간격은 라인과 마커 렌더링 모두에 적용됩니다. NaN 포인트는 히트 테스트에서 건너뜁니다.

- **`Gauge` 타입은 `AddSeries()`를 사용하지 않습니다.** `ChartView::Type::GAUGE`는 전용 `SetGauge*()` 및 `AddGaugeRange()` 메서드를 사용합니다. GAUGE 차트에서 `AddSeries()`를 호출해도 아무 효과가 없습니다.

- **`ChartSection` NaN 경계.** `SetXMin(NaN)` 또는 `SetXMax(NaN)`을 설정하면 섹션이 플롯 영역의 왼쪽 또는 오른쪽 끝까지 확장됩니다. `SetYMin(NaN)` / `SetYMax(NaN)`도 동일하게 적용됩니다.

- **레이아웃 배치.** `ChartView`는 올바른 경계로 `OnArrange`가 호출되도록 dali-ui 레이아웃 트리에 추가해야 합니다. 플로팅 또는 오버레이 차트의 경우 `AbsoluteLayout`과 `AbsoluteLayoutParams`를 사용하여 위치와 크기를 정밀하게 제어하세요.

- **즉각적인 갱신을 위한 `SetUpdateThrottle(0.0f)`.** 기본 스로틀 `16 ms`는 빠른 데이터 변경을 프레임당 최대 한 번의 갱신으로 묶습니다. 서브 프레임 즉각성이 필요한 경우에만 `0.0f`를 사용하세요. 대부분의 사용 사례에서는 기본값이 적합합니다.

- **`SetFindingStrategy(SAME_X)`는 `MultiPointSelectedSignal`을 발생시킵니다.** `SAME_X` 또는 `SAME_X_NEAREST_Y`를 사용할 때 `MultiPointSelectedSignal`은 매칭되는 각 포인트마다 한 번씩 발생합니다. `DataPointSelectedSignal`은 독립적으로 가장 가까운 단일 포인트에 대해 여전히 발생합니다.

- **`SetLegendToggleEnabled(false)`는 시그널 전용 모드입니다.** `false`일 때도 `LegendItemTappedSignal`은 발생하지만, `ChartView`는 해당 시리즈를 자동으로 숨기거나 표시하지 않습니다. 이를 통해 콜백에서 커스텀 토글 로직을 구현할 수 있습니다.

<br/>

---

[← 목록으로 돌아가기](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
