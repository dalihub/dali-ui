# Lottie: DepthIndex

SetDepthIndex로 LottieAnimationView 내 비주얼 렌더링 순서를 제어하는지 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (빨간색 배경)
- 상태 라벨: DepthIndex 값
- 버튼 행: Depth -1 / Depth 0

## 테스트 1: DepthIndex 변경

1. [Depth 0] (기본값) 상태를 확인한다
2. **기대 결과**: Lottie 애니메이션이 빨간색 배경 위에 정상 표시됨
3. [Depth -1] 버튼을 탭한다
4. **기대 결과**: Lottie가 배경 뒤로 이동하여 빨간색 배경만 보임
5. [Depth 0] 버튼을 탭한다
6. **기대 결과**: Lottie가 다시 배경 위에 표시됨

## 통과 기준

- DepthIndex < 0이면 Lottie가 배경 뒤에 그려져 보이지 않아야 한다
- DepthIndex >= 0이면 Lottie가 배경 위에 정상 표시되어야 한다
- 0 이상의 값은 모두 동일한 결과를 보여야 한다