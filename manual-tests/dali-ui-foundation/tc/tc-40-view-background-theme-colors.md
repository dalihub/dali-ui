# 40. View Background Theme Colors

View background color와 gradient stop color가 color table 변경에 따라 갱신되는지 확인한다.

## 화면 구성

- 상단 상태 라벨: 현재 color table 상태 (`Table A` / `Table B`)
- 왼쪽 카드: `SetBackgroundColor(UiColor token)`으로 설정된 background color
- 오른쪽 카드: token stop color를 가진 `SetBackgroundGradient(...)` background
- 하단 버튼: `Toggle Color Table`

## 테스트 1: Background color token 갱신

1. Manual Tests에서 `40. View Background Theme Colors`를 연다.
2. 왼쪽 카드의 배경색을 확인한다.
3. `Toggle Color Table` 버튼을 탭한다.
4. **기대 결과**: 상태 라벨이 `Table B`로 변경되고, 왼쪽 카드의 배경색이 즉시 변경된다.
5. `Toggle Color Table` 버튼을 다시 탭한다.
6. **기대 결과**: 상태 라벨이 `Table A`로 변경되고, 왼쪽 카드의 배경색이 원래 색으로 돌아온다.

## 테스트 2: Background gradient token 갱신

1. 오른쪽 카드의 gradient 배경을 확인한다.
2. `Toggle Color Table` 버튼을 탭한다.
3. **기대 결과**: 오른쪽 카드의 gradient 시작/끝 색상이 즉시 변경된다.
4. `Toggle Color Table` 버튼을 다시 탭한다.
5. **기대 결과**: 오른쪽 카드의 gradient가 원래 색상 조합으로 돌아온다.

## 통과 기준

- color table 전환 시 background color와 background gradient가 모두 즉시 갱신되어야 한다.
- 여러 번 토글해도 카드 배경이 `Table A` / `Table B` 상태에 맞게 반복 전환되어야 한다.
- 화면 전환 후 다시 TC에 진입했을 때 초기 상태는 `Table A`여야 한다.
