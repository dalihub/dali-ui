# View KeyEvent Signal

Label에 키 포커스를 이동하면서 KeyEventSignal과 FocusChangedSignal이 정상 동작하는지 확인한다.

## 화면 구성

- 세로로 나열된 3개의 Label (각각 다른 배경색)
- 각 Label에 포커스 상태, 마지막 키 상태(Press/Release), 키 이름이 표시됨

## 테스트 1: 포커스 이동

1. Tab 또는 방향키로 Label 1에 포커스를 준다
2. **기대 결과**: Label 1에 `Focused: YES` 표시
3. Tab/방향키로 Label 2로 포커스를 이동한다
4. **기대 결과**: Label 1은 `Focused: NO`, Label 2는 `Focused: YES`

## 테스트 2: 키 이벤트 수신

1. Label 1에 포커스를 둔 상태에서 아무 키를 누른다
2. **기대 결과**: Label 1에 `Key State: Press`, `Key Name: <누른 키>` 표시
3. 키를 놓는다
4. **기대 결과**: `Key State: Release` 로 변경
5. 포커스를 Label 2로 이동한 뒤 동일한 키를 누른다
6. **기대 결과**: Label 2에 해당 키 정보가 표시되고, Label 1의 키 정보는 이전 값 그대로 유지

## 통과 기준

- 포커스를 가진 Label만 `Focused: YES`로 표시되어야 한다
- 키 이벤트는 포커스를 가진 Label에만 전달되어야 한다
- Press/Release 상태가 정확히 반영되어야 한다
