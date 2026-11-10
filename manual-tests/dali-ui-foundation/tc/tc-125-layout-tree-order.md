# 125. Layout: Tree and order

이 TC는 신규 Layout suite의 **Tree and order** 검증입니다. 대응 소스는 `tc-125-layout-tree-order.cpp`입니다. 기존 TC의 실행 결과를 사용하지 않습니다.

## 실행 조건

- foundation manual-test 앱에서 `125. Layout`을 검색하고 표시명이 일치하는 TC에 진입하십시오.
- 다른 입력·animation을 중지하고 `Reset run`을 한 번 실행하십시오. 첫 scenario와 새로운 `run_id`를 기록하십시오.
- 기본 수치는 scale=1, LTR, parent-local 좌표이며 좌표·extent 단위는 visual unit입니다. 각 scenario가 scale/direction을 바꾸면 아래 명시값을 사용하십시오.
- 수치 actual은 TC의 결과 행 또는 같은 run의 stdout 원문에서 읽으십시오. screenshot에서 위치를 눈대중으로 추정하지 마십시오.
- 모든 관측은 공개 API로 수행하므로 별도의 build 옵션이나 capability 준비가 필요하지 않습니다. required row가 누락되면 PASS로 처리하지 마십시오.
- 화면 조건: 앱 window는 480×800 이상이어야 하며 fixture stage는 HUD 아래 fixture 영역에 렌더링됩니다. 큰 fixture는 window 밖으로 이어질 수 있으며 판정은 렌더 좌표(screen extents)로 수행하므로 잘린 화면을 FAIL 근거로 삼지 마십시오.

## 조작 및 판정

