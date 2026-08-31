# View KeyEvent Signal

Label에 키 포커스를 이동하면서 KeyEventSignal과 FocusChangedSignal이 정상 동작하는지 확인한다.

**Label 3는 대조군이다** — 순회를 Label 2에서 멈추므로 키를 **한 번도 받지 않는다.**
"키는 포커스를 가진 Label에만 간다"의 가장 직접적인 증거는 한 번도 변하지 않은 라벨이다.
(구 명세 5단계 "Label 1의 키 정보가 이전 값 그대로"는 포커스 이동 수단이 Tab뿐이라 그
Tab이 나가는 Label을 덮어써 수행 불가능했다 — 같은 목적을 대조군이 더 강하게 증명한다.)

진입 시 세 Label 모두 `Key State: NONE / Key Name: NONE`이다 (배열 0번만 초기화되던 표시
결함은 2026-08-26 수정 — 실기 확인).

참고: 첫 Tab은 셸의 `< Back` 라벨로 간다. Label 1에 도달하려면 진입 후 Tab **두 번**.

## 화면 구성

- 세로로 나열된 3개의 Label (각각 다른 배경색)
- 각 Label에 포커스 상태, 마지막 키 상태(Press/Release), 키 이름이 표시됨

## 테스트 1: 포커스 이동

1. Tab을 두 번 눌러 Label 1에 포커스를 준다
2. **기대 결과**: Label 1에 `Focused: YES` 표시 (다른 둘은 NO)
3. Tab으로 Label 2로 포커스를 이동한다
4. **기대 결과**: Label 1은 `Focused: NO`, Label 2는 `Focused: YES`

## 테스트 2: 키 이벤트 수신 — Label 3는 NONE을 유지한다

1. Label 1에 포커스를 둔 상태에서 Return을 누른다
2. **기대 결과**: Label 1에 `Key State: Release`, `Key Name: Return` 표시
   (DOWN/UP이 순간에 지나가므로 눈에 남는 것은 Release다; 누르고 있는 동안의 `Press`
   유지는 키를 붙잡는 자동 테스트가 확인한다)
3. Tab으로 Label 2로 이동한다 (그 Tab이 나가는 Label 1에 `Key Name: Tab`을 남긴다 — 정상)
4. Return을 누른다
5. **기대 결과**: Label 2에 `Key Name: Return`. **Label 3는 여전히
   `Key State: NONE / Key Name: NONE`** — 키를 한 번도 받은 적 없는 대조군

## 통과 기준

- 포커스를 가진 Label만 `Focused: YES`로 표시되어야 한다
- 키 이벤트는 포커스를 가진 Label에만 전달되어야 한다 — **순회가 닿지 않은 Label 3가
  NONE을 유지하는 것이 직접 증거**
- Press/Release 상태가 정확히 반영되어야 한다 (키를 누르고 있는 동안 `Press`가 유지되는
  것 포함 — keyboard down/up을 쓰는 자동 테스트로 판정)
