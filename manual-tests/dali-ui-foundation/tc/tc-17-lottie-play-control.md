# 17. Lottie: Play / Pause / Stop / JumpToFrame

Play, Pause, Stop, JumpToFrame 제어와 GetPlayState, GetCurrentFrame, GetTotalFrame,
AnimationFinishedSignal, ResourceReadySignal 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240) — 진입 시 자동 재생하지 않음
- 상태 라벨: State / Frame / TotalFrame / Finished / ResourceReady 카운트
- 버튼 행: Play / Pause / Stop / Jump→10

## 테스트 1: ResourceReadySignal (화면 진입 직후 — 다른 조작 전에 확인)

1. 화면에 진입해 아무 버튼도 누르지 않은 상태에서 라벨을 확인한다
2. **기대 결과**: `ResourceReady: 1 | AnimationFinished: 0` — 로딩 완료 시 정확히 1회

## 테스트 2: 재생 제어

1. [Play] 버튼을 탭한다
2. **기대 결과**: `State: PLAYING`, 프레임 카운터가 계속 변한다
3. [Pause] 버튼을 탭한다
4. **기대 결과**: `State: PAUSED`, 프레임 카운터가 멈추고 그 값이 유지된다
5. [Play] 버튼을 탭한다
6. **기대 결과**: `State: PLAYING`, 카운터가 다시 변하기 시작한다

## 테스트 3: JumpToFrame (일시정지 상태에서)

1. [Pause] 버튼을 탭한다 — 재생 중 점프는 값이 즉시 흘러가 라벨로 확인할 수 없으므로,
   멈춘 상태에서 점프해 도착 프레임을 못 박는다 (`SetCurrentFrameNumber`는 양쪽 상태에서 적용된다)
2. [Jump→10] 버튼을 탭한다
3. **기대 결과**: 라벨 전체가 `State: PAUSED | Frame: 10/64`이고 그 값이 유지된다

## 테스트 4: Stop

1. [Play] 후 [Stop] 버튼을 탭한다
2. **기대 결과**: `State: STOPPED`, 프레임은 기본 StopBehavior(CURRENT_FRAME)대로 정지
   시점 값에 머문다
3. **기대 결과**: `AnimationFinished` 카운트는 **움직이지 않는다** — 이 신호는 애니메이션이
   **저절로 끝났을 때**(유한 LoopCount 소진) 발생하는 신호다. 명시적 `Stop()`은 완료가
   아니라 중단이므로 신호를 쏘지 않는다 (AnimatedImageView와 공통 계약; 자연 종료 시
   발생하는 것은 `tc-15-lottie-loop.md` 테스트 1이 확인한다)

## 통과 기준

- ResourceReadySignal이 로딩 완료 시 1회 발생해야 한다 (진입 직후 확인)
- 각 버튼 탭 후 State 라벨이 올바른 값이어야 한다 ("즉시"라는 지연 주장은 잴 수 없어
  값 자체만 확인; Pause의 "멈춤"은 값이 일정 시간 유지되는 것으로 판정)
- 일시정지 상태의 JumpToFrame이 지정 프레임에 도착해 유지되어야 한다
- 명시적 Stop은 상태를 STOPPED로 바꾸되 AnimationFinishedSignal을 발생시키지 않아야 한다
