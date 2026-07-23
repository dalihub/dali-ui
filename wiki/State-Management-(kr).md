# State Management

## Overview

dali-ui의 View는 현재 상태를 **`ViewState`** 라는 bitmask 값으로 관리합니다. 여러 상태가 동시에 활성화될 수 있으며, `+` / `-` 연산자로 상태를 조합하거나 제거할 수 있습니다.

상태가 바뀔 때마다 `StateChangedSignal`이 발생하고, `StateEvent`를 통해 변경 전후의 상태와 원인 입력 이벤트를 확인할 수 있습니다.

<br/>

## Predefined States

| 상태 | 설명 |
|------|------|
| `ViewState::NORMAL` | 아무 상태도 활성화되지 않은 기본 상태 (비트 없음) |
| `ViewState::FOCUSED` | 키보드 포커스를 가진 상태 |
| `ViewState::FOCUS_INDICATED` | 포커스를 가지고 있고, 그 포커스를 시각적으로 표시해야 하는 상태 |
| `ViewState::PRESSED` | 터치 또는 키 입력으로 눌린 상태 |
| `ViewState::HOVERED` | hover 입력이 View 위에 있는 상태 |
| `ViewState::DISABLED` | 비활성화된 상태 |
| `ViewState::PSEUDO_DISABLED` | 시각적으로는 비활성화처럼 보이지만 실제로는 상호작용 가능한 상태 |
| `ViewState::SELECTED` | 선택된 상태 |

