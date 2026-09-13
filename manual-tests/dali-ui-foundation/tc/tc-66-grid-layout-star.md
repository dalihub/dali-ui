# 66. GridLayout: Star Sizing

고정 track과 Star(2):Star(1) 비례 분배 및 column span을 확인한다.

## 화면 구성

- 고정 높이 header/footer와 Star 중간 행
- 고정 100px 열과 Star(2), Star(1) 열
- 전체 열을 span하는 header/footer

## 테스트 절차

1. 중간 행의 두 star 열 폭을 비교한다.
2. **기대 결과**: 고정 sidebar를 제외한 공간을 2:1로 나눈다.
3. header/footer를 확인한다.
4. **기대 결과**: 각 영역이 세 열 전체를 끊김 없이 span한다.

## 통과 기준

- 창 크기가 변해도 star 비율과 span이 유지된다.
