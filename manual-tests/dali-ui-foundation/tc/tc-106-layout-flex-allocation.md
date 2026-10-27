# 106. Layout: Flex allocation

이 TC는 신규 Layout suite의 **Flex allocation** 검증입니다. 대응 소스는 `tc-106-layout-flex-allocation.cpp`입니다. 기존 TC의 실행 결과를 사용하지 않습니다.

## 실행 조건

- foundation manual-test 앱에서 `106. Layout`을 검색하고 표시명이 일치하는 TC에 진입하십시오.
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

- `F06.grow`: basis 20/40, free=90, grow 1:2이므로 width=50/100입니다.
- `F07.shrink`: basis 100/200, deficit 50, shrink 가중치는 100:400이므로 width=90/160입니다.
- `F10.maximum`, `F11.minimum`: grow/shrink slot은 100/50이지만 a는 자기 max 60 / min 80으로 clamp되어 start edge(x=0)에서 60 / 80 폭으로 배치됩니다. measured는 배치 크기와 같은 60/80이고 b의 slot은 재분배 없이 (100,0,100,10) / (50,0,50,10)입니다.
- `F09.match-grow`: fixed 50 뒤 MATCH_PARENT+grow child는 x50, width150입니다.
- `F08.zero-basis`: 명시 basis0은 intrinsic 40/30을 대체하며 grow child만 전체 100을 받습니다.

- `F16.margin-padding`: 120×80 flex(padding (3,5,7,11), alignItems STRETCH)의 grow 1 child(margin (2,4,6,8))는 (5,13,106,48)로 렌더됩니다(content 112×62에서 margin 6/14 제외).

`Rect`는 x/y/width/height의 네 필수 행, `Size`는 width/height의 두 필수 행입니다. 일반 geometry 허용오차는 0.001이며 정수·순서·bool·별도로 지정한 exact 항목은 정확히 일치해야 합니다. NaN/Inf는 일반 geometry 비교에서 실패합니다. expected는 입력 표·식에서 계산하며 candidate 출력으로 갱신하지 마십시오.

- `F15.shrink-clamp`: basis1/100, shrink100/1, container10이면 deficit91을 weighted basis100:100에 45.5씩 배분합니다. 첫 width는max(0,1−45.5)=0, 둘째 54.5입니다. 현재 allocation 정책의 개별 zero clamp 이후 재분배를 임의 가정하지 않습니다.

- `F17.wrap-minimum-grow`: WRAP_CONTENT 폭 flex(min width 100, basis 20/30, grow 1/1, 높이 30)는 measured 100×30이고, arrange가 실제 폭 100으로 free 50을 나누어 a (0,0,45,10), b (45,0,55,10)입니다. measure 시점의 50을 arrange에서 재사용하면 FAIL입니다.
- `F10.grow-maximum-kept`: 300×50 flex(FLEX_START)에서 a=Leaf(60,20) grow 1, b=MATCH_PARENT 폭·높이 20, grow 1, 최대 폭 50입니다. line은 60+0(b의 probe)이므로 free 240을 120씩 나누어 a 180, b의 slot 120이 됩니다. b의 slot은 Measure에서 EXACTLY로 배정되어 최대 폭으로 clamp된 measured (50,20)을 보고하고, x=180에서 50 폭으로 배치됩니다. 남는 70은 a에 재분배되지 않습니다. `first`는 a 렌더 (0,0,180,20), b 렌더 (180,0,50,20), b measured (50,20)이고 `arrange-only`(flex InvalidateArrange)는 저장된 slot을 다시 써서 같은 값입니다. 각 10행입니다.
- `F18.shrink-assigned-exact`: 100×50 flex(FLEX_START)에서 probe a(80×19, shrink 1)와 b=Leaf(60,10)(shrink 1)는 line 140, deficit 40을 basis 가중 80:60으로 나눕니다. a=80−40×80/140=400/7≈57.142857, b=300/7≈42.857143입니다. 좁아진 a는 Measure에서 자기 share에 EXACTLY로 다시 측정되므로 measured (400/7,19), 마지막 constraint 폭 400/7이고, 렌더 a (0,0,400/7,19), b (400/7,0,300/7,10)입니다. 11행입니다.

## 근거와 검증 경계

`flex-layout-manager.cpp`의 basis, grow, basis-weighted shrink, MATCH_PARENT 최종 재측정을 독립 식으로 검증합니다. 한 줄 flex는 결정된 main 크기에서 grow/shrink 분배를 Measure에서 계산하고, 그 share가 배정된 child는 share에서 EXACTLY로 측정되어 자기 min/max로 clamp된 크기를 보고합니다. 배치는 slot의 start edge를 유지하며 clamp로 남거나 모자란 공간을 형제에게 재분배하지 않습니다.

`Snapshot`은 `LayoutController`가 `LayoutFinished`로 보고한 최종 parent-local target bounds(post-RTL, pre-transition)입니다. `GetArrangedBounds()`의 logical rect(pre-RTL)와 구분하십시오. 행 이름에 `logical`이나 `arranged`가 포함되어도 좌표 의미는 관측 API를 기준으로 판단하십시오. `Rendered` 행(id.x/.y/.width/.height, `*.rendered.*` 포함)은 current scene-graph 상태에서 계산한 화면 사각형을 부모 또는 명시한 base 기준으로 관측한 값이며 RTL 적용 후 좌표입니다. `GetMeasuredSize`는 해당 pass의 측정 결과입니다. controller 완료 snapshot, transition 중간 current geometry, Window의 최종 화면 합성은 서로 다른 관측입니다. callback 측정·배치 횟수는 controller pass가 호출한 synthetic producer의 횟수입니다.

## 종료와 재현

`< Back`으로 나간 뒤 같은 TC에 다시 진입하고 `Reset run` 이후 첫 scenario를 반복하십시오. run ID·counter가 새로 시작하고 초기 기대값이 같아야 합니다. 실패는 scenario/input/action/actual/expected를 함께 보존하십시오. 기존 Layout TC 실행으로 대체하지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 11 scenario, 12 action, 101 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `F06.grow` | `run` : 8 |
| `F07.shrink` | `run` : 8 |
| `F10.maximum` | `run` : 10 |
| `F11.minimum` | `run` : 10 |
| `F09.match-grow` | `run` : 4 |
| `F08.zero-basis` | `run` : 8 |
| `F15.shrink-clamp` | `run` : 8 |
| `F16.margin-padding` | `run` : 4 |
| `F17.wrap-minimum-grow` | `run` : 10 |
| `F10.grow-maximum-kept` | `first` : 10 → `arrange-only` : 10 |
| `F18.shrink-assigned-exact` | `run` : 11 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC106 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