1. TC 진입 시 run이 자동으로 시작되며 HUD 첫 줄에 `scenario k/N`, `steps i/M`, 다음 step ID가 표시됩니다. 처음부터 다시 시작하려면 `Reset run`을 누르십시오. `Run scenario`는 현재 scenario의 남은 step을 순서대로 자동 실행하고, `Run step`은 다음 step 하나만 실행하며, `Run all`은 남은 scenario 전체를 자동 실행합니다.
2. 각 step의 관측 경로를 따르십시오. `AfterLayout`은 LayoutController pass와 필수 View의 완료 fence를 기다립니다. `AfterRender`는 그 뒤 새 render task의 완료와 current geometry 반영을 기다립니다. 기본 제한은 layout fence 10초, fresh frame 4초, render settle 4초이며 각각 `observer.timeout`, `render.frame-timeout`, `render.settle-timeout`으로 실패를 기록합니다. 실패 시 마지막 출력·화면을 함께 보존하십시오. 특별한 quiet/시간 조건이 있는 step은 `Run step`으로 하나씩 실행하며 아래 지시가 우선합니다.
3. 각 step이 끝나면 `LR_READY`에 이어 그 step의 `LR_RESULT`와 모든 `LR_CHECK` 행이 stdout에 자동 출력되고 HUD에 요약과 실패 행이 표시됩니다. 별도의 읽기 조작은 없습니다.
4. `required_checks`와 실행 검사 수가 같고 0보다 크며, 모든 필수 행의 actual/expected/오차가 일치해야 해당 step을 통과시킵니다. 누적 실패가 하나라도 있으면 전체 TC를 PASS로 기록하지 마십시오.
5. scenario가 끝나면 `Next scenario`로 이동해 반복하거나 `Run all`로 남은 scenario 전체를 자동 실행하십시오. 생략한 scenario를 통과로 추정하지 마십시오. `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.
6. 끝에서 run/scenario/step, 필수·실행 행 수, 모든 실패 행, stdout 증거를 저장하십시오.

## 시나리오와 독립 기대값

- `ORDER.update/preserve`: a20,b30,c40에서 a를 c 위로 올립니다. `ORDER.update`는 상속된 `Dali::Actor` 호출 `a.RaiseAbove(c)`를 사용하며 논리 순서 b,c,a와 a x70/b x0입니다. `ORDER.preserve`는 `Dali::Ui::Extension::View::RaiseAbovePreservingLayout(a, c)`를 사용하며 논리 순서 a,b,c와 a x0/b x20입니다.
- `ORDER.non-view`: a와 b 사이 장식 Actor를 integration API로 추가합니다. Actor child 수 3, View child 수 2이며 b x20입니다.


- 추가 ORDER.preserve-raise / preserve-lower / preserve-lower-below / preserve-raise-to-top / preserve-lower-to-bottom / preserve-raise-above는 `reorder`가 각각 20행입니다. 초기 Actor와 논리 순서 a,b,c, 각 width20/30/40에서 `Dali::Ui::Extension::View`의 `...PreservingLayout()` 함수를 하나씩 호출합니다(아래 표에서는 `Dali::Ui::Extension::View::`를 생략했습니다). Raise/Lower는 한 단계, RaiseToTop/LowerToBottom은 맨 위/맨 아래, RaiseAbove/LowerBelow는 지정 sibling 바로 위/아래라는 공개 동작을 사용합니다. `ORDER.preserve-raise-above`는 `ORDER.preserve`와 같은 호출이지만 Actor 순서 행을 함께 검사하므로, 호출이 아무것도 바꾸지 않은 경우도 FAIL로 구분합니다.

| scenario | 실제 호출 | expected Actor 순서 | expected 논리 순서 |
|---|---|---|---|
| ORDER.preserve-raise | RaisePreservingLayout(a) | b,a,c | a,b,c |
| ORDER.preserve-lower | LowerPreservingLayout(c) | a,c,b | a,b,c |
| ORDER.preserve-lower-below | LowerBelowPreservingLayout(c,a) | c,a,b | a,b,c |
| ORDER.preserve-raise-to-top | RaiseToTopPreservingLayout(a) | b,c,a | a,b,c |
| ORDER.preserve-lower-to-bottom | LowerToBottomPreservingLayout(c) | c,a,b | a,b,c |
| ORDER.preserve-raise-above | RaiseAbovePreservingLayout(a,c) | b,c,a | a,b,c |

위 표의 각 scenario는 GetChildAt의 실제 Actor 순서 3행과 GetChildViewAt의 논리 순서 3행, 두 child count=3의 2행을 별도로 검사합니다. 이후 실제 rect는 a=(0,0,20,10), b=(20,0,30,10), c=(50,0,40,10)의 12행이어야 합니다. 논리 순서만 맞고 시각 순서가 바뀌지 않는 경우도 FAIL입니다. fresh fixture를 사용하므로 이전 시나리오의 z-order를 expected에 섞지 마십시오.

- `ORDER.rebuild-*`와 `ORDER.keep-*`는 `mount`(12행) → `preserve`(20행) → 마지막 action의 세 단계입니다. `preserve`는 `...PreservingLayout()` 호출 하나로 Actor 순서와 논리 순서를 서로 다르게 만든 뒤 그 상태(바뀐 Actor 순서, 논리 순서 a,b,c, rect 불변)를 검사합니다. 마지막 action은 `...PreservingLayout()`이 아닌 호출 하나입니다. `rebuild`는 논리 순서 전체가 Actor 순서와 같아지는 호출이고, `keep`은 기존 child의 논리 순서가 그대로 남는 호출입니다. d는 마지막 action에서 새로 만드는 15×10 View입니다.

| scenario | `preserve`의 호출과 Actor 순서 | 마지막 action과 호출 | expected Actor 순서 | expected 논리 순서 | 논리 순서대로의 expected x |
|---|---|---|---|---|---|
| ORDER.rebuild-reorder | RaiseToTopPreservingLayout(a): b,c,a | `lower-c`: c.Lower() | c,b,a | c,b,a | c0, b40, a70 |
| ORDER.rebuild-insert-existing | RaisePreservingLayout(a): b,a,c | `insert-c-above-b`: stack.InsertAbove(c,b) | b,c,a | b,c,a | b0, c30, a70 |
| ORDER.rebuild-insert-new | RaisePreservingLayout(a): b,a,c | `insert-d-above-b`: stack.InsertAbove(d,b) | b,d,a,c | b,d,a,c | b0, d30, a45, c65 |
| ORDER.keep-add | RaisePreservingLayout(a): b,a,c | `add-d`: stack.Add(d) | b,a,c,d | a,b,c,d | a0, b20, c50, d90 |
| ORDER.keep-insert-last | RaiseToTopPreservingLayout(a): b,c,a | `insert-d-above-a`: stack.InsertAbove(d,a) | b,c,a,d | a,b,c,d | a0, b20, c50, d90 |
| ORDER.keep-remove | RaiseToTopPreservingLayout(a): b,c,a | `remove-c`: stack.Remove(c) | b,a | a,b | a0, b20 |

`preserve`와 마지막 action은 child 수 n에 대해 2+6n행입니다. 두 child count 2행, slot마다 GetChildViewAt의 논리 순서 1행과 GetChildAt의 Actor 순서 1행, child마다 rect 4행이며 n=3은 20행, n=4는 26행, n=2는 14행입니다. rect의 x는 expected 논리 순서대로 요청 width(a20, b30, c40, d15)를 누적한 값이고 y=0, height=10입니다. `ORDER.rebuild-insert-new`에서 새 View만 Actor 위치에 맞춰 넣고 기존 child의 논리 순서를 그대로 두면 논리 순서가 a,d,b,c(a x0, d x20, b x35)가 되므로 FAIL입니다. `ORDER.keep-remove`의 c는 scene에서 빠지므로 완료 fence에 넣지 않습니다. `...PreservingLayout()` 호출은 layout pass를 요청하지 않으므로, mount 뒤의 모든 action은 형제 View의 요청 width를 1 늘려 pass를 일으킨 뒤 관측합니다.

- `ORDER.update/preserve`는 `mount`(a (0,0,20,10), b (20,0,30,10)) → `raise-a-above-c`(호출 뒤 형제 pass: `ORDER.update`는 a (70,…), b (0,…); `ORDER.preserve`는 불변)이고, `ORDER.preserve-*`는 `mount` → `reorder`(Actor 순서 변경, logical 순서와 렌더 rect 유지)이며, `ORDER.rebuild-*`와 `ORDER.keep-*`는 `mount` → `preserve` → 마지막 action입니다.

`Rect`는 x/y/width/height의 네 필수 행, `Size`는 width/height의 두 필수 행입니다. 일반 geometry 허용오차는 0.001이며 정수·순서·bool·별도로 지정한 exact 항목은 정확히 일치해야 합니다. NaN/Inf는 일반 geometry 비교에서 실패합니다. expected는 입력 표·식에서 계산하며 candidate 출력으로 갱신하지 마십시오.

## 근거와 검증 경계

상속된 `Dali::Actor`의 순서 변경·삽입·추가·제거 호출과 `Dali::Ui::Extension::View`의 `...PreservingLayout()` 함수 6종이 Actor 순서와 논리 순서에 주는 영향, 그리고 non-View exclusion을 각기 실제 API로 검사합니다.

`Snapshot`은 `LayoutController`가 `LayoutFinished`로 보고한 최종 parent-local target bounds(post-RTL, pre-transition)입니다. `GetArrangedBounds()`의 logical rect(pre-RTL)와 구분하십시오. 행 이름에 `logical`이나 `arranged`가 포함되어도 좌표 의미는 관측 API를 기준으로 판단하십시오. `Rendered` 행(id.x/.y/.width/.height, `*.rendered.*` 포함)은 current scene-graph 상태에서 계산한 화면 사각형을 부모 또는 명시한 base 기준으로 관측한 값이며 RTL 적용 후 좌표입니다. `GetMeasuredSize`는 해당 pass의 측정 결과입니다. controller 완료 snapshot, transition 중간 current geometry, Window의 최종 화면 합성은 서로 다른 관측입니다. callback 측정·배치 횟수는 controller pass가 호출한 synthetic producer의 횟수입니다.

## 종료와 재현

`< Back`으로 나간 뒤 같은 TC에 다시 진입하고 `Reset run` 이후 첫 scenario를 반복하십시오. run ID·counter가 새로 시작하고 초기 기대값이 같아야 합니다. 실패는 scenario/input/action/actual/expected를 함께 보존하십시오. 기존 Layout TC 실행으로 대체하지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 15 scenario, 35 action, 559 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `ORDER.update` | `mount` : 8 → `raise-a-above-c` : 10 |
| `ORDER.preserve` | `mount` : 8 → `raise-a-above-c` : 10 |
| `ORDER.non-view` | `run` : 7 |
| `ORDER.preserve-raise` | `mount` : 12 → `reorder` : 20 |
| `ORDER.preserve-lower` | `mount` : 12 → `reorder` : 20 |
| `ORDER.preserve-lower-below` | `mount` : 12 → `reorder` : 20 |
| `ORDER.preserve-raise-to-top` | `mount` : 12 → `reorder` : 20 |
| `ORDER.preserve-lower-to-bottom` | `mount` : 12 → `reorder` : 20 |
| `ORDER.preserve-raise-above` | `mount` : 12 → `reorder` : 20 |
| `ORDER.rebuild-reorder` | `mount` : 12 → `preserve` : 20 → `lower-c` : 20 |
| `ORDER.rebuild-insert-existing` | `mount` : 12 → `preserve` : 20 → `insert-c-above-b` : 20 |
| `ORDER.rebuild-insert-new` | `mount` : 12 → `preserve` : 20 → `insert-d-above-b` : 26 |
| `ORDER.keep-add` | `mount` : 12 → `preserve` : 20 → `add-d` : 26 |
| `ORDER.keep-insert-last` | `mount` : 12 → `preserve` : 20 → `insert-d-above-a` : 26 |
| `ORDER.keep-remove` | `mount` : 12 → `preserve` : 20 → `remove-c` : 14 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC125 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
