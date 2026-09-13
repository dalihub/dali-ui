# 64. FlexLayout: Layout Direction

FlexLayout의 RTL 순서 반전과 standalone 예외를 확인한다.

## 화면 구성

- 고정 폭 빨강·파랑 사이 grow 초록
- 좌상단 standalone `Change LayoutDirection` 버튼

## 테스트 절차

1. 버튼으로 RTL과 LTR을 번갈아 선택한다.
2. **기대 결과**: 세 flex 자식의 물리 위치는 좌우 반전되지만 grow 폭은 유지되고 버튼은 움직이지 않는다.

## 통과 기준

- RTL 변경이 flex 크기 분배를 바꾸지 않고 위치만 mirror한다.
