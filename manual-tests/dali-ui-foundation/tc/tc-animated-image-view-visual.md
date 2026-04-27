# AnimatedImageView: ImageColor / PixelArea / FittingMode

SetImageColor / GetImageColor, SetPixelArea / GetPixelArea, SetFittingMode / GetFittingMode 동작을 확인한다. 재생 중 시각 속성 변경을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: Color / PixelArea / FittingMode 정보
- 버튼 행 1: White / Red / 50% Alpha (ImageColor)
- 버튼 행 2: PA: Full / PA: TopLeft (PixelArea)
- 버튼 행 3: FIT_KEEP / FILL (FittingMode)

## 테스트 1: ImageColor

1. [Play] 상태에서 [Red] 버튼을 탭한다
2. **기대 결과**: 빨간색 틴트가 애니메이션에 적용됨
3. [50% Alpha] 버튼을 탭한다
4. **기대 결과**: 반투명 상태로 표시
5. [White] 버튼을 탭한다
6. **기대 결과**: 틴트 제거, 원래 색상 복원

## 테스트 2: PixelArea

1. [PA: TopLeft] 버튼을 탭한다
2. **기대 결과**: 좌상단 1/4 영역만 표시됨
3. [PA: Full] 버튼을 탭한다
4. **기대 결과**: 전체 애니메이션 다시 표시

## 테스트 3: FittingMode

1. [FILL] 버튼을 탭한다
2. **기대 결과**: 가로세로 비율 무시하고 뷰를 채움
3. [FIT_KEEP] 버튼을 탭한다
4. **기대 결과**: 가로세로 비율 유지

## 통과 기준

- 각 버튼 탭 후 시각 효과가 재생 중에 즉시 적용되어야 한다
- GetImageColor / GetPixelArea / GetFittingMode 반환값이 설정한 값과 일치해야 한다