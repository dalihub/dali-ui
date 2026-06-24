# View Shadow

Shadow animation과 corner radius 변경에 따른 shadow 렌더링을 눈으로 확인한다.

## Steps

1. Manual Tests에서 `View Shadow`를 연다.
2. 위쪽 왼쪽 카드의 shadow가 주기적으로 흐려지고 진해지는지 확인한다.
3. 위쪽 오른쪽 static shadow 카드와 비교한다.
4. 가운데 `RADIUS 0` / `RADIUS 42` 카드의 multi shadow가 각 corner radius를 따라 렌더되는지 확인한다.
5. 아래쪽 `ANIMATED radius` 카드의 multi shadow가 corner radius animation을 따라가는지 확인한다.

## Expected Result

- 위쪽 왼쪽 카드의 first shadow blur radius가 0에서 큰 값으로 왕복한다.
- 위쪽 왼쪽 카드의 first shadow opacity가 낮은 값에서 높은 값으로 왕복한다.
- 위쪽 오른쪽 카드는 고정 shadow 상태를 유지한다.
- 가운데 static 카드들은 서로 다른 corner radius에 맞는 shadow shape을 보여준다.
- 아래쪽 animated 카드의 shadow shape은 corner radius animation을 따라 왕복한다.
