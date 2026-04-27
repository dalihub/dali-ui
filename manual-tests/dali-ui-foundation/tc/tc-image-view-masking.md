# ImageView Alpha Masking

AlphaMaskUrl, CropToMask, MaskingMode(ON_RENDERING / ON_LOADING) 동작을 확인한다.

## 화면 구성

- 좌측: CropToMask OFF 이미지 프리뷰 (200x200)
- 우측: CropToMask ON 이미지 프리뷰 (200x200)
- 상태 라벨: Mask / MaskingMode / CropToMask 상태
- 버튼 행 1: Circle Mask / Bubble Mask / No Mask
- 버튼 행 2: MaskingMode ON_RENDERING / MaskingMode ON_LOADING

## 테스트 1: AlphaMask 적용

1. [Circle Mask] 버튼을 탭한다
2. **기대 결과**: 애니메이션이 원형 마스크로 클리핑됨, 좌우 프리뷰 차이 확인
3. [Bubble Mask] 버튼을 탭한다
4. **기대 결과**: 마스크 모양이 버블 형태로 변경됨
5. [No Mask] 버튼을 탭한다
6. **기대 결과**: 마스크 제거, 원래 사각형으로 표시됨

## 테스트 2: CropToMask 비교

1. 마스크 적용 상태에서 좌우 프리뷰를 비교한다
2. **기대 결과**: 좌측(CropToMask OFF)은 원본 크기 유지, 우측(CropToMask ON)은 마스크 영역에 맞게 크롭됨

## 테스트 3: MaskingMode 변경

1. [MaskingMode ON_LOADING] 버튼을 탭한다
2. **기대 결과**: 로딩 시점에 마스크 적용
3. [MaskingMode ON_RENDERING] 버튼을 탭한다
4. **기대 결과**: 렌더링 시점에 마스크 적용

## 통과 기준

- 마스크 적용 시 마스크 알파 채널 모양대로만 이미지가 표시되어야 한다
- CropToMask ON: 뷰가 마스크 크기에 맞게 크롭되어야 한다
- GetAlphaMaskUrl / IsCropToMask / GetMaskingMode 반환값이 설정한 값과 일치해야 한다