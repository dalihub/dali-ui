# 153. Layout: Update performance

이 문서는 [tc-153-layout-update-performance.cpp](tc-153-layout-update-performance.cpp)의 실행 절차입니다. 기존 Layout TC 결과는 합산하지 마십시오. 전체 실행 조건은 [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오.

## 실행 및 완료 확인

1. app와 실제 resolve된 library의 build-ID, revision, build type을 기록하고 stdout을 저장하십시오. launcher에서 `153. Layout`을 검색하여 해당 TC에 진입하십시오.
2. `TC153.Reset run`을 한 번 실행하고 새 `run`을 기록하십시오. scenario 내 step 사이에는 Reset하지 마십시오.
3. `Run scenario`(step 하나씩 진행하려면 `Run step`)를 누른 뒤 `LR_ACTION`의 scenario/step/action_seq/required_checks를 기록하십시오. 동기 step은 즉시 완료됩니다. Window/resource step은 해당 episode의 완료까지 기다리십시오. 10초 내 완료되지 않으면 PENDING을 PASS로 처리하지 마십시오. 반복 sample step만 최대 120초를 허용합니다.
4. 각 step 완료 시 자동 출력되는 **모든** `LR_CHECK` 행을 읽으십시오. 행의 tc/pid/run/scenario/step/action_seq를 연결하여 다른 실행의 값과 혼합하지 마십시오. 같은 행을 다시 읽으면 값이 같아야 합니다.
5. 해당 step의 실제 행 수와 required_checks가 정확히 같아야 합니다. 일반 rect 허용오차는 0.001, 정수/bool/ID는 exact입니다. NaN/Inf, 누락/잘림, 중복 읽기로 채운 행 수, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. `passed`만 믿지 말고 actual/expected/tolerance를 재계산하십시오.
6. 모든 step 후 `Next scenario`로 이동하고 모든 scenario를 실행하십시오. 마지막 `LR_RESULT`의 completed_scenarios=required_scenarios, failures=0 및 전체 행 증거가 필요합니다. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오. 실패 후 Reset으로 실패 이력을 지우지 마십시오.

## 입력과 독립 기대값

N=16/128/512의 3개 scenario에서 반복 sample step `sparse`, `dense`, `same-value`를 먼저 실행하십시오. 필수 행 수는 step당 189+4*N입니다. 이 TC는 시간을 측정하지 않으며 Debug와 Release 빌드에서 같은 행을 출력합니다.

Stack horizontal(i번째 x=10*i, width 8, y 0)을 화면의 stage에 부착하고 초기 `ProcessLayouts()`로 settle합니다. sparse는 중앙 item 한 개의 height만 15/16으로 왕복하고 dense는 모든 item을 변경합니다. same-value는 이미 16인 값을 16으로 설정하며, 아무것도 invalidate되지 않으므로 형제 leaf의 폭을 바꿔 pass를 유도합니다. 각 sample은 setter(+same-value의 형제 변경)와 `ProcessLayouts()` 1회이며, sparse/dense의 변경 height는 sample j가 짝수이면 15, 홀수이면 16이므로 초기 16에서 시작한 모든 sample이 실제 변경입니다. root 16 높이에서 각 leaf의 요청 height가 유지되어야 하며 다른 leaf가 변경되면 FAIL입니다. `sample.immediate.*`의 scalar values는 12+6*N, mismatch와 maximum_error는 0이어야 하고, step 끝의 `rendered.*`는 화면 부착된 워크로드에 대해 LayoutController가 배치한 event-side leaf별 사각형입니다(렌더 프레임 아님).

이어서 같은 scenario에서 leaf producer budget step `work-sparse`, `work-dense`, `work-same-value`를 실행하고 step당 14+4*N행을 확인하십시오(leaf budget 2, `output.measured` 2, `output.arranged` 4, geometry 3, rendered 3+4N). 두 step 집합은 하나의 빌드에서 연속 실행합니다. workload의 모든 leaf에 계수용 MeasureCallback/ArrangeCallback을 설치하며(TC152와 같은 방식, root는 계수하지 않음) 초기 settle 뒤의 counter를 기준으로 합니다. 변경 leaf 수 M은 sparse 1, dense N, same-value 0이며 leaf Measure producer와 Arrange producer의 실행 수가 각각 정확히 M이어야 합니다(`work.leaf.measure-producers`, `work.leaf.arrange-producers`). 나머지 leaf는 cache에서 처리되어야 합니다. root의 hit·replay, geometry write, invalidation·ancestor 방문 수, storage 요청량은 관측하지 않습니다.

반복 sample step과 leaf producer budget step의 행이 모두 통과해야 최종 PASS이며, 이 TC는 외부 증거를 요구하지 않습니다.

warm-up 없이 각 sample j=0…30에서 `sample.immediate.j.*` 3행과 `sample.geometry.j.*` 3행을 검사하고 step 끝에서 `rendered.*`(집계 3행 + leaf별 4행)을 검사하므로 step당 31×6+3+4*N=189+4*N행입니다. 같은 값 재설정이 불필요하게 leaf를 다시 측정하면 `work-same-value`의 leaf producer budget(0)이 검출합니다.

## 종료와 증거

`< Back`으로 나간 뒤 다시 진입하여 첫 step을 재실행하십시오. 이전 timer/callback/fixture/추가 Window가 남아 있으면 FAIL입니다. 결과에는 scenario/input/step, 실제값과 기대값, 최초 실패 행, 전체 stdout, artifact fingerprint를 첨부하십시오. `python3 layout-validation-tools.py check-log LOG --tc TC153 --profile release --manifest MANIFEST --manifest-sha256 PIN`은 저장된 행의 누락과 수치를 다시 검사하는 보조 수단입니다. 이 도구는 앱을 실행하거나 서버의 UI 자동 조작을 대체하지 않습니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 3 scenario, 18 action, 17571 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `n16` | `sparse` : 253 → `dense` : 253 → `same-value` : 253 → `work-sparse` : 78 → `work-dense` : 78 → `work-same-value` : 78 |
| `n128` | `sparse` : 701 → `dense` : 701 → `same-value` : 701 → `work-sparse` : 526 → `work-dense` : 526 → `work-same-value` : 526 |
| `n512` | `sparse` : 2237 → `dense` : 2237 → `same-value` : 2237 → `work-sparse` : 2062 → `work-dense` : 2062 → `work-same-value` : 2062 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC153 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
