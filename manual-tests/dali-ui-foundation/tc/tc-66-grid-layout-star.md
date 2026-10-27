# 66. GridLayout: Star Sizing

고정 track과 Star(2):Star(1) 비례 분배 및 column span을 확인한다.

## 화면 구성

- 고정 높이 header/footer와 Star 중간 행
- 고정 100px 열과 Star(2), Star(1) 열
- 전체 열을 span하는 header/footer

## 테스트 절차

1. `Parent: Width 800`, `Parent: Height 600`, `Parent: 800x600`, `Parent: Full`을 차례로 누르며 중간 행의 두 star 열 폭을 비교한다.
2. **기대 결과**: 고정 sidebar를 제외한 공간을 2:1로 나눈다.
3. header/footer를 확인한다.
4. **기대 결과**: 각 영역이 세 열 전체를 끊김 없이 span한다.

## 통과 기준

- 부모 크기가 변해도 star 비율과 span이 유지된다.

## 크기 제어 범위

- 버튼은 테스트 대상 부모의 requested size를 변경한다. `Full`은 버튼 행을 제외한 host의 가용 영역을 채운다.
- 고정 수치는 layout 단위이며 화면 좌표와의 수치 비교는 effective scale=1 기준이다.
- 부모 크기 변화에 따른 재배치를 검증한다. 실제 Window resize 이벤트 및 `WINDOW_RESIZED` transition 정책은 이 절차의 검증 범위에 포함하지 않는다.
