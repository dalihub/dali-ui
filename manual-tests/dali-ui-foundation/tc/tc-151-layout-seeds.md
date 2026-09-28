# 151. Layout: Deterministic seeds

이 TC는 신규 Layout suite의 **Deterministic seeds** 검증입니다. 대응 소스는 `tc-151-layout-seeds.cpp`입니다. 기존 TC의 실행 결과를 사용하지 않습니다.

## 실행 조건

- foundation manual-test 앱에서 `151. Layout`을 검색하고 표시명이 일치하는 TC에 진입하십시오.
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

- `R01.seed.1/17/73/257/65537`: LCG `state=state*1664525+1013904223`를 unsigned32 wrap으로 갱신하여 width=1+state%47, height=1+state%31의 16개 child를 만듭니다. seed는 실행 전 고정됩니다.
- intrinsic width=Σwidth+3×15, height=max(height)입니다. i번째 x=앞선 width의 합+3i입니다. vertical tree는 입력 두 축을 교환하며 y가 같은 합이어야 합니다. 각 seed 필수 130행을 모두 읽으십시오.
- R06–R08의 실패 축소·의도적 결함 주입·source coverage는 suite 보조 도구의 별도 실행 증거입니다. 이 TC의 PASS가 보조 검증까지 수행했다는 뜻은 아닙니다. 원본이 같은 assertion을 PASS하고 mutant가 예상 사유로 FAIL한 경우만 검출력 증거로 집계하십시오.

- 모든 seed scenario는 화면 stage에 부착됩니다. `R01.seed.*`는 한 action에서 horizontal/vertical stack의 렌더 rect를 검사하고, `R02.flex-seed.*`는 `forward` → `reverse` → `reverse-rtl` → `translated-parent`(margin (31,0,17,0)) → `transposed`, `R04.grid-seed.*`는 `ltr` → `rtl` → `translated-parent` → `transposed`, `R05.absolute-seed.*`는 `ltr` → `translated-parent` → `rtl` → `transposed`의 step으로 나뉘며 각 step은 모든 child의 렌더 rect(부모 기준)를 검사합니다. `transposed` step은 첫 step부터 같은 stage에 부착되어 있던 transposed tree의 root Arrange를 invalidate하여 pass를 유도한 뒤 그 fence에서 검사합니다.

`Rect`는 x/y/width/height의 네 필수 행, `Size`는 width/height의 두 필수 행입니다. 일반 geometry 허용오차는 0.001이며 정수·순서·bool·별도로 지정한 exact 항목은 정확히 일치해야 합니다. NaN/Inf는 일반 geometry 비교에서 실패합니다. expected는 입력 표·식에서 계산하며 candidate 출력으로 갱신하지 마십시오.

추가 manager별 고정 seed는 1/73/65537이며 LCG 식은 동일합니다. 다음 9개 scenario도 각각 실행하십시오.

- `R02.flex-seed.*`: 8개 basis width=1+state%47, height=1+state%31, grow1입니다. container main=Σbasis+120이므로 각 child width=basis+15입니다. forward prefix sum, ROW_REVERSE, ROW_REVERSE+RTL의 이중 반전, parent(31,17) 이동 후 local 좌표 보존, COLUMN으로 축 교환을 각각 32행씩 검사합니다. 필수 160행입니다.
- `R04.grid-seed.*`: rows4개=20+state%20, columns3개=15+state%20, row gap3/column gap5입니다. 각 cell 좌표는 prefix track sum+앞선 gap입니다. 12 cell의 LTR/RTL/parent 이동/행열 전치 결과를 각각 48행씩 검사합니다. 필수 192행입니다.
- `R05.absolute-seed.*`: 12 child의 width1.47/height1.31과 고정식 위치를 생성합니다. 홀수 child만 X_PROPORTIONAL이며 p=(state%7−1)/4이고 x=(200−width)p입니다. 짝수는 x=state%50−10, 모든 y=state%30−5입니다. LTR/parent 이동/RTL x=200−x−width/축 전치의 네 묶음 48행씩, 필수 192행입니다. 전치에서는 X_PROPORTIONAL을 Y_PROPORTIONAL로 교환합니다.

이 seed 입력은 각 manager의 명시한 유효 domain을 검증합니다. 실행하지 않은 seed나 다른 입력 domain의 conformance까지 일반화하지 마십시오.

## 근거와 검증 경계

독립 prefix sum oracle과 axis 교환 관계를 고정 seed로 재현합니다. candidate 출력이나 현재 배치 결과를 expected로 저장하지 않습니다.

