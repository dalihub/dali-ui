# 48. AbsoluteLayout: Margin and Padding

AbsoluteLayout의 50px padding, 자식 margin, 중첩 layout 여백을 확인한다.

## 화면 구성

- 50px padding을 가진 root
- margin 없는 빨강과 50px margin을 가진 초록·파랑
- 자체 padding/margin을 가진 중첩 AbsoluteLayout

## 테스트 절차

1. root 가장자리와 각 색상 영역 사이의 간격을 비교한다.
2. **기대 결과**: root padding과 각 자식 margin이 독립적으로 반영된다.
3. 중첩 영역 내부를 확인한다.
4. **기대 결과**: 중첩 layout의 padding과 내부 자식 margin이 바깥 여백과 별도로 적용된다.

## 통과 기준

- 모든 여백은 코드에 설정된 실제 값 50px을 기준으로 구분된다.
