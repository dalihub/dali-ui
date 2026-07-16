[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/ChartView-Architecture.md)

# DALi UI Components — ChartView 아키텍처

> 이 문서는 컴포넌트 기여자 및 구현 내용을 이해해야 하는 개발자를 위해 `ChartView`의 내부 설계를 설명합니다.
> 사용 방법은 [ChartView 사용자 가이드](ChartView-(kr).md)를 참고하세요.

---

## 목차

1. [개요](#1-개요)
2. [패키지 구조](#2-패키지-구조)
3. [레이어 아키텍처](#3-레이어-아키텍처)
4. [Public API 레이어](#4-public-api-레이어)
   - 4.1 [Handle 패턴](#41-handle-패턴)
   - 4.2 [GetImplementation — ADL 패턴](#42-getimplementation--adl-패턴)
5. [Integration API 레이어](#5-integration-api-레이어)
   - 5.1 [ChartViewImpl](#51-chartviewimpl)
   - 5.2 [데이터 흐름](#52-데이터-흐름)
6. [렌더링 레이어](#6-렌더링-레이어)
   - 6.1 [3-캔버스 분리 및 설계 근거](#61-3-캔버스-분리-및-설계-근거)
   - 6.2 [ChartRenderer](#62-chartrenderer)
   - 6.3 [ChartLayoutManager](#63-chartlayoutmanager)
7. [데이터 모델](#7-데이터-모델)
8. [ScaleEngine](#8-scaleengine)
9. [HitTester](#9-hittester)
10. [제스처 처리](#10-제스처-처리)
11. [애니메이션 시스템](#11-애니메이션-시스템)

---

## 1. 개요

`ChartView`는 DALi UI Foundation 위에서 Line, Bar, Pie, Area, Scatter, Gauge 차트를 렌더링합니다. 벡터 그래픽을 위해 ThorVG로 구동되는 여러 `CanvasView` 레이어를 사용하고, 모든 텍스트(축 레이블, 제목, 범례, 툴팁)에는 `Ui::Label` 액터를 사용합니다.

주요 설계 결정 사항:

- **3-캔버스 분리** — 정적 배경, 동적 데이터, 인터랙티브 오버레이를 독립적인 래스터화 대상으로 분리하여, 터치 이벤트 발생 시 얇은 오버레이 레이어만 재래스터화합니다.
- **스로틀 타이머** — 레이아웃/렌더 사이클을 시작하기 전에 빠르게 연속 호출되는 `series.SetValues()`를 합산 처리합니다.
- **`Rebuild*()` 직접 호출 우회** — 초기 레이아웃 이후 데이터 업데이트 시, `RelayoutRequest()`를 거치지 않고 `RebuildData()`를 직접 호출한 뒤 `RequestRasterization()`을 실행합니다. `RelayoutRequest()`는 Adaptor를 깨우지 않기 때문입니다.
- **명시적 제스처 디텍터** — `ViewImpl`이 `EnableGestureDetection`을 노출하지 않으므로, `ChartViewImpl`이 `PanGestureDetector`와 `PinchGestureDetector` 멤버를 직접 관리합니다.

---

## 2. 패키지 구조

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

## 3. 레이어 아키텍처

```
┌──────────────────────────────────────────────┐
│              애플리케이션 코드                │
│  ChartView  LineSeries  ChartAxis  ...        │
└──────────────────────┬───────────────────────┘
                       │  public-api/chart/
┌──────────────────────▼───────────────────────┐
│            Public API 레이어                  │
│  Handle 클래스 (ChartView : View,             │
│  ChartSeries : BaseHandle, ...)               │
└──────────────────────┬───────────────────────┘
                       │  integration-api/chart/
┌──────────────────────▼───────────────────────┐
│          Integration API 레이어               │
│  ChartViewImpl  ChartSeriesImpl               │
│  ChartModel  StyleConfig                      │
└────────────┬──────────────┬──────────────────┘
             │              │
    ┌────────▼───┐   ┌──────▼──────────────────┐
    │ ScaleEngine│   │      렌더링 레이어        │
    │ HitTester  │   │  BackgroundRenderer      │
    └────────────┘   │  DataRenderer            │
                     │  OverlayRenderer         │
                     │  ChartLayoutManager      │
                     └─────────────────────────┘
```

| 레이어 | 위치 | 가시성 |
|--------|------|--------|
| Public API | `public-api/chart/` | 앱 개발자 |
| Extension API | `extension-api/` | DALi UI 모듈 제공자 |
| Integration API | `integration-api/chart/` | 컴포넌트 기여자 |
| 렌더링 헬퍼 | `integration-api/chart/` | `ChartViewImpl` 내부용 |
| DALi UI Foundation | `dali-ui-foundation/` | 공유 인프라 |

---

## 4. Public API 레이어

### 4.1 Handle 패턴

`ChartView`는 모든 dali-ui 뷰와 동일한 p-impl handle 패턴을 따릅니다 ([View 아키텍처](View.md) 참고).

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

`ChartView` handle을 복사하면 동일한 `ChartViewImpl`을 가리키는 두 번째 handle이 생성됩니다. 모든 handle이 소멸되면 impl도 함께 소멸됩니다.

시리즈 handle(`ChartSeries`, `LineSeries`, …)은 시리즈가 씬 액터가 아니기 때문에 `View` 대신 `Dali::BaseHandle`을 상속합니다. 해당 impl(`Integration::ChartSeries`, …)은 `Dali::BaseObject`를 상속합니다.

규칙 요약:

| 항목 | 규칙 |
|------|------|
| Handle 소멸자 | 반드시 **비가상(non-virtual)** |
| Handle 데이터 멤버 | 없음 — 모든 상태는 impl에 존재 |
| 로직 위치 | impl에만 존재 |
| 타입 등록 | impl `.cpp`에서 `DALI_TYPE_REGISTRATION_BEGIN` 사용 |

<br/>

### 4.2 GetImplementation — ADL 패턴

시리즈에 대한 `GetImplementation` 헬퍼는 `namespace Dali::Ui::Integration` 내부가 아닌 `namespace Dali::Ui` 내부에 정의됩니다. 이 위치 덕분에 네임스페이스 접두사 없이 **인수 의존적 조회(ADL, Argument-Dependent Lookup)**를 통해 접근할 수 있습니다.

```cpp
// integration-api/chart/chart-series-impl.h
// Defined at namespace Dali::Ui scope — outside Integration

inline Integration::ChartSeries& GetImplementation(Ui::ChartSeries& handle)
{
  DALI_ASSERT_ALWAYS(handle && "ChartSeries handle is empty");
  return static_cast<Integration::ChartSeries&>(handle.GetBaseObject());
}
```

impl 코드에서의 사용 예:

```cpp
// ADL resolves the call — no prefix needed
GetImplementation(series).DataChangedSignal().Connect(...);   // correct

// Integration::GetImplementation(series)...                  // would fail — wrong namespace
```

> **참고:** `ChartView` 자체는 `ViewImpl`이 제공하는 `GetImpl()`을 사용하여 `ChartView` handle로부터 `ChartViewImpl&`을 얻으며, 이는 표준 dali-ui 패턴과 동일합니다.

---

## 5. Integration API 레이어

### 5.1 ChartViewImpl

`ChartViewImpl`은 `ViewImpl`을 상속하며 두 가지 라이프사이클 메서드를 오버라이드합니다.

**`OnInitialize()`** — DALi handle이 생성된 후 한 번 호출됩니다:

- `ViewImpl::OnInitialize()`를 호출하여 `PIVOT=TOP_LEFT`, `PARENT_ORIGIN=TOP_LEFT`를 설정합니다.
- 생성자에서 지정된 크기를 `SetRequestedWidth/Height`로 등록하여 `AbsoluteLayoutParams` 없이도 레이아웃 시스템이 뷰를 측정할 수 있도록 합니다.
- `SYNCHRONOUS_LOADING=true`, `RASTERIZATION_REQUEST_MANUALLY=true`로 설정된 세 개의 `CanvasView` 레이어를 생성하고 `Self()`에 추가합니다.
- 스로틀 타이머(16 ms, 단발성)와 애니메이션 타이머(16 ms, 반복)를 생성합니다.
- `Self()`에 `TouchedSignal`과 `HoveredSignal`을 연결합니다.

**`OnArrange(bounds)`** — 뷰 위치 재조정이 필요할 때 레이아웃 시스템이 각 프레임마다 호출합니다:

```
ViewImpl::OnArrange는 의도적으로 호출하지 않습니다.

이유: 이 메서드는 모든 자식 View를 순회하며 AbsoluteLayoutParams가
없는 자식에 대해 Arrange(0,0,w,h)를 호출합니다. 캔버스 레이어는
MATCH_PARENT 요청 크기로 선언되어 있지만, 상속된 ViewImpl 레이아웃은
Layout 부모가 아닌 경우 이를 올바르게 전파하지 못하므로 SIZE가
0×0(WRAP_CONTENT 자연 크기 = 0)으로 초기화됩니다.

대신, OnArrange는 최종 self bounds를 반환하고(프레임워크가 반환된
LayoutRect를 Self()에 적용) Rebuild*() 함수를 직접 호출합니다.
ViewImpl::OnInitialize에서 설정된 PIVOT=TOP_LEFT 덕분에 POSITION은
피벗 연산 없이 왼쪽 상단 모서리로 매핑됩니다.

텍스트 레이블 위치(축 눈금, 범례, 제목)는 PlaceTextLabels()에서
Actor::Property::POSITION을 통해 설정됩니다. ViewImpl::OnArrange를
건너뛰기 때문에 레이아웃 시스템이 해당 위치를 초기화하지 않습니다.
또한 레이블에는 POSITION_USES_PIVOT=true를 설정하여, TOP_LEFT 이외의
피벗(예: Y축 눈금 레이블의 CENTER_RIGHT, X축 눈금 레이블의
TOP_CENTER)이 최종 화면 위치 계산 시 올바르게 적용됩니다.
```

```cpp
LayoutRect ChartViewImpl::OnArrange(const LayoutRect& bounds)
{
  // Intentionally skip ViewImpl::OnArrange — see comment above.
  // Self geometry is applied centrally by the framework (ViewImpl::Arrange)
  // from the returned bounds; this override does not set Self() directly.
  const Vector2 newSize(bounds.width, bounds.height);

  if(newSize != mSize && newSize.width > 0.0f && newSize.height > 0.0f)
  {
    mSize = newSize;
    mNeedsBackgroundUpdate = mNeedsDataUpdate = true;
  }
  if(mNeedsBackgroundUpdate) { RebuildBackground(); mNeedsBackgroundUpdate = false; }
  if(mNeedsDataUpdate)       { RebuildData();       mNeedsDataUpdate       = false; }

  return bounds;
}
```

<br/>

### 5.2 데이터 흐름

**`series.SetValues()`에 의해 트리거되는 정상 상태 업데이트:**

```
series.SetValues(newData)
  │
  │  Integration::ChartSeries::EmitDataChangedSignal()
  ▼
ChartViewImpl::OnSeriesDataChanged()
  │  mPendingUpdate = true
  │  throttleMs < 1 → 직접 호출, 그렇지 않으면 mUpdateThrottleTimer.Start()
  ▼
ChartViewImpl::OnUpdateThrottleTimer()
  │  mModel.ComputeAutoRange()
  │  UpdateScale()
  │
  ├─ animation.duration > 0 ─► mAnimTimer 시작 (16 ms 반복)
  │
  └─ 애니메이션 없고 mLastLayout 유효 ─► RebuildBackground() + RebuildData()
       │                                   각 캔버스에 RequestRasterization() 호출
       │                                   (즉시 Adaptor 깨움)
       └─ mLastLayout 유효하지 않음 ──────► mNeedsBackgroundUpdate = mNeedsDataUpdate = true
                                            RelayoutRequest()
                                            (다음 OnArrange 프레임으로 지연)
```

> **왜 데이터 업데이트 시 RelayoutRequest()를 우회하는가?**
> `RelayoutRequest()`는 뷰를 `LayoutController`에 등록할 뿐입니다. Adaptor에 새 렌더 패스를 예약하도록 신호를 보내지 않습니다. 타이머 구동 애니메이션이 다른 이벤트 없이 `RelayoutRequest()`를 호출하면 Adaptor는 유휴 상태를 유지하고 차트가 멈춥니다. `RebuildData()` + `RequestRasterization()`을 직접 호출하면 매 타이머 틱마다 Adaptor를 깨울 수 있습니다.

---

## 6. 렌더링 레이어

### 6.1 3-캔버스 분리 및 설계 근거

`ChartViewImpl`은 동일한 위치와 크기로 겹쳐 쌓인 세 개의 `CanvasView` 액터를 생성합니다:

```
Self() (ChartView 액터)
 ├── mBackgroundCanvas  (z-order 0)
 ├── mDataCanvas        (z-order 1)
 └── mOverlayCanvas     (z-order 2)
```

| 캔버스 | 내용 | 재래스터화 시점 |
|--------|------|----------------|
| `mBackgroundCanvas` | 차트 배경 채색, 격자선, 축선, 눈금 표시, 범례 스워치 | 크기 변경, 축 설정 변경, 스타일 변경 시 |
| `mDataCanvas` | 시리즈 형상 — 선, 채색 영역, 막대, 마커, 파이 슬라이스, 게이지 호, 데이터 레이블 | 데이터 변경 시 |
| `mOverlayCanvas` | 크로스헤어 선, 히트 포인트 강조 원 | 터치/호버 이벤트 발생 시 |

이 분리 구조 덕분에 인터랙티브 호버 이벤트 발생 시 `mOverlayCanvas`(일반적으로 얇은 크로스헤어)만 재래스터화하고, 더 무거운 `mBackgroundCanvas`와 `mDataCanvas`는 변경되지 않습니다. `ChartViewImpl`의 `mNeedsBackgroundUpdate` / `mNeedsDataUpdate` 플래그가 이 분리를 강제합니다.

세 캔버스 모두 다음과 같이 설정됩니다:
- `SYNCHRONOUS_LOADING = true` — 래스터화가 렌더 스레드에서 동기적으로 수행됩니다.
- `RASTERIZATION_REQUEST_MANUALLY = true` — 캔버스가 자동 래스터화하지 않으며, `RequestRasterization()`을 명시적으로 호출해야 합니다.
- `SENSITIVE = false` — 터치 이벤트가 `ChartView` 액터로 전달됩니다.

<br/>

### 6.2 ChartRenderer

각각 `Ui::CanvasView&` 참조를 받는 세 개의 렌더러 클래스:

**`BackgroundRenderer`**는 정적 요소를 렌더링합니다:

```
Render(canvas, model, scale, layout)
  ├── RenderBackground()    — 배경 사각형 채색
  ├── RenderSections()      — ChartSection 밴드 / 임계선
  ├── RenderGrid()          — 수평 / 수직 격자선 (파선 옵션 포함)
  ├── RenderAxes()          — X 및 Y 축선
  ├── RenderTickMarks()     — 각 격자 위치의 짧은 눈금 표시
  └── RenderLegendSwatches()— 범례 텍스트 옆의 색상 사각형 스워치
```

**`DataRenderer`**는 동적 시리즈 데이터를 렌더링하고 텍스트 오버레이를 위한 `DataLabelInfo`를 반환합니다:

```
Render(canvas, model, scale, layout, pOldYValues, animProgress)
  ├── RenderFillAreas()   — LineSeries 채색 다각형 (AREA 타입 / FillEnabled)
  ├── RenderLines()       — LineSeries 폴리라인 (부드러운 큐빅 베지어 옵션 포함)
  ├── RenderMarkers()     — 포인트별 마커 형상 (원/사각형/삼각형/마름모)
  ├── RenderScatters()    — ScatterSeries 포인트 클라우드
  ├── RenderBars()        — BarSeries 개별 막대
  ├── RenderStackedBars() — BarSeries 누적 막대
  ├── RenderPie()         — PieSeries 호 슬라이스
  ├── RenderGauge()       — 게이지 호 트랙 + 진행 호
  └── RenderDataLabels()  → vector<DataLabelInfo> 반환
```

`DataRenderer::Render()`는 `std::vector<DataLabelInfo>`를 반환합니다. `ChartViewImpl`은 이를 `mDataLabels`의 `Ui::Label` 액터 풀에 적용하며, 필요 시 새 레이블을 생성하고 초과분은 숨깁니다.

**`OverlayRenderer`**는 인터랙티브 장식 요소를 렌더링합니다:

```cpp
void RenderCrosshair(canvas, hitPos, scale, style);
void RenderHighlight(canvas, hitPos, color, radius);
void Clear(canvas);
```

<br/>

### 6.3 ChartLayoutManager

`ChartLayoutManager::ComputeLayout()`은 `totalSize`와 `ChartModel`을 `LayoutResult`로 변환합니다:

```
ComputeLayout(totalSize, model, scale)
  │
  ├── 축 제목 텍스트의 너비 / 높이 측정
  ├── Y축 눈금 레이블 너비를 측정하여 왼쪽 여백 결정
  ├── X축 눈금 레이블 높이를 측정하여 하단 여백 결정
  ├── plotArea(Rect<float>) 계산 = totalSize에서 모든 여백을 뺀 값
  ├── Y축 및 X축에 대해 ScaleEngine::ComputeNiceTicks() 호출
  ├── xTickLabels / yTickLabels 구성 (위치 + 텍스트 + 각도)
  ├── legendItems 구성 (아이콘 중심 + 텍스트 위치 + 이름)
  └── LayoutResult 반환
```

`LayoutResult`는 `ChartViewImpl::mLastLayout`에 캐시되며 세 렌더러와 `ChartHitTester` 모두가 재사용합니다.

주요 상수:

| 상수 | 값 | 목적 |
|------|----|------|
| `PADDING` | 10 px | 차트 외부 여백 |
| `TICK_LENGTH` | 5 px | 눈금 표시 길이 |
| `TICK_LABEL_GAP` | 4 px | 눈금과 레이블 사이 간격 |
| `LEGEND_SWATCH` | 12 px | 범례 색상 스워치 크기 |
| `LEGEND_GAP` | 6 px | 스워치와 레이블 텍스트 사이 간격 |

---

## 7. 데이터 모델

`ChartModel`은 `ChartViewImpl`이 단독으로 소유하는 순수 데이터 컨테이너입니다. 다음을 보유합니다:

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

`ComputeAutoRange()`는 데이터 또는 축 변경 후마다 호출됩니다. 다음을 처리합니다:
- NaN 포인트 제외
- BarSeries 기준선을 0으로 설정
- 누적 막대의 누산 합계
- 양쪽 축에 대한 패딩 비율(`SetDataPadding`)
- 막대 시리즈 X축 ±0.5 막대 너비 패딩

`StyleConfig`는 모든 시각적 및 동작 설정을 6개의 중첩 구조체로 집약합니다:

| 하위 구조체 | 필드 |
|-------------|------|
| `Visibility` | `showGrid`, `showLegend`, `showTooltip`, `showMarkers`, `legendToggleEnabled` |
| `RenderStyle` | `backgroundColor`, `gridColor`, `axisColor`, `titleColor`, `lineWidth`, `markerRadius` |
| `LayoutStyle` | `axisLabelSize`, `titleSize`, `legendPosition`, `titlePosition` |
| `Interaction` | `touchEnabled`, `hoverEnabled`, `hitThreshold`, `findingStrategy`, `zoomModeFlags`, `zoomClampEnabled`, `autoFitY` |
| `AnimationConfig` | `duration`, `easing` |
| `GaugeConfig` | `minValue`, `maxValue`, `value`, `arcSpanDegrees`, `startAngleDegrees`, `arcWidthRatio`, `trackColor`, `progressColor`, `centerLabel`, `ranges` |

---

## 8. ScaleEngine

`ScaleEngine`은 **데이터 좌표**와 **캔버스 픽셀 좌표** 사이를 매핑합니다. `ComputeAutoRange()` 호출 후(또는 줌/패닝이 뷰포트를 변경한 후) `ChartViewImpl::UpdateScale()`에 의해 갱신됩니다.

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

좌표 규약: `ToCanvasY`는 Y축을 반전하여 더 큰 데이터 값이 화면에서 위쪽에 표시되도록 합니다.

`ComputeNiceTicks()`는 표준 크기-분수 알고리즘(1×, 2×, 5×, 10× 거듭제곱 단위 간격)을 사용하여 "깔끔하게" 반올림된 눈금 값을 계산하며, `ChartAxis::SetMinStep()`으로 설정된 `minStep`으로 선택적으로 클램핑됩니다.

줌/패닝이 활성화된 경우(`mViewportActive == true`), `UpdateScale()`은 전체 자동 범위 대신 뷰포트의 데이터 공간 경계를 사용하므로, 모든 렌더링과 히트 테스팅이 자동으로 줌된 창 내에서 동작합니다.

---

## 9. HitTester

`ChartHitTester`는 터치 위치에 가장 가까운 데이터 포인트를 찾습니다. 세 `CanvasView` 레이어가 `Self()`와 동일한 원점 및 크기를 공유하므로, **액터 로컬 좌표**에서 동작하며 이는 캔버스 픽셀 좌표와 동일합니다.

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

전략 선택은 `ChartView::SetFindingStrategy()`로 제어합니다:

| `FindingStrategy` | 사용 메서드 | 사용 사례 |
|-------------------|-------------|-----------|
| `NEAREST` | `FindNearest` | 산점도 / 단일 시리즈 |
| `SAME_X` | `FindBySameX(nearestY=false)` | 다중 시리즈 툴팁 |
| `SAME_X_NEAREST_Y` | `FindBySameX(nearestY=true)` | 다중 시리즈, 가장 가까운 Y 선택 |

반복 도중 NaN 데이터 포인트는 건너뜁니다. 비가시 시리즈(`SetVisible(false)`)는 검색에서 제외됩니다.

`ChartViewImpl`에 반환되는 `HitResult` 필드:

| 필드 | 설명 |
|------|------|
| `isValid` | 임계값 내에 포인트가 없으면 `false` |
| `seriesIndex` / `pointIndex` | 모델 내 위치 |
| `dataX` / `dataY` | 데이터 공간 좌표 |
| `canvasPos` | 크로스헤어 배치를 위한 캔버스 공간 위치 |
| `seriesColor` | 강조 원 색상 지정에 사용 |
| `xLabel` | X축 레이블 문자열 또는 `dataX`의 문자열 변환값 |

---

## 10. 제스처 처리

`ViewImpl`이 `EnableGestureDetection`을 노출하지 않으므로, `ChartViewImpl`은 제스처 디텍터를 명시적 멤버로 소유합니다:

```cpp
Dali::PanGestureDetector   mPanDetector;
Dali::PinchGestureDetector mPinchDetector;
```

디텍터는 `SetZoomMode()`로 패닝 또는 줌이 활성화될 때만 생성되어 연결됩니다:

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

제스처 콜백은 DALi 시그널 시그니처 `Signal<void(Actor, PanGesture)>`에 맞게 인수를 값으로 받습니다:

```cpp
void OnPanGesture(Actor actor, Dali::PanGesture pan);
void OnPinchGesture(Actor actor, Dali::PinchGesture pinch);
```

**패닝**은 뷰포트를 데이터 공간에서 이동합니다. **핀치**는 핀치 시작 시 데이터 공간 중심을 기준으로 줌합니다. `SetZoomClampEnabled(true)`(기본값)인 경우 둘 다 전체 데이터 범위로 클램핑됩니다. 각 제스처 업데이트 후 `ApplyViewportToScale()`이 새 뷰포트 경계를 `ScaleEngine`에 반영하고 `RebuildBackground()` + `RebuildData()`를 직접 트리거합니다(스로틀 타이머 경로와 동일한 이유로 레이아웃 시스템을 우회합니다).

줌 모드 플래그는 비트 필드로 조합 가능합니다:

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

## 11. 애니메이션 시스템

`SetAnimationDuration(ms)`가 0이 아니고 데이터가 변경되면, `ChartViewImpl`은 새 데이터를 적용하기 전 현재 캔버스 Y 위치를 스냅샷으로 저장한 뒤, 설정된 지속 시간 동안 이전 위치와 새 위치 사이를 보간합니다.

**애니메이션 타이머 루프 (16 ms 틱):**

```
mAnimOldCanvasY = CaptureCanvasY()   ← 데이터 변경 전 스냅샷
  │
series.SetValues(newData)
  │
OnSeriesDataChanged()
  │  mAnimStartTime = now
  │  mAnimTimer.Start()
  ▼
OnAnimTimer()  [16 ms마다 실행]
  │  elapsed = now - mAnimStartTime
  │  rawT    = min(elapsed / duration, 1.0)
  │  t       = ApplyEasing(rawT, easing)
  │
  ├── RebuildDataAnimated(mAnimOldCanvasY, t)
  │     DataRenderer.Render(canvas, model, scale, layout,
  │                         &oldY, t)   ← lerp old→new Y
  │     mDataCanvas.RequestRasterization()
  │
  └── rawT >= 1.0 → 타이머 정지, mAnimOldCanvasY 초기화
```

`CaptureCanvasY()`는 각 시리즈 데이터 포인트를 `ScaleEngine::ToCanvasY()`를 통해 플랫 `vector<float>`로 변환하며 NaN 간격을 보존합니다. 렌더링 도중 `DataRenderer`는 각 캔버스 Y를 선형 보간합니다:

```
canvasY = oldY + (newY - oldY) * t
```

이전 또는 새 값 중 어느 한쪽이 NaN인 포인트는 보간을 건너뜁니다(간격이 즉시 표시됩니다).

**이징 함수:**

| `EasingType` | 공식 | 효과 |
|--------------|------|------|
| `LINEAR` (0) | `t` | 일정한 속도 |
| `EASE_OUT` (1) | `1 - (1-t)²` | 빠르게 시작, 천천히 종료 (기본값) |
| `EASE_IN_OUT` (2) | `t<0.5 ? 2t² : 1-2(1-t)²` | 천천히 시작, 천천히 종료 |

`mLastRenderedCanvasY`는 애니메이션 완료 시(`t == 1.0`) 갱신되어 다음 전환 시 `mAnimOldCanvasY`로 사용됩니다.

---

[← 컴포넌트 목록으로](Components.md) | [ChartView 사용자 가이드](ChartView-(kr).md)
