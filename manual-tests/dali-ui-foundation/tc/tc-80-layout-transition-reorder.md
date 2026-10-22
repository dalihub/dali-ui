# 80. LayoutTransition: Reorder

목록 drag reorder 중 proxy slot, sibling CHANGE transition, drop settle, edge auto-scroll을 확인한다.

## 화면 구성

- ENTER/EXIT/Edit 버튼
- 세로 ScrollView 안의 색상 카드 목록. ScrollView는 Stack weight=1로 버튼 행을 제외한 높이를 채운다.
- TC-local drag overlay

## 테스트 절차

1. Edit를 누르고 카드를 눌러 위아래로 드래그한다.
2. **기대 결과**: 원본 위치에는 투명 proxy가 남고 floating 카드가 손가락을 따르며
   경계를 지날 때 sibling이 애니메이션으로 재배치된다.
3. 손을 놓는다.
4. **기대 결과**: 카드가 목표 slot으로 이동해 settle되고 proxy와 floating 복제본이 남지 않는다.
5. 초기 12개 카드가 있는 상태에서 카드를 viewport 위/아래 가장자리의 60 layout 단위 이내로 드래그한다. 아래쪽은 실제 화면 안의 ScrollView 하단을 사용한다.
6. **기대 결과**: 손을 놓지 않아도 목록이 해당 방향으로 자동 스크롤되고 off-screen slot까지 이동할 수 있다.

## 통과 기준

- 논리 순서와 화면 순서가 일치하고 반복 reorder 후 actor 수가 누적되지 않는다.

## Viewport 확인

- 1920x1080, effective scale=1에서 공통 헤더 60과 TC 버튼 행 60을 제외한 ScrollView 높이는 960이고 화면 y=120~1080에 놓인다.
- 아래 감지 영역 안에 손가락을 유지하면 손을 놓지 않아도 scroll position과 proxy 목표가 갱신되어야 한다. 스크롤 시작/끝 경계에서는 더 이상 이동하지 않는 것이 정상이다.

## 에이전트가 직접 수행할 reorder 검증

[공통 로그 분석 절차](layout-transition-logs.md)를 따른다. 별도 fixture 없이 원래 Edit·drag로 목록을 재배치한다.

1. 초기 화면의 카드 순서를 확인한다. REORDER 첫 표본의 카드 ID별 y와 parent를 보존한다. Edit를 켜고 보이는 카드를 한 slot 위로 드래그한 뒤 놓는다.
2. 실행 후 REORDER와 DROP run을 모두 분석한다. DROP만 확인하면 drag 중 이미 끝난 sibling 전환을 놓칠 수 있다. 같은 sibling ID가 시작 y와 목표 y 사이의 중간 위치를 거치는지 확인한다. 한 slot은 height 80+spacing 10=90이며 목표는 실제 논리 순서로 계산한다.
3. floating/drop 카드의 시작 위치는 pointer에 따라 달라지므로 sibling의 ±90 공식을 적용하지 않는다. 최종 논리·시각 순서 일치, proxy/복제본 제거, 카드 수를 확인한다.
4. 위/아래 edge auto-scroll도 수행한다. scroll offset·parent-local 좌표·화면 좌표를 구분한다. 기록의 좌표만으로 gesture와 scroll을 모두 통과시키지 않는다.
