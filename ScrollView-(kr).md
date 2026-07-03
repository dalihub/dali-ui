# ScrollView 사용 가이드

`ScrollView`는 뷰포트보다 큰 콘텐츠를 표시할 수 있는 스크롤 가능한 컨테이너입니다. 팬 제스처 기반의 스크롤, 플링(관성) 스크롤, 스크롤 방향 설정, 스크롤바 가시성 제어를 지원합니다.

---

## 목차

1. [기본 설정](#1-기본-설정)
2. [콘텐츠 추가 및 레이아웃](#2-콘텐츠-추가-및-레이아웃)
3. [스크롤 방향](#3-스크롤-방향)
4. [주요 프로퍼티 및 메서드](#4-주요-프로퍼티-및-메서드)
   - [스크롤 위치](#41-스크롤-위치)
   - [플링 동작](#42-플링-동작)
   - [오버스크롤 모드](#43-오버스크롤-모드)
   - [EdgeEffect](#44-edgeeffect)
   - [스크롤바 가시성](#45-스크롤바-가시성)
   - [스크롤 상태 조회](#46-스크롤-상태-조회)
   - [Focus 및 Key 스크롤](#47-focus-및-key-스크롤)
5. [프로그래밍 방식 스크롤](#5-프로그래밍-방식-스크롤)
6. [이벤트 (시그널)](#6-이벤트-시그널)
7. [설정 방식](#7-설정-방식)
8. [기본값 정리](#8-기본값-정리)
9. [문제 해결](#9-문제-해결)
10. [주의 사항](#10-주의-사항)

---

## 1. 기본 설정

정적 팩토리 메서드 `New()`로 `ScrollView`를 생성하고 뷰포트 크기를 설정합니다. 스크롤 API를 사용하기 전에 반드시 콘텐츠 뷰를 먼저 설정해야 합니다.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::Ui;

// 400x300 뷰포트를 가진 ScrollView 생성
ScrollView scrollView = ScrollView::New();
scrollView.SetRequestedWidth(400.0f);
scrollView.SetRequestedHeight(300.0f);

// 뷰포트보다 큰 콘텐츠 생성
View content = View::New();
content.SetRequestedWidth(400.0f);
content.SetRequestedHeight(2000.0f);

// 콘텐츠 설정 — 스크롤 API 호출 전에 반드시 설정해야 함
scrollView.SetContent(content);

// 씬에 추가
window.Add(scrollView);
```

> **중요:** `SetScrollPosition()`, `ScrollTo()`, `ScrollToX()`, `ScrollToY()` 등의 스크롤 API는 `SetContent()` 호출 이후에만 사용할 수 있습니다. 콘텐츠 없이 스크롤 API를 호출하면 크래시가 발생합니다.

---

## 2. 콘텐츠 추가 및 레이아웃

`ScrollView`는 **하나의 콘텐츠 뷰**만 가질 수 있습니다. 콘텐츠 뷰가 스크롤 가능한 캔버스 역할을 하며, 그 내부에 원하는 뷰 계층 구조를 구성할 수 있습니다.

### 콘텐츠 설정

```cpp
View content = View::New();
content.SetRequestedWidth(800.0f);
content.SetRequestedHeight(800.0f);

scrollView.SetContent(content);
```

### 콘텐츠 조회

```cpp
View current = scrollView.GetContent();
if(current)
{
    // 콘텐츠 뷰 사용
}
```

### 콘텐츠 내부에 중첩 레이아웃 구성

콘텐츠 뷰 내부에 레이아웃을 사용하여 자식 뷰를 배치할 수 있습니다:

```cpp
View content = View::New();
content.SetRequestedWidth(400.0f);
content.SetRequestedHeight(3000.0f);

StackLayout layout = StackLayout::New();
layout.SetDirection(StackLayout::Direction::Vertical);
layout.SetSpacing(8.0f);
content.SetLayout(layout);

for(int i = 0; i < 20; ++i)
{
    View item = View::New();
    item.SetRequestedWidth(400.0f);
    item.SetRequestedHeight(140.0f);
    content.Add(item);
}

scrollView.SetContent(content);
```

---

## 3. 스크롤 방향

스크롤 방향은 사용자가 팬 제스처를 할 수 있는 축과 스크롤 API가 동작하는 방향을 결정합니다.

```cpp
// 세로 스크롤만 허용 (기본값)
scrollView.SetScrollDirection(ScrollDirection::Vertical);

// 가로 스크롤만 허용
scrollView.SetScrollDirection(ScrollDirection::Horizontal);

// 가로 및 세로 모두 허용
scrollView.SetScrollDirection(ScrollDirection::Both);

// 현재 방향 조회
ScrollDirection dir = scrollView.GetScrollDirection();
```

| 값 | 동작 |
|---|---|
| `ScrollDirection::Vertical` | Y축 방향으로만 팬 및 플링. 기본값. |
| `ScrollDirection::Horizontal` | X축 방향으로만 팬 및 플링. |
| `ScrollDirection::Both` | X, Y 양축 2D 스크롤. |

`Vertical` 또는 `Horizontal` 설정 시, 반대 축의 제스처는 내부적으로 억제됩니다.

---

## 4. 주요 프로퍼티 및 메서드

### 4.1 스크롤 위치

스크롤 위치는 콘텐츠가 원점에서 얼마나 이동했는지를 픽셀 단위로 나타냅니다. `(0, 0)`은 콘텐츠의 시작(좌상단)입니다.

```cpp
// 현재 스크롤 위치 조회
Vector2 pos = scrollView.GetScrollPosition();

// 스크롤 위치를 즉시 설정 (애니메이션 없음)
// 콘텐츠가 설정된 후에만 호출 가능
scrollView.SetScrollPosition(Vector2(0.0f, 500.0f));
```

> `SetScrollPosition()`은 `ScrollTo(position, false)`와 동일하게 동작합니다. 애니메이션 없이 즉시 적용됩니다.

---

### 4.2 플링 동작

플링(관성) 스크롤은 사용자가 속도를 가지고 팬 제스처를 끝낼 때 발생합니다. 아래 프로퍼티로 플링의 물리적 특성을 조정할 수 있습니다.

#### MaxFlingDistance — 최대 플링 거리

한 번의 플링 제스처에서 콘텐츠가 이동할 수 있는 최대 거리입니다.

```cpp
// 기본값: 6000.0f 픽셀
scrollView.SetMaxFlingDistance(3000.0f);

float dist = scrollView.GetMaxFlingDistance();
```

#### MinimumFlingDuration / MaximumFlingDuration — 플링 애니메이션 지속 시간

플링 애니메이션 지속 시간은 이동 거리에 비례하며, 이 두 값(밀리초) 사이로 제한됩니다.

```cpp
// 기본값: 최소 1000ms, 최대 2000ms
scrollView.SetMinimumFlingDuration(500);
scrollView.SetMaximumFlingDuration(2500);

int minDur = scrollView.GetMinimumFlingDuration();
int maxDur = scrollView.GetMaximumFlingDuration();
```

#### FlingSensitivity — 플링 감도

제스처 속도를 플링 거리로 변환할 때 적용되는 배수입니다. 값이 클수록 같은 속도에서 더 멀리 이동합니다.

```cpp
// 기본값: 1.0f
scrollView.SetFlingSensitivity(1.5f);

float sensitivity = scrollView.GetFlingSensitivity();
```

#### DecelerationRate — 감속 비율

플링 애니메이션의 감속을 제어합니다. 속도를 거리로 변환하는 공식에 사용되며, `1.0`에 가까울수록 감속이 느려져 더 오래 이동합니다.

```cpp
// 기본값: 0.998f
scrollView.SetDecelerationRate(0.995f);

float rate = scrollView.GetDecelerationRate();
```

---

### 4.3 오버스크롤 모드

콘텐츠를 경계 너머로 스크롤할 수 있는지(고무줄 효과) 제어합니다.

```cpp
// 기본값: ContentScrolls — 콘텐츠가 뷰포트보다 클 때만 오버스크롤 허용
scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);

// 오버스크롤 허용 안 함
scrollView.SetOverScrollMode(OverScrollMode::Never);

// 항상 오버스크롤 허용
scrollView.SetOverScrollMode(OverScrollMode::Always);

OverScrollMode mode = scrollView.GetOverScrollMode();
```

| 값 | 동작 |
|---|---|
| `OverScrollMode::Never` | 경계에서 콘텐츠가 정확히 멈춥니다. |
| `OverScrollMode::Always` | 항상 오버스크롤이 허용됩니다. |
| `OverScrollMode::ContentScrolls` | 콘텐츠가 뷰포트보다 클 때만 허용됩니다. 기본값. |

---

### 4.4 EdgeEffect

`ScrollView`는 콘텐츠가 스크롤 경계를 넘어 드래그되거나 플링될 때 `EdgeEffect`를 사용해 피드백을 줄 수 있습니다.

기본 제공되는 `BounceEdgeEffect`는 콘텐츠를 경계 바깥으로 살짝 이동시킨 뒤 다시 원래 위치로 되돌리는 bounce 효과를 제공합니다.

```cpp
#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect.h>

// 세로 ScrollView: start = 상단, end = 하단
BounceEdgeEffect startEffect = BounceEdgeEffect::New(ScrollDirection::Vertical);
startEffect.SetPullResistance(0.35f);
startEffect.SetBounceDuration(0.35f);

BounceEdgeEffect endEffect = BounceEdgeEffect::New(ScrollDirection::Vertical);
endEffect.SetPullResistance(0.35f);
endEffect.SetBounceDuration(0.35f);

scrollView.SetStartEdgeEffect(startEffect);
scrollView.SetEndEdgeEffect(endEffect);
scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
```

가로 스크롤에서는 `ScrollDirection::Horizontal`로 effect를 생성합니다.

```cpp
BounceEdgeEffect leftEffect = BounceEdgeEffect::New(ScrollDirection::Horizontal);
BounceEdgeEffect rightEffect = BounceEdgeEffect::New(ScrollDirection::Horizontal);

scrollView.SetStartEdgeEffect(leftEffect);
scrollView.SetEndEdgeEffect(rightEffect);
```

EdgeEffect를 제거하려면 초기화되지 않은 `EdgeEffect` handle을 전달합니다.

```cpp
scrollView.SetStartEdgeEffect(EdgeEffect());
scrollView.SetEndEdgeEffect(EdgeEffect());
```

| 메서드 | 의미 |
|---|---|
| `SetStartEdgeEffect(effect)` | 시작 경계 effect를 설정합니다. 세로 스크롤에서는 상단, 가로 스크롤에서는 좌측입니다. |
| `SetEndEdgeEffect(effect)` | 끝 경계 effect를 설정합니다. 세로 스크롤에서는 하단, 가로 스크롤에서는 우측입니다. |
| `GetStartEdgeEffect()` | 현재 시작 경계 effect를 반환합니다. |
| `GetEndEdgeEffect()` | 현재 끝 경계 effect를 반환합니다. |

> 콘텐츠가 설정되어 있으면 `ScrollView`가 effect의 source를 콘텐츠 뷰로 설정합니다. 일반적인 사용에서는 effect를 생성해 `ScrollView`에 지정하면 됩니다.
>
> `OverScrollMode`는 경계 밖으로 스크롤을 허용할지 결정하고, `EdgeEffect`는 그 경계에서 표시할 시각 피드백을 결정합니다. `OverScrollMode::Never`가 설정되어 있으면 edge effect를 지정해도 경계 피드백은 발생하지 않는 것이 정상입니다.

---

### 4.5 스크롤바 가시성

세로 및 가로 스크롤바가 표시되는 시점을 제어합니다.

```cpp
// 세로 스크롤바
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);    // 기본값
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);

// 가로 스크롤바
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);  // 기본값
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Always);
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);

ScrollBarVisibility vVis = scrollView.GetVerticalScrollBarVisibility();
ScrollBarVisibility hVis = scrollView.GetHorizontalScrollBarVisibility();
```

| 값 | 동작 |
|---|---|
| `ScrollBarVisibility::Auto` | 스크롤 중에만 표시됩니다. 기본값. |
| `ScrollBarVisibility::Always` | 항상 표시됩니다. |
| `ScrollBarVisibility::Never` | 표시되지 않습니다. |

---

### 4.6 스크롤 상태 조회

```cpp
bool scrolling = scrollView.IsScrolling();
```

`ScrollStartedSignal`과 `ScrollFinishedSignal` 사이, 즉 드래그 및 플링 애니메이션이 진행 중인 동안 `true`를 반환합니다.

---

### 4.7 Focus 및 Key 스크롤

`ScrollView`는 키보드나 리모컨 기반 포커스 이동 중 focused content가 뷰포트 안에 보이도록 자동 스크롤할 수 있습니다.

기본 동작 원칙은 다음과 같습니다.

- 콘텐츠 뷰의 자손이 포커스를 얻으면 `ScrollView`가 해당 child가 보이도록 자동 스크롤할 수 있습니다.
- 포커스된 child를 뷰포트 안의 어느 위치로 보낼지는 `SetFocusScrollToPosition()`으로 정합니다.
- 포커스된 item이 뷰포트 경계에 딱 붙지 않도록 `SetFocusScrollPeek()`로 작은 추가 offset을 줄 수 있습니다.
- 필요한 경우 key scroll을 켜서, 멀리 있는 item으로 바로 포커스를 넘기기 전에 일정 거리씩 먼저 스크롤하도록 만들 수 있습니다.

```cpp
// 기본값은 true입니다. 앱이 직접 포커스 스크롤을 관리하려면 false로 설정합니다.
scrollView.SetScrollOnFocus(true);

// 기본값은 MakeVisible입니다. Start, Center, End도 사용할 수 있습니다.
scrollView.SetFocusScrollToPosition(ScrollToPosition::MakeVisible);

// MakeVisible이 경계로 스크롤할 때 24px의 peek offset을 추가합니다.
scrollView.SetFocusScrollPeek(24.0f);
```

`SetFocusScrollPeek()`는 `ScrollToPosition::MakeVisible`에서만 적용됩니다. 포커스된 item이 뷰포트 밖에 있을 때 `ScrollView`는 먼저 item이 보이는 가장 가까운 경계를 선택하고, 이동 방향으로 peek 거리만큼 조금 더 스크롤합니다. 최종 위치는 항상 유효한 스크롤 범위 안으로 clamp됩니다.

Key 기반 단계 스크롤은 다음과 같이 설정합니다.

```cpp
scrollView.SetKeyScrollEnabled(true);
scrollView.SetKeyScrollStep(200.0f);
```

Key scroll이 켜져 있을 때 방향키 이동 원칙은 다음과 같습니다.

- 다음 focusable child가 충분히 가까우면 그 child로 포커스가 이동합니다.
- 다음 focusable child가 key scroll step보다 멀면 `ScrollView`가 한 step만 스크롤하고 현재 포커스를 유지합니다.
- `PAGE_UP` / `PAGE_DOWN`은 뷰포트 크기만큼 스크롤한 뒤, 목적지 뷰포트에서 가장 적절한 item에 포커스를 줍니다.
- 스크롤 경계에 도달했고 다음 item이 없으면 포커스는 `ScrollView` 밖으로 이동할 수 있습니다. EdgeEffect가 설정되어 있으면 경계 피드백이 발생합니다.

---

## 5. 프로그래밍 방식 스크롤

모든 스크롤 메서드는 선택적 `animation` 파라미터(기본값 `true`)를 받습니다. `false`로 설정하면 위치가 즉시 적용됩니다.

### 특정 위치로 스크롤

```cpp
// 애니메이션 적용 (기본값)
scrollView.ScrollTo(Vector2(0.0f, 300.0f));

// 즉시 적용 (애니메이션 없음)
scrollView.ScrollTo(Vector2(0.0f, 300.0f), false);
```

### 단일 축으로 스크롤

```cpp
// X 위치 = 200으로 스크롤 (가로)
scrollView.ScrollToX(200.0f);
scrollView.ScrollToX(200.0f, false);  // 즉시 적용

// Y 위치 = 500으로 스크롤 (세로)
scrollView.ScrollToY(500.0f);
scrollView.ScrollToY(500.0f, false);  // 즉시 적용
```

### 자식 뷰가 보이도록 스크롤

`ScrollTo(View child, ...)`는 지정한 자식 뷰가 뷰포트 내에 들어올 때까지 최소한의 거리만 스크롤합니다.

```cpp
// 애니메이션으로 childView가 보이도록 스크롤 (기본값)
scrollView.ScrollTo(childView);

// 즉시 적용
scrollView.ScrollTo(childView, false);
```

`ScrollToPosition` 파라미터로 자식 뷰가 뷰포트 내에서 정렬되는 위치를 지정할 수 있습니다:

```cpp
// 자식 뷰가 뷰포트의 시작(상단/좌측)에 정렬되도록 스크롤
scrollView.ScrollTo(childView, true, ScrollToPosition::Start);

// 자식 뷰가 뷰포트 가운데에 위치하도록 스크롤
scrollView.ScrollTo(childView, true, ScrollToPosition::Center);

// 자식 뷰가 뷰포트의 끝(하단/우측)에 정렬되도록 스크롤
scrollView.ScrollTo(childView, true, ScrollToPosition::End);

// 자식 뷰가 보일 정도로만 최소한 스크롤 (기본값)
scrollView.ScrollTo(childView, true, ScrollToPosition::MakeVisible);
```

| `ScrollToPosition` | 동작 |
|---|---|
| `MakeVisible` | 자식 뷰가 보일 정도로만 최소한 스크롤. 기본값. |
| `Start` | 자식 뷰가 뷰포트의 시작(상단/좌측)에 정렬. |
| `Center` | 자식 뷰가 뷰포트 중앙에 위치. |
| `End` | 자식 뷰가 뷰포트의 끝(하단/우측)에 정렬. |

> **참고:** `ScrollTo(View child, ...)`는 `child`가 콘텐츠 뷰의 자손이어야 합니다.

---

## 6. 이벤트 (시그널)

`ScrollView`는 스크롤과 드래그 생명 주기에 관한 6개의 시그널을 제공합니다. 콜백 클래스는 반드시 `ConnectionTracker`를 상속해야 합니다.

### 시그널 종류

| 시그널 | 시그니처 | 발생 시점 |
|---|---|---|
| `ScrollStartedSignal` | `void(ScrollView)` | 스크롤 시작 (드래그 또는 플링 시작) |
| `ScrollingSignal` | `void(ScrollView)` | 스크롤 중 매 프레임 |
| `ScrollFinishedSignal` | `void(ScrollView)` | 스크롤 종료 (드래그 해제 또는 플링 완료) |
| `DragStartedSignal` | `void(ScrollView)` | 팬 임계값을 넘어 드래그 시작 |
| `DraggingSignal` | `void(ScrollView, float deltaX, float deltaY)` | 드래그 중 매 팬 제스처 업데이트 |
| `DragFinishedSignal` | `void(ScrollView)` | 팬 제스처 해제 |

### 시그널 연결 방법

콜백 클래스는 반드시 `ConnectionTracker`를 상속해야 합니다:

```cpp
class MyScrollHandler : public Dali::ConnectionTracker
{
public:
    void OnScrollStarted(ScrollView scrollView)
    {
        // 스크롤 시작 시 초기화 작업
        // scrollView.GetScrollPosition()으로 시작 위치 확인 가능
    }

    void OnScrolling(ScrollView scrollView)
    {
        Vector2 pos = scrollView.GetScrollPosition();
        // 현재 스크롤 위치에 따라 UI 업데이트
    }

    void OnScrollFinished(ScrollView scrollView)
    {
        // 스크롤이 완전히 멈춘 후 처리
    }

    void OnDragStarted(ScrollView scrollView)
    {
        // 사용자가 드래그를 시작함
    }

    void OnDragging(ScrollView scrollView, float deltaX, float deltaY)
    {
        // deltaX, deltaY: 이전 이벤트 이후 이동한 픽셀
        // 패럴랙스 효과나 헤더 접기 애니메이션에 활용
    }

    void OnDragFinished(ScrollView scrollView)
    {
        // 사용자가 손을 뗌. 이후 플링 애니메이션이 시작될 수 있음
    }
};
```

```cpp
MyScrollHandler handler;

scrollView.ScrollStartedSignal().Connect(&handler, &MyScrollHandler::OnScrollStarted);
scrollView.ScrollingSignal().Connect(&handler, &MyScrollHandler::OnScrolling);
scrollView.ScrollFinishedSignal().Connect(&handler, &MyScrollHandler::OnScrollFinished);
scrollView.DragStartedSignal().Connect(&handler, &MyScrollHandler::OnDragStarted);
scrollView.DraggingSignal().Connect(&handler, &MyScrollHandler::OnDragging);
scrollView.DragFinishedSignal().Connect(&handler, &MyScrollHandler::OnDragFinished);
```

### 시그널 발생 순서

```
사용자가 화면 터치
        │
        ▼
  [팬 임계값: 5px 초과]
        │
        ▼
  DragStartedSignal ──► ScrollStartedSignal
        │
        ▼  (매 프레임)
  DraggingSignal ──────► ScrollingSignal
        │
        ▼
  사용자가 손을 뗌
        │
        ▼
  DragFinishedSignal
        │
        ├─ 충분한 속도?
        │         Yes ──► 플링 애니메이션 ──► ScrollingSignal (매 프레임)
        │                                           │
        │                                           ▼
        │                                   ScrollFinishedSignal
        │         No  ──► ScrollFinishedSignal
```

> `ScrollStartedSignal`은 한 번의 스크롤 세션에서 딱 한 번만 발생합니다. `ScrollFinishedSignal`은 드래그와 플링 애니메이션이 모두 끝난 후에 발생합니다.

### 실용 예제: 스크롤에 따라 헤더 투명화

```cpp
void OnDragging(ScrollView scrollView, float deltaX, float deltaY)
{
    float scrollY = scrollView.GetScrollPosition().y;

    // 100px 이상 스크롤 시 헤더를 점점 숨김
    float headerOpacity = std::max(0.0f, 1.0f - scrollY / 100.0f);
    mHeader.SetOpacity(headerOpacity);
}
```

### 실용 예제: 맨 위로 스크롤 버튼

```cpp
void OnScrollFinished(ScrollView scrollView)
{
    // 최상단이 아닐 때만 버튼 표시
    bool atTop = scrollView.GetScrollPosition().y < 10.0f;
    mScrollToTopButton.SetVisible(!atTop);
}

void OnButtonClicked()
{
    mScrollView.ScrollTo(Vector2(0.0f, 0.0f));  // 최상단으로 애니메이션 스크롤
}
```

---

## 7. 설정 방식

`ScrollView` setter는 메서드 체이닝을 사용하지 않습니다. 일반적인 순차 setter 호출 방식으로 handle을 설정합니다.

```cpp
ScrollView scrollView = ScrollView::New();

scrollView.SetScrollDirection(ScrollDirection::Vertical);
scrollView.SetMaxFlingDistance(4000.0f);
scrollView.SetMinimumFlingDuration(800);
scrollView.SetMaximumFlingDuration(2000);
scrollView.SetFlingSensitivity(1.2f);
scrollView.SetDecelerationRate(0.997f);
scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
scrollView.SetContent(content);
```

이 방식은 `View`, `ScrollView`처럼 상속 및 확장 가능성이 있는 handle class에서 권장되는 설정 스타일입니다.

---

## 8. 기본값 정리

| 프로퍼티 | 기본값 |
|---|---|
| `ScrollDirection` | `Vertical` |
| `MaxFlingDistance` | `6000.0f` px |
| `MinimumFlingDuration` | `1000` ms |
| `MaximumFlingDuration` | `2000` ms |
| `FlingSensitivity` | `1.0f` |
| `DecelerationRate` | `0.998f` |
| `OverScrollMode` | `ContentScrolls` |
| `VerticalScrollBarVisibility` | `Auto` |
| `HorizontalScrollBarVisibility` | `Auto` |
| 팬 임계값 | `5.0f` px (내부 고정값, 변경 불가) |

---

## 9. 문제 해결

| 증상 | 확인할 내용 |
|---|---|
| 콘텐츠가 스크롤되지 않음 | `SetContent()`가 호출되었는지, 콘텐츠 크기가 뷰포트보다 큰지, `SetScrollDirection()`이 원하는 축의 이동을 허용하는지 확인합니다. |
| `ScrollTo()`를 호출해도 `(0, 0)`에 머무름 | 아직 layout이 완료되지 않았거나 콘텐츠가 뷰포트보다 크지 않을 수 있습니다. 콘텐츠와 뷰포트 크기가 확정된 뒤 스크롤 API를 호출하세요. |
| 스크롤바가 보이지 않음 | `SetVerticalScrollBarVisibility()` / `SetHorizontalScrollBarVisibility()` 설정, 콘텐츠 크기, 뷰포트 크기, 해당 축에서 실제 스크롤 가능 여부를 확인합니다. |
| EdgeEffect 피드백이 보이지 않음 | `SetStartEdgeEffect()` / `SetEndEdgeEffect()`가 설정되었는지, `OverScrollMode`가 `Never`가 아닌지, 사용자가 실제 경계 방향으로 드래그 또는 플링하고 있는지 확인합니다. |
| 포커스된 child가 보이도록 스크롤되지 않음 | `SetScrollOnFocus(true)` 설정, focused view가 콘텐츠 뷰의 자손인지, child가 keyboard focusable인지 확인합니다. |
| 방향키 이동이 너무 멀리 있는 item으로 바로 넘어감 | `SetKeyScrollEnabled(true)`를 켜고 `SetKeyScrollStep()` 값을 조정합니다. |

---

## 10. 주의 사항

- **콘텐츠를 먼저 설정해야 합니다.** `SetContent()` 호출 전에 `ScrollTo`, `ScrollToX`, `ScrollToY`, `SetScrollPosition`을 호출하면 내부에서 콘텐츠 액터의 프로퍼티에 직접 접근하므로 크래시가 발생합니다.

- **콘텐츠 뷰는 하나만 설정 가능합니다.** 목록 같은 구조를 표시하려면 콘텐츠 뷰 내부에 레이아웃을 사용하여 구성하세요.

- **뷰포트 크기가 스크롤에 영향을 줍니다.** 스크롤 가능 영역은 `콘텐츠 크기 - 뷰포트 크기`로 계산됩니다. ScrollView가 아직 레이아웃되지 않은 상태(예: 스테이지에 추가되기 전)에서는 뷰포트 크기가 `0`으로 처리되어 모든 스크롤 위치가 `(0, 0)`으로 클램핑될 수 있습니다.

- **스크롤바 렌더링은 플랫폼에 의존합니다.** `ScrollBarVisibility` 프로퍼티는 설정값을 저장하지만, 실제 스크롤바 렌더링은 플랫폼의 테마와 스크롤바 액터의 구현에 따라 달라집니다. 실제 동작은 대상 환경에서 확인하세요.

- **Interactive child와 drag interception.** 버튼, 슬라이더 같은 interactive child는 tap/click 입력을 받을 수 있습니다. 다만 pointer 이동이 pan threshold를 넘으면 `ScrollView`가 제스처를 가로채 스크롤로 전환할 수 있습니다. 자식 뷰의 tap과 drag 동작이 시각적으로 구분되도록 설계하는 것이 좋습니다.

- **`IsScrolling()`은 드래그와 플링을 모두 포함합니다.** `ScrollStartedSignal` 발생 시 `true`로 설정되고 `ScrollFinishedSignal` 발생 시 `false`로 초기화됩니다. 따라서 사용자가 손을 뗀 후 플링 애니메이션이 진행되는 동안에도 `true`를 반환합니다.

- **`DragFinishedSignal`은 `ScrollFinishedSignal`보다 먼저 발생합니다.** 사용자가 손을 떼면 `DragFinishedSignal`이 먼저 발생합니다. 이후 플링 애니메이션이 이어지는 경우, `ScrollFinishedSignal`은 애니메이션이 완전히 끝난 후에 발생합니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
