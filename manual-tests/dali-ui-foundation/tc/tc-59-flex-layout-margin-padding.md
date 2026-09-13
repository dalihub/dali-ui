# 59. FlexLayout: Margin and Padding

FlexLayout의 50px padding/margin과 중첩 flex 배치를 확인한다.

## 화면 구성

- 세로 root의 빨강·초록·파랑
- 50px margin을 가진 초록
- padding/margin을 가진 중첩 가로 FlexLayout

## 테스트 절차

1. 세로 영역의 여백을 비교한다.
2. **기대 결과**: root padding과 초록 margin이 별도로 보인다.
3. 중첩 행의 세 자식을 확인한다.
4. **기대 결과**: grow 자식들이 공간을 나누며 가운데 자식의 50px margin이 추가로 반영된다.

## 통과 기준

- 실제 설정값 50px 기준의 여백과 nested grow가 모두 유지된다.
