# View Background Gradient Colors

`View::SetBackgroundGradient()`로 설정한 gradient의 stop color와
`ViewAnimationBridge::BackgroundGradientStartOffset()` animation을 확인한다.

## 화면 구성

- 왼쪽 카드: 보라색, 분홍색, 노란색 stop color를 가진 고정 gradient
- 오른쪽 카드: 같은 gradient에 `startOffset`을 0에서 1까지 왕복 animation 적용

## 테스트 절차

1. Manual Tests에서 `View Background Gradient Colors`를 연다.
2. 왼쪽 카드에 보라색, 분홍색, 노란색 gradient가 표시되는지 확인한다.
3. 오른쪽 카드에서 같은 색상 조합이 연속적으로 이동한 뒤 반대 방향으로 돌아오는지 확인한다.

## 통과 기준

- 두 카드 모두 동일한 gradient stop color를 표시한다.
- 오른쪽 카드의 gradient가 끊기거나 색이 사라지지 않고 반복 왕복한다.
