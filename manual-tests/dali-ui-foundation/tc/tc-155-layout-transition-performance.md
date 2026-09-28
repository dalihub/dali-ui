# 155. Layout: Transition work regression

이 문서는 [tc-155-layout-transition-performance.cpp](tc-155-layout-transition-performance.cpp)의 신규 TC만 실행하는 절차입니다. 이전 Layout TC의 실행 결과를 이 TC의 증거로 합산하지 마십시오.

## 실행 조건과 조작

1. 검증할 app와 실제로 resolve되는 library의 경로, revision, build-ID 및 build type(Debug/Release)을 기록하십시오. 모든 관측은 공개 API로 수행하므로 별도의 build profile 전환은 없습니다. 이 TC는 시간을 측정하지 않으며 Debug와 Release 빌드에서 같은 행을 출력합니다.
2. 검증 artifact의 `manual-tests/dali-ui-foundation/bin/manual-test-dali-ui-foundation`을 실행하고 stdout을 손실 없이 저장하십시오. launcher에서 `155. Layout: Transition work regression` 항목을 선택하십시오. 격리 build인 경우 해당 artifact 안의 binary를 실행하십시오.
3. `TC155.Reset run`을 한 번 눌러 새 run ID를 기록하십시오. 이후 실패를 지우기 위해 Reset하지 마십시오. 버튼 표시는 `Reset run`이며 Actor name과 accessibility name은 앞의 TC ID를 포함합니다.
4. 각 scenario에서 `TC155.Run scenario`를 누르면 남은 step이 순서대로 자동 실행됩니다(step 하나씩 진행하려면 `TC155.Run step`). `LR_ACTION`의 scenario/step/required_checks가 아래 순서와 일치하는지 먼저 확인하십시오. 각 step이 끝나면 `LR_READY`와 그 step의 모든 행이 자동 출력됩니다.
5. scene 단계는 해당 Window의 View target snapshot과 Window fence까지 기다립니다. 일반적으로 3초 안에 완료됩니다. 완료되지 않아도 조작하지 말고 framework의 자체 timeout을 기다리십시오: layout fence 10초 후 `observer.timeout`, fresh frame 4초 후 `render.frame-timeout`, render settle 4초 후 `render.settle-timeout` 행으로 실패가 기록됩니다. `LR_READY`가 도착하더라도 필수 행 누락이나 행 수 불일치는 PASS가 아닙니다. 그 전에 버튼을 누르면 `LR_NOOP`(step still running)만 남고 step은 계속 실행 중입니다. timeout 행이 나오면 실행 환경과 로그를 함께 보존하십시오.
6. 각 step의 모든 검사 행은 완료 시 stdout에 자동 출력되며 HUD에는 요약과 실패 행만 표시되므로 행 수와 숫자는 stdout에서 빠짐없이 수집하십시오. 모든 step이 끝난 뒤에만 `TC155.Next scenario`를 누르거나, `TC155.Run all`로 남은 scenario를 이어서 자동 실행하십시오. 총 scenario 수는 30개입니다(반복 sample `TR15` 15개 + work-count `TR15W` 15개). `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.

## 단계와 필수 관측 수

mode 0~4 × node count N=1/32/128에 대해 반복 sample scenario `TR15.mode-M-nN` 15개와 work-count scenario `TR15W.mode-M-nN` 15개, 총 30개를 모두 실행하십시오. 두 계열 모두 `mount`로 시작하며, `TR15`는 `samples`를, `TR15W`는 `work-count`를 이어서 실행합니다. `work-count`가 다시 연결하는 transition callback과 fixture 폭 토글이 반복 sample을 오염시키지 않도록 두 계열은 반드시 별도 scenario로 분리되어 있습니다. 필수 행 수는 다음과 같습니다.

| N | `mount` | `TR15W` / `work-count` | `TR15` / `samples` |
| --- | ---: | ---: | ---: |
| 1 | 7 | 9 | 217 |
| 32 | 162 | 164 | 5022 |
| 128 | 642 | 644 | 19902 |

전체 leaf geometry block은 `5×N+2`행입니다. 두 개의 count 행과 각 leaf의 identity/order 한 행, target x/y/width/height 네 행으로 구성됩니다. `mount`는 block 한 개, `work-count`는 block 한 개와 count 두 행입니다. `samples`는 sample 0~30의 31개 block입니다. N=128에서도 `TR15` scenario의 누적 20544행이 모두 필요합니다. stdout 전체를 수집하는 방법을 사용하십시오.

`Rect`는 x/y/width/height 네 행, `Size`는 width/height 두 행입니다. 일반 scalar/count/bool 검사 하나는 한 행입니다. `LR_CHECK`의 run/scenario/step/action_seq를 `LR_ACTION`과 대조하고 다음 action 시작 전까지의 로그 구간을 함께 저장하여 같은 이름의 행이 서로 다른 step에서 섞이지 않게 하십시오. `LR_RESULT`의 completed_scenarios/required_scenarios, rows와 page 범위를 함께 보관하십시오.

## 독립 기대값과 관측 의미

- mode 0=transition attachment 없음, 1=붙어 있지만 입력 불변인 dormant, 2=CHANGE disabled, 3=active Spec, 4=active Animator입니다. mode 0은 process 전체에 dispatcher가 한 번도 생성되지 않았다는 주장이 아닙니다.

- `mount`는 첫 child만 관측하지 않습니다. N개 leaf 전부를 required target set으로 등록하고 같은 Window fence에서 snapshot을 받습니다. 각 leaf i(0≤i<N)의 target은 `(x=0, y=i, width=80, height=1)`이고 logical child 수와 저장 handle 수가 N이며 `GetChildViewAt(i)`가 생성한 i번째 leaf와 같아야 합니다.

- `work-count`는 transition이 있는 mode에서 `SetOnStart`/`SetOnFinished` lifecycle callback을 연결하고, active Animator(mode 4)에서는 계수용 animator callback으로 바꾼 뒤 update 한 번(dormant mode 1은 ProcessLayouts 한 번)을 실행합니다. 같은 동기 action에서 lifecycle start 수는 mode 3/4만 N이고 0/1/2는 0이어야 합니다(`performance.transition.start-count`). 수동 `ProcessLayouts()`의 마지막 animator tick은 새 animator를 raw 0으로 한 번씩 dispatch하므로 animator callback 수는 mode 4만 N이고 나머지는 0입니다(`performance.first-tick-count`). 그 직후 다음 Layout이나 timer 처리를 하지 않고 전체 leaf target을 검사합니다. update mode 0/2/3/4는 `(0,i,81,1)`, dormant mode 1은 `(0,i,80,1)`이어야 합니다. `performance.work.handles`, `.logical-children`, `.leaf-i.identity-order`와 각 `.target` 네 행을 모두 대조하십시오.

- `samples`는 시간을 측정하지 않습니다. warm-up 없이 `mount`가 확인한 폭 80에서 시작하여 31개 sample을 실행합니다. update mode 0/2/3/4는 sample마다 폭 변경과 ProcessLayouts를 9번, dormant mode 1은 sample마다 ProcessLayouts를 128번 실행합니다. update의 requested width는 81/80을 왕복합니다. 홀수 9회이므로 sample의 마지막 입력은 직전 sample(첫 sample은 mount 상태)과 반드시 다릅니다. 업데이트를 전혀 수행하지 않은 구현이 초기 폭 80을 보존하여 통과하는 것을 막습니다. update mode의 sample 사이에는 20ms 간격이 있어 진행 중인 transition과 실제 frame이 다음 sample의 update와 겹칩니다.

- 각 sample의 마지막 ProcessLayouts 직후 `performance.sample-j` block에서 모든 leaf의 독립 기대값을 확인합니다. update mode의 sample j가 짝수(0,2,...,30)이면 `(0,i,81,1)`, 홀수이면 `(0,i,80,1)`입니다. dormant는 모든 sample에서 `(0,i,80,1)`입니다. 각 sample의 `handles=N`, `logical-children=N`, 모든 `.leaf-i.identity-order=1`도 필수입니다. expected width는 sample 순서에서 정하며 candidate가 반환한 width나 상태 counter로 생성하지 않습니다. 검사는 다음 Update/ProcessLayouts/Delay 전에 수행됩니다. 뒤의 정상 sample이 앞의 실패를 상쇄하지 않습니다.

- 이 target 검사는 기존 public `ViewImpl::GetArrangedBounds()`의 const getter를 사용합니다. getter는 마지막 Arrange의 logical 결과를 복사하며 Arrange, event flush, animation 진행 조작, animation 완료 대기를 수행하지 않습니다. LTR, scale 1인 이 fixture에서는 logical target과 visual target이 같습니다. Spec은 진행 중인 Actor geometry를 되돌려 animate하고, Animator callback은 의도적으로 Actor geometry를 쓰지 않으므로 현재 Actor 좌표를 final Layout target으로 판정하면 잘못된 결과가 됩니다. TC155는 native Spec의 진행 중 geometry나 최종 rendered geometry 성공을 주장하지 않습니다. 해당 기능의 별도 신규 correctness TC는 그대로 필수입니다. sample 뒤 재배치나 animation 진행 조작·완료 대기로 결과를 복구한 다음 값을 읽는 방법은 이 oracle의 증거로 사용할 수 없습니다.

- `samples`와 `work-count`의 행이 모두 있어야 최종 PASS입니다. 이 TC는 외부 증거를 요구하지 않으며 `EXTERNAL_REQUIRED`를 내지 않습니다.

## Pass/Fail 판정

- 모든 leaf target은 x=0, y=0~127, width=80 또는 81, height=1 범위이며 절대오차 0.001 이하만 통과합니다. 0.01 이상의 오차는 검출해야 합니다. NaN/Inf는 크기와 무관하게 FAIL입니다. count/identity/order는 정확히 일치해야 합니다. 이 TC는 intermediate rendered animation 좌표나 progress를 판정하지 않습니다.
- `LR_CHECK`의 passed 표시만 믿지 말고 actual/expected/tolerance를 다시 대조하십시오. 같은 action에서 필요한 행 수보다 적거나 값이 잘렸거나 같은 행만 중복 수집되면 PASS로 처리하지 마십시오.
- 모든 필수 action이 실행되고 expected checks와 실제 checks가 일치하며, 전체 30개 scenario가 완료되고 누적 failures=0이어야 합니다. 마지막 페이지 또는 마지막 scenario만 PASS인 것은 TC PASS가 아닙니다. 반복 sample과 work-count 계열을 모두 충족해야 최종 PASS입니다.
- missing snapshot, observer overflow, 예상 밖 exception/crash/hang는 PASS가 아닙니다. 현재 제품 오류가 나타나도 expected를 actual로 바꾸거나 xfail로 제외하지 마십시오.
- HUD 갱신이 화면 layout을 발생시켜도 이전 step의 고정된 결과는 바뀌면 안 됩니다. 같은 run/step의 행이 다시 출력될 때 값이 바뀌면 observer 오류로 FAIL입니다.

## 정리와 재실행

launcher로 돌아가면 TC가 signal과 transition callback을 끊고 추가 Window와 fixture tree를 해제합니다. focus 변경 TC는 저장한 focus를 복원합니다. 다시 진입했을 때 초기 scenario/step과 새 run ID인지 확인하십시오. 이전 transition의 callback, 남은 ghost, 추가 Window, 이전 실패행이 새 run으로 유입되면 FAIL입니다. 전체 suite의 재진입 검증에서는 실제 launcher 진입→전체 실행→나가기 과정을 반복하고, 내부 subtree 재생성을 실제 진입/퇴장으로 대신 세지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 30 scenario, 60 action, 137900 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `TR15.mode-0-n1` | `mount` : 7 → `samples` : 217 |
| `TR15W.mode-0-n1` | `mount` : 7 → `work-count` : 9 |
| `TR15.mode-0-n32` | `mount` : 162 → `samples` : 5022 |
| `TR15W.mode-0-n32` | `mount` : 162 → `work-count` : 164 |
| `TR15.mode-0-n128` | `mount` : 642 → `samples` : 19902 |
| `TR15W.mode-0-n128` | `mount` : 642 → `work-count` : 644 |
| `TR15.mode-1-n1` | `mount` : 7 → `samples` : 217 |
| `TR15W.mode-1-n1` | `mount` : 7 → `work-count` : 9 |
| `TR15.mode-1-n32` | `mount` : 162 → `samples` : 5022 |
| `TR15W.mode-1-n32` | `mount` : 162 → `work-count` : 164 |
| `TR15.mode-1-n128` | `mount` : 642 → `samples` : 19902 |
| `TR15W.mode-1-n128` | `mount` : 642 → `work-count` : 644 |
| `TR15.mode-2-n1` | `mount` : 7 → `samples` : 217 |
| `TR15W.mode-2-n1` | `mount` : 7 → `work-count` : 9 |
| `TR15.mode-2-n32` | `mount` : 162 → `samples` : 5022 |
| `TR15W.mode-2-n32` | `mount` : 162 → `work-count` : 164 |
| `TR15.mode-2-n128` | `mount` : 642 → `samples` : 19902 |
| `TR15W.mode-2-n128` | `mount` : 642 → `work-count` : 644 |
| `TR15.mode-3-n1` | `mount` : 7 → `samples` : 217 |
| `TR15W.mode-3-n1` | `mount` : 7 → `work-count` : 9 |
| `TR15.mode-3-n32` | `mount` : 162 → `samples` : 5022 |
| `TR15W.mode-3-n32` | `mount` : 162 → `work-count` : 164 |
| `TR15.mode-3-n128` | `mount` : 642 → `samples` : 19902 |
| `TR15W.mode-3-n128` | `mount` : 642 → `work-count` : 644 |
| `TR15.mode-4-n1` | `mount` : 7 → `samples` : 217 |
| `TR15W.mode-4-n1` | `mount` : 7 → `work-count` : 9 |
| `TR15.mode-4-n32` | `mount` : 162 → `samples` : 5022 |
| `TR15W.mode-4-n32` | `mount` : 162 → `work-count` : 164 |
| `TR15.mode-4-n128` | `mount` : 642 → `samples` : 19902 |
| `TR15W.mode-4-n128` | `mount` : 642 → `work-count` : 644 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC155 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
