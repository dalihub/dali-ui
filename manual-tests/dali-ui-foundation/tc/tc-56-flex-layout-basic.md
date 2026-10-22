# 56. FlexLayout: Basic Grow

FlexLayout ROW에서 고정 폭과 flex-grow 자식의 공간 분배를 확인한다.

## 화면 구성

- 좌우 50px padding
- 고정 폭 100 빨강·파랑 사이의 grow=1 초록

## 테스트 절차

1. `Parent: Width 800`과 `Parent: Full`을 번갈아 누르며 세 영역을 확인한다.
2. **기대 결과**: 빨강과 파랑은 100px을 유지하고 초록만 남은 폭을 채운다.

## 통과 기준

- 초록이 음수나 0으로 붕괴하지 않고 가용 공간을 정확히 채운다.

## 크기 제어 범위

- 버튼은 테스트 대상 부모의 requested size를 변경한다. `Full`은 버튼 행을 제외한 host의 가용 영역을 채운다.
- 고정 수치는 layout 단위이며 화면 좌표와의 수치 비교는 effective scale=1 기준이다.
- 부모 크기 변화에 따른 재배치를 검증한다. 실제 Window resize 이벤트 및 `WINDOW_RESIZED` transition 정책은 이 절차의 검증 범위에 포함하지 않는다.
