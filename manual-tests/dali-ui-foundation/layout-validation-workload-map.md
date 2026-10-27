# Layout workload 검증 연결표

이 문서는 신규 suite의 TC145·TC152·TC153·TC156을 중심으로 **source decision → scenario/action → 실제 assertion ID → 독립 기대값**을 연결합니다. TC154·TC155는 인접 신규 TC로만 함께 표시합니다. 기존 Layout TC 실행에 의존하지 않습니다.

이는 소스와 선언된 검사 계약을 대조한 표입니다. 아래 행은 실행·통과·branch coverage 달성의 증거가 아닙니다. 실행 증거는 동일 artifact의 `run_id/scenario_id/action_seq`, 원문 CHECK/RESULT, 완료 fence, 검토·고정한 contract fingerprint로 별도 연결하십시오. gcov branch가 실행되었다는 사실만으로 해당 결정이 독립 assertion으로 검증되었다고 기록하지 마십시오.

## 표기와 범위

- `F/`는 저장소의 `dali-ui-foundation/`, `T/`는 이 문서 옆 `tc/`입니다. `파일:행`은 작성 당시 탐색 기준점이며 변경 후에는 함수·predicate 이름으로 재확인하십시오.
- `Rect(prefix)`는 실제 ID `prefix.x/.y/.width/.height` 네 행, `Size(prefix)`는 `.width/.height` 두 행입니다. 구현은 `T/layout-validation-support.cpp:201`입니다.
- geometry 기본 tolerance=0.001, integer/bool/count/epoch/ID는 exact입니다. `ImmediateWorkResult::Check`는 `.values`를 실제 비교 scalar 수와 대조하고 `.mismatches=0`, `.maximum_error=0`을 검사합니다. tolerance 밖 또는 nonfinite scalar는 mismatch에 포함됩니다. 마지막 sample의 개별 geometry 행도 유지됩니다.
- `RequireExternalVerification`이 있는 action(TC154)의 로컬 CHECK 통과는 해당 MD가 요구하는 외부 증거를 대신하지 않습니다. TC152·TC153·TC155·TC156은 외부 증거를 요구하지 않습니다.
- 이 suite는 시간을 측정하지 않습니다. 반복 sample step은 producer callback을 설치하지 않으며, leaf producer count는 work-budget step에서만 설치합니다.

## 현재 선언된 계약 크기

다음 수치는 컴파일된 앱의 `BuildScenarios()` 선언에서 생성한 manifest 합계입니다. manifest에는 `release` profile 하나만 있습니다. 실행 결과가 아닙니다.

| TC | scenario / action / required checks | 검사 계약 |
|---|---|---|
| TC145 | 57 / 160 / 3368 | 기존2131 + same-count/partial/notify542 + animation 중 resize512(같은 수/작은 수 × 정수/분수·부분 마지막 page) + 정지 상태 no-op animated 요청 뒤 page 수 감소162 + SC.content-arranged-extent21; SC.match-constraints17행 + SC.content-arranged-extent21행 |
| TC152 | 12 / 84 / 82940 | manager 4×N{16,128,512}; 반복 sample step 4개(189+4N)와 work-budget step 3개(11+4N) |
| TC153 | 3 / 18 / 17571 | N{16,128,512}; 반복 sample step 3개(189+4N)와 work-budget step 3개(14+4N) |
| TC154 | 7 / 7 / 4146 | flat 4×771(256 cycle×3 + 최종 렌더 3) + depth 34/258/770 |
| TC155 | 30 / 60 / 137900 | mode 0~4×N{1,32,128}; 반복 sample `TR15` 15개(mount+samples)와 work-budget `TR15W` 15개(mount+work-count) |
| TC156 | 18 / 28 / 6998 | 반복 sample6722 + 실제 RecyclerView correctness companion276 |

## TC152: controller 계산 phase

구현은 `T/tc-152-layout-compute-performance.cpp`의 `RunPhase`, `RepeatPhase`, `CheckWork`, `ObserveImmediate`와 `T/layout-validation-workloads.h`입니다. workload는 `StageOnWindow`로 부착되며 pass는 `LayoutController::ProcessLayouts()`를 통합니다. N={16,128,512}, W=10N−2, H=16, leaf i=(10i,0,8,16)이 독립 기대값입니다.

