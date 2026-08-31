# ImageView: FittingMode

4가지 FittingMode(FIT_KEEP / FILL / OVER_FIT / CENTER)의 시각적 차이를 확인한다.

소재는 **박스(280x160)보다 양축 모두 작은** `people-small-10.jpg`(128x128)다. CENTER는
**박스가 원본보다 클 때만** 원본 크기를 유지하고, 원본이 박스를 넘으면 FIT_KEEP과 같은
min-ratio 축소로 떨어진다(계약 — enum 문서에 명시됨). 큰 소재로는 CENTER와 FIT_KEEP이
비트 단위로 같아져 결함처럼 보인다(2026-08-21 실측, 소재 문제로 판정). 이 소재에서는
네 모드가 전부 갈린다: FIT_KEEP 160x160 / CENTER 128x128 / FILL 280x160 / OVER_FIT
280x280 상하 잘림.

## 화면 구성

- 중앙: 이미지 프리뷰 (280x160, 가로형, 빨간색 테두리) — 접근성 이름 `ImagePreview`
- 상태 라벨: 현재 FittingMode 및 GetFittingMode 일치 여부 (진짜 비교 — `match: OK/NG`)
- 버튼 행: FIT_KEEP / FILL / OVER_FIT / CENTER

## 테스트 1: FittingMode 변경

1. [FIT_KEEP] (기본값) 상태를 확인한다
2. **기대 결과**: 가로세로 비율 유지, 128x128이 160x160으로 확대 + 좌우 레터박스
3. [FILL] 버튼을 탭한다
4. **기대 결과**: 이미지가 박스를 채우도록 늘어남, 가로세로 비율 무시
5. [OVER_FIT] 버튼을 탭한다
6. **기대 결과**: 가로세로 비율 유지, 박스를 덮도록 확대되어 상하가 잘림
7. [CENTER] 버튼을 탭한다
8. **기대 결과**: **원본 128x128 크기 그대로** 중앙 정렬 (박스가 원본보다 크므로)

## 통과 기준

- 네 모드의 캡처가 **서로 전부 달라야** 한다 (여섯 쌍 전부 — CENTER와 FIT_KEEP이 같아지는
  회귀를 정확히 이걸로 잡는다; 골든·승인 불요, 소재가 바뀌어도 유효)
- GetFittingMode() 반환값이 설정한 값과 일치해야 한다 (`match: OK`)
