# 92. View Layout: Padding

plain View의 padding이 고정/WRAP_CONTENT/MATCH_PARENT descendant에 적용되는지 확인한다.

## 화면 구성

- 50px padding을 가진 빨강·초록·파랑 부모
- 각 부모 내부의 노랑·청록·자홍 자식

## 테스트 절차

1. 세 부모 내부의 자식 경계를 확인한다.
2. **기대 결과**: 각 자식이 부모 content box 기준으로 네 방향 50px 안쪽에 배치된다.
3. MATCH_PARENT 내부 자식의 폭을 확인한다.
4. **기대 결과**: 부모 폭에서 좌우 padding을 제외한 폭을 채운다.
5. WRAP_CONTENT 초록을 확인한다.
6. **기대 결과**: 고정 100x100 자식과 padding을 합한 content 크기를 갖는다.

## 통과 기준

- padding이 부모 외부 위치가 아니라 내부 content bounds에만 적용된다.
