# 68. GridLayout: Margin and Padding

GridLayout padding과 일반/spanning cell margin을 확인한다.

## 화면 구성

- 회색 root와 50px padding
- margin 없는 빨강·노랑, 50px margin의 초록·파랑
- 두 열 span과 50px margin을 가진 청록

## 테스트 절차

1. 회색 테두리와 각 cell 내부 여백을 비교한다.
2. **기대 결과**: root padding이 외곽에 보이고 margin cell에는 각 track 안쪽으로 50px 회색 공간이 생긴다.
3. 청록 영역을 확인한다.
4. **기대 결과**: 두 열을 span한 전체 영역을 기준으로 50px margin이 적용된다.

## 통과 기준

- spacing=0 상태에서도 padding과 margin의 역할이 명확히 분리된다.
