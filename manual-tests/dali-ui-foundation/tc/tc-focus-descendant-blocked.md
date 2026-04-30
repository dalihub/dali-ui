# Focus DescendantBlocked

SetDescendantFocusBlocked(true) 설정 시 자식이 포커스를 받지 못하는지 확인한다.

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

- Block ON: 자식에 포커스 불가
- Block OFF: 자식에 포커스 가능
- Block ON/OFF 설정이 즉시 반영되어야 한다
