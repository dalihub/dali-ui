# 60. FlexLayout: Reverse Directions

ROW_REVERSE와 COLUMN_REVERSE의 시작점과 역순 배치를 확인한다.

## 화면 구성

- 위쪽 ROW_REVERSE 영역
- 아래쪽 COLUMN_REVERSE 영역
- 서로 다른 높이 또는 폭의 빨강·초록·파랑

## 테스트 절차

1. 위 영역의 순서와 정렬을 확인한다.
2. **기대 결과**: 자식이 오른쪽 시작점에서 역순으로 가로 배치된다.
3. 아래 영역을 확인한다.
4. **기대 결과**: 자식이 아래쪽 시작점에서 역순으로 세로 배치된다.

## 통과 기준

- reverse 방향에서 FLEX_START가 반대쪽 물리 가장자리를 기준으로 동작한다.
