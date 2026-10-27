# Layout 신규 검증 suite

이 문서는 `manual-tests/dali-ui-foundation/tc/tc-95…tc-157` Layout 회귀 suite(63개 TC)의 구성 결정과 서버 LLM의 실행·판정 기준입니다. 관측은 controller의 event-side 결과, 새 render task 완료 후의 current scene geometry, 별도 subtree render의 pixel, 직접 호출한 계산 결과로 구분합니다. 같은 geometry 수치라도 관측 경로가 다르므로 서로의 증거로 대체하지 마십시오.

## 현재 상태

- 실행 상태: 아래 내용은 수정된 source의 검증 계약입니다. 현재 변경본의 최종 전체 실행과 독립 반복 실행 결과는 별도 실행 보고서로 판정하십시오. 이전 artifact의 실행 합계나 일부 TC의 통과를 현재 변경본의 전체 통과로 사용하지 마십시오. 서버 실행 전 manifest의 `execution_evidence`는 `pending`입니다.
- 기존 Layout TC45–TC94의 C++/MD는 유지하지만 신규 suite의 판정에 합산하지 않습니다. 기존 launcher, `tc/*.cpp` 자동 등록과 module별 앱 구조를 사용합니다. TC별 MD의 검토된 scenario/action/required_checks가 실행 계약입니다.
- TC138 resize 관측은 `SetPositionSize()` 전에 `AfterLayout()` listener를 연결합니다. resize 호출 안에서 layout 완료 signal이 동기 발생하면 호출 후 연결한 listener가 이를 놓치는 것이 이전 timeout의 원인이었습니다. 수정 후에도 timeout이 생기면 새 원본 로그를 보존하고 관측 실패로 처리하십시오.
- transition은 진행 중 geometry를 표본화하지 않습니다. 시작한 action에서 동기 사실(start 수, 수동 `ProcessLayouts()`가 끝날 때 dispatch된 첫 animator context)을 검사하고, 자연 완료는 `AfterFrame()`의 readiness로 lifecycle event를 기다린 뒤 새 render task 완료 후의 끝 상태를 독립 expected와 비교합니다. readiness는 event만 판별하며 expected geometry를 기다리지 않고, frame deadline은 liveness 한도일 뿐입니다. 재실행에서 통과했다고 최초 실패를 삭제하거나 tolerance를 늘리지 마십시오.
- TC152/153은 `StageOnWindow`로 독립 layout root에 workload를 부착하여 HUD의 weighted stack이 측정 횟수에 개입하는 것을 줄입니다. measure mode 도입 후에는 HUD content 영역이 EXACTLY로 측정되어 weight child인 fixture host를 Measure에서 바로 분배하므로 stage subtree가 pass마다 한 번만 측정될 것으로 예측되며(어떤 행도 이를 단정하지 않음), `StageOnWindow`는 workload를 HUD pass와 분리하기 위해 계속 사용합니다. TC154와 TC156의 controller workload는 일반 stage를 사용합니다. 각 sample의 event-side geometry와 마지막 sample 뒤 `VerifyFinalGeometry`의 current scene geometry는 별도 관측입니다. 큰 workload의 fresh frame timeout도 FAIL이며 event-side 값으로 대신 통과시키지 않습니다.
- TC146의 기존 `FixedRecycler` 직접 호출은 layouter 계산 단위 검증으로 유지합니다. 추가 `window-*` scenario는 실제 `RecyclerView`를 controller에 부착하여 양축, cache 범위, variable extent/decoration, scroll 경계, empty/zero viewport를 검사합니다. TC156의 `variable-recycler` sample은 직접 layouter 호출 범위이며, 별도 `window-variable-recycler-n16/n128` scenario가 실제 controller와 current geometry를 검사합니다. 두 범위를 합쳐 모두 화면 렌더 결과라고 설명하지 마십시오.
- TC157은 GLES `Capture`의 별도 subtree render 결과 pixel을 검사합니다. 기본 manager 5종과 geometry를 유지한 color 변경을 다룹니다. OS가 제시한 Window 전체 framebuffer, 다른 Window의 occlusion, source 밖 조상 clipping이나 다른 render task와의 합성을 검증하는 TC는 아닙니다.
- 이 suite는 시간을 측정하지 않습니다. TC152·TC153·TC155·TC156의 반복 sample은 정확성만 검사하며, 시간 표본 출력과 reference/candidate 비교 도구는 없습니다. 도구 명령과 결과 JSON 판정은 [도구 계약](layout-validation-tools.md)을 따르십시오.
- 제품 결함의 출처는 결함별로 분류해서 보고하십시오. suite가 드러낸 결함 중 `ScrollView` animation 종료 시 축소된 범위 밖 종점 복원, `PageScrollView` snap 목표 page의 무clamp commit, `RecyclerView` 비중첩 scroll 점프의 gap 항목 잔류는 이 suite보다 앞선 upstream 결함입니다. 부분 마지막 page 선택의 강등은 같은 branch의 제품 수정 commit이 만들고 이어서 고친 것이며, viewport 전용 resize 경로의 도달 가능성도 그 commit이 만든 것입니다. 출처를 분류하지 않으면 회귀와 기존 결함을 구분할 수 없습니다.

확인된 제품 결함은 expected를 낮추거나 검사 행을 삭제해 숨기지 않습니다. 결함을 드러낸 행은 제품이 고쳐질 때까지 실패로 남기고, 어떤 구현 결함을 가리키는지 행 ID와 함께 기록하십시오. 제품 결함으로 기록된 실패를 테스트 오류로 재분류하려면 메커니즘 수준의 증거를 이 문서에 기록해야 합니다. 실패 값을 쓰는 코드 경로를 특정하고, 그 경로만 바꾸거나 건너뛰었을 때 결과가 달라지는 실험을 함께 남기십시오. 재분류한 행은 원래 의도를 보존해야 하며, 그 의도가 막으려는 제품 결함에서는 여전히 실패해야 합니다. 예상 실패 개수를 맞추는 것은 PASS 기준이 아닙니다.

이 원칙의 예외는 BASELINE 정렬 TC 한 건뿐입니다. 이 TC는 제품에 구현되지 않은 기능(`FlexAlign::BASELINE`의 text baseline 정렬)을 검사했으며, 2026-09-30 이 suite 관리자가 명시적으로 승인한 결정에 따라 행이 아니라 TC 전체를 삭제했습니다. 이 승인은 이 TC에만 적용됩니다. 구현된 기능의 결함을 드러내는 행을 삭제하거나 expected를 낮추는 근거로 사용하지 마십시오.

