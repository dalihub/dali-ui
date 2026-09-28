# 156. Layout: Complex layout performance

[tc-156-layout-complex-performance.cpp](tc-156-layout-complex-performance.cpp)의 신규 fixture만 실행합니다. 기존 Layout TC 실행 결과로 대체하지 마십시오. 공통 실행·증거 규칙은 [suite 검증 기준](../layout-validation-coverage.md)을 사용합니다.

## 실행 방법

1. 실행 전에 app 및 resolve된 library의 revision/build-ID, build type, 해상도/DPI를 저장하십시오. 이 TC는 시간을 측정하지 않으며 Debug와 Release 빌드에서 같은 행을 출력합니다.
2. launcher에서 `156. Layout`을 검색하여 해당 TC에 진입하고 `Reset run`을 한 번 실행합니다. 새 run ID를 기록하십시오. `Run scenario`(또는 `Run step`)를 누르고 LR_ACTION의 scenario/step/action_seq/required_checks를 기록하십시오.
3. 16개 family scenario에서는 `samples` step 하나를 실행합니다. 추가된 `window-variable-recycler-n16/n128`은 아래 명시된 여섯 correctness action을 실행합니다. 일반 manager의 leaf producer exact budget은 TC152/153의 work step이 검사합니다.
4. `samples` step은 최대 120초를 허용합니다. 시간 초과·PENDING·unexpected exception·crash는 PASS가 아닙니다.
5. 각 step 완료 시 자동 출력되는 모든 행을 stdout 원문에서 읽습니다. actual/expected/tolerance를 직접 대조하며 NaN/Inf, 누락·잘림, 다른 run의 행 혼합, 행 수 부족, cleanup 오류는 FAIL입니다. 같은 행을 반복 읽어 required_checks를 채우지 마십시오.
6. `Next scenario`로 아래 18개 scenario를 모두 실행합니다. window integration 12개를 포함하여 총 28 step입니다. 종료 후 다시 진입하여 첫 step을 재실행하고 이전 fixture/callback이 남지 않는지 확인하십시오. `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.

## 독립 입력과 기대값

각 family를 N=16/128에서 실행합니다. deep의 N은 root 아래 depth, balanced의 N은 leaf 수입니다. 계산 트리의 모든 중간 container도 검증 대상입니다. fixture는 화면의 stage에 부착되어 초기 `ProcessLayouts()`로 settle됩니다(variable-recycler는 고정 Recycler를 사용하는 layouter 경로이며 stage가 없습니다). 각 sample은 독립 expected 배열을 만든 뒤 setter 또는 cross constraint 변경과 controller pass(`ProcessLayouts()`) 또는 ItemsLayouter::OnLayoutChildren을 한 번 실행합니다. warm-up 없이 31개 sample(j=0…30)을 모두 검사하며, state는 초기 0에서 sample 0의 1부터 0/1을 번갈아 바꿉니다.

| family | 왕복 입력 | 수치 oracle |
|---|---|---|
| deep-dirty | 마지막 leaf height20↔21 | root를 포함한 N+1개 node의 width120, height20/21, local x=y=0 |
| balanced-dirty | 오른쪽 끝 leaf height20↔21 | binary vertical Stack, 간격 2. k leaf subtree 높이 22*k−2에 변경 branch만+1. 두 번째 child local y는 첫 subtree 높이+2. root height22*N−2 또는+1 |
| flex-wrap | root width40↔50 | basis10, height16, grow/shrink0. columns4/5, item i의 x=10*(i%columns), y=16*floor(i/columns), width10/height16 |
| flex-grow | root width12*N↔16*N | basis8, grow weight1/3 교대, 총 weight2*N. item width=8+2*weight 또는 8+4*weight, x는 앞 item width의 누적 합 |
| flex-shrink | root width12*N↔8*N | basis16, shrink weight1/3 교대. item width=16−2*weight 또는 16−4*weight, x는 누적 합 |
| grid-star-span | root width250↔375 | columns STAR1/2/1/1, N개의 STAR1 row, root height20*N. unit=width/5. 각 row의 첫 item=(0,20*row,3*unit,20), 둘째=(3*unit,20*row,unit,20), 마지막 item=(4*unit,0,unit,20*N)이며 전체 row span |
| variable-recycler | cross120↔160 | 실제 LinearItemsLayouter integration API, item height10/20/30 반복, decoration left3/right7/top2/bottom4, spacing5. item x3,width=cross−10,y=앞 item들의(height+11) 합+2. range=각(height+6)의 합+5*(N−1), offset0, first0,lastN−1. content bounds도 decoration 포함한 별도 rect로 검사 |
| text-reflow | fixed width25↔40 | Label 24px, 문자열 A를 N회, CHARACTER wrap, requested height WRAP_CONTENT. 높이 expected는 각 sample 전에 같은 Label의 `GetHeightForWidth(width)`로 구합니다(명시 폭과 WRAP 높이에서 Label measure가 계산하는 값과 같은 함수). font family, glyph advance, 줄 높이 상수를 가정하지 않습니다. pass 뒤 measured/arranged rect를 검사하고, 마지막에 H(25)≥H(40)(`final.text.narrow-not-shorter`)을 확인합니다 |

각 sample의 `sample.immediate.*`는 scalar values 개수, mismatch 0, maximum_error 0의 3행입니다. pass 직후의 measured size와 Actor geometry를 다음 pass 전에 읽습니다. 보정용 pass를 실행한 뒤 오류를 숨기는 방법은 허용하지 않습니다. 마지막 sample 뒤 새 `REFRESH_ONCE` render task 완료를 거쳐 root와 모든 node의 current rect x/y/width/height를 개별 행(`final.root.*`, `final.node.i.*`)으로 출력합니다. stage가 없는 Recycler는 item과 content bounds를 event-side rect로 검증합니다.

| family | 최종 detail 행 D | `samples` step required_checks |
|---|---:|---:|
| deep-dirty |4*(N+1)|93+D|
| balanced-dirty |4*(2*N−1)|93+D|
| Flex 세 family |4*(N+1)|93+D|
| grid-star-span |4*(2*N+2)|93+D|
| variable-recycler |4*N|93+D|
| text-reflow |5|98|

text-reflow의 detail 5행은 `final.root.*` 4행과 font와 무관한 관계 `final.text.narrow-not-shorter`(좁은 폭이 줄 수를 줄이지 않음) 1행입니다. stage 높이는 가장 높은 상태(폭 25)의 H(25)+30으로 잡습니다. text shaping 엔진 전체 기능의 검증 결과로 확대하지 마십시오.

## 실제 RecyclerView correctness companion

RecyclerView는 AbsoluteLayout의 명시적인 bounds slot으로 viewport 제약을 받습니다. 외부 200×160 stage는 화면 관측을 위한 공간이며 실제 viewport 크기로 사용하지 않습니다.

`window-variable-recycler-n16`과 `window-variable-recycler-n128`은 각각 `prime-cache` → `viewport` → `middle` → `end` → `wider` → `start`를 실행하며 action마다 23행입니다. 이 12개 action은 direct `variable-recycler` sample의 event-side geometry 검사와 함께 실제 adapter/controller/frame 경로를 검증합니다.

- fixture의 실제 viewport는50, cross는120입니다. heights는10/20/30 반복, decoration은 left3/right7/top2/bottom4, spacing5로 direct `variable-recycler` fixture와 같습니다. item i의 slot start는 앞 item의 `(height+11)` 합, rect는 `(3,slotStart+2,cross−10,height)`입니다. range는 N16에서481, N128에서3953입니다.
- `prime-cache`는 after-cache100000으로 모든 N개 holder를 실제 RecyclerView를 통해 측정합니다. viewport 크기는50을 유지합니다. 모든 materialized item의 current parent-local rect를 검사하고 실제 viewport와 겹치는 item만 screen geometry를 검사합니다. 이는 준비/정확성 action입니다.
- `viewport`는 cache0으로 되돌리고 offset0의 active holder 집합을 검사합니다. `middle`은 item N/2의 독립 slot start로 이동합니다. `end`는 큰 offset을 요청하여 `range−50`으로 clamp되는지 확인합니다. `wider`는 cross160으로 변경하여 item width가150이 되는지 확인하고 offset은 유지합니다. `start`는 offset0으로 복귀합니다.
- 각23행은 viewport geometry/scene 연결, catalog count, range/public offset/layouter offset/extent, first/last, materialized count 및 정확한 index 집합, stable ID, 모든 materialized current-local geometry, visible screen geometry, 실제 visible 검사 수, maximum error, scroller 존재/current x/y, idle을 검증합니다. screen expected는 content rect에서 scroll offset을 뺀 값입니다. mismatch/maximum error는0이어야 합니다.
- 각 action은 입력 후 새로운 `REFRESH_ONCE` render task의 완료로 관측을 시작하며 expected geometry가 나올 때까지 기다리지 않습니다. cache 밖 item을 실제 pixel 관측했다고 주장하지 마십시오. 이 검사는 Window compositor의 다른 task/occlusion까지 입증하지 않습니다. timeout·missing holder·frame 오류는 PASS가 아닙니다.

## 판정

최종 PASS에는 TC156 전체 geometry 행과 window integration 행이 모두 필요하며, 일반 manager의 leaf producer budget은 TC152/153이 담당합니다. 이 TC는 외부 증거를 요구하지 않으며 `EXTERNAL_REQUIRED`를 내지 않습니다. 이 TC는 Layout 계산 결과의 geometry를 검사하며 window presentation latency나 Recycler 전체 adapter/render throughput을 검사하지 않습니다. variable Recycler의 모든 item이 viewport100000 안에 있어 계산 대상이 고정됩니다. `python3 layout-validation-tools.py check-log LOG --tc TC156 --profile release --manifest MANIFEST --manifest-sha256 PIN`은 저장된 행을 재검사하는 보조 수단이며 UI 실행을 대신하지 않습니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 18 scenario, 28 action, 6998 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `deep-dirty-n16` | `samples` : 161 |
| `deep-dirty-n128` | `samples` : 609 |
| `balanced-dirty-n16` | `samples` : 217 |
| `balanced-dirty-n128` | `samples` : 1113 |
| `flex-wrap-n16` | `samples` : 161 |
| `flex-wrap-n128` | `samples` : 609 |
| `flex-grow-n16` | `samples` : 161 |
| `flex-grow-n128` | `samples` : 609 |
| `flex-shrink-n16` | `samples` : 161 |
| `flex-shrink-n128` | `samples` : 609 |
| `grid-star-span-n16` | `samples` : 229 |
| `grid-star-span-n128` | `samples` : 1125 |
| `variable-recycler-n16` | `samples` : 157 |
| `variable-recycler-n128` | `samples` : 605 |
| `text-reflow-n16` | `samples` : 98 |
| `text-reflow-n128` | `samples` : 98 |
| `window-variable-recycler-n16` | `prime-cache` : 23 → `viewport` : 23 → `middle` : 23 → `end` : 23 → `wider` : 23 → `start` : 23 |
| `window-variable-recycler-n128` | `prime-cache` : 23 → `viewport` : 23 → `middle` : 23 → `end` : 23 → `wider` : 23 → `start` : 23 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC156 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
