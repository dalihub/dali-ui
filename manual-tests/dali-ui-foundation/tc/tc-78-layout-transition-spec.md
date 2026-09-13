# 78. LayoutTransition: Spec

declarative spec 방식의 ENTER/EXIT/CHANGE bounds 및 opacity transition을 확인한다.

## 화면 구성

- ENTER/EXIT/CHANGE 버튼
- 초기 색상 자식 세 개와 0.4초 EASE_IN_OUT_SINE transition

## 테스트 절차

1. ENTER를 누른다.
2. **기대 결과**: 새 자식이 위쪽 기준 높이 0에서 확장되며 opacity 0에서 1로 변하고 기존 자식도 부드럽게 이동한다.
3. CHANGE를 누른다.
4. **기대 결과**: 모든 자식 높이가 80과 160 사이에서 전환되며 위치와 크기가 보간된다.
5. EXIT를 누른다.
6. **기대 결과**: 마지막 자식이 위쪽으로 축소되고 fade-out된 뒤 unparent된다.

## 통과 기준

- 세 slot이 중첩 잔상 없이 같은 timing으로 완료된다.
