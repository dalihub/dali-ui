# 39. Scroll Hover Retarget

포인터를 움직이지 않은 상태에서 `ScrollView`의 아이템이 이동할 때 hover 대상이 올바르게 갱신되는지 확인한다.

## 테스트 절차

1. 스크롤 영역의 아이템 하나 위에 포인터를 올리고 그대로 유지한다.
2. 포인터를 움직이지 않은 채 마우스 휠이나 터치패드로 직접 아래 방향으로 스크롤한다.
3. 여러 아이템이 포인터 아래를 지나도록 충분히 스크롤하면서 아이템 색상과 상태 변경 로그를 확인한다.
4. 필요하면 반대 방향으로도 직접 스크롤하여 동일하게 확인한다.

## 기대 결과

- 포인터 아래에서 벗어나는 아이템은 기본색으로 돌아가고 `StateChangedSignal`에서 `HOVERED` 상태가 제거된다.
- 새로 포인터 아래에 들어오는 아이템은 초록색으로 바뀌고 `StateChangedSignal`에서 `HOVERED` 상태가 추가된다.
- 상태 변경 로그에서 각 대상 변경은 이전 아이템의 `HOVERED removed` 후 새 아이템의 `HOVERED added` 순서로 나타난다.
- 수동 스크롤 중 `HOVERED added`와 `removed`가 각각 1회 이상 기록되고 `ordering failures=0`이어야 한다.
- 위 조건을 만족하면 상단 상태 영역에 `Result: PASS`가 표시된다.

## 실패 판정

- 스크롤 중 hover된 아이템이 바뀌지 않는다.
- 이전 아이템이 hover 색으로 남는다.
- 새 아이템에 `HOVERED`가 추가되기 전에 이전 아이템의 `HOVERED` 제거가 누락된다.
- `Result: FAIL` 또는 `ordering failures`가 1 이상 표시된다.
