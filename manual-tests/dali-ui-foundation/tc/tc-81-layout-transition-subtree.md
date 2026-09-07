# 81. LayoutTransition: Subtree

root transition의 SUBTREE와 DIRECT_CHILDREN reflow 범위를 비교한다.

## 화면 구성

- Toggle layout, Scope, Add/Remove item 버튼
- spacer와 transition 없는 중첩 card/inner item

## 테스트 절차

1. SUBTREE에서 Toggle layout을 누른다.
2. **기대 결과**: card 이동과 내부 item 크기·위치가 하나의 root transition으로 함께 보간된다.
3. Scope를 DIRECT로 바꾸고 다시 토글한다.
4. **기대 결과**: 직접 자식 card는 애니메이션되지만 내부 item은 최종 위치로 즉시 이동한다.
5. 각 scope에서 item을 추가·제거한다.
6. **기대 결과**: SUBTREE에서는 상속 ENTER/EXIT이 적용되고 DIRECT에서는 즉시 나타나거나 사라진다.

## 통과 기준

- scope 변경이 중첩 자식에만 차이를 만들고 root 직접 자식 transition은 유지된다.
