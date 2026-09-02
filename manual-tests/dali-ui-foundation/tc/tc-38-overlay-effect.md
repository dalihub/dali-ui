# 38. OverlayEffect

InteractiveView에 기본 state effect로 적용되는 OverlayEffect가 Pressed/FocusIndicated 상태와
state effect target 설정을 올바르게 반영하는지 확인한다.

**판정에 필요한 값** (소스·실측 확정, 2026-08-26):

| 무엇 | 값 |
|---|---|
| 오버레이 합성 공식 | 검정, `α = 1 − (1−a)^n` (n = 활성 상태 수) |
| 이 화면 네 카드의 `a` | **0.4** — 상태 1개면 α=0.40, 2개 겹치면 **α=0.64** |
| `ROUND` 카드의 `a` | **0.1** (Round() 기본값) — 다른 카드의 1/4이라 **훨씬 옅다** |
| 휴지 상태 | **오버레이가 아예 없다** (활성 상태 0이면 통째로 해제; 실측 α=0.00) |
| recoil | **누르는 동안 카드가 0.98배로 줄어든다** (0.1초) |
| disabled | 불투명도가 **0.4배**가 된다 |

α는 픽셀에서 역산한다: `α = 1 − 측정색 ÷ 원색`.

## 화면 구성

- `FOCUS INDICATED`: navigation/key 입력으로 focus indication을 받는 카드
- `PRESS ONLY`: **터치로는 포커스를 받지 않는** 카드 (`SetFocusOnTouchEnabled(false)` —
  focus indication을 안 받는다는 뜻이 아니다; **Tab 키로 가면 FocusIndicated가 되어
  오버레이가 남는다**. 테스트 2는 탭으로만 조작할 것)
- `STATE EFFECT TARGET`: owner가 터치를 받되 overlay는 안쪽 target에 표시되는 카드
- `ROUND`: `OverlayEffect::Round()` preset — a=0.1, relative 0.5 radius
- `DISABLED TARGET`: disabled 상태에서 overlay/pressed 효과가 안 들어가는지 확인하는 카드
- `TOGGLE DISABLED`: `DISABLED TARGET`의 enabled 상태를 토글하는 카드

## 테스트 1: Focus indicated 카드

1. Tab 키로 `FOCUS INDICATED` 카드에 focus indication을 이동한다
2. **기대 결과**: 오버레이가 없다가 **검정 α=0.40**이 생긴다 (휴지엔 오버레이 자체가 없다)
3. 같은 카드를 누른 채로 유지한다 (키보드 Return 홀드 — 터치 DOWN은 focus indication을
   지우므로 이 복합 상태는 키 경로에서만 만들어진다)
4. **기대 결과**: 두 상태가 합성되어 **α가 0.40에서 0.64로** 짙어진다 (`1−(1−0.4)²`)
5. 손을 뗀다
6. **기대 결과**: Pressed 몫만 사라져 α=0.40으로 돌아온다

## 테스트 2: Press only 카드 (탭으로만)

1. `PRESS ONLY` 카드를 **탭으로** 누른 채 유지한다
2. **기대 결과**: 누르고 있는 동안에만 Pressed overlay(α=0.40)와 recoil(0.98배 축소)
3. 손을 뗀다
4. **기대 결과**: 터치로는 포커스를 받지 않는 카드이므로 overlay가 완전히 사라진다 (α=0.00)

## 테스트 3: State effect target 카드

1. `STATE EFFECT TARGET` 카드를 누른 채로 유지한다
2. **기대 결과**: 안쪽 주황 target 영역만 어두워지고(α>0) **카드의 나머지 영역은 원색
   그대로**(α=0.00)
3. 손을 뗀다
4. **기대 결과**: target overlay가 완전히 사라진다

## 테스트 4: Round preset 카드

1. Tab 키로 `ROUND` 카드에 focus indication을 준다 (또는 누른 채 유지)
2. **기대 결과**: 카드가 128x128 정사각형이므로 relative 0.5 radius = **완전한 원** —
   **네 모서리는 안 덮이고**(α=0.00, 실측 원색 그대로) 중앙만 덮인다(**α=0.10** — 다른
   카드의 1/4이라 훨씬 옅다; "안 뜬다"로 오해하지 말 것)

## 테스트 5: Disabled 카드

1. `DISABLED TARGET` 카드를 누른 채로 유지한다
2. **기대 결과**: enabled 상태이므로 Pressed overlay(α=0.40)와 recoil이 표시된다
3. 손을 뗀 뒤 `TOGGLE DISABLED` 카드를 탭한다
4. **기대 결과**: disabled — **불투명도가 0.4배**로 떨어져 흐려진다
5. disabled 상태의 `DISABLED TARGET`을 누른 채로 유지한다
6. **기대 결과**: overlay(α=0.00 유지)·recoil(크기 변화 없음) 모두 나타나지 않는다
7. `TOGGLE DISABLED`를 다시 탭한다
8. **기대 결과**: 원래 불투명도로 복구, 다시 누르면 효과가 돌아온다

## 통과 기준

- Pressed+FocusIndicated 합성: α가 0.40 → **0.64**로 짙어져야 한다
- focus indication 없는 카드(탭 조작)는 release 후 α=0.00이어야 한다
- state effect target 카드: target 영역 α>0 **그리고** 나머지 영역 α=0.00
- Round(): 모서리 α=0.00 · 중앙 α=0.10 (완전한 원)
- disabled: 불투명도 0.4배 · 눌러도 α=0.00 · recoil 없음 — 재활성화로 완전 복구
