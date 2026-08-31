# 06. AnimatedImageView: AlphaMask / CropToMask / MaskingMode

AlphaMaskUrl, CropToMask, MaskingMode 동작을 확인한다. 마스크 적용/해제를 정지 화면의 픽셀로 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: Mask / CropToMask / MaskingMode 상태 (Mask는 GetAlphaMaskUrl 반환 경로를 그대로 출력)
- 버튼 행 1: Play / Stop
- 버튼 행 2: Circle Mask / Bubble Mask / No Mask
- 버튼 행 3: CropToMask ON / CropToMask OFF
- 버튼 행 4: MaskingMode ON_RENDERING / MaskingMode ON_LOADING

> **참고**: 마스크·CropToMask·MaskingMode setter는 비주얼을 다시 만들며 재생을 멈추고
> 첫 프레임을 정지 상태로 표시한다(프레임워크 동작). 그 정지 화면끼리 비교하면 되고,
> 재생을 계속 보려면 [Play]를 다시 누른다.

## 테스트 1: AlphaMask 적용

1. [Circle Mask] 버튼을 탭한다
2. **기대 결과**: 애니메이션이 원형 마스크로 클리핑됨 — No Mask 정지 화면과 달라야 한다
3. [Bubble Mask] 버튼을 탭한다
4. **기대 결과**: 마스크 모양이 버블 형태로 변경됨 — Circle 정지 화면과 달라야 한다
5. [No Mask] 버튼을 탭한다
6. **기대 결과**: 마스크 제거, 원래 사각형으로 표시됨
7. 라벨의 Mask에 GetAlphaMaskUrl이 반환한 경로가 표시되는지 확인 (마스크 없음이면 none)

## 테스트 2: CropToMask

1. 마스크 적용 상태에서 [CropToMask ON] 버튼을 탭한다
2. **기대 결과**: 뷰 크기가 마스크 영역에 맞게 조정됨 — OFF 정지 화면과 달라야 한다
3. [CropToMask OFF] 버튼을 탭한다
4. **기대 결과**: 원래 뷰 크기 유지

## 테스트 3: MaskingMode

1. [Circle Mask] + [CropToMask ON] 상태에서 [MaskingMode ON_RENDERING]을 탭하고 정지 화면을 확인한다
2. [MaskingMode ON_LOADING]을 탭하고 정지 화면을 확인한다
3. **기대 결과**: 두 모드의 정지 화면이 **같아야 한다** — ON_LOADING은 로딩 시 CPU가 마스크를
   구워 텍스처 1장을 만들고, ON_RENDERING은 셰이더가 그릴 때 2장으로 합성한다. 결과 그림이
   같게 나오는 것이 정답이며, CropToMask ON 조합에서 두 코드 경로가 실제로 갈리므로
   한쪽이 깨지면 이 비교가 잡는다.

## 통과 기준

- 마스크 적용 시 마스크 알파 채널 모양대로만 애니메이션이 표시되어야 한다 (Circle vs No Mask,
  Circle vs Bubble의 정지 화면 차이로 판정)
- CropToMask ON: 뷰가 마스크 크기에 맞게 크롭되어야 한다 (ON vs OFF 차이로 판정)
- MaskingMode: CropToMask ON 상태에서 ON_RENDERING과 ON_LOADING의 결과가 같아야 한다
- GetAlphaMaskUrl / IsCropToMask / GetMaskingMode 반환값이 설정한 값과 일치해야 한다
  (Mask 라벨은 getter 반환값을 그대로 출력한다)
