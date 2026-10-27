# 138. Layout: Controller root constraints

이 문서는 [tc-138-layout-controller-root-constraints.cpp](tc-138-layout-controller-root-constraints.cpp)의 신규 TC만 실행하는 절차입니다. 이전 Layout TC의 실행 결과를 이 TC의 증거로 합산하지 마십시오.

## 실행 조건과 조작

1. 검증할 app와 실제로 resolve되는 library의 경로, revision, build-ID 및 build type(Debug/Release)을 기록하십시오. 모든 관측은 공개 API로 수행하므로 별도의 build profile 전환은 없습니다.
2. 검증 artifact의 `manual-tests/dali-ui-foundation/bin/manual-test-dali-ui-foundation`을 실행하고 stdout을 손실 없이 저장하십시오. launcher에서 `138. Layout: Controller root constraints` 항목을 선택하십시오. 격리 build인 경우 해당 artifact 안의 binary를 실행하십시오.
3. `TC138.Reset run`을 한 번 눌러 새 run ID를 기록하십시오. 이후 실패를 지우기 위해 Reset하지 마십시오. 버튼 표시는 `Reset run`이며 Actor name과 accessibility name은 앞의 TC ID를 포함합니다.
4. 각 scenario에서 `TC138.Run scenario`를 누르면 남은 step이 순서대로 자동 실행됩니다(step 하나씩 진행하려면 `TC138.Run step`). `LR_ACTION`의 scenario/step/required_checks가 아래 순서와 일치하는지 먼저 확인하십시오. 각 step이 끝나면 `LR_READY`와 그 step의 모든 행이 자동 출력됩니다.
5. scene 단계는 해당 Window의 View target snapshot과 Window fence까지 기다립니다. 일반적으로 3초 안에 완료됩니다. 완료되지 않아도 조작하지 말고 framework의 자체 timeout을 기다리십시오: layout fence 10초 후 `observer.timeout`, fresh frame 4초 후 `render.frame-timeout`, render settle 4초 후 `render.settle-timeout` 행으로 실패가 기록됩니다. `LR_READY`가 도착하더라도 필수 행 누락이나 행 수 불일치는 PASS가 아닙니다. 그 전에 버튼을 누르면 `LR_NOOP`(step still running)만 남고 step은 계속 실행 중입니다. timeout 행이 나오면 실행 환경과 로그를 함께 보존하십시오.
6. 각 step의 모든 검사 행은 완료 시 stdout에 자동 출력되며 HUD에는 요약과 실패 행만 표시되므로 행 수와 숫자는 stdout에서 빠짐없이 수집하십시오. 모든 step이 끝난 뒤에만 `TC138.Next scenario`를 누르거나, `TC138.Run all`로 남은 scenario를 이어서 자동 실행하십시오. 총 scenario 수는 7개입니다. `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.

추가 창 제목은 `TC138 root constraints` 또는 `TC138 resize`입니다. 조작 버튼과 결과는 원래 manual-test launcher 창에 남아 있습니다. 추가 창이 앞에 나타나면 원래 창의 고유 `TC138.Run scenario`/`TC138.Run step` 식별자로 대상을 선택하십시오. 추가 창에 같은 위치를 클릭하여 다음 단계가 실행되었다고 간주하지 마십시오.

## 단계와 필수 관측 수

`CT01.constraint-0`~`3`: 각각 `run` 8개입니다. `CT01.real-window-resize`: `create` 4개 → `resize` 4개입니다. `resize`는 추가 창의 `SetPositionSize(40,40,400,300)`이 실제 창 크기 변경으로 반영되어야 완료됩니다. 창이 실제로 리사이즈되는 환경에서도 `observer.timeout`이 관측될 수 있으며(개발 환경에서 `xwininfo`로 리사이즈를 확인했는데도 timeout), 원인은 아직 확정되지 않았습니다. 그러므로 window manager 능력 문제로 단정하지 말고 환경·timing 의존으로 기록한 뒤 다른 host/타깃에서 재실행해 판정하십시오. 어떤 경우에도 timeout을 PASS로 기록하지 마십시오. 자세한 근거는 `layout-validation-coverage.md`의 환경/timing 의존 항목을 보십시오. `CT01.root-maximum`: `pass` 10개 → `application-measure` 3개 → `below-maximum` 5개입니다. `CT01.standalone-window-root`: `mount` 8개 → `reconnect` 1개 → `window-resize` 7개입니다.

`Rect`는 x/y/width/height 네 행, `Size`는 width/height 두 행입니다. 일반 scalar/count/bool 검사 하나는 한 행입니다. `LR_CHECK`의 run/scenario/step/action_seq를 `LR_ACTION`과 대조하고 다음 action 시작 전까지의 로그 구간을 함께 저장하여 같은 이름의 행이 서로 다른 step에서 섞이지 않게 하십시오. `LR_RESULT`의 completed_scenarios/required_scenarios, rows와 page 범위를 함께 보관하십시오.

## 독립 기대값과 관측 의미

- 합성 Measure producer는 고정 (80,40)을 반환하는 plain View입니다. built-in manager에 계측 callback을 덮어쓰지 않습니다. margin=(3,7,5,11), requested position=(11,13)이므로 root 위치는 (14,18)입니다.

- mode 0: requested=(300,200), producer constraint=(290,184), 배치 크기=(80,40)입니다. mode 1: parent Actor=(250,180), MATCH_PARENT constraint/배치=(240,164)입니다. mode 2: parent 없음, Window=(320,240), constraint/배치=(310,224)입니다. mode 3: parent Actor 크기 0, constraint/배치=(0,0)이며 Window 크기로 대체하면 FAIL입니다.

- 실제 Window resize scenario는 별도 Window의 MATCH_PARENT root=(0,0,320,240)을 확인한 후 OS 크기를 400×300으로 변경합니다. 해당 Window의 fence에서 root=(0,0,400,300)을 확인하십시오. fence가 완료되지 않으면 환경·timing 의존(원인 미확정)으로 기록하고 PASS로 간주하지 마십시오.

- `CT01.root-maximum`: `CT01.constraint-*`와 같은 margin·요청 위치·합성 producer를 쓰되 root를 MATCH×MATCH, 최대 200×150으로 두고 추가 Window(30,30,320,240)에 직접 붙입니다. 실현된 창 크기를 Rw×Rh(입력)라 하면 MATCH_PARENT root 축은 (window − margin)을 자기 min/max로 clamp한 값에서 EXACTLY로 측정되므로, 전제 `precondition.window-above-maximum`(Rw−10≥200, Rh−16≥150) 아래에서 producer constraint는 (min(Rw−10,200), min(Rh−16,150))=(200,150), measured는 EXACTLY 보고 (200,150), root bounds는 (11+3,13+5,200,150)=(14,18,200,150)입니다(`pass`, 10행). `application-measure`는 pass의 clamp 전 숫자로 `Measure(Rw−10, Rh−16)`을 호출합니다. View 부모가 없는 root는 MATCH_PARENT 축을 EXACTLY로 유도하고 key가 clamp된 (200,150)이므로 cache hit입니다: 결과 (200,150), 합성 producer 증가 0(3행). root의 measure callback은 cache hit에서 호출되지 않으므로 producer 증가 0이 hit의 공개 증거이며, hit/producer event 수는 내부 관측이므로 검사하지 않습니다. `below-maximum`의 `Measure(150,100)`은 새 exact 크기라 producer가 1회 실행되고 결과와 producer constraint 모두 (150,100)입니다(5행).
- `CT01.standalone-window-root`: main window의 Absolute root 아래에 WRAP×WRAP STANDALONE View(합성 producer 80×40, 요청 위치 (11,13), margin (3,7,5,11))를 둡니다. main window 크기 W×H(check 시점 입력)에서 WRAP 축은 window − margin으로 측정되므로 기록된 constraint는 (W−10, H−16), measured (80,40), controller target (14,18,80,40)입니다(`mount`, 8행). `reconnect`(동기)에서 root를 fixture host에서 떼었다가 다시 붙이고 `ProcessLayouts()`하면 producer가 정확히 1회 다시 실행됩니다(1행). fixture host는 View이므로 떼고 붙일 때 host의 `OnChildRemoved`/`OnChildAdded`가 root subtree 전체의 cache를 reset합니다(`ResetSubtreeScaleAndLayoutCaches`). 따라서 이 재측정은 scene 해제 시 window-bounded STANDALONE의 cache drop이 없어도 일어나며, 이 행은 그 drop을 판별하지 않습니다. 그 drop은 내부 cache 상태이므로 이 suite의 어떤 행도 직접 고정하지 않습니다. 이 행이 고정하는 것은 다시 붙인 뒤 부모의 측정·배치와 STANDALONE 자신의 root pass가 모두 같은 window 제약을 써서 producer가 1회로 끝난다는 점입니다. `window-resize`는 fence를 먼저 연결한 뒤 같은 크기로 `OnWindowResize(W,H)`를 호출합니다. 다시 등록된 STANDALONE은 추적되는 root이므로 resize가 그것까지 무효화하여, fence에서 producer가 1회 실행되고(`resize.producer-runs`) target (14,18,80,40), constraint (W−10, H−16)입니다(7행, docs/layout-structure.md STANDALONE 절). measureDirty 같은 내부 flag는 관측하지 않으며, resize가 STANDALONE에 닿지 않으면 producer가 실행되지 않아 이 행이 실패합니다.

## Pass/Fail 판정

- 작은 target 좌표는 절대오차 0.001 이하, 실제 rendered animation 좌표는 0.01 이하입니다. progress는 해당 행의 tolerance(0.0001 또는 0.00001)를 사용하십시오. NaN/Inf는 크기와 무관하게 FAIL입니다. count/ID/bool/exception condition은 정확히 일치해야 합니다.
- `LR_CHECK`의 passed 표시만 믿지 말고 actual/expected/tolerance를 다시 대조하십시오. 같은 action에서 필요한 행 수보다 적거나 값이 잘렸거나 같은 행만 중복 수집되면 PASS로 처리하지 마십시오.
- 모든 필수 action이 실행되고 expected checks와 실제 checks가 일치하며, 전체 7개 scenario가 완료되고 누적 failures=0이어야 합니다. 마지막 페이지 또는 마지막 scenario만 PASS인 것은 TC PASS가 아닙니다.
- missing snapshot, observer overflow, 예상 밖 exception/crash/hang는 PASS가 아닙니다. 현재 제품 오류가 나타나도 expected를 actual로 바꾸거나 xfail로 제외하지 마십시오.
- HUD 갱신이 화면 layout을 발생시켜도 이전 step의 고정된 결과는 바뀌면 안 됩니다. 같은 run/step의 행이 다시 출력될 때 값이 바뀌면 observer 오류로 FAIL입니다.

## 정리와 재실행

launcher로 돌아가면 TC가 signal과 transition callback을 끊고 추가 Window와 fixture tree를 해제합니다. focus 변경 TC는 저장한 focus를 복원합니다. 다시 진입했을 때 초기 scenario/step과 새 run ID인지 확인하십시오. 이전 transition의 callback, 남은 ghost, 추가 Window, 이전 실패행이 새 run으로 유입되면 FAIL입니다. 전체 suite의 재진입 검증에서는 실제 launcher 진입→전체 실행→나가기 과정을 반복하고, 내부 subtree 재생성을 실제 진입/퇴장으로 대신 세지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 7 scenario, 12 action, 74 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `CT01.constraint-0` | `run` : 8 |
| `CT01.constraint-1` | `run` : 8 |
| `CT01.constraint-2` | `run` : 8 |
| `CT01.constraint-3` | `run` : 8 |
| `CT01.real-window-resize` | `create` : 4 → `resize` : 4 |
| `CT01.root-maximum` | `pass` : 10 → `application-measure` : 3 → `below-maximum` : 5 |
| `CT01.standalone-window-root` | `mount` : 8 → `reconnect` : 1 → `window-resize` : 7 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC138 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
