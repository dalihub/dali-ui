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

## 에이전트가 직접 수행할 scope 검증

[공통 로그 분석 절차](layout-transition-logs.md)를 따른다. 원래 Toggle/Scope/Add·Remove 버튼으로 실제 nested card를 조작한다.

1. 새 진입 SUBTREE에서 Toggle을 누른다. spacer height 40→160, inner 세 자식 height 60→100의 중간값을 로그에서 확인한다. 같은 표본의 spacer와 inner 보간 비율 `(h-40)/120`, `(h-60)/40`이 유효 중간 구간에서 비슷한지 비교하고 최종 160/100을 확인한다. card·inner y는 각 부모 기준으로 spacing 10을 포함해 계산한다.
2. 이전 동작이 끝난 뒤 Scope를 DIRECT로 바꾸고 Toggle해 되돌린다. spacer/card의 중간 이동이 기록되는 동안 inner height는 60으로 이미 바뀌고 유지되어야 한다. SUBTREE의 중간 inner 값과 비교한다. 표본 누락만으로 snap을 단정하지 않는다.
3. SUBTREE로 복원해 magenta 자식을 추가·제거한다. 중간 height와 opacity 및 최종 연결/제거를 확인한다. bounds는 0.4초 sine, opacity는 0.3초 기본 easing이므로 `h/60=opacity`를 요구하지 않는다.
4. DIRECT에서 추가·제거한다. 자식은 추가 후 height 60·opacity 1로 나타나고, 제거 후 parent/scene은 0이어야 한다. 충분한 표본이 있는 구간에서 SUBTREE의 중간 변화와 비교한다. 내부 Animation 존재 여부나 lifecycle 횟수를 이 기록으로 단정하지 않는다.
