
# Touch & Gesture 동작 방식 및 코드 구현 가이드


## 목차
1. [TouchEvent](#touchevent)
2. [InterceptTouchEvent](#intercepttouchevent)
3. [HoverEvent](#hoverevent)
4. [Gesture Detection by HandleEvent](#gesture-detection-by-handleevent)
5. [Gesture Propagation](#gesture-propagation)
6. [Example 1: 기본 제스처 인식](#example-1-기본-제스처-인식)
7. [Example 2: InterceptTouchEvent를 활용한 제스처 처리](#example-2-intercepttouchevent를-활용한-제스처-처리)

---

## TouchEvent

geometry Z 축으로 hittest 및 이벤트가 전파됩니다.

### 터치 이벤트 수신 방법

View는 `TouchedSignal`을 통해 터치 이벤트를 수신합니다. 콜백에서는 `View`가 아닌 `Actor`를 받기 때문에 View 전용 API를 사용하려면 다운캐스트가 필요합니다.

멤버 함수를 사용하는 방법:

```cpp
class MyController : public ConnectionTracker
{
public:
  void SetupTouchHandler(View view)
  {
    view.TouchedSignal().Connect(this, &MyController::OnTouched);
  }

private:
  bool OnTouched(Actor actor, const TouchEvent& touch)
  {
    // Actor를 View로 다운캐스트
    View view = View::DownCast(actor);

    if(touch.GetState(0) == PointState::DOWN)
    {
      // 터치 다운 처리
      return true;  // consume — 이후 터치 이벤트를 이 View가 계속 수신
    }
    return false;  // 하위 뷰로 이벤트 전파
  }
};
```

람다를 사용하면 간결하게 작성할 수 있습니다:

```cpp
view.TouchedSignal().Connect(&tracker, [](Actor actor, const TouchEvent& touch) -> bool {
  View view = View::DownCast(actor);

  if(touch.GetState(0) == PointState::DOWN)
  {
    // 터치 다운 처리
    return true;  // consumed
  }
  return false;  // 하위 뷰로 이벤트 전파
});
```

> [!NOTE]
> `true`(consume)를 반환하면 이 View에서 이후 터치 이벤트(Motion, Finished)를 계속 수신합니다. `false`를 반환하면 geometry Z축 순서로 다음 뷰에 이벤트가 전파되며, 이 터치 시퀀스의 후속 이벤트를 **받을 수 없습니다**.

<br/>

### 이벤트 전파 방식

아래 그림과 같이 4개의 뷰가 겹쳐 있을 때 터치를 하게 되면 터치된 좌표 기준으로 geometry 방향으로 hittest가 진행됩니다.

```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  ┌─────────────────────────┐  │  │
│  │  │       Red View          │  │  │
│  │  │  ┌─────────────────┐    │  │  │
│  │  │  │   Orange View   │    │  │  │
│  │  │  └─────────────────┘    │  │  │
│  │  └─────────────────────────┘  │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

- "빨간색" 뷰에 TouchEvent가 등록되어 있다면 "빨간색" 뷰가 hit되고 "빨간색" 뷰부터 geometry 하위 방향으로 이벤트가 전파됩니다.
- "빨간색"뷰의 TouchEvent에서 `return false`를 하게 되면 "빨간색" → "노란색" → "파란색" 방향으로 이벤트가 전파됩니다.

### Consume 동작

TouchEvent에서 `return true`는 consume으로 이제 앞으로 발생되는 TouchEvent는 해당 뷰가 이벤트를 받겠다는 의미입니다.

"노란색"뷰의 TouchEvent에서 `return true`를 하게 되면 이제부터 노란색 뷰가 TouchEvent를 consume하겠다는 의미로 이후의 TouchEvent는 노란색 뷰만 받습니다.

### 이벤트 전파 예시

아래와 같은 순서가 됩니다:

```
빨간색 뷰 TouchDown return false
노란색 뷰 TouchDown return true
노란색 뷰 TouchMotion return true
노란색 뷰 TouchMotion return true
노란색 뷰 TouchFinished return true
```

### 터치 이벤트 전파 특징

- 터치 이벤트 전파는 부모-자식 관계와 상관없이 geometry Z축으로 전파됩니다.
- 기본적으로 자신에게서 시작한 터치이벤트만 계속 받을 수 있습니다.
- 이것은 기존 `AllowOnlyOwnTouch`와 `GrabTouchAfterLeave` 옵션이 On되어 있는 것과 동일합니다.
- 새로운 터치에서는 위 두 개의 옵션을 따로 제공하지 않습니다. 디폴트로 On입니다.
- `Leaved`는 없어졌습니다.

### 부모 영역과 터치 이벤트

터치 이벤트 전달시 부모의 영역을 벗어난 위치를 터치하였을 경우는 터치 이벤트가 전달되지 않습니다. (Android와 동일)

#### 예시: Red가 부모이고 Orange는 자식관계일 때

- Orange 터치시 부모의 영역을 벗어난 좌표를 터치했을 경우 터치 이벤트는 전달되지 않습니다.
- Red와 Orange 영역이 겹친 곳을 터치했을 경우 Red, Orange는 intercept 및 touch event를 받을 수 있습니다.
- Red 영역을 벗어난 Orange 영역을 터치했을 경우 Orange는 intercept 및 touch event를 받을 수 없습니다.

```
          Blue
        /      \
   Yellow      Red
                 |
              Orange
```


---

## InterceptTouchEvent

geometry Z 축으로 hittest 및 이벤트 전파됩니다.

### 이벤트 전파 방식

- InterceptTouchEvent는 **부모-자식 관계로 전파**됩니다. (Android와 동일)
- 형제 관계의 ViewA(아래), ViewB(위)가 겹쳐 있을 때 ViewA에서 intercept를 하면 ViewB는 터치 이벤트를 받지 못합니다.

**InterceptTouchEvent의 경우 hit된 View기준으로 부모 → 자식으로 전파되고 TouchEvent는 Z-order순으로 전파됨**


### 이벤트 전파 예시

아래 그림과 같이 4개의 뷰가 겹쳐 있을 때 터치를 하게 되면 터치된 좌표 기준으로 자식 geometry 방향으로 hittest가 진행됩니다.

```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  ┌─────────────────────────┐  │  │
│  │  │       Red View          │  │  │
│  │  │  ┌─────────────────┐    │  │  │
│  │  │  │   Orange View   │    │  │  │
│  │  │  └─────────────────┘    │  │  │
│  │  └─────────────────────────┘  │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

- 터치된 좌표 기준으로 "파란색"뷰에 InterceptTouchEvent가 등록되어 있다면 "파란색"뷰에 InterceptTouchEvent가 호출되고 자식 geometry 상위 방향으로 이벤트 전파 됩니다.
- "파란색"뷰의 InterceptTouchEvent에서 `return false`를 하게되면 "파란색" → "노란색" → "빨간색" 방향으로 이벤트가 전파됩니다.

### Consume 동작

InterceptTouchEvent에서 `return true`는 consume으로 이제 앞으로 발생되는 TouchEvent는 해당 뷰가 이벤트를 받겠다는 의미입니다.

"노란색"뷰의 InterceptTouchEvent에서 `return true`를 하게 되면 노란색 뷰가 터치를 가로챘기 때문에 이후의 TouchEvent는 노란색 뷰만 받습니다.

### 이벤트 전파 예시

아래와 같은 순서가 됩니다:

```
파란색 뷰 InterceptTouchDown return false
노란색 뷰 InterceptTouchDown return true
노란색 뷰 TouchDown return true
노란색 뷰 TouchMotion return true
노란색 뷰 TouchMotion return true
노란색 뷰 TouchFinished return true
```

### Sample: InterceptTouchEvent 활용

Blue와 Yellow에 InterceptTouchEvent를 등록했습니다. 또한 Yellow는 InterceptTouch를 consume합니다.

**동작 과정:**
1. 터치시 Blue와 Yellow가 InterceptTouch를 받습니다.
2. Yellow가 interceptTouch를 consume했기 때문에 앞으로 오는 터치는 Yellow가 받게 됩니다.
3. 그래서 Red에 TouchEvent를 등록했더라도 Yellow가 TouchEvent를 받습니다.

---

## HoverEvent

geometry Z 축으로 이벤트 전파됩니다.

### 이벤트 상태

- 뷰에 진입하게 되면 **Started** 상태가 되고 뷰에서 진출하게 되면 **Leave** 상태가 됩니다.

### 이벤트 전파 예시

아래 그림에서 주황색 뷰에서 오른쪽으로 Hover가 이동한다면:
```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  ┌─────────────────────────┐  │  │
│  │  │       Red View          │  │  │
│  │  │  ┌─────────────────┐    │  │  │
│  │  │  │   Orange View   │    │  │  │
│  │  │  └─────────────────┘    │  │  │
│  │  └─────────────────────────┘  │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

```
주황색 → 빨간색 → 노란색 → 파란색 으로 "Started" 이벤트를 받습니다.
```

- 주황색뷰를 벗어날때 주황색 뷰는 "Leave" 이벤트를 받습니다. 다른 빨간색, 노란색, 파란색 뷰는 그대로 "Motion" 이벤트를 받습니다.
- 빨간색뷰를 벗어날때 빨간색 뷰는 "Leave" 이벤트를 받습니다. 다른 노란색, 파란색 뷰는 그대로 "Motion" 이벤트를 받습니다.

### 역방향 이동

역으로 오른쪽에서 다시 왼쪽으로 Hover가 이동한다면:
- 새로운 뷰에 진입하게 될때 해당뷰는 "Started"를 받게 됩니다.
- 기존 Motion 이벤트를 받고 있던 뷰들은 그대로 Motion을 받습니다.

### Sample: HoverEvent 동작

Yellow, Red, Orange에 HoverEvent를 등록합니다.

**왼쪽에서 오른쪽 방향으로 마우스를 이동:**
1. Yellow에 진입할때 Yellow가 "Down"("Started")를 받습니다.
2. Red에 진입할때 Red는 "Down"을 받습니다. 기존 Yellow는 그대로 Motion을 받습니다.
3. Orange에 진입할때 Orange는 "Down"을 받습니다. 기존 Yellow와 Red는 그대로 Motion을 받습니다.

**오른쪽에서 왼쪽 방향으로 마우스를 이동:**
1. Orange에서 진출할 때 Orange는 "Leave"를 받습니다. 기존 Yellow와 Red는 그대로 Motion을 받습니다.
2. Red에서 진출할 때 Red는 "Leave"를 받습니다. 기존 Yellow는 그대로 Motion을 받습니다.
3. Yellow에서 진출할 때 Yellow는 "Leave"를 받습니다.

### Consume 동작

- Consume을 하게 되면 하위의 뷰는 이벤트를 받지 못합니다.
- 터치와 다르게 Consume했다고 해서 Consume한 뷰가 이후의 Hover이벤트를 받아가는 건 아닙니다.
- 단지 하위의 뷰에게 이벤트를 전파를 막기만 합니다. (Android와 동일)

---

## Gesture Detection by HandleEvent

Gesture는 TouchEvent 안에서 `HandleEvent`하여 수행됩니다.

### 기본 사용법

```csharp
// PanGestureDetector 생성 및 시그널 연결
PanGestureDetector panGestureDetector = PanGestureDetector::New();
panGestureDetector.DetectedSignal().Connect(this, &YourClass::OnPan);

// TouchEvent 콜백에서 HandleEvent 호출
bool YourClass::OnTouched(Actor actor, const TouchEvent& touch)
{
    // true가 되어야 해당 actor에서 계속 터치 이벤트를 받아 제스처를 인식시킬 수 있습니다.
    bool consumed = tapGestureDetector.HandleEvent(actor, touch);
    return consumed;
}


void YourClass::OnPan(Actor actor, const PanGesture& pan)
{
    DALI_LOG_ERROR("OnPan\n");
}
```

### 동작 원리

1. **TouchEvent 수신**: TouchEvent를 받으면 원하는 GestureDetector에 `HandleEvent`를 호출합니다. 인자로 제스처가 인식되는 뷰와 터치이벤트를 넘깁니다.

2. **Touch Consume 필수**: `HandleEvent` 이후 발생하는 touch event는 해당 뷰에서 **consume을 해야** 해당 뷰에서 연속된 touch를 가지고 제스처를 인식할 수 있습니다.

3. **Consume하지 않을 경우**: 만약 `GestureDetector.HandleEvent` 후 `return false`로 TouchEvent를 흘려보낸다면, 해당 뷰는 이후에 오는 TouchEvent를 받지 못하기 때문에 제스처 인식을 할 수 없습니다. 

---

## Gesture Propagation

View의 TouchEvent에서 Gesture를 인식시키려면 해당 View가 Touch를 **consume**해야만 합니다. 그렇기 때문에 제스처를 하위의 뷰로 전파할 수 없습니다. 

### 제스처 인식의 제약사항

- 제스처를 인식하기 위해서는 **연속된 터치이벤트**가 필요합니다.
- 제스처를 인식했는지 여부에 따라 TouchEvent의 consume 여부를 나중에 다시 정할 수 없습니다.
- 특정 View가 Gesture 인식을 위한 `HandleEvent`를 하게 되면 하위의 뷰는 터치를 받지 못하니 제스처 인식을 할 수 없게 됩니다.

---

## Example 1: 기본 제스처 인식

### 시나리오 설명

- Blue, Yellow, Red, Orange 모두 TouchEvent를 등록합니다.
- Red에는 `TapGestureDetector`를, Yellow에는 `LongPressGestureDetector`를 등록합니다.

### 이벤트 흐름

```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  (LongPressGestureDetector)   │  │
│  │  ┌──────────────────────────┐ │  │
│  │  │       Red View           │ │  │
│  │  │  (TapGestureDetector)    │ │  │
│  │  │  ┌─────────────────┐     │ │  │
│  │  │  │   Orange View   │     │ │  │
│  │  │  └─────────────────┘     │ │  │
│  │  └──────────────────────────┘ │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

### 상세 동작

1. 터치를 했을 때 **Orange가 먼저** 터치 이벤트를 받고 consume하지 않았기 때문에 그 다음 **RedView**가 터치이벤트를 받습니다.

2. RedView의 TouchEvent에서 `GestureDetector.HandleEvent()`를 통해 제스처 인식을 시작합니다.

3. 제스처 인식이 되려면 해당 View가 consume되어야 하기 때문에 **RedView에서 TouchEvent는 consume**됩니다.

### RedView 터치이벤트 콜백

```csharp
bool RedTouched(Actor actor, const TouchEvent& touch)
{
    DALI_LOG_ERROR(" ->Red View %d\n", touch.GetState(0));
    return redConsumed || tapGestureDetector.HandleEvent(actor, touch);
}
```

### 결과

- **하위에 있던 YellowView는 터치 이벤트를 받지 못합니다.**

- Orange를 한 번 터치/업하게 되면 RedView가 TapGesture를 받아 `OnTap` 로그가 찍힙니다.

- YellowView 위에서 롱프레스를 하게 되면 LongPressGesture를 받아 `OnLong` 로그가 찍힙니다.

- **RedView 위에서 롱프레스를 하게 되면**: RedView는 TapGesture를 위한 TouchEvent를 consume했기 때문에 YellowView에게는 TouchEvent가 전달되지 못하고 YellowView는 LongPressGestureEvent를 받지 못합니다.

### 핵심 포인트

> 특정 View가 Gesture 인식을 위한 `HandleEvent`를 하게 되면 하위의 뷰는 터치를 받지 못하니 제스처 인식을 할 수 없게 됩니다. Android도 마찬가지이며 따로 제스처 전파를 하지 못합니다.

---

## Example 2: InterceptTouchEvent를 활용한 제스처 처리

### 시나리오

OrangeView는 LongPressGesture를 받고, 아래에 있는 YellowView는 PanGesture를 받기 원할 때 어떻게 할 수 있는가?

- OrangeView를 LongPress한 채로 터치를 움직이면 LongPress는 풀리고 YellowView에서 PanGesture를 받기 원할 때

### 구현 코드

```csharp
class LongPanGestureTest
{
public:
    void Setup()
    {
        // OrangeView TouchEvent 설정
        orangeView.TouchedSignal().Connect(this, &LongPanGestureTest::OnOrangeTouched);

        // PanGestureDetector 생성 및 시그널 연결
        mPanGestureDetector = PanGestureDetector::New();
        mPanGestureDetector.DetectedSignal().Connect(this, &LongPanGestureTest::OnPan);

        // YellowView InterceptTouchEvent 설정
        yellowView.InterceptTouchedSignal().Connect(this, &LongPanGestureTest::OnYellowInterceptTouch);

        // YellowView TouchEvent 설정
        yellowView.TouchedSignal().Connect(this, &LongPanGestureTest::OnYellowTouched);

        mIsDetected = false;
    }

private:
    bool OnOrangeTouched(Actor actor, const TouchEvent& touch)
    {
        DALI_LOG_ERROR(" ->orangeView touch %d\n", touch.GetState(0));
        mLongPressGestureDetector.HandleEvent(actor, touch);
        return true;
    }

    void OnPan(Actor actor, const PanGesture& pan)
    {
        DALI_LOG_ERROR(" ->yellowView OnPan %d %d\n", pan.GetState(), mIsDetected);
        if (pan.GetState() == GestureState::FINISHED || pan.GetState() == GestureState::CANCELLED)
        {
            mIsDetected = false;
            yellowView.SetProperty(Actor::Property::COLOR, Color::YELLOW);
        }
        else
        {
            mIsDetected = true;
            yellowView.SetProperty(Actor::Property::COLOR, Color::YELLOW * 0.7f);
        }
    }

    // yellowView에서 터치를 인터셉트 하고 있다가 Pan 제스처를 인식하게 되면 touch를 consume하도록 합니다.
    // 그럼 이제 앞으로 오게되는 터치는 yellowView가 받게 됩니다.
    bool OnYellowInterceptTouch(Actor actor, const TouchEvent& touch)
    {
        DALI_LOG_ERROR(" ->yellowView intercepted %d!!! %d\n", mIsDetected, touch.GetState(0));
        mPanGestureDetector.HandleEvent(actor, touch);
        return mIsDetected;
    }

    bool OnYellowTouched(Actor actor, const TouchEvent& touch)
    {
        DALI_LOG_ERROR(" ->yellowView TouchEvent!!! %d\n", touch.GetState(0));
        mPanGestureDetector.HandleEvent(actor, touch);
        return true;
    }

private:
    Actor               orangeView;
    Actor               yellowView;
    LongPressGestureDetector mLongPressGestureDetector;
    PanGestureDetector  mPanGestureDetector;
    bool                mIsDetected;
};
```

### 동작 과정

```
┌─────────────────────────────────────────────────────────────────┐
│  1. OrangeView 롱프레스 → LongPressGesture 발생                  │
│                                                                 │
│  2. 터치 이동 시작 → YellowView의 InterceptTouchEvent 모니터링    │
│                                                                 │
│  3. 일정 이상 이동 → PanGesture 인식                             │
│     - YellowView.InterceptTouchEvent return true (consume)      │
│     - OrangeView은 Interrupted 수신                             │
│     - LongPressGesture 종료                                     │
│                                                                 │
│  4. 이후 터치 → YellowView에서 PanGesture 수신                   │
└─────────────────────────────────────────────────────────────────┘
```

### 상세 설명

1. **OrangeView를 롱프레스하면**: LongPressGesture가 발생합니다.

2. **이 상태에서 터치를 움직이면**: 일정 이상 움직이면 YellowView에서 `InterceptTouchEvent`를 consume시키게 되고, OrangeView는 `Interrupted`를 받게 되며 동시에 LongPressGesture가 끝나게 됩니다.

3. **YellowView는 이제 PanGesture를 받게 됩니다.**

### InterceptTouchEvent의 역할

`InterceptTouchEvent`는 상위 View가 하위 View로 전달되는 터치 이벤트를 가로채는 메커니즘입니다:

- **return false**: 터치 이벤트를 하위 View로 전달 (기본 동작)
- **return true**: 터치 이벤트를 인터셉트하여 상위 View에서 처리

이를 통해 복잡한 제스처 시나리오에서 유연하게 터치 이벤트를 제어할 수 있습니다.

---

## 요약

| 상황 | 동작 |
|------|------|
| TouchEvent consume | 해당 View에서 계속 터치 이벤트 수신 |
| TouchEvent consume 안 함 | 이후 터치 이벤트 수신 불가, 제스처 인식 불가 |
| 상위 View에서 제스처 인식 | 하위 View로 터치 전파 불가 |
| InterceptTouchEvent 사용 | 상위 View에서 터치 인터셉트 후 제스처 전환 가능 |

---

## 참고 사항
