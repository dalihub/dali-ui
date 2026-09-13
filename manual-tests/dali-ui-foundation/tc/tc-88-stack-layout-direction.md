# 88. StackLayout: Layout Direction

가로 StackLayout의 RTL mirroring과 standalone 예외를 확인한다.

## 화면 구성

- 고정 폭 빨강·파랑 사이 weight 초록
- 좌상단 standalone `Change LayoutDirection` 버튼

## 테스트 절차

1. 버튼으로 RTL을 선택한다.
2. **기대 결과**: 빨강과 파랑의 물리 위치가 바뀌고 초록은 가운데 남은 폭을 채우며 버튼은 고정된다.
3. LTR로 되돌린다.
4. **기대 결과**: 초기 순서가 복원된다.

## 통과 기준

- 방향 변경 후에도 고정 폭, weight, spacing이 보존된다.
