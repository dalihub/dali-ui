# 90. View Layout: Wrap Content

plain View의 WRAP_CONTENT가 위치가 지정된 자식의 바깥 경계까지 포함하는지 확인한다.

## 화면 구성

- 파랑 WRAP_CONTENT root
- 원점의 빨강 50x50
- `(100, 100)`의 초록 100x100

## 테스트 절차

1. 파랑 root의 오른쪽·아래쪽 경계를 확인한다.
2. **기대 결과**: 두 번째 자식의 끝점까지 포함해 root가 200x200이 된다.

## 통과 기준

- 자식 position이 WRAP_CONTENT 측정에서 누락되지 않는다.
