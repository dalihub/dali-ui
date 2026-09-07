# 80. LayoutTransition: Reorder

목록 drag reorder 중 proxy slot, sibling CHANGE transition, drop settle, edge auto-scroll을 확인한다.

## 화면 구성

- ENTER/EXIT/Edit 버튼
- 세로 ScrollView 안의 색상 카드 목록
- TC-local drag overlay

## 테스트 절차

1. Edit를 누르고 카드를 눌러 위아래로 드래그한다.
2. **기대 결과**: 원본 위치에는 투명 proxy가 남고 floating 카드가 손가락을 따르며
   경계를 지날 때 sibling이 애니메이션으로 재배치된다.
3. 손을 놓는다.
4. **기대 결과**: 카드가 목표 slot으로 이동해 settle되고 proxy와 floating 복제본이 남지 않는다.
5. 카드를 viewport 위/아래 가장자리로 드래그한다.
6. **기대 결과**: 손을 놓지 않아도 목록이 해당 방향으로 자동 스크롤되고 off-screen slot까지 이동할 수 있다.

## 통과 기준

- 논리 순서와 화면 순서가 일치하고 반복 reorder 후 actor 수가 누적되지 않는다.
