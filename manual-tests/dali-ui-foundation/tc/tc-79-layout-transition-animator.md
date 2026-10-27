# 79. LayoutTransition: Animator

animator callback이 progress에 따라 ENTER/EXIT/CHANGE 속성을 직접 기록하는지 확인한다.

## 화면 구성

- ENTER/EXIT/CHANGE 및 `ENTER > CHANGE` 버튼
- spec TC와 같은 0.4초 외형 계약을 callback으로 구현한 자식 목록

## 테스트 절차

1. ENTER, CHANGE, EXIT를 차례로 누른다.
2. **기대 결과**: ENTER/EXIT은 height와 opacity를, CHANGE는 layout bounds를 callback 진행률에 맞춰 보간한다.
3. `ENTER > CHANGE`를 눌러 실제 ENTER 후 100ms Timer로 successor CHANGE를 시작한다.
4. **기대 결과**: 취소된 slot의 잔여 상태 때문에 자식이 사라지거나 잘못된 opacity에 머물지 않는다.

## 통과 기준

- callback 방식이 spec 방식과 같은 최종 geometry와 visibility를 만든다.

## 에이전트가 직접 수행할 callback 검증

[공통 로그 분석 절차](layout-transition-logs.md)를 따른다. 원래 callback과 실제 목록을 사용하며 geometry와 CONTEXT 로그는 별도의 증거로 읽는다.

1. ENTER를 누른다. 중간 CONTEXT 로그의 `0<raw<1`과 `eased=(1-cos(π×raw))/2`를 비교한다. slot=0, 목표 bounds height=80을 확인한다. 별도의 current 표본에서 중간 높이·opacity 및 최종 80/1을 확인한다.
2. CHANGE와 EXIT. slot=2/1, raw/eased 관계와 from/to bounds를 비교한다. 렌더링된 시작·중간·최종 geometry는 TC78과 같은 목표값으로 확인한다. EXIT 후 제거를 확인한다.
3. 초기 상태에서 `TC79.Interrupt` (`ENTER > CHANGE`)를 누른다. 이 버튼은 실제 ENTER 후 100ms Timer로 기존 CHANGE 동작을 호출한다. AI 입력을 0.4초 안에 반복할 필요가 없다. 로그에서 같은 새 자식 ID의 ENTER 중간 context와 후속 CHANGE context, CHANGE 완료 후 목표 높이·opacity 1을 확인한다. 다른 actor의 CHANGE와 혼동하지 않는다.
4. Timer가 늦어 ENTER가 먼저 끝났다면 중도 취소를 검증한 것이 아니다. 같은 actor ID의 ENTER와 CHANGE CONTEXT 로그 순서·clock을 확인한다. CHANGE 이전에 ENTER raw=1이 기록됐다면 중도 취소로 통과시키지 않는다. 필요한 표본이 없으면 미검증으로 기록하고 재실행한다. 취소된 ENTER의 정상 완료를 요구하지 않는다.
5. Back·재진입 후 초기 상태를 확인한다. 예약 동작 중 화면을 나가면 CHANGE Timer도 정리되어야 한다.
