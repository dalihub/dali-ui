# 78. LayoutTransition: Spec

declarative spec 방식의 ENTER/EXIT/CHANGE bounds 및 opacity transition을 확인한다.

## 화면 구성

- ENTER/EXIT/CHANGE 버튼
- 초기 색상 자식 세 개와 0.4초 EASE_IN_OUT_SINE transition

## 테스트 절차

1. ENTER를 누른다.
2. **기대 결과**: 새 자식이 위쪽 기준 높이 0에서 확장되며 opacity 0에서 1로 변하고 기존 자식은 기존 위치를 유지한다. 새 자식은 목록 끝에 추가되므로 기존 자식의 이동을 요구하지 않는다.
3. CHANGE를 누른다.
4. **기대 결과**: 모든 자식 높이가 80과 160 사이에서 전환되며 위치와 크기가 보간된다.
5. EXIT를 누른다.
6. **기대 결과**: 마지막 자식이 위쪽으로 축소되고 fade-out된 뒤 unparent된다.

## 통과 기준

- 각 버튼 동작이 완료된 뒤 geometry와 visibility가 기대 상태에 도달하고 EXIT 대상은 unparent된다. 잔상이나 중복 자식이 남지 않는다.
- 각 slot의 설정은 0.4초 EASE_IN_OUT_SINE이다. 세 버튼이 서로 다른 동작을 시작하므로 세 slot의 항상 동시 완료를 요구하지 않는다.

## 에이전트가 직접 수행할 전환 검증

[공통 로그 분석 절차](layout-transition-logs.md)를 따른다. 원래 ENTER/CHANGE/EXIT 버튼을 각각 실행한 뒤, 수집한 로그의 run과 actor ID를 연결해 분석한다.

1. 새 진입에서 ENTER. 새 actor ID의 유효한 중간 높이 `0<h<80`, opacity `0<opacity<1`을 확인한다. 같은 run/sample에서 같은 easing을 쓰는 높이와 opacity는 `h/80≈opacity`이어야 한다. 기존 세 자식의 위치는 유지되어야 한다. 마지막 로그와 현재 화면에서 새 자식의 높이 80·opacity 1을 확인한다.
2. CHANGE. 높이 80→160, y는 0/90/180/270→0/170/340/510이다. actor별 시작과 목표 사이의 중간값을 확인하고, 같은 run/sample에서 모든 자식 높이와 sibling 간격 `height+10`을 비교한다. 최종 목표 bounds를 확인한다.
3. EXIT. 직전 높이 H에서 유효한 중간 높이 `0<h<H`, opacity `0<opacity<1`, `h/H≈opacity`를 확인한다. 마지막 자식이므로 기존 siblings의 이동을 요구하지 않는다. 최종 parent=0·scene=0과 화면의 제거를 확인한다.
4. 최종 화면과 전환 중 로그를 함께 확인한다. 기록을 전환의 실제 재생 속도나 16ms 단위 동시 완료 증거로 사용하지 않는다.
