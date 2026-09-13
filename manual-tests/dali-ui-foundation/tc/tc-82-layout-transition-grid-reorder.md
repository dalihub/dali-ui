# 82. LayoutTransition: Grid Reorder

3열 grid의 long-press reorder, row/column 재지정, CHANGE transition과 auto-scroll을 확인한다.

## 화면 구성

- SVG 아이콘 12개의 3열 GridLayout
- 상단 아이콘 행과 400x600 rounded panel
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
