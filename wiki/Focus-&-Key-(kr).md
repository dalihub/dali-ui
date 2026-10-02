## 개요

DALi UI의 포커스 관리 시스템은 두 가지 핵심 개념으로 구성됩니다:

- **FocusManager** — 현재 포커스를 가진 View를 관리하는 싱글턴 매니저
- **View 시그널** — View별 키 이벤트, 포커스 변경, 상태 전환 콜백

View는 포커스를 얻은 상태여야만 키 이벤트를 수신할 수 있습니다.

<br/>

## 포커스 가능 조건

View가 포커스를 받으려면 focus system이 해당 View를 유효한 focus target으로 판단할 수 있어야 합니다:

1. View가 초기화되어 있어야 합니다.
2. View가 View tree / scene에 붙어 있어야 합니다.
3. View가 visible 상태여야 합니다.
4. View가 enabled 상태여야 합니다.
5. View가 focusable이어야 합니다. 일반 `View`의 기본값은 **focusable false**이므로, 필요하면 `SetFocusable(true)`를 호출해야 합니다.
6. ancestor 중 `SetDescendantFocusBlocked(true)`가 설정된 View가 없어야 합니다.

```cpp
View button = View::New()
  .SetFocusable(true);

scene.Add(button);
FocusManager::Get().RequestFocus(button);
```

> [!IMPORTANT]
> 포커스 성능을 위해, 하위에 focusable View가 절대 없는 container에는 가능한 `SetDescendantFocusBlocked(true)`를 설정해 두는 것을 권장합니다. Focus search가 포커스를 받을 수 없는 subtree 전체를 건너뛸 수 있습니다.

> [!NOTE]
> `InteractiveView`, `InputField` 등 일부 View는 기본적으로 focusable입니다. 일반 `View`에 `AsInteractive()`를 호출하는 경우에도 focusable이 자동으로 활성화됩니다.

### Descendant focus 차단

`SetDescendantFocusBlocked(true)`는 해당 View의 모든 descendant가 포커스를 받지 못하도록 막습니다:

```cpp
container.SetDescendantFocusBlocked(true);
```

장식용 subtree 또는 focusable View가 없다는 것이 명확한 subtree에 사용하세요. 해당 ancestor 아래의 descendant는 `RequestFocus()`, `SetCurrentFocusView()`, focus navigation에서 거부됩니다.

<br/>

## 포커스 이동

### 프로그래밍 방식 포커스 설정

```cpp
auto focusMgr = FocusManager::Get();

// 특정 View에 직접 focus 설정
focusMgr.SetCurrentFocusView(button);

// 또는 child delegation을 포함한 focus 요청
focusMgr.RequestFocus(containerOrButton);

// 현재 포커스된 View 조회
View focused = focusMgr.GetCurrentFocusView();

// 전역 포커스 해제 (다른 윈도우의 독립 포커스는 유지)
focusMgr.ClearFocus();
```

### SetCurrentFocusView와 RequestFocus의 차이

일반적인 application focus 요청에는 `RequestFocus()`를 사용하세요. 아래와 같은 resolve 를 수행합니다:

1. 대상 View에 focus request를 보냅니다.
2. 만약 대상 View가 레이아웃이라면 가장 가까운 focusable한 descendant를 찾아 request를 위임합니다.
4. 최종 resolve된 View가 current focus로 설정됩니다.
5. 대상 또는 candidate가 blocked 상태이거나 scene에 없거나 disabled, invisible, non-focusable이면 실패합니다.

`SetCurrentFocusView()`는 child delegation 없이 정확히 그 View 자체에 focus를 설정하고 싶을 때 사용합니다. 해당 View 자체가 focusable이 아니면, focusable child를 가지고 있어도 실패합니다.

```cpp
// layout 자신이 focusable하지 않아도 첫 번째 focusable child로 위임될 수 있습니다.
focusMgr.RequestFocus(layout);
```
```cpp
// layout 자신에게 직접 focus를 시도합니다. child로 위임하지 않습니다.
focusMgr.SetCurrentFocusView(layout);
```

