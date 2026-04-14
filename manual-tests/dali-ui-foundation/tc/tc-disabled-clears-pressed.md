# Disabled clears Pressed

Pressed 상태에서 Disabled가 되면 Pressed가 해제되는지 확인한다.

## 화면 구성

- 파란색 InteractiveView ("Press & Hold me")
- 초록색 "Re-enable" 버튼
- 상태 변경 로그 라벨

## 테스트 1: Touch press

1. 파란색 뷰를 터치한 채로 유지한다
2. 로그에 `[Normal] -> [Pressed]` 표시 확인
3. 1.5초간 유지하면 타이머가 Disable 시킨다
4. **기대 결과**: `[Pressed] -> [Pressed, Disabled]` -> `[Pressed, Disabled] -> [Disabled]` 순서로 로그 표시, "PRESSED cleared by DISABLED!" 메시지 출력
5. 뷰가 빨간색으로 변경됨
6. "Re-enable" 버튼을 탭하여 초기화

## 테스트 2: Key press

1. Tab/방향키로 파란색 뷰에 포커스를 이동한다
2. 로그에 `[Normal] -> [Focused]` 표시 확인
3. Enter 또는 Space를 누른 채로 유지한다
4. 로그에 `[Focused] -> [Focused, Pressed]` 표시 확인
5. 1.5초간 유지하면 타이머가 Disable 시킨다
6. **기대 결과**: Pressed와 Focused가 모두 해제되어 최종 상태가 `[Disabled]`

## 통과 기준

- 두 테스트 모두 Disable 후 Pressed 상태가 남아있지 않아야 한다
- 로그에 "PRESSED cleared by DISABLED!" 메시지가 출력되어야 한다
