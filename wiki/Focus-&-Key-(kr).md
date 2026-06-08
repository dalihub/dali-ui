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

// 포커스 해제
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

`FocusGroup`은 containment boundary입니다. 포커스가 `FocusGroup` 내부에 있을 때 기본 키 포커스 이동은 해당 subtree 안으로 제한됩니다. 패널, 다이얼로그, 팝업, 컴포넌트 내부처럼 arrow key나 Tab / Shift+Tab으로 focus가 밖으로 빠져나가면 안 되는 영역에 사용하세요.

<br/>

## 키 이벤트 처리

포커스를 가진 View는 모든 키 입력/해제에 대해 `KeyEventSignal`을 발생시킵니다. `true`를 반환하면 이벤트를 소비하여 추가 전파를 막습니다.

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

View subtree의 focus navigation을 직접 제어하려면 focus navigation callback을 설정합니다:

```cpp
View MyFocusNavigation(View currentFocusedView, FocusDirection direction)
{
  if(direction == FocusDirection::RIGHT)
  {
    return FindCustomRightTarget(currentFocusedView);
  }
  return View(); // 빈 handle을 반환하면 parent/default navigation이 이어서 처리합니다.
}

view.SetFocusNavigationCallback(FocusNavigationCallback::New(&MyFocusNavigation));
```

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
| `SetKeyLongPressThreshold` | 키를 롱프레스로 인식하기 위한 최소 임계값 (키 pressed 이벤트 발생 횟수) |

```cpp
bool MyKeyPredicate(const Dali::String& keyName)
{
  return keyName == "Return" || keyName == "KP_Enter";
}

UiConfig::New()
  .SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE)   // 키를 뗄 때 클릭 시그널 발동
  .SetExecutionKeyPredicate(MyKeyPredicate)        // Return, KP_Enter를 클릭 키로 인식
  .SetKeyLongPressThreshold(3)                     // 키 pressed 이벤트가 3회 이상 연속 감지되면 롱프레스로 인식
  .Apply();
```

> [!WARNING]
> `SetExecutionKeyPredicate`는 캡처 없는 함수 포인터만 허용합니다. `nullptr`를 전달하면 기본값("Return")으로 복원됩니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
