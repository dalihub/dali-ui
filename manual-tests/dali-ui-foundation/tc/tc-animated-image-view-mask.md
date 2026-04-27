# AnimatedImageView: AlphaMask / CropToMask / MaskingMode

AlphaMaskUrl, CropToMask, MaskingMode 동작을 확인한다. 재생 중 마스크 적용/해제를 시각적으로 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: Mask / CropToMask / MaskingMode 상태
- 버튼 행 1: Circle Mask / Bubble Mask / No Mask
- 버튼 행 2: CropToMask ON / CropToMask OFF
- 버튼 행 3: MaskingMode ON_RENDERING / MaskingMode ON_LOADING

## 테스트 1: AlphaMask 적용

1. [Circle Mask] 버튼을 탭한다
2. **기대 결과**: 애니메이션이 원형 마스크로 클리핑되어 재생됨
3. [Bubble Mask] 버튼을 탭한다
4. **기대 결과**: 마스크 모양이 버블 형태로 변경됨
5. [No Mask] 버튼을 탭한다
6. **기대 결과**: 마스크 제거, 원래 사각형으로 표시됨

## 테스트 2: CropToMask

1. 마스크 적용 상태에서 [CropToMask ON] 버튼을 탭한다
2. **기대 결과**: 뷰 크기가 마스크 영역에 맞게 조정됨
3. [CropToMask OFF] 버튼을 탭한다
4. **기대 결과**: 원래 뷰 크기 유지

## 통과 기준

- 마스크 적용 시 마스크 알파 채널 모양대로만 애니메이션이 표시되어야 한다
- CropToMask ON: 뷰가 마스크 크기에 맞게 크롭되어야 한다
- GetAlphaMaskUrl / IsCropToMask / GetMaskingMode 반환값이 설정한 값과 일치해야 한다