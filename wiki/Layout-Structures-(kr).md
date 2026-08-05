# DALi UI Foundation - Layout

`Layout`은 레이아웃 알고리즘을 사용하여 자식 뷰를 배치하는 View입니다. 레이아웃 시스템은 View 계층 구조 내에서 자식 뷰의 **크기(Measure)**와 **위치(Arrange)**를 계산합니다. dali-ui는 `StackLayout`, `FlexLayout`, `GridLayout`, `AbsoluteLayout` 네 가지 내장 레이아웃 타입을 제공합니다. 레이아웃 처리는 윈도우별 `LayoutController`에 의해 구동되며, 매 프레임마다 무효화된 레이아웃 루트에 대해 Measure와 Arrange를 실행합니다.

---

## 목차

1. [기본 설정](#1-기본-설정)
2. [StackLayout](#2-stacklayout)
   - [방향과 간격](#21-방향과-간격)
   - [Weight](#22-weight)
   - [교차축 정렬](#23-교차축-정렬)
3. [FlexLayout](#3-flexlayout)
   - [Direction과 Wrap](#31-direction과-wrap)
   - [Justify Content](#32-justify-content)
   - [Align Items와 Align Content](#33-align-items와-align-content)
   - [자식별 Flex 파라미터](#34-자식별-flex-파라미터)
4. [GridLayout](#4-gridlayout)
   - [행과 열 정의](#41-행과-열-정의)
   - [GridLength 크기 지정](#42-gridlength-크기-지정)
   - [그리드 간격](#43-그리드-간격)
   - [자식별 Grid 파라미터](#44-자식별-grid-파라미터)
5. [AbsoluteLayout](#5-absolutelayout)
   - [픽셀 좌표 배치](#51-픽셀-좌표-배치)
   - [비례 좌표 배치](#52-비례-좌표-배치)
6. [크기 상수와 공통 속성](#6-크기-상수와-공통-속성)
   - [WRAP_CONTENT와 MATCH_PARENT](#61-wrap_content와-match_parent)
   - [마진과 패딩](#62-마진과-패딩)
   - [LayoutParams 값 의미론](#63-layoutparams-값-의미론)
7. [커스텀 레이아웃 콜백](#7-커스텀-레이아웃-콜백)
8. [레이아웃 처리](#8-레이아웃-처리)
   - [2단계 레이아웃](#81-2단계-레이아웃)
   - [무효화 흐름](#82-무효화-흐름)
   - [레이아웃 결과 캐싱](#83-레이아웃-결과-캐싱)
9. [메서드 체이닝](#9-메서드-체이닝)
10. [기본값](#10-기본값)
11. [주의사항](#11-주의사항)

---

## 1. 기본 설정

정적 팩토리 메서드 `New()`로 레이아웃을 생성하고 자식 뷰를 추가합니다. 레이아웃의 요청 크기를 설정하고 윈도우에 추가합니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

// 윈도우를 채우는 세로 StackLayout 생성
StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);

// 자식 추가
View child1 = View::New();
child1.SetRequestedHeight(100.0f);
root.Add(child1);

View child2 = View::New();
child2.SetRequestedHeight(200.0f);
root.Add(child2);

// 씬에 추가
window.Add(root);
```

> **참고:** 모든 레이아웃 타입은 `Layout`을 상속하고, `Layout`은 `View`를 상속합니다. 자식 관리에는 `Add(View)`, `Insert(index, View)`, `Remove(View)`, `RemoveAllChildren()`을 사용합니다.

---

## 2. StackLayout

`StackLayout`은 자식을 단일 행 또는 열로 배치하며, 선택적으로 간격을 지정할 수 있습니다. 자식은 고정 크기를 가지거나 weight를 사용하여 남은 공간을 나눌 수 있습니다.

### 2.1 방향과 간격

```cpp
// 세로 스택 (기본값)
StackLayout vStack = StackLayout::New(StackOrientation::VERTICAL);

// 가로 스택
StackLayout hStack = StackLayout::New(StackOrientation::HORIZONTAL);

// 생성 후 방향 변경
vStack.SetOrientation(StackOrientation::HORIZONTAL);
StackOrientation ori = vStack.GetOrientation();

// 자식 간 간격 설정 (픽셀)
vStack.SetSpacing(10.0f);
float spacing = vStack.GetSpacing();
```

| 값 | 동작 |
|---|---|
| `StackOrientation::VERTICAL` | 자식을 위에서 아래로 배치. 기본값. |
| `StackOrientation::HORIZONTAL` | 자식을 왼쪽에서 오른쪽으로 배치. |

---

### 2.2 Weight

`weight > 0`인 자식들은 남은 주축 공간을 비례적으로 나눠 가집니다. 해당 자식의 주축 `RequestedWidth`/`RequestedHeight`는 무시됩니다. `weight == 0`인 자식들은 일반적으로 측정됩니다.

```cpp
// 상단 바: 고정 높이 (weight 0, 일반 측정)
View topBar = View::New();
topBar.SetBackgroundColor(Color::RED);
topBar.SetRequestedHeight(80.0f);
topBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
root.Add(topBar);

// 중간: weight 적용, 남은 공간 차지
View middle = View::New();
middle.SetBackgroundColor(Color::GREEN);
middle.SetLayoutParams(
    StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
root.Add(middle);

// 하단 바: 고정 높이
View bottomBar = View::New();
bottomBar.SetBackgroundColor(Color::BLUE);
bottomBar.SetRequestedHeight(60.0f);
bottomBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
root.Add(bottomBar);
```

> 세로 스택에서 `RequestedHeight`는 주축, `RequestedWidth`는 교차축입니다. 가로 스택에서는 축이 반대가 됩니다.

---

### 2.3 교차축 정렬

`StackLayoutParams`는 교차축에서 각 자식의 정렬 방식을 제어합니다.

```cpp
view.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));

StackLayoutParams params;
view.TryGetLayoutParams(params);
LayoutAlignment align = params.GetAlignment();
```

| 값 | 동작 |
|---|---|
| `LayoutAlignment::FILL` | 교차축을 채움. |
| `LayoutAlignment::START` | 시작(왼쪽/위)에 정렬. 기본값. |
| `LayoutAlignment::CENTER` | 교차축 중앙 정렬. |
| `LayoutAlignment::END` | 끝(오른쪽/아래)에 정렬. |

---

## 3. FlexLayout

`FlexLayout`은 CSS Flexbox 알고리즘을 사용하여 자식을 배치하며, 방향, 줄 바꿈, 유연한 크기 조정을 지원합니다.

```cpp
FlexLayout flex = FlexLayout::New();
flex.SetRequestedWidth(MATCH_PARENT);
flex.SetRequestedHeight(MATCH_PARENT);
```

### 3.1 Direction과 Wrap

```cpp
// Flex 방향 설정
flex.SetDirection(FlexDirection::ROW);
FlexDirection dir = flex.GetDirection();

// 줄 바꿈 설정
flex.SetWrap(FlexWrap::WRAP);
FlexWrap wrap = flex.GetWrap();
```

| `FlexDirection` | 동작 |
|---|---|
| `ROW` | 왼쪽에서 오른쪽. 기본값. |
| `ROW_REVERSE` | 오른쪽에서 왼쪽. |
| `COLUMN` | 위에서 아래. |
| `COLUMN_REVERSE` | 아래에서 위. |

| `FlexWrap` | 동작 |
|---|---|
| `NO_WRAP` | 한 줄, 줄 바꿈 없음. 기본값. |
| `WRAP` | 공간이 부족하면 다음 줄로 바꿈. |
| `WRAP_REVERSE` | 역방향으로 줄 바꿈. |

---

### 3.2 Justify Content

주축 방향의 자식 정렬을 제어합니다.

```cpp
flex.SetJustifyContent(FlexJustify::CENTER);
FlexJustify justify = flex.GetJustifyContent();
```

| 값 | 동작 |
|---|---|
| `FlexJustify::FLEX_START` | 자식을 시작 방향에 배치. 기본값. |
| `FlexJustify::FLEX_END` | 자식을 끝 방향에 배치. |
| `FlexJustify::CENTER` | 자식을 중앙에 배치. |
| `FlexJustify::SPACE_BETWEEN` | 자식 사이 균등 간격, 양 끝 간격 없음. |
| `FlexJustify::SPACE_AROUND` | 각 자식 주위에 균등 간격. |
| `FlexJustify::SPACE_EVENLY` | 자식과 양 끝 사이 모두 균등 간격. |

---

### 3.3 Align Items와 Align Content

```cpp
// 단일 줄의 교차축 정렬
flex.SetAlignItems(FlexAlign::STRETCH);
FlexAlign items = flex.GetAlignItems();

// 여러 줄의 교차축 정렬 (줄 바꿈 시)
flex.SetAlignContent(FlexAlign::CENTER);
FlexAlign content = flex.GetAlignContent();
```

| `FlexAlign` | 동작 |
|---|---|
| `AUTO` | 부모 설정을 따름. |
| `FLEX_START` | 시작에 정렬. |
| `FLEX_END` | 끝에 정렬. |
| `CENTER` | 중앙 정렬. |
| `STRETCH` | 채우기. AlignItems 기본값. |
| `BASELINE` | 베이스라인 정렬. |

---

### 3.4 자식별 Flex 파라미터

```cpp
view.SetLayoutParams(FlexLayoutParams::New()
    .SetFlexGrow(1.0f)
    .SetFlexShrink(0.0f)
    .SetFlexBasis(100.0f)
    .SetAlignSelf(FlexAlign::CENTER));

FlexLayoutParams params;
view.TryGetLayoutParams(params);
```

| 메서드 | 설명 | 기본값 |
|---|---|---|
| `SetFlexGrow(float)` | 남은 공간 분배 비율 | `0` |
| `SetFlexShrink(float)` | 공간 부족 시 축소 비율 | `1` |
| `SetFlexBasis(float)` | grow/shrink 적용 전 초기 주축 크기 (`WRAP_CONTENT`이면 자동) | `WRAP_CONTENT` |
| `SetAlignSelf(FlexAlign)` | 교차축 정렬 재정의 (`AUTO`면 부모의 alignItems를 따름) | `AUTO` |                                                                                                                                                                                            
### 예제

```cpp
FlexLayout root = FlexLayout::New();
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);
root.SetDirection(FlexDirection::ROW);
root.SetAlignItems(FlexAlign::STRETCH);
root.SetViewPadding(Extents(20, 20, 20, 20));

// 고정 너비 자식
View redBox = View::New();
redBox.SetBackgroundColor(Color::RED);
redBox.SetRequestedWidth(100.0f);
root.Add(redBox);

// 유연한 자식 (남은 공간 차지)
View greenBox = View::New();
greenBox.SetBackgroundColor(Color::GREEN);
greenBox.SetRequestedWidth(WRAP_CONTENT);
greenBox.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
root.Add(greenBox);

// 고정 너비 자식
View blueBox = View::New();
blueBox.SetBackgroundColor(Color::BLUE);
blueBox.SetRequestedWidth(80.0f);
root.Add(blueBox);

window.Add(root);
```

---

## 4. GridLayout

`GridLayout`은 구성 가능한 행과 열로 이루어진 그리드에 자식을 배치합니다. 각 자식은 `GridLayoutParams`를 사용하여 특정 셀에 배치됩니다.

```cpp
GridLayout grid = GridLayout::New();
grid.SetRequestedWidth(MATCH_PARENT);
grid.SetRequestedHeight(MATCH_PARENT);
```

### 4.1 행과 열 정의

```cpp
// 행과 열을 하나씩 추가
grid.AddRowDefinition(GridLength::Absolute(100.0f));
grid.AddRowDefinition(GridLength::Star(1.0f));
grid.AddColumnDefinition(GridLength::Absolute(200.0f));
grid.AddColumnDefinition(GridLength::Star(2.0f));

// 한 번에 모두 설정
grid.SetRowDefinitions({GridLength::Absolute(100.0f), GridLength::Star(1.0f)});
grid.SetColumnDefinitions({GridLength::Absolute(200.0f), GridLength::Star(2.0f)});

// 조회
uint32_t rows = grid.GetRowCount();
uint32_t cols = grid.GetColumnCount();
std::vector<GridLength> rowDefs = grid.GetRowDefinitions();
std::vector<GridLength> colDefs = grid.GetColumnDefinitions();

// 정의 초기화
grid.ClearRowDefinitions();
grid.ClearColumnDefinitions();
```

---

### 4.2 GridLength 크기 지정

`GridLength`는 행 높이와 열 너비의 계산 방식을 지정합니다.

| 팩토리 메서드 | 동작 |
|---|---|
| `GridLength::Absolute(float pixels)` | 고정 픽셀 크기. |
| `GridLength::Star(float factor)` | 남은 공간의 비례 배분. 기본 factor는 `1.0`. |
| `GridLength::Auto()` | 해당 행/열에서 가장 큰 자식에 맞게 크기 조정. |

```cpp
// 예: 헤더 (고정), 콘텐츠 (유연), 푸터 (자동)
grid.AddRowDefinition(GridLength::Absolute(60.0f));
grid.AddRowDefinition(GridLength::Star(1.0f));
grid.AddRowDefinition(GridLength::Auto());

// 두 열: 가용 너비의 1/3과 2/3
grid.AddColumnDefinition(GridLength::Star(1.0f));
grid.AddColumnDefinition(GridLength::Star(2.0f));
```

---

### 4.3 그리드 간격

```cpp
grid.SetRowSpacing(10.0f);
grid.SetColumnSpacing(10.0f);

float rowSpacing = grid.GetRowSpacing();
float colSpacing = grid.GetColumnSpacing();
```

---

### 4.4 자식별 Grid 파라미터

```cpp
view.SetLayoutParams(GridLayoutParams::New()
    .SetRow(1)
    .SetColumn(2)
    .SetRowSpan(1)
    .SetColumnSpan(3)
    .SetHorizontalAlignment(LayoutAlignment::CENTER)
    .SetVerticalAlignment(LayoutAlignment::FILL));

GridLayoutParams params;
view.TryGetLayoutParams(params);
```

| 메서드 | 설명 | 기본값 |
|---|---|---|
| `SetRow(uint32_t)` | 0 기반 행 인덱스 | `0` |
| `SetColumn(uint32_t)` | 0 기반 열 인덱스 | `0` |
| `SetRowSpan(uint32_t)` | 차지할 행 수 | `1` |
| `SetColumnSpan(uint32_t)` | 차지할 열 수 | `1` |
| `SetHorizontalAlignment(LayoutAlignment)` | 셀 내 가로 정렬 | `FILL` |
| `SetVerticalAlignment(LayoutAlignment)` | 셀 내 세로 정렬 | `FILL` |

### 예제

```cpp
GridLayout root = GridLayout::New();
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);
root.SetViewPadding(Extents(20, 20, 20, 20));

root.AddRowDefinition(GridLength::Absolute(100.0f));
root.AddRowDefinition(GridLength::Absolute(120.0f));
root.AddRowDefinition(GridLength::Absolute(80.0f));
root.AddColumnDefinition(GridLength::Absolute(200.0f));
root.AddColumnDefinition(GridLength::Absolute(160.0f));

root.SetRowSpacing(10.0f);
root.SetColumnSpacing(10.0f);

// Cell (0,0)
View cell00 = View::New();
cell00.SetBackgroundColor(Color::RED);
cell00.SetLayoutParams(GridLayoutParams::New());
root.Add(cell00);

// Cell (0,1)
View cell01 = View::New();
cell01.SetBackgroundColor(Color::GREEN);
cell01.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
root.Add(cell01);

// Cell (1,0)
View cell10 = View::New();
cell10.SetBackgroundColor(Color::BLUE);
cell10.SetLayoutParams(GridLayoutParams::New().SetRow(1));
root.Add(cell10);

window.Add(root);
```

---

## 5. AbsoluteLayout

`AbsoluteLayout`은 자식을 명시적 좌표와 크기로 배치합니다. 자식들은 서로 영향을 주지 않습니다.

```cpp
AbsoluteLayout absolute = AbsoluteLayout::New();
absolute.SetRequestedWidth(MATCH_PARENT);
absolute.SetRequestedHeight(MATCH_PARENT);
```

### 5.1 픽셀 좌표 배치

```cpp
View redBox = View::New();
redBox.SetBackgroundColor(Color::RED);
redBox.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(20.0f, 20.0f, 120.0f, 80.0f)));
absolute.Add(redBox);

// 개별 setter도 사용 가능
View blueBox = View::New();
blueBox.SetBackgroundColor(Color::BLUE);
blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX(280.0f).SetY(400.0f).SetWidth(100.0f).SetHeight(60.0f));
absolute.Add(blueBox);
```

| 메서드 | 설명 | 기본값 |
|---|---|---|
| `SetBounds(LayoutRect)` | 위치와 크기 설정 (x, y, width, height) | `(0, 0, -1, -1)` |
| `SetX(float)` / `SetY(float)` | 위치를 개별 설정 | `0` |
| `SetWidth(float)` / `SetHeight(float)` | 크기를 개별 설정 (`-1`이면 View 자체 크기 사용) | `-1` |
| `SetFlags(AbsoluteLayoutFlags)` | 비례 위치/크기 플래그 | `NONE` |

---

### 5.2 비례 좌표 배치

`AbsoluteLayoutFlags`를 사용하여 bounds 값을 부모 대비 비례값(0.0 ~ 1.0)으로 해석합니다.

```cpp
// 중앙 박스: 너비 50%, 높이 30%, 왼쪽에서 25%, 위에서 35% 위치
View greenBox = View::New();
greenBox.SetBackgroundColor(Color::GREEN);
greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.25f, 0.35f, 0.5f, 0.3f))
    .SetFlags(AbsoluteLayoutFlags::ALL));
absolute.Add(greenBox);                                                                                                                                                                                                                                                                    ```
                                                                                                                                                                                                                                                                                           | 값 | 동작 |
|---|---|
| `AbsoluteLayoutFlags::NONE` | 위치와 크기 모두 픽셀 값. 기본값. |
| `AbsoluteLayoutFlags::POSITION_PROPORTIONAL` | 위치가 부모 크기에 비례. |
| `AbsoluteLayoutFlags::SIZE_PROPORTIONAL` | 크기가 부모 크기에 비례. |
| `AbsoluteLayoutFlags::ALL` | 위치와 크기 모두 비례. |

---

## 6. 크기 상수와 공통 속성

### 6.1 WRAP_CONTENT와 MATCH_PARENT

모든 View는 `SetRequestedWidth()` / `SetRequestedHeight()`를 사용하여 원하는 크기를 지정합니다. 두 가지 특별한 상수가 있습니다:

```cpp
// 콘텐츠에 맞게 크기 조정 (기본값)
view.SetRequestedWidth(WRAP_CONTENT);   // -1.0f

// 부모 컨테이너 채우기
view.SetRequestedHeight(MATCH_PARENT);  // -2.0f

// 고정 픽셀 크기
view.SetRequestedWidth(200.0f);
```

| 상수 | 값 | 동작 |
|---|---|---|
| `WRAP_CONTENT` | `-1.0f` | 뷰가 콘텐츠에 맞게 크기 조정 (자연 크기 또는 자식 바운딩 박스). 기본값. |
| `MATCH_PARENT` | `-2.0f` | 부모 컨테이너의 가용 공간을 채움. |

---

### 6.2 마진과 패딩

```cpp
// 마진: 뷰 외부 여백
view.SetViewMargin(Extents(10, 10, 5, 5));  // start, end, top, bottom
Extents margin = view.GetViewMargin();

// 패딩: 뷰 내부 여백
view.SetViewPadding(Extents(20, 20, 20, 20));
Extents padding = view.GetViewPadding();
```

---

### 6.3 LayoutParams 값 의미론

`SetLayoutParams()`는 독립적인 복사본을 저장합니다. `TryGetLayoutParams(out)`은 독립적인 스냅샷을 복사해 내보내며, 변경 사항을 반영하고 레이아웃을 무효화하려면 수정한 스냅샷을 `SetLayoutParams()`에 다시 전달해야 합니다.

```cpp
auto params = GridLayoutParams::New().SetRowSpan(2).SetColumnSpan(2);

params.SetColumn(0);
viewA.SetLayoutParams(params);

params.SetColumn(1);
viewB.SetLayoutParams(params); // viewA는 column 0을 유지

GridLayoutParams snapshot;
viewA.TryGetLayoutParams(snapshot);
snapshot.SetRow(1);              // viewA는 아직 변경되지 않음
viewA.SetLayoutParams(snapshot); // 변경 반영 및 레이아웃 무효화
```

---

## 7. 커스텀 레이아웃 콜백

서브클래싱 없이 커스텀 measure/arrange 로직을 구현하려면 모든 View 또는 Layout에서 `SetMeasureCallback()`과 `SetArrangeCallback()`을 사용할 수 있습니다. 콜백이 설정되면 기본 LayoutManager보다 **우선** 적용됩니다.

```cpp
// Measure 콜백 시그니처: MeasuredSize(View, float widthConstraint, float heightConstraint)
MeasuredSize MyMeasure(View view, float widthConstraint, float heightConstraint)
{
    return MeasuredSize(200.0f, 100.0f);
}

// Arrange 콜백 시그니처: LayoutRect(View, const LayoutRect& bounds)
LayoutRect MyArrange(View view, const LayoutRect& bounds)
{
    return bounds; // 프레임워크가 x/y/width/height를 적용하는 최종 self bounds
}

Layout layout = Layout::New();
layout.SetMeasureCallback(LayoutMeasureCallback::New(&MyMeasure));
layout.SetArrangeCallback(LayoutArrangeCallback::New(&MyArrange));
```

멤버 함수의 경우 객체 인스턴스를 전달합니다:

```cpp
layout.SetMeasureCallback(LayoutMeasureCallback::New(this, &MyClass::OnMeasure));
layout.SetArrangeCallback(LayoutArrangeCallback::New(this, &MyClass::OnArrange));
```

---

## 8. 레이아웃 처리

### 8.1 2단계 레이아웃

매 프레임마다 `LayoutController`는 무효화된 레이아웃 루트에 대해 두 단계를 실행합니다:

1. **Measure** — 부모로부터 받은 너비/높이 제약 조건에 따라 각 View가 원하는 `MeasuredSize`를 계산합니다. 결과는 캐시됩니다. 제약 조건과 유효 스케일이 변경되지 않고 이 view에 무효화가 발생하지 않았다면 measure 구현은 호출되지 않습니다.
2. **Arrange** — 각 View에 `LayoutRect`(위치 + 크기)가 주어집니다. 정렬과 마진을 적용한 후, LayoutManager가 자식을 bounds 내에 배치합니다. 이 결과도 캐시됩니다. 입력 bounds, 유효 레이아웃 방향, 유효 스케일이 변경되지 않고 무효화가 발생하지 않았다면 arrange 구현은 생략되고 저장된 결과가 재생됩니다. 지오메트리는 그대로 반영되고 `LayoutFinished`도 그대로 발생하며, 재계산만 생략됩니다.

---

### 8.2 무효화 흐름

레이아웃 속성이 변경되면(크기, 자식, 파라미터), 무효화가 레이아웃 루트까지 상위 전파되고 다음 프레임 처리를 위해 `LayoutController`에 등록됩니다.

```
View 속성 변경 (크기, 자식, 파라미터)
        │
        ▼
  InvalidateMeasure() / InvalidateArrange()
        │
        ▼  (상위 전파)
  레이아웃 루트 도달
        │
        ▼
  LayoutController에 등록
        │
        ▼  (다음 프레임)
  ProcessLayouts()
        │
        ├── Measure (하향식)
        │
        └── Arrange (하향식)
```

**레이아웃 루트**는 부모가 레이아웃이 아닌 최상위 View입니다. `LayoutController`는 윈도우별로 획득합니다:

```cpp
LayoutController controller = LayoutController::Get(window);
```

> 일반적인 사용에서 `LayoutController`에 대한 명시적 호출은 필요 없습니다. 무효화가 자동으로 재레이아웃을 트리거합니다.

---

### 8.3 레이아웃 결과 캐싱

두 단계는 모두 결과를 캐시하며, 이것이 안정된 레이아웃 pass를 저렴하게 만듭니다. 또한 이 캐시는 measure/arrange 구현이 무엇을 읽어도 되는지를 정의합니다.

#### 8.3.1 Measure 캐시

- measure 캐시는 무조건 동작합니다. opt-out은 없습니다.
- hit이 되려면 다음이 모두 성립해야 합니다. 완료된 측정이 게시한 유효한 항목이 있고, view가 measure-dirty가 아니며, pass가 오염되지 않았고, 유효 스케일이 EXACT하게 일치하며, 두 제약 조건이 허용 오차 내에서 같아야 합니다.
- 유효 스케일은 캐시 KEY의 항입니다. 따라서 스케일 무효화를 놓쳐도 miss로 격하되어 측정이 한 번 다시 수행될 뿐이며, 다른 스케일에서 계산된 크기가 서빙되는 일은 결코 없습니다.
- 그러므로 measure 구현은 다음의 순수 함수여야 합니다. 두 제약 조건, view의 유효 스케일, view의 유효 레이아웃 방향, view 자신의 layout 추적 상태(요청 크기, 패딩, 마진, 최소/최대 범위, layout params, 자식 목록), 그리고 자식들의 측정 크기입니다.
- 그 외에 읽는 것은 구현이 소유합니다. 해당 상태가 바뀔 때 구현이 직접 `InvalidateMeasure()`를 호출해야 합니다.
- 무관한 pass가 도는 것만으로는 오래된 결과가 회복되지 않습니다. 조상이 miss해도 조상은 이 view를 같은 입력으로 다시 측정하므로 이 view는 여전히 hit하며, 형제의 무효화는 위로만 전파되어 이 view에 닿지 않습니다.

#### 8.3.2 Arrange 결과 캐시

- arrange hit은 가지치기(prune)가 아닙니다. 저장된 서브트리는 재생(replay)됩니다. 각 view 자신의 지오메트리가 반영되고, 오른쪽에서 왼쪽 방향의 자식은 미러링되며, `LayoutFinished`도 발생합니다. 생략되는 것은 arrange 구현뿐입니다.
- 노드 단위 hit이 되려면 다음이 모두 성립해야 합니다. 유효한 항목이 있고, 정책이 `ArrangePolicy::ALWAYS`가 아니며, view가 dirty/오염/차단 상태가 아니고, 입력 rect가 EXACT하게 일치하며, 기록된 레이아웃 방향이 현재 방향과 같고, 소비되지 않은 standalone 자식이 없어야 합니다.
- rect 비교는 허용 오차가 아닌 정확 비교입니다.
- 서브트리 hit은 입력 rect 일치를 제외한 위 조건을 arrange 결과를 가진 모든 하위 노드에서 다시 검사합니다. 하위 노드의 KEY 일치는 이 view의 KEY 일치와 정책으로부터 함의됩니다.

#### 8.3.3 ArrangePolicy

- `ArrangePolicy::IF_CHANGED`가 기본값입니다.
- `ArrangePolicy::ALWAYS`는 view에 도달하는 모든 pass에서 arrange 구현을 실행합니다.
- arrange 구현이 layout 무효화가 추적하지 않는 상태를 읽거나, 매 pass마다 외부에서 관찰되는 작업을 수행한다면 ALWAYS를 선택하십시오.
- 선택 지점: `OnArrange()` 재정의에는 `ViewImpl::SetArrangePolicy`, 콜백에는 `View::SetArrangeCallback(callback, ArrangePolicy::ALWAYS)`, layout manager에는 protected `LayoutManager::SetArrangePolicy`를 사용합니다.
- ALWAYS는 pass를 예약하지 않습니다. 상태 setter는 여전히 무효화를 호출해야 합니다.

#### 8.3.4 커스텀 LayoutManager 상태

- `LayoutManager`가 보관한 상태는 두 캐시 KEY 어느 쪽에도 들어가지 않습니다.
- 그러한 상태를 바꾸는 모든 setter는 protected `InvalidateOwnerMeasure()`를 호출해야 하며, 배치만 바뀐다면 `InvalidateOwnerArrange()`를 호출합니다.
- 이들은 정확히 `owner->InvalidateMeasure()` / `owner->InvalidateArrange()`이며, manager가 attach되기 전에 호출해도 안전한 no-op입니다.
- 내장 manager들은 이렇게 배선되어 있습니다.

```cpp
void MyManager::SetGap(float gap)
{
  if(mGap == gap) { return; }  // 같은 값이면 아무것도 예약하지 않습니다
  mGap = gap;
  InvalidateOwnerMeasure();    // 배치만 바뀐다면 InvalidateOwnerArrange()
}
```

#### 8.3.5 무효화 병합

pass가 돌기 전에 반복해서 발생한 무효화는 병합됩니다. 지역(local) 절반은 항상 수행되고, 레이아웃 루트까지 올라가는 상위 탐색은 이미 등록된 대기 항목이 살아 있는 동안 생략됩니다. pass가 실행되는 중에는 병합이 비활성화됩니다. 따라서 한 프레임에 여러 속성 변경을 모아서 적용하면 레이아웃 pass는 한 번만 발생하며, 무효화가 유실되는 일은 없습니다.

#### 8.3.6 전체 계약

전체 계약은 [리포지토리의 레이아웃 가이드](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/docs/layout-structure.md)를 참고하십시오.

---

## 9. 메서드 체이닝

모든 레이아웃 setter는 체이닝 메서드를 통해 유연한 설정을 지원합니다:

```cpp
StackLayout stack = StackLayout::New()
    .Orientation(StackOrientation::VERTICAL)
    .Spacing(10.0f)
    .RequestedWidth(MATCH_PARENT)
    .RequestedHeight(MATCH_PARENT)
    .ViewPadding(Extents(20, 20, 20, 20));

FlexLayout flex = FlexLayout::New()
    .Direction(FlexDirection::ROW)
    .Wrap(FlexWrap::WRAP)
    .JustifyContent(FlexJustify::SPACE_BETWEEN)
    .AlignItems(FlexAlign::CENTER);

GridLayout grid = GridLayout::New()
    .Rows({GridLength::Absolute(60.0f), GridLength::Star(1.0f)})
    .Columns({GridLength::Star(1.0f), GridLength::Star(2.0f)})
    .RowSpacing(10.0f)
    .ColumnSpacing(10.0f);
```

LayoutParams setter도 체이닝을 위해 `*this`를 반환합니다:

```cpp
view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
view.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(2).SetColumnSpan(3));
view.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f).SetFlexShrink(0.0f));
```

---

## 10. 기본값

### StackLayout

| 속성 | 기본값 |
|---|---|
| `Orientation` | `VERTICAL` |
| `Spacing` | `0` |

### StackLayoutParams

| 속성 | 기본값 |
|---|---|
| `Weight` | `0` |
| `Alignment` | `START` |

### FlexLayout

| 속성 | 기본값 |
|---|---|
| `Direction` | `ROW` |
| `Wrap` | `NO_WRAP` |
| `JustifyContent` | `FLEX_START` |
| `AlignItems` | `STRETCH` |
| `AlignContent` | `STRETCH` |

### FlexLayoutParams

| 속성 | 기본값 |
|---|---|
| `FlexGrow` | `0` |
| `FlexShrink` | `1` |
| `FlexBasis` | `WRAP_CONTENT` |
| `AlignSelf` | `AUTO` |

### GridLayoutParams

| 속성 | 기본값 |
|---|---|
| `Row` | `0` |
| `Column` | `0` |
| `RowSpan` | `1` |
| `ColumnSpan` | `1` |
| `HorizontalAlignment` | `FILL` |
| `VerticalAlignment` | `FILL` |

### AbsoluteLayoutParams

| 속성 | 기본값 |
|---|---|
| `Bounds` | `(0, 0, -1, -1)` |
| `Flags` | `NONE` |

---

## 11. 주의사항

- **Layout은 View를 상속합니다.** 모든 View 메서드(`SetRequestedWidth`, `SetViewMargin`, `SetViewPadding`, `Add`, `Remove` 등)를 모든 레이아웃 타입에서 사용할 수 있습니다.

- **View당 하나의 LayoutManager.** 각 레이아웃 서브클래스(StackLayout, FlexLayout 등)는 초기화 시 자체 LayoutManager를 연결합니다. 커스텀 레이아웃은 `LayoutManager`를 서브클래싱하고 `View::AttachLayoutManager()`로 임의의 View에 연결할 수 있습니다.

- **LayoutParams는 값 의미론을 사용합니다.** `SetLayoutParams()`는 독립적인 복사본을 저장하며, `TryGetLayoutParams(out)`으로 얻은 스냅샷의 변경은 `SetLayoutParams()`로 다시 전달해야 반영됩니다.

- **콜백은 LayoutManager를 재정의합니다.** View에 `SetMeasureCallback()` 또는 `SetArrangeCallback()`이 설정되면, 해당 View에서 콜백이 기본 LayoutManager보다 우선합니다.

- **레이아웃 루트가 처리를 주도합니다.** 레이아웃 루트는 부모가 레이아웃이 아닌 View입니다. 무효화는 레이아웃 루트에 도달할 때까지 상위로 전파되며, 루트가 `LayoutController`에 등록됩니다. 컨트롤러는 매 프레임마다 대기 중인 모든 루트를 처리합니다.

- **Measure 결과는 캐시됩니다.** `OnMeasure()`에 전달된 너비 및 높이 제약 조건이 마지막 호출 이후 변경되지 않고, 유효 스케일도 변경되지 않으면 측정을 건너뜁니다. 재계산을 강제하려면 `InvalidateMeasure()`를 호출하세요.

- **Arrange 결과도 캐시됩니다.** 기본 정책인 `ArrangePolicy::IF_CHANGED`에서는 입력 bounds, 유효 레이아웃 방향, 유효 스케일이 변경되지 않으면 `OnArrange()`가 생략되고 저장된 결과가 재생됩니다. 재계산을 강제하려면 `InvalidateArrange()`를 호출하거나, 구현이 매 pass마다 실행되어야 한다면 `ArrangePolicy::ALWAYS`를 선택하세요.

- **`GetSize()`는 배치된 크기를 반환합니다.** `View::GetSize()`는 요청된 크기가 아닌 Arrange 단계 이후의 실제 렌더링된 크기를 반환합니다.

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr))
