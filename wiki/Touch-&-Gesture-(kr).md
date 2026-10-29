[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Touch-&-Gesture)

# Touch & Gesture 동작 방식 및 코드 구현 가이드


## 목차

1. [문서 범위](#문서-범위)
2. [TouchEvent](#touchevent)
3. [Touch 전파 차단](#touch-전파-차단)
4. [InterceptTouchEvent](#intercepttouchevent)
5. [HoverEvent](#hoverevent)
6. [Gesture Detection by HandleEvent](#gesture-detection-by-handleevent)
7. [Gesture Propagation](#gesture-propagation)
8. [Overlay 안의 중첩 스크롤](#overlay-안의-중첩-스크롤)
9. [입력 장치별 제스처 인식 옵션](#입력-장치별-제스처-인식-옵션)
10. [Example 1: 기본 제스처 인식](#example-1-기본-제스처-인식)
11. [Example 2: InterceptTouchEvent를 활용한 제스처 처리](#example-2-intercepttouchevent를-활용한-제스처-처리)
12. [요약](#요약)
13. [Touch 경계 진단 로그](#touch-경계-진단-로그)
14. [구현 확인 근거](#구현-확인-근거)

---

## 문서 범위

이 문서는 DALi UI가 application 생성 시 기본으로 활성화하는 **GEOMETRY input hit testing**을 기준으로 설명합니다.
TouchEvent와 HoverEvent 모두 front-to-back coordinate candidate를 사용하지만 수명 규칙은 다릅니다.
TouchEvent는 최초 DOWN에서 candidate를 고정하고 stream owner를 선택할 수 있는 반면, HoverEvent는 매 입력을
다시 hit-test하여 실제 방문한 candidate prefix를 active 상태로 유지합니다. `GeometryHittestEnabled`를 `false`로
변경하면 legacy PARENT hit-test 경로가 사용됩니다. 이 설정은 application 시작 시 선택하고 input event 처리가
시작된 뒤에는 변경하지 않습니다.

이 문서에서 사용하는 용어는 다음과 같습니다.

- **device**: `TouchEvent`의 point를 구분하는 device ID. 일반적으로 한 pointer의 수명을 식별합니다.
- **initial routing group**: 전파 가능한 좌표 후보 중 앞쪽부터 찾은 PINCH 또는 ROTATION detector 요구 Actor의 ID. 해당 후보가 없으면 최초 hit Actor ID를 사용하며, 같은 group ID의 device가 같은 stream에 합류합니다.
- **stream**: 같은 initial routing group에 속한 하나 이상의 device와 candidate, recipient, owner 상태를 함께
  관리하는 TouchEvent 전달 단위
- **coordinate candidate**: 최초 DOWN 좌표의 geometry hit-test로 정해지는 front-to-back TouchEvent 후보
- **owner**: TouchEvent를 처음 consume하여 stream 종료 또는 관찰 경로의 intercept 전까지 전달을 전담하는 Actor
- **recipient**: 해당 touch stream의 TouchEvent callback을 실제로 한 번 이상 받은 Actor
- **interceptor**: ancestor path의 InterceptTouchEvent callback에서 `true`를 반환하여 기존 전달 범위를 제한한 Actor
- **ancestor path**: target의 root부터 target 방향으로 이어지는 실제 부모-자식 경로

device와 stream의 관계는 다음과 같습니다.

```text
device 4 ─┐
          ├─ 같은 initial routing group ID ──> Stream 1
device 7 ─┘

device 9 ─── 다른 routing group ID ─> Stream 2

각 Stream
├─ coordinate candidates
├─ ACTIVE/TERMINATED recipients
├─ captured Touch propagation boundaries
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
- intercept되었지만 새 owner가 없음: 고정된 interceptor→root 경로를 최초 Touch 전파 차단 범위 안에서 전달

경계는 owner를 선택하지 않고 일반 전달만 제한합니다. [Touch 전파 차단](#touch-전파-차단)를 참고하세요.

### 같은 View 안에서의 소비

GEOMETRY에서는 Actor 자체의 Touch 처리와 연결된 Touch callback 반환값을 OR하며, 연결된 callback은 모두
호출합니다. 한 callback의 `false`가 같은 View의 다른 처리에서 발생한 소비를 취소하지는 않습니다.
예를 들어 click 또는 long-press 처리가 활성화된 InteractiveTrait는 앱 callback이 `false`여도 DOWN을
소비할 수 있습니다. 아래 예제는 다른 내부 처리나 callback이 입력을 소비하지 않는 조건을 가정합니다.

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
callback에서 `false`를 반환해도 ownership은 해제되지 않습니다. 최초 hit Actor 경로의 Actor는 owner
자신이나 그 아래 자식이라도 후속 이벤트를 intercept할 수 있습니다.

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
callback을 받지 않은 Actor는 별도의 intercept observer로 추적합니다. owner 선택만으로 관찰이 끝나지는
않습니다. Intercept 성공이나 stream 종료 때 남아 있는 active observer도 InterceptTouchEvent로 정리용
INTERRUPTED를 받으므로 detector에 취소를 전달해야 합니다.

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

## Touch 전파 차단

Overlay 뒤의 일반 Touch 전파를 막으면서 앞쪽 자식들의 제스처 인식을 유지하려면
`Actor::SetTouchPropagationBlocked(true)`를 사용합니다. `View`도 이 API를 상속합니다.
조회 함수는 `IsTouchPropagationBlocked()`이며 기본값은 `false`입니다.
API가 포함된 Core 버전(`@SINCE_2_5.46`)이 필요하고 GEOMETRY Touch에만 적용됩니다.

이 설정은 **자신 이후의 후보로도 Touch를 전파할지**를 정합니다. Actor의 hit 영역은 해당 좌표에서
Touch 후보가 되는지를 정하며, 전파 차단 설정으로 Actor의 크기나 hit 영역이 바뀌지는 않습니다.
자신은 Touch를 받고 `false`를 반환해도 뒤쪽 후보로의 전파를 중단할 수 있습니다.
이하에서 *전파 경계*는 이 설정이 켜진 Actor 중 전달 순서상 처음 만나는 Actor를 뜻합니다.

```cpp
View overlay = View::New();
overlay.SetTouchPropagationBlocked(true);

// 선택 사항: Touch callback이 없어도 경계는 동작합니다.
overlay.TouchEventSignal().Connect(&tracker, [](Actor, const TouchEvent&)
{
  return false; // stream을 소비하지 않고 관찰합니다.
});
```

경계 Actor는 Touch callback이 없어도 보임·민감도·hit 영역 조건을 만족하면 hit-test 후보가 됩니다.
후보에서 제외되는 것이 아니므로 연결된 callback으로 Touch를 받을 수 있습니다. front-to-back 순서에서
처음 만난 경계까지 일반 Touch를 전달하고 이후 후보로는 전파하지 않습니다. 실제 부모뿐 아니라 앞쪽 콘텐츠
뒤에 겹친 sibling도 좌표 후보라면 경계로 사용할 수 있습니다.

```text
앞쪽 아이템 → 가로 리스트 → 세로 스크롤 → Overlay 경계
                                           │
                                           └─ 일반 Touch 전파 중단
                                              배경은 전달 대상에서 제외
```

이 그림은 DOWN 좌표에서 해당 Actor들이 후보가 된 경우입니다. 자식이 부모 경계의 hit 영역 밖으로 나간 경우까지
자동으로 막는 것은 아니므로 Overlay의 크기나 clipping 범위를 의도에 맞게 구성해야 합니다.

| 동작 | 결과 |
|---|---|
| 경계 활성화 + callback 없음 또는 `false` 반환 | owner 선택 없이 일반 전파만 중단 |
| 앞쪽 View나 경계가 실제로 Touch를 소비 | consumer를 owner로 정하고 다른 active Touch recipient에 `INTERRUPTED` 전달 |
| 앞쪽 자식이 경계보다 먼저 소비 | 경계에 지속적인 관찰 권한은 없음. 이미 active였다면 경계도 `INTERRUPTED` 수신 가능 |
| 진행 중 경계 해제·숨김·삭제 | 같은 stream을 배경으로 다시 열지 않음. 새 stream부터 변경 반영 |
| 경계 밖 조상의 명시적 Intercept | 가로채기 허용. 경계 API는 Intercept를 막지 않음 |
| Hover·Wheel·PARENT Touch | 신규 API의 영향 없음 |

경계 Actor는 최초 DOWN에서 확정하며, 같은 routed stream의 내부 재시작 때도 범위를 유지합니다.
앞쪽 sibling이 나중에 Intercept하더라도 일반 전달은 최초 허용 범위를 지켜야 하므로, 부모 경로를 통해
DOWN에서 제외된 배경이나 더 바깥 경계가 다시 포함되지 않습니다. 경계는 취소 통지를 차단하지 않습니다.
일반 Touch를 받은 적 없는 배경 Actor에 단지 경계 뒤에 있다는 이유로 `INTERRUPTED`를 만들지는 않습니다.

Overlay의 Touch callback이 무조건 `true`를 반환하면 Overlay가 owner가 되고 앞쪽 active recipient를 취소합니다.
경계 API로 전환할 때는 이 소비 callback을 제거해야 합니다. `false` callback을 추가하는 것으로 다른 callback이나
InteractiveTrait의 소비를 취소할 수는 없습니다. Hover·Wheel은 별도 정책으로 처리하며, 배경 Touch 차단만을 위해
빈 gesture detector를 붙일 필요는 없습니다.

---

## InterceptTouchEvent

InterceptTouchEvent는 geometry로 겹친 전체 Actor 목록이 아니라 실제 부모-자식 경로로 전달됩니다.

### 이벤트 전파 방식

- Intercept가 성공하기 전에는 root → 최초 hit Actor 경로를 검사하며 hit Actor 자신도 포함합니다.
- Touch owner가 정해져도 경로를 줄이지 않습니다. 경로 위의 owner 자신과 그 아래 자식도 dispatch 조건을
  만족하면 InterceptTouchEvent를 받을 수 있습니다.
- 최초 hit가 Scene에서 분리되어도 확정된 owner가 남아 있으면 root → owner 경로로 전환하며 owner도 포함합니다.
- 최초 hit 경로 밖의 sibling은 검사하지 않습니다. 뒤쪽 sibling이 Touch owner가 되어도 Intercept는
  그 sibling 경로가 아니라 최초 hit branch를 계속 따릅니다.

일반 Touch는 Intercept 전에는 좌표 후보, Intercept 후에는 interceptor→root 경로로 전달됩니다.
최초 전파 경계는 일반 전달 범위를 제한하며, Intercept 경로의 탐색은 중단하지 않습니다.

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

- Orange가 최초 hit라면 Red가 Touch owner가 되어도 Blue → Yellow → Red → Orange 경로를 유지합니다.
  Red와 Orange도 Intercept callback 대상이 될 수 있습니다.
- owner 선택 후 일반 Touch는 Red에만 전달되지만 그 아래의 Intercept 관찰은 계속될 수 있습니다.
- 모든 callback이 `false`이면 일반 TouchEvent 전달을 계속합니다.

### Consume 동작

InterceptTouchEvent에서 `true`를 반환하면 interceptor가 자동으로 owner가 되는 것이 아니라, 현재 stream의
일반 TouchEvent 전달 범위가 `root → interceptor` hierarchy path로 고정됩니다. 일반 전달은 interceptor부터
root 방향으로 진행하며 최초 전파 경계도 적용합니다. 경계 밖 조상의 명시적 Intercept는 계속 허용합니다.

Yellow View가 기존 owner의 후속 이벤트를 intercept하면 다음 순서로 처리합니다.

1. Yellow를 제외한 기존 owner와 이전 ACTIVE recipient가 `INTERRUPTED`를 정확히 한 번 받습니다.
2. 같은 현재 이벤트의 일반 TouchEvent를 Yellow → Yellow의 부모 → root 순서로 전달합니다.
3. 이 제한된 경로에서 처음 `true`를 반환한 Actor가 새 owner가 됩니다.
4. 아무도 consume하지 않으면 owner가 없는 채로 같은 제한 경로가 후속 이벤트에서도 사용됩니다.
5. 한 번 성공적으로 intercept된 stream에서는 InterceptTouchEvent callback을 다시 순회하지 않습니다.

따라서 Yellow에 Touch callback이 연결되어 있고 그 callback이 `true`를 반환하는 일반적인 경우에는 Yellow가
owner가 되지만, InterceptTouchEvent의 반환값만으로 ownership이 확정되지는 않습니다.

```text
실제 hierarchy: Root → Blue → Yellow → Red(owner) → Orange(initial hit)

Intercept 검사: Root → Blue → Yellow(true)     [Yellow가 true를 반환하여 여기서 중단]
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

GEOMETRY HoverEvent는 매 입력을 hit-test하고 hover 가능한 결과를 front-to-back으로 방문합니다. 한 Actor의
전체 dispatch 결과는 intrinsic `OnHoverEvent()`와 같은 Actor에 연결된 모든 callback 반환값을 OR한 값입니다.
이 결과가 `false`이면 다음 geometry candidate로 진행하고, `true`이면 현재 입력의 candidate 순회를 중단합니다.
다음 입력은 다시 hit-test하고 반환값을 다시 평가하므로 consume은 future hover를 capture하지 않습니다.

순회가 멈출 때까지 실제 방문한 Actor들이 active-target prefix가 됩니다. `false`를 반환한 Actor도 active 상태로
남고, 영역 안에 있는 동안 후속 `MOTION`을 계속 받으며 unrelated sibling과 동시에 active일 수 있습니다.

### 이벤트 상태

- `MOTION`에서 새 candidate가 등장하면 synthetic `STARTED`를 받은 뒤 같은 입력의 `MOTION`도 받습니다.
- 기존 active candidate는 `MOTION`을 받습니다.
- 현재 candidate dispatch가 끝난 뒤, 새 visited prefix에서 빠진 이전 active target은 모두 `LEAVE`를 받습니다.
- hidden, insensitive, disabled, disconnected, remove 또는 reparent된 active target은 `INTERRUPTED`를 한 번 받습니다.
- `FINISHED`와 명시적 `INTERRUPTED`는 consume 여부와 관계없이 모든 active target에 전달된 뒤 목록을 비웁니다.

GEOMETRY hover는 모든 active target에 `STARTED`와 `LEAVE`를 보장하며 `LEAVE_REQUIRED`를 적용하지 않습니다.
PARENT hover와 touch에서는 기존의 property 제어 동작을 유지합니다.

### 이벤트 전파 예시

A와 B가 서로 parent-child 관계가 아닌 sibling이고, B가 앞에 있으며 두 영역이 일부 겹친다고 가정합니다.

```text
A 전용 영역       A/B 겹침 영역       B 전용 영역
┌──────── A ──────────┐
          ┌──────── B ──────────┐
```

B가 `false`, A가 `true`를 반환한다고 가정하면 trace는 다음과 같습니다.

```text
A 전용 -> 겹침   : B STARTED -> B MOTION -> A MOTION
겹침 안에서 이동 : B MOTION -> A MOTION
겹침 -> B 전용   : B MOTION -> A LEAVE
B 전용 -> 겹침   : B MOTION -> A STARTED -> A MOTION
```

겹침 영역에서는 B의 `false` 결과로 A까지 방문하므로 A와 B가 모두 active입니다. B가 `true`로 바뀌면 B가 현재
이벤트를 받은 뒤 순회가 중단되고, 이전에 active였던 A는 B dispatch가 끝난 다음 `LEAVE`를 받습니다. B가 다시
`false`를 반환하면 A는 `STARTED`와 `MOTION`으로 재진입합니다.

GEOMETRY에서 실제 parent는 별도 bubbling 대상이 아닙니다. front-most child와 sibling subtree 뒤의 flat hit-list
위치에 있을 때만 방문됩니다.

```text
B callback false : B -> A -> 공통 Parent -> Root (consume될 때까지)
B callback true  : 현재 입력은 B에서 중단
```

geometry 결과에 포함된 hover 가능 Actor만 순회에 참여합니다. callback의 Actor 인자는 현재 receiver를 나타내지만,
`HoverEvent::GetHitActor()`와 local position은 뒤의 A나 Parent callback에서도 primary front-most hit Actor(겹침의
B)를 계속 나타냅니다.

plain `View`는 intrinsic hover consume이 없으므로 raw callback이 `false`를 반환할 때 sibling fall-through를 확인할
수 있습니다. 반면 `InteractiveView` 또는 `InteractiveTrait`을 가진 `View`는 hover lifecycle을 intrinsic하게
처리하며 `true`를 반환합니다. 따라서 추가 raw callback이 `false`여도 전체 dispatch는 일반적으로 최상위
interactive View에서 consume됩니다.

### Consume 동작

- `false`를 반환하면 다음 front-to-back geometry candidate로 진행하며 unrelated sibling일 수도 있습니다.
- `true`를 반환하면 현재 candidate 순회만 중단하며 future hover owner 또는 capture를 만들지 않습니다.
- 한 Actor에 연결된 callback은 모두 실행됩니다. intrinsic 처리와 callback 반환값을 OR한 뒤 다음 candidate 진행
  여부를 결정합니다.

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

3. **Ownership 선택**: 인식 후 `HandleEvent()`는 `true`를 반환할 수 있습니다. Touch callback이 그 결과
   또는 앱의 처리 결정을 `true`로 반환해야 해당 View가 owner가 됩니다. 이전 active recipient는 `INTERRUPTED`를 받아 recognizer를 정리합니다.

4. **Owner 유지**: owner가 된 뒤 `HandleEvent()`가 `false`를 반환하더라도 ownership은 해제되지 않습니다.
   stream 종료 또는 관찰 경로의 intercept까지 해당 View가 TouchEvent를 계속 받습니다.

---

### 인식 결과와 Touch owner 선택은 별개

`HandleEvent()`는 recognizer에 입력을 전달합니다. 이동 방향, 남은 스크롤 범위, edit mode 같은 조건에 따라
처리가 달라진다면 detector의 인식 결과를 그대로 반환하기보다 앱의 처리 결정을 Touch callback에서 반환합니다.
대기 중 recognizer나 눌림 상태가 남지 않도록 UP/INTERRUPTED도 전달하고 정리해야 합니다.

LongPress는 Touch callback 사이에 타이머로 인식될 수 있습니다. 이 signal 자체가 TouchEvent에서 `true`를
반환하는 것은 아니므로 그 시점에 Touch owner가 자동 확정되지는 않습니다.
`GestureDetector::CancelAllOtherGestureDetectors()`는 별도의 명시적 recognizer 취소 API입니다.
현재 구현은 GestureEventProcessor에 등록된 다른 detector들을 취소하며, 하나의 Touch owner·Actor subtree·pointer
stream으로 범위가 제한되지 않습니다. 여러 손가락의 독립 동작이 필요할 때는 이 API의 영향을 함께 고려해야 합니다.

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
- Intercept 관찰 경로의 Actor는 owner 자신이나 그 아래 자식이라도 후속 이벤트를 가로챌 수 있습니다.
  제한된 경로에서 새로 Touch를 소비한 Actor가 owner가 됩니다.

---

## Overlay 안의 중첩 스크롤

```text
Overlay (Touch 전파 차단)
└─ 세로 스크롤                             ↑ ↓
   ├─ 가로 행 A                            ← →
   │  ├─ 아이템 A1: Tap / LongPress
   │  └─ 아이템 A2: Tap / LongPress
   └─ 가로 행 B                            ← →
      ├─ 아이템 B1: Tap / LongPress
      └─ 아이템 B2: Tap / LongPress
```

일반 Touch callback에서 Pan을 처리하는 custom 스크롤 View라면 필요한 후보들이 DOWN을 소비하지 않고
받도록 구성합니다. 이후 detector의 방향 제한이나 앱의 조건으로 이동을 처리할 View를 결정할 수 있습니다.

```cpp
outerPan.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
rowAPan.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
rowBPan.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
```

| 입력 | owner가 정해지기 전의 결정 |
|---|---|
| DOWN | 필요한 후보가 관찰. Overlay는 `false`를 반환하고 경계 설정으로 배경 전파만 중단 |
| 가로 이동 | 해당 행이 스크롤 가능 여부를 판단한 뒤 Touch를 소비. 다른 active Touch recipient는 취소 |
| 세로 이동 | 가로 행은 소비하지 않고, 세로 스크롤이 인식 후 소비 |
| 해당 행에서 그 방향으로 스크롤 불가 | owner 선택 전에는 `false`를 유지해 다음 허용 후보에 판단 기회 전달 |

**NUI 시절에도 Pan을 전파할지 판단하는 앱 코드가 필요했고, 그 결과로 `e.Handled`를 설정했습니다.**
custom GEOMETRY Touch 처리에서는 그 결정을 Touch 소비 여부로 표현합니다. 전파 경계는 배경 차단을 별도로
담당합니다. 이미 owner가 된 뒤 스크롤 끝에서 `false`를 반환한다고 같은 드래그가 부모로 넘어가지는 않습니다.
진행 중 전환에는 관찰 경로의 Intercept나 별도의 앱 조정 정책이 필요합니다.

위 흐름은 Core의 일반 Touch 처리 패턴입니다. 내장 `Dali::Ui::ScrollView`는 InterceptTouchEvent에서 Pan에
입력을 전달하고 가로채기 후 일반 Touch를 소비합니다. 자식 InteractiveTrait가 DOWN을 소비해도 부모는
Intercept로 스크롤을 인식할 수 있습니다. 같은 이벤트를 Intercept와 Touch 양쪽에서 detector에 중복 입력하지
않고, 바깥 스크롤이 안쪽 행의 가로 드래그까지 선점하지 않도록 방향과 우선순위를 정해야 합니다.
[ScrollView](ScrollView-(kr).md)와 [child drag 조정](#child-drag와-scrollview-조정)을 참고하세요.

서로 다른 routing group에서 시작한 두 손가락은 각 행을 독립적으로 조작할 수 있습니다. 경계 안의
PINCH/ROTATION 후보는 두 point를 의도적으로 하나의 그룹으로 묶을 수 있습니다. 독립 동작에는 detector 상태도
분리해야 하며, 하나의 detector를 공유하거나 전역 취소 정책을 사용하면 다시 서로 영향을 줄 수 있습니다.

---

## 입력 장치별 제스처 인식 옵션

같은 제스처 detector라도 어떤 입력 장치에서 시작됐는지에 따라 다르게 인식되도록 만들 수 있습니다 —
마우스, 터치스크린 손가락, TV 포인팅 리모컨, 또는 특정 이름의 장치 하나까지도 구분할 수 있습니다.
이건 위에서 다룬 전파 방식(PARENT/GEOMETRY)과는 별개입니다 — `Attach()`로 붙였든 `HandleEvent()`로
직접 먹였든 상관없이, 시퀀스를 시작한(눌린 순간의) 장치를 기준으로 제스처 시퀀스당 한 번만 해소됩니다.

### GestureDeviceSelector

`Dali::GestureDeviceSelector`가 옵션·임계값을 적용할 장치를 지정합니다:

```cpp
GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE);
GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON);
GestureDeviceSelector::ByDeviceName("Pointing Device"); // 정확히 일치(exact), 대소문자 구분
```

같은 detector에 등록된 여러 selector가 동시에 매칭되면 가장 구체적인 것이 이깁니다 — device name >
class+subclass > class 순.

### 두 계층

- **Detector-local 옵션** — `PanGestureDetector`, `TapGestureDetector`, `LongPressGestureDetector`는
  selector별로 완전한 `Options` 스냅샷을 등록할 수 있습니다. `PinchGestureDetector`와
  `RotationGestureDetector`는 전역 하나짜리 설정조차 detector 레벨엔 없어서, 여기에 등록할 대상 자체가
  없습니다.

  ```cpp
  PanGestureDetector pan = PanGestureDetector::New();

  // 마우스: 수평에 가까운 드래그일 때만 팬을 시작한다.
  PanGestureDetector::Options mouseOptions = pan.GetDefaultOptions();
  mouseOptions.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  pan.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseOptions);

  pan.Attach(view);
  ```

  `GetDeviceOptions(selector, options)`, `ClearDeviceOptions(selector)`가 나머지 API입니다.

- **앱 전역 `Dali::GestureThresholds`** — 5종 제스처(pan/tap/long press/pinch/rotation) 전부를
  다룹니다. 원래는 앱 전체에 하나뿐이던 인식 임계값입니다. device별로 등록하면 앱이 직접 붙인
  detector뿐 아니라 dali-ui 컴포넌트가 내부적으로 만드는 detector(`ScrollView`, `InteractiveTrait`,
  `InputEditor`)에도 그대로 적용됩니다.

  ```cpp
  // 마우스: 팬이 시작되려면 터치 기본값보다 더 길게 드래그해야 한다.
  GestureThresholds::PanThresholds mouseThresholds = GestureThresholds::GetDefaultPanThresholds();
  mouseThresholds.SetMinimumDistance(40);
  GestureThresholds::SetPanThresholds(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseThresholds);
  ```

| 제스처 | Detector-local `Options` | `GestureThresholds` |
|---|---|---|
| Pan | touches 범위, motion age, 각도 | 최소 거리, 최소 pan 이벤트 수 |
| Tap | taps 범위, receive-all-tap-events | multi-tap 간격, holding time, motion distance |
| Long Press | touches 범위 | 최소 holding time |
| Pinch | — (detector-local 설정 자체가 없음) | 최소 거리, 최소 touch 이벤트 수(시작 전/후) |
| Rotation | — (detector-local 설정 자체가 없음) | 최소 touch 이벤트 수(시작 전/후) |

### UiConfig와의 관계

[UiConfig](Configuration-(kr).md)의 제스처 setter(`SetLongPressGestureMinimumHoldingTime()` 등)는
`GestureThresholds`가 device selector 매칭 실패 시 쓰는 단일 전역값을 설정하며, 반드시 `Apply()` 이전에
호출해야 합니다. `GestureThresholds`는 별개의 API라 `Apply()` 이후를 포함해 언제든 설정·해제할 수
있습니다 — 호출 순서와 무관하게, 매칭되는 selector가 있으면 항상 `UiConfig`/기본값보다 우선합니다.

완전히 동작하는 예제는
[device-gesture-options 샘플](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/device-gesture-options)을
참고하세요.

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

`InterceptTouchEvent`는 최초 hit로 이어지는 관찰 경로의 Actor가 일반 Touch 전달 범위를
자신부터 root 방향으로 제한하는 메커니즘이며, 일반 전달에는 전파 경계도 적용합니다.

- **return false**: 터치 이벤트를 하위 View로 전달 (기본 동작)
- **return true**: interceptor를 제외한 기존 active recipient를 `INTERRUPTED`로 종료하고
  interceptor → root 순서의 일반 Touch 전달을 시작

새 owner는 이 일반 Touch 전달에서 처음 `true`를 반환한 Actor입니다. 보통 interceptor가 자신의 Touch
callback에서 `true`를 반환하여 owner가 되지만 이는 별개의 consume 결정입니다.

최초 hit 경로 밖의 sibling은 Intercept로 관찰하지 못합니다. 그런 sibling도 경쟁하게 하려면 판단할 때까지
일반 Touch를 소비하지 않고 전파해야 합니다. 이미 다른 Actor가 owner라면 제외된 sibling은 일반 Touch의
late consume으로 참여할 수 없습니다.

이를 통해 복잡한 제스처 시나리오에서 유연하게 터치 이벤트를 제어할 수 있습니다.

---

## 요약

| 상황 | 동작 |
|------|------|
| owner가 없는 상태에서 TouchEvent `true` | 첫 consumer가 stable owner가 됨 |
| owner가 없는 상태에서 TouchEvent `false` | 다음 coordinate candidate로 전달되며 후속 이벤트에서 late consume 가능 |
| 다른 Actor가 owner가 됨 | 새 owner를 제외한 이전 active recipient가 `INTERRUPTED`를 정확히 한 번 받음 |
| owner의 후속 TouchEvent `false` | ownership을 해제하지 않으며 owner가 계속 수신 |
| InterceptTouchEvent `true` | 관찰 경로의 Actor가 일반 Touch를 interceptor→root로 제한하며 전파 경계도 적용 |
| Intercept 이후 TouchEvent `true` | 제한된 경로의 첫 consumer가 새 owner가 됨 |
| 최초 hit 경로 밖의 InterceptTouchEvent | 최초 hit가 Scene에 있는 동안 호출되지 않음 |
| Touch 전파 차단 활성화 | owner 선택 없이 일반 전파 중단. 취소 통지와 Intercept는 유지 |

### Stream 종료, multi-touch 및 TouchEvent 정보

- `UP`은 해당 device를 stream에서 분리하고 마지막 active device의 `UP`이 stream을 종료합니다.
- raw `INTERRUPTED`는 point별 route로 나누지 않고 모든 active geometry stream에 전달하는 전역 취소입니다.
  아직 고정 interceptor가 없는 stream에서는 root부터 intercept callback을 먼저 호출하고 ACTIVE recipient에
  terminal TouchEvent를 전달합니다. Intercept callback의 반환값은 이 종료를 막거나 새 owner를 만들지 않습니다.
- 이미 활성 stream에 연결된 device ID에서 새 DOWN이 오면 기존 stream 전체를 먼저 취소합니다. 기존 stream의
  모든 ACTIVE recipient는 `INTERRUPTED`를 정확히 한 번 받고, 그 뒤 새 DOWN의 hit-test와 routing을 시작합니다.
- 추적 중인 primary target 또는 owner가 insensitive/unhittable 상태가 되면 처리 중 해당 ACTIVE recipient를
  `INTERRUPTED`로 종료합니다. 확정된 owner를 제거하면 stream이 종료됩니다. owner와 다른 최초 hit만 제거된
  경우에는 owner의 stream, render task, 물리적 UP 수신을 유지하고 Intercept 경로를 owner로 전환합니다.
  확정된 owner가 없는 상태에서 추적 target이 분리되면 남은 recipient를 종료합니다.
- owner가 아닌 다른 ACTIVE recipient가 일시적으로 dispatch 불가능해진 사실만으로 ownership이 바뀌지는
  않습니다. 해당 Actor는 dispatch 조건을 만족하지 않는 동안 일반 TouchEvent에서 제외되고, 새 owner 선택이나
  stream 종료 시 ACTIVE 상태라면 terminal 정리 대상이 됩니다.
- Touch callback의 첫 번째 `Actor` 인자는 실제 recipient입니다.
- `TouchEvent::GetHitActor(pointIndex)`는 callback recipient가 아니라 stream의 최초 geometry hit Actor를
  유지합니다. 첫 point는 `GetHitActor(0)`으로 확인합니다.
- event의 모든 point에서 local position은 실제 callback recipient의 좌표계로 변환됩니다.
- `DISPATCH_TOUCH_MOTION=false`인 Actor도 Motion과 `UP` 또는 `INTERRUPTED`가 섞인 event는 terminal point를
  잃지 않도록 전달받습니다.
- 전파 가능한 PINCH/ROTATION 후보가 있으면 서로 다른 자식 Actor의 point도 같은 stream에 들어갑니다.
  해당 후보가 없으면 최초 hit Actor ID로 그룹을 정합니다. 이 결정 전에 전파 경계를 적용하므로 경계 뒤
  배경의 PINCH/ROTATION detector가 앞쪽 stream을 합칠 수 없습니다.
  그룹화 조건은 Actor에 Attach된 detector에서 가져옵니다. `HandleEvent()`를 수동 호출하는 것만으로
  해당 Actor가 그룹화 후보로 등록되지는 않습니다.
- 서로 다른 initial routing group은 나중에 같은 Touch owner를 선택해도 독립적입니다. 한 stream에서 일부
  pointer의 UP은 남은 pointer를 종료하지 않습니다.

```text
PINCH/ROTATION 그룹 후보가 없는 경우:
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

## Touch 경계 진단 로그

consumer 없이 최초에 저장한 경계에서 일반 전달이 끝나면 Core가 다음과 같은 로그를 남깁니다.

```text
Touch propagation stopped by SetTouchPropagationBlocked(true): boundaryId(4), name(Overlay), deviceId(2), state(UP)
```

상태가 바뀐 각 point의 device ID와 상태를 기록합니다. DOWN/UP은 일반 릴리스 로그, MOTION은 Debug용 Touch
processor 필터를 사용하며 STATIONARY는 생략합니다. 따라서 `[STATIONARY, MOTION]`은 일반 로그를 반복 출력하지
않고, `[MOTION, UP]`에서는 필터가 꺼져 있어도 두 번째 point의 UP을 기록합니다.

```sh
LOG_TOUCH_PROCESSOR=1 ./your-app  # DEBUG_ENABLED Core 빌드에서 MOTION 로그도 확인
```

앞쪽 consumer나 취소·분리 때문에 전달이 끝난 경우에는 이 경계 차단 로그를 출력하지 않습니다.
현재 설정이 나중에 해제되었더라도 로그는 해당 stream의 최초 경계를 기준으로 표시합니다.

## 구현 확인 근거

2026-10-29 로컬 구현을 기준으로 다음 경로를 대조했습니다.

- `dali-core/dali/internal/event/events/geometry-touch-stream-router.cpp`: 그룹 결정, point route, Scene signal 발생 조건.
- `dali-core/dali/internal/event/events/geometry-touch-event-processor.cpp`: owner/Intercept 경로, 취소, 수명, 로그.
- `dali-core/dali/internal/event/events/geometry-touch-propagation.h`: 일반 Touch의 최초 전파 경계.
- `dali-core/dali/internal/event/events/gesture-detector-impl.cpp`, `gesture-event-processor.cpp`: HandleEvent와 명시적 detector 취소.
- `dali-core/automated-tests/src/dali/utc-Dali-GeoTouchStream.cpp`: 경계·Intercept·제거·독립/그룹 포인터 회귀 사례.
- `dali-ui-foundation/integration-api/scroll-view-impl.cpp`, `internal/views/view/interactive-trait-impl.cpp`: 내장 ScrollView와 InteractiveTrait의 입력 처리.

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
