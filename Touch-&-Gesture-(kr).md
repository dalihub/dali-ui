
# Touch & Gesture 동작 방식 및 코드 구현 가이드


## 목차
1. [문서 범위](#문서-범위)
2. [TouchEvent](#touchevent)
3. [InterceptTouchEvent](#intercepttouchevent)
4. [HoverEvent](#hoverevent)
5. [Gesture Detection by HandleEvent](#gesture-detection-by-handleevent)
6. [Gesture Propagation](#gesture-propagation)
7. [Example 1: 기본 제스처 인식](#example-1-기본-제스처-인식)
8. [Example 2: InterceptTouchEvent를 활용한 제스처 처리](#example-2-intercepttouchevent를-활용한-제스처-처리)

---

## 문서 범위

이 문서는 DALi UI가 application 생성 시 기본으로 활성화하는 **GEOMETRY touch propagation**을 기준으로 설명합니다.
`GeometryHittestEnabled`를 `false`로 변경하면 primary hit Actor에서 부모 방향으로 전달하는 PARENT 방식이 사용되며,
아래의 coordinate candidate 및 late consume 동작과 다릅니다. 이 설정은 application 시작 시 선택하고 input event
처리가 시작된 뒤에는 변경하지 않습니다.

이 문서에서 사용하는 용어는 다음과 같습니다.

- **device**: `TouchEvent`의 point를 구분하는 device ID. 일반적으로 한 pointer의 수명을 식별합니다.
- **initial routing group**: 최초 DOWN에서 hit된 Actor의 ID. 같은 ID로 시작한 device가 같은 stream에 합류합니다.
- **stream**: 같은 initial routing group에 속한 하나 이상의 device와 candidate, recipient, owner 상태를 함께
  관리하는 TouchEvent 전달 단위
- **coordinate candidate**: 최초 DOWN 좌표의 geometry hit-test로 정해지는 front-to-back TouchEvent 후보
- **owner**: TouchEvent를 처음 consume하여 stream 종료 또는 조상의 intercept 전까지 전달을 전담하는 Actor
- **recipient**: 해당 touch stream의 TouchEvent callback을 실제로 한 번 이상 받은 Actor
- **interceptor**: ancestor path의 InterceptTouchEvent callback에서 `true`를 반환하여 기존 전달 범위를 제한한 Actor
- **ancestor path**: target의 root부터 target 방향으로 이어지는 실제 부모-자식 경로

device와 stream의 관계는 다음과 같습니다.

```text
device 4 ─┐
          ├─ 같은 initial hit Actor ID ──> Stream 1
device 7 ─┘

device 9 ─── 다른 initial hit Actor ID ─> Stream 2

각 Stream
├─ coordinate candidates
├─ ACTIVE/TERMINATED recipients
└─ owner / interceptor

Stream 1과 Stream 2가 나중에 같은 owner를 선택해도 두 stream은 합쳐지지 않습니다.
```

stream의 주요 수명 단계와 ownership 선택은 다음과 같습니다. Intercept는 phase 자체가 아니라 TouchEvent의
전달 범위를 바꾸는 별도 arbitration입니다.

```text
최초 DOWN
   │
   ▼
UNOWNED ── TouchEvent true ──> OWNED
   │                            │
   │ TouchEvent false           │ 후속 TouchEvent는 owner에게만 전달
   │ 실제 수신자는 ACTIVE       │
   └─ 다음 event에서 재시도     │
                                │
UNOWNED 또는 OWNED ── 마지막 UP / 강제 취소 ──> FINISHING ──> FINISHED

InterceptTouchEvent true
   └─ 기존 전달 범위를 interceptor→root Touch 경로로 제한
      └─ 그 경로의 첫 TouchEvent consumer가 owner
```

---

## TouchEvent

최초 DOWN 좌표에서 geometry hit-test를 수행합니다. Intercept되기 전 owner가 없는 stream의 일반
TouchEvent는 부모-자식 관계가 아니라 coordinate candidate의 front-to-back 순서로 전달됩니다.

일반 TouchEvent의 전달 범위는 현재 stream 상태에 따라 다음과 같이 달라집니다.

- owner가 없고 아직 intercept되지 않음: 최초 DOWN의 coordinate candidate를 front-to-back으로 전달
- owner가 있음: 현재 owner에게만 전달
- intercept되었지만 새 owner가 없음: 고정된 interceptor→root 경로로 전달

### 터치 이벤트 수신 방법

View는 `TouchEventSignal`을 통해 터치 이벤트를 수신합니다. 콜백에서는 `View`가 아닌 `Actor`를 받기 때문에 View 전용 API를 사용하려면 다운캐스트가 필요합니다.

멤버 함수를 사용하는 방법:

```cpp
class MyController : public ConnectionTracker
{
public:
  void SetupTouchHandler(View view)
  {
    view.TouchEventSignal().Connect(this, &MyController::OnTouched);
  }

private:
  bool OnTouched(Actor actor, const TouchEvent& touch)
  {
    // Actor를 View로 다운캐스트
    View view = View::DownCast(actor);

    if(touch.GetState(0) == PointState::DOWN)
    {
      // 터치 다운 처리
      return true;  // consume — 이 View가 owner가 됨
    }
    return false;  // owner가 있으면 유지되고, 없을 때만 다음 coordinate candidate로 전달
  }
};
```

람다를 사용하면 간결하게 작성할 수 있습니다:

```cpp
view.TouchEventSignal().Connect(&tracker, [](Actor actor, const TouchEvent& touch) -> bool {
  View view = View::DownCast(actor);

  if(touch.GetState(0) == PointState::DOWN)
  {
    // 터치 다운 처리
    return true;  // consumed
  }
  return false;  // owner가 있으면 유지되고, 없을 때만 다음 coordinate candidate로 전달
});
```

> [!NOTE]
> owner가 없는 상태에서 `true`를 반환한 첫 Actor가 stable owner가 됩니다. `false`를 반환하면 다음
> coordinate candidate로 전달되지만, 해당 Actor도 active recipient로 남아 후속 이벤트를 다시 받고
> 나중에 consume할 수 있습니다. 다른 Actor가 owner가 되면 이전 active recipient는 `INTERRUPTED`를
> 정확히 한 번 받고 더 이상 이 stream을 받지 않습니다.

<br/>

### 이벤트 전파 방식

아래 그림과 같이 4개의 View가 화면에서 겹쳐 있을 때 DOWN 좌표를 기준으로 geometry hit-test를 수행합니다.
이 그림은 화면상의 겹침을 나타내며 View 사이의 부모-자식 관계를 의미하지 않습니다.

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

- 예를 들어 빨간색, 노란색, 파란색 View가 해당 좌표의 candidate라면 빨간색 → 노란색 → 파란색 순서로 전달됩니다.
- 빨간색 View가 `false`를 반환하면 같은 이벤트는 노란색 View로 진행됩니다. 아무 Actor도 consume하지
  않았다면 빨간색 View도 다음 Motion에서 다시 callback을 받습니다.

### Consume 동작

TouchEvent에서 처음 `true`를 반환한 Actor가 owner가 됩니다.

노란색 View가 `true`를 반환하면 이후 TouchEvent는 노란색 View에 전달됩니다. owner가 된 뒤 후속
callback에서 `false`를 반환해도 ownership은 해제되지 않습니다. 다만 owner의 조상은 후속 이벤트를
intercept할 수 있습니다.

### 이벤트 전파 예시

아래와 같은 순서가 됩니다:

```
빨간색 뷰 TouchDown return false
노란색 뷰 TouchDown return true
빨간색 뷰 Interrupted
노란색 뷰 TouchMotion return true
노란색 뷰 TouchMotion return true
노란색 뷰 TouchFinished return true
```

빨간색 View는 같은 DOWN을 이미 받은 active recipient이므로 노란색 View가 owner가 된 직후
`INTERRUPTED`를 받습니다. 이전 이벤트에서 TouchEvent를 받은 recipient가 여러 개라면 새 owner를 제외한
모든 active recipient가 각각 한 번씩 `INTERRUPTED`를 받습니다. Intercept callback만 받고 일반 TouchEvent
callback을 받지 않은 Actor는 recipient가 아니므로 이 규칙의 대상이 아닙니다.

```text
Red TouchEvent false  ──> Red = ACTIVE recipient
Yellow TouchEvent true ──> Yellow = ACTIVE recipient + owner
                           Red receives INTERRUPTED and becomes TERMINATED
```

### 터치 이벤트 전파 특징

- 최초 DOWN에서 만든 coordinate candidate 순서는 부모-자식 관계와 독립적입니다.
- owner가 정해지기 전에는 최초 candidate가 후속 이벤트에서도 경쟁할 수 있지만, Motion 좌표에서 새로
  만난 Actor가 이 stream의 candidate로 추가되지는 않습니다.
- owner는 pointer가 Actor 영역 밖으로 이동해도 stream이 끝날 때까지 유지됩니다.
- GEOMETRY Touch는 영역 이탈에 따른 `PointState::LEAVE`를 발생시키지 않습니다. HoverEvent의
  `PointState::LEAVE`는 그대로 사용됩니다.

### 부모 영역과 터치 이벤트

자식 View의 hit-test는 부모 View의 영역에 자동으로 제한되지 않습니다. 자식이 부모 영역 밖에 있더라도
해당 좌표에서 hittable하면 일반 TouchEvent의 coordinate candidate가 될 수 있습니다. 부모에
`CLIP_CHILDREN` 또는 `CLIP_TO_BOUNDING_BOX`가 설정된 경우에만 clipping 영역 밖의 자식이 hit-test에서
제외됩니다.

#### 예시: Red가 부모이고 Orange는 자식관계일 때

- Red와 Orange 영역이 겹친 좌표에서는 둘 다 일반 TouchEvent의 coordinate candidate가 될 수 있습니다.
- Red 영역 밖의 Orange를 터치해도 clipping이 없다면 Orange는 일반 TouchEvent를 받을 수 있습니다.
- Red는 해당 좌표의 candidate가 아니더라도 Orange의 조상이므로 InterceptTouchEvent를 받을 수 있습니다.
- Red가 intercept하지 않으면 Red의 일반 TouchEvent는 좌표 후보가 아닌 한 호출되지 않습니다.
- Red가 intercept하면 Red를 제외한 이전 active recipient는 `INTERRUPTED`를 받고, Red에 Touch callback이
  연결되어 있다면 Red부터 root 방향으로 일반 TouchEvent 전달을 시작합니다. Red가 `true`를 반환해야 Red가
  owner가 됩니다.

```
          Blue
        /      \
   Yellow      Red
                 |
              Orange
```


---

## InterceptTouchEvent

InterceptTouchEvent는 geometry로 겹친 전체 Actor 목록이 아니라 실제 부모-자식 경로로 전달됩니다.

### 이벤트 전파 방식

- owner가 없으면 최초 hit Actor의 ancestor path를 root에서 target 방향으로 검사합니다.
- owner가 있으면 현재 owner의 ancestor path를 root에서 owner의 직전 부모까지 검사합니다. owner 자신은
  intercept 대상에서 제외됩니다.
- 겹쳐 있더라도 owner와 부모-자식 관계가 아닌 sibling branch에는 InterceptTouchEvent가 전달되지 않습니다.

**InterceptTouchEvent는 hierarchy 순서입니다. 일반 TouchEvent는 intercept 전에는 coordinate candidate
순서이고, intercept 후에는 고정된 interceptor→root 순서입니다.**


### 이벤트 전파 예시

다음 예시는 Blue → Yellow → Red → Orange가 실제 부모-자식 관계인 경우입니다.

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

- Red가 현재 owner라면 Blue → Yellow 순서로 InterceptTouchEvent가 호출되고 Red 자신의 intercept callback은
  호출되지 않습니다.
- 아직 owner가 없고 Orange가 최초 hit Actor라면 Blue → Yellow → Red → Orange 경로를 검사할 수 있습니다.
- 모든 callback이 `false`이면 일반 TouchEvent 전달을 계속합니다.

### Consume 동작

InterceptTouchEvent에서 `true`를 반환하면 interceptor가 자동으로 owner가 되는 것이 아니라, 현재 stream의
일반 TouchEvent 전달 범위가 `root → interceptor` hierarchy path로 고정됩니다.

Yellow View가 기존 owner의 후속 이벤트를 intercept하면 다음 순서로 처리합니다.

1. Yellow를 제외한 기존 owner와 이전 ACTIVE recipient가 `INTERRUPTED`를 정확히 한 번 받습니다.
2. 같은 현재 이벤트의 일반 TouchEvent를 Yellow → Yellow의 부모 → root 순서로 전달합니다.
3. 이 제한된 경로에서 처음 `true`를 반환한 Actor가 새 owner가 됩니다.
4. 아무도 consume하지 않으면 owner가 없는 채로 같은 제한 경로가 후속 이벤트에서도 사용됩니다.
5. 한 번 성공적으로 intercept된 stream에서는 InterceptTouchEvent callback을 다시 순회하지 않습니다.

따라서 Yellow에 Touch callback이 연결되어 있고 그 callback이 `true`를 반환하는 일반적인 경우에는 Yellow가
owner가 되지만, InterceptTouchEvent의 반환값만으로 ownership이 확정되지는 않습니다.

```text
실제 hierarchy: Root → Blue → Yellow → Red(owner)

Intercept 검사: Root → Blue → Yellow(true)     [Red owner는 검사 대상에서 제외]
기존 전달 종료: Red → INTERRUPTED
일반 Touch 전달: Yellow → Blue → Root           [처음 true를 반환한 Actor가 새 owner]
```

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

Blue와 Yellow는 Red의 실제 조상이고 두 View에 InterceptTouchEvent를 등록했습니다. Yellow는
InterceptTouch를 consume합니다.

**동작 과정:**
1. Blue와 Yellow가 root-to-target 순서로 InterceptTouch를 받습니다.
2. Yellow가 InterceptTouch를 consume하면 자손 방향의 intercept 탐색과 기존 target 전달을 중단합니다.
3. 기존에 Red가 일반 TouchEvent를 받은 active recipient였다면 Red는 먼저 `INTERRUPTED`를 받습니다.
4. 일반 TouchEvent는 같은 이벤트부터 Yellow → Blue 순서로 진행합니다.
5. 이 예시처럼 Yellow의 Touch callback이 `true`를 반환하면 Yellow가 owner가 되고 이후 stream을 전담합니다.

---

## HoverEvent

HoverEvent는 해당 좌표의 geometry candidate를 front-to-back 순서로 전달합니다.

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
1. Yellow에 진입할 때 Yellow가 "Started"를 받습니다.
2. Red에 진입할 때 Red는 "Started"를 받습니다. 기존 Yellow는 그대로 Motion을 받습니다.
3. Orange에 진입할 때 Orange는 "Started"를 받습니다. 기존 Yellow와 Red는 그대로 Motion을 받습니다.

**오른쪽에서 왼쪽 방향으로 마우스를 이동:**
1. Orange에서 진출할 때 Orange는 "Leave"를 받습니다. 기존 Yellow와 Red는 그대로 Motion을 받습니다.
2. Red에서 진출할 때 Red는 "Leave"를 받습니다. 기존 Yellow는 그대로 Motion을 받습니다.
3. Yellow에서 진출할 때 Yellow는 "Leave"를 받습니다.

### Consume 동작

- Consume을 하면 뒤쪽 geometry candidate는 해당 이벤트를 받지 못합니다.
- 터치와 다르게 Consume했다고 해서 Consume한 뷰가 이후의 Hover이벤트를 받아가는 건 아닙니다.
- 현재 이벤트가 뒤쪽 candidate로 진행하는 것만 막습니다.

---

## Gesture Detection by HandleEvent

GEOMETRY mode에서 GestureDetector를 사용하는 방법은 두 가지입니다.

1. `GestureDetector::Attach(view)`를 사용하면 detector가 View의 TouchEvent에 연결됩니다.
2. Touch callback에서 `GestureDetector::HandleEvent()`를 직접 호출하면 intercept를 포함한 custom arbitration을
   구성할 수 있습니다.

같은 detector에 두 방식을 동시에 사용하면 동일 이벤트를 중복 전달할 수 있으므로 한 가지 방식만 선택해야
합니다. 다음 예제는 `Attach()`를 호출하지 않고 Touch callback에서 직접 전달하는 방식입니다.

### 기본 사용법

```cpp
// PanGestureDetector 생성 및 signal 연결. Attach(view)는 호출하지 않습니다.
mPanGestureDetector = PanGestureDetector::New();
mPanGestureDetector.DetectedSignal().Connect(this, &YourClass::OnPan);

// TouchEvent callback에서 HandleEvent 호출
bool YourClass::OnTouched(Actor actor, const TouchEvent& touch)
{
  // Gesture가 아직 인식되지 않았다면 false일 수 있습니다.
  // callback을 받은 View는 owner가 없는 동안 ACTIVE recipient로 남습니다.
  return mPanGestureDetector.HandleEvent(actor, touch);
}

void YourClass::OnPan(Actor /*actor*/, const PanGesture& /*pan*/)
{
  // Pan 상태에 따라 UI를 갱신합니다.
}
```

### 동작 원리

1. **TouchEvent 수신**: TouchEvent를 받으면 원하는 GestureDetector에 `HandleEvent`를 호출합니다. 인자로
   제스처를 인식할 View와 TouchEvent를 넘깁니다.

2. **UNOWNED 상태에서 분석**: detector가 아직 gesture를 인식하지 않았다면 `HandleEvent()`는 `false`를 반환할
   수 있습니다. Touch callback을 받은 View는 ACTIVE recipient로 남아 후속 TouchEvent를 계속 받을 수 있습니다.

3. **Ownership 선택**: detector가 gesture를 인식하여 `HandleEvent()`가 `true`를 반환하면 해당 View가 owner가
   됩니다. 이전 active recipient는 `INTERRUPTED`를 받아 recognizer를 정리합니다.

4. **Owner 유지**: owner가 된 뒤 `HandleEvent()`가 `false`를 반환하더라도 ownership은 해제되지 않습니다.
   stream 종료 또는 조상의 intercept까지 해당 View가 TouchEvent를 계속 받습니다.

---

## Gesture Propagation

Gesture signal 자체가 다른 View로 전파되는 것은 아닙니다. 다만 owner가 정해지기 전에는 동일 좌표의 여러
Touch candidate가 같은 stream을 받고 각자의 GestureDetector를 진행할 수 있습니다. 이 과정은 gesture
전파가 아니라 **touch candidate 사이의 gesture arbitration**입니다.

### 제스처 인식의 제약사항

- 제스처를 인식하기 위해서는 **연속된 터치이벤트**가 필요합니다.
- owner가 없는 동안에는 DOWN에서 `false`를 반환한 candidate도 Motion에서 gesture를 인식해 late consumer가
  될 수 있습니다.
- 처음 consume한 View가 owner가 되며, 다른 active recipient는 `INTERRUPTED`를 받습니다.
- owner가 정해진 뒤에는 다른 coordinate candidate로 일반 TouchEvent를 계속 전파하지 않습니다.
- owner의 실제 조상은 후속 Motion에서 intercept하여 전달 범위를 제한하고, 그 경로의 새 Touch consumer를
  owner로 선택할 수 있습니다.

---

## Example 1: 기본 제스처 인식

### 시나리오 설명

- 화면의 같은 좌표에 Orange, Red, Yellow가 front-to-back 순서로 겹쳐 있습니다.
- Orange의 Touch callback은 `false`를 반환합니다.
- Red의 Touch callback은 `TapGestureDetector::HandleEvent()` 결과를 반환합니다.
- Yellow의 Touch callback은 `LongPressGestureDetector::HandleEvent()` 결과를 반환합니다.
- 두 detector 모두 `Attach()`하지 않고 Touch callback에서 수동으로 event를 전달합니다.

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

1. DOWN에서 Orange가 `false`를 반환하면 Red와 Yellow도 같은 TouchEvent를 순서대로 받을 수 있습니다.

2. Red와 Yellow의 `HandleEvent()`가 아직 `false`여도 owner가 없으므로 둘 다 후속 이벤트의 ACTIVE
   recipient로 남습니다.

3. 어느 detector든 먼저 `true`를 반환하면 해당 View가 owner가 됩니다. 새 owner를 제외하고 이전에
   TouchEvent callback을 받은 active recipient는 `INTERRUPTED`를 받습니다.

### RedView 터치이벤트 콜백

```cpp
bool RedTouched(Actor actor, const TouchEvent& touch)
{
  return mTapGestureDetector.HandleEvent(actor, touch);
}

bool YellowTouched(Actor actor, const TouchEvent& touch)
{
  return mLongPressGestureDetector.HandleEvent(actor, touch);
}
```

### 결과

- Red가 DOWN을 미리 consume하지 않았다면 Yellow도 후속 TouchEvent를 받아 LongPress를 분석할 수 있습니다.
- Red의 Tap이 먼저 ownership을 선택하면 Yellow를 포함한 이전 active recipient는 종료됩니다.
- Yellow의 LongPress가 전달 중인 TouchEvent에서 먼저 ownership을 선택하면 Red와 Orange가
  `INTERRUPTED`를 받고 Yellow가 이후 stream을 전담합니다.
- Red가 DOWN부터 독점해야 한다면 `HandleEvent()` 결과와 관계없이 `true`를 반환해야 합니다. 이 경우
  Yellow는 같은 stream의 후속 TouchEvent를 받지 않습니다.

### 핵심 포인트

> `HandleEvent()` 호출 자체는 ownership을 확정하지 않습니다. GEOMETRY mode에서는 callback이 실제로
> `true`를 반환한 시점에 owner가 선택되며, 그전까지 여러 coordinate candidate가 gesture를 분석할 수
> 있습니다.

---

## Example 2: InterceptTouchEvent를 활용한 제스처 처리

### 시나리오

OrangeView는 LongPressGesture를 받고, OrangeView의 **실제 조상인 YellowView**는 PanGesture를 받으려고
합니다.

- OrangeView를 LongPress한 채로 움직여 pan threshold를 넘으면 YellowView가 stream을 intercept하고
  OrangeView의 LongPress를 취소합니다.
- YellowView가 단순히 OrangeView 뒤에 겹친 sibling이면 이 예제의 InterceptTouchEvent는 호출되지 않습니다.

```text
YellowView (parent, Pan)
└── OrangeView (child, LongPress)
```

### 구현 코드

```cpp
class LongPanGestureTest : public ConnectionTracker
{
public:
  void Setup(View yellow, View orange)
  {
    mYellowView = yellow;
    mOrangeView = orange;
    mYellowView.Add(mOrangeView); // intercept가 성립하는 실제 hierarchy

    mOrangeView.TouchEventSignal().Connect(this, &LongPanGestureTest::OnOrangeTouched);

    // 두 detector 모두 Attach하지 않고 touch/intercept callback에서 수동으로 전달합니다.
    mLongPressGestureDetector = LongPressGestureDetector::New();
    mLongPressGestureDetector.DetectedSignal().Connect(this, &LongPanGestureTest::OnLongPress);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &LongPanGestureTest::OnPan);

    mYellowView.InterceptTouchEventSignal().Connect(this, &LongPanGestureTest::OnYellowInterceptTouch);
    mYellowView.TouchEventSignal().Connect(this, &LongPanGestureTest::OnYellowTouched);

    mIsDetected      = false;
    mJustIntercepted = false;
  }

private:
  bool OnOrangeTouched(Actor actor, const TouchEvent& touch)
  {
    mLongPressGestureDetector.HandleEvent(actor, touch);
    return true; // DOWN부터 Orange를 owner로 유지합니다.
  }

  void OnLongPress(Actor /*actor*/, const LongPressGesture& /*gesture*/)
  {
    // LongPress 상태에 따라 Orange UI를 갱신합니다.
  }

  void OnPan(Actor /*actor*/, const PanGesture& pan)
  {
    if(pan.GetState() == GestureState::FINISHED || pan.GetState() == GestureState::CANCELLED)
    {
      mIsDetected = false;
      mYellowView.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::YELLOW);
    }
    else
    {
      mIsDetected = true;
      mYellowView.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::YELLOW * 0.7f);
    }
  }

  // Yellow는 owner인 Orange의 ancestor이므로 후속 event를 관찰할 수 있습니다.
  // Pan을 인식한 event에서 전달 범위를 Yellow→root로 제한합니다.
  bool OnYellowInterceptTouch(Actor actor, const TouchEvent& touch)
  {
    const bool wasDetected = mIsDetected;
    mPanGestureDetector.HandleEvent(actor, touch);
    mJustIntercepted = !wasDetected && mIsDetected;
    return mIsDetected;
  }

  bool OnYellowTouched(Actor actor, const TouchEvent& touch)
  {
    // Intercept가 시작된 현재 event는 위 callback에서 이미 detector에 전달했습니다.
    if(mJustIntercepted)
    {
      mJustIntercepted = false;
      return true; // 일반 Touch consume으로 Yellow가 owner가 됩니다.
    }

    mPanGestureDetector.HandleEvent(actor, touch);
    return true;
  }

private:
  View                     mOrangeView;
  View                     mYellowView;
  LongPressGestureDetector mLongPressGestureDetector;
  PanGestureDetector       mPanGestureDetector;
  bool                     mIsDetected;
  bool                     mJustIntercepted;
};
```

### 동작 과정

```
┌─────────────────────────────────────────────────────────────────┐
│  1. OrangeView 롱프레스 → LongPressGesture 발생                  │
│                                                                 │
│  2. 터치 이동 시작 → ancestor YellowView가 intercept path에서 관찰 │
│                                                                 │
│  3. 일정 이상 이동 → PanGesture 인식                             │
│     - YellowView.InterceptTouchEvent return true (consume)      │
│     - OrangeView는 Interrupted 수신                             │
│     - LongPressGesture 종료                                     │
│     - YellowView.TouchEvent return true → Yellow가 owner         │
│                                                                 │
│  4. 이후 터치 → YellowView에서 PanGesture 수신                   │
└─────────────────────────────────────────────────────────────────┘
```

### 상세 설명

1. **OrangeView를 롱프레스하면**: LongPressGesture가 발생합니다.

2. **이 상태에서 터치를 움직이면**: 일정 이상 움직였을 때 YellowView가 `InterceptTouchEvent`에서 `true`를
   반환합니다. OrangeView는 active recipient이므로 `Interrupted`를 정확히 한 번 받고 LongPressGesture를
   정리합니다.

3. **같은 이벤트의 Yellow Touch callback이 `true`를 반환하면** YellowView가 새 owner가 되어 이후
   PanGesture를 받습니다. Intercept callback만 `true`를 반환하고 일반 Touch callback이 consume하지 않으면
   Yellow의 ownership은 확정되지 않습니다.

### InterceptTouchEvent의 역할

`InterceptTouchEvent`는 현재 owner로 이어지는 실제 ancestor가 touch stream의 일반 Touch 전달 범위를
자신부터 root까지로 제한하는 메커니즘입니다.

- **return false**: 터치 이벤트를 하위 View로 전달 (기본 동작)
- **return true**: interceptor를 제외한 기존 active recipient를 `INTERRUPTED`로 종료하고
  interceptor → root 순서의 일반 Touch 전달을 시작

새 owner는 이 일반 Touch 전달에서 처음 `true`를 반환한 Actor입니다. 보통 interceptor가 자신의 Touch
callback에서 `true`를 반환하여 owner가 되지만 이는 별개의 consume 결정입니다.

겹쳐 있기만 한 sibling View는 이 경로에 포함되지 않습니다. 뒤쪽 sibling이 gesture를 늦게 인식하도록
하려면 일반 TouchEvent의 coordinate candidate 및 late consume을 사용해야 합니다.

이를 통해 복잡한 제스처 시나리오에서 유연하게 터치 이벤트를 제어할 수 있습니다.

---

## 요약

| 상황 | 동작 |
|------|------|
| owner가 없는 상태에서 TouchEvent `true` | 첫 consumer가 stable owner가 됨 |
| owner가 없는 상태에서 TouchEvent `false` | 다음 coordinate candidate로 전달되며 후속 이벤트에서 late consume 가능 |
| 다른 Actor가 owner가 됨 | 새 owner를 제외한 이전 active recipient가 `INTERRUPTED`를 정확히 한 번 받음 |
| owner의 후속 TouchEvent `false` | ownership을 해제하지 않으며 owner가 계속 수신 |
| InterceptTouchEvent `true` | 현재 owner의 실제 ancestor가 일반 Touch 전달 범위를 interceptor→root로 제한 |
| Intercept 이후 TouchEvent `true` | 제한된 경로의 첫 consumer가 새 owner가 됨 |
| 겹친 sibling의 InterceptTouchEvent | ancestry가 아니므로 호출되지 않음 |

### Stream 종료, multi-touch 및 TouchEvent 정보

- `UP`은 해당 device를 stream에서 분리하고 마지막 active device의 `UP`이 stream을 종료합니다.
- raw `INTERRUPTED`는 point별 route로 나누지 않고 모든 active geometry stream에 전달하는 전역 취소입니다.
  아직 고정 interceptor가 없는 stream에서는 root부터 intercept callback을 먼저 호출하고 ACTIVE recipient에
  terminal TouchEvent를 전달합니다. Intercept callback의 반환값은 이 종료를 막거나 새 owner를 만들지 않습니다.
- 이미 활성 stream에 연결된 device ID에서 새 DOWN이 오면 기존 stream 전체를 먼저 취소합니다. 기존 stream의
  모든 ACTIVE recipient는 `INTERRUPTED`를 정확히 한 번 받고, 그 뒤 새 DOWN의 hit-test와 routing을 시작합니다.
- processor가 추적하는 initial hit 또는 current owner가 insensitive/unhittable 상태가 되면 다음 처리에서 해당
  ACTIVE recipient를 `INTERRUPTED`로 종료합니다. 추적 중인 Actor가 Scene에서 분리되면 stream의 모든 ACTIVE
  recipient를 종료하고 stream을 정리합니다.
- owner가 아닌 다른 ACTIVE recipient가 일시적으로 dispatch 불가능해진 사실만으로 ownership이 바뀌지는
  않습니다. 해당 Actor는 dispatch 조건을 만족하지 않는 동안 일반 TouchEvent에서 제외되고, 새 owner 선택이나
  stream 종료 시 ACTIVE 상태라면 terminal 정리 대상이 됩니다.
- Touch callback의 첫 번째 `Actor` 인자는 실제 recipient입니다.
- `TouchEvent::GetHitActor(pointIndex)`는 callback recipient가 아니라 stream의 최초 geometry hit Actor를
  유지합니다. 첫 point는 `GetHitActor(0)`으로 확인합니다.
- event의 모든 point에서 local position은 실제 callback recipient의 좌표계로 변환됩니다.
- `DISPATCH_TOUCH_MOTION=false`인 Actor도 Motion과 `UP` 또는 `INTERRUPTED`가 섞인 event는 terminal point를
  잃지 않도록 전달받습니다.
- 같은 initial hit Actor ID, 즉 같은 initial routing group에서 시작한 multi-touch point는 한 stream을
  공유합니다. 서로 다른 initial routing group은 나중에 owner가 같아져도 독립 stream이며, 일부 pointer의
  UP은 남은 pointer를 종료하지 않습니다.

```text
device 4 DOWN on Actor A ─┐
                         ├─ Stream 1 (initial routing group = Actor A ID)
device 7 DOWN on Actor A ─┘

device 9 DOWN on Actor B ─── Stream 2 (initial routing group = Actor B ID)

Stream 1 owner = Parent P
Stream 2 owner = Parent P    ← owner가 같아도 stream은 독립적
```

#### Scene TouchEventSignal

`Scene::TouchEventSignal()`은 Actor callback의 consume 결과가 아니라 전체 Scene의 raw physical boundary를
기준으로 한 번만 발생합니다.

| raw device 상태 변화 | Scene signal |
|------|------|
| active device 0개 → 1개 이상 | first DOWN event를 한 번 전달 |
| active device가 남아 있는 Motion 또는 중간 device 추가/종료 | 전달하지 않음 |
| active device 1개 이상 → 0개 | last UP event를 한 번 전달 |
| raw `INTERRUPTED` | 한 번 전달하고 active device 상태를 모두 정리 |

하나의 raw event가 여러 geometry stream으로 나뉘어도 Scene signal 횟수는 늘어나지 않습니다. 각 stream의
Actor callback을 모두 처리한 뒤 Scene callback을 호출합니다.

### Child Drag와 ScrollView 조정

`ScrollView`는 내부적으로 `InterceptTouchEvent`와
`PanGestureDetector::HandleEvent()`를 사용합니다. pan threshold에 도달하면 touch
sequence를 consume하는 것은 일반 scroll에는 올바르지만, child가 소유한
drag-and-drop gesture와 충돌할 수 있습니다.

active child drag 동안 pan scroll만 중지합니다.

```cpp
void OnDragStarted(View, DragAndDropDetector)
{
  scrollView.SetPanScrollEnabled(false);
}

void OnDragEnded(View, DragAndDropDetector)
{
  scrollView.SetPanScrollEnabled(true);
}
```

비활성화된 동안 `ScrollView`는 child touch sequence를 intercept하지 않고 pan
displacement로 content를 움직이지 않습니다. `ScrollTo()` 같은 programmatic API는
계속 사용할 수 있으므로 drag-and-drop edge auto-scroll은 구현할 수 있습니다.

child touch callback이 `true`를 반환하는 것만으로는 parent `ScrollView`가 이후
motion을 intercept하는 것을 막을 수 없습니다. active child drag 동안 parent의
intercept 정책 자체를 중지해야 합니다.

source/target 등록, custom preview, target acceptance, edge auto-scroll 전체 예제는
[In-Scene Drag and Drop](https://github.sec.samsung.net/NUI/dali-ui/wiki/In-Scene-Drag-and-Drop-(kr))
문서를 참고하세요.

---

## 참고 사항

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