| phase/action | 실행 경로 | 독립 검증 |
|---|---|---|
| `construction` | sample마다 fixture 생성·부착·settle pass1회 | 요청값 scalar3+2N, 이후 모든 leaf geometry |
| `first_pass` / `work-first_pass` | 새 root의 controller pass1회 | immediate12+6N; leaf Measure·Arrange producer 각 N(Grid가 같은 constraint로 다시 묻는 측정은 cache hit) |
| `warm_pass` / `work-warm_pass` | 형제 Nudge 후 pass32회 | leaf Measure·Arrange producer 0(workload 전체를 cache에서 처리) |
| `idle_pass` / `work-idle_pass` | clean Window의 pass4096회 | leaf producer 0 및 전체 geometry 불변 |

`CountLeafProducers`는 workload의 모든 leaf에 계수용 MeasureCallback/ArrangeCallback을 설치하고 `CheckLeafProducers`는 phase 동작 동안의 실행 수를 exact로 검사합니다(`work.leaf.measure-producers`, `work.leaf.arrange-producers`). leaf는 고정 크기의 자식 없는 View이므로 요청 크기를 보고하고 받은 slot을 그대로 돌려주는 callback은 기본 producer와 같은 layout을 만듭니다. container에 callback을 설치하면 manager를 대체하므로 root/container의 producer, cache hit, replay, geometry write, storage는 관측하지 않습니다.

warm-up 없이 31개 sample마다 `sample.immediate.*`, `sample.geometry.*`를 검사합니다. 최종 `rendered.*`는 새 render task 완료 뒤 current scene geometry를 읽습니다. 모든 leaf를 검사하며 frame timeout 때 event-side 결과로 대체하지 않습니다. exact 행 수는 [TC152 TC](tc/tc-152-layout-compute-performance.md)의 검토된 계약을 따릅니다.

## TC153: sparse·dense·same-value invalidation

구현: `T/tc-153-layout-update-performance.cpp` `UpdateWork/RepeatUpdate`; source는 `ViewDataImpl::SetRequestedHeight → ApplyRequestedHeight`의 동일값 guard와 `InvalidateMeasure` (`view-data-impl.cpp:2275`), Measure hit/miss, Arrange hit/replay입니다. N={16,128,512}; Stack geometry는 TC152와 같고 height16→15를 바꿉니다.

| scenario/action | 독립 변경 입력 | 실제 assertion ID / 식 |
|---|---|---|
| `nN → work-sparse` / 반복 `sparse` | 가운데 child 하나만 변경, C=1 | `work.leaf.measure-producers=C`, `work.leaf.arrange-producers=C`; 나머지 leaf는 cache에서 처리 |
| `nN → work-dense` / `dense` | 전체N child 변경, C=N | 같은 C식 |
| `nN → work-same-value` / `same-value` | 모든 child height16 재설정, C=0 | leaf producer 0; 형제 nudge pass는 workload를 cache에서 처리 |
| 모든 leaf budget action | 반환과 현재 geometry | `output.measured.width/height=W/16`, `output.arranged`=(0,0,W,16), `geometry.i`의 height만 해당 input15/16, `geometry.visited=N`, `.mismatches=0`, `.maximum_error=0` |
| 모든 반복 action | sample j마다 15/16 교대(j 짝수 15, 홀수 16); same-value는 16 고정 | `sample.immediate.j.values=12+6N`, `.mismatches=0`, `.maximum_error=0`; `sample.geometry.j.visited/mismatches/maximum_error`; fresh frame 이후 `rendered.i` 전체 Rect |

반복 step의 각 sample은 setter와 controller pass 직후 expected height를 갱신하고 검사하며, 반환·geometry를 복구하는 추가 layout은 호출하지 않습니다. 불필요한 leaf producer 실행은 work-budget step의 exact 검사로 판정하십시오.

## TC156: 복합 계산 workload의 독립 oracle

