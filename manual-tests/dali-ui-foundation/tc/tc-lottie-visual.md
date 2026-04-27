# Lottie: SetImageColor / SetDesiredSize

SetImageColor / GetImageColor, SetDesiredWidth / GetDesiredWidth / SetDesiredHeight / GetDesiredHeight 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: ImageColor / DesiredSize 정보
- 버튼 행 1: White / Red / 50% Alpha (ImageColor)
- 버튼 행 2: Desired 50x50 / Desired 0x0

## 테스트 1: ImageColor

1. [Play] 상태에서 원래 색상을 확인한다
2. [Red] 버튼을 탭한다
3. **기대 결과**: 전체 애니메이션에 빨간색 틴트 적용
4. [50% Alpha] 버튼을 탭한다
5. **기대 결과**: 반투명 상태로 표시
6. [White] 버튼을 탭한다
7. **기대 결과**: 틴트 제거, 원래 색상 복원
8. GetImageColor 반환값이 설정한 값과 일치하는지 확인

## 테스트 2: DesiredSize

1. [Desired 50x50] 버튼을 탭한다
2. **기대 결과**: 저해상도로 렌더링되어 흐리게 표시됨
3. [Desired 0x0] 버튼을 탭한다
4. **기대 결과**: 제한 없음, 뷰 크기에 맞춰 렌더링

## 통과 기준

- SetImageColor 후 GetImageColor가 동일한 값을 반환해야 한다
- DesiredSize 50x50 시 이미지가 흐리게 보여야 한다