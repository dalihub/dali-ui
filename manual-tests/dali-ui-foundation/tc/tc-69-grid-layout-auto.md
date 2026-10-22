# 69. GridLayout: Auto Sizing

batch definition API로 구성한 Auto/Star 혼합 track을 확인한다.

## 화면 구성

- Auto/Star/Auto 세 행과 Auto/Star 두 열
- 고정 크기 content가 Auto track을 결정하는 header/sidebar/footer

## 테스트 절차

1. Auto 행·열과 Star 영역의 크기를 비교한다.
2. **기대 결과**: Auto track은 content 크기에 맞고 Star track은 남은 공간 전체를 차지한다.
3. `Parent: Width 800`, `Parent: Height 600`, `Parent: 800x600`, `Parent: Full`을 차례로 누른다.
4. **기대 결과**: Auto track은 유지되고 Star 영역만 가용 공간에 따라 변한다.

## 통과 기준

- `SetRowDefinitions`와 `SetColumnDefinitions` 결과가 개별 정의와 같은 규칙을 따른다.

## 크기 제어 범위

- 버튼은 테스트 대상 부모의 requested size를 변경한다. `Full`은 버튼 행을 제외한 host의 가용 영역을 채운다.
- 고정 수치는 layout 단위이며 화면 좌표와의 수치 비교는 effective scale=1 기준이다.
- 부모 크기 변화에 따른 재배치를 검증한다. 실제 Window resize 이벤트 및 `WINDOW_RESIZED` transition 정책은 이 절차의 검증 범위에 포함하지 않는다.