구현: `T/tc-156-layout-complex-performance.cpp`의 `ComplexFixture::Prepare/Mutate/Calculate/Observe`, `RepeatComplex`입니다. 각 family는 N16/N128 두 scenario이고 action은 단일 `samples`입니다. state s는 sample 0의 1부터 0/1을 번갈아 바꾸며 expected를 먼저 구성한 뒤 setter+계산을 실행하고 직후에 관측합니다.

모든 sample j=0…30의 실제 assertion은 `sample.immediate.j.values/.mismatches/.maximum_error`이며 warm-up sample은 없습니다. 마지막 sample 뒤 `final.root`와 `final.node.i`를 개별 Rect로 남깁니다. **TC156은 producer exact budget을 추가로 계수하지 않으며**, family별 geometry만 검사합니다. 단순 manager의 leaf producer budget은 TC152/153이 담당합니다.

| family / source decision | 독립 fixture·식 | scalar 수 / 개별 행 범위 |
|---|---|---|
| `deep-dirty-nN`: Stack `Measure/Arrange`, child invalidation의 ancestor 경로 | depth N, 폭 120, leaf height20+s; 모든 N ancestor WRAP height20+s; 각 local origin0 | root+N descendant, values=10+4N, D=4(N+1) |
| `balanced-dirty-nN`: 2-child Stack 합산·spacing·한쪽 branch invalidation | N leaves의 완전 이진 tree, spacing2; root H=22N−2+s. rightmost leaf와 해당 ancestor만 height+ s, sibling geometry 고정 | descendants2N−2, values=10+4(2N−2), D=4(2N−1) |
| `flex-wrap-nN`: `FlexLayoutManager::Measure/Arrange`, wrap line 경계와 `ArrangeOneFlexLine` | basis10,height16; containerW=40/50. columns=4/5; rect i=`(10(i mod columns),16 floor(i/columns),10,16)` | values=10+4N, D=4(N+1). ROW/WRAP/FLEX_START의 여러 line; reverse/다른 align을 이 workload가 포괄하지 않음 |
| `flex-grow-nN`: `ApplyFlexGrowShrink :135`의 positive free space | basis8, 반복 weight1/3. containerW=12N/16N, sumGrow=2N; child width=`8+(2 또는4)×weight`, x=이전 폭 prefix sum | values=10+4N, D=4(N+1); full available width 보존 |
| `flex-shrink-nN`: 같은 helper의 negative free space와 scaled shrink | basis16, weight1/3, containerW=12N/8N; sum(shrink×basis)=32N; width=`16−(2 또는4)×weight`, x=prefix sum | values=10+4N, D=4(N+1); 이 fixture는 음수 floor나 min/max 재분배 정책을 검사하지 않음 |
| `grid-star-span-nN`: `ApplyGridDefinitions :264`, `ComputeGridPositions :367`, `ArrangeGridChildrenToCells :387` | columns STAR1:2:1:1, N개 동일STAR row; W250/375, unit=W/5, H=20N. 매 row child `(0,20i,3unit,20)` 및 `(3unit,20i,unit,20)`, 마지막 full row span `(4unit,0,unit,20N)` | child2N+1, values=10+4(2N+1), D=4(2N+2) |
| `variable-recycler-nN`: `LinearItemsLayouterImpl::OnLayoutChildren :72`, `SettleBy :462`, `LayoutWindow :491`, `LayoutChunk :263`, `MeasureSlot :287`, `MeasureUpdate :315`, `GetItemBounds :149`, `GetEstimatedContentExtent :404` | VERTICAL, 실제 h_i=10(1+i mod3), decoration L/R/T/B=3/7/2/4, spacing5, cross120/160. outer y_i=Σ(k<i)(h_k+11); actor rect=(3,y_i+2,cross−10,h_i); item bound=(0,y_i,cross,h_i+6); range=Σ(h_i+11)−5 | values=4+8N, D=4N; range/offset0/visible first0/lastN−1와 모든 actor/item bound. viewport100000의 전체 표시 integration fixture이며 실제 RecyclerView virtualization·Group layouter 전체를 이 행으로 대신하지 않음 |
| `text-reflow-nN`: `LabelImpl::OnMeasure`의 width-dependent height, `GetHeightForWidth` | Label 24px, A N개, CHARACTER wrap, W25/40; expected 높이 H(W)=`Label::GetHeightForWidth(W)`를 sample마다 실행 중 공개 API로 구함(font family·glyph advance·줄 높이 상수 없음). 마지막에 H(25)≥H(40) | values10, D5(`final.root` 4 + `final.text.narrow-not-shorter`). text engine shaping 전체·다국어 전체 또는 async image decode 성능을 이 workload의 범위로 주장하지 않음 |

