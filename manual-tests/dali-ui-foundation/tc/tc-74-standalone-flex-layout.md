# 74. Standalone: FlexLayout

FlexLayout의 STANDALONE 자식이 flex line과 grow 계산에서 제외되는지 확인한다.

## 화면 구성

- 일반 고정 빨강과 grow 초록
- 요청 좌표 `(300, 300)`의 standalone 파랑 100x100

## 테스트 절차

1. 초록의 폭과 파랑 위치를 확인한다.
2. **기대 결과**: 초록은 파랑 폭을 차감하지 않고 flex 가용 공간을 채우며 파랑은 지정 좌표에 독립 배치된다.

## 통과 기준

- standalone 자식이 flex line의 basis/grow 계산에 포함되지 않는다.
