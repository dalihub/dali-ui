# 152. Layout: Compute performance

이 문서는 [tc-152-layout-compute-performance.cpp](tc-152-layout-compute-performance.cpp)의 실행 절차입니다. 기존 Layout TC 결과는 합산하지 마십시오. 전체 실행 조건은 [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오.

## 실행 및 완료 확인

1. app와 실제 resolve된 library의 build-ID, revision, build type을 기록하고 stdout을 저장하십시오. launcher에서 `152. Layout`을 검색하여 해당 TC에 진입하십시오.
2. `TC152.Reset run`을 한 번 실행하고 새 `run`을 기록하십시오. scenario 내 step 사이에는 Reset하지 마십시오.
3. `Run scenario`(step 하나씩 진행하려면 `Run step`)를 누른 뒤 `LR_ACTION`의 scenario/step/action_seq/required_checks를 기록하십시오. 동기 step은 즉시 완료됩니다. Window/resource step은 해당 episode의 완료까지 기다리십시오. 10초 내 완료되지 않으면 PENDING을 PASS로 처리하지 마십시오. 반복 sample step만 최대 120초를 허용합니다.
4. 각 step 완료 시 자동 출력되는 **모든** `LR_CHECK` 행을 읽으십시오. 행의 tc/pid/run/scenario/step/action_seq를 연결하여 다른 실행의 값과 혼합하지 마십시오. 같은 행을 다시 읽으면 값이 같아야 합니다.
5. 해당 step의 실제 행 수와 required_checks가 정확히 같아야 합니다. 일반 rect 허용오차는 0.001, 정수/bool/ID는 exact입니다. NaN/Inf, 누락/잘림, 중복 읽기로 채운 행 수, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. `passed`만 믿지 말고 actual/expected/tolerance를 재계산하십시오.
6. 모든 step 후 `Next scenario`로 이동하고 모든 scenario를 실행하십시오. 마지막 `LR_RESULT`의 completed_scenarios=required_scenarios, failures=0 및 전체 행 증거가 필요합니다. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오. 실패 후 Reset으로 실패 이력을 지우지 마십시오.

## 입력과 독립 기대값

각 Stack/Flex/Grid/Absolute × N=16/128/512, 총 12개 scenario에서 반복 sample step `construction`, `first_pass`, `warm_pass`, `idle_pass`를 먼저 순서대로 실행하십시오. step당 필수 행 수는 189+4*N입니다. 이 TC는 시간을 측정하지 않으며 Debug와 Release 빌드에서 같은 행을 출력합니다.

fixture의 i번째 leaf는 (10*i,0,8,16), root는 (10*N−2,16)이며 workload는 화면의 stage에 부착되고 형제 leaf 하나가 같은 stage에 있습니다. 모든 layout은 `LayoutController::ProcessLayouts()`(응용이 사용하는 controller 경로)로 수행합니다. phase마다 sample 하나에서 수행하는 동작은 다음과 같습니다.

| phase | sample마다 수행하는 동작 | 의미 |
|---|---|---|
| `construction` | `FlatWorkload` 생성 1회 | 생성한 View/params의 요청값 |
| `first_pass` | 부착 직후 `ProcessLayouts()` 1회 | 첫 measure/arrange producer 경로 |
| `warm_pass` | (형제 폭 변경 + `ProcessLayouts()`) 32회 | root measure/arrange cache hit과 subtree replay |
| `idle_pass` | dirty 없는 `ProcessLayouts()` 4096회 | dirty 없는 pass가 workload를 바꾸지 않음 |

warm-up 없이 sample j=0…30마다 새 workload를 만들어 부착하고(첫 sample은 stage 생성, 이후는 root 교체) `first_pass`가 아니면 settle pass를 한 번 실행한 뒤 위 동작을 수행합니다. 각 sample에서 `sample.immediate.j.*` 3행(`construction`은 요청값 values=3+2*N, 나머지는 root measured/arranged와 모든 leaf의 measured slot·event-side rect values=12+6*N)과 `sample.geometry.j.*` 3행(leaf 집계)을 검사하고, step 끝에서 화면 부착된 워크로드의 새 `REFRESH_ONCE` render task 완료 뒤 current scene geometry로 `rendered.*`(집계 3행 + leaf별 4행)을 검사합니다. 따라서 step당 31×6+3+4*N=189+4*N행입니다.

이어서 같은 12개 scenario마다 leaf producer budget step `work-first_pass`, `work-warm_pass`, `work-idle_pass`를 실행하십시오. 두 step 집합은 하나의 빌드에서 연속 실행합니다. 행 수는 step당 11+4*N입니다(immediate 3, leaf budget 2, geometry 3, rendered 3+4N). workload를 만들 때 모든 leaf에 계수용 MeasureCallback/ArrangeCallback을 설치합니다. leaf는 고정 크기의 자식 없는 View이므로 요청 크기를 보고하고 받은 slot을 그대로 돌려주는 callback은 기본 producer와 같은 layout을 만듭니다. container에 callback을 설치하면 manager를 대체하므로 root와 container는 계수하지 않습니다. phase 동작 직전의 counter를 기준으로 다음 leaf producer 실행 수를 exact로 검사합니다(`work.leaf.measure-producers`, `work.leaf.arrange-producers`).

| phase | leaf Measure producer | leaf Arrange producer |
|---|---|---|
| first pass | N | N |
| warm pass ×32 | 0 | 0 |
| idle pass ×4096 | 0 | 0 |

first pass는 모든 leaf를 정확히 한 번 측정·배치합니다(Grid가 같은 constraint로 다시 묻는 측정은 cache hit이므로 producer를 다시 실행하지 않습니다). warm/idle pass는 workload 전체를 cache에서 처리하므로 leaf producer가 실행되지 않습니다. root의 cache hit·replay, geometry write, wake·park 요청, 명명된 storage 요청량은 내부 상태이므로 이 TC가 관측하지 않습니다.

leaf producer budget의 exact 실패를 반복 sample step의 통과로 상쇄하지 마십시오.

최종 PASS에는 모든 scenario의 반복 sample step과 work-budget step 행이 모두 필요합니다. 이 TC는 외부 증거를 요구하지 않으며 `EXTERNAL_REQUIRED`를 내지 않습니다. 큰 N의 workload는 window 폭을 넘을 수 있습니다. 최종 `rendered.*`는 `AfterFrame`으로 새 render task 완료를 받은 뒤 모든 leaf의 current scene geometry를 독립 식과 비교합니다. 화면 밖 leaf까지 OS Window에서 실제 보였다는 의미는 아닙니다. frame 실패 시 event-side 값으로 대체하지 마십시오.

## 종료와 증거

`< Back`으로 나간 뒤 다시 진입하여 첫 step을 재실행하십시오. 이전 timer/callback/fixture/추가 Window가 남아 있으면 FAIL입니다. 결과에는 scenario/input/step, 실제값과 기대값, 최초 실패 행, 전체 stdout, artifact fingerprint를 첨부하십시오. `python3 layout-validation-tools.py check-log LOG --tc TC152 --profile release --manifest MANIFEST --manifest-sha256 PIN`은 저장된 행의 누락과 수치를 다시 검사하는 보조 수단입니다. 이 도구는 앱을 실행하거나 서버의 UI 자동 조작을 대체하지 않습니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 12 scenario, 84 action, 82940 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `stack-n16` | `construction` : 253 → `first_pass` : 253 → `warm_pass` : 253 → `idle_pass` : 253 → `work-first_pass` : 75 → `work-warm_pass` : 75 → `work-idle_pass` : 75 |
| `stack-n128` | `construction` : 701 → `first_pass` : 701 → `warm_pass` : 701 → `idle_pass` : 701 → `work-first_pass` : 523 → `work-warm_pass` : 523 → `work-idle_pass` : 523 |
| `stack-n512` | `construction` : 2237 → `first_pass` : 2237 → `warm_pass` : 2237 → `idle_pass` : 2237 → `work-first_pass` : 2059 → `work-warm_pass` : 2059 → `work-idle_pass` : 2059 |
| `flex-n16` | `construction` : 253 → `first_pass` : 253 → `warm_pass` : 253 → `idle_pass` : 253 → `work-first_pass` : 75 → `work-warm_pass` : 75 → `work-idle_pass` : 75 |
| `flex-n128` | `construction` : 701 → `first_pass` : 701 → `warm_pass` : 701 → `idle_pass` : 701 → `work-first_pass` : 523 → `work-warm_pass` : 523 → `work-idle_pass` : 523 |
| `flex-n512` | `construction` : 2237 → `first_pass` : 2237 → `warm_pass` : 2237 → `idle_pass` : 2237 → `work-first_pass` : 2059 → `work-warm_pass` : 2059 → `work-idle_pass` : 2059 |
| `grid-n16` | `construction` : 253 → `first_pass` : 253 → `warm_pass` : 253 → `idle_pass` : 253 → `work-first_pass` : 75 → `work-warm_pass` : 75 → `work-idle_pass` : 75 |
| `grid-n128` | `construction` : 701 → `first_pass` : 701 → `warm_pass` : 701 → `idle_pass` : 701 → `work-first_pass` : 523 → `work-warm_pass` : 523 → `work-idle_pass` : 523 |
| `grid-n512` | `construction` : 2237 → `first_pass` : 2237 → `warm_pass` : 2237 → `idle_pass` : 2237 → `work-first_pass` : 2059 → `work-warm_pass` : 2059 → `work-idle_pass` : 2059 |
| `absolute-n16` | `construction` : 253 → `first_pass` : 253 → `warm_pass` : 253 → `idle_pass` : 253 → `work-first_pass` : 75 → `work-warm_pass` : 75 → `work-idle_pass` : 75 |
| `absolute-n128` | `construction` : 701 → `first_pass` : 701 → `warm_pass` : 701 → `idle_pass` : 701 → `work-first_pass` : 523 → `work-warm_pass` : 523 → `work-idle_pass` : 523 |
| `absolute-n512` | `construction` : 2237 → `first_pass` : 2237 → `warm_pass` : 2237 → `idle_pass` : 2237 → `work-first_pass` : 2059 → `work-warm_pass` : 2059 → `work-idle_pass` : 2059 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC152 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