## 인접 soak·transition workload 연결

| source decision | 신규 scenario/action | 실제 assertion ID / 범위 |
|---|---|---|
| `ReplayCachedArrange`가 변조된 Actor position/size를 cached result로 복구 | TC154 `stack/flex/grid/absolute → run` | 128child,256cycle; index=(cycle×37) mod128에 x=−123.25,width1을 쓴 뒤 `cycle.j.visited=128`, `.mismatches=0`, `.maximum_error=0`;16cycle마다 명시 invalidate. heap leak 판정은 TC MD의 10번 실제 재진입·cleanup/RSS/PSS/allocation-profile 증거가 별도 필요 |
| 깊은 WRAP ancestor의 padding 합산 | TC154 `depth-8/64/192 → run` | `deep.root`=(8+2D,16+2D), `deep.child.i`=(1,1,8+2i,16+2i). deep tree 반복 갱신은 TC156 |
| transition 미설정/사용하지 않는 handle/disabled/active Spec/active Animator | TC155 `TR15.mode-{0…4}-n{1,32,128} → mount/work-count` | `performance.fixture/work.handles`, `.logical-children`, `.leaf-i.identity-order`, `.leaf-i.target` 모든 node; `performance.transition.start-count`=mode≥3 ? N:0, `performance.first-tick-count`=mode4 ? N:0(같은 수동 pass의 마지막 animator tick이 새 animator를 raw 0으로 한 번 dispatch) |
| 같은 transition workload의 반복 갱신 | TC155 `TR15` scenario의 `samples` | `performance.sample-j.*` 모든 logical target, samples31, sample마다 ProcessLayouts128(dormant) 또는 update9. active animation Actor 중간값 대신 완료 logical target을 읽음 |

## TC145: ScrollView·PageScrollView source 결정

구현: `T/tc-145-layout-scroll.cpp`. 실제 조작·관측 순서는 [TC145 MD](tc/tc-145-layout-scroll.md)를 따르십시오. Window action은 지정 View snapshot과 Window fence를 모두 요구합니다. 아래에서 `actual geometry`는 fence 후 Actor event property이며 animation pixel 추정값이 아닙니다.

