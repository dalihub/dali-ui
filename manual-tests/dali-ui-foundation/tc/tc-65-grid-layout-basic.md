# 65. GridLayout: Basic

고정 row/column 크기와 명시적 cell 배치를 확인한다.

## 화면 구성

- 행 높이 50/100/200, 열 폭 50/100
- row/column spacing 10과 여섯 색상 cell

## 테스트 절차

1. 각 cell의 행·열 위치와 track 크기를 확인한다.
2. **기대 결과**: 모든 cell이 지정된 3x2 위치에 들어가고 행·열 사이에 10px 간격이 있다.

## 통과 기준

- 실제 `GridLength::Absolute` 값과 화면의 track 크기가 일치한다.
