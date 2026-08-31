# Lottie: LoopCount / LoopingMode / StopBehavior

SetLoopCount / GetLoopCount, SetLoopingMode / GetLoopingMode, SetStopBehavior /
GetStopBehavior, AnimationFinishedSignal 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240) — 진입 시 자동 재생하지 않음, [Play]로 시작
- 상태 라벨 (2줄):
  `Loop: X | LoopMode: Y | Idx: n/N` /
  `Stop: Z | Frame: n/N | Finished: C`
  - `Loop`/`LoopMode`/`Stop`은 getter, `Frame`은 `GetCurrentFrame()/GetTotalFrame()` (100ms 폴)
  - `Idx: n/N`은 화면이 프레임 흐름에서 계산한 **현재 루프 번호**(1부터) —
    RESTART는 큰 폭의 역방향 점프(랩), AUTO_REVERSE는 하강→상승 전환(밸리)을 센다.
    [Play]는 정지 상태에서 누르면 인덱스를 0으로 리셋한다
- 버튼 행 1: Play / Stop
- 버튼 행 2: Loop -1(무한) / Loop 1 / Loop 2 / Loop 3
- 버튼 행 3: LoopMode RESTART / LoopMode AUTO_REVERSE
- 버튼 행 4: Stop CURRENT / Stop FIRST / Stop LAST

## 테스트 1: LoopCount 제한

1. [Loop: 2] 버튼을 탭한다
2. [Play] 버튼을 탭한다
3. **기대 결과**: `Idx`가 1 → 2를 거치고, 2루프가 끝나면 `Finished` 카운트가 +1
   (실측: RESTART 2루프 ≈ 2.1초, 종료 후 `Frame: 63/64`에 정지 — CURRENT 기본값)
4. [Loop: -1] 버튼을 탭하고 [Play] 버튼을 탭한다
5. **기대 결과**: `Idx: n/inf`가 계속 증가하고 `Finished`는 움직이지 않는다
   (유한 시간 관측이므로 "영원히"가 아니라 "관측 창 동안"의 주장이다)

## 테스트 2: LoopingMode

1. [Loop: 2] 상태에서 [LoopMode: AUTO_REVERSE] 버튼을 탭하고 [Play] 버튼을 탭한다
2. **기대 결과**: `Frame` 값의 흐름이 상승 → 하강으로 **방향을 바꾼다** (핑퐁).
   AUTO_REVERSE의 1루프는 왕복 1회다 (실측: 2루프 ≈ 4초, 종료 시 `Frame: 0/64`)
3. [LoopMode: RESTART] 버튼을 탭하고 [Play] 버튼을 탭한다
4. **기대 결과**: `Frame` 흐름이 상승만 반복하고 매 루프 시작점으로 점프한다

## 테스트 3: StopBehavior

1. [Stop: LAST] 버튼을 탭한다
2. **기대 결과**: 라벨이 `Stop: LAST_FRAME` (기본값 CURRENT_FRAME에서 벗어남 — getter 왕복)
3. [Play] → (재생 중) [Stop] 버튼을 탭한다
4. **기대 결과**: `Frame`이 마지막 프레임에서 정지하고 그 값이 유지된다

> `Stop: CURRENT`(기본값)는 **사양상 정지 시점의 임의 프레임**에 머무므로 특정 값을
> 기대하면 안 된다 — "정지 후 값이 더 변하지 않는다"까지만 검증한다.
> `Stop: FIRST`/`LAST`는 프레임 캐시가 켜진 조합에서 드물게 비주얼을 영구히 얼리는
> 별개의 결함이 조사돼 있다 (AnimatedImageView 쪽 9번 리뷰와 동일 사안).

## 검증 방법

`Frame`/`Idx`를 짧은 간격으로 여러 번 읽어 값의 흐름으로 판정한다 — 방향 전환(AUTO_REVERSE),
랩(RESTART), 루프 수(Idx), 정지(값 유지)가 전부 같은 샘플에서 나온다.

## 통과 기준

- LoopCount=2: `Idx`가 2까지만 진행하고, 2루프 종료 시 AnimationFinishedSignal이 발생해야 한다
- AUTO_REVERSE: `Frame` 진행 방향이 루프 경계에서 반전되어야 한다 (핑퐁)
- StopBehavior: getter 왕복이 성립하고, [Stop] 후 `Frame` 값이 유지되어야 한다
  (LAST는 마지막 프레임, CURRENT는 임의 프레임 — 값 고정 기대는 LAST/FIRST만)
