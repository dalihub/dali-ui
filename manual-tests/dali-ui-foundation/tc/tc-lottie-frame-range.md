# Lottie: SetMinMaxFrame / FrameSpeedFactor

SetMinMaxFrame / SetMinMaxFrameByMarker로 재생 프레임 범위를 제한하고, SetFrameSpeedFactor /
GetFrameSpeedFactor로 속도를 조절하는지 확인한다.

구간과 속도는 "무엇을 그릴지"가 아니라 **"어떻게 재생할지"**다. 두 setter 모두 재생 중인
비주얼에 제자리로 적용되며 **재생을 중단하지 않는다** (lottie-android가 setMinFrame /
setMaxFrame / setSpeed를 애니메이터 파라미터로 다루는 것과 같은 방식).

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: `Frame: n/N | Range: min-max | Speed: x.xxxxxx`
  - `Frame`은 `GetCurrentFrame()/GetTotalFrame()`, `Range`는 `GetMinMaxFrame()`,
    `Speed`는 `GetFrameSpeedFactor()` — 셋 다 진짜 getter다
  - 범위를 아직 설정하지 않았으면 `Range`는 컴포지션 전체 `0-N`을 표시한다
- 버튼 행 1: Play / Stop
- 버튼 행 2: Full / First Half / Second Half / First 10
- 버튼 행 3: Speed 0.25x / Speed 0.5x / Speed 1.0x / Speed 2.0x

## 테스트 1: SetMinMaxFrame (재생 유지 + 범위 준수)

1. [Play] 버튼을 탭한다 — 프레임 카운터가 계속 변한다
2. [Second Half] 버튼을 탭한다
3. **기대 결과**: 재생이 멈추지 않고 계속된다 (탭 후에도 카운터가 계속 변함)
4. **기대 결과**: `Range: 32-64` (getter 반환값), 이후 관측되는 **모든** `Frame` 값이 [32, 64] 안
5. [First 10] 버튼을 탭한다
6. **기대 결과**: 재생이 유지되고 관측값이 [0, 10] 범위로 이동, `Range: 0-10`
7. [Full] 버튼을 탭한다
8. **기대 결과**: `Range: 0-64`, 전체 범위 재생 복원

## 테스트 2: FrameSpeedFactor (재생 유지 + 진행량 차이)

1. 재생 중 [Speed 0.25x] 버튼을 탭한다
2. **기대 결과**: 재생이 멈추지 않고 계속되며 `Speed: 0.250000`
3. 같은 길이의 시간 창에서 프레임 진행량을 기록한다
4. [Speed 2.0x] 버튼을 탭하고 같은 길이의 시간 창에서 진행량을 기록한다
5. **기대 결과**: 진행량이 0.25x < 1.0x < 2.0x 순서 (정확한 배율까지는 요구하지 않음 —
   관측 주기가 프레임 간격보다 길어 배율 자체는 측정 오차에 묻힌다)

## 검증 방법

프레임 카운터를 **시간축으로** 본다: 라벨을 짧은 간격으로 여러 번 읽어 값의 흐름을 모으면
"멈추지 않았다"(서로 다른 값 여러 개) / "범위 안"(모든 값이 [min, max]) / "더 빠르다"(같은
창의 진행량 비교)가 전부 그 값들로 판정된다.

## 통과 기준

- 재생 중 SetMinMaxFrame / SetFrameSpeedFactor가 **재생을 중단하지 않아야 한다**
- SetMinMaxFrame 후 관측되는 모든 프레임 값이 지정한 범위 안이어야 한다
- `Range` / `Speed` 라벨은 getter 반환값이어야 하며 설정값과 일치해야 한다
- 속도 배율에 따라 같은 시간 동안의 진행량이 뚜렷하게 달라져야 한다
