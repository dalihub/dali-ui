# 143. Layout: Text and image resources

이 문서는 [tc-143-layout-text-resources.cpp](tc-143-layout-text-resources.cpp)의 실행 절차입니다. 기존 Layout TC 결과는 합산하지 마십시오. 전체 실행 조건은 [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오.

## 실행 및 완료 확인

1. app와 실제 resolve된 library의 build-ID, revision, build type을 기록하고 stdout을 저장하십시오. launcher에서 `143. Layout`을 검색하여 해당 TC에 진입하십시오.
2. `TC143.Reset run`을 한 번 실행하고 새 `run`을 기록하십시오. scenario 내 step 사이에는 Reset하지 마십시오.
3. `Run scenario`(step 하나씩 진행하려면 `Run step`)를 누른 뒤 `LR_ACTION`의 scenario/step/action_seq/required_checks를 기록하십시오. 동기 step은 즉시 완료됩니다. Window/resource step은 해당 episode의 완료까지 기다리십시오. 10초 내 완료되지 않으면 PENDING을 PASS로 처리하지 마십시오. 성능 sample step만 최대 120초를 허용합니다.
4. 각 step 완료 시 자동 출력되는 **모든** `LR_CHECK` 행을 읽으십시오. 행의 tc/pid/run/scenario/step/action_seq를 연결하여 다른 실행의 값과 혼합하지 마십시오. 같은 행을 다시 읽으면 값이 같아야 합니다.
5. 해당 step의 실제 행 수와 required_checks가 정확히 같아야 합니다. 일반 rect 허용오차는 0.001, 정수/bool/ID는 exact입니다. NaN/Inf, 누락/잘림, 중복 읽기로 채운 행 수, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. `passed`만 믿지 말고 actual/expected/tolerance를 재계산하십시오.
6. 모든 step 후 `Next scenario`로 이동하고 모든 scenario를 실행하십시오. 마지막 `LR_RESULT`의 completed_scenarios=required_scenarios, failures=0 및 전체 행 증거가 필요합니다. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오. 실패 후 Reset으로 실패 이력을 지우지 마십시오.

## 입력과 독립 기대값

| scenario / step | 실행 후 필수 관측값 | 검사 수 |
|---|---|---:|
| text-model / single-A | 24px `A`, frame target (0,0,100,120), natural size 양수 | 6 |
| text-model / two-lines | `A\nA`, natural 높이가 직전(1줄) natural 높이보다 큼 | 6 |
| text-model / font-size | 36px로 변경, natural 폭과 높이가 24px 두 줄 natural보다 큼 | 7 |
| text-model / return-A | 24px 단일 A로 복귀, natural = single-A의 natural | 7 |
| text-model / align-center | 세로 정렬 CENTER, natural = single-A의 natural | 7 |
| text-model / align-end | 세로 정렬 END, natural = single-A의 natural | 7 |
| image-247 / wrap → fixed-width → fixed-height → both-fixed | 원본 247×167(`res/heartsframe-plain.png`), 너비 고정 시 494×334, 높이 고정 시 741×501, 양축 고정 80×30 | 8/6/6/7 |
| image-326 / wrap → fixed-width → fixed-height → both-fixed | 원본 326×171(`res/dali-logo-anim-001.png`), 너비 고정 시 652×342, 높이 고정 시 978×513, 양축 고정 80×30 | 8/6/6/7 |
| image-async / load-247 → replace-326 → return-247 | 각 ResourceReady 1회, READY, 실제 Window target=(0,0,247,167)→(0,0,326,171)→(0,0,247,167), scene 연결 | 각 12 |

text 기대값은 특정 font에 의존하지 않습니다. font family를 지정하지 않으며 font 파일 경로, glyph index, advance·ascender·descender 같은 metric 상수를 사용하지 않습니다. 각 step은 변경된 text의 core relayout을 기다린 뒤 공개 `Label::GetNaturalSize()`로 natural size를 읽고 같은 Label의 이전 값과의 관계로 판정합니다. 두 줄은 한 줄보다 높고(`text.natural.two-lines.taller`), 36px은 24px보다 넓고 높으며(`text.natural.font.wider/taller`), 같은 text와 크기로 돌아오면 처음 natural과 같고(`text.natural.restored.*`), 세로 정렬은 natural size를 바꾸지 않습니다(`text.natural.alignment-independent.*`). `text.target`은 Label frame (0,0,100,120)입니다. 줄·glyph 수, text model의 control 크기와 render offset, font identity는 내부 상태이거나 font에 의존하므로 검사하지 않습니다.

비동기 image TC는 URL 변경 전에 action별 ResourceReady count와 child/Window snapshot 상태를 초기화합니다. 요청 이후 child LayoutFinished를 받은 동일 Window fence만 유효한 episode로 기록하고, 다음 fence와 child 관측을 혼합하지 않습니다. ResourceReady와 유효 episode의 도착 순서는 어느 쪽이 먼저여도 허용합니다. 둘 다 관측한 뒤 일회성 100ms event에서 최신 완료 snapshot(image.async.target)과 그 시점의 실제 Actor target(image.current.target)을 각각 새 이미지의 독립 수치와 비교합니다. 대기 중 새 자연 Window fence가 오면 최신 완료 snapshot을 반영하고, 결과를 확정한 뒤에는 overwrite하지 않습니다. image.request.window-observed=true, ready.count=1, status=READY, scene 연결도 필수입니다. 각 action은 12행입니다. ResourceReady 이후 새 fence가 반드시 온다고 가정하지 않으며 TC가 InvalidateMeasure/Measure/Arrange를 추가로 호출하지 않습니다. 10초 내 양쪽 완료를 확보하지 못하거나 frozen/current 값이 새 expected와 다르면 실패 원인을 기록하고 PASS하지 마십시오. 자연 크기 getter는 image visual 생성을 수행할 수 있으므로 이 TC의 수치 조회를 비변이 observer 검증으로 해석하지 마십시오.

## Fixed text의 실제 완료 관측

text-model의 fixed100×120 Label은 SetText/SetFontSize/SetVerticalTextAlignment 뒤 새 LayoutFinished를 요구하지 않습니다. LabelImpl::InvalidateTextMeasure는 WRAP 축이 있을 때만 Measure를 무효화하며 fixed text 내용은 core Relayout 경로로 갱신될 수 있습니다. TC는 setter보다 먼저 action을 arm하고 DevelActor::OnRelayoutSignal에서 fresh count와 Actor rect를 복사합니다. 이 공개 signal은 core ActorSizer::SetNegotiatedSize가 LabelImpl::OnRelayout을 호출한 뒤 emit합니다.

첫 mount는 자연 Window fence도 함께 기다립니다. 이후 text-model step은 새 core Relayout 완료만 요구합니다. target을 고정한 뒤 일회성 1ms event에서, core relayout stack이 풀린 상태로 natural size를 읽습니다. timer는 완료를 만들어내는 방법이 아니며 fresh relayout signal이 없으면 검사 자체를 시작하지 않습니다(`text.relayout.after-action`=true). text-model은 `single-A`/`two-lines` 각 6행, 나머지 4 step 각 7행입니다. 강제 InvalidateMeasure/Arrange 또는 text 렌더링 모드 변경은 하지 않습니다.

`align-center`/`align-end`는 `return-A` 뒤의 같은 Label(24px `A`, 1줄)에 `SetVerticalTextAlignment(CENTER/END)`만 호출합니다. 이 setter는 text relayout을 요청하므로 다음 core Relayout을 기다린 뒤, 정렬이 natural size를 바꾸지 않는지(`text.natural.alignment-independent.width/height`가 single-A의 natural과 같음) 확인합니다. 정렬 offset은 text controller 내부 값이므로 관측하지 않습니다.

## 종료와 증거

`< Back`으로 나간 뒤 다시 진입하여 첫 step을 재실행하십시오. 이전 timer/callback/fixture/추가 Window가 남아 있으면 FAIL입니다. 결과에는 scenario/input/step, 실제값과 기대값, 최초 실패 행, 전체 stdout, artifact fingerprint를 첨부하십시오. `python3 layout-validation-tools.py check-log LOG --tc TC143`은 저장된 행의 누락과 수치를 다시 검사하는 보조 수단입니다. 이 도구는 앱을 실행하거나 서버의 UI 자동 조작을 대체하지 않습니다.

## 추가 intrinsic scenario

- `text-intrinsic`은 7 step, 모두 45행입니다(6/7/7/6/6/7/6). H(w)=`Label::GetHeightForWidth(w)`, N=`Label::GetNaturalSize()`는 Label measure 자체가 쓰는 함수이므로(WRAP 폭은 natural 폭, 높이는 배정되거나 측정된 폭의 H) 모든 기대값을 실행 중 같은 공개 API로 구합니다. 24px A의 WRAP measured는 (N.x, H(N.x))입니다. CHARACTER wrap의 A6개를 MATCH_PARENT로 폭 25 stage에 두면 measured 폭 0, 높이 H(25), 렌더 (0,0,25,H(25))이고 H(25)는 한 줄 A의 natural 높이보다 커야 합니다(`intrinsic.narrow.wraps`). 폭 40이면 높이 H(40)이고 H(40)≤H(25)여야 합니다(`intrinsic.wide.not-taller`). requested width25는 25×H(25), text를 A로 바꾸면 25×H(25), WRAP+font36이면 (N36.x, H(N36.x))이고 N36은 24px natural보다 폭과 높이가 모두 커야 합니다(`intrinsic.font.larger`). empty text는 0×0입니다. 같은 함수에 같은 입력을 쓰므로 기본 허용오차 0.001을 사용합니다.
- 특정 한글 font의 cmap·advance를 고정하던 scenario는 삭제했습니다. 그 대상이 font 파일의 identity였기 때문입니다.

`image-failure-recovery`는 `invalid` 7행과 `recovered` 6행입니다. 존재하지 않는 경로(`res/layout-validation-absent.png`)의 resource에서 fixed Measure 80×30, status FAILED, Arrange(3,5,80,30)를 확인하고, 유효한 326×171 이미지(`res/dali-logo-anim-001.png`)로 교체하고 WRAP으로 변경한 Measure 326×171과 렌더 (3,5,326,171)을 확인하십시오. 실패 resource용 파일을 추가하지 않기 위해 없는 경로를 사용합니다(`res/`에서 이미지가 아닌 파일은 Lottie JSON뿐이며 정상적으로 READY가 됩니다).

## 단계 구성

- `text-intrinsic`은 `natural`(300×300 stage: (N.x, H(N.x))) → `narrow`(AAAAAA, CHARACTER wrap, MATCH_PARENT, stage 폭 25: measured 폭 0, 높이 H(25), 렌더 (0,0,25,H(25)), 줄바꿈 관계) → `wide`(stage 폭 40: H(40), H(40)≤H(25)) → `fixed-width`(요청 폭 25, stage 300) → `changed-text`(A: 25×H(25)) → `font-size`(WRAP, 36px: (N36.x, H(N36.x)), 24px보다 큼) → `empty`(0×0)의 7 step이며, 각 step은 measured 2행 + 렌더 4행이고 관계 행이 있는 step은 1행을 더합니다.
- `image-247`/`image-326`은 `wrap`(natural 2행 + measured + 렌더) → `fixed-width` → `fixed-height` → `both-fixed`(+status)의 4 step이고, `image-failure-recovery`는 `invalid`(80×30, margin (3,0,5,0): 렌더 (3,5,80,30), status FAILED) → `recovered`(326×171)입니다. 큰 고정 크기는 300×300 stage 밖으로 이어질 수 있으며 판정은 렌더 좌표로 수행하므로 잘린 화면을 FAIL 근거로 삼지 마십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 6 scenario, 26 action, 188 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `text-model` | `single-A` : 6 → `two-lines` : 6 → `font-size` : 7 → `return-A` : 7 → `align-center` : 7 → `align-end` : 7 |
| `text-intrinsic` | `natural` : 6 → `narrow` : 7 → `wide` : 7 → `fixed-width` : 6 → `changed-text` : 6 → `font-size` : 7 → `empty` : 6 |
| `image-247` | `wrap` : 8 → `fixed-width` : 6 → `fixed-height` : 6 → `both-fixed` : 7 |
| `image-326` | `wrap` : 8 → `fixed-width` : 6 → `fixed-height` : 6 → `both-fixed` : 7 |
| `image-async` | `load-247` : 12 → `replace-326` : 12 → `return-247` : 12 |
| `image-failure-recovery` | `invalid` : 7 → `recovered` : 6 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC143 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
