# Text Layout

Label의 줄바꿈, MaximumLines, 크기 측정, TextFit, ImageSpan 및 Sync/Async 전환을 자동 검증한다.

## 화면 구성

- 목록의 `Text Layout` 항목 (`text-layout`)
- 자동 검증 실행 버튼 `Run auto: Sync + Async [V]` (`text-layout-V`)
- 현재 case와 Sync/Async 경로 (`text-layout-progress`)
- 실행 상태와 최종 PASS/FAIL (`text-layout-status`)
- 결과 요약, 실패 내역과 Report 경로 (`text-layout-failures`)

## 테스트 절차

1. 앱 목록에서 `Text Layout`에 진입한다.
2. `Run auto: Sync + Async [V]`를 한 번 누른다. 키보드가 있으면 V로 대체할 수 있다.
3. **기대 결과**: `AUTO VERIFY RUNNING`이 표시되고 case 번호가 증가한다.
4. 진행 상태를 1~2초 간격으로 확인하며 완료를 기다린다. 실행 중에는 다른 조작을 하지 않는다.
5. **기대 결과**: `AUTO VERIFY PASS`, 총 `438 cases`, `0 failures`가 표시된다.
6. 결과 영역이 화면 밖에 있으면 상세 영역(`text-layout-details`)을 스크롤하여 확인한다.
7. **기대 결과**: `Sync: 216 cases | Async: 222 cases | Failed cases: 0`이며,
   결과 영역의 accessibility value는 `PASS`이다.
8. 최종 결과 화면과 Report 경로의 로그를 저장한다.
9. **기대 결과**: 로그 마지막에 아래 SUMMARY와 PASS가 기록된다.

```text
[TEXT_LAYOUT][SUMMARY] cases=438 checks=... pass=... fail=0 syncCases=216 asyncCases=222 failedCases=0
[TEXT_LAYOUT][AUTO][PASS]
```

실패하면 case ID, category/name, expected/observed와 전체 로그를 저장한다.
화면에는 처음 20개 실패 check만 표시된다. 진행이 멈추거나 오류/종료로 완료하지 못하면
마지막 case ID와 화면·로그를 남기고 미완료로 기록한다.

## 통과 기준

- 전체 438개 case가 완료되고 Sync 216개 / Async 222개 모두 실행된다.
- 최종 화면과 로그가 PASS이며 `fail=0`, `failedCases=0`이다.
- Check 수는 고정하지 않는다. 이전 실행이나 중간 check의 PASS를 최종 결과로 사용하지 않는다.
