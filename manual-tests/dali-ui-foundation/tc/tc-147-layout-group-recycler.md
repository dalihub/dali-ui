# 147. Layout: Grouped item layout

이 문서는 [tc-147-layout-group-recycler.cpp](tc-147-layout-group-recycler.cpp)의 실행 절차입니다. 기존 Layout TC 결과는 합산하지 마십시오. 전체 실행 조건은 [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오.

## 실행 및 완료 확인

1. app와 실제 resolve된 library의 build-ID, revision, build type을 기록하고 stdout을 저장하십시오. launcher에서 `147. Layout`을 검색하여 해당 TC에 진입하십시오.
2. `TC147.Reset run`을 한 번 실행하고 새 `run`을 기록하십시오. scenario 내 step 사이에는 Reset하지 마십시오.
3. `Run scenario`(step 하나씩 진행하려면 `Run step`)를 누른 뒤 `LR_ACTION`의 scenario/step/action_seq/required_checks를 기록하십시오. 동기 step은 즉시 완료됩니다. Window/resource step은 해당 episode의 완료까지 기다리십시오. 10초 내 완료되지 않으면 PENDING을 PASS로 처리하지 마십시오. 성능 sample step만 최대 120초를 허용합니다.
4. 각 step 완료 시 자동 출력되는 **모든** `LR_CHECK` 행을 읽으십시오. 행의 tc/pid/run/scenario/step/action_seq를 연결하여 다른 실행의 값과 혼합하지 마십시오. 같은 행을 다시 읽으면 값이 같아야 합니다.
5. 해당 step의 실제 행 수와 required_checks가 정확히 같아야 합니다. 일반 rect 허용오차는 0.001, 정수/bool/ID는 exact입니다. NaN/Inf, 누락/잘림, 중복 읽기로 채운 행 수, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. `passed`만 믿지 말고 actual/expected/tolerance를 재계산하십시오.
6. 모든 step 후 `Next scenario`로 이동하고 모든 scenario를 실행하십시오. 마지막 `LR_RESULT`의 completed_scenarios=required_scenarios, failures=0 및 전체 행 증거가 필요합니다. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오. 실패 후 Reset으로 실패 이력을 지우지 마십시오.

## 입력과 독립 기대값

`vertical-group-matrix` scenario의 `run`를 실행하여 122개 행을 검증하십시오.

입력 group item counts=(1,3,0,2), header 여부=(true,false,true,false), header11/body20/gap7, item spacing3입니다. flatten 결과 11개를 모두 확인하십시오.

| flat position | type | group | height |
|---:|---|---:|---:|
|0|HEADER|0|11|
|1|SINGLE|0|20|
|2|GROUP_GAP|1|7|
|3|TOP|1|20|
|4|MIDDLE|1|20|
|5|BOTTOM|1|20|
|6|GROUP_GAP|2|7|
|7|HEADER|2|11|
|8|GROUP_GAP|3|7|
|9|TOP|3|20|
|10|BOTTOM|3|20|

각 y는 앞선 모든 height의 합 + 3*position입니다. 총 range=163+30=193입니다. body의 left/right margin5/9, cross120이므로 x5,width106이며 header/gap은 x0,width120입니다. 각 item의 실제 Measure, Actor rect, GetItemBounds를 표와 비교하십시오. margin80/80은 body width0/x80, 음수 margin은 0으로 제한됩니다. adapter 해제 시 inset 없음, empty data와 ClearDataSource 후 flat count0이어야 합니다.

현재 public 계약에서 지원하는 VERTICAL 경로를 검증합니다. 이 TC로 미지원 horizontal group 동작을 보증하지 마십시오.


## 실제 Window와 GroupAdapter 재측정

두 번째 scenario window-group-remeasure도 반드시 실행하십시오. 첫 번째 scenario의 FixedRecycler 결과로 이 scenario를 대체하지 마십시오. 실제 RecyclerView(120×160), GroupAdapter, GroupLinearItemsLayouter를 연결하고, RecyclerView는 주어진 constraint를 그대로 채우므로 화면의 120×160 stage 아래에 부착합니다(window 크기의 host에 직접 부착하지 않습니다). group counts=(1,1), 두 header=true, header11/gap7/spacing3입니다. 두 body의 독립 producer는 전달된 width가 100 이상이면 height20,100 미만이면 height40을 반환합니다. 이 producer의 호출 수는 body Measure 생산 횟수이며 내부 cache 전체 방문 수를 뜻하지 않습니다.

| step | required_checks | 관측과 기대값 |
|---|---:|---|
| attach |30| 120×160 stage에 부착한 RecyclerView root=(0,0,120,160), margin5/9, body Measure width106, body height20, range81 |
| narrow-body |28| SetBodyHorizontalMargin(20,20)에서 invalidation signal 정확히 1회, body Measure 재실행, width80/height40, range121 |
| narrow-window-fence |30| 명시적인 InvalidateArrange 후 Window fence에서도 root 크기와 좁은 body 배치 유지 |
| restore-body |28| margin5/9 복원 시 invalidation1회와 body Measure 재실행, width106/height20, range81 복원 |

각 단계의 필수 actual.row.0.4.{x,y,width,height}는 아래 표와 같아야 합니다. actual.row.count=5, margin.left/right, actual.range, body.measure.width.1/4도 모두 확인하십시오. narrow/restore에는 각각 *.invalidation.delta와 *.body.producer.ran이 추가됩니다. 실제 row가 없거나 producer가 없으면 다른 성공 행으로 부족한 검사를 채울 수 없습니다.

| row/type | margin5/9의 rect | margin20/20의 rect |
|---|---|---|
|0 HEADER|(0,0,120,11)|(0,0,120,11)|
|1 BODY_SINGLE|(5,14,106,20)|(20,14,80,40)|
|2 GAP|(0,37,120,7)|(0,57,120,7)|
|3 HEADER|(0,47,120,11)|(0,67,120,11)|
|4 BODY_SINGLE|(5,61,106,20)|(20,81,80,40)|

y는 앞선 실제 입력 height와 spacing3의 합이며 range=11+bodyHeight+7+11+bodyHeight+4×3=41+2×bodyHeight입니다. header와 gap은 body margin 변경 후에도 x0/width120이어야 합니다. expected를 GetItemBounds나 현재 측정값에서 생성하지 않습니다.

margin setter의 LayoutInvalidatedSignal은 RecyclerView의 실제 동기 재배치를 유발합니다. narrow-body/restore-body는 setter 반환 직후 이미 바뀌어야 할 rect와 Measure 입력을 확인합니다. narrow-window-fence는 별도 step에서 InvalidateArrange를 요청하여 다음 Window 완료 시에도 결과가 보존되는지 확인합니다. 이 명시적인 요청을 margin setter 자체의 Window invalidation 증거로 기록하지 마십시오. fence callback은 관측만 수행하며 Measure/Arrange를 재호출하지 않습니다. cleanup에서는 observer 연결과 producer callback을 해제합니다.

근거 경로는 internal/recycler/group-linear-items-layouter-impl.cpp의 SetBodyHorizontalMargin/GetItemCrossInset, internal/recycler/linear-items-layouter-impl.cpp의 LayoutChunk, integration-api/recycler/recycler-view-impl.cpp의 OnLayoutInvalidated/OnArrange입니다.

## 종료와 증거

`< Back`으로 나간 뒤 다시 진입하여 첫 step을 재실행하십시오. 이전 timer/callback/fixture/추가 Window가 남아 있으면 FAIL입니다. 결과에는 scenario/input/step, 실제값과 기대값, 최초 실패 행, 전체 stdout, artifact fingerprint를 첨부하십시오. `python3 layout-validation-tools.py check-log LOG --tc TC147`은 저장된 행의 누락과 수치를 다시 검사하는 보조 수단입니다. 이 도구는 앱을 실행하거나 서버의 UI 자동 조작을 대체하지 않습니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 2 scenario, 5 action, 238 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `vertical-group-matrix` | `run` : 122 |
| `window-group-remeasure` | `attach` : 30 → `narrow-body` : 28 → `narrow-window-fence` : 30 → `restore-body` : 28 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC147 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
