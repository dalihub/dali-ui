# Focus Tab Order

Tab / Shift+Tab (FORWARD / BACKWARD) 포커스 이동이 container별 sibling screen position과 subtree 순회 기준으로 정상 동작하는지 확인한다.

기하 정렬 규칙은 **"y 먼저, y가 같으면 x"(읽기 순서; RTL이면 x는 역순)**이다
(`focus-finder.cpp`). "top-left 좌표 순"이 아니다 — 두 규칙은 x·y가 상충하는 쌍에서만
갈리고, Scenario 1~3에는 그런 쌍이 하나도 없어 판별하지 못한다(실측). Scenario 4가
그 판별을 위해 존재한다.

## 화면 구성

네 가지 시나리오가 세로로 나열된다. View 기본색은 순서대로 분홍색(`#E8D5E0`), 연녹색(`#D5E0D8`), 파란색(`#D5DBE8`), 베이지색(`#E0D8D5`), 연노란색(`#E0E0D5`), 연보라색(`#D5D5E0`)을 반복 사용한다. 포커스를 받은 View는 주황색(`#FF9800`)으로 변경됨.

## Scenario 1: Reverse-index position

child index와 screen position이 반대인 경우.

```
  [v2(idx2)]  [v1(idx1)]  [v0(idx0)]
   x=0         x=100       x=200
```

### 테스트

1. v2(가장 왼쪽)에 포커스를 둔다
2. Tab 키를 누른다
3. **기대 결과**: v1(중간)으로 이동
4. Tab 키를 다시 누른다
5. **기대 결과**: v0(가장 오른쪽)으로 이동
6. Shift+Tab을 누른다
7. **기대 결과**: v1로 돌아감

## Scenario 2: Nested layouts

두 그룹이 가로로 나열되고, 각 그룹 안에 두 개의 view가 세로로 나열. 같은 parent의 sibling은 screen position 기준으로 정렬되고, 선택된 sibling의 subtree를 먼저 순회한다.

```
  [groupA]    [groupB]
   ├─ a1       ├─ b1
   └─ a2       └─ b2
```

### 테스트

1. a1에 포커스를 둔다
2. Tab을 3번 누른다
3. **기대 결과**: a1 → a2 → b1 → b2 순서로 이동

## Scenario 3: Overlapping squares (중앙 정렬)

세 정사각형이 중앙 정렬로 겹쳐있다. large > medium > small 순서로 크기가 크다.

```
  +------- large -------+
  |   +--- medium ---+  |
  |   |  +- small -+ |  |
  |   |  |         | |  |
  |   |  +---------+ |  |
  |   +---------------+ |
  +---------------------+
```

### 테스트

1. large에 포커스를 둔다
2. Tab을 2번 누른다
3. **기대 결과**: large → medium → small 순서로 이동 (top-left 좌표 순)
4. Shift+Tab을 2번 누른다
5. **기대 결과**: small → medium → large 순서로 복귀

## Scenario 4: y/x 상충 쌍 — 정렬 규칙 판별

P(x=100, y=40)와 Q(x=200, y=0). "x 먼저"라면 P→Q, **"y 먼저"라면 Q→P**로 정확히 반대
순서가 나온다.

### 테스트

1. Q를 탭해 포커스를 준다
2. Tab 키를 누른다
3. **기대 결과**: **P로 이동** (Q가 y=0으로 먼저 — 실측 확인)
4. Shift+Tab을 누른다
5. **기대 결과**: Q로 복귀

## 통과 기준

- Scenario 1: Tab 순서가 child index가 아닌 screen position 기준이어야 한다
- Scenario 2: 중첩 그룹 간 Tab 순서가 container별 sibling ordering과 subtree 순회를 따라야 한다
- Scenario 3: 겹친 View들도 기하 정렬 규칙을 따라야 한다 (동심 배치라 두 규칙이 같은 답 —
  규칙 자체의 판별은 Scenario 4가 맡는다)
- **Scenario 4: 순서가 Q → P여야 한다 — "y 먼저, 동점이면 x" 규칙의 직접 판별**
- 모든 시나리오에서 Shift+Tab은 Tab의 정확한 역순이어야 한다
- 배치 전제는 절대 좌표가 아니라 **상대 관계**(누가 누구의 왼쪽/위인가)로 확인한다 —
  절대 좌표를 못 박으면 무관한 레이아웃 변화(라벨 줄바꿈 등)가 검사를 깨뜨린다
