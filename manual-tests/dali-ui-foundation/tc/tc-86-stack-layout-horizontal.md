# 86. StackLayout: Horizontal

가로 StackLayout의 cross-axis alignment와 weight를 네 행에서 비교한다.

## 화면 구성

- START/CENTER/END/FILL alignment별 가로 행
- 각 행의 고정 폭 자식 두 개와 weight=1 자식

## 테스트 절차

1. 각 행에서 세 자식의 세로 위치와 높이를 비교한다.
2. **기대 결과**: 앞 세 행은 각각 위/중앙/아래 정렬되고 FILL 행은 높이 전체를 채운다.
3. 세 번째 자식의 폭을 확인한다.
4. **기대 결과**: 각 행에서 weight=1 자식이 고정 폭과 spacing을 제외한 나머지를 채운다.

## 통과 기준

- horizontal main-axis weight와 vertical cross-axis alignment가 동시에 유지된다.