2026-09-29 재검증에서 이전 리뷰가 제품 결함으로 고정했던 그룹을 위 기준으로 다시 확인했습니다. 이전 분류는 메커니즘을 검증하지 않은 가설이었습니다. 아래 표는 그중 현재 suite에 남은 결정입니다. 나머지 그룹(TC120 standalone slot 소비 bit, TC132 ancestor replay 중 진행 조작, TC143 text control 높이)은 해당 행이 내부 상태 관측을 쓰지 않도록 다시 작성되면서 더 이상 존재하지 않습니다.

| TC / 관측 행 | 재검증 결과 | 메커니즘과 실험 | 조치 |
|---|---|---|---|
| 삭제한 BASELINE 정렬 TC `F14.align-items`, `F14.align-self` / `shared-rendered-baseline` | 제품 기능 미구현 | `FlexLayoutManager`의 교차축 배치(`ArrangeOneFlexLine`)는 `FlexAlign::BASELINE`을 FLEX_START와 같은 offset 0으로 처리하며, layout manager가 자식의 text baseline을 얻는 경로가 제품에 없습니다. 두 Label이 같은 top에 놓여 baseline이 16과 34로 달랐고, debugger로 한쪽 y만 +18 옮기면 행이 통과했습니다. 테스트의 관측은 정확했고, 검사 대상인 정렬 기능이 제품에 없었습니다. | 존재하지 않는 기능을 검사하는 TC이므로 2026-09-30 suite 관리자의 결정으로 이 TC를 삭제했습니다(CPP·MD, manifest의 case와 두 profile 계약). 제품은 바꾸지 않았고 BASELINE은 계속 FLEX_START처럼 배치됩니다. 이 suite는 BASELINE의 text baseline 정렬을 검사하지 않으며, FLEX_START 결과를 BASELINE의 expected로 두는 행도 없습니다. 기능이 구현되면 rendered baseline 일치를 검사하는 TC를 새로 추가하십시오. |

현재 이 문서가 인정하는 알려진 제품 결함 행은 없으므로 manifest에 선언된 63개 TC 모두의 기대 결과는 실패 행 0입니다. `check-log`의 기대 status는 `observations-valid`(exit 0)입니다. 단, `EXTERNAL_REQUIRED`로 끝나는 1개 TC(TC154)는 `--external` 없이 검사하면 `invalid-evidence`(exit 2)입니다. `--external`로 검사하면 `observations-valid`이지만 `external_verification_pending=true`(exit 1)이며, 각 TC MD가 요구하는 외부 증거가 남아 있습니다. 현재 변경본이 이 기대를 만족하는지는 위 실행 상태 항목대로 별도 실행 보고서로 판정하십시오.

실패 행이 하나라도 나오면 회귀 또는 새 결함 후보로 취급하십시오. 행 ID, actual/expected/tolerance, 해당 action의 `LR_CHECK`·`LR_FRAME` 원문과 전체 stdout을 보존하고, 위 기준으로 원인을 확인하기 전에는 expected·tolerance를 바꾸거나 known failure로 등록하거나 재실행 통과로 덮지 마십시오.

TC120 fixture에서 발견되었던 경로 의존 크기(standalone child가 마지막으로 측정한 경로에 따라 높이 80 또는 90으로 배치됨)는 B(6d085cdc)로 해결되었습니다. STANDALONE의 각 축은 어느 pass가 측정하든 하나의 유도식을 따르고, WRAP_CONTENT 축은 window 크기 − 자기 margin으로 측정되므로 이 fixture의 slot은 경로와 무관하게 (7,9,min(300,W),min(90,H))이며 480×800 이상 window에서 (7,9,300,90)입니다. FIXED 200×80 부모를 넘는 90은 문서화된 계약입니다: WRAP_CONTENT Standalone child는 부모 크기로 제한되지 않습니다(docs/layout-structure.md:790-829, `public-api/views/view.h` `SetLayoutMode()`). `path-independence` step이 부모 `InvalidateMeasure()` 뒤에도 같은 slot과 producer 수를 고정합니다([TC120](tc/tc-120-layout-out-of-band.md)).

검증하지 않은 잔여 위험도 기록합니다. 앱이 공개 `LayoutAnimatorCallback`으로 직접 보간하는 transition이 진행 중일 때 bounds가 같은 조상 layout pass가 오면, 그 child의 geometry가 보존되는지는 어떤 TC도 검사하지 않습니다. TC132 `TR14.ancestor-replay-during-enter`는 spec(`LayoutBoundsEffect`) ENTER만 검사하며, TC132·TC135 MD와 [transition map](layout-validation-transition-map.md)은 이 경우를 검사 범위 밖으로 적고 있습니다. 이 경우의 실패가 보고되면 위 기준에 따라 제품 결함 후보로 조사하십시오.

검사하지 않은 알려진 한계도 기록합니다. RecyclerView의 WRAP_CONTENT item에 application이 mode 없는 `Measure()`를 호출하면, layouter가 cross 축을 EXACTLY로 측정한 항목(`internal/recycler/linear-items-layouter-impl.cpp` `MeasureSlot`)과 다른 mode가 유도되어 cache miss가 됩니다. measure mode 변경이 알려진 한계로 남긴 경우이며 `View::Measure()` 문서와 문자 그대로는 어긋나므로, 이 suite는 이를 PASS로 고정하지 않습니다(TC146 `window-wrap-cross-*`는 layouter 자신의 측정만 검사합니다).

measure mode의 다음 두 부분도 검사하지 않은 알려진 한계입니다. 첫째, wrapping Flex(FLEX_WRAP) 부모의 child에 application이 mode 없는 `Measure()`를 호출할 때의 유도(`ViewDataImpl::DeriveMeasureModes`의 FLEX_WRAP 분기: 모든 축 AT_MOST)는 어떤 행도 검사하지 않습니다. TC107 `F19.wrap-match-follower`는 layout pass 자신의 측정만 검사하고, TC115 `K11.application-measure.{k}`는 DEFAULT·STACK·FLEX·GRID·ABSOLUTE·SCROLL 부모만 다룹니다. 둘째, measure mode가 measure cache key의 항이라는 점(`ViewDataImpl::Measure`의 hit 조건이 저장된 mode와 이번 호출의 정규화된 mode를 비교)은 같은 숫자를 다른 mode로 측정하는 step이 없어 직접 고정되지 않습니다. `K11`의 hit은 key에 mode 항이 없어도 hit이므로 이 항을 판별하지 않습니다.

