# ImageView: ImageColor / PixelArea

SetImageColor 틴트 효과와 SetPixelArea 영역 클리핑을 확인한다.

정지 이미지 화면이라 노이즈 바닥값이 0이다 — 캡처 비교가 정지 컨트롤 없이 그대로 성립한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200) — 접근성 이름 **`ImagePreview`**, 역할 IMAGE.
  픽셀 비교 영역은 좌표 하드코딩 대신 이 이름으로 선택한다 (좌표를 박아두면 레이아웃이
  바뀌었을 때 빨개지는 게 아니라 조용히 엉뚱한 자리를 비교하게 된다)
- 상태 라벨: 현재 Color 및 PixelArea 정보 — 둘 다 진짜 getter다
  (`GetImageColor()`, `GetPixelArea()`)
- 버튼 행 1: White / Red / Green / Blue / 50% Alpha
- 버튼 행 2: PA: Full / PA: TopLeft / PA: Center

## 테스트 1: ImageColor 틴트

1. 진입 직후 프리뷰를 캡처해 둔다 (White 기준)
2. [Red] → [Green] → [Blue] → [50% Alpha] 순서로 탭하며, 각 탭 직후 라벨을 한 번 읽고
   프리뷰를 캡처한다
3. **기대 결과**: 라벨이 각 설정값 그대로 (`R255 G0 B0 A255`, …, 50% Alpha는 **A127** —
   `(int)(0.5f×255)` 버림)
4. **기대 결과**: 캡처 5장(White 포함)이 **서로 전부 달라야** 한다
5. [White] 버튼을 탭한다
6. **기대 결과**: 캡처가 처음 White 캡처와 **같아야** 한다 (복원 — 왕복이 제자리로)

## 테스트 2: PixelArea 클리핑

1. [PA: TopLeft] 버튼을 탭한다 — 좌상단 1/4만 표시, 캡처
2. [PA: Center] 버튼을 탭한다 — 중앙 영역만 표시, 캡처
3. **기대 결과**: Full/TopLeft/Center 캡처가 **서로 전부 달라야** 한다 — TopLeft↔Center
   교차 비교가 없으면 "전체냐 아니냐"만 구분하는 구현도 통과한다
4. [PA: Full] 버튼을 탭한다
5. **기대 결과**: 처음 Full 캡처와 **같아야** 한다 (복원)

## 통과 기준

- GetImageColor / GetPixelArea 반환값이 설정한 값과 일치해야 한다 (라벨이 getter를 출력)
- 색상 버튼별 캡처가 서로 전부 다르고, White 복원이 처음과 같아야 한다
- PixelArea 3종 캡처가 서로 전부 다르고, Full 복원이 처음과 같아야 한다
- 픽셀 비교 영역은 `ImagePreview` 접근성 이름으로 선택되어야 한다

("즉시 적용"이라는 지연 주장은 재시도하는 단언으로 잴 수 없어, 탭 직후 단일 읽기/캡처로
값과 픽셀만 확인한다.)
