# Focus DescendantBlocked

컨테이너의 자손 포커스를 차단하면 자식이 포커스를 받지 못하는지 확인한다.

실제 API는 **`SetAllowDescendantFocusEnabled(bool)` / `IsAllowDescendantFocusEnabled()`**
(`Dali::Actor`, `Ui::View`가 상속)이고 **의미가 반전**돼 있다 — Block ON = Allow **OFF**.
(구 명세의 `SetDescendantFocusBlocked(true)`는 존재하지 않는 이름이었다.) 화면의
`BLOCKED:` 라벨은 `IsAllowDescendantFocusEnabled()`를 되읽어 반전 표기한다.

## 화면 구성

- 상단: 상태 라벨
- "Set Block OFF" 버튼
- "Set Block ON" 버튼
- "RequestFocus on Child" 버튼
- Container는 Block OFF일 때 파란색(`#D5DBE8`), Block ON일 때 빨간색(`#CC3333`)으로 표시됨
- Focusable Child는 기본 분홍색(`#E8D5E0`), 포커스 시 주황색(`#FF9800`)으로 표시됨

## 테스트 1: Block OFF 상태에서 포커스

1. "Set Block OFF" 버튼을 탭한다
2. "RequestFocus on Child" 버튼을 탭한다
3. **기대 결과**: Child가 주황색(`#FF9800`)으로 변경됨. "Child focused: SUCCESS" 표시

## 테스트 2: Block ON 상태에서 포커스 거부

1. "Set Block ON" 버튼을 탭한다 (Container가 빨간색(`#CC3333`)으로 변경되고 기존 포커스가 해제됨)
2. "RequestFocus on Child" 버튼을 탭한다
3. **기대 결과**: Child에 포커스가 가지 않고 기본 분홍색(`#E8D5E0`)을 유지함. "Child focus REJECTED" 표시

## 테스트 3: Block 해제 후 다시 포커스

1. "Set Block OFF" 버튼을 탭한다 (Container가 파란색(`#D5DBE8`)으로 복원)
2. "RequestFocus on Child" 버튼을 탭한다
3. **기대 결과**: Child가 다시 포커스를 받아 주황색(`#FF9800`)으로 변경됨. "Child focused: SUCCESS" 표시

## 통과 기준

- Block ON(=Allow OFF): 자식에 포커스 불가 — 라벨(`REJECTED`)·트리(포커스 없음)·픽셀(자식
  상자가 RequestFocus 전후 비트 동일, 실측 0px) 세 경로가 모두 동의해야 한다
- Block OFF: 자식에 포커스 가능 (반환값 SUCCESS + 접근성 FOCUSED)
- `BLOCKED:` 라벨이 `IsAllowDescendantFocusEnabled()` 되읽기와 일치해야 한다
- 네 색(`#D5DBE8`/`#CC3333`/`#E8D5E0`/`#FF9800`)은 골든 한 장으로 고정한다
- ("즉시"의 지연 시간 절반은 재시도 단언으로 잴 수 없어 참고로 둔다)
