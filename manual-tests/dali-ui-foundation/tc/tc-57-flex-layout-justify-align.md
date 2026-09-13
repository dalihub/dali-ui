# 57. FlexLayout: Justify and Align

SPACE_BETWEEN, AlignItems CENTER와 자식별 AlignSelf override를 확인한다.

## 화면 구성

- 가로 FlexLayout 안의 폭 50·높이 200인 여섯 색상 자식
- 기본 CENTER와 FLEX_START/CENTER/FLEX_END/BASELINE override

## 테스트 절차

1. 자식 사이 가로 간격을 확인한다.
2. **기대 결과**: 첫 자식과 마지막 자식이 양 끝에 놓이고 중간 간격이 균등하다.
3. 세로 위치를 비교한다.
4. **기대 결과**: AlignSelf가 설정된 자식은 각각 start/center/end/baseline 규칙을 따르고 나머지는 AlignItems CENTER를 따른다.

## 통과 기준

- justify와 개별 cross-axis override가 서로 독립적으로 적용된다.