> [!NOTE]
> `FOCUSED`는 View가 현재 input focus를 가진 상태를 말하고, `FOCUS_INDICATED`는 focus를 가진 View에 시각적 focus 피드백을 표시해야 하는 상태를 말합니다. 예를 들어 입력 컨텍스트가 key에서 touch로 전환되면 View는 `FOCUSED`를 유지한 상태로 `FOCUS_INDICATED`만 해제될 수 있습니다. 이 개념은 CSS [`:focus-visible`](https://www.w3.org/TR/selectors-4/#the-focus-visible-pseudo)과 유사합니다.

자주 쓰이는 복합 상태도 미리 정의되어 있습니다.

| 복합 상태 | 구성 |
|-----------|------|
| `ViewState::SELECTED_PRESSED` | `SELECTED + PRESSED` |
| `ViewState::SELECTED_FOCUSED` | `SELECTED + FOCUSED` |
| `ViewState::DISABLED_SELECTED` | `DISABLED + SELECTED` |

<br/>

### View별로 가질 수 있는 상태

| View 종류 | 가질 수 있는 상태 |
|-----------|-----------------|
| View | `FOCUSED`, `FOCUS_INDICATED`, `DISABLED` |
| View (Interactive) | + `PRESSED`, `HOVERED`, `PSEUDO_DISABLED` |
| View (Selectable) | + `SELECTED` |

> [!NOTE]
> `AsInteractive()` 또는 `AsSelectable()`을 통해 View에 해당 Trait을 붙여 상태를 확장할 수 있습니다.

<br/>

## Querying State

```cpp
ViewState state = view.GetState();

// 특정 상태 포함 여부 (모든 비트가 설정되어 있어야 true)
if(state.Contains(ViewState::PRESSED)) { ... }

// 복합 상태와의 교집합 여부 (하나 이상의 비트가 겹치면 true)
if(state.HasIntersectionWith(ViewState::DISABLED + ViewState::PSEUDO_DISABLED)) { ... }

// NORMAL 상태 여부
if(state.IsNormal()) { ... }

// 디버깅용 문자열
Dali::String str = state.ToString(); // e.g. "Focused, Pressed"
```

`StateChangedSignal`의 `prev`/`cur` 인자를 직접 다룰 때는 `ViewState`의 상태 변화 헬퍼를 사용할 수 있습니다.

```cpp
// state가 이번 전환에서 새로 추가되었는가?
bool added = ViewState::FOCUSED.WasAdded(prev, cur);

// state가 이번 전환에서 제거되었는가?
bool removed = ViewState::FOCUSED.WasRemoved(prev, cur);

// 추가 또는 제거되었는가?
bool changed = ViewState::FOCUSED.WasChanged(prev, cur);

// 복합 상태의 비트 중 하나라도 변경되었는가?
bool anyChanged = (ViewState::PRESSED + ViewState::FOCUSED).AnyChanged(prev, cur);
```

<br/>

## Reacting to State Changes

`View::StateChangedSignal()`을 연결하면 상태가 바뀔 때마다 `StateEvent`가 전달됩니다.

```cpp
view.StateChangedSignal().Connect(tracker, [](View v, const StateEvent& e) {
  if(e.Added(ViewState::FOCUSED))   { /* 포커스 획득 */ }
  if(e.Removed(ViewState::PRESSED)) { /* 눌림 해제 */ }
  if(e.Changed(ViewState::DISABLED)) { /* 활성화/비활성화 전환 */ }

  // 변경 전/후 상태 직접 조회
  ViewState prev = e.GetPrev();
  ViewState cur  = e.GetCurrent();

  const InputEvent& cause = e.GetCause();
  if(!cause.IsProgrammatic()) {
    // 입력 이벤트가 원인인 전환
  }

  if(cause.IsCancellation()) {
    // cancellation 또는 reset으로 인한 전환
  }
});
```

> `InputEvent::IsProgrammatic()`이 `true`이면 `SetEnabled()` 등 코드에 의한 변경입니다.

<br/>

## Triggering State Changes

predefined 상태들은 시스템이 자동으로 관리합니다.

| 상태 | 관리 주체 | 변경 방법 |
|------|-----------|-----------|
| `FOCUSED` | 포커스 시스템 | 자동 |
| `FOCUS_INDICATED` | 포커스 시스템 | 포커스가 도달한 방식과 시각적 focus 피드백 표시 필요 여부에 따라 자동 |
| `PRESSED` | `InteractiveTrait` | 터치/키 입력 시 자동 |
| `HOVERED` | `InteractiveTrait` | hover 입력 시 자동 |
| `DISABLED` | `View` | `view.SetEnabled(false / true)` |
| `PSEUDO_DISABLED` | `InteractiveTrait` | `interactiveTrait.SetPseudoDisabled(true / false)` |
| `SELECTED` | `SelectableTrait` | `selectableTrait.SetSelected(true / false)` |

```cpp
// DISABLED
view.SetEnabled(false);

// PSEUDO_DISABLED
InteractiveTrait interactive = view.AsInteractive();
interactive.SetPseudoDisabled(true);

// HOVERED
interactive.HoveredChangedSignal().Connect(tracker, [](View view, bool hovered, InputEvent event) {
  // hover 진입/해제로 상태가 변경됨
});
bool hovered = interactive.IsHovered();

// SELECTED (클릭 시 토글)
SelectableTrait selectable = view.AsSelectable();
selectable.SetSelected(true);
selectable.SetToggleByClickEnabled(true);
```

> [!NOTE]
> `HOVERED`는 `PRESSED`와 독립적이므로 두 상태가 동시에 활성화될 수 있습니다. `SetClickable(false)`는 hover 상태 관리를 비활성화하지 않습니다. `DISABLED`와 `PSEUDO_DISABLED`는 `HOVERED`를 해제하고, 새로 설정되지 않도록 막습니다.

<br/>

## Custom States

`ViewState::Create()`로 커스텀 상태를 등록할 수 있습니다. 같은 이름으로 다시 호출하면 동일한 비트마스크를 반환합니다.

```cpp
static const ViewState Loading = ViewState::Create("Loading");
static const ViewState Error   = ViewState::Create("Error");

// 상태 조합
auto loadingOrError = Loading + Error;
```

> 커스텀 상태의 설정은 `Extension::View::SetState()`를 통해 이루어지며, extension 개발자 대상입니다.

> `ViewState`는 전체 32개의 bit slot을 가집니다. predefined 상태들이 그중 일부를 사용하고, 남은 slot을 커스텀 상태에 사용할 수 있습니다. 사용 가능한 bit 공간을 초과해 등록하면 `DaliException`이 발생합니다.

<br/>

## State Propagation

(TBD)

<br/>

## Extension Developer Notes

커스텀 상태를 만든 경우, `Extension::View::SetState()`로 해당 상태의 on/off를 제어합니다. `StateChangedSignal`은 자동으로 발생합니다.

```cpp
Extension::View::SetState(viewImpl, Loading, true);   // Loading 상태 활성화
Extension::View::SetState(viewImpl, Loading, false);  // Loading 상태 해제
```

extension에서 ID로 식별할 수 있는 observer가 필요하면
`SetNamedStateObserver()`를 사용할 수 있습니다. 같은 ID를 다시 등록하면
기존 observer를 교체하며, connection tracker가 파괴되면 연결된 observer도
자동으로 제거됩니다.

```cpp
Extension::View::SetNamedStateObserver(viewImpl, "LoadingStyle", tracker,
  [](View view, const StateEvent& event) {
    // 상태 변경에 반응합니다.
  });

Extension::View::UnsetNamedStateObserver(viewImpl, "LoadingStyle");
```

동일 ID로 등록된 callback 안에서 그 observer를 교체하거나 직접 해제하면
안 됩니다. 해당 callback에서 실행될 수 있는 코드는
`UnsetNamedStateObserverIfNotExecuting()`을 사용할 수 있습니다. 이 함수는
해당 observer가 실행 중이거나 존재하지 않으면 `false`를 반환합니다.
observer dispatch 중에도 다른 ID의 observer는 제거할 수 있습니다.

테마/색상 연동이 필요하다면 Color & Theme 문서의 Framework Developer Notes를 참고하세요.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
