# 46. AbsoluteLayout: Proportional

복합 비례 플래그가 부모 크기를 기준으로 위치와 크기를 계산하는지 확인한다.

## 화면 구성

- 부모 전체를 채우는 파랑 `SIZE_PROPORTIONAL`
- 중앙의 부모 절반 크기 빨강 `ALL`
- 중앙 100x100 초록 `POSITION_PROPORTIONAL`

## 테스트 절차

1. `Parent: Width 800`, `Parent: Height 600`, `Parent: 800x600`, `Parent: Full`을 차례로 누르며 세 영역을 확인한다.
2. **기대 결과**: 파랑은 항상 전체를 채우고 빨강은 부모의 절반 크기를 유지하며 초록은 100x100인 채 중앙을 유지한다.

## 통과 기준

- 비례 크기와 비례 위치가 부모의 현재 content bounds를 기준으로 갱신된다.

## 크기 제어 범위

- 버튼은 테스트 대상 부모의 requested size를 변경한다. `Full`은 버튼 행을 제외한 host의 가용 영역을 채운다.
- 고정 수치는 layout 단위이며 화면 좌표와의 수치 비교는 effective scale=1 기준이다.
- 부모 크기 변화에 따른 재배치를 검증한다. 실제 Window resize 이벤트 및 `WINDOW_RESIZED` transition 정책은 이 절차의 검증 범위에 포함하지 않는다.