TC143/TC156의 text 기대값은 특정 font에 의존하지 않습니다. 실행 중 공개 `Label::GetNaturalSize()`/`GetHeightForWidth()`로 구하거나 같은 Label의 값끼리 비교하므로 font 설정을 따로 준비할 필요가 없습니다. TC156 text stage는 가장 높은 상태의 높이 H(25)+30으로 잡으며, 최종 geometry는 다른 controller family와 함께 fresh frame 뒤의 current 값을 읽습니다. TC150 effect 관측은 지속되는 render task를 사용하는 fixture이며, 단일 render 후 해제되는 resource와 혼동하지 마십시오.

## 구성 결정과 검증 계약

| 반드시 만족할 조건 | 적용 방식 |
|---|---|
| 기존 manual-tests 구조 유지 | 같은 foundation 앱에 신규 TC 등록, 디렉터리·실행 파일 추가 없음. launcher에는 TC 번호를 숫자로 정렬하는 비교만 추가 |
| 문제를 재현 가능한 작은 단위로 격리 | core/manager/cache/controller/transition/integration/performance별 TC와 scenario 분리 |
| 기존 Layout TC에 의존하지 않음 | 신규 suite에 default View, custom callback/manager, 4개 manager, standalone, scale/direction, lifecycle 포함 |
| 실제 관측 경로 구분 | controller target, current scene geometry, subtree pixel, 직접 계산 결과를 각각 명시 |
| 구현과 독립적인 expected | 입력 표·합/비율/좌표 식·정해진 상태 전이 및 event 순서 사용 |
| 관측의 간섭 억제 | work-count step과 반복 sample step의 분리, leaf producer 계수 callback은 work-budget step에서만 설치 |
| LLM이 실행 및 검증 가능 | 고정 버튼 이름, TC/scenario/step/action 식별자, actual/expected/tolerance, exact required_checks, timeout 및 전체 증거 |

하나의 대형 TC로 모든 상태를 공유하면 이전 실패가 다음 fixture에 영향을 줍니다. 따라서 TC는 분리하되 공통 검증 helper와 기존 앱을 공유합니다. 신규 executable이나 서버 runner는 추가하지 않습니다.

유한한 TC 집합으로 가능한 모든 Layout 오류의 부재를 증명할 수는 없습니다. 완료 기준은 아래 지원 경로의 명시적 oracle, 실행 증거, source branch와 assertion 연결 및 positive-control mutation입니다.

## 검증 모델: 화면 부착과 렌더 결과

화면 fixture를 사용하는 scenario는 다음 관측을 조합합니다. 직접 layouter 호출, detached/out-of-band 측정 및 event-side 전용 행에는 해당 관측 범위를 별도로 적용하십시오.

1. `Run::Stage(width, height)`는 HUD 아래 host에 고정 크기 stage를 부착합니다. 배경 없는 fixture View에는 구별 가능한 색이 부여됩니다. 독립 layout root는 `Run::AttachToWindow` 또는 `StageOnWindow`를 사용합니다. 화면이나 screenshot은 fixture 부착의 보조 증거이며, 서브픽셀 geometry의 수치 판정을 대신하지 않습니다.
2. `Run::AfterRender(views, action)`는 View와 Window의 layout 완료 signal을 먼저 관측합니다(기본 watchdog 10초). 이후 `AfterFrame()`이 window의 1×1 sentinel actor를 source로 하는 새 `REFRESH_ONCE` render task의 완료를 기다립니다(`FRAME_DEADLINE_MS` 4초). 이 task에는 framebuffer가 없으므로 pixel readback도 GPU sync도 없고, 시도별로 만료되는 내부 timeout도 없습니다. 마지막으로 `IsSettled()`가 current extents와 event-side target의 일치를 확인합니다(16ms polling, 최대 `DEFAULT_SETTLE_MS` 4초). 두 상수는 서로 독립입니다. `IsSettled()` 자체는 geometry 동등성 검사이며 frame identity를 증명하지 않습니다. 새 frame 증거는 앞선 task 완료에서 얻습니다.
3. layout이 바뀌지 않는 color 변경이나 transition 완료 대기는 `AfterFrame()`을 직접 사용합니다. optional readiness는 lifecycle event 같은 입력 전달만 확인하고, expected geometry를 만족할 때까지 기다리는 용도로 사용하지 않습니다. readiness는 task 요청 전과 완료 후 모두 성립해야 하며, 완료 후 성립하지 않으면 새 task를 다시 요청합니다. frame callback은 action epoch, fence sequence, attempt 번호로 구분합니다. Reset/Back은 이전 callback을 차단하고 sentinel task를 window의 task list에서 제거합니다. 비정상 완료는 `render.frame-timeout` 행과 `LR_FRAME` 레코드를 남깁니다([도구 계약](layout-validation-tools.md) 참조).
4. frame fence는 scene-graph가 새 render task를 소비했다는 증거이며 OS Window presentation 완료가 아닙니다. pixel을 읽는 관측은 별도로 `Run::CapturePixels`의 `Capture`이며 현재 dependency에서 GLES가 필요합니다(source subtree를 별도 camera/framebuffer에 렌더한 완료 증거). 큰 workload가 Window 밖으로 이어져도 current scene extents는 읽을 수 있지만, 그 노드의 화면 가시성이나 실제 pixel을 검사했다는 뜻은 아닙니다. frame timeout을 event-side fallback으로 통과시키지 마십시오.
5. 반복 workload TC는 pass/sample마다 controller 결과를 event-side로 검사합니다. TC152–154 및 TC156의 controller family는 마지막 pass/sample 뒤 fresh task를 거쳐 current geometry를 별도로 검사합니다. TC156 직접 recycler sample은 controller 경로와 구분하고 대응 integration scenario의 결과를 함께 보존하십시오.

