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

실패하면 case key, case ID, category/name, expected/observed와 전체 로그를 저장한다.

## Case 식별

`[TEXT_LAYOUT][CASE][BEGIN] id=C0003 index=3 key=core-matrix/s01/max-1/clip/sync name=core-matrix ...`

- `id`(`C0001`…)는 실행 순서다. case가 추가되면 뒤 번호가 모두 밀린다.
- `name`은 case 묶음 이름이다. `core-matrix`처럼 여러 case가 같은 이름을 쓴다.
- `key`는 **case마다 고유하고 순서와 무관한 식별자**다. 묶음 이름, scenario 번호(`sNN`),
  scenario 기본값과 다른 설정(`max-`, `clip`/`ellipsis`, `wrap-`, `align-`, `rtl`/`ltr`, `fit-`,
  `layout-`, `pad-`, `w`/`h`, `q`, `rapid`), 반복 sequence의 단계(`step-N`), 렌더러(`sync`/`async`)
  순서로 만든다. 설정이 같은 case가 둘이면 두 결과가 한 식별자로 섞이므로, 중복 key는
  `[TEXT_LAYOUT][ERROR] duplicate case key ...`와 exit 2로 실행 전에 막는다.
- 이전 실행과 결과를 비교하거나 한 case만 재현할 때는 `key`를 쓴다
  (`DALI_TEXT_CASE_FILTER=core-matrix/s01/max-1/clip/sync`).
화면에는 처음 20개 실패 check만 표시된다. 진행이 멈추거나 오류/종료로 완료하지 못하면
마지막 case ID와 화면·로그를 남기고 미완료로 기록한다.

## 통과 기준

- 전체 438개 case가 완료되고 Sync 216개 / Async 222개 모두 실행된다.
- 최종 화면과 로그가 PASS이며 `fail=0`, `failedCases=0`이다.
- Check 수는 고정하지 않는다. 이전 실행이나 중간 check의 PASS를 최종 결과로 사용하지 않는다.
