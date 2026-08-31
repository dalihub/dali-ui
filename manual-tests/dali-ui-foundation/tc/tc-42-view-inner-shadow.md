# 42. View Inner Shadow

typed `InnerShadow` API가 대칭/방향별 inset과 View corner radius를 올바르게 반영하는지 눈으로 확인한다.

## Steps

1. Manual Tests에서 `42. View Inner Shadow`를 연다.
2. `EVEN` 카드의 네 변에서 shadow가 같은 깊이로 보이는지 확인한다.
3. `START + TOP` 카드에서 왼쪽과 위쪽 shadow가 오른쪽과 아래쪽보다 더 깊게 보이는지 확인한다.
4. `END + BOTTOM` 카드에서 오른쪽과 아래쪽 shadow가 왼쪽과 위쪽보다 더 깊게 보이는지 확인한다.
5. `ANIMATED corner radius` 카드의 모서리가 직각과 둥근 형태 사이를 왕복할 때(**편도
   1.8초 / 왕복 3.6초**, AUTO_REVERSE — 한 왕복 이상 지켜볼 것) inner shadow도 모서리를
   따라가는지 확인한다.

## Expected Result

- `EVEN` 카드는 네 방향이 균일한 inner shadow를 보여준다. (균일한가/더 깊은가는 사람
  승인 몫 — 승인된 골든이 회귀를 지킨다)
- 방향별 inset 카드들은 큰 inset을 지정한 변에서 shadow가 더 안쪽까지 보인다.
- 모든 inner shadow는 View 바깥으로 그려지지 않는다. (자동 판정: 카드 **바깥 3px** 띠가
  순수 배경색 — 1px 띠는 안티에일리어싱 경계라 판정 불가; 정적·애니메이션 카드 모두,
  애니메이션 카드는 전 프레임에서 — 실측 0px)
- animated 카드의 inner shadow 모서리는 View corner radius animation과 **분리되지 않고**
  함께 변한다. (자동 판정: 모서리 지점 값이 배경↔그림자 사이를 연속적으로 오간다 — 실측
  5단계 전이; 그림자가 안 따라가면 그 점은 배경/짙은 그림자 둘로만 튄다.) **"잘리지
  않고"의 프레임 단위 결함은 표본 해상도 밖 — 육안 확인**으로 남긴다.
