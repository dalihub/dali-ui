# 82. LayoutTransition: Grid Reorder

3열 grid의 long-press reorder, row/column 재지정, CHANGE transition과 auto-scroll을 확인한다.

## 화면 구성

- SVG 아이콘 12개의 3열 GridLayout
- 상단 아이콘 행과 400x600 rounded panel (창 높이가 1080보다 작으면 패널 높이는 머리줄 아래 남은 높이에 맞춘다 — 패널 전체가 화면 안에 있어야 한다)
- TC-local drag overlay

## 테스트 절차

1. grid cell을 0.5초 이상 누른 뒤 다른 cell 위로 드래그한다.
2. **기대 결과**: 선택 cell이 floating 상태가 되고 proxy가 slot을 보존하며 통과한 cell들의 row/column이 애니메이션으로 바뀐다.
3. 빈 cell margin을 드래그해 일반 scroll을 확인한다.
4. **기대 결과**: cell gesture와 충돌하지 않고 ScrollView가 움직인다.
5. drag 상태로 viewport 가장자리에 머문다.
6. **기대 결과**: auto-scroll하면서 proxy 목표가 계속 갱신된다.
7. 손을 놓는다.
8. **기대 결과**: cell이 목표 위치에 정착하고 아이콘·라벨·proxy 중복이 없다.

## 통과 기준

- 모든 SVG가 로드되고 reorder 후 grid 논리 순서와 시각 순서가 일치한다.

## 에이전트가 직접 수행할 grid reorder 검증

[공통 로그 분석 절차](layout-transition-logs.md)를 따른다. 원래 panel과 viewport에서 조작한다.

1. 초기 화면과 REORDER 첫 로그에서 cell의 이름·ID·parent-local x/y와 논리 row/column을 보존한다. cell을 0.5초 이상 누르고 인접 slot으로 드래그한 뒤 놓는다.
2. REORDER와 DROP run을 모두 분석한다. 영향받은 sibling의 목표 row/column을 실제 순서로 계산한다. column 간격은 120, row 간격은 150이다. 행 경계를 넘는 이동은 두 축을 함께 계산한다.
3. actor별 시작·목표 사이의 실제 중간 x/y와 최종 좌표, 폭·높이 유지를 확인한다. raw나 정확한 25/50/75% 좌표를 요구하지 않는다. floating preview에 grid 공식을 적용하지 않는다.
4. 최종 이름·ID별 논리·시각 순서와 proxy/preview·아이콘·라벨 중복 제거를 확인한다. 원래 long-press·일반 scroll·edge auto-scroll도 수행한다. SVG 렌더링은 최종 화면으로 별도 확인한다.
