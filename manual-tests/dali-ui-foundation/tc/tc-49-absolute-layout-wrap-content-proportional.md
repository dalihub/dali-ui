# 49. AbsoluteLayout: Wrap Content Proportional

최소 크기로 확정된 WRAP_CONTENT 부모에서 비례 자식이 Arrange 단계에 올바른 크기를 받는지 확인한다.

## 화면 구성

- 최소 폭 400, 높이 200의 회색 root
- 고정 폭 200 빨강과 부모 폭의 절반인 초록 자식
- 초록 내부의 50px 파랑과 폭 절반인 노랑

## 테스트 절차

1. 색상 영역들의 가로 폭을 비교한다.
2. **기대 결과**: root는 400px, 빨강과 초록은 각각 200px, 초록 내부 파랑은 50px, 노랑은 100px이다.

## 통과 기준

- WRAP_CONTENT 측정과 비례 Arrange 사이에 순환 또는 0 크기가 발생하지 않는다.
