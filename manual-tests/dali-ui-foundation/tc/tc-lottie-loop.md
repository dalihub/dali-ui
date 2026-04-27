# Lottie: LoopCount / LoopingMode / StopBehavior

SetLoopCount / GetLoopCount, SetLoopingMode / GetLoopingMode, SetStopBehavior / GetStopBehavior, AnimationFinishedSignal 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: LoopCount / LoopingMode / StopBehavior / Finished 카운트
- 버튼 행 1: Play / Stop
- 버튼 행 2: Loop -1(무한) / Loop 1 / Loop 2 / Loop 3
- 버튼 행 3: LoopMode RESTART / LoopMode AUTO_REVERSE
- 버튼 행 4: Stop CURRENT / Stop FIRST / Stop LAST

## 테스트 1: LoopCount 제한

1. [Loop: 2] 버튼을 탭한다
2. [Play] 버튼을 탭한다
3. **기대 결과**: 2루프 후 자동 정지, AnimationFinishedSignal 카운트 = 1
4. [Loop: -1] 버튼을 탭한다
5. [Play] 버튼을 탭한다
6. **기대 결과**: 무한 반복

## 테스트 2: LoopingMode

1. [LoopMode: AUTO_REVERSE] 버튼을 탭한다
2. [Play] 버튼을 탭한다
3. **기대 결과**: 정방향/역방향 교차 재생 (핑퐁)
4. [LoopMode: RESTART] 버튼을 탭한다
5. [Play] 버튼을 탭한다
6. **기대 결과**: 매 루프 시작점에서 재시작

## 테스트 3: StopBehavior

1. [Stop: LAST] 버튼을 탭한다
2. [Play] -> [Stop] 버튼을 탭한다
3. **기대 결과**: 마지막 프레임에서 정지

## 통과 기준

- LoopCount=2: 정확히 2루프 후 AnimationFinishedSignal이 발생해야 한다
- AUTO_REVERSE: 정방향/역방향 핑퐁 재생이어야 한다
- StopBehavior: Stop 후 표시되는 프레임이 설정한 값과 일치해야 한다