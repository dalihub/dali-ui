# 70. GridLayout: Wrap Content Star

최소 폭의 WRAP_CONTENT GridLayout에서 Auto와 Star 열의 공간 분배를 확인한다.

## 화면 구성

- 최소 폭 400·높이 200 root
- 고정 폭 200 빨강 Auto 열
- 나머지 Star 열의 초록과 내부 1:1 파랑·노랑

## 테스트 절차

1. 각 열과 중첩 cell의 폭을 비교한다.
2. **기대 결과**: Auto와 Star 열이 200px씩 차지하고 내부 파랑·노랑은 100px씩 차지한다.

## 통과 기준

- WRAP_CONTENT 최소 크기에서 Star 열이 0으로 붕괴하지 않는다.