| 관측 | 실제 의미 |
|---|---|
| `Rect(View, ...)`, `Bounds` | event-side 속성. 렌더 완료 증거가 아님 |
| `Snapshot` | View LayoutFinished로 받은 parent-local 최종 actor target. post-RTL, transition 적용 전 |
| `ArrangedBounds` (`ViewImpl::GetArrangedBounds()`) | pre-RTL logical rect |
| `Rendered` / `RenderedBounds` | current scene-graph screen extents를 부모 또는 지정 base의 current origin 기준으로 환산한 값. post-RTL. pixel sampling은 아님 |
| `GetMeasuredSize` / producer count | 측정 결과 및 Measure/Arrange callback 실행 횟수 |
| lifecycle / animator callback | `SetOnStart`/`SetOnFinished` 호출 수와 `LayoutAnimatorContext`(slot/cause/raw/progress/from/to) |
| TC157 `*.pixel` | 별도 subtree Capture buffer의 표본점 RGB. 기대색과 채널별 ±3 이내 |

## 기존 Layout TC45–TC94와의 대응

기존 TC의 통과 기준을 라이브러리 코드 경로 기준으로 대조한 결과입니다. 판정에 합산하지 않는 기존 TC는 그대로 두되, 라이브러리 경로가 비어 있던 항목은 신규 scenario로 옮겼습니다.

| 기존 TC | 신규 대응 |
|---|---|
| 45–52 Absolute | TC112 `A01/A02/A07`, TC113 `A04–A08`, TC103 `C15.standalone-replay.4`, TC97 `match-budget` |
| 53–55 custom callback/manager | TC116/TC117/TC120(콜백), TC99 `C19.custom-manager`(앱 정의 LayoutManager와 `InvalidateOwnerMeasure`), TC103 `C15.standalone-replay.0` |
| 56–64 Flex | TC106 `F06/F07/F12/F15/F16/F17`, TC107 `F02.*/F03/F13`, TC108 `F01/F04/F05`, TC103 `C15.standalone-replay.2` |
| 65–71 Grid | TC109 `G01–G04/G08/G14/G15`, TC110 `G05/G06/G09/G10`, TC111, TC103 `C15.standalone-replay.3` |
| 72–76 Standalone | TC114 `S10.*`, `G14.mode-change` |
| 77 PARK | TC122 |
| 78–82 Transition | TC129, TC131, TC132, TC133, TC134, TC135, TC137; reorder는 TC130의 REORDERED cause까지(TC 내부 drag/proxy/auto-scroll/SVG는 라이브러리 layout 범위 밖) |
| 83–88 Stack | TC104 `S01/S04/S05`, TC105 `S06/S07`, TC103 `C15.LTR-RTL-LTR`·`C15.standalone-replay.1` |
| 89–94 View | TC97 `match-budget/wrap-bounds/wrap-match/C11`, TC95 `C01.padded`, TC138 `CT01`, TC103 `explicit-child`·`C15.standalone-replay.0` |

대응이 없는 것은 TC47의 add-order Z-order(layout 계산이 아닌 actor 그리기 순서), TC57의 `FlexAlign::BASELINE` align-self(제품이 BASELINE을 FLEX_START처럼 배치하는 미구현 기능이며, 같은 이유로 신규 suite의 BASELINE 정렬 TC를 삭제했습니다)와 TC80/82의 gesture 구현부뿐입니다. 창 크기 변경에 따른 manager별 재계산은 root 제약 변경 경로(TC138 `CT01.real-window-resize`, TC130 WINDOW_RESIZED)와 stage 크기 변경 step이 같은 코드를 지나므로 별도 scenario를 두지 않았습니다.

## 지원 경로와 신규 TC 매핑

