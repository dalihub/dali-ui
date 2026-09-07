# 84. StackLayout: Alignment

세로 StackLayout 자식의 cross-axis START/CENTER/END/FILL을 비교한다.

## 화면 구성

- 폭 100의 빨강·초록·파랑과 FILL 노랑
- 행 사이 50px spacing

## 테스트 절차

1. 네 행의 수평 위치와 폭을 확인한다.
2. **기대 결과**: 빨강은 왼쪽, 초록은 중앙, 파랑은 오른쪽, 노랑은 가용 폭 전체를 채운다.

## 통과 기준

- 각 alignment가 동일한 부모 bounds에서 명확히 구분된다.
