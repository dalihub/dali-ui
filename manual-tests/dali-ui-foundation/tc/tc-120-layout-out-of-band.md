# 120. Layout: Out of band layout

이 TC는 신규 Layout suite의 **Out of band layout** 검증입니다. 대응 소스는 `tc-120-layout-out-of-band.cpp`입니다. 기존 TC의 실행 결과를 사용하지 않습니다.

## 실행 조건

- foundation manual-test 앱에서 `120. Layout`을 검색하고 표시명이 일치하는 TC에 진입하십시오.
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

- `K14.external-measure`는 14행입니다(`mount` 4, `external` 9, `repeat` 1). parent 200×80 stack의 일반 MATCH_PARENT child를 정상 측정·배치한 뒤(`before.slot`) child만 70×40으로 외부 Measure합니다. producer가 정확히 1회 실행되고(`external.producer`) 그 constraint가 70×40이어야 합니다(`external.constraint`). 형제 leaf 변경으로 유도한 다음 pass에서 parent가 child를 200×80 slot으로 다시 측정해야 합니다(`restored.constraint`). out-of-band Measure가 ancestor cache를 내렸는지는 내부 상태라 관측하지 않습니다. child 자신의 arrange cache도 함께 내려가 그것만으로 parent가 child를 다시 측정하므로, 이 행들은 다음 pass가 slot을 복원하는지만 확인합니다. 렌더 rect (0,0,200,80)도 복원되어야 합니다(`restored`). `repeat`의 형제 pass는 다시 게시된 cache로 처리되어 child producer가 실행되지 않아야 합니다(`repeat.no-producer`). weight 없는 MATCH 주축 child는 producer가 보고한 크기를 유지하므로 `GetMeasuredSize()`=(200,80)은 검사하지 않습니다.
- `K14.external-standalone-measure`는 16행입니다. 부모는 FIXED 200×80 HORIZONTAL stack이며 200×110 stage 안에 있으므로 부모가 받는 incoming 제약은 200×110입니다. child는 STANDALONE WRAP_CONTENT, natural size 300×90, 요청 위치 7/9입니다. `mount`는 렌더 (7,9,200,80)을 확인합니다(`before.slot`). mount settle의 마지막 측정은 부모 pass 뒤에 실행되는 child 자신의 layout root pass입니다. `warmup`은 형제 leaf 변경으로 부모 pass를 한 번 더 실행하여 부모와 child를 함께 정착시키고 렌더 (7,9,200,80)을 확인합니다(`warmup.slot`). 이 부모 pass가 child producer를 다시 실행하는지는 root pass가 남긴 cache key와 부모 arrange 크기의 일치 여부에 달려 있고 제품도 한 번의 재실행을 허용하므로, 횟수는 검사하지 않고 `warmup` 뒤의 producer 수를 n으로 다시 잡습니다. `external`의 Measure(70,40)는 70×40을 반환해야 하고(`external.measure-result`, n+1), 다음 부모 pass는 child를 200×80로 한 번 보정 재측정하여(n+2, `corrective-child-producer`) 렌더 (7,9,200,80)을 복원해야 합니다(`corrected.slot`). `repeat`의 다음 pass는 child를 다시 측정하지 않아 n+2를 유지해야 합니다(`repeat.no-child-producer`). slot 소비 bit와 부모 cache flag는 내부 상태이므로 관측하지 않습니다.
- K14.external-arrange는 16행입니다. child를(75,20,10,5)로 직접 이동한 뒤 동일 parent Arrange가 본래(20,0,30,10)를 복원해야 합니다.

필수 행은 일반 child에서 `before.slot` 4축, `external.producer`, `external.constraint`와 `restored.constraint`의 두 축, `restored` 4축, `repeat.no-producer`입니다. standalone에서는 `before.slot` 4축, `warmup.slot` 4축, `external.measure-result` 두 축, `corrective-child-producer`, `corrected.slot` 4축, `repeat.no-child-producer`를 확인하십시오.

