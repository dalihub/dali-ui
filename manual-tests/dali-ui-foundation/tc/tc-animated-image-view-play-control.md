# AnimatedImageView: Play / Pause / Stop / JumpToFrame

Play, Pause, Stop, JumpToFrame 제어와 GetPlayState, GetCurrentFrame, GetTotalFrame, AnimationFinishedSignal 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: State / Frame / TotalFrame / Finished 카운트
- 버튼 행: Play / Pause / Stop / Jump->5

## 테스트 1: 재생 제어

1. [Play] 버튼을 탭한다
2. **기대 결과**: State: PLAYING, 프레임 카운터 증가
3. [Pause] 버튼을 탭한다
4. **기대 결과**: State: PAUSED, 프레임 카운터 정지 (값이 일정 시간 유지되는지 확인)
5. [Play] 버튼을 탭한다
6. **기대 결과**: State: PLAYING, 프레임 카운터 재개

## 테스트 2: JumpToFrame

1. [Pause] 버튼을 탭해 일시정지한다
2. [Jump->5] 버튼을 탭한다
3. **기대 결과**: State: PAUSED 유지, Frame: 5 — JumpToFrame은 표시 프레임만 옮기고
   재생 상태는 바꾸지 않는다

## 테스트 3: Stop

1. [Play] 후 [Stop] 버튼을 탭한다
2. **기대 결과**: State: STOPPED. **AnimationFinishedSignal 카운트는 변하지 않는다** —
   완료 신호는 루프를 다 돈 자연 종료에서만 발생한다("애니메이션이 끝났다"와 "사용자가
   껐다"의 구분). 자연 종료 신호는 LoopCount / StopBehavior TC에서 검증한다

## 통과 기준

- 각 버튼 탭 후 State/Frame 라벨이 즉시 올바른 값으로 업데이트되어야 한다
- 명시적 Stop은 AnimationFinishedSignal을 발생시키지 않아야 한다 (자연 종료 전용 신호)
- Pause 상태의 JumpToFrame은 재생 상태를 유지한 채 프레임만 이동해야 한다
