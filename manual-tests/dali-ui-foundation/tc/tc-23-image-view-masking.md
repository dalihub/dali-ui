# 23. ImageView Alpha Masking

AlphaMaskUrl, CropToMask, MaskingMode(ON_RENDERING / ON_LOADING) 동작을 확인한다.

이 화면은 대조군이 내장돼 있다 — 좌측 프리뷰는 CropToMask OFF, 우측은 ON으로 **고정**이고
같은 마스크가 양쪽에 걸린다. "마스크 없으면 좌우가 같고, 걸면 다르다"가 크롭 동작의
직접 증거다.

## 화면 구성

- 좌측: CropToMask OFF 이미지 프리뷰 (200x200) — 접근성 이름 `PreviewCropOff`
- 우측: CropToMask ON 이미지 프리뷰 (200x200) — 접근성 이름 `PreviewCropOn`
- 상태 라벨: `Mask: <파일명|none> | MaskingMode: … | CropL: … | CropR: …` — 네 필드 전부
  진짜 getter (`GetAlphaMaskUrl()`의 파일명 / `GetMaskingMode()` / `IsCropToMask()` 좌·우)
- 버튼 행 1: Circle Mask / Bubble Mask / No Mask
- 버튼 행 2: MaskingMode ON_RENDERING / MaskingMode ON_LOADING

## 테스트 1: AlphaMask 적용

1. [Circle Mask] 버튼을 탭한다
2. **기대 결과**: 라벨이 `Mask: contact-cards-mask.png` (getter의 파일명), 좌측 프리뷰가
   마스크 적용 전과 달라짐 (마스크가 픽셀까지 도달)
3. [Bubble Mask] 버튼을 탭한다
4. **기대 결과**: 라벨이 `Mask: mask.png`, 렌더가 circle 상태와 달라짐
5. [No Mask] 버튼을 탭한다
6. **기대 결과**: 라벨이 `Mask: none`, 좌측이 처음의 무마스크 프레임과 **정확히 같게** 복원

## 테스트 2: CropToMask 비교 (좌우 동시)

1. 마스크 없는 상태에서 좌우 프리뷰가 **같은지** 확인한다 (대조군 — 실측 0px)
2. [Circle Mask]를 걸고 좌우를 비교한다
3. **기대 결과**: 좌(OFF)와 우(ON)가 **달라야** 한다 — 마스크(128x128)가 프리뷰(200x200)보다
   작아 크롭이 걸리면 크기가 줄어 확실히 갈린다 (실측 27,607px 차이)
4. **기대 결과**: 라벨 `CropL: OFF | CropR: ON` (getter 왕복)

## 테스트 3: MaskingMode 비교 — 두 모드는 "같아야" 한다

ON_LOADING과 ON_RENDERING은 **언제 합성하느냐**의 차이지 무엇을 합성하느냐가 아니다.
두 경로가 같은 그림을 내는 것이 정상이다.

1. [Circle Mask] 상태에서 우측 프리뷰를 캡처한다 (ON_RENDERING)
2. [MaskingMode ON_LOADING] 버튼을 탭하고 우측을 다시 캡처한다
3. **기대 결과**: 라벨 `MaskingMode: ON_LOADING` (getter), 두 캡처가 **같아야** 한다
   (실측 3px·최대 편차 8 — 미세한 AA 노이즈뿐)

## 통과 기준

- 마스크 적용 시 렌더가 실제로 바뀌고, 어느 마스크인지 라벨(getter 파일명)로 판정돼야 한다
- CropToMask: 마스크 없으면 좌우 동일, 걸면 좌우 상이 — 해제 시 처음으로 정확히 복원
- MaskingMode: 두 모드의 결과 그림이 같아야 하고, `GetMaskingMode()` 왕복이 성립해야 한다
- GetAlphaMaskUrl / IsCropToMask / GetMaskingMode 반환값이 설정한 값과 일치해야 한다