| 경로 / 실제 source 기준 | 신규 TC | 주 관측 / oracle |
|---|---|---|
| `internal/views/view/view-data-impl.cpp`: Normalize, requested size/position, min/max, margin/padding, Measure/Arrange dispatch, 앱 정의 LayoutManager(`InvalidateOwnerMeasure`) | TC95–101 | 제한값 표, callback 입력·반환, exact/epsilon 경계, 렌더 사각형 |
| effective scale, global scale, inherited LTR/RTL | TC102–103 | scale별 시각 단위, parent-local mirror 식(logical vs 렌더) |
| `public-api/layouts/stack-layout-manager.cpp` | TC104–105 | 고정/weight 잔여 공간, spacing, cross alignment(FILL과 고정 크기), WRAP/clamp |
| `public-api/layouts/flex-layout-manager.cpp` | TC106–108 | basis/grow/shrink, line 분할과 wrap measure, direction/reverse/wrap/RTL, justify/align(BASELINE 제외: 미구현), padding/margin |
| `public-api/layouts/grid-layout-manager.cpp` | TC109–111 | absolute/auto/star track과 wrap measure, span과 auto deficit, clamp/empty/boundary |
| `public-api/layouts/absolute-layout-manager.cpp` | TC112–113 | 고정/비율 축별 독립 식, 음수 위치, wrap, RTL |
| standalone / descendant participation | TC114 | parent 계산 제외와 subtree 내부 계산의 분리 |
| Measure cache, Arrange replay/policy | TC115–117 | 형제 pass를 통한 producer count, key 경계, 손상된 Actor geometry 복원 |
| invalidation / dependency owner / out-of-band | TC118–120 | producer 재실행과 ancestor 재측정, Arrange 시점 재측정 뒤의 cache 유지, out-of-band 호출 뒤 복원 |
| View completion, PARK, reentry/exception, replay 중 tree 변경 | TC121–124 | 동일 episode 신호·순서, quiet interval, 예외 뒤 복구 |
| tree order/reparent/off-scene/Window cleanup | TC125–127 | 각 child target과 새로운 parent 기준값, lifetime 정리 |
| `internal/layouts/layout-transition-dispatcher.cpp` 및 public transition API | TC128–137 | descriptor, scope/cause, bounds/clip, 시작 시점 사실·첫 animator context·자연 완료의 끝 상태, interruption/exit/focus/lifecycle |
| `public-api/layouts/layout-controller.cpp` | TC138–142 | root constraint(실제 window 크기), batching, View→Window 완료, lifetime, exception rollback |
| 실제 Label/ImageView resource 및 중첩 manager | TC143–144 | 기존 res image, 실행 중 유도한 text 기대값, resource fence, A→B→C→A 전체 geometry(snapshot + 렌더) |
| ScrollableBase / ScrollView | TC145 | viewport/content extent, offset/clamp, 실제 scene integration |
| `internal/recycler/linear-items-layouter-impl.cpp` | TC146 | 직접 layouter 계산과 실제 RecyclerView/controller의 visible/cache range, decorated extent, current geometry |
| `internal/recycler/group-linear-items-layouter-impl.cpp`, GroupAdapter | TC147 | flatten type/group/position, body/header/gap margin |
| 실제 RecyclerView / ItemAdapter | TC148 | stable ID, insert/move/remove/content/size/replacement, Window viewport |
| input coordinate/clip 및 RenderEffect resize | TC149–150 | 실제 touch hit/local position, RenderTask framebuffer texture 크기 |
| seed / metamorphic validation | TC151 | 고정 seed와 독립 arithmetic, 축 교환·RTL·translation |
| construction/first pass/warm pass/idle pass | TC152 | 매 sample 전체 geometry, leaf producer budget step |
| sparse/dense/same-value 갱신 | TC153 | 매 sample 전체 geometry, leaf producer budget step |
| 반복 pass/깊이/손상 복원 | TC154 | 매 cycle 전체 노드, 깊이별 닫힌 식, 최종 렌더 집계 |
| 복합 tree/reflow workload | TC156 | deep/balanced, Flex wrap/grow/shrink, Grid star/span, variable Recycler, text reflow |
| transition work | TC155 | absent/dormant/disabled/active spec/animator의 start 수·첫 animator dispatch 수와 매 sample logical target |
| subtree 렌더 픽셀 | TC157 | manager별 solid box 및 동일 geometry color 변경의 pixel과 current 사각형 |
| measure mode·두 단계 측정·arrange clamp: `internal/views/view/view-data-impl.cpp` `DeriveMeasureModes`, `NormaliseMeasureModes`, `CanSkipArrangeRemeasure`, `ClampToOwnSizeBounds`, `HasSizeDependentContent`, `internal/layouts/measure-mode.h` | TC95–TC97, TC99–TC110, TC112, TC113, TC115, TC116, TC119, TC130, TC138, TC143, TC145, TC146 | 할당 축은 slot을 자기 min/max로 clamp해 보고, 크기 없는 축의 두 단계 측정과 빈 slot, start edge(RTL은 오른쪽 끝)를 유지하는 clamp, mode 없는 application `Measure()`의 cache hit(producer 불변), Arrange 재측정 생략의 producer 수 |
| STANDALONE 유도: `internal/layouts/standalone-bounds-utils.h` `DeriveStandaloneMeasureConstraint`/`DeriveStandaloneMeasureModes`, `public-api/layouts/layout-controller.cpp` `ProcessLayoutRoot`·`ReleaseLayoutRootInternal`, view-data-impl.cpp scene 연결/해제·mode 전환 | TC114, TC120, TC126, TC131, TC138, TC139 | 축별 유도식(MATCH=부모 extent−margin, WRAP=window−margin, 명시=요청×scale), 경로 무관 slot과 producer 수, mode 전환 뒤 일반 child 배치, scene 해제 뒤 부모 extent 배치와 재연결 뒤 window 측정, window resize 뒤 producer 재실행, 부모 layout target |

조건별 source와 실제 assertion 연결은 [core map](layout-validation-core-map.md), [transition/controller map](layout-validation-transition-map.md), [workload map](layout-validation-workload-map.md)에 기록합니다. 이 표는 source 경로와 TC 책임의 매핑이며 모든 branch가 실제 실행되었다는 증거표가 아닙니다.

## 실행 조건과 build

이 suite는 공개 API(public-api, extension-api, 기존 integration-api)로만 관측하므로 library에 test 전용 코드나 build 옵션이 필요 없습니다. 모든 TC의 계약은 하나이며 `reviewed_contracts`에는 `release` profile 하나만 있습니다. 남은 구분은 build type뿐입니다.

- **build type**: 시간을 측정하는 TC가 없으므로 Release 빌드를 요구하는 TC도 없습니다. 모든 TC는 Debug와 Release 빌드에서 같은 scenario/step/required_checks 계약을 사용합니다.
- 계약표에 없는 scenario를 실행하거나 판정하지 마십시오. `cases[]`는 TC id와 소스 stem만 나열합니다.
- `ldd`/process maps의 실제 로딩 경로와 ELF build-ID를 저장하여 다른 library를 잘못 검증하지 않도록 하십시오.

manual-tests CMake는 설치된 `dali2-core`, `dali2-adaptor`, `dali2-ui-foundation`, `dali2-ui-components`를 pkg-config로 요구합니다(`PKG_CONFIG_PATH`를 설치 prefix로 지정). 표준 절차는 library를 설치한 뒤 manual-tests를 구성하는 것입니다.

```sh
# 1) library: 설치 prefix에 설치
cmake -S "$SOURCE/build/tizen" -B "$WORK/lib-release" -DCMAKE_INSTALL_PREFIX="$DESKTOP_PREFIX" \
  -DCMAKE_BUILD_TYPE=Release -DENABLE_DEBUG=OFF -DENABLE_PKG_CONFIGURE=ON
cmake --build "$WORK/lib-release" --target dali2-ui-foundation -j4 && cmake --install "$WORK/lib-release"
# 2) app
cmake -S "$SOURCE/manual-tests" -B "$WORK/app-release" -DCMAKE_BUILD_TYPE=Release
cmake --build "$WORK/app-release" --target manual-test-dali-ui-foundation -j4
```

manual-tests의 실행 파일은 어느 build tree에서 빌드하든 source의 `manual-tests/dali-ui-foundation/bin/`에 쓰이므로, 여러 구성을 만들 때는 순서대로 빌드하고 각 바이너리를 따로 보관하십시오. library를 `CMAKE_BUILD_TYPE=Debug`로 구성하면 `ENABLE_DEBUG`가 켜져 Release core에 없는 Log::Filter symbol을 참조할 수 있으므로 Release/RelWithDebInfo를 사용하십시오. Debug library에서는 `DALI_ASSERT_DEBUG`가 DaliException을 던지므로, 무효 입력을 일부러 주입하는 scenario(TC96 `R03.returned-rect`, TC128, TC142 등)는 주입을 step 안의 **동기** `ProcessLayouts()`/setter 호출로만 수행하고 그 호출을 try/catch로 감싼 뒤 다음 pass 전에 주입을 제거합니다. controller의 비동기 pass에서 예외가 나면 앱이 abort되므로 그런 abort는 TC 결함으로 취급하십시오.