| source function / decision | scenario → action | 실제 assertion ID / 독립 식 |
|---|---|---|
| `ScrollViewLayoutManager::Measure :87`(child constraint와 mode :123-131)의 MATCH/non-MATCH 축 constraint: MATCH 축은 viewport에 EXACTLY, WRAP 축은 FLT_MAX에 UNSPECIFIED | `SC.match-constraints.{0…3} → run` | `owner.measure`100×80, `measure.constraint.width/height`=(MATCH ?72/60:FLT_MAX), `content.measure`=(MATCH ?72/60:240/200) |
| `ScrollViewLayoutManager::Arrange :140`의 MATCH 대입과 `CanSkipArrangeRemeasure :194`: 할당 축의 slot이 측정 key와 같으면 Arrange-owned 재측정 생략 | 같은 scenario | `owner.arrange`=(0,0,100,80), `content.arranged`=(11,7,w,h); `last.constraint.width/height`=(bits&1 ?72:FLT_MAX, bits&2 ?60:FLT_MAX)(마지막 producer 호출이 measure pass 호출); `measure.producers`=1(모든 bits) |
| `ScrollViewLayoutManager::Arrange :207-215`: scrollable 크기는 content의 arrange 결과(자기 min/max로 clamp된 slot 또는 callback 반환 rect) | `SC.content-arranged-extent → minimum/scroll-end/custom-return` | `extent.content.measured`(100,200), `extent.content`(0,0,100,200), `extent.scrollable.height`200, `extent.viewport.height`80; `extent.scroll.y`=200−80=120, `extent.scroll.x`0, `extent.scrolled`(0,−120,100,200); `extent.custom.scrollable.height/width`300/37, `extent.custom.scroll.y`=300−80=220, `extent.custom.scrolled`(0,−220,37,300) |
| Scroll manager `ArrangePolicy::ALWAYS`가 current Actor position을 다시 읽음 | `I05.scroll-preservation → scroll-to/settled-layout-replay` | `scroll-x/y`50/60, `scrolled-content` 및 `preserved`=(−50,−60,240,200), `preserved-scroll-x/y`50/60 |
| `ScrollViewImpl::AdjustScrollPosition :1558`, `SetScrollPosition :230` | 같은 scenario `upper-clamp/negative-clamp` | `maximum-x/y`140/120 및 `clamped-content`; `minimum-x/y`0 및 `origin-content` |
| `SetScrollableWidth/Height :245/:271`, `UpdateScrollingProperties :2258`의 viewport 재계산 | 같은 scenario `resize-viewport` | `larger-viewport`160×120, `new-maximum-x/y`80, `actual-content`=(−80,−80,240,200). content 크기가 같다는 이유로 viewport update를 건너뛰면 검출 |
| `SetContent` 교체 후 range/parent 갱신 | 같은 scenario `replace-smaller-content` | `small-content`60×40, `old-detached`, `small-scroll-x/y=0` |
| padding origin과 MATCH inner extent, axis별 scroll clamp | `SC.axis-padding-margin.{0…2} → mount/maximum/resize/padding-change` | `viewport`(arrange된 actor rect), `content`, `scroll.x/y`, `cached-viewport.width/height`(ScrollViewImpl이 저장한 viewport), `scrollable.width/height`, `direction`. content width=Horizontal ?240:viewportW−start−end; height=Vertical ?200:viewportH−top−bottom; origin=(start−scrollX,top−scrollY) |
| content 내부 default View의 padding+child margin 합성 | 같은 scenario 모든 action | `nested-margin`=(5,13,max(contentW−14,0),max(contentH−32,0)); 내부 padding3/5/7/11, margin2/4/6/8. ScrollView가 content 자체 margin을 처리한다는 미정 계약을 가정하지 않음 |
| `PageScrollViewImpl::GetPageCount :124`, default page viewport | `PAGE.horizontal/vertical → mount-default-size` | `initial.count=3`, `.current=0`, `.scroll.main/cross=0`, `.content`250×80(axis별 전치), `.scrollable=250`, `.viewport=100`; `initial.signal-count=1`, `page.signal.page/count=0/3` |
| `ScrollToPage :147`, page index clamp와 부분 마지막 page | 같은 scenario `page-boundaries` | `negative.*`:count3/current0/scroll0; `last-partial.*`:count3/current2/scroll150; 마지막 signal2/3 |
| viewport 변경에 따른 default page 수/현재 index 갱신 | 같은 scenario `viewport-resize` | `resized.count=2`, `.current=1`, `.scroll.main=125`, `.viewport=125`, `.scrollable=250`, content origin−125; signal1/2 |
| `SetPageSize :85`와 `GetEffectivePageSize`, axis component 선택 | 같은 scenario `explicit-page-size` | `explicit.count=3`, `.current=1`, `.scroll.main=125`; `explicit.width/height`=main100/cross17; signal1/3 |
| `NotifyPagesInserted :375`: index≤current, 즉시 expected count/range, notification suppression | 같은 scenario `insert-before-current` | `immediate-insert.count/current=4/2`, `.scroll.main=200`, `.scrollable=400`, `.content`의 extent는이전 250; `insert.signal-delta=1`; `settled-insert.*` 실제 extent350; `insert.order`, `old-current-local` main200/extent100, `old-current-visible=0` |
| `NotifyPagesRemoved :429`: before-current 이동, 즉시/다음 layout 구분 | 같은 scenario `remove-before-current` | `immediate-remove.count/current=3/1`, `.scroll.main=100`, `.scrollable=300`, 이전 Actor extent350; `remove.signal-delta=1`; `settled-remove.*` extent250 |
| 같은 함수의 zero page/current−1, default page 복원 | 같은 scenario `remove-all/restore-default` | `immediate-empty/settled-empty.count=0`, `.current=−1`, scroll0; Actor extent250→0, `empty.signal-delta=1`; `restored.count/current=1/0`, content100, scroll0, signal0/1 |

