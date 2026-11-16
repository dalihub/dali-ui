# 157. Layout: Rendered pixels

이 TC는 layout manager별 current scene geometry와 별도 subtree render의 pixel 색을 검사합니다. 대응 소스는 `tc-157-layout-rendered-pixels.cpp`입니다. geometry가 같은 color 변경도 새 frame과 pixel에 반영되는지 검사합니다.

## 실행 조건

- foundation manual-test 앱에서 `157. Layout`을 검색하고 표시명이 일치하는 TC에 진입하십시오.
- 다른 입력·animation을 중지하고 `Reset run`을 한 번 실행하십시오. 새 `run`과 첫 scenario를 기록하고 stdout/stderr를 모두 보존하십시오.
- 앱 Window는 480×800 이상이어야 합니다. fixture 영역은 HUD 아래에 있습니다.
- 현재 dependency의 `Dali::Capture`는 GLES backend를 요구합니다. backend와 실제 로드된 app/library artifact를 기록하십시오. layout 내부 상태 관측 API는 사용하지 않습니다.
- `Capture`는 source subtree를 별도 camera/framebuffer에 렌더합니다. OS가 제시한 Window 전체 화면을 그대로 읽는 기능이 아니므로 다른 Window의 occlusion, source 밖 조상 clipping, 다른 render task와의 합성을 통과했다고 판단하지 마십시오.

## 조작 및 판정

1. `Run all`을 눌러 6개 scenario를 순서대로 실행하십시오. 단계별 재현에는 `Run step`을 사용합니다. `P01.pixels.*`는 각 1개 action이며 `P02.same-geometry-color`는 `mount`, `color-only`의 2개 action입니다.
2. `P01`과 `P02`의 mount는 View/Window layout fence, 새 `REFRESH_ONCE` render task 완료, geometry settle을 거친 뒤 stage subtree를 pixel Capture합니다. frame fence의 task에는 framebuffer가 없고 pixel을 읽지 않습니다. pixel을 읽는 것은 그 뒤의 `Dali::Capture`뿐입니다. `P02`의 `color-only`는 layout 변경 없이 새 frame을 기다린 뒤 같은 geometry와 변경된 pixel을 검사합니다.
3. 같은 `(tc,pid,run,scenario,step,action_seq)`의 `LR_READY`, `LR_RESULT`, 모든 `LR_CHECK`를 보존하십시오. `LR_READY`는 완료 통지이며 PASS 판정이 아닙니다. 기본 제한은 layout fence 10초, fresh frame 4초(`FRAME_DEADLINE_MS`), geometry settle 4초(`DEFAULT_SETTLE_MS`), pixel Capture 4초이며 경로에서 사용하는 각 단계에 적용됩니다. frame fence에는 시도별 timeout이 없으므로 재시도 예산도 없습니다. readiness가 완료 후 성립하지 않을 때만 같은 deadline 안에서 새 task를 요청합니다. deadline을 넘기면 `render.frame-timeout` 행과 `LR_FRAME` 레코드가 남으며 둘 다 보존하십시오. pixel probe Capture 실패는 선언된 pixel 행에 그대로 FAIL로 기록됩니다.
4. 각 action의 실제 행 수가 아래 `required_checks`와 정확히 같고 모든 행이 PASS여야 합니다. layout/frame timeout, 누락된 행 또는 exception은 PASS가 아닙니다. pixel Capture가 실패하면 해당 probe의 `*.pixel` 행에 실패 사유가 기록됩니다.
5. 마지막 `LR_RESULT`가 `completed_scenarios == required_scenarios`가 될 때까지 기다리십시오. 완료 전 `FAILING`/`INCOMPLETE`는 종결 verdict가 아닙니다. 전체 완료 후 `< Back`으로 나가 같은 run의 `LR_CASE_END`를 수집하십시오.

## 시나리오와 독립 기대값

`P01`의 상자 색은 palette 0/1/2 = `229,57,53` / `30,136,229` / `67,160,71`입니다. 200×100 container의 배경은 kind별 `FixtureColor(kind)`이며 view `255,224,178`, stack `200,230,201`, flex `187,222,251`, grid `248,187,208`, absolute `209,196,233`입니다. stage는 container에 덮이므로 `P01`에서는 container와 상자를 표본으로 삼습니다.

