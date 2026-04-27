# Lottie: Play / Pause / Stop / JumpToFrame

Play, Pause, Stop, JumpToFrame 제어와 GetPlayState, GetCurrentFrame, GetTotalFrame, AnimationFinishedSignal, ResourceReadySignal 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: State / Frame / TotalFrame / Finished / ResourceReady 카운트
- 버튼 행: Play / Pause / Stop / Jump->10

## 테스트 1: 재생 제어

1. [Play] 버튼을 탭한다
2. **기대 결과**: State: PLAYING, 프레임 카운터 증가
3. [Pause] 버튼을 탭한다
4. **기대 결과**: State: PAUSED, 프레임 카운터 정지
5. [Play] 버튼을 탭한다
6. **기대 결과**: State: PLAYING, 프레임 카운터 재개

## 테스트 2: JumpToFrame

1. [Jump->10] 버튼을 탭한다
2. **기대 결과**: 지정된 프레임으로 이동

## 테스트 3: Stop

1. [Play] 후 [Stop] 버튼을 탭한다
2. **기대 결과**: State: STOPPED, AnimationFinishedSignal 카운트 증가

## 테스트 4: ResourceReadySignal

1. **기대 결과**: 로딩 완료 시 ResourceReadySignal이 1회 발생

## 통과 기준

- 각 버튼 탭 후 State/Frame 라벨이 즉시 올바른 값으로 업데이트되어야 한다
- ResourceReadySignal이 로딩 완료 시 1회 발생해야 한다
- AnimationFinishedSignal이 Stop 시 발생해야 한다