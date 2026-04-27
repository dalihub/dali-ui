# AnimatedImageView: FrameSpeedFactor / FrameDelay

SetFrameSpeedFactor / GetFrameSpeedFactor, SetFrameDelay / GetFrameDelay 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: Speed / Delay 정보
- 버튼 행 1: Play / Stop
- 버튼 행 2: Speed 0.25x / Speed 1.0x / Speed 4.0x
- 버튼 행 3: Delay 0ms / Delay 100ms / Delay 500ms

## 테스트 1: FrameSpeedFactor

1. [Play] 버튼을 탭하여 기본 속도(1.0x)를 확인한다
2. [Speed 0.25x] 버튼을 탭한다
3. **기대 결과**: 매우 느린 재생
4. [Speed 4.0x] 버튼을 탭한다
5. **기대 결과**: 매우 빠른 재생
6. GetFrameSpeedFactor 반환값이 라벨에 표시되는지 확인

## 테스트 2: FrameDelay

1. [Delay 0ms] 버튼을 탭한다
2. **기대 결과**: 최대한 빠르게 재생
3. [Delay 500ms] 버튼을 탭한다
4. **기대 결과**: 각 프레임이 0.5초간 표시됨

## 통과 기준

- Speed 버튼 클릭 후 애니메이션 속도가 시각적으로 변해야 한다
- Delay 버튼 클릭 후 프레임 전환 간격이 변해야 한다
- GetFrameSpeedFactor / GetFrameDelay 반환값이 설정한 값과 일치해야 한다