## stdout 레코드 문법

앱은 아래 레코드를 stdout에 한 줄씩 출력합니다. JSON 본문은 `LR_READY`와 `LR_EXTERNAL_REQUIRED`를 제외하고 모두 단일 JSON object입니다. 숫자 형식은 항상 `.` 소수점(LC_NUMERIC과 무관)입니다.

| 레코드 | 시점 | 필드 |
|---|---|---|
| `LR_CASE_CONTRACT` | TC 진입/`Reset run` | `tc, pid, run, scenarios[{id, steps[{id, required_checks}]}]` — manifest와 exact 일치해야 함 |
| `LR_ACTION` | step 시작(`Run all`/`Run scenario`/`Run step`) | `tc, pid, run, scenario, step, action_seq, required_checks` |
| `LR_READY` | action 완료 | `tc= pid= run= scenario= step= action_seq= checks= failures=` (key=value) |
| `LR_RESULT` | step 완료 직후(자동), scenario 이동, `Reset run` | `tc, pid, run, scenario, scenario_index, scenario_count, step, step_index, step_count, completed_steps, completed_scenarios, required_scenarios, checks, failures, verdict(PASS/FAIL/EXTERNAL_REQUIRED/FAILING/INCOMPLETE/PENDING), rows` — `PASS`/`FAIL`/`EXTERNAL_REQUIRED`는 **종결** verdict이며 `completed_scenarios == required_scenarios`일 때만 출력됩니다. 완료 전의 상태는 실패가 있으면 `FAILING`, 없으면 `INCOMPLETE`이고, step 실행 중에는 `PENDING`입니다. `FAILING`/`INCOMPLETE`/`PENDING`을 종결 판정으로 읽지 마십시오 |
| `LR_CHECK` | `LR_RESULT` 직후 해당 step의 전체 행(자동) | `tc, pid, run, scenario, row, step, action_seq, kind(numeric/integer/text/color/failure), id, actual, expected, tolerance, passed` |
| `LR_EXTERNAL_REQUIRED` | 외부 검증이 필요한 action | 사유 문자열 |
| `LR_NOOP` | 순서에 맞지 않는 버튼 입력 | `tc, pid, run, button, reason` — 버튼이 무시되었음을 뜻하며 검사 행을 만들지 않음 |
| `LR_CLEANUP_ERROR` | scenario cleanup 예외 | `tc, pid, run, scenario` (stdout과 stderr 모두) |
| `LR_CASE_END` | `< Back`/`Reset run` | `tc, pid, run, completed_scenarios, required_scenarios, completed_steps, failures, cleanup_passed, reason(exit/reset)` — `completed_scenarios == required_scenarios`인지로 그 run이 완료되었는지 판정합니다 |

결과 행은 `(tc, pid, run, scenario, action_seq, row)`로 식별합니다. 같은 행의 재출력은 추가 검사가 아닙니다.

## LLM 실행 절차

