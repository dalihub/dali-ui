# 83. StackLayout: Basic Weight

세로 StackLayout에서 고정 높이와 weight 자식의 공간 분배를 확인한다.

## 화면 구성

- 50px padding과 10px spacing
- 높이 100 빨강·파랑 사이의 weight=1 초록

## 테스트 절차

1. `Parent: Height 600`과 `Parent: Full`을 번갈아 누르며 세 영역을 확인한다.
2. **기대 결과**: 빨강과 파랑은 100px을 유지하고 초록만 남은 세로 공간을 채운다.

## 통과 기준

- 고정 높이, spacing, padding을 제외한 나머지가 weight 자식에 배정된다.

## 크기 제어 범위

- 버튼은 테스트 대상 부모의 requested size를 변경한다. `Full`은 버튼 행을 제외한 host의 가용 영역을 채운다.
- 고정 수치는 layout 단위이며 화면 좌표와의 수치 비교는 effective scale=1 기준이다.
- 부모 크기 변화에 따른 재배치를 검증한다. 실제 Window resize 이벤트 및 `WINDOW_RESIZED` transition 정책은 이 절차의 검증 범위에 포함하지 않는다.