<br/>

### 비활성 윈도우의 독립 포커스

기본 정책에서는 비활성 윈도우의 요청을 다음 활성화용 대상으로 저장합니다.
독립 옵션을 명시적으로 켜면 활성 메인윈도우의 포커스를 유지하면서 비활성
서브윈도우에도 실제 포커스를 적용할 수 있습니다:

```cpp
subWindow.SetAcceptFocus(false); // 서브윈도우를 표시하기 전에 설정
focusMgr.SetIndependentFocusEnabled(subWindow, true);
focusMgr.SetCurrentFocusView(subView);

View primary = focusMgr.GetCurrentFocusView();           // mainView
View mainActual = focusMgr.GetCurrentFocusView(mainWindow); // mainView
View subActual = focusMgr.GetCurrentFocusView(subWindow); // subView
```

윈도우가 visible이고 View가 일반 포커스 조건을 만족해야 합니다. 옵션만 켜서는
저장된 대상을 실제 포커스하지 않습니다. 기본값은 false이며
`IsIndependentFocusEnabled(window)`로 조회합니다. 두 View의 `FOCUSED` 상태가
유지되고, subView를 추가하면서 mainView에 포커스 상실을 통지하지 않습니다.
논리적 포커스만 바꾸므로 네이티브 활성화나 키그랩은 별도로 설정해야 합니다.

키 대상은 이벤트를 전달한 SceneHolder의 윈도우로 선택합니다. 이벤트의 nonzero
윈도우 ID가 native ID와 다르면 전달하지 않고, ID 0은 전달 SceneHolder를 사용합니다.
소비되지 않은 키는 같은 윈도우의 부모로 전파하고 방향키는 그 윈도우 내부에서 탐색합니다.
View가 키를 소비하면 부모 전달과 탐색이 모두 중단됩니다.

윈도우 범위의 `MoveFocus(window, direction)`, `MoveFocusBackward(window)`,
`ClearFocusIndication(window)`, `ClearFocus(window)`도 제공합니다.
`MoveFocusBackward(window)`는 현재 탐색 대상과 다른 가장 최근의 유효한 이력을
찾습니다. 일반 비활성 창에서는 예약 대상만 변경하며, 반복 호출은 이력을 한 단계씩
거슬러 올라갑니다. 현재 탐색 대상이 없으면 이력이 하나뿐이어도 가장 최근의 유효한
대상으로 복귀합니다. `ClearFocus(window)`는 실제 포커스와 예약 대상을 지우지만
이력은 유지하므로, 이후 명시적으로 `MoveFocusBackward(window)`를 호출하여
복귀할 수 있습니다. 유효한 대상이 없으면 포커스와 예약 대상은 바뀌지 않습니다.
서브 숨김·옵션 해제·대상 무효화는 서브만 해제합니다. 재표시나 옵션 재설정 후에는
명시적으로 다시 요청해야 합니다. 실제 활성화된 독립 View의 전역 역할을 바꿀 때
불필요한 포커스 상실·획득을 반복하지 않습니다.
InputField·InputEditor와 custom IME 세션은 이번 지원 범위에 포함하지 않습니다.

기존 `FocusChangedSignal()`은 전역 대상의 변경만 알립니다. 각 윈도우의 실제
대상 변경은 새로운 신호로 관찰합니다:

```cpp
focusMgr.WindowFocusChangedSignal().Connect(&tracker,
  [](Window window, View previous, View current) {
    // window 내부의 실제 포커스 변경
  });
```

기기 조작·기대 로그·라이브러리와 샘플의 빌드 버전은
[focus-key-grab 샘플](../samples/focus-key-grab/README.md)을 참고하세요.

<br/>

### 방향 포커스 이동

프레임워크는 사용자가 화살표 키를 누르면 자동으로 포커스를 이동합니다. 코드에서 직접 호출할 수도 있습니다:

```cpp
focusMgr.MoveFocus(FocusDirection::RIGHT);
focusMgr.MoveFocus(FocusDirection::DOWN);
focusMgr.MoveFocusBackward();  // 이전에 포커스되었던 View로 복귀
```

