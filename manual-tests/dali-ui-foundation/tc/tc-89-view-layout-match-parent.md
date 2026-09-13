# 89. View Layout: Match Parent

MATCH_PARENT View가 launcher header 아래의 전체 TC content 영역을 채우는지 확인한다.

## 화면 구성

- TC contentArea의 유일한 빨강 View

## 테스트 절차

1. TC에 진입해 빨강 영역의 경계를 확인한다.
2. **기대 결과**: 빨강이 header 아래에서 화면 오른쪽과 아래쪽 끝까지 빈틈없이 채운다.

## 통과 기준

- MATCH_PARENT가 Window 전체가 아닌 제공된 content bounds를 정확히 따른다.