`Snapshot`은 `LayoutController`가 `LayoutFinished`로 보고한 최종 parent-local target bounds(post-RTL, pre-transition)입니다. `GetArrangedBounds()`의 logical rect(pre-RTL)와 구분하십시오. 행 이름에 `logical`이나 `arranged`가 포함되어도 좌표 의미는 관측 API를 기준으로 판단하십시오. `Rendered` 행(id.x/.y/.width/.height, `*.rendered.*` 포함)은 current scene-graph 상태에서 계산한 화면 사각형을 부모 또는 명시한 base 기준으로 관측한 값이며 RTL 적용 후 좌표입니다. `GetMeasuredSize`는 해당 pass의 측정 결과입니다. controller 완료 snapshot, transition 중간 current geometry, Window의 최종 화면 합성은 서로 다른 관측입니다. callback 측정·배치 횟수는 controller pass가 호출한 synthetic producer의 횟수입니다.

## 종료와 재현

`< Back`으로 나간 뒤 같은 TC에 다시 진입하고 `Reset run` 이후 첫 scenario를 반복하십시오. run ID·counter가 새로 시작하고 초기 기대값이 같아야 합니다. 실패는 scenario/input/action/actual/expected를 함께 보존하십시오. 기존 Layout TC 실행으로 대체하지 마십시오.

## 축소 후보의 별도 재현 절차

이 TC는 JSON을 런타임에 불러오지 않습니다. 자동 suite의 PASS 조건과 실패 분석용 fixture 변경을 구분하십시오. LLM agent가 축소를 수행하려면 원본을 보존한 별도 source/build 복사본을 사용하십시오.

1. 실패한 seed를 위 LCG로 다시 전개하여 실제 입력만 `{"tc":"TC151","scenario":"R01.seed.73","assertion":"horizontal.3.x","seed":73,"nodes":[{"id":0,"width":12,"height":7}]}` 형태로 저장하십시오. 예시 node 수치는 설명용이며 실패 seed의 입력으로 교체해야 합니다. actual Layout 출력은 입력이나 expected에 넣지 마십시오.
2. `python3 layout-validation-tools.py shrink-candidates failure.json > candidates.json`을 실행하십시오. 도구가 만든 `reproduced=false`는 미검증 후보입니다.
3. 별도 복사본의 해당 scenario에서 LCG 생성 loop만 후보 nodes의 명시적 width/height 목록으로 교체하십시오. 두 tree에 같은 목록을 쓰고 node의 원래 id를 assertion 이름으로 보존하십시오. R01의 `total += 45`는 `3*max(N-1,0)`, 두 `i<16`은 `i<N`, required_checks는 `2+8*N`으로 함께 바꾸십시오. 독립 prefix 식과 axis 변환은 유지하십시오. 다른 manager는 해당 MD의 basis/track/proportional 입력과 공식을 같은 방식으로 보존하십시오.
4. 별도 source를 빌드하고 같은 scenario를 UI로 실행하십시오. 보존한 원래 node의 **동일 의미 assertion**이 같은 수치 오류로 실패해야 그 후보를 채택할 수 있습니다. node가 삭제되었거나 실패 의미가 달라지면 동일 재현으로 세지 마십시오. compile error·crash·row-count mismatch는 축소 성공이 아닙니다.
5. 채택한 후보를 다시 입력으로 줄이고 더 삭제할 수 없으면 입력 JSON·fixture patch·binary/source hash·전체 result와 cleanup 증거를 남기십시오. 변경한 분석 fixture는 원래 regression suite manifest에 자동 등록하거나 원본 PASS 결과와 합치지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 14 scenario, 44 action, 2282 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `R01.seed.1` | `run` : 130 |
| `R01.seed.17` | `run` : 130 |
| `R01.seed.73` | `run` : 130 |
| `R01.seed.257` | `run` : 130 |
| `R01.seed.65537` | `run` : 130 |
| `R02.flex-seed.1` | `forward` : 32 → `reverse` : 32 → `reverse-rtl` : 32 → `translated-parent` : 32 → `transposed` : 32 |
| `R04.grid-seed.1` | `ltr` : 48 → `rtl` : 48 → `translated-parent` : 48 → `transposed` : 48 |
| `R05.absolute-seed.1` | `ltr` : 48 → `translated-parent` : 48 → `rtl` : 48 → `transposed` : 48 |
| `R02.flex-seed.73` | `forward` : 32 → `reverse` : 32 → `reverse-rtl` : 32 → `translated-parent` : 32 → `transposed` : 32 |
| `R04.grid-seed.73` | `ltr` : 48 → `rtl` : 48 → `translated-parent` : 48 → `transposed` : 48 |
| `R05.absolute-seed.73` | `ltr` : 48 → `translated-parent` : 48 → `rtl` : 48 → `transposed` : 48 |
| `R02.flex-seed.65537` | `forward` : 32 → `reverse` : 32 → `reverse-rtl` : 32 → `translated-parent` : 32 → `transposed` : 32 |
| `R04.grid-seed.65537` | `ltr` : 48 → `rtl` : 48 → `translated-parent` : 48 → `transposed` : 48 |
| `R05.absolute-seed.65537` | `ltr` : 48 → `translated-parent` : 48 → `rtl` : 48 → `transposed` : 48 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC151 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