사용 가능한 방향: `LEFT`, `RIGHT`, `UP`, `DOWN`, `PAGE_UP`, `PAGE_DOWN`, `FORWARD`, `BACKWARD`, `CLOCKWISE`, `COUNTER_CLOCKWISE`.

<br/>

### 명시적 포커스 이웃 설정

특정 View 간의 방향별 포커스 관계를 직접 지정할 수 있습니다:

```cpp
viewA.SetRightFocusableView(viewB);
viewB.SetLeftFocusableView(viewA);
viewA.SetForwardFocusableView(next);
next.SetBackwardFocusableView(viewA);
```

사용 가능한 setter: `SetLeftFocusableView`, `SetRightFocusableView`, `SetUpFocusableView`, `SetDownFocusableView`, `SetClockwiseFocusableView`, `SetCounterClockwiseFocusableView`, `SetForwardFocusableView`, `SetBackwardFocusableView`.

<br/>

### 포커스 그룹

```cpp
focusMgr.SetAsFocusGroup(container, true);  // 이 subtree 안에 focus 이동을 제한
```

`FocusGroup`은 사용자 navigation의 containment boundary입니다. View policy,
명시적 이웃, application fallback, 기본 finder가 선택한 candidate는 모두 가장
가까운 Focus Group subtree 안에 있어야 합니다. Focus Group View 자체의 local
policy까지 호출하지만 그 바깥 ancestor policy는 호출하지 않습니다.

`RequestFocus()`와 `SetCurrentFocusView()`는 명시적인 programmatic operation이므로
Focus Group 밖으로 이동할 수 있습니다. 따라서 dialog나 popup을 닫고 다른 곳에
focus를 복원하기 위한 별도 escape API는 필요하지 않습니다.

<br/>

## 키 이벤트 처리

포커스된 키 대상 View는 자신의 윈도우로 전달된 키 입력/해제에 대해
`KeyEventSignal`을 발생시킵니다. `true`를 반환하면 이벤트를 소비하여
부모 전달과 방향키 탐색을 중단합니다.

멤버 함수를 사용하는 방식:

```cpp
class MyController : public ConnectionTracker
{
public:
  void SetupKeyHandler(View view)
  {
    view.SetFocusable(true);
    view.KeyEventSignal().Connect(this, &MyController::OnKeyEvent);
  }

private:
  bool OnKeyEvent(View view, const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::State::DOWN)
    {
      if(event.GetKeyName() == "Return")
      {
        // Enter 키 처리
        return true;  // 이벤트 소비
      }
    }
    return false;  // 이벤트 전파 허용
  }
};
```

람다를 사용하여 간결하게 작성할 수도 있습니다:

```cpp
view.KeyEventSignal().Connect(&tracker, [](View view, const KeyEvent& event) -> bool {
  if(event.GetState() == KeyEvent::State::DOWN)
  {
    if(event.GetKeyName() == "Return")
    {
      // Enter 키 처리
      return true;  // 이벤트 소비
    }
  }
  return false;  // 이벤트 전파 허용
});
```

<br/>

## 포커스 변경에 반응하기

### View별 시그널

```cpp
view.FocusChangedSignal().Connect(&tracker, [](View view, bool focused) {
  if(focused)
  {
    // View가 포커스를 얻음
  }
  else
  {
    // View가 포커스를 잃음
  }
});
```

<br/>

### 글로벌 시그널 (FocusManager)

```cpp
focusMgr.FocusChangedSignal().Connect(&tracker, [](View oldFocus, View newFocus) {
  // oldFocus에서 newFocus로 포커스 이동
});
```

<br/>

### 커스텀 포커스 네비게이션

규칙을 소유하는 가장 좁은 범위의 확장 방법을 선택하세요:

