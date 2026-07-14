# Layout

dali-ui의 레이아웃 시스템은 **Measure → Arrange** 두 단계로 동작합니다.
레이아웃 재계산이 필요하다고 선언되면 해당 프레임에 `LayoutController`가 루트부터 두 단계를 순서대로 수행합니다.

레이아웃 계산 메커니즘을 자세히 확인하고 싶다면, [여기](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Structures)를 참고하세요.

<br/>

## 크기 지정

모든 View는 `SetRequestedWidth` / `SetRequestedHeight`로 원하는 크기를 지정합니다.

| 값 | 의미 |
|---|---|
| `WRAP_CONTENT` (`-1`) | 콘텐츠(자식) 크기에 맞춤 (기본값) |
| `MATCH_PARENT` (`-2`) | 부모 컨테이너를 가득 채움 |
| `100.0f` 등 양수 | 고정 크기 |

<br/>

## Margin & Padding

Margin은 View의 **바깥 여백**으로, 부모 레이아웃 안에서 이웃 View 또는 컨테이너 경계와의 간격을 만듭니다.
Padding은 View의 **안쪽 여백**으로, View의 배경 영역과 자식 콘텐츠 사이의 공간을 만듭니다.

![layout-margin-padding](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/b0fa6f35-d006-4109-9261-0e3479e52808)

`Extents`의 인자 순서는 **(start, end, top, bottom)** 입니다. `start`/`end`는 레이아웃 방향에 따라 결정되며, LTR에서는 각각 왼쪽/오른쪽입니다.

```cpp
// 인자 순서: Extents(start, end, top, bottom)
view.SetViewMargin(Extents(4_spx, 8_spx, 12_spx, 16_spx));    // start 4, end 8, top 12, bottom 16
view.SetViewPadding(Extents(12_spx, 12_spx, 12_spx, 12_spx)); // 사방 12spx
```

<br/>

## Visibility

```cpp
view.SetVisibility(true);   // 보임
view.SetVisibility(false);  // 숨김 (레이아웃 공간에서 빠짐)
```

불투명도 조절은 `SetOpacity(float)`를 사용합니다.

<br/>

## StackLayout

자식 View들을 한 방향으로 순서대로 쌓습니다.

![layout-stack](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/796f7d76-440e-4efe-9d49-8085b9cc4089)

**언제 사용하면 좋은가**: 버튼 목록, 메뉴 항목, 폼 필드처럼 단순히 세로 또는 가로로 나열할 때.

```cpp
StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
stack.SetSpacing(8_spx);
stack.SetRequestedWidth(MATCH_PARENT);
stack.SetRequestedHeight(WRAP_CONTENT);
stack.AddChildren({
  label,
  button,
  input,
});
```

**Per-child 옵션** (`StackLayoutParams`):

| 옵션 | 설명 |
|---|---|
| `SetWeight(float)` | 남은 공간을 비율대로 분배. weight > 0이면 RequestedSize 무시 |
| `SetAlignment(LayoutAlignment)` | 교차축 정렬 (FILL / START / CENTER / END) |

```cpp
// 남은 공간을 2:1로 분배
viewA.SetLayoutParams(StackLayoutParams::New().SetWeight(2.0f));
viewB.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
```

