# 76. Standalone: StackLayout

StackLayout의 STANDALONE 자식이 누적 크기와 spacing에서 제외되는지 확인한다.

## 화면 구성

- 고정 높이 빨강과 남은 공간을 채우는 초록
- 요청 좌표 `(300, 300)`의 standalone 파랑 100x100

## 테스트 절차

1. 초록 영역과 파랑 위치를 확인한다.
2. **기대 결과**: 초록은 파랑 높이나 spacing을 차감하지 않고 남은 stack 공간을 채우며 파랑은 독립 배치된다.

## 통과 기준

- standalone 자식 전후에 추가 stack spacing이 생기지 않는다.
