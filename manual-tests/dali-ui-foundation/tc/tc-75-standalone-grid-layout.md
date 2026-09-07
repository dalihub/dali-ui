# 75. Standalone: GridLayout

GridLayout의 STANDALONE 자식이 cell placement에서 제외되는지 확인한다.

## 화면 구성

- 고정 track의 일반 grid cell
- 요청 좌표 `(300, 300)`의 standalone 파랑 100x100

## 테스트 절차

1. 일반 cell과 파랑을 확인한다.
2. **기대 결과**: 일반 cell은 지정 row/column에 있고 파랑은 원래 grid params 대신 자체 좌표에 놓인다.

## 통과 기준

- standalone 자식이 grid cell을 점유하거나 다른 cell을 이동시키지 않는다.
