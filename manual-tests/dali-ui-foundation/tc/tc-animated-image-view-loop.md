# AnimatedImageView: LoopCount / StopBehavior

SetLoopCount / GetLoopCount, SetStopBehavior / GetStopBehavior, AnimationFinishedSignal 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: LoopCount / StopBehavior / Loop 인덱스 / Finished 카운트
  (`Loop: n/N`은 재생 시작 후 몇 바퀴째인지 — 프레임 wrap을 세어 표시)
- 버튼 행 1: Play / Stop
- 버튼 행 2: Loop -1(무한) / Loop 1 / Loop 2 / Loop 5
- 버튼 행 3: Stop CURRENT / Stop FIRST / Stop LAST

## 테스트 1: LoopCount 제한

1. [Loop: 2] 버튼을 탭한다
2. [Play] 버튼을 탭한다
3. **기대 결과**: 2루프 후 자동 정지, AnimationFinishedSignal 카운트 = 1,
   라벨이 `Loop: 2/2`에서 멈춤 (1바퀴도 3바퀴도 아님)
4. [Loop: -1] 버튼을 탭한다
5. [Play] 버튼을 탭한다
6. **기대 결과**: 무한 반복 — Loop 인덱스가 계속 증가하고 Finished는 그대로

## 테스트 2: StopBehavior

1. [Stop: FIRST] 버튼을 탭한다
2. [Play] -> [Stop] 버튼을 탭한다
3. **기대 결과**: 첫 번째 프레임에서 정지 — 언제 Stop을 눌러도 같은 그림
4. [Stop: LAST] 버튼을 탭한다
5. [Play] -> [Stop] 버튼을 탭한다
6. **기대 결과**: 마지막 프레임에서 정지 — FIRST의 정지 화면과 다른 그림
7. [Stop: CURRENT] 상태에서 [Play] -> [Stop]을 누른다
8. **기대 결과**: **마지막으로 그린 프레임에 그대로 머문다** — 정지 프레임은 Stop을 누른
   타이밍을 따라가므로 매번 달라도 정상이다. 고정된 프레임을 기대하면 안 된다 (사양)

> **즉시 점프 의미**: FIRST/LAST의 Stop은 그 프레임으로 **즉시 점프**한다 — 재생이 거기까지
> 진행되기를 기다리지 않는다. 대상 프레임이 아직 읽히는 중이면 읽기가 끝나는 대로(한두
> 프레임 지연) 표시된다.

## 통과 기준

- LoopCount=2: 정확히 2루프 후 AnimationFinishedSignal이 발생해야 한다 (`Loop: 2/2` +
  Finished: 1로 판정 — 루프 인덱스가 없으면 1바퀴와 3바퀴를 구분할 수 없다)
- StopBehavior FIRST/LAST: Stop 후 표시되는 프레임이 설정한 값과 일치해야 한다
- StopBehavior CURRENT: 정지 프레임은 탭 타이밍에 따라 달라진다 — 이것이 사양이다
