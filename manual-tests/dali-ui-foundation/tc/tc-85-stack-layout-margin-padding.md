# 85. StackLayout: Margin and Padding

StackLayout의 다양한 margin, root padding과 중첩 가로 stack을 확인한다.

## 화면 구성

- 50px root padding
- 좌우/상하/전체 margin이 다른 세 행
- 50px margin을 가진 중첩 가로 StackLayout

## 테스트 절차

1. 세 행의 물리 여백을 비교한다.
2. **기대 결과**: 각 Insets 방향에만 대응하는 빈 공간이 생긴다.
3. 중첩 행의 네 자식을 확인한다.
4. **기대 결과**: 가로 orientation, 10px spacing, alignment/weight 조합이 바깥 세로 stack 안에서 유지된다.

## 통과 기준

- margin 방향과 nested stack 방향이 혼동 없이 적용된다.
