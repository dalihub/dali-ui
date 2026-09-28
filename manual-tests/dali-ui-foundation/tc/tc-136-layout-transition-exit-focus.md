# 136. Layout: EXIT ghost interaction and focus

이 문서는 [tc-136-layout-transition-exit-focus.cpp](tc-136-layout-transition-exit-focus.cpp)의 신규 TC만 실행하는 절차입니다. 이전 Layout TC의 실행 결과를 이 TC의 증거로 합산하지 마십시오.

## 실행 조건과 조작

1. 검증할 app와 실제로 resolve되는 library의 경로, revision, build-ID 및 build type(Debug/Release)을 기록하십시오. 모든 관측은 공개 API로 수행하므로 별도의 build profile 전환은 없습니다.
2. 검증 artifact의 `manual-tests/dali-ui-foundation/bin/manual-test-dali-ui-foundation`을 실행하고 stdout을 손실 없이 저장하십시오. launcher에서 `136. Layout: EXIT ghost interaction and focus` 항목을 선택하십시오. 격리 build인 경우 해당 artifact 안의 binary를 실행하십시오.
3. `TC136.Reset run`을 한 번 눌러 새 run ID를 기록하십시오. 이후 실패를 지우기 위해 Reset하지 마십시오. 버튼 표시는 `Reset run`이며 Actor name과 accessibility name은 앞의 TC ID를 포함합니다.
4. 각 scenario에서 `TC136.Run scenario`를 누르면 남은 step이 순서대로 자동 실행됩니다(step 하나씩 진행하려면 `TC136.Run step`). `LR_ACTION`의 scenario/step/required_checks가 아래 순서와 일치하는지 먼저 확인하십시오. 각 step이 끝나면 `LR_READY`와 그 step의 모든 행이 자동 출력됩니다.
5. scene 단계는 해당 Window의 View target snapshot과 Window fence까지 기다립니다. 일반적으로 3초 안에 완료됩니다. 완료되지 않아도 조작하지 말고 framework의 자체 timeout을 기다리십시오: layout fence 10초 후 `observer.timeout`, fresh frame 4초 후 `render.frame-timeout`, render settle 4초 후 `render.settle-timeout` 행으로 실패가 기록됩니다. `LR_READY`가 도착하더라도 필수 행 누락이나 행 수 불일치는 PASS가 아닙니다. 그 전에 버튼을 누르면 `LR_NOOP`(step still running)만 남고 step은 계속 실행 중입니다. timeout 행이 나오면 실행 환경과 로그를 함께 보존하십시오. animation 자연 완료 단계에서는 CPP에 적힌 650~1000ms Delay 관측까지 기다리십시오.
6. 각 step의 모든 검사 행은 완료 시 stdout에 자동 출력되며 HUD에는 요약과 실패 행만 표시되므로 행 수와 숫자는 stdout에서 빠짐없이 수집하십시오. 모든 step이 끝난 뒤에만 `TC136.Next scenario`를 누르거나, `TC136.Run all`로 남은 scenario를 이어서 자동 실행하십시오. 총 scenario 수는 6개입니다. `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.

## 단계와 필수 관측 수

`TR09.ghost-0`~`3`: `mount` 4개 → `start-and-mutate` 11개 → `finish-or-cancel` 5개입니다. `TR10.focus-reentrant-reparent`: `mount` 4개 → `focused-remove` 8개입니다.

`Rect`는 x/y/width/height 네 행, `Size`는 width/height 두 행입니다. 일반 scalar/count/bool 검사 하나는 한 행입니다. `LR_CHECK`의 run/scenario/step/action_seq를 `LR_ACTION`과 대조하고 다음 action 시작 전까지의 로그 구간을 함께 저장하여 같은 이름의 행이 서로 다른 step에서 섞이지 않게 하십시오. `LR_RESULT`의 completed_scenarios/required_scenarios, rows와 page 범위를 함께 보관하십시오.

`TR10.restore-before-remove-listener`는 `mount` 4개 → `remove` 1개 → `listener-result` 7개입니다.

## 독립 기대값과 관측 의미

- EXIT 시작 시 logical child=0, Actor child=1, 실제 parent=root입니다. SENSITIVE/FOCUSABLE/FOCUS_ON_TOUCH는 모두 false이고 start=1이어야 합니다.

- operation 0은 중복 Remove, 1은 같은 parent Add, 2는 다른 parent로 reparent, 3은 RemoveAll입니다. 중복 start/finish가 없어야 합니다. 다른 parent로 이동하는 경우만 EXIT가 취소되며 finish=0, 새 parent를 유지합니다. 나머지는 자연 완료 후 finish=1/unparent입니다. 세 interaction 속성은 원래 true로 복원되어야 합니다. 0.2초 EXIT가 step 사이에 끝나지 않도록 EXIT 시작과 변이를 한 action(`start-and-mutate`)에서 수행합니다. `finish-or-cancel`은 reparent(operation 2)에서는 새 frame 하나 뒤에, 나머지는 EXIT의 자연 완료 event 뒤에 검사합니다(frame deadline은 liveness 한도일 뿐입니다). dispatcher 내부 대기열이 비었는지는 관측하지 않습니다.

- focus scenario 전에는 실행 Window를 활성화하십시오. `focus.request`뿐 아니라 `focus.actual`이 true인지 확인하십시오. 실제 focus 상실 callback에서 reparent되면 EXIT OnStart/OnFinished는 모두 0이고 새 parent와 원래 focusable/touch 값을 유지해야 합니다. Window가 비활성이어서 실제 focus를 얻지 못한 실행은 PASS로 처리할 수 없습니다.

- 실제 ChildRemovedSignal listener에서 interaction bit mask(SENSITIVE=1, FOCUSABLE=2, FOCUS_ON_TOUCH=4)는 이미 7이어야 합니다. listener가 세 값을 false로 바꾸면 OnFinished의 mask=0이고 완료 뒤에도 세 값이 false여야 합니다. 늦은 framework 복원이 listener 변경을 덮으면 FAIL입니다.

## Pass/Fail 판정

- 작은 target 좌표는 절대오차 0.001 이하, 실제 rendered animation 좌표는 0.01 이하입니다. progress는 해당 행의 tolerance(0.0001 또는 0.00001)를 사용하십시오. NaN/Inf는 크기와 무관하게 FAIL입니다. count/ID/bool/exception condition은 정확히 일치해야 합니다.
- `LR_CHECK`의 passed 표시만 믿지 말고 actual/expected/tolerance를 다시 대조하십시오. 같은 action에서 필요한 행 수보다 적거나 값이 잘렸거나 같은 행만 중복 수집되면 PASS로 처리하지 마십시오.
- 모든 필수 action이 실행되고 expected checks와 실제 checks가 일치하며, 전체 6개 scenario가 완료되고 누적 failures=0이어야 합니다. 마지막 페이지 또는 마지막 scenario만 PASS인 것은 TC PASS가 아닙니다.
- missing snapshot, observer overflow, 예상 밖 exception/crash/hang는 PASS가 아닙니다. 현재 제품 오류가 나타나도 expected를 actual로 바꾸거나 xfail로 제외하지 마십시오.
- HUD 갱신이 화면 layout을 발생시켜도 이전 step의 고정된 결과는 바뀌면 안 됩니다. 같은 run/step의 행이 다시 출력될 때 값이 바뀌면 observer 오류로 FAIL입니다.

## 정리와 재실행

launcher로 돌아가면 TC가 signal과 transition callback을 끊고 추가 Window와 fixture tree를 해제합니다. focus 변경 TC는 저장한 focus를 복원합니다. 다시 진입했을 때 초기 scenario/step과 새 run ID인지 확인하십시오. 이전 transition의 callback, 남은 ghost, 추가 Window, 이전 실패행이 새 run으로 유입되면 FAIL입니다. 전체 suite의 재진입 검증에서는 실제 launcher 진입→전체 실행→나가기 과정을 반복하고, 내부 subtree 재생성을 실제 진입/퇴장으로 대신 세지 마십시오.

## interaction 속성별 고유 check ID

3개 속성 반복은 `sensitive`, `focusable`, `touch` suffix로 구분합니다. 전체 ID는 `ghost.interaction.disabled.SUFFIX`, `ghost.interaction.restored.SUFFIX`, `listener.change-preserved.SUFFIX`입니다. 속성별 행을 모두 보존하며 같은 action에서 반복한 같은 ID를 별도 검사로 간주하지 마십시오. required_checks는 그대로 유지합니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 6 scenario, 17 action, 104 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `TR09.ghost-0` | `mount` : 4 → `start-and-mutate` : 11 → `finish-or-cancel` : 5 |
| `TR09.ghost-1` | `mount` : 4 → `start-and-mutate` : 11 → `finish-or-cancel` : 5 |
| `TR09.ghost-2` | `mount` : 4 → `start-and-mutate` : 11 → `finish-or-cancel` : 5 |
| `TR09.ghost-3` | `mount` : 4 → `start-and-mutate` : 11 → `finish-or-cancel` : 5 |
| `TR10.focus-reentrant-reparent` | `mount` : 4 → `focused-remove` : 8 |
| `TR10.restore-before-remove-listener` | `mount` : 4 → `remove` : 1 → `listener-result` : 7 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC136 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
