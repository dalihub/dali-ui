# 43. View Shadow

Shadow animation과 corner radius 변경에 따른 shadow 렌더링을 눈으로 확인한다.

애니메이션 주기: blur+opacity 카드 **편도 1.4초 / 왕복 2.8초**, radius 카드 **편도 1.6초 /
왕복 3.2초** (둘 다 AUTO_REVERSE). 사람이 볼 때는 각각 한 왕복 이상 지켜볼 것.

이 화면은 **애니메이션의 양 끝점을 정적 카드가 스스로 제공한다** — `RADIUS 0`과
`RADIUS 42`가 `ANIMATED radius`의 두 극값 기준 프레임이다.

## Steps

1. Manual Tests에서 `43. View Shadow`를 연다.
2. 위쪽 왼쪽 카드의 shadow가 주기적으로 흐려지고 진해지는지 확인한다 (2.8초 왕복).
3. 위쪽 오른쪽 static shadow 카드와 비교한다 — 이쪽은 전혀 변하지 않아야 한다.
4. 가운데 `RADIUS 0` / `RADIUS 42` 카드의 multi shadow가 각 corner radius를 따라 렌더되는지 확인한다.
5. 아래쪽 `ANIMATED radius` 카드의 multi shadow가 corner radius animation을 따라가는지 확인한다 —
   양 극단에서 가운데 두 static 카드 각각과 같은 모양이 되는지 본다.

## Expected Result

- 위쪽 왼쪽 카드의 first shadow blur radius가 0에서 큰 값으로 왕복한다.
  (자동 판정: 카드 아래 띠에서 그림자 도달 거리가 **33 ↔ 47px** 왕복 — 실측)
- 위쪽 왼쪽 카드의 first shadow opacity가 낮은 값에서 높은 값으로 왕복한다.
  (자동 판정: 가장 짙은 지점 밝기가 **215 ↔ 61** 왕복 — blur만 커지면 퍼지며 옅어져
  최저값이 올라가야 하므로, 같은 방향으로 움직이는 것이 opacity가 따로 오른다는 증거)
- 위쪽 오른쪽 카드는 고정 shadow 상태를 유지한다. (자동 판정: 애니메이션 카드와 같은 표본
  집합에서 16회 전부 동일 — 실측)
- 가운데 static 카드들은 서로 다른 corner radius에 맞는 shadow shape을 보여준다.
  (자동 판정: 16px 좌측 띠가 서로 다름 + 좌하단 50x50 모서리 그림자 픽셀 수가 다름 —
  실측 radius 0: 파랑 416/주황 816 vs radius 42: 파랑 359/주황 530. "각각이 정확히 자기
  radius인가"는 승인 그림이 필요해 육안 몫으로 남는다)
- 아래쪽 animated 카드의 shadow shape은 corner radius animation을 따라 왕복한다.
  (자동 판정: 애니메이션 표본의 극값이 **`RADIUS 0` · `RADIUS 42` 카드 각각과 일치** —
  실측 0.65초 표본 = radius 42 카드, 1.28초 표본 = radius 0 카드. "뭔가 움직인다"가 아니라
  "정해진 두 상태를 오간다"가 기준이다)
