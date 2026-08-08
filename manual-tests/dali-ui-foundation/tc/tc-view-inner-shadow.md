# View Inner Shadow

typed `InnerShadow` API가 대칭/방향별 inset과 View corner radius를 올바르게 반영하는지 눈으로 확인한다.

## Steps

1. Manual Tests에서 `View Inner Shadow`를 연다.
2. `EVEN` 카드의 네 변에서 shadow가 같은 깊이로 보이는지 확인한다.
3. `START + TOP` 카드에서 왼쪽과 위쪽 shadow가 오른쪽과 아래쪽보다 더 깊게 보이는지 확인한다.
4. `END + BOTTOM` 카드에서 오른쪽과 아래쪽 shadow가 왼쪽과 위쪽보다 더 깊게 보이는지 확인한다.
5. `ANIMATED corner radius` 카드의 모서리가 직각과 둥근 형태 사이를 왕복할 때 inner shadow도 모서리를 따라가는지 확인한다.

## Expected Result

- `EVEN` 카드는 네 방향이 균일한 inner shadow를 보여준다.
- 방향별 inset 카드들은 큰 inset을 지정한 변에서 shadow가 더 안쪽까지 보인다.
- 모든 inner shadow는 View 바깥으로 그려지지 않는다.
- animated 카드의 inner shadow 모서리는 View corner radius animation과 분리되거나 잘리지 않고 함께 변한다.
