## 개요

DALi UI의 포커스 관리 시스템은 두 가지 핵심 개념으로 구성됩니다:

- **FocusManager** — 현재 포커스를 가진 View를 관리하는 싱글턴 매니저
- **View 시그널** — View별 키 이벤트, 포커스 변경, 상태 전환 콜백

View는 포커스를 얻은 상태여야만 키 이벤트를 수신할 수 있습니다.

<br/>

## View를 포커스 가능하게 만들기

기본적으로 View는 포커스를 받을 수 **없습니다**. 명시적으로 활성화해야 합니다:

```cpp
View view = View::New();
view.SetFocusable(true);       // 키보드 네비게이션으로 이 View에 도달 가능
view.SetTouchFocusable(true);  // 터치 시에도 포커스 부여
```

단, `InteractiveView`, `InputField` 등 일부 View는 기본적으로 포커스가 활성화되어 있습니다. 또한 일반 View에 `AsInteractive()`를 사용하여 Interactive 특성을 부여하는 경우에도 자동으로 focusable이 활성화됩니다.

<br/>

## 포커스 이동

### 프로그래밍 방식 포커스 이동

```cpp
auto focusMgr = FocusManager::Get();

// 특정 View로 포커스 이동
focusMgr.SetCurrentFocusActor(button);

// 현재 포커스된 액터 조회
Actor focused = focusMgr.GetCurrentFocusActor();

// 포커스 해제
focusMgr.ClearFocus();
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
```

사용 가능한 방향: `SetLeftFocusableView`, `SetRightFocusableView`, `SetUpFocusableView`, `SetDownFocusableView`, `SetClockwiseFocusableView`, `SetCounterClockwiseFocusableView`.

<br/>

### 포커스 그룹 & 루핑

```cpp
focusMgr.SetAsFocusGroup(container, true);  // 이 서브트리 내에서 포커스 순환
focusMgr.SetFocusGroupLoop(true);            // 그룹 경계에서 순환 처리
```

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
focusMgr.FocusChangedSignal().Connect(&tracker, [](Actor oldFocus, Actor newFocus) {
  // oldFocus에서 newFocus로 포커스 이동
});
```

<br/>

### 커스텀 포커스 알고리즘

`PreFocusChangeSignal`을 통해 기본 포커스 이동 로직을 재정의할 수 있습니다:

```cpp
focusMgr.PreFocusChangeSignal().Connect(&tracker,
  [](Actor current, Actor proposed, FocusDirection direction) -> Actor {
    // 포커스를 받을 액터를 반환합니다.
    // proposed를 반환하면 기본 동작, 다른 액터를 반환하면 재정의.
    return proposed;
  });
```

<br/>

## InteractiveView: 키를 통한 클릭 & 롱프레스 시그널

`InteractiveView`는 키 이벤트를 고수준 인터랙션(클릭, 롱프레스, 눌림 상태)으로 변환합니다. 기본적으로 focusable이 활성화되어 있으며, `ClickedSignal`, `LongPressedSignal` 등의 시그널을 직접 제공합니다:

```cpp
InteractiveView view = InteractiveView::New();

// 키 클릭 발동 시점 제어
view.SetKeyClickPolicy(KeyClickPolicy::ON_PRESS);    // 키 다운 시 클릭 시그널 발동

// ClickedSignal — 기본적으로 Enter/Return 키 해제 시 발생
view.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
  // 클릭됨 (터치 탭 또는 키보드 Enter)
});
```

### 일반 View를 Interactive로 만들기

`InteractiveView`를 사용하지 않더라도 일반 `View`에 `AsInteractive()`를 호출하면 동일한 인터랙션 기능을 부여할 수 있습니다. focusable도 자동으로 활성화됩니다:

```cpp
View view = View::New();
view.AsInteractive([&](InteractiveTrait trait) {
  trait.ClickedSignal().Connect(&tracker, [](View v, const InputEvent& event) {
    // 클릭됨 (터치 탭 또는 키보드 Enter)
  });
});
```

설정이 필요 없는 경우 콜백 없이 `view.AsInteractive()`만 호출해도 됩니다.

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

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)