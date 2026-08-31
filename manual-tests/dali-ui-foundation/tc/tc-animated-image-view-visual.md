# AnimatedImageView: ImageColor / PixelArea / FittingMode

SetImageColor / GetImageColor, SetPixelArea / GetPixelArea, SetFittingMode / GetFittingMode 동작을 확인한다. 재생 중/정지 상태의 시각 속성 변경을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200, 소스는 **256x128 비정사각형** dog-anim-wide.webp)
- 상태 라벨: Color / PixelArea / FittingMode 정보
- 버튼 행 1: Play / Stop
- 버튼 행 2: White / Red / Blue / 50% Alpha (ImageColor)
- 버튼 행 3: PA: Full / PA: TopLeft / PA: Center (PixelArea)
- 버튼 행 4: FIT_KEEP / FILL / OVER_FIT (FittingMode)

> **소스가 비정사각형인 이유**: 정사각형 소스를 정사각형 프리뷰에 넣으면 FIT_KEEP·FILL·
> OVER_FIT이 수학적으로 같은 결과가 되어 어떤 구현도 차이를 낼 수 없다. 2:1 소스라야
> 세 모드가 갈린다 — FIT_KEEP은 위아래 여백, FILL은 세로로 늘어남, OVER_FIT은 좌우 잘림.

## 테스트 1: ImageColor

1. [Play] 상태에서 [Red] 버튼을 탭한다
2. **기대 결과**: 빨간색 틴트가 재생 중인 애니메이션에 즉시 적용됨
3. [Blue] 버튼을 탭한다
4. **기대 결과**: 파란색 틴트로 변경됨
5. [50% Alpha] 버튼을 탭한다
6. **기대 결과**: 반투명 상태로 표시
7. [White] 버튼을 탭한다
8. **기대 결과**: 틴트 제거, 원래 색상 복원

## 테스트 2: PixelArea

1. [Stop]으로 정지시킨 뒤 [PA: TopLeft] 버튼을 탭한다
2. **기대 결과**: 원본의 좌상단 1/4 영역만 표시됨 (정지 화면이므로 픽셀로 비교 가능)
3. [PA: Center] 버튼을 탭한다
4. **기대 결과**: 원본의 중앙 1/4 영역만 표시됨 — TopLeft와 다른 그림
5. [PA: Full] 버튼을 탭한다
6. **기대 결과**: 전체 애니메이션 다시 표시

## 테스트 3: FittingMode

1. [Stop]으로 정지시킨 뒤 [FILL] 버튼을 탭한다
2. **기대 결과**: 가로세로 비율을 무시하고 뷰(200x200)를 가득 채움 (세로로 늘어남)
3. [FIT_KEEP] 버튼을 탭한다
4. **기대 결과**: 비율 유지 — 200x100으로 표시되고 위아래에 배경 여백이 생김
5. [OVER_FIT] 버튼을 탭한다
6. **기대 결과**: 비율 유지 + 뷰를 덮음 — 좌우가 잘림. 세 모드의 그림이 서로 달라야 한다

## 통과 기준

- 각 버튼 탭 후 시각 효과가 적용되어야 한다 (틴트는 재생 중 즉시, PixelArea/FittingMode는
  정지 화면의 픽셀 비교로 판정)
- GetImageColor / GetPixelArea / GetFittingMode 반환값이 설정한 값과 일치해야 한다
