# 51. AbsoluteLayout: Per-axis Proportional

축별 비례 플래그가 지정된 축에만 적용되는지 확인한다.

## 화면 구성

- 왼쪽의 높이 50% 노랑
- 위쪽의 폭 50% 파랑
- 우상단 고정 100x100 빨강
- 좌하단 고정 100x100 초록

## 테스트 절차

1. `Parent: Width 800`, `Parent: Height 600`, `Parent: 800x600`, `Parent: Full`을 차례로 누르며 네 사각형을 확인한다.
2. **기대 결과**: 노랑은 높이만, 파랑은 폭만 변하고 빨강·초록은 크기를 유지한 채 해당 가장자리에 붙는다.

## 통과 기준

- X/Y/Width/Height 비례 플래그가 다른 축의 값에 영향을 주지 않는다.

## 크기 제어 범위

- 버튼은 테스트 대상 부모의 requested size를 변경한다. `Full`은 버튼 행을 제외한 host의 가용 영역을 채운다.
- 고정 수치는 layout 단위이며 화면 좌표와의 수치 비교는 effective scale=1 기준이다.
- 부모 크기 변화에 따른 재배치를 검증한다. 실제 Window resize 이벤트 및 `WINDOW_RESIZED` transition 정책은 이 절차의 검증 범위에 포함하지 않는다.
