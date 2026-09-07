# 73. Standalone: AbsoluteLayout

AbsoluteLayout의 STANDALONE 자식이 LayoutParams 배치에서 제외되는지 확인한다.

## 화면 구성

- LayoutRect로 배치된 빨강·초록
- 요청 좌표 `(300, 300)`의 standalone 파랑 100x100

## 테스트 절차

1. 세 사각형의 위치를 확인한다.
2. **기대 결과**: 빨강·초록은 LayoutRect를 따르고 파랑은 자체 RequestedX/Y와 크기를 따른다.

## 통과 기준

- standalone 자식에 부모 AbsoluteLayout 배치가 덮어쓰이지 않는다.