샘플: [stacklayout](https://github.com/dalihub/dali-ui/tree/devel/samples/stacklayout)

<br/>

## AbsoluteLayout

자식을 명시적인 좌표에 배치합니다.

![layout-absolute](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/7be0b741-8c9c-404b-940d-ffbfeea99f06)

**언제 사용하면 좋은가**: 자유로운 오버레이, 떠있는 UI 요소, 또는 픽셀 단위 정밀 제어가 필요할 때.

```cpp
icon.SetLayoutParams(
  AbsoluteLayoutParams::New()
    .SetX(20_spx).SetY(20_spx)
    .SetWidth(48_spx).SetHeight(48_spx));

badge.SetLayoutParams(
  AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.9f, 0.0f, 0.1f, 0.1f))
    .SetFlags(AbsoluteLayoutFlags::ALL));  // 비율 기반

AbsoluteLayout absolute = AbsoluteLayout::New();
absolute.SetRequestedWidth(MATCH_PARENT);
absolute.SetRequestedHeight(MATCH_PARENT);
absolute.AddChildren({icon, badge});
```

**AbsoluteLayoutFlags**:

| 플래그 | 의미 |
|---|---|
| `NONE` | 픽셀 절대값 |
| `POSITION_PROPORTIONAL` | x, y를 부모 크기 대비 비율(0.0~1.0)로 해석 |
| `SIZE_PROPORTIONAL` | width, height를 부모 크기 대비 비율로 해석 |
| `ALL` | 위치와 크기 모두 비율 |

샘플: [absolutelayout](https://github.com/dalihub/dali-ui/tree/devel/samples/absolutelayout)

<br/>

## FlexLayout

CSS Flexbox 알고리즘을 사용합니다.

![layout-flex](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/f66017e6-2f92-4df9-b60b-124b217a8be4)

**언제 사용하면 좋은가**: 방향 전환, 줄바꿈, 정렬 제어가 필요한 복잡한 1차원 레이아웃.

```cpp
FlexLayout flex = FlexLayout::New();
flex.SetDirection(FlexDirection::ROW);
flex.SetWrap(FlexWrap::WRAP);
flex.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
flex.SetAlignItems(FlexAlign::CENTER);
flex.AddChildren({itemA, itemB, itemC});
```

**Per-child 옵션** (`FlexLayoutParams`):

| 옵션 | 설명 |
|---|---|
| `SetFlexGrow(float)` | 남은 공간을 비율대로 늘림 |
| `SetFlexShrink(float)` | 공간 부족 시 비율대로 줄임 |
| `SetFlexBasis(float)` | grow/shrink 전 기본 크기 (`WRAP_CONTENT`로 auto) |
| `SetAlignSelf(FlexAlign)` | 개별 교차축 정렬 (부모의 AlignItems 오버라이드) |

샘플: [flexlayout](https://github.com/dalihub/dali-ui/tree/devel/samples/flexlayout)

<br/>

## GridLayout

행과 열을 정의하고 자식을 셀에 배치합니다.

![layout-grid](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/cd3ef92e-d819-4760-90b0-6a398f4e84ae)

**언제 사용하면 좋은가**: 앱 그리드, 설정 화면, 대시보드처럼 2차원 격자가 필요할 때.

```cpp
GridLayout grid = GridLayout::New();
grid.SetRowDefinitions({GridLength::Auto(), GridLength::Star(), GridLength::Absolute(100_spx)});
grid.SetColumnDefinitions({GridLength::Star(), GridLength::Star(2.0f)});
grid.SetRowSpacing(4_spx);
grid.SetColumnSpacing(4_spx);
grid.AddChildren({header, sideBar, main, footer});
```

**GridLength 타입**:

| 타입 | 생성 | 의미 |
|---|---|---|
| Absolute | `GridLength::Absolute(100_spx)` | 고정 크기 |
| Star | `GridLength::Star(1.0f)` | 남은 공간 비율 분배 |
| Auto | `GridLength::Auto()` | 자식의 자연 크기에 맞춤 |

**Per-child 옵션** (`GridLayoutParams`):

| 옵션 | 설명 |
|---|---|
| `SetRow(uint32_t)` | 배치할 행 인덱스 (0-based) |
| `SetColumn(uint32_t)` | 배치할 열 인덱스 (0-based) |
| `SetRowSpan(uint32_t)` | 점유할 행 수 |
| `SetColumnSpan(uint32_t)` | 점유할 열 수 |
| `SetHorizontalAlignment(LayoutAlignment)` | 셀 내 수평 정렬 |
| `SetVerticalAlignment(LayoutAlignment)` | 셀 내 수직 정렬 |

```cpp
// 동일한 base params를 복사해서 여러 자식에 사용
auto base = GridLayoutParams::New().SetRowSpan(2).SetColumnSpan(2);
viewA.SetLayoutParams(GridLayoutParams::New(base).SetRow(0).SetColumn(0));
viewB.SetLayoutParams(GridLayoutParams::New(base).SetRow(0).SetColumn(2));
```

> **주의**: `SetLayoutParams()`는 핸들을 그대로 저장합니다. 동일한 params 핸들을 여러 View에 전달하면
> 상태를 공유하게 됩니다. 여러 View에 재사용할 때는 반드시 `New(other)`로 복사하세요.

샘플: [gridlayout](https://github.com/dalihub/dali-ui/tree/devel/samples/gridlayout)

<br/>

## 콜백 등록을 통한 커스텀 레이아웃

`SetMeasureCallback` / `SetArrangeCallback`으로 임의의 레이아웃 로직을 주입할 수 있습니다.

```cpp
// 자식들을 대각선으로 배치하는 커스텀 레이아웃
struct DiagonalLayout
{
  static MeasuredSize OnMeasure(View self, float wConstraint, float hConstraint)
  {
    float totalW = 0, totalH = 0;
    for(uint32_t i = 0; i < self.GetChildViewCount(); ++i)
    {
      auto sz = self.GetChildViewAt(i).Measure(wConstraint - totalW, hConstraint - totalH);
      totalW += sz.width;
      totalH += sz.height;
    }
    return {totalW, totalH};
  }

  static LayoutRect OnArrange(View self, const LayoutRect& bounds)
  {
    float x = bounds.x, y = bounds.y;
    for(uint32_t i = 0; i < self.GetChildViewCount(); ++i)
    {
      auto sz = self.GetChildViewAt(i).GetMeasuredSize();
      self.GetChildViewAt(i).Arrange({x, y, sz.width, sz.height});
      x += sz.width;
      y += sz.height;
    }
    return bounds; // 프레임워크가 x/y/width/height를 적용하는 최종 self bounds
  }
};

Layout root = Layout::New();
root.SetMeasureCallback(LayoutMeasureCallback::New(&DiagonalLayout::OnMeasure));
root.SetArrangeCallback(LayoutArrangeCallback::New(&DiagonalLayout::OnArrange));
```

샘플: [customlayout](https://github.com/dalihub/dali-ui/tree/devel/samples/customlayout)

<br/>

## LayoutManager를 이용한 컴포넌트 레이아웃

`StackLayoutManager`, `GridLayoutManager` 등 public-api에서 제공하는 레이아웃 매니저 모듈을
재사용하거나 상속하여 기존 알고리즘 위에 컴포넌트를 만들 수 있습니다.
컴포넌트는 `View::AttachLayoutManager()`로 매니저를 부착하며, 이후 View가 Measure / Arrange를
매니저로 자동 디스패치합니다(`MeasureCallback` / `ArrangeCallback`이 설정된 경우 매니저보다 우선합니다).

```cpp
class MyButtonImpl : public Dali::Ui::ViewImpl
{
protected:
  void OnInitialize() override
  {
    ViewImpl::OnInitialize();
    // 기존 매니저 재사용; View가 Measure/Arrange를 매니저로 디스패치합니다.
    AttachLayoutManager(Dali::MakeUnique<Dali::Ui::StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
  }
};
```

완전히 커스텀 알고리즘을 구현하려면 `Dali::Ui::LayoutManager`를 상속하여 `Measure()` / `Arrange()`를
오버라이드한 뒤 같은 방식으로 부착합니다
([custom-layout-manager](https://github.com/dalihub/dali-ui/tree/devel/samples/custom-layout-manager) 샘플 참고).

<br/>

## Layout Transition

`LayoutTransition`은 View의 자식들이 레이아웃 결과에 따라 추가, 제거, 이동, 크기 변경될 때 이를 애니메이션으로 연결합니다. 자식 추가 시 ENTER, `RemovePolicy::ANIMATE_EXIT`로 제거 시 EXIT, 기존 자식의 배치 결과가 바뀔 때 CHANGE 슬롯이 동작합니다.

스택에 항목을 삽입하거나, 그리드에서 카드를 제거하거나, 창 크기 변경으로 레이아웃이 재배치될 때처럼 레이아웃 변화가 자연스럽게 이어져야 하는 경우에 사용합니다.

슬롯 동작, spec mode, animator callback, reflow scope, 주의 사항은 [Layout Transition](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Transition-(kr))을 참고하세요.

샘플: [layout-transition](https://github.com/dalihub/dali-ui/tree/devel/samples/layout-transition)

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