Page signal buffer 검사 ID는 각 action의 `page.signal.buffer`(고정 64 event, overflow없음), `page.signal.page`, `page.signal.count`입니다. Notify의 임시 range(page100×expected count400/300)는 실제 마지막 부분 page를 포함하는 최종 extent350/250과 다릅니다. source 내부 임시값을 최종 geometry oracle로 사용하지 않습니다.

Scroll fling/gesture 물리·scrollbar appearance 전체는 여기의 Layout 계산 분모가 아닙니다. Page의 current/after/overlap·index/count clamp·pre-layout notification은 아래 추가 행으로 연결됩니다. positive effect가 다른 before-current fixture를 실행했다는 이유만으로 이 분기까지 실행되었다고 주장하지 마십시오.

## 추가 Page decision 연결

| source decision | 신규 scenario/action | 실제 assertion ID / 독립 기대 |
|---|---|---|
| `NotifyPagesInserted`: index=current의 shift, index>current의 유지, 음수/과도 index clamp | TC145 `PAGE.notify-{horizontal/vertical}.{insert-at-current/insert-after/insert-negative-index/insert-past-end} → mount/notify-and-settle` | initial5pages/current2; 최종(count,current)=(6,3)/(6,2)/(6,3)/(6,2). `notify.immediate.*`, `notify.signal-delta=1`, `notify.logical-count`, `notify.settled.*`, `notify.visible-identity/local/origin` |
| `NotifyPagesRemoved`: after/current/overlap/마지막 current, index clamp, excess count clamp | 같은 prefix의 `remove-after/current/through-current/last-current/negative-index/past-end/all-clamped` | initial5pages/current2(마지막 current만 4), 최종(count,current)=(4,2)/(4,2)/(2,1)/(4,3)/(4,1)/(4,2)/(0,−1). content extent500→100×count, current<0이면 scroll0 아니면 100×current; empty는 `notify.empty-children` |
| nonpositive insert/remove count early return | `PAGE.notify-AXIS.nonpositive-count → notify-noop` | `noop.0…3` 각 count5/current2/scroll200/content500 유지, `.signal-delta=0`; 마지막 signal과 logical count도 유지 |
| `SetPageSize` primary<1 return, same-size return; default pageLen<1 count fallback | `PAGE.pre-layout-AXIS.{insert-before/insert-after/remove-empty} → notify-before-layout` | `pending.initial/explicit/same-size/default.*`; default count1/current0, explicit count0/current−1; viewport/scrollable0, signal0, page size getter100/17(axis별) |
| Notify의 pre-layout pgLen0 및 expected count, oldTotal0 remove no-op | 같은 action | `pending.notified.*`: before2/1,after2/0,empty0/−1; scroll0/range0; signal-count1. empty 재호출 `pending.already-empty.*` 동일값/signal1 |
| 첫 실제 layout에서 expected page와 표시 위치 일치 | 같은 scenario `attach-and-settle` | `pending.settled.*`, `pending.visible-identity/local/origin`; before current1/scroll100,after0/0,empty−1/0. pending navigation 유실은 expected를 0으로 바꾸지 않고 FAIL로 유지 |

## 실제 증거 연결

각 map 행을 완료 증거와 연결할 때는 `source revision`, `app/library SHA`, `profile`, `TC/scenario/action`, `assertion IDs`, `actual/expected`, `run/log SHA`, `coverage artifact(있을 경우)`를 함께 기록하십시오. mutant 검출은 먼저 같은 baseline TC가 통과한 뒤 하나의 source predicate/formula만 바꾸고 지정 assertion이 실패한 경우로 한정합니다. compile 실패·앱 시작 실패·관측 누락은 수치 oracle의 mutant 검출 성공으로 집계하지 마십시오.