| 요구사항 | API | 범위와 우선순위 |
|---|---|---|
| 소수 View 사이의 고정 관계 | 방향별 focusable View setter | View local policy 다음 |
| 기존 container 인스턴스의 정책 | `View::SetFocusNavigationCallback()` | 해당 container subtree |
| custom View 클래스가 구현하는 정책 | `ViewImpl::OnFocusNavigationRequested()` | 해당 View 종류. 인스턴스 callback이 없을 때만 사용 |
| 앱 전역 알고리즘 또는 첫 focus 선택 | `FocusManager::SetFocusNavigationFallback()` | local policy와 명시 target 다음, `FocusFinder` 이전 |
| 일반적인 geometry/linear 탐색 | framework `FocusFinder` | 최종 fallback |

View local callback과 application callback은 같은 signature를 사용합니다:

```cpp
FocusNavigationResult GetNextFocusableView(
  View current,
  FocusNavigationContext context);
```

`FocusNavigationResult`는 처리 결과를 명확히 구분합니다:

- `NotHandled()`: 다음 policy로 진행합니다.
- `MoveTo(view)`: policy 탐색을 종료하고 framework가 candidate를 검증하고
  resolve한 뒤 focus를 설정합니다. candidate가 invalid라면 요청이 실패하며 더
  낮은 우선순위로 조용히 fallback하지 않습니다.
- `Stay()`: focus를 이동하지 않고 요청을 소비합니다. 따라서 `MoveFocus()`는
  `false`를 반환합니다.

immutable context에서는 direction, device 종류, device name, 원본 `InputEvent`,
source `Window`, 가장 가까운 Focus Group을 조회할 수 있습니다. Policy는 focus를
직접 변경하거나 navigation API를 재귀 호출하지 말고 결과만 반환해야 합니다.

#### Container policy

```cpp
FocusNavigationResult NavigatePanel(View current, FocusNavigationContext context)
{
  if(context.GetDirection() == FocusDirection::RIGHT)
  {
    if(View candidate = FindCustomRightTarget(current))
    {
      return FocusNavigationResult::MoveTo(candidate);
    }
    return FocusNavigationResult::Stay();
  }
  return FocusNavigationResult::NotHandled();
}

panel.SetFocusNavigationCallback(FocusNavigationCallback::New(&NavigatePanel));
```

callback 설정은 해당 인스턴스의 virtual policy를 대체하며, 빈 callback을 설정하면
virtual policy를 다시 사용합니다. Callback은 target의 lifetime을 연장하지 않으므로
member-function target이 파괴되기 전에 callback을 해제해야 합니다.

#### Application fallback과 첫 focus

```cpp
class ApplicationNavigation
{
public:
  FocusNavigationResult GetNextFocusableView(
    View current,
    FocusNavigationContext context)
  {
    if(!current)
    {
      return FocusNavigationResult::MoveTo(FindEntryView(context.GetWindow()));
    }

    if(View candidate = FindApplicationCandidate(current, context))
    {
      return FocusNavigationResult::MoveTo(candidate);
    }
    return FocusNavigationResult::NotHandled();
  }
};

manager.SetFocusNavigationFallback(
  FocusNavigationCallback::New(&algorithm, &ApplicationNavigation::GetNextFocusableView));
```

Application fallback은 하나만 저장됩니다. 새 callback은 기존 callback을 교체하고,
빈 callback은 이를 해제합니다. Framework default algorithm이 꺼져 있어도 fallback은
호출됩니다. Current focus가 없으면 navigation Window를 결정할 수 있는 경우에
fallback부터 호출하고, `NotHandled()`일 때 default finder가 활성화되어 있으면 finder로
진행합니다. Programmatic `MoveFocus()`는 current focus의 Window 또는 마지막으로 focus된
Window를 사용하며, 둘 다 없으면 fallback을 호출하지 않고 실패합니다. 입력으로 시작된
navigation은 input source Window를 사용할 수 있습니다.

전체 우선순위는 다음과 같습니다:

1. 가장 가까운 container부터 바깥쪽으로 View local policy 호출. 가장 가까운 Focus
   Group View 자체까지 포함
2. 명시적 방향 target
3. application fallback
4. framework `FocusFinder`

