# ImageView: ImageColor / PixelArea

SetImageColor 틴트 효과와 SetPixelArea 영역 클리핑을 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200)
- 상태 라벨: 현재 Color 및 PixelArea 정보
- 버튼 행 1: White / Red / Green / Blue / 50% Alpha
- 버튼 행 2: PA: Full / PA: TopLeft / PA: Center

## 테스트 1: ImageColor 틴트

1. [Red] 버튼을 탭한다
2. **기대 결과**: 이미지에 빨간색 틴트가 적용됨, 상태 라벨에 RGBA 값 표시
3. [Green] 버튼을 탭한다
4. **기대 결과**: 초록색 틴트 적용
5. [50% Alpha] 버튼을 탭한다
6. **기대 결과**: 반투명 상태로 표시
7. [White] 버튼을 탭한다
8. **기대 결과**: 원래 상태로 복원

## 테스트 2: PixelArea 클리핑

1. [PA: TopLeft] 버튼을 탭한다
2. **기대 결과**: 이미지의 좌상단 1/4 영역만 표시됨
3. [PA: Center] 버튼을 탭한다
4. **기대 결과**: 이미지의 중앙 영역만 표시됨
5. [PA: Full] 버튼을 탭한다
6. **기대 결과**: 전체 이미지가 다시 표시됨

## 통과 기준

- 각 색상 버튼 탭 후 해당 틴트가 즉시 적용되어야 한다
- PixelArea 변경 시 지정된 영역만 표시되어야 한다
- GetImageColor / GetPixelArea 반환값이 설정한 값과 일치해야 한다