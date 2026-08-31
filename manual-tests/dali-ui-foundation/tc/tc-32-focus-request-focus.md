# 32. Focus RequestFocus

RequestFocus를 Layout에 호출했을 때 첫 번째 focusable 자식으로 위임되는지 확인한다.

## 화면 구성

- 상단: 상태 라벨
- "RequestFocus on Layout" 버튼
- Layout: [Non-focusable] [Focusable] 두 자식 포함
- 두 자식은 기본 분홍색(`#E8D5E0`), 포커스된 자식은 주황색(`#FF9800`)으로 표시됨

## 테스트 1: Layout에 RequestFocus

1. "RequestFocus on Layout" 버튼을 탭한다
2. **기대 결과**: "Focusable" 자식이 주황색(`#FF9800`)으로 변경됨. 상태 라벨에 "SUCCESS" 표시

## 통과 기준

- Non-focusable 자식은 건너뛰고 Focusable 자식이 포커스를 받아야 한다
- Layout 자체가 포커스를 받으면 안 된다
