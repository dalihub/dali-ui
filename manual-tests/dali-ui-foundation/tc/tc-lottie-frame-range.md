# Lottie: SetMinMaxFrame / FrameSpeedFactor

SetMinMaxFrame / SetMinMaxFrameByMarker로 재생 프레임 범위를 제한하고, SetFrameSpeedFactor / GetFrameSpeedFactor로 속도를 조절하는지 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: Min/Max Frame / Current Frame / Total Frame / Speed
- 버튼 행 1: Play / Stop
- 버튼 행 2: First Half / Second Half / First 10 / Full
- 버튼 행 3: Speed 0.25x / Speed 1.0x / Speed 2.0x

## 테스트 1: SetMinMaxFrame

1. [Play] 버튼을 탭하여 전체 프레임 범위 재생을 확인한다
2. [First Half] 버튼을 탭한다
3. **기대 결과**: 프레임 카운터가 전반부에서만 루프됨
4. [Second Half] 버튼을 탭한다
5. **기대 결과**: 후반부 프레임만 재생됨
6. [First 10] 버튼을 탭한다
7. **기대 결과**: 프레임 0~10만 반복됨
8. [Full] 버튼을 탭한다
9. **기대 결과**: 전체 범위 복원

## 테스트 2: FrameSpeedFactor

1. [Speed 0.25x] 버튼을 탭한다
2. **기대 결과**: 느린 재생
3. [Speed 2.0x] 버튼을 탭한다
4. **기대 결과**: 빠른 재생

## 통과 기준

- SetMinMaxFrame 후 프레임 카운터가 지정된 범위 내에서만 변경되어야 한다
- FrameSpeedFactor 변경 시 재생 속도가 시각적으로 변해야 한다