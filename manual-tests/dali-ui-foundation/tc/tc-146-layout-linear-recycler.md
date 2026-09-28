# 146. Layout: Linear item layout

이 문서는 [tc-146-layout-linear-recycler.cpp](tc-146-layout-linear-recycler.cpp)의 실행 절차입니다. 기존 Layout TC 결과는 합산하지 마십시오. 전체 실행 조건은 [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오.

## 실행 및 완료 확인

1. app와 실제 resolve된 library의 build-ID, revision, build type을 기록하고 stdout을 저장하십시오. launcher에서 `146. Layout`을 검색하여 해당 TC에 진입하십시오.
2. `TC146.Reset run`을 한 번 실행하고 새 `run`을 기록하십시오. scenario 내 step 사이에는 Reset하지 마십시오.
3. `Run scenario`(step 하나씩 진행하려면 `Run step`)를 누른 뒤 `LR_ACTION`의 scenario/step/action_seq/required_checks를 기록하십시오. 동기 step은 즉시 완료됩니다. Window/resource step은 해당 episode의 완료까지 기다리십시오. 10초 내 완료되지 않으면 PENDING을 PASS로 처리하지 마십시오. 성능 sample step만 최대 120초를 허용합니다.
4. 각 step 완료 시 자동 출력되는 **모든** `LR_CHECK` 행을 읽으십시오. 행의 tc/pid/run/scenario/step/action_seq를 연결하여 다른 실행의 값과 혼합하지 마십시오. 같은 행을 다시 읽으면 값이 같아야 합니다.
5. 해당 step의 실제 행 수와 required_checks가 정확히 같아야 합니다. 일반 rect 허용오차는 0.001, 정수/bool/ID는 exact입니다. NaN/Inf, 누락/잘림, 중복 읽기로 채운 행 수, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. `passed`만 믿지 말고 actual/expected/tolerance를 재계산하십시오.
6. 모든 step 후 `Next scenario`로 이동하고 모든 scenario를 실행하십시오. 마지막 `LR_RESULT`의 completed_scenarios=required_scenarios, failures=0 및 전체 행 증거가 필요합니다. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오. 실패 후 Reset으로 실패 이력을 지우지 마십시오.

## 입력과 독립 기대값

| scenario | 주요 독립 기대값 | 검사 수 |
|---|---|---:|
| vertical, horizontal | item 10개, extent20, gap2 → range218, viewport50 | 각각 35 |
| cache-window | before/after22, 초기 materialized0.3, offset44에서 1.5, range218 | 8 |
| variable-decoration | heights10/30/20, offsets3/7/2/4, gap5 → range88 | 22 |
| empty-zero-viewport | range0, offset0, scroll consumption0 및 빈 경계 | 6 |

각 scenario의 `run`를 실행하십시오. fixed vertical은 initial item0.2의 rect=(0,0/22/44,120,20), horizontal은 (0/22/44,0,20,120)입니다. offset26에서 first1/last3, 큰 양의 scroll의 추가 소비량 142 및 offset168/first7/last9, 경계에서 추가 소비 0, 큰 음의 scroll 소비−168입니다. 다른 축의 scroll 소비는 0입니다. item4 content bounds main offset은 88입니다. 마지막은 first0/last2입니다.

variable-decoration에서 item rect는 (3,2,110,10),(3,23,110,30),(3,64,110,20), item1 slot=(0,21,120,36)입니다. 음수 item extent는 1, 음수 spacing은 0으로 제한됩니다. 방향 변경 후 offset0과 first0을 확인하십시오.

앞의 다섯 scenario는 built-in layouter를 통제된 detached Recycler bridge로 직접 검증합니다. 아래 `window-*` scenario는 실제 Window에 붙인 RecyclerView와 adapter를 사용합니다. 두 종류의 증거를 구분하십시오. TC148은 별도의 데이터 변경·focus lifecycle을 검증합니다.

## 실제 RecyclerView와 frame 검증

RecyclerView는 AbsoluteLayout의 명시적인 bounds slot으로 viewport 제약을 받습니다. 외부 200×160 stage는 화면 관측을 위한 공간이며 실제 viewport 크기로 사용하지 않습니다.

각 일반 window action은 23행입니다. controller 처리 또는 공개 scroll/cache API 호출 뒤 새 `REFRESH_ONCE` render task의 완료를 기다리고, 기대 geometry는 대기 조건으로 사용하지 않습니다. viewport는 50, cross extent는 120이며 `wider` 이후 160입니다. frame 완료는 새 update/render 상태의 증거이며 OS compositor 최종 합성의 증거는 아닙니다.

| scenario | action 순서와 독립 expected offset | 추가 expected |
|---|---|---|
| `window-vertical`, `window-horizontal` | attach=0 → middle=26 → end=168 → start=0 | 10개 ×20, spacing2, range218. cache0에서 materialized0..2 → 1..3 → 7..9 → 0..2 |
| `window-cache-vertical`, `window-cache-horizontal` | attach=0 → middle=44 → end=168 | cache before/after22. materialized0..3 → 1..5 → 6..9. range218, viewport50 유지 |
| `window-variable-decoration` | attach=0 → middle=26 → end=38 → wider=38 → start=0 | heights10/30/20, spacing5, decoration3/7/2/4, range88. slot start0/21/62, item y2/23/64. wider 전 width110, 후 width150 |
| `window-empty` | run=0 | count0, range0, viewport50, holder 생성0 |
| `window-zero-viewport` | run=0 | count10, estimated range218, viewport0, holder 생성0 |

variable fixture는 attach 준비 중 public cache 범위를 넓혀 세 item을 모두 측정하고 즉시 cache0으로 되돌립니다. 실제 viewport는 항상 50입니다. 이 준비는 아직 측정되지 않은 item의 추정 크기와 독립 수치 oracle을 혼동하지 않기 위한 것이며 성능 측정이 아닙니다.

23개 관측 행은 다음을 검증합니다. viewport rect 4행, scene 연결, catalog count, range, public/layouter offset, viewport extent, first/last active index, materialized count 및 정확한 index 집합, bound stable ID, 모든 materialized item의 update-thread parent-local rect mismatch 수, visible item의 viewport-relative screen rect mismatch 수, 실제 확인한 visible item 수, maximum error, scroller 존재와 current x/y, scroll idle입니다. item rect는 slot start와 decoration의 독립 합으로 계산하며 screen rect는 주축에서 offset을 뺍니다. cache item도 current local rect를 검사하되 화면 밖 item을 pixel 검증했다고 보고하지 마십시오. 빈경계의 두 `run`은 holder 생성0 검사를 추가하여 각각24행입니다.

모든 geometry mismatch와 maximum error는0, visible checked 수는 독립 visible item 수와 같아야 합니다. 누락 holder는 검사 생략이 아니라 materialized identity/local mismatch 실패입니다. 전체 view/scroller가 보이는 상태에서 current geometry를 읽습니다. 오류를 숨기기 위한 추가 layout pass나 expected 갱신은 허용하지 않습니다.

## 종료와 증거

`< Back`으로 나간 뒤 다시 진입하여 첫 step을 재실행하십시오. 이전 timer/callback/fixture/추가 Window가 남아 있으면 FAIL입니다. 결과에는 scenario/input/step, 실제값과 기대값, 최초 실패 행, 전체 stdout, artifact fingerprint를 첨부하십시오. `python3 layout-validation-tools.py check-log LOG --tc TC146`은 저장된 행의 누락과 수치를 다시 검사하는 보조 수단입니다. 이 도구는 앱을 실행하거나 서버의 UI 자동 조작을 대체하지 않습니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 12 scenario, 26 action, 591 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `vertical` | `run` : 35 |
| `horizontal` | `run` : 35 |
| `cache-window` | `run` : 8 |
| `variable-decoration` | `run` : 22 |
| `empty-zero-viewport` | `run` : 6 |
| `window-vertical` | `attach` : 23 → `middle` : 23 → `end` : 23 → `start` : 23 |
| `window-cache-vertical` | `attach` : 23 → `middle` : 23 → `end` : 23 |
| `window-horizontal` | `attach` : 23 → `middle` : 23 → `end` : 23 → `start` : 23 |
| `window-cache-horizontal` | `attach` : 23 → `middle` : 23 → `end` : 23 |
| `window-variable-decoration` | `attach` : 23 → `middle` : 23 → `end` : 23 → `wider` : 23 → `start` : 23 |
| `window-empty` | `run` : 24 |
| `window-zero-viewport` | `run` : 24 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC146 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
