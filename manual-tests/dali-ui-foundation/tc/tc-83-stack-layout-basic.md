# 83. StackLayout: Basic Weight

세로 StackLayout에서 고정 높이와 weight 자식의 공간 분배를 확인한다.

## 화면 구성

- 50px padding과 10px spacing
- 높이 100 빨강·파랑 사이의 weight=1 초록

## 테스트 절차

1. 창 높이를 변경하며 세 영역을 확인한다.
2. **기대 결과**: 빨강과 파랑은 100px을 유지하고 초록만 남은 세로 공간을 채운다.

## 통과 기준

- 고정 높이, spacing, padding을 제외한 나머지가 weight 자식에 배정된다.
