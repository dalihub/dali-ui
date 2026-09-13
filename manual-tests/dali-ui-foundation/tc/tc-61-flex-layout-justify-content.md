# 61. FlexLayout: Justify Content

네 가지 JustifyContent 모드의 주축 공간 분배를 비교한다.

## 화면 구성

- 세 개의 50x50 자식을 가진 네 행
- 순서대로 FLEX_END, CENTER, SPACE_AROUND, SPACE_EVENLY

## 테스트 절차

1. 네 행의 첫/마지막 여백과 자식 사이 간격을 비교한다.
2. **기대 결과**: FLEX_END는 끝에 모이고 CENTER는 중앙에 모이며 SPACE_AROUND와
   SPACE_EVENLY는 각 규칙에 맞는 가장자리/내부 간격을 만든다.

## 통과 기준

- 네 행이 서로 구분되는 예상 분포를 보인다.
