# 87. StackLayout: Match Parent and Weight

최소 폭 400의 가로 StackLayout에서 MATCH_PARENT와 weight 조합을 확인한다.

## 화면 구성

- 높이 200·최소 폭 400 root
- 고정 폭 200 빨강과 weight=1 초록
- 초록을 1:1로 나누는 파랑·노랑

## 테스트 절차

1. 색상 영역의 폭과 높이를 확인한다.
2. **기대 결과**: 모든 자식은 높이 200을 채우고 빨강/초록은 200px씩, 파랑/노랑은 100px씩 차지한다.

## 통과 기준

- main-axis weight와 cross-axis MATCH_PARENT가 각 축에서 독립 동작한다.
