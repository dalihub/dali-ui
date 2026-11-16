# 131. Layout: Transition scope and owner resolution

이 문서는 [tc-131-layout-transition-scope.cpp](tc-131-layout-transition-scope.cpp)의 신규 TC만 실행하는 절차입니다. 이전 Layout TC의 실행 결과를 이 TC의 증거로 합산하지 마십시오.

## 실행 조건과 조작

1. 검증할 app와 실제로 resolve되는 library의 경로, revision, build-ID 및 build type(Debug/Release)을 기록하십시오. 모든 관측은 공개 API로 수행하므로 별도의 build profile 전환은 없습니다.
2. 검증 artifact의 `manual-tests/dali-ui-foundation/bin/manual-test-dali-ui-foundation`을 실행하고 stdout을 손실 없이 저장하십시오. launcher에서 `131. Layout: Transition scope and owner resolution` 항목을 선택하십시오. 격리 build인 경우 해당 artifact 안의 binary를 실행하십시오.
3. `TC131.Reset run`을 한 번 눌러 새 run ID를 기록하십시오. 이후 실패를 지우기 위해 Reset하지 마십시오. 버튼 표시는 `Reset run`이며 Actor name과 accessibility name은 앞의 TC ID를 포함합니다.
4. 각 scenario에서 `TC131.Run scenario`를 누르면 남은 step이 순서대로 자동 실행됩니다(step 하나씩 진행하려면 `TC131.Run step`). `LR_ACTION`의 scenario/step/required_checks가 아래 순서와 일치하는지 먼저 확인하십시오. 각 step이 끝나면 `LR_READY`와 그 step의 모든 행이 자동 출력됩니다.
5. scene 단계는 해당 Window의 View target snapshot과 Window fence까지 기다립니다. 일반적으로 3초 안에 완료됩니다. 완료되지 않아도 조작하지 말고 framework의 자체 timeout을 기다리십시오: layout fence 10초 후 `observer.timeout`, fresh frame 4초 후 `render.frame-timeout`, render settle 4초 후 `render.settle-timeout` 행으로 실패가 기록됩니다. `LR_READY`가 도착하더라도 필수 행 누락이나 행 수 불일치는 PASS가 아닙니다. 그 전에 버튼을 누르면 `LR_NOOP`(step still running)만 남고 step은 계속 실행 중입니다. timeout 행이 나오면 실행 환경과 로그를 함께 보존하십시오. 자연 완료를 기다리는 step은 고정 Delay 없이 완료 event까지 대기하며, 도착하지 않으면 fresh frame timeout으로 실패합니다.
6. 각 step의 모든 검사 행은 완료 시 stdout에 자동 출력되며 HUD에는 요약과 실패 행만 표시되므로 행 수와 숫자는 stdout에서 빠짐없이 수집하십시오. 모든 step이 끝난 뒤에만 `TC131.Next scenario`를 누르거나, `TC131.Run all`로 남은 scenario를 이어서 자동 실행하십시오. 총 scenario 수는 11개입니다. `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.

추가 창 제목은 `TC131 self root`입니다. 조작 버튼과 결과는 원래 manual-test launcher 창에 남아 있습니다. 추가 창이 앞에 나타나면 원래 창의 고유 `TC131.Run scenario`/`TC131.Run step` 식별자로 대상을 선택하십시오. 추가 창에 같은 위치를 클릭하여 다음 단계가 실행되었다고 간주하지 마십시오.

## 단계와 필수 관측 수

`TR04.scope-0`~`scope-7`: `mount` 4개 → `change-grandchild` 6개입니다. 마지막 `TR04.self-root-inert`는 `self-root` 5개입니다.

`Rect`는 x/y/width/height 네 행, `Size`는 width/height 두 행입니다. 일반 scalar/count/bool 검사 하나는 한 행입니다. `LR_CHECK`의 run/scenario/step/action_seq를 `LR_ACTION`과 대조하고 다음 action 시작 전까지의 로그 구간을 함께 저장하여 같은 이름의 행이 서로 다른 step에서 섞이지 않게 하십시오. `LR_RESULT`의 completed_scenarios/required_scenarios, rows와 page 범위를 함께 보관하십시오.

`TR04.inherited-enter-exit-parent-units`는 `mount-inherited` 4개 → `inherited-enter` 1개 → `inherited-enter-finish` 6개 → `inherited-exit` 3개 → `inherited-exit-finish` 7개입니다. `TR04.standalone-parent-target`은 `mount` 8개 → `change-held` 15개입니다.

## 독립 기대값과 관측 의미

- root → nested(200×100) → child tree에서 child의 local bounds가 (10,15,80,40)에서 (40,15,80,40)으로 바뀝니다.

- mode 0은 DIRECT_CHILDREN, 1은 SUBTREE, 2는 ISOLATE_SUBTREE, 3은 PASS_THROUGH, 4는 가까운 parent의 transition, 5는 child self transition, 6은 standalone 경계, 7은 self transition이 있지만 child PASS_THROUGH입니다.

- ancestor callback은 mode 1/3에서만 1회, 가까운/self callback은 mode 4/5에서만 1회입니다. 나머지는 0회여야 합니다. 이 횟수는 수동 `ProcessLayouts()`의 마지막 animator tick이 새 animator를 raw 0으로 한 번 dispatch한 결과입니다. 모든 모드에서 logical target(`ViewImpl::GetArrangedBounds()`)은 동일합니다.

- 마지막 scenario는 실제 추가 Window의 root에 self transition을 붙이고 폭을 300→280으로 바꿉니다. root=(0,0,280,200), start=0을 확인하십시오.

- inherited ENTER/EXIT의 owner는 root, direct parent는 nested입니다. PARENT_FRACTION .5 offset은 root 300×200이 아닌 nested 200×100에 곱합니다. base=(10,15,80,40), endpoint=(110,65,80,40)이며 root extent를 쓰면 (160,115)가 됩니다. ENTER는 시작(start 1)과 자연 완료 뒤 base로의 정착(`inherited.enter.final`, start/finish 각 1)을, EXIT는 시작(ghost가 논리 child에서 빠지고 actor parent는 nested 유지, start 1)과 자연 완료 시점의 endpoint(`inherited.exit.end`: `OnFinished(EXIT)` 때의 current geometry), start/finish 각 1, nested ghost 제거를 확인하십시오. 진행 중 geometry는 표본화하지 않으며 완료는 lifecycle event로 기다립니다(frame deadline은 liveness 한도일 뿐입니다).

- `TR04.standalone-parent-target`: 200×100 plain View root → MATCH×MATCH nested → MATCH×MATCH STANDALONE child(요청 위치·margin 0)입니다. `mount`의 controller 보고 target은 nested·child 모두 (0,0,200,100)입니다. `change-held`에서 geometry를 쓰지 않고 기록만 하는 CHANGE animator를 root에 붙인 뒤(DIRECT_CHILDREN), root를 300×150으로 바꾸고 child를 `InvalidateMeasure()`하여 같은 batch에 child 자신의 turn을 queue한 다음 `ProcessLayouts()`를 실행합니다. 모든 검사는 그 호출과 같은 action에서 동기로 읽으므로 실시간 tick이 끼어들 수 없습니다. nested의 CHANGE animator가 actor SIZE를 from 값에 붙잡으므로 nested의 actor bounds는 (0,0,200,100)이지만 layout target(`ViewImpl::GetArrangedBounds()`)은 (0,0,300,150)입니다. 깊이 순서로 부모 다음에 실행되는 child의 자기 pass는 부모의 actor SIZE가 아니라 layout target을 읽으므로 child bounds (0,0,300,150), measured (300,150)(EXACTLY)입니다. 호출의 마지막 animator tick이 nested의 CHANGE context를 기록해야 합니다(`target.parent.animator-context`, 15행, docs/layout-structure.md STANDALONE 절의 parent layout target).

## Pass/Fail 판정

- 작은 target 좌표는 절대오차 0.001 이하, 실제 rendered animation 좌표는 0.01 이하입니다. progress는 해당 행의 tolerance(0.0001 또는 0.00001)를 사용하십시오. NaN/Inf는 크기와 무관하게 FAIL입니다. count/ID/bool/exception condition은 정확히 일치해야 합니다.
- `LR_CHECK`의 passed 표시만 믿지 말고 actual/expected/tolerance를 다시 대조하십시오. 같은 action에서 필요한 행 수보다 적거나 값이 잘렸거나 같은 행만 중복 수집되면 PASS로 처리하지 마십시오.
- 모든 필수 action이 실행되고 expected checks와 실제 checks가 일치하며, 전체 11개 scenario가 완료되고 누적 failures=0이어야 합니다. 마지막 페이지 또는 마지막 scenario만 PASS인 것은 TC PASS가 아닙니다.
- missing snapshot, observer overflow, 예상 밖 exception/crash/hang는 PASS가 아닙니다. 현재 제품 오류가 나타나도 expected를 actual로 바꾸거나 xfail로 제외하지 마십시오.
- HUD 갱신이 화면 layout을 발생시켜도 이전 step의 고정된 결과는 바뀌면 안 됩니다. 같은 run/step의 행이 다시 출력될 때 값이 바뀌면 observer 오류로 FAIL입니다.

## 정리와 재실행

launcher로 돌아가면 TC가 signal과 transition callback을 끊고 추가 Window와 fixture tree를 해제합니다. focus 변경 TC는 저장한 focus를 복원합니다. 다시 진입했을 때 초기 scenario/step과 새 run ID인지 확인하십시오. 이전 transition의 callback, 남은 ghost, 추가 Window, 이전 실패행이 새 run으로 유입되면 FAIL입니다. 전체 suite의 재진입 검증에서는 실제 launcher 진입→전체 실행→나가기 과정을 반복하고, 내부 subtree 재생성을 실제 진입/퇴장으로 대신 세지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 11 scenario, 24 action, 129 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `TR04.scope-0` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-1` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-2` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-3` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-4` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-5` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-6` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.scope-7` | `mount` : 4 → `change-grandchild` : 6 |
| `TR04.self-root-inert` | `self-root` : 5 |
| `TR04.inherited-enter-exit-parent-units` | `mount-inherited` : 4 → `inherited-enter` : 1 → `inherited-enter-finish` : 6 → `inherited-exit` : 3 → `inherited-exit-finish` : 7 |
| `TR04.standalone-parent-target` | `mount` : 8 → `change-held` : 15 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC131 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