1. `python3 layout-validation-tools.py audit`으로 CPP/MD 쌍, MD 계약 합계, 문서가 인용한 manifest SHA-256을 검증하십시오. 이 명령은 앱을 실행하지 않습니다.
2. 환경: `LC_ALL=C`(또는 최소 `LC_NUMERIC=C`)로 앱을 실행하십시오. DPI/scale/해상도/locale, window 크기(480×800 이상)를 기록하십시오. text 기대값은 실행 중 구하므로 특정 font 설정이 필요하지 않습니다.
3. 앱 stdout/stderr를 파일로 보존하고 launcher에서 TC 번호에 `. Layout`을 붙여(예: `95. Layout`) 검색하십시오. TC 진입 시 run이 자동으로 시작되고(HUD에 `scenario k/N`, `steps i/M` 표시), `Run all`은 남은 scenario 전체를, `Run scenario`는 현재 scenario의 남은 step을, `Run step`은 다음 step 하나만 실행합니다. 각 step 완료 시 `LR_READY`, `LR_RESULT`, 그 step의 모든 `LR_CHECK`가 자동 출력됩니다. 버튼 표시는 공통이고 Actor/accessibility name에는 `TCxx.` 접두사(`TC95.Run all` 등)가 있습니다. 자동 quiet 구간이 있는 TC122는 MD가 지시하는 대로 `Run step`으로 진행하십시오.
4. `LR_READY`의 tc/pid/run/scenario/step/action_seq가 현재 step과 일치할 때까지 stdout을 수동적으로 관측하십시오. 이 표시는 step 완료 통지이며 PASS 판정 자체가 아닙니다. timeout은 TC별 MD를 따르십시오. 공통 경로는 layout fence 10초, fresh task 4초, geometry settle 4초의 단계별 제한이며, pixel Capture에는 별도 4초 제한이 있습니다. 완료 직후 그 step의 `LR_RESULT`와 **모든 `LR_CHECK` 행이 자동으로 stdout에 출력**되며 HUD에는 요약과 실패 행만 표시됩니다. 누락된 행을 통과로 추정하지 마십시오. `LR_RESULT`의 verdict는 `completed_scenarios == required_scenarios`가 되기 전에는 종결 판정이 아니므로, `FAILING`/`INCOMPLETE`를 보고 실행을 중단하지 마십시오.
5. `LR_NOOP`가 출력되면 그 버튼은 무시된 것입니다(예: 이전 step이 아직 실행 중인데 `Run step`을 누름, 자동 실행 중 `Next scenario`를 누름). 사유를 확인하고 절차를 이어가되 검사 수에 포함하지 마십시오.
6. 렌더 행(`Rendered`), controller snapshot 행, measured 행을 구별하십시오. 렌더 행은 부모 기준 화면 사각형(RTL 적용 후)이고 snapshot 행은 controller가 완료 signal로 보고한 parent-local 최종 bounds(역시 RTL 적용 후)이며, pre-RTL logical rect는 `ViewImpl::GetArrangedBounds()`로 읽은 행입니다. animation current property는 transition 완료 뒤의 별도 관측입니다. screenshot만으로 서브픽셀 geometry를 판정하지 마십시오. screenshot은 stage 안에 fixture가 그려졌는지의 보조 증거로만 사용하십시오.
7. 일반 async timeout은 각 TC MD를 따르십시오. TC122 `arm-passive-observation`이 끝날 때까지(약 11초) 버튼 입력·pointer 이동·스크린 조작을 하지 마십시오. 수집 시스템이 stdout을 수동적으로 기록하는 것은 가능합니다.
8. 같은 행의 actual/expected/tolerance를 다시 계산하고 required_checks와 실제 행 수의 정확일치, 완료 scenario 수, 누적 실패 수를 확인하십시오.
9. 앱이 EXTERNAL_REQUIRED를 내면(TC154) 해당 MD의 외부 soak 증거가 미완료입니다. observer overflow, timeout은 PASS가 아닙니다.
10. `python3 layout-validation-tools.py check-log LOG --tc TCxx --profile release --manifest MANIFEST --manifest-sha256 PIN`으로 저장된 수치를 다시 검증할 수 있습니다(`--profile`에는 manifest의 유일한 profile인 `release`를 지정합니다). 외부 증거가 필요한 TC154의 internal evidence에 한해 `--external`을 추가할 수 있으며 이 옵션은 external 판정을 완료하지 않습니다. 결과 `status`는 세 가지입니다: `observations-valid`(증거가 유효하고 실패 행 없음, exit 0), `observations-valid-with-failures`(증거는 유효하며 실패 행을 `failing_rows`에 TC별로 나열, exit 1), `invalid-evidence`(증거 자체를 검증할 수 없음 — 누락 scenario/행, 중복 id, 합계 불일치, cleanup 예외, 잘린 레코드 등, exit 2). 실패 행의 존재와 증거의 무효를 구분하십시오: 알려진 제품 결함이 있으면(현재는 없습니다) 그 TC는 `observations-valid-with-failures`이고 `failing_rows`가 그 행과 정확히 일치해야 하며, `invalid-evidence`는 재실행 대상입니다. layout/frame timeout으로 필수 관측이 누락되거나 settle 실패 행이 추가되어 exact 행 수가 깨지면 `invalid-evidence`가 됩니다. `LR_FRAME` 레코드는 계약이 선언했고 이미 시작된 같은 scenario/step의 action에 귀속되어야 하며(그렇지 않으면 `invalid-evidence`), 레코드와 `render.frame-timeout` 행은 어느 한쪽만 있어도 `invalid-evidence`입니다. 레코드가 하나라도 있으면 status는 `observations-valid-with-failures`이고 exit는 0이 아닙니다. 원본 `observer.timeout`, `render.frame-timeout`, `render.settle-timeout` 및 `protocol.required_checks` 행, frame fence가 남긴 `LR_FRAME` 레코드(도구 결과의 `frame_failures`)와 도구의 `error`를 보존하고 관측 실패로 보고하십시오. 이것은 timeout을 무시하는 예외가 아니며 **어떤 경우에도 PASS로 처리하지 않습니다**. 증거를 맞추려고 watchdog이나 계약을 바꾸지 마십시오.
11. TC에서 Back으로 나온 뒤 해당 run의 `LR_CASE_END`를 보존하십시오. `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 일치해야 합니다. 알려진 제품 결함이 있는 TC(현재는 없습니다)는 failures>0으로 종결되며, 그 수가 실패 행 수와 같아야 합니다. END 누락·cleanup 예외는 PASS가 아닙니다. 재진입하여 초기 상태를 검증하십시오.

`PIN`은 이번 candidate 로그에서 계산하는 값이 아닙니다. 운영자가 별도로 검토한 manifest 파일의 SHA256을 신뢰 저장소에 사전 고정하십시오. `reviewed_contracts.profiles`에는 TC별 전체 scenario/step/required_checks가 있고, 도구는 app 출력과 exact equality를 요구합니다. manifest에는 `release` profile 하나만 있습니다. 계약을 바꾸려면 source/MD 변경을 검토한 뒤 새 pin을 배포하십시오. [transition map](layout-validation-transition-map.md)이 인용한 manifest SHA-256은 저장소의 manifest에 맞춰져 있으며 `audit`이 이를 검사합니다.

## 독립 oracle, coverage, mutation

- golden expected를 candidate 출력에서 만들지 마십시오. CPP의 arithmetic fixture와 대응 MD의 표/식을 기준으로 판단하십시오. 기존 Layout TC의 PASS를 신규 scenario의 증거로 합산하지 마십시오.
- 새로 추가되는 Layout API/분기는 source→fixture input→action→actual observer→expected 식→assertion→실행 증거로 연결하여 manifest와 TC를 함께 갱신하십시오. 계약(manifest·MD 표)은 source의 독립 expected와 필수 검사 범위를 검토한 뒤 갱신하고, 컴파일된 앱의 `BuildScenarios()` 출력과 exact 대조합니다. candidate 출력에 맞춰 계약을 자동 승인하거나 pin을 바꾸지 마십시오.
- coverage는 coverage 전용 artifact에서 별도 수집하십시오. launcher/HUD가 우연히 실행한 줄은 해당 TC의 oracle이 검증한 분기로 세지 마십시오.
- positive control은 격리된 복사본에서 한 변이씩 적용하십시오. 예: Stack spacing 항 제거→TC104/TC151, Flex grow 비율 변경→TC106, Grid star 비율 변경→TC109, Absolute `(available-child)*p`를 `available*p`로 변경→TC113, Measure key 비교 제거→TC115, replay geometry write 생략→TC116, RTL mirror 생략→TC103/TC108, ancestor invalidation 전파 생략→TC118, Arrange 시점 재측정의 parent 소유 처리 생략→TC119, transition progress 적용 생략→TC132/134, recycler spacing 제거→TC146, resource invalidation 누락→TC143, out-of-band Measure의 slot 표시 생략→TC120 `K14.external-standalone-measure`의 `corrective-child-producer`, 같은 bounds pass의 진행 중 ENTER 재시작·취소→TC132 `TR14.ancestor-replay-during-enter`, `ClampToOwnSizeBounds` 생략→TC95 `C07.match-clamp`·TC105 `S08`·TC106 `F10`/`F11`·TC103 `C15.clamped-mirror`·TC116 `K21.clamped-arrange-hit`, `DeriveMeasureModes`의 한 parent 분기를 AT_MOST로→TC115 `K11.application-measure.{k}`(DEFAULT k=0, STACK k=1·2, FLEX k=3, GRID k=4, ABSOLUTE k=5, SCROLL k=6)·TC99 `C19.custom-manager-modes`의 `exact`(NONE, custom parent)·TC96 `R04.unbounded-exact`(View 부모 없음; FLEX_WRAP 분기는 위 알려진 한계), WRAP main이 EXACTLY로 측정된 stack이 Measure에서 weight를 분배하지 않게(`StackLayoutManager` Measure의 `isMainAxisWrap`에서 `mainDetermined` 항 제거)→TC105 `S16.weighted-wrap-exact`, stack weight child의 main mode를 Measure 단계에서만 AT_MOST로→TC115 `K11.application-measure.1`의 `producer-once`(Arrange의 weight 재측정이 EXACTLY share로 측정값을 덮으므로 TC104 `S12`의 값은 그대로), Measure·Arrange 두 단계 모두 AT_MOST로→TC104 `S12.weight-exact-share`의 `b`·TC115 `K11.application-measure.1`의 `measured`, `EMPTY_SLOT_TOLERANCE`를 0으로→TC101 `C20.empty-slot`, layouter `MeasureSlot`의 cross mode를 AT_MOST로→TC146 `window-wrap-cross-*`, STANDALONE WRAP 제약을 부모 extent로→TC114 `S10.wrap-window-bound`·TC120, `ReleaseLayoutRootInternal` 생략→TC139 `CT02.mode-switch-cancels-self-turn`의 `switch.default.*`·TC114 `G14.mode-release`의 `release` step(남은 root 등록의 자기 turn이 child를 요청 위치로 다시 배치), 자기 root pass가 부모 actor SIZE를 읽게→TC131 `TR04.standalone-parent-target`, 명시 크기 standalone을 incoming 제약으로 측정→TC114 `S10.explicit-requested`, ScrollView scrollable 크기를 slot으로→TC145 `SC.content-arranged-extent`, Label `OnMeasure`가 EXACTLY로 할당된 폭 대신 자기 폭 clamp 결과에서 높이를 구하게(`widthAssigned`의 EXACTLY 항 제거)→TC143 `text-stretched`의 `conflict` step `stretched.conflict.height`·`stretched.conflict.height-at-width`·렌더 높이(최소 100 > 최대 25에서 H(25) 대신 줄 수가 더 적은 H(100); 같은 scenario의 `fill`/`maximum`/`minimum`은 두 폭의 높이가 같아 판별하지 않음).
- 각 변이는 원본의 **같은 assertion이 PASS**하고 변이에서 **예정한 actual 불일치로 FAIL**해야 kill입니다. compile failure·환경 crash·이미 원본에서 실패하던 baseline assertion(알려진 제품 결함 행 포함)은 mutation kill로 세지 마십시오.
- TC151의 seed 실패는 seed와 실현된 입력 node 목록을 보존하십시오. `shrink-candidates`는 한 node를 삭제한 후보 JSON을 만들 뿐이며, 후보를 TC에서 재현하여 동일 assertion 실패가 유지되는지 확인한 뒤에만 축소된 재현이라고 기록하십시오.

## 관측자 비용과 알려진 한계

TC152/153은 controller pass 종류별 반복 sample과 exact leaf producer budget을, TC155는 transition 상태별 work count와 반복 sample을 분리합니다. 여러 manager/N/phase를 합쳐 회귀를 숨기지 마십시오.

알려진 한계와 관측 실패 판정:

1. 이 suite에는 시간 판정이 없습니다. 반복 sample의 통과를 시간 성능 회귀가 없다는 증거로 사용하지 마십시오.
2. TC156은 geometry를 검사하지만 work-count budget을 계수하지 않습니다. 직접 recycler sample과 실제 RecyclerView integration action도 별도 범위입니다.
3. TC155 active transition의 반복 sample은 진행 중인 transition과 겹치므로 logical target만 판정하며, transition 작업량은 work-count로 판정하십시오.
4. layout/frame timeout은 continuation을 실행하지 못해 필수 행이 부족할 수 있습니다. settle timeout은 실패 행을 추가한 뒤 continuation을 실행하므로 행 수가 초과할 수 있습니다. `check-log`는 두 경우 모두 exact 계약을 유지하여 `invalid-evidence`로 판정합니다. 최초 원인 행과 도구 오류를 함께 기록하고 PASS로 바꾸지 마십시오. TC157 pixel capture 실패는 각 선언 probe에 실패 행을 기록합니다.
5. frame 완료는 sentinel actor를 source로 한 새 render task의 완료이며 OS Window 합성 결과가 아닙니다. 그 task가 그린 것이 아니라 task가 생성된 시점 이후의 update/render 한 쌍이 증거입니다. offscreen 노드의 current geometry 관측도 그 노드의 pixel 가시성을 입증하지 않습니다.
6. `Run::IsSettled`만으로는 동일 geometry의 색·text·clip 변경을 판별할 수 없습니다. fresh task 경로를 유지하고, 실제 색 검증은 TC157 `P02.same-geometry-color`의 pixel 행을 사용하십시오.
7. TC152/153의 leaf producer budget은 leaf만 계수합니다. root·container의 작업량과 HUD drain 범위는 관측하지 않으므로 leaf budget 통과만으로 모든 layout 비용이 없다고 결론 내리지 마십시오.
8. 종료 후 `Append`가 failure count만 증가시키면 실패 행 합계와 다르므로 `invalid-evidence`입니다. 종료·cleanup·재실행의 identity를 함께 보존하십시오.
9. transition은 진행 중 geometry를 표본화하지 않습니다. TC130–137은 시작 시점 사실, 첫 animator context, 자연 완료의 끝 상태만 검사하고, TC134의 delay/cap은 표본 간 불변식(raw 단조 증가, tick당 증가량 상한)으로만 검사합니다. 진행 중 위치·제품 내부 Animation·transition 내부 상태(elapsed, 저장한 interaction, ghost 상태)는 공개 API로 관측할 수 없어 검사하지 않습니다.

## 실행 보고서

보고서에는 fixture/source fingerprint, build type, 전체 scenario/action 목록, 실패와 미실행 구분, 최초 불일치 수치, coverage attribution, mutation kill/미검증 목록을 남기십시오. 새 suite가 현재 제품 오류를 발견하면 expected를 낮추거나 xfail로 숨기지 마십시오. product fix는 별도 변경으로 추적하고 동일 신규 TC를 다시 실행하십시오.