- `K14.external-measure`는 `mount`(200×80 stack의 MATCH probe 렌더 (0,0,200,80)) → `external`(out-of-band `Measure(70,40)`의 producer 1회와 constraint, 형제 leaf 변경으로 유도한 pass 뒤 constraint 200×80과 렌더 (0,0,200,80) 복원) → `repeat`(producer 불변)이고, `K14.external-standalone-measure`는 `mount` → `warmup`(형제 leaf 변경으로 부모 pass 한 번, producer 기준 재설정) → `external`(외부 측정, 보정 재측정으로 producer +2, 렌더 (7,9,200,80)) → `repeat`(producer 불변)입니다. 모두 공개 API와 probe producer 호출 횟수로만 관측합니다. `K14.external-arrange`는 `mount` → `external-arrange`(직접 Arrange 결과는 event-side (75,20,10,5), 다음 pass 뒤 렌더 (20,0,30,10))입니다.

`Rect`는 x/y/width/height의 네 필수 행, `Size`는 width/height의 두 필수 행입니다. 일반 geometry 허용오차는 0.001이며 정수·순서·bool·별도로 지정한 exact 항목은 정확히 일치해야 합니다. NaN/Inf는 일반 geometry 비교에서 실패합니다. expected는 입력 표·식에서 계산하며 candidate 출력으로 갱신하지 마십시오.

## 근거와 검증 경계

parent traversal 외부에서 수행되는 child Measure/Arrange가 cached result와 replay를 망가뜨리지 않는지 검사합니다. internal/views/view/view-data-impl.cpp의 InvalidateAncestorLayoutCachesForMeasureMiss는 dirty나 poison을 세우지 않고 cache만 내리며, 이 TC는 그 결과를 다음 pass의 producer constraint와 호출 횟수로 판정합니다. MeasureStandaloneChildren/ArrangeStandaloneChildren만 standalone slot을 소비하며, 그 내부 bit는 관측하지 않습니다.

이전 버전은 standalone slot의 소비 bit를 내부 상태에서 직접 읽었습니다. 이 bit는 공개 API로 관측할 수 없으므로 이 버전은 같은 경로를 보정 재측정 횟수(`corrective-child-producer`)와 렌더 slot으로만 판정합니다([검증 기준](../layout-validation-coverage.md)).

미해결 설계 질문(별도 조사, 결함으로 분류하지 않음): 이 fixture의 child는 마지막으로 측정한 경로에 따라 높이 80(root pass, 부모 SIZE 제약) 또는 90(부모 재측정, incoming 제약 200×110)으로 배치될 수 있습니다. 입력을 바꾸지 않고 부모 `InvalidateMeasure()`만 호출해도 90이 되어 FIXED 부모 높이 80을 넘습니다. 이 TC는 기존 `before.slot`/`corrected.slot` 행 외에 이 경로의 geometry를 새로 고정하지 않습니다.

`Snapshot`은 `LayoutController`가 `LayoutFinished`로 보고한 최종 parent-local target bounds(post-RTL, pre-transition)입니다. `GetArrangedBounds()`의 logical rect(pre-RTL)와 구분하십시오. 행 이름에 `logical`이나 `arranged`가 포함되어도 좌표 의미는 관측 API를 기준으로 판단하십시오. `Rendered` 행(id.x/.y/.width/.height, `*.rendered.*` 포함)은 current scene-graph 상태에서 계산한 화면 사각형을 부모 또는 명시한 base 기준으로 관측한 값이며 RTL 적용 후 좌표입니다. `GetMeasuredSize`는 해당 pass의 측정 결과입니다. controller 완료 snapshot, transition 중간 current geometry, Window의 최종 화면 합성은 서로 다른 관측입니다. callback 측정·배치 횟수는 controller pass가 호출한 synthetic producer의 횟수입니다.

## 종료와 재현

`< Back`으로 나간 뒤 같은 TC에 다시 진입하고 `Reset run` 이후 첫 scenario를 반복하십시오. run ID·counter가 새로 시작하고 초기 기대값이 같아야 합니다. 실패는 scenario/input/action/actual/expected를 함께 보존하십시오. 기존 Layout TC 실행으로 대체하지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 3 scenario, 9 action, 46 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `K14.external-measure` | `mount` : 4 → `external` : 9 → `repeat` : 1 |
| `K14.external-standalone-measure` | `mount` : 4 → `warmup` : 4 → `external` : 7 → `repeat` : 1 |
| `K14.external-arrange` | `mount` : 8 → `external-arrange` : 8 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC120 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
