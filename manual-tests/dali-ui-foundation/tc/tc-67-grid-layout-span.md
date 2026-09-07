# 67. GridLayout: Span

3x3 Star grid에서 RowSpan과 ColumnSpan을 확인한다.

## 화면 구성

- 첫 행 두 열을 차지하는 빨강
- 오른쪽 아래 2x2를 차지하는 노랑
- 나머지 초록·파랑·청록 cell

## 테스트 절차

1. 색상 cell의 경계와 10px spacing을 확인한다.
2. **기대 결과**: 빨강은 두 열, 노랑은 두 행과 두 열을 차지하며 다른 cell과 겹치지 않는다.

## 통과 기준

- span 영역이 해당 track과 spacing을 정확히 합친 크기로 배치된다.
