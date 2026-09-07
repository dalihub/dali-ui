# 71. GridLayout: Layout Direction

GridLayout의 RTL cell mirroring과 standalone 예외를 확인한다.

## 화면 구성

- 고정 크기 3x2 grid
- 좌상단 standalone `Change LayoutDirection` 버튼

## 테스트 절차

1. 버튼으로 RTL로 전환한다.
2. **기대 결과**: 각 행의 열 위치가 좌우 반전되며 행 위치와 cell 크기는 유지된다. 버튼은 움직이지 않는다.
3. LTR로 복귀한다.
4. **기대 결과**: 초기 cell 위치가 복원된다.

## 통과 기준

- RTL은 column의 물리 위치만 mirror하고 standalone child를 제외한다.