| Scenario | 부모 기준 상자 rect `(x,y,width,height)` |
|---|---|
| `P01.pixels.view` | (10,10,40,30), (70,10,40,30), (130,10,40,30) |
| `P01.pixels.stack` | (0,0,40,30), (60,0,40,30), (120,0,40,30); HORIZONTAL, spacing 20 |
| `P01.pixels.flex` | (0,0,40,30), (40,0,40,30), (80,0,40,30); ROW, FLEX_START |
| `P01.pixels.grid` | (0,0,40,30), (60,0,40,30), (120,0,40,30); 열 폭 40, 간격 20, START |
| `P01.pixels.absolute` | (10,10,40,30), (70,10,40,30), (130,10,40,30) |

각 `P01` action의 16행은 `box.{i}.x/.y/.width/.height` 12행, 각 상자 중심의 `box.{i}.pixel` 3행, (195,95)의 `container.pixel` 1행입니다. 중심 좌표는 독립 expected rect의 `(x+20,y+15)`입니다.

`P02.same-geometry-color`는 80×60 stage 안에 (10,10,40,30)의 상자 하나를 둡니다. `mount`에서 palette 0인 빨강을, `color-only`에서 palette 1인 파랑을 검사합니다. 두 action 모두 `stable.box.x/.y/.width/.height` 4행의 expected는 (10,10,40,30)으로 같습니다. stage 좌표 (30,25)의 pixel은 각각 `stable.initial.pixel` = `229,57,53`, `stable.changed.pixel` = `30,136,229`여야 합니다. geometry가 같다는 이유만으로 이전 frame의 빨강을 통과시키지 마십시오.

pixel 행의 actual/expected는 `r,g,b` 문자열이며 채널별 8-bit 값의 오차가 ±3 이내여야 PASS입니다. geometry는 각 행에 기록된 tolerance를 적용합니다. anti-aliasing 경계는 표본점에서 제외했습니다.

## 근거와 검증 경계

`Rendered`는 current scene-graph screen extents를 부모의 current origin 기준으로 환산합니다. pixel 행은 `Dali::Capture`가 source subtree를 새 task로 렌더한 buffer에서 읽습니다. 두 관측을 함께 사용하지만 Window presentation이나 전체 합성 화면의 증거로 확대하지 마십시오. backend 미지원, source 미연결, capture 실패는 원본 사유를 보존하여 관측 환경 오류로 분류하며 최종 PASS로 처리하지 않습니다.

## 고정 실행 계약과 종료 증거

Profile `release`: 6 scenario, 7 action, 90 checks. 아래 전체 scenario/action이 검토된 manifest와 앱 `LR_CASE_CONTRACT`에서 정확히 일치해야 합니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `P01.pixels.view` | `run` : 16 |
| `P01.pixels.stack` | `run` : 16 |
| `P01.pixels.flex` | `run` : 16 |
| `P01.pixels.grid` | `run` : 16 |
| `P01.pixels.absolute` | `run` : 16 |
| `P02.same-geometry-color` | `mount` : 5 → `color-only` : 5 |

마지막 결과 뒤 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`, `completed_scenarios == required_scenarios`이고 모든 count가 마지막 `LR_RESULT`와 같아야 합니다. failures=0과 이 완료 조건이 함께 성립해야 PASS입니다. END 누락·cleanup 오류는 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 다음 명령을 실행하십시오.

```sh
python3 layout-validation-tools.py check-log LOG --tc TC157 --profile release --manifest MANIFEST --manifest-sha256 PIN
```

`--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. exit 0의 `observations-valid`는 실패 없는 유효 관측이며, `observations-valid-with-failures`(exit 1)는 유효한 실패 행을 `failing_rows`로 반환합니다. `invalid-evidence`(exit 2)는 누락·초과 행이나 종료 불일치 등으로 증거가 계약을 충족하지 못한 결과입니다.

layout/frame timeout은 continuation을 실행하지 못해 필수 행이 누락될 수 있고, settle timeout은 실패 행을 추가하여 계약 행 수를 초과할 수 있습니다. strict 검증은 이를 `invalid-evidence`로 유지합니다. 원본 `observer.timeout`, `render.frame-timeout`, `render.settle-timeout`, `protocol.required_checks`와 도구의 `error`를 함께 기록하고 관측 실패로 보고하십시오. timeout이나 행 수 불일치를 무시하여 PASS로 바꾸지 마십시오.

재현 시 같은 TC에 재진입하여 `Reset run` 뒤 해당 scenario를 반복하고, 새 identity와 최초 실패 기록을 모두 보존하십시오. Reset/Back은 이전 continuation을 차단하지만 Capture handle 해제만으로 GPU task의 즉시 취소를 보장하지 않으므로 이전 task의 자연 종료와 resource 정리도 별도 확인 대상입니다.
