# Interactive Hover State

InteractiveTrait이 제공하는 `HOVERED` 상태와 hover 관련 signal/API 정책을 수동으로 확인한다.

## 화면 구성

- Normal interactive target
- Clickable=false target
- Policy target
- Reset / Toggle enabled / Toggle pseudo-disabled 버튼
- StateChangedSignal, HoveredChangedSignal, PressedChangedSignal 로그

## 테스트 1: Hover enter / leave

1. 포인터를 Normal interactive target 위로 이동한다
2. **기대 결과**: 배경색이 hover 색으로 바뀌고 `State`에 `Hovered`가 포함된다
3. 포인터를 target 밖으로 이동한다
4. **기대 결과**: `Hovered`가 해제된다

## 테스트 2: View 간 hover 이동

1. 포인터를 Normal interactive target 위에 올린다
2. 포인터를 Clickable=false target으로 이동한다
3. **기대 결과**: Normal target은 `Hovered`가 해제되고 Clickable=false target은 `Hovered`가 설정된다

## 테스트 3: Hover + Press 조합

1. 포인터를 Normal interactive target 위에 올린다
2. target을 누른 상태로 유지한다
3. **기대 결과**: `State`에 `Hovered`와 `Pressed`가 동시에 포함된다
4. 누름을 해제한다
5. **기대 결과**: `Pressed`만 해제되고 포인터가 여전히 위에 있으면 `Hovered`는 유지된다

## 테스트 4: Clickable=false

1. 포인터를 Clickable=false target 위로 이동한다
2. **기대 결과**: `Clickable: false` 상태에서도 `Hovered`가 설정된다

## 테스트 5: Disabled / pseudo-disabled

1. 포인터를 Policy target 위에 올린다
2. Toggle enabled 버튼을 눌러 disabled 상태로 전환한다
3. **기대 결과**: `Hovered`가 해제되고 다시 포인터를 올려도 설정되지 않는다
4. Reset을 누른 뒤 다시 Policy target 위에 포인터를 올린다
5. Toggle pseudo-disabled 버튼을 누른다
6. **기대 결과**: `Hovered`가 해제되고 pseudo-disabled 상태에서는 다시 설정되지 않는다

## 통과 기준

- Hover enter/leave와 view 간 이동에서 `Hovered` 상태가 정확히 갱신되어야 한다
- `Hovered`와 `Pressed`는 동시에 활성화될 수 있어야 한다
- `SetClickable(false)`는 hover 상태 관리를 막지 않아야 한다
- disabled/pseudo-disabled 진입 시 `Hovered`가 해제되고, 해당 상태에서는 새로 설정되지 않아야 한다
