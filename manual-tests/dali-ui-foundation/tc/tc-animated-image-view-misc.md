# AnimatedImageView: Misc APIs

DepthIndex, PreMultipliedAlpha, DesiredSize, PlaceholderUrl, ImageLoadWithViewSize, SamplingMode 등 기타 API 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200, 빨간색 배경)
- 상태 라벨: 각 플래그/값 상태
- 버튼 행 1: Depth -1 / Depth 0
- 버튼 행 2: Desired 50x50 / Desired 0x0
- 버튼 행 3: Set Placeholder / Clear Placeholder / Reload URL
- 버튼 행 4: PreMult ON / PreMult OFF
- 버튼 행 5: LoadWithViewSize ON / LoadWithViewSize OFF / SamplingMode 토글

## 테스트 1: DepthIndex

1. [Depth: -1] 버튼을 탭한다
2. **기대 결과**: 애니메이션이 빨간색 배경 뒤로 숨겨짐
3. [Depth: 0] 버튼을 탭한다
4. **기대 결과**: 애니메이션이 다시 보임

## 테스트 2: DesiredSize

1. [Desired 50x50] 버튼을 탭한다
2. **기대 결과**: 저해상도로 렌더링되어 흐리게 표시됨
3. [Desired 0x0] 버튼을 탭한다
4. **기대 결과**: 원본 해상도로 복원

## 테스트 3: Placeholder

1. [Set Placeholder] 버튼을 탭한다
2. [Reload URL] 버튼을 탭한다
3. **기대 결과**: 로딩 중 플레이스홀더가 잠시 표시됨
4. [Clear Placeholder] 버튼을 탭한다
5. **기대 결과**: GetPlaceholderUrl이 빈 문자열 반환

## 통과 기준

- 모든 Set 호출 후 Get/Is 반환값이 라벨에 올바르게 표시되어야 한다
- 음수 DepthIndex 시 애니메이션이 배경 뒤에 숨겨야 한다
- DesiredSize 50x50 시 저해상도 이미지가 보여야 한다