`OnFocusRequested()`의 역할은 다릅니다. Policy가 `MoveTo()`로 container를 선택한
뒤 그 container를 실제 focusable View로 resolve하는 hook이며, navigation algorithm
확장 지점이 아닙니다.

#### dali-toolkit에서 이전

`DevelKeyboardFocusManager::CustomAlgorithmInterface` 사용 코드는 다음처럼 나눕니다:

- component 또는 특정 영역의 규칙은 owning container callback이나
  `OnFocusNavigationRequested()` override로 이동
- 앱 전체 공통 규칙은 `SetFocusNavigationFallback()`에 등록
- 같은 fallback에서 빈 `current`를 처리하여 첫 focus 선택
- 기본 탐색을 중단하려면 `Stay()`, 위임하려면 `NotHandled()` 반환

기존 raw algorithm pointer와 달리 manager는 callback object를 소유하고,
input/window/scope context와 명시적인 결과 타입을 제공합니다. 단, member-function
callback의 target object는 여전히 호출자가 lifetime을 관리해야 합니다.

<br/>

## InteractiveView: 키를 통한 클릭 & 롱프레스 시그널

`InteractiveView`는 키 이벤트를 고수준 인터랙션(클릭, 롱프레스, 눌림 상태)으로 변환합니다. 기본적으로 focusable이 활성화되어 있으며, `ClickedSignal`, `LongPressedSignal` 등의 시그널을 직접 제공합니다:

```cpp
InteractiveView view = InteractiveView::New();

// 키 클릭 발동 시점 제어
view.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS); // 키 다운 시 클릭 시그널 발동

// ClickedSignal
view.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
  // 클릭됨 (터치 탭 또는 실행키 Enter)
});
```

롱프레스 처리는 기본적으로 활성화됩니다. Signal handler 연결을 유지한 채 touch와 key
`LongPressedSignal` 전달을 모두 일시 중지하려면 `SetLongPressEnabled(false)`를 사용합니다.
Tap/click은 `SetClickable()`로 독립적으로 제어합니다.

### 일반 View를 Interactive로 만들기

`InteractiveView`를 사용하지 않더라도 일반 `View`에 `AsInteractive()`를 호출하면 동일한 인터랙션 기능을 부여할 수 있습니다. focusable도 자동으로 활성화됩니다:

```cpp
View view = View::New();
InteractiveTrait interactive = view.AsInteractive();
interactive.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
  // 클릭됨 (터치 탭 또는 실행키 Enter)
});
```

설정이 필요 없는 경우 `view.AsInteractive()`만 호출하고 반환된 trait을 무시해도 됩니다.

<br/>

### UiConfig를 통한 글로벌 기본값 설정

`UiConfig`에서 모든 Interactive View에 적용되는 기본 동작을 설정할 수 있습니다:

| 설정 | 설명 |
|---|---|
| `SetKeyClickPolicy` | 기본 키 클릭 발동 시점 (ON_RELEASE / ON_PRESS / DISABLED) |
| `SetExecutionKeyPredicate` | 클릭/롱프레스를 트리거하는 키 판별 함수 (기본값: "Return") |
| `SetLongPressKeyEventMinimumCount` | 키를 롱프레스로 인식하기 위한 최소 키 pressed 이벤트 개수 (최초 key down 포함) |

```cpp
bool MyKeyPredicate(const Dali::String& keyName)
{
  return keyName == "Return" || keyName == "KP_Enter";
}

UiConfig config = UiConfig::New();
config.SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);   // 키를 뗄 때 클릭 시그널 발동
config.SetExecutionKeyPredicate(MyKeyPredicate);        // Return, KP_Enter를 클릭 키로 인식
config.SetLongPressKeyEventMinimumCount(3);             // 키 pressed 이벤트가 3회 이상 연속 감지되면 롱프레스로 인식
config.Apply();
```

> [!WARNING]
> `SetExecutionKeyPredicate`는 캡처 없는 함수 포인터만 허용합니다. `nullptr`를 전달하면 기본값("Return")으로 복원됩니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
