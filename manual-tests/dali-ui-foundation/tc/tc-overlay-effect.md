# OverlayEffect

InteractiveView에 기본 state effect로 적용되는 OverlayEffect가 Pressed/FocusIndicated 상태와 state effect target 설정을 올바르게 반영하는지 확인한다.

## 화면 구성

- `FOCUS INDICATED`: navigation/key 입력으로 focus indication을 받는 카드
- `PRESS ONLY`: 누르는 동안에만 Pressed overlay를 표시하는 카드
- `STATE EFFECT TARGET`: owner가 터치를 받되 overlay는 안쪽 target에 표시되는 카드
- `ROUND`: `OverlayEffect::Round()` preset을 적용한 카드
- `DISABLED TARGET`: disabled 상태에서도 overlay/pressed 효과가 들어가지 않는지 확인하는 카드
- `TOGGLE DISABLED`: `DISABLED TARGET`의 enabled 상태를 토글하는 카드

## 테스트 1: Focus indicated 카드

1. navigation/key 입력으로 `FOCUS INDICATED` 카드에 focus indication을 이동한다.
2. **기대 결과**: 카드가 FocusIndicated 상태가 되어 기본 overlay보다 진한 dim이 유지된다.
3. 같은 카드를 다시 누른 채로 유지한다.
4. **기대 결과**: FocusIndicated overlay와 Pressed overlay가 함께 적용된 것처럼 더 어둡게 보인다.
5. 손을 뗀다.
6. **기대 결과**: Pressed overlay만 사라지고 FocusIndicated overlay는 유지된다.

## 테스트 2: Press only 카드

1. `PRESS ONLY` 카드를 누른 채로 유지한다.
2. **기대 결과**: 누르고 있는 동안에만 Pressed overlay가 표시된다.
3. 손을 뗀다.
4. **기대 결과**: FocusIndicated 상태가 없으므로 overlay가 완전히 사라진다.


## 테스트 3: State effect target 카드

1. `STATE EFFECT TARGET` 카드를 누른 채로 유지한다.
2. **기대 결과**: 안쪽 주황색 target 영역에만 Pressed overlay가 표시된다.
3. 손을 뗀다.
4. **기대 결과**: target overlay가 완전히 사라진다.

## 테스트 4: Round preset 카드

1. `ROUND` 카드를 누른 채로 유지한다.
2. **기대 결과**: target view의 corner radius와 관계없이 relative `0.5` radius overlay가 표시된다.
3. 손을 뗀다.
4. **기대 결과**: Pressed overlay가 사라진다.

## 테스트 5: Disabled 카드

1. `DISABLED TARGET` 카드를 누른 채로 유지한다.
2. **기대 결과**: enabled 상태이므로 Pressed overlay와 recoil 효과가 표시된다.
3. 손을 뗀 뒤 `TOGGLE DISABLED` 카드를 탭한다.
4. **기대 결과**: `DISABLED TARGET`이 disabled 상태가 되며 opacity가 낮아져 흐리게 보인다.
5. disabled 상태의 `DISABLED TARGET`을 누른 채로 유지한다.
6. **기대 결과**: Pressed overlay, FocusIndicated overlay, recoil 효과가 표시되지 않는다.
7. `TOGGLE DISABLED` 카드를 다시 탭한다.
8. **기대 결과**: `DISABLED TARGET`이 enabled 상태로 돌아오며 opacity가 원래대로 복구되고, 다시 누르면 Pressed overlay와 recoil 효과가 표시된다.

## 통과 기준

- Pressed와 FocusIndicated가 동시에 적용될 때 overlay가 중첩된 것처럼 더 어둡게 보여야 한다.
- focus indication이 없는 카드는 release 후 overlay가 남지 않아야 한다.
- state effect target이 설정된 카드는 overlay가 owner가 아니라 target view에만 표시되어야 한다.
- `OverlayEffect::Round()`는 항상 relative `0.5` corner radius를 사용해야 한다.
- disabled 상태의 카드는 흐리게 표시되고, 누르거나 focus를 시도해도 overlay/recoil 효과가 표시되지 않아야 한다.
