# Disabled clears Pressed

Pressed 상태에서 Disabled가 되면 Pressed가 해제되는지 확인한다.

로그는 화면 진입 시 초기화된다(구 구현은 클래스 멤버가 재진입을 넘어 살아남아 이전 방문
내용 위에서 단언이 통과할 수 있었다 — 2026-08-26 수정). 복합 상태 문자열의 필드 순서는
빌드마다 다를 수 있다(`ViewState::ToString()`이 unordered_map 순회) — 재빌드 후 이 두 줄이
깨지면 먼저 철자를 의심할 것.

**포커스 이동 함정** (실측): 진입 직후에는 **Tab 두 번**(첫 번째는 셸 `< Back`)으로 파란
뷰에 닿지만, **[Re-enable]을 탭한 뒤에는 Tab이 그 버튼에 갇힌다** — 그때는 **↑ 방향키**로
이동하거나 화면을 나갔다 들어올 것.

## 화면 구성

- 파란색 InteractiveView ("Press & Hold me")
- 초록색 "Re-enable" 버튼
- 상태 변경 로그 라벨 (컴포넌트 `StateChangedSignal`의 prev/current를 그대로 찍음)

## 테스트 1: Touch press

1. 파란색 뷰를 터치한 채로 유지한다
2. 로그에 `[Normal] -> [Pressed]` 표시 확인
3. 1.5초간 유지하면 타이머가 Disable 시킨다
4. **기대 결과**: `[Pressed] -> [Disabled]` 로그 표시, "PRESSED cleared by DISABLED!" 출력
5. 뷰가 빨간색 계열로 변경됨 (disabled overlay가 불투명도를 0.4배로 낮춰 실제로는
   "빨강 40%"로 보인다)
6. "Re-enable" 버튼을 탭하여 초기화

## 테스트 2: Key press (Return)

1. **화면에 새로 들어와 Tab 두 번**(또는 ↑ 방향키)으로 파란 뷰에 포커스를 이동한다
2. 로그에 `[Normal] -> [FocusIndicated, Focused]` 표시 확인 (키 경로의 첫 줄은 터치와
   다르다 — 터치는 `[Normal] -> [Pressed]`)
3. **Enter(Return)** 를 누른 채로 유지한다 — 기본 실행 키는 Return 하나다(Space는 실행
   키가 아니라 아무 일도 하지 않는다; 명세 오류로 판정되어 절차에서 제외 — 2026-08-26
   사용자 결정)
4. 로그에 `[FocusIndicated, Focused] -> [FocusIndicated, Pressed, Focused]` 표시 확인
5. 1.5초간 유지하면 타이머가 Disable 시킨다
6. **기대 결과**: Pressed와 Focused가 모두 해제되어 최종 상태가 `[Disabled]`

## 테스트 3: 재활성화 후 반복

1. "Re-enable"을 탭한 뒤 **↑ 방향키**로 파란 뷰에 다시 포커스를 준다 (Tab은 이때 막힌다)
2. Return을 다시 누른 채 유지한다
3. **기대 결과**: 테스트 2와 동일한 로그가 한 번 더 쌓인다 — 같은 절차가 재활성화 후에도
   동일하게 동작해야 한다

## 통과 기준

- 두 입력 경로 모두 Disable 후 Pressed 상태가 남아있지 않아야 한다
- 로그에 "PRESSED cleared by DISABLED!" 메시지가 출력되어야 한다
- Re-enable 후 같은 절차를 반복해도 동일하게 동작해야 한다
