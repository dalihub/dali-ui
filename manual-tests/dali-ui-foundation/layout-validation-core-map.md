# Layout core 의사결정과 신규 assertion 연결표

이 문서는 TC95–TC127 및 TC151의 현재 C++ fixture와 source 의사결정을 연결합니다. 파일 존재, syntax 검사, coverage 진입만으로 runtime PASS를 뜻하지 않습니다. 전체 source branch의 완료 목록이 아니며, 표에 없는 조건을 검증했다고 판단하지 마십시오. 전체 구성·실행 조건·coverage·mutation 절차는 [suite 실행 명세](layout-validation-coverage.md)를 함께 사용하십시오.

## 읽는 방법과 실행 증거

- Source 기호는 아래 경로와 함수명을 함께 식별합니다. 줄 번호는 변경 시 쉽게 어긋나므로 식별자로 사용하지 않습니다. 조건식이나 함수가 바뀌면 fixture 입력·oracle·check도 함께 검토하십시오.
- 별도 action을 적지 않은 Single scenario의 step ID는 run입니다. 순차 scenario에는 실제 step ID를 적었습니다. `Run step`으로 실행하고 완료 시 자동 출력되는 `LR_RESULT`/`LR_CHECK` 행을 수집하십시오.
- R은 직접 Measure 반환·GetMeasuredSize·Arrange 반환·Actor event-side rect, W는 필수 View들의 callback snapshot을 동일 Window 완료 fence에서 고정한 값, E는 별도 외부 관측입니다. producer 호출 수는 R에 포함합니다. 직접 Arrange 결과를 render 완료나 Window 완료의 증거로 해석하지 마십시오.
- Size("p")는 p.width/p.height, Rect("p")는 p.x/p.y/p.width/p.height 네 check입니다. Id("a.", i)는 점을 하나 더 붙이므로 실제 ID는 a.0입니다. 아래 ..{i}는 이 실제 형식을 표시합니다. float를 std::to_string으로 붙이는 scenario는 예를 들어 C09.max-wins.20.000000입니다.
- 표의 check는 대표 연결 ID입니다. 완료 조건은 해당 build의 LR_CASE_CONTRACT에 있는 모든 scenario/step, 필수 assertion ID, required_checks와 실제 행 수의 정확일치입니다. 대표 ID 몇 개나 다른 assertion의 중복으로 빠진 검사를 대체하지 마십시오.
- 결과는 (tc, pid, run, scenario, action_seq, row)와 source hash, app/library build-ID, 실제 로딩 경로, build type, 계약 로그를 함께 보존하십시오. 같은 행의 재출력은 추가 검사가 아닙니다.
- R/W expected는 입력과 합·비율·좌표식으로 계산합니다. TC가 지정한 절대 허용오차를 사용하며, 큰 좌표에서 오차를 확대하는 전역 relative tolerance로 대체하지 마십시오. NaN/Inf, overflow, observer 미준비, timeout, 검사 누락은 PASS가 아닙니다.
- producer count는 synthetic producer callback의 호출 수입니다. 전체 allocation/private cache access 횟수로 확대하지 마십시오. cache flag, dependency owner, generation, scale key 같은 내부 상태는 관측하지 않으며 그 효과를 producer 호출 수와 geometry로 판정합니다.

## Source 기호

| 기호 | Source와 함수/계약 |
|---|---|
| V | [view-data-impl.cpp](../../dali-ui-foundation/internal/views/view/view-data-impl.cpp): ViewDataImpl의 Measure, MeasureDefault, ArrangeImpl, ArrangeDefault, ApplyConstraints, cache/invalidation/child 관리, DeriveMeasureModes, NormaliseMeasureModes, CanSkipArrangeRemeasure, ClampToOwnSizeBounds, HasSizeDependentContent |
| API | [view.h](../../dali-ui-foundation/public-api/views/view.h), [view.cpp](../../dali-ui-foundation/public-api/views/view.cpp), [view-impl.cpp](../../dali-ui-foundation/public-api/views/view-impl.cpp): setter·callback·manager·standalone·order 계약과 전달 경로 |
| P | [Stack params](../../dali-ui-foundation/public-api/layouts/stack-layout-params.cpp), [Flex params](../../dali-ui-foundation/public-api/layouts/flex-layout-params.cpp), [Grid params](../../dali-ui-foundation/public-api/layouts/grid-layout-params.cpp), [Absolute params](../../dali-ui-foundation/public-api/layouts/absolute-layout-params.cpp), V의 SetLayoutParams/GetLayoutParams |
| S | [stack-layout-manager.cpp](../../dali-ui-foundation/public-api/layouts/stack-layout-manager.cpp): MeasureStackNonWeightChildren, MeasureStackWeightChildren, StackLayoutManager::Measure/Arrange |
| F | [flex-layout-manager.cpp](../../dali-ui-foundation/public-api/layouts/flex-layout-manager.cpp): BuildFlexLinesForArrange, ApplyFlexGrowShrink, GetFlexJustifyOffsets, ArrangeOneFlexLine, FlexLayoutManager::Measure/Arrange |
| G | [grid-layout-manager.cpp](../../dali-ui-foundation/public-api/layouts/grid-layout-manager.cpp): MeasureGridChildrenAndFillAuto, ApplyGridDefinitions, ComputeGridPositions, ArrangeGridChildrenToCells |
| A | [absolute-layout-manager.cpp](../../dali-ui-foundation/public-api/layouts/absolute-layout-manager.cpp): GetChildBounds, GetChildFlags, AbsoluteLayoutManager::Measure/Arrange |
| ST | [standalone-bounds-utils.h](../../dali-ui-foundation/internal/layouts/standalone-bounds-utils.h): SnapshotStandaloneSlotInputs, ResolveStandaloneExtent, DeriveStandaloneMeasureConstraint, DeriveStandaloneMeasureModes, DeriveStandaloneRootBounds; V의 MeasureStandaloneChildren/ArrangeStandaloneChild/ArrangeStandaloneChildren |
| MM | [measure-mode.h](../../dali-ui-foundation/internal/layouts/measure-mode.h): MeasureMode(AT_MOST/EXACTLY/UNSPECIFIED), MAX_EXACT_MEASURE_SIZE, EMPTY_SLOT_TOLERANCE, SnapEmptySlot |
| SC | [layout-scale-manager-impl.cpp](../../dali-ui-foundation/internal/layout-scale-manager-impl.cpp), [LayoutScaleManager extension API](../../dali-ui-foundation/extension-api/layout-scale-manager.cpp); V의 GetEffectiveLayoutScale와 scale cache/property 동기화 |
| DEP | [layout-dependency-scope.cpp](../../dali-ui-foundation/internal/layouts/layout-dependency-scope.cpp), [layout-invalidation-generation.cpp](../../dali-ui-foundation/internal/layouts/layout-invalidation-generation.cpp); V의 ancestor invalidation/owner stop |
| CT | [layout-controller.cpp](../../dali-ui-foundation/public-api/layouts/layout-controller.cpp): ProcessLayouts, Process, EmitPendingViewLayoutFinishedSignals, Get, Remove, idle wake/완료 queue, ProcessLayoutRoot의 MATCH root 축 clamp, ReleaseLayoutRootInternal |
| OBS | [공통 fixture](tc/layout-validation-fixtures.h), [공통 support](tc/layout-validation-support.h) |

## 기본 constraint, 값, dispatch, setter

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| V MeasureDefault 및 S/F/G/A Measure: 기여 child 없는 WRAP와 padding | TC95 C01.empty.{kind}, kind=0.4 | empty=0×0; padding start/end=3/5,top/bottom=7/11 →8×18 | empty.*, padded.* / R |
| V Measure/ApplyConstraints: 고정 요청, min/max clamp | TC95 C02.fixed.{kind}..., C07.clamp.{kind}... | 요청 0/1/37.5/120; min40,max100에 39/40/41/99/100/101 →clamp 결과 | measured.*, arranged.*, clamp.* / R |
| V Measure: normalized constraint와 min=max | TC95 C08.equal-bounds.{kind} | min=max60×30; incoming1×1과 400×400 모두 60×30 | small-budget.*, large-budget.* / R |
| V callback의 producer constraint clamp | TC95 C03.intrinsic | intrinsic37×19; 작은 constraint20×10; 큰 constraint로 복귀 37×19 | free.*, limited.*, restored.* / R |
| V Measure EXACTLY 보고(ApplyConstraints)와 ArrangeImpl ClampToOwnSizeBounds: MATCH slot의 min/max clamp, start edge 유지 | TC95 C07.match-clamp.{0..4} (below-minimum → inside → above-maximum) | MATCH×MATCH container, margin7/0/11/0, min40/max100; stage46×50/77×81/108×112의 slot 39/70/101 →measured·렌더 (40,40)/(70,70)/(100,100), 위치 7,11 | below-minimum.*, inside.*, above-maximum.* / R |
| API/V requested/min/max setter의 유한값·음수 거부 | TC96 C09.reject.{0.3} | -3,NaN,+Inf,-Inf를 요청/최소/최대 양축에 설정; 이전 37×19,min3×2,max100×80 유지 | requested-width, requested-height, minimum-*, maximum-*, preserved-measure.* / R |
| API max-wins; V ApplyConstraints의 clamp 순서 | TC96 C09.max-wins.{20,70,150}.000000 | min100/max40,min90/max30; 요청이 최소 아래/사이/위여도 40×30 | maximum-wins.* / R |
| requested sentinel canonicalization | TC96 C09.sentinel.* | WRAP_CONTENT와 MATCH_PARENT 주변±0.0005 →정확한 sentinel | canonical-width, canonical-height / R, tolerance=0 |
| P SetFlexGrow/SetFlexShrink: 음수 factor | TC96 R03.negative-flex-factors | grow=-1,shrink=-2 →0/0 | grow, shrink / R, tolerance=0 |
| V ArrangeImpl: custom 반환 rect 안전성/복구 | TC96 R03.returned-rect.{0.2} | 음수width,NaN x,+Inf height. Debug예외/Release전체inputfallback;Actor의유효provisional입력 7,9,53,29유지;후속 11,13,61,31복구 | invalid-result-not-published, safe-actor.*, recovered.* / R. 한 종류의 예외 정책 전체를 강제하는 검사는 아닙니다. |
| V Measure: View 부모 없는 MATCH 축 EXACTLY, MM MAX_EXACT_MEASURE_SIZE와 음수의 AT_MOST 강등 | TC96 R04.unbounded-exact | detached MATCH×MATCH, min13×17, scale1: Measure(200,150)→200×150, (1e29,1e29)→1e29, FLT_MAX/1e30/−1→minimum 13×17 | exact.*, float-max.*, threshold.*, below-threshold.*, negative.* / R |
| V ApplyConstraints/ClampToOwnSizeBounds의 max-wins 순서, MATCH slot | TC96 C09.max-wins.match | MATCH×MATCH min100×90,max40×30, stage300×300: EXACTLY300→max(300,100)→min(.,40)=40, 높이30; slot(0,0,300,300)→(0,0,40,30) | maximum-wins.* / R |
| V ArrangeDefault: parent 이동과 child local 좌표 분리 | TC97 C10.C12.local-coordinate | parent70,90→100,120; child는 padding+요청+margin=16,26,37,19 유지 | parent.*, child.*, child-after-parent-move.* / R |
| V MeasureDefault: child far edge의 max | TC97 C04.wrap-bounds | 겹친 child extent70×35→80×35 | wrap.*, changed.* / R |
| V MeasureDefault: 결정된 부모의 MATCH child는 EXACTLY로 slot 보고, 크기 없는 WRAP 부모에서 MATCH child의 minimum 기여 | TC97 C05.C11.match-budget, C06.wrap-match | 결정된 부모: measured도 EXACTLY slot 86×28, 최종slot5,13,86,28; WRAP 부모: childmin30×20→10×20 | exact-slot.*, match-slot.*, wrap-minimum.*, reduced-minimum.* / R |
| V MeasureDefault/ArrangeDefault: padding 후 음수 잔여 clamp | TC97 C11.exhausted-padding | content budget가 padding보다 작으면 빈 slot max(0,10−16−6)=0에서 EXACTLY로 측정(0은 EXACTLY 유지)되어 0 extent | zero-measure.*, zero-content.* / R |
| V MeasureDefault 두 단계 측정: WRAP 부모가 자기 폭을 정한 뒤 MATCH child를 slot에서 EXACTLY 재측정 | TC97 C06.two-step (mount → widen-seed) | seed30(→50), MATCH 폭 m 안 wrapping probe37×19: 1단계 m(0,19) probe, 2단계 m EXACTLY30 →probe 높이38 →p·m (30,38), inner (0,0,30,38); seed50 →(50,19), inner (0,0,37,19) | p.*, p.rendered.*, m.*, m.rendered.*, inner.rendered.* / R |
| V MeasureDefault: 명시 크기 view는 자기 min/max로 clamp된 크기 안에서 content 측정 | TC97 C11.explicit-clamped-content | 요청30×50,min폭60 →content 폭60; wrapping probe 마지막 constraint 폭60, (37,19) 줄바꿈 없음; p (60,50) | p.*, p.rendered.*, inner.last-width, inner.*, inner.rendered.* / R |
| P/V SetLayoutParams/GetLayoutParams: 값 복사/명시 재할당 | TC98 C16.stack-copy, C16.flex-copy, C16.grid-copy, C16.absolute-copy | 두 View에 같은 params 설정 후 원본/읽은 복사본 변경; 재설정만 반영. Flex17/2/3,Grid row2,col3,span4/5,Absolute1,2,30,40 | read-a, read-b, a-original-copy, b-original-copy, snapshot-is-copy, explicit-reassign, basis, grow, shrink, align, snapshot-independent, row, column, row-span, column-span, alignment, bounds.*, flags / R |
| V Measure: callback > attached manager > default | TC99 C17.callback-manager-default | default max30×15→한 번 manager attach 후 callback7×9→callback제거 후 Stack20+7+30=57×15 | default-before-attach.*, callback-priority.*, manager-after-remove.* / R. null attach나 detach를 사용하지 않습니다. |
| V AttachLayoutManager; S/F/G/A public container equivalence | TC99 C18.attached-manager..{1.4} | container와 같은 manager를 붙인 View 각각 child20×10; Flexcross100,Grid200×100,나머지 20×10 | container.*, attached.* / R. actual끼리만 비교하지 않고 각각 숫자와 비교합니다. |
| V 앱 정의 LayoutManager 서브클래스: virtual Measure/Arrange dispatch, InvalidateOwnerMeasure, measure cache와 기본 IF_CHANGED | TC99 C19.custom-manager (mount → step-1.5 → same-value → sibling-pass → step-1.0) | 대각선 manager: a20×10,b30×15,c40×20 →measured90×45, b(20,10),c(50,25); step1.5 →b(30,15),c(75,37.5), producer +1/+1; 같은 값 setter →+0/+0; 형제 pass →+0/+0 | custom.*, step.*, same.*, sibling.*, restore.* / R, delta는 tolerance=0 |
| V DeriveChildModesAsCustomParent: custom manager의 mode 없는 child Measure | TC99 C19.custom-manager-modes (budget → exact) | 대각선 manager root: WRAP(AT_MOST)이면 MATCH child d는 minimum 6×4, root26×14, d(20,10,6,4); root MATCH×MATCH(EXACTLY 200×100)이면 d EXACTLY (180,90), root (200,100), d(20,10,180,90) | budget.*, exact.* / R |
| V tracked setters의 changed/no-op 값 비교와 invalidation | TC100 K05.tracked.{0.11} | requested W/H,padding,margin,min/max양축,Stack/Flex/Grid/Absolute params 변경→producer+1; 동일값→+0 | changed-producer, same-producer, measured-width / R. index→setter 순서는 C++ changes 배열입니다. |
| manager child participation과 visibility | TC100 C19.hidden-participates | A20,gap7,B30; A visible=false 뒤에도 B.x27 | visible-b.*, hidden-b.* / R |
| V tracked min/max setter, NormaliseMeasureConstraint, CanSkipArrangeRemeasure, ClampToOwnSizeBounds | TC100 K06.minmax-arrange (mount → maximum → same) | MATCH 폭 probe37×19, stage200×100: producer1, measured(200,19); max120 →producer+1, last-width120, (120,19), 렌더(0,0,120,19); 같은 값+형제 pass →+0, (0,0,120,19) | mounted-producer, mounted.*, changed-producer, last-width, changed.*, same-producer, same.rendered.* / R |
| V Measure normalized key FloatEqual (내부 cache 정책 고정: 0.001 epsilon과 정규화 key를 명시적으로 고정하는 행이며 의도적인 cache 정책 변경 시 재기준 필요) | TC101 C20.measure-tolerance (mount→below-epsilon→above-epsilon→normalized-minimum→normalized-hit) | stage 폭 128→128.0005는 setter가 무시(형제 pass에서 hit), 128.002 miss; min 200에서 stage 120/130은 같은 normalized 200 | below-epsilon-hit, above-epsilon-miss, normalized-hit, normalized-producer-input / W |
| V requested setter 값 비교 및 P weight mode 변화 | TC101 C20.pixel-setter, C20.exact-params | setter0.0005무시/0.002반영; weight0→0.0005는 nonweight→weight로 바뀜 | below-epsilon, above-epsilon, mode-change-producer, params-present, exact-stored-weight / R |
| V CanServeArrangeFromCache: exact rect key와 requested position setter의 epsilon 필터 | TC101 C20.requested-position-epsilon | 요청 x 100→100의 다음 float(100+2^-17)는 ranged epsilon(100·FLT_EPSILON) 안이라 setter가 무시(형제 pass 뒤 producer 불변, 렌더 x 100), 100.002는 producer +1과 렌더 x 100.002 | unchanged-producer, exact-position, changed-producer, moved-position / W |
| V Measure cache와 CanServeArrangeFromCache: 첫 pass와 관련 없는 pass | TC101 C20.first-pass-cache (mount → unrelated-pass) | 첫 controller pass에서 measure/arrange producer 각 1회; 형제만 바꾼 pass는 두 cache hit로 1/1 유지 | first.measure-producer, first.arrange-producer, probe.*, repeat.measure-cache, repeat.arrange-cache / R |
| MM EMPTY_SLOT_TOLERANCE/SnapEmptySlot: 0.001 미만의 자기 폭은 빈 slot | TC101 C20.empty-slot (empty → own-width → empty-again) | seed 0.0005 →빈 slot, m은 at-most probe 유지, Arrange에서 0.0005×19에 EXACTLY →p·m (0.0005,19); seed 0.002 →slot, probe 줄바꿈 →(0.002,38); 다시 0.0005 →(0.0005,19) | p.*, p.rendered.*, m.*, m.rendered.* / R |

## Scale와 direction

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| SC/V effective scale 정책과 global scalable gate | TC102 C13.scale.*, C14.K12.master-switch | scale1/1.25/1.5/2; DISABLED parent200×100,INHERIT child20×10,ENABLED child30s×15s; master ON/OFF/ON은 74×38/37×19/74×38 | parent-disabled.*, child-inherit.*, child-enabled.*, enabled.*, disabled.*, restored.* / R |
| SC invalid scale setter 보존 | TC102 R03.scale-finite | NaN,-1,+Inf,subnormal에서 이전scale1 유지 | nan-rejected, negative-rejected, infinity-rejected, subnormal-rejected / R |
| V Measure scale key exact 비교 및 invalidation | TC102 C20.scale-exact-key | sub-epsilon scale 변화 후 producer 재계산과 effective scale(extension API `GetEffectiveLayoutScale`) 일치; 같은scale 반복 hit | producer-recomputed, exact-effective, same-scale-hit / R. setter가 cache를 먼저 무효화하므로 방어용 key 비교 단독 branch의 도달 증거와 구분하십시오. |
| V Measure: 외부에서 바뀐 effective-scale property 복구 | TC102 C13.actor-scale-repair | 등록 이름으로 찾은 property index에 외부값 9 설정; 다음Measure는 producer hit이어도 property를 1로 복구 | property-index, corrupted, actor-scale, producer-hit / R |
| SC/V ApplyConstraints·ClampToOwnSizeBounds: natural min/max × scale, scale된 margin | TC102 C13.scaled-clamp.{1,1.25,1.5,2}.000000 | ENABLED MATCH×MATCH, margin10/0/20/0, max120×80, unscaled stage300×200: (min(300−10s,120s), min(200−20s,80s)) at (10s,20s) →(120,80),(150,100),(180,120),(240,160) | clamped.*, clamped.rendered.* / R |
| V ApplyLayoutDirection와 replay direction key | TC103 C15.LTR-RTL-LTR | parent200,A20,B30,gap7; x(0,27)→(180,143)→(0,27), x'=W-x-w를 한 번 적용 | a.{0.2}.*, b.{0.2}.* / R |
| V explicit/inherited direction의 subtree 경계 | TC103 C15.explicit-child | RTL root300,explicitLTR child100은x200,그 안 leaf는x0 | child-mirrored-by-parent.*, leaf-explicit-ltr.* / R |
| V ApplyLayoutDirection: STANDALONE child 제외, cache-hit replay의 mirror fold(View/S/F/G/A) | TC103 C15.standalone-replay.{0.4} (ltr → rtl → replay → ltr-again) | parent200×60; a20×10,b30×15는 x'=200−x−w; standalone10×10은 요청(150,40) 유지; RTL 상태 InvalidateArrange 재pass 뒤 동일 | a.*, b.*, standalone.* / R |
| V ApplyLayoutDirection: clamp된 logical 폭으로 mirror(slot의 오른쪽 끝 유지), replay 두 경로 | TC103 C15.clamped-mirror (ltr → rtl → parent-replay → child-replay → ltr-again) | parent300×200, MATCH child margin10/0/20/0,max120×80: logical (10,20,120,80); RTL 300−10−120=170 →(170,20,120,80), 부모 hit replay·child hit 모두 같음; LTR 복귀 (10,20,120,80) | ltr.*, rtl.rendered.*, rtl.logical.*, rtl.reported.*, rtl.measured.*, parent-replay.rendered.*, child-replay.rendered.*, ltr-again.rendered.* / R |

## Stack와 Flex

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| S nonweight/weight pass, spacing 차감, 축 분기 | TC104 S01.horizontal, S01.vertical | main300,fixed60,gap10×2,remaining220,weight1:2; slots60/220÷3/440÷3; 마지막 시작 80+220÷3,끝 300 | fixed.*, weight1.*, weight2.* / R |
| S Arrange cross START/CENTER/END/FILL | TC104 S04.cross.{0.3} | parent100×60,child20×10 →y0/25/50/0,fill height60 | aligned.* / R |
| S margin accumulation과 main overflow | TC104 S05.margin-gap-overflow | margin2/3/4/5,width30의slot35; gap7 후B.x42,parentwidth40밖에도 요청 보존 | margin.*, overflow.* / R |
| S StackCrossMode FILL EXACTLY와 ClampToOwnSizeBounds, RTL 끝 edge | TC104 S04.fill-maximum (ltr → rtl) | vertical300×100, FILL·max폭120 a: measured(120,20), 렌더(0,0,120,20), b(0,20,50,10); RTL a(180,0,120,20), logical(0,0,120,20), b(250,20,50,10) | a.*, a.rendered.*, b.rendered.*, a.rtl.*, a.logical.*, b.rtl.* / R |
| S cross MATCH EXACTLY, weight 없는 MATCH main AT_MOST | TC104 S11.match-report | vertical300×200: c MATCH 폭 margin10/20/5/0 →(270,20) at (10,5); d MATCH×MATCH →measured(300,0), 렌더(0,25,300,200) | c.*, c.rendered.*, d.*, d.rendered.* / R |
| S MeasureStackWeightChildren: share EXACTLY | TC104 S12.weight-exact-share | horizontal300×80, gap10, a60, weight1 probe →share230: (230,19), last-width230, (70,0,230,19) | b.*, b.rendered.*, b.last-width / R |
| S weight slot의 자기 min/max clamp(start edge 유지), 형제 allocation 불변 | TC105 S08.minimum, S08.maximum | weight1/1,parent200에서 slot 각 100; a는 min120/max60으로 clamp되어 (0,0,120,10)/(0,0,60,10), b는 재분배 없이 (100,0,100,10); measured 120/60 | measured.*, allocated-a.*, allocated-b.* / R |
| S WRAP의 weight intrinsic 기여와 parent min | TC105 S06.wrap-weight | 20+gap10+30=60; parentmin100이면 100 | wrap.*, minimum.* / R |
| S unweighted MATCH와 weighted MATCH 선택 | TC105 S07.match-overflow | fixed20 뒤 unweighted MATCH100은overflow; weight1이면remaining80 | unweighted.*, weighted.* / R |
| S clamp된 container bounds 안 child 배치 | TC105 S15.clamped-container | MATCH×MATCH vertical stack max폭120 in 300×200: s(0,0,120,200)/(120,200); g MATCH (120,30); END h (70,30,50,40) | s.*, s.measured.*, g.*, g.measured.*, h.* / R |
| S WRAP main이 EXACTLY로 측정되면 Measure에서 weight 분배 | TC105 S16.weighted-wrap-exact | FILL horizontal inner(EXACTLY200), a120 + weight wrapping flex →80에서 2줄 →inner(200,20), f(120,0,80,20), 셋째 Leaf(0,10,40,10) | inner.*, inner.rendered.*, f.rendered.*, f.third.* / R |
| S phase 1b: 크기 없는 cross의 MATCH child를 해결된 cross에서 EXACTLY 재측정 | TC105 S17.two-step-match | WRAP outer, seed100, MATCH 폭 wrapping flex: probe 200(1줄) →EXACTLY100 2줄 →outer·wf (100,20) | outer.*, outer.rendered.*, wf.*, wf.rendered.* / R |
| S phase 1b와 V HasSizeDependentContent: 폭에 따라 content가 바뀌는 FILL child 재측정 | TC105 S17.two-step-stretched | WRAP outer, seed100, FILL WRAP stack c 안 MATCH wrapping flex: c (0,10) →100에서 EXACTLY →c·outer (100,20), wf (0,0,100,20) | outer.*, outer.rendered.*, c.*, c.rendered.*, wf.rendered.* / R |
| F ApplyFlexGrowShrink: positive/negative free space | TC106 F06.grow, F07.shrink | grow basis20/40,free90,1:2→50/100; shrink basis100/200,parent250,weights100:400→90/160 | a.*, b.* / R |
| F grow/shrink slot의 자기 min/max clamp(start edge 유지), 재분배 없음 | TC106 F10.maximum, F11.minimum | grow slot100 / shrink slot50; a는 max60 / min80으로 clamp되어 (0,0,60,10)/(0,0,80,10), measured 60/80; b는 (100,0,100,10)/(50,0,50,10) | measured-a.*, allocated-a.*, allocated-b.* / R. clamp로 생긴 차이를 형제에게 재분배하지 않음을 검사합니다. |
| F WRAP measure 뒤 minimum으로 커진 arrange 폭에서 grow 분배 | TC106 F17.wrap-minimum-grow | WRAP flex min100, basis20/30 grow1/1 →measured100, free50 →45/55 | minimum.* / R |
| F basis0 대 intrinsic, MATCH grow 선택 | TC106 F08.zero-basis, F09.match-grow | intrinsic40/30이어도 explicitbasis0; grow1child100. fixed50뒤MATCHgrow150 | zero.*, growing.*, match.* / R |
| F shrink 후 개별 0 clamp, 재분배 없음 | TC106 F15.shrink-clamp | basis1/100,shrink100/1,parent10; deficit91을 같은 100가중치로 차감 →max(0,1-45.5)=0,100-45.5=54.5 | clamped.*, remaining.* / R. 이 clamp 정책에서 합계=parent10을 강제하지 않습니다. |
| F ResolveFlexLineModes main step: MATCH grow share를 Measure에서 EXACTLY 배정, 저장 slot 재사용, 재분배 없음 | TC106 F10.grow-maximum-kept (first → arrange-only) | flex300×50: a60 grow1, MATCH b grow1 max50 →free240, a180, b slot120을 50으로 clamp at x180; arrange-only 동일 | a.*, b.*, b.measured.* / R |
| F ResolveFlexLineModes: shrink로 좁아진 child를 share에서 EXACTLY 재측정 | TC106 F18.shrink-assigned-exact | flex100: probe80 shrink1, b60 shrink1 →deficit40 가중 80:60 →a 400/7, b 300/7; a measured(400/7,19), last-width400/7 | a.*, a.last-width, a.rendered.*, b.rendered.* / R |
| F BuildFlexLinesForArrange: exact fit/초과/wrap reverse | TC107 F02.wrap.* | main100/99,widths40/60/30;100은 첫두child같은line,99는첫child뒤break; reverse는 line cross순서 반전 | a.*, b.*, c.* / R |
| F 줄 단위 ApplyFlexGrowShrink: 각 line의 free space 분배와 단독 초과 item shrink | TC107 F02.wrap-grow-lines | parent100; [40g1,50g1] free10→45/55; [30g2,30] free40→70/30; [120 shrink1] 단독→100 | a, b, c, d, e / R |
| F Arrange alignContent START/CENTER/END/STRETCH | TC107 F13.content.{0.3} | lineheight10/20,parentcross100,free70;첫y0/35/70/0,stretch둘째y45 | line0.*, line1.* / R |
| V ClassifyProducerAsParent FLEX_WRAP: wrapping flex는 모든 child AT_MOST | TC107 F19.wrap-match-follower | wrapping flex100×80의 MATCH 폭 a: measured(0,10)(minimum), 렌더(0,0,100,10)(line 채움) | a.*, a.rendered.* / R |
| F GetFlexJustifyOffsets: 여섯 justify | TC108 F04.justify.{0.5} | parent140,width20/40,free80; START(0,20),END(80,100),CENTER(40,60),BETWEEN(0,100),AROUND(20,80),EVENLY(80/3,20+160/3) | a.*, b.* / R |
| F GetFlexJustifyOffsets: freeSpace<=0 | TC108 F12.overflow..{3.5} | SPACE_BETWEEN/AROUND/EVENLY에서 overflow로 음수 분산gap이 생기지 않음 | a.*, b.* / R; 구체 main/width는 해당 CPP/MD 입력을 사용합니다. |
| F ArrangeOneFlexLine: alignSelf AUTO/override | TC108 F05.align-self.{0.4} | parentEND,child20×10,cross60; AUTO/START/END/CENTER/STRETCH y50/0/50/25/0,stretchheight60 | self.*, inherited.* / R |
| F IsMainAxisHorizontal/IsMainAxisReversed | TC108 F01.direction.{0.3} | ROW/ROW_REVERSE/COLUMN/COLUMN_REVERSE;main100,child20/30;reverse80/50 | a.*, b.* / R |
| F STRETCH cross EXACTLY와 ClampToOwnSizeBounds | TC108 F05.stretch-clamp | flex300×100 STRETCH: a 높이 WRAP max40 →measured(60,40), 렌더(0,0,60,40); b (60,100) at (60,0) | a.*, a.rendered.*, b.*, b.rendered.* / R |

F ArrangeOneFlexLine의 `FlexAlign::BASELINE` 분기는 검사하지 않습니다. 현재 제품은 BASELINE을 FLEX_START와 같은 교차축 offset 0으로 배치하고 자식의 text baseline을 얻는 경로가 없어, text baseline 정렬 기능이 구현되어 있지 않습니다. 이 기능을 검사하던 TC는 존재하지 않는 기능을 검사하므로 2026-09-30 suite 관리자의 결정으로 삭제했습니다([suite 실행 명세](layout-validation-coverage.md)). 공개 헤더 `layout-types.h`는 `FlexAlign::BASELINE`을 "Baseline alignment"로만 설명하고 미구현 사실을 적지 않으므로, 이 문서와 공개 API 문서 사이의 간극은 제품 문서의 후속 과제로 남깁니다. FLEX_START와 같은 결과를 BASELINE의 expected로 삼는 행을 추가하지 마십시오. 기능이 구현되면 실제 rendered baseline 일치를 검사하는 TC를 새로 추가하고 이 표에 연결하십시오.

## Grid와 Absolute

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| G ApplyGridDefinitions/ComputeGridPositions: ABSOLUTE와 spacing | TC109 G01.absolute | rows20/30,gap5;columns40/50,gap7;각prefix에spacing포함 | a.*, b.*, c.* / R |
| G STAR 잔여 공간과 weight | TC109 G02.star | parent330,absolute50,spacing합 20,remaining260을 1:2분배 | a.*, b.*, c.* / R |
| G MeasureGridChildrenAndFillAuto: AUTO floor | TC109 G03.auto | AUTO40,gap10,parent200 →STAR150 | auto.*, star.* / R |
| G STAR weight0와 음수 잔여 clamp | TC109 G08.zero-star-overflow | parent40,absolute60,gap7 →STARwidth0,start67 | absolute.*, zero.* / R |
| G implicit cell/명시 definition 선택 | TC109 G04.implicit | parent100×60에서fullcell;explicitcolumn30,row20으로변경 | implicit.*, explicit.* / R |
| G WRAP measure 뒤 minimum으로 커진 arrange 폭에서 STAR 분배 | TC109 G15.wrap-minimum-star | WRAP grid min100, columns{Absolute30,Star1} →measured100, star열70 | minimum.* / R |
| G MeasureGridChildrenPhased: MATCH child의 cell 배정(ABSOLUTE·결정된 STAR 열은 Measure, STAR 행은 Arrange) | TC109 G17.match-cell-measured | grid300×100, 열{50,Star},행{40,Star}: m1 (50,40); m2 폭250(Measure), 높이60(Arrange) →(250,60) at (50,40) | m1.*, m1.rendered.*, m2.*, m2.rendered.* / R |
| G span extent와 내부 spacing | TC110 G05.span-spacing | span2width30+7+50=87,height20+5+30=55;둘째spanx37,y25,127×75 | span00.*, span11.* / R |
| G multi-span AUTO deficit 배분 | TC110 G06.auto-deficit | track20/30,gap10→60;span100의deficit40을둘에 20씩→40/50 | auto-a.*, auto-b.* / R |
| G ArrangeGridChildrenToCells: alignment, margin/padding | TC110 G09.align.{0.3}, G10.padding-margin | cell100×60,child20×10 →START0,0/CENTER40,25/END80,50/FILL100×60;별도padding/margin slot5,13,106,48 | aligned.*, cell.* / R |
| G ArrangeGridChildrenToCells: clamp된 measured 크기로 정렬, FILL은 cell을 start edge에서 clamp | TC110 G09.clamped-align.{0..3} | cell100×50, probe80×40 max60×30: measured(60,30); START(0,0)/CENTER(20,10)/END(40,20)/FILL(0,0), 폭60·높이30; b(100,0,200,50) | a.*, a.rendered.*, b.rendered.* / R |
| G 크기 없는 STAR 열의 share와 cell보다 넓은 WRAP child의 Arrange 재측정 | TC110 G18.oversize-remeasured | WRAP grid 높이30 min폭100, 열{30,Star}: STAR=max(30,100)−30=70; probe150 →Arrange에서 70 cell 재측정 (70,19), last-width70, (30,0,70,30) | grid.*, a.rendered.*, b.*, b.last-width, b.rendered.* / R |
| G row/column index clamp; P span minimum | TC111 G12.index..{0,1,UINT_MAX}, G12.span..{0,1,2,UINT_MAX} | first0,0,30,20;last/초과 37,25,50,30;span0→1,span2/초과→남은유효 2track87×55 | cell.*, params, normalized-span, span.* / R |
| G definition clear와 implicit fallback | TC111 G13.definition-clear | 명시definition제거 후parent100×60fullcell | defined.*, cleared.* / R |
| A explicit local bounds와 parent이동 | TC112 A01.explicit-local | parent70,90→100,120에도child11,13,37,19유지 | child.*, moved-parent.* / R |
| A explicit extent경계와 음수position | TC112 A02.extent.*, A07.wrap-negative-position | width0/1/37/250,x=-20,y7;WRAP far edge -20+100=80,-5+20=15 | explicit.*, wrap.* / R |
| A intrinsic extent sentinel과 MATCH delegate | TC112 A03.intrinsic-match | sentinel에서요청 37×19;MATCH로바꾸면 100×60 | intrinsic.*, match.* / R |
| A 명시 bounds extent EXACTLY, MATCH 축의 content extent 배정 | TC112 A09.bounds-exact | layout200×100: probe bounds(10,20,70,30) →(70,30), last(70,30); MATCH m bounds(5,7,−1,−1) →(200,100) at (5,7) | c.*, c.last-width, c.last-height, c.rendered.*, m.*, m.rendered.* / R |
| A 네 proportional flag 독립 선택 | TC113 A04.flags.{0.15}, A05.all-flags | parent200×100,bounds(.25,.5,.5,.25);비율width100/height25,position=(parent-child)*p;ALL(.5,.5,.5,.5)→50,25,100,50 | flags.*, all.* / R |
| A position비율 [0,1] 바깥과RTL | TC113 A08.position.* | p=-.25/0/.5/1/1.25,parent300,child100 →x=200p;RTL=200-200p | ltr.*, rtl.* / R |
| A Measure: WRAP의 circular contribution 제외 | TC113 A06.wrap-circularity | position만비율+width40 →WRAP40×20;width비율이면해당축기여 0 →0×20 | position-only.*, size-circular.* / R |
| A 비례 slot의 ClampToOwnSizeBounds, clamp 전 extent로 계산한 비례 위치 | TC113 A10.proportional-maximum | layout300×200, 폭.5 max100: a (0,0,100,20); X·W 비례 b x=(300−150)×1=150 →(150,30,100,20) | a.*, a.rendered.*, b.*, b.rendered.* / R |
| A 두 단계: WRAP layout이 폭을 정한 뒤 비례 child 측정 | TC113 A11.proportional-last | WRAP layout, b100×10, 폭.5 probe: last-width .5×100=50, (50,19), layout(100,19) | a.last-width, a.*, a.rendered.*, layout.* / R |

## Standalone, cache, invalidation, ownership

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| V/S/F/G/A normal/STANDALONE contribution | TC114 S10.standalone.{0.4}, G14.mode-change | standalone은parentWRAP/spacing/index계산제외;요청 7,9에서 도출한 (7,9,100,80)에 배치;mode변경포함/제외재계산 | parent-measured.*, independent.*, logical-count, ordinary.*, excluded.* / R |
| ST DeriveStandaloneRootBounds/ArrangeStandaloneChild MATCH양축 | TC114 S10.match-margin-clamp.{0.4} | parent200×100,padding20/30/40/10무시;ownmargin7/11/13/17,request5/9 →12,22,182,70;ownminW190/max195,minH20/max60 →12,22,190,60 | full-parent-minus-own-margin.*, own-minmax-after-match.* / R |
| ST DeriveStandaloneMeasureConstraint MATCH: 부모 extent−margin을 자기 min/max로 clamp해 EXACTLY | TC114 S10.match-measured (mount → arrange-only) | parent200×150, margin5/0/7/0,max60×45 →(5,7,60,45), measured(60,45); 부모 InvalidateArrange 뒤 동일 | s.*, s.measured.* / R |
| ST WRAP: window−margin 제약(window 크기는 입력) | TC114 S10.wrap-window-bound | parent20×10, probe150×90 요청(3,4) margin1/2/3/4: last (W−3,H−7), measured(150,90), (4,7,150,90) | s.last-width, s.last-height, s.measured.*, s.* / R |
| ST 명시 크기: 요청×scale, margin은 바깥 | TC114 S10.explicit-requested | probe 요청40×30, 위치(5,6), margin2/0/3/0: last(40,30), (7,9,40,30); parent (20,10) | s.last-width, s.last-height, s.measured.*, s.*, parent.measured.* / R |
| ST 축별 혼합: WRAP는 window, MATCH는 부모 | TC114 S10.mixed-axes (parent-120 → parent-200) | parent100×120→200, 폭WRAP·높이MATCH probe150×90: (150,120)→(150,200), last-height 120→200 | s.measured.*, s.last-height, s.* / R |
| V MeasureStandaloneChildren 잠정 크기, ArrangeStandaloneChild 재측정 | TC114 S10.placement-remeasure | WRAP parent(ordinary120×60), MATCH×MATCH probe: 잠정200×100 뒤 최종120×60 재측정 →last(120,60), (0,0,120,60) | s.*, s.measured.*, s.last-width, s.last-height, parent.measured.* / R |
| V SetLayoutMode STANDALONE→DEFAULT, CT ReleaseLayoutRootInternal | TC114 G14.mode-release (standalone → release) | spacing7 stack, a20×10, standalone30×15 at(50,40); 같은 action에서 폭35+DEFAULT →(27,0,35,15), root(62,15)(등록이 남아 자기 turn이 실행되면 요청 위치 (50,40)에 재배치) | s.*, root.measured.* / R |
| V Measure cold/hit,height key 변화 | TC115 K01.same-key | 같은 100×50producer1;height51에서producer2 | cold.*, hit.*, producer-once, height-key / R |
| V normalized min/max key | TC115 K02.normalized-key | minW120,maxH30;raw40×100/50×200모두 120×30;measured120×19 | producer-once, normalized-width, normalized-height, clamped-result.* / R |
| V explicit invalidation과 producer 내부값 변화 구분 | TC115 K03.explicit-invalidation | intrinsic37→53만바꾸면 37hit;InvalidateMeasure뒤 53,다음hit | cached.*, unchanged-producer, invalidated.*, fresh-producer, fresh-cache / R |
| V ancestor invalidation과 clean sibling cache | TC115 K04.sibling-isolation | A20→40,B30;root70;Aproducer증가/B유지 | root.*, dirty-producer, clean-producer / R |
| V Measure(mode 없는 public) → DeriveMeasureModes: parent 종류별 pass와 같은 mode 유도 | TC115 K11.application-measure.{0..6} (pass → app-hit; k6은 K12 뒤에 선언) | 100×60 container k의 30×10 probe: pass measured k0 (100,60), k1 (100,60), k2 (100,10), k3 (30,60), k4 (100,60), k5 (50,40), k6 ScrollView content(MATCH 폭 EXACTLY 100, WRAP 높이 UNSPECIFIED FLT_MAX) (100,10), producer1; 같은 숫자(k6은 (100,FLT_MAX))의 application Measure는 cache hit: 반환 = pass measured, callback leaf의 producer 불변 | measured.*, producer-once, rendered.*, returned.*, producer-unchanged / R |
| V CanSkipArrangeRemeasure: 같은 slot의 측정이 있으면 Arrange 재측정 생략 | TC115 K12.arrange-remeasure-skip (mount → invalidate) | 자기 root인 stage100×60의 MATCH×MATCH probe: measured(100,60); InvalidateMeasure 뒤 producer 정확히 +1(Arrange 시점 재측정이 producer를 다시 실행하면 +2), measured(100,60) | measured.*, rendered.*, measure-producer / R |
| V ApplySelfBoundsIfChanged/ReplayArrangeSubtreeFromCache | TC116 K08.actor-repair, K09.subtree-replay | 유효Measure/Arrange후Actor손상→hit에서rect복구;PIVOT.x=.5유지,POSITION_USES_PIVOT=false | initial.*, repaired.*, producer-once, pivot-x, placement-ignores-pivot, origin-x, a.*, b.*, a-producer, b-producer / R |
| V ArrangeImpl의 callback 반환 publication/hit | TC116 K10.returned-bounds | 유효Measure후입력과다른finite반환rect;동일입력hit는반환target복원 | returned.*, hit.*, producer / R |
| V ArrangeImpl: producer는 clamp된 slot을 받고 arrange key는 raw slot | TC116 K21.clamped-arrange-hit (mount → external-hit → pass) | MATCH×MATCH probe max120×80 in 300×200: callback 수신(0,0,120,80), measured(120,80); actor SIZE 손상 뒤 Arrange(0,0,300,200)은 hit로 (0,0,120,80) 반환·복원, producer 불변; 형제 pass도 hit | received.*, measured.*, c.*, hit.*, repaired.*, no-arrange-producer, no-measure-producer, pass.no-arrange-producer / R |
| V CanServeArrangeFromCache의IF_CHANGED/ALWAYS/subtree gate | TC117 K15.if-changed, K15.always, K16.always-descendant, K16.policy-change | Measure선행;반복 3회IF_CHANGED1/ALWAYS3;mixed의ALWAYS실행/clean sibling hit;policy교체 3→4 | producer, bounds.*, always-runs, clean-sibling-hit, a.*, b.*, always-total, if-changed-total / R |
| DEP/V InvalidateMeasure ancestor 전파 | TC118 K06.generation / mount, invalidate | WRAP stack root 안 probe 20×10의 보고 폭을 30으로 바꾸고 child InvalidateMeasure;다음 pass에서 producer 재실행과 root measured 30×10 | producer-reran, root.measured.*, probe.rendered.* / R |
| V InvalidateArrange: Measure producer 재실행 없음 | TC118 K07.arrange-only / mount, invalidate-arrange, repeat | Arrange만무효화;다음Arrange후measureproducer1/arrangeproducer2;stage만 InvalidateArrange하면 같은 bounds 재배치는 cache가 처리(1/2 유지) | measure-producer, arrange-producer, repeat.measure-producer, repeat.arrange-producer / R |
| DEP ArrangeOwnedMeasureScope,V cache-miss ancestor stop | TC119 K13.owner.{0..4} / run | 자기 min/max가 없는 wrapping parent와 유일한 child MATCH×MATCH probe(30×10)를 300×100 stage에 부착: parent는 자기 크기가 없어 child의 at-most probe(첫 producer constraint 300×100)를 유지하여 30×10이 되고, Arrange의 final slot 30×10에서 child를 재측정(owned constraint 30×10). 이미 게시된 parent measured size와 같은 constraint의 producer 호출이 Arrange 시점 재측정 | owned-producer-observed, budget-constraint.*, owned-constraint.*, measured.*, final-slot.* / R |
| DEP owner 처리 뒤 parent cache 유지 | TC119 같은scenario / stable | 형제 leaf만 바꾼 pass에서 child producer 재실행 없음(소유가 잘못 처리되면 parent가 child를 다시 측정) | stable.no-producer / R |
| V MeasureDefault/A Measure: 자기 minimum이 parent에 크기를 주면 MATCH child를 Measure에서 slot에 EXACTLY, Arrange는 재측정 생략 | TC119 K13.measure-pass-slot.{0,4} | WRAP parent min120×60, MATCH×MATCH probe30×10: 두 호출 모두 parent의 measured size 게시 전이라 Arrange 시점 기준의 호출 owned-calls0, probe producer 2회(probe와 exact), 마지막 constraint·measured (120,60), (0,0,120,60) | owned-calls, child-producer, final-measure-constraint.*, measured.*, child.* / R |
| V InvalidateAncestorLayoutCachesForMeasureMiss: out-of-band | TC120 K14.external-measure / mount, external, repeat | 최종slot200×80뒤외부 70×40(producer+1);다음parent계산이 child를 200×80 constraint로 재측정(ancestor cache clear 자체는 관측하지 않음);이후 형제 pass는 producer 없음 | before.slot.*, external.producer, external.constraint.*, restored.constraint.*, restored.*, repeat.no-producer / R |
| ST 하나의 STANDALONE 유도식, unconsumed slot corrective Measure와 standalone boundary | TC120 K14.external-standalone-measure / mount, warmup, external, repeat, path-independence | intrinsic300×90, FIXED parent200×80. WRAP STANDALONE은 모든 경로에서 window−margin으로 측정되어 slot (7,9,min(300,W),min(90,H))(W×H는 main window, 입력); mount 직후 부모 measure가 유일한 producer 실행이라 producer 1회; warmup의 부모 pass 한 번(producer 기준 재설정); 외부 70×40 측정 뒤 다음 parentArrange가 child를 window 제약으로 한 번 보정 재측정해 slot 복원(producer+2); 반복+0; 부모 InvalidateMeasure 뒤에도 같은 slot, producer 불변 | before.slot.*, mount.producer-once, warmup.slot.*, external.measure-result.*, corrective-child-producer, corrected.slot.*, repeat.no-child-producer, path-independence.slot.*, path-independence.no-producer / R |
| V InvalidateParentArrangeCacheForOutOfBandArrange | TC120 K14.external-arrange | 외부childArrange로owner기록slot변경;다음parentArrange정상slot재생성 | external.*, restored.* / R |

standalone slot의 소비 bit, TC119의 dependency owner, TC120의 cache 상태는 내부 상태이므로 관측하지 않으며, 각 TC는 그 효과를 producer 호출 수와 constraint로 판정합니다.

## 완료, PARK, reentry, replay 변경, tree/Window lifetime

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| CT View완료queue→Windowfence;V LayoutFinished delivery | TC121 L03.fenced-snapshots / mount, resize-child | A20×10,B30×15→Awidth40;같은episode에서A0,0,40,10/B40,0,30,15 | a.*, b.*, a-notified, b-notified, changed.*, shifted.* / W |
| API setter no-op와 directArrange 완료신호 | TC121 같은scenario / no-op-setter, direct-arrange | 같은width40설정및directArrange7,9,40,10은동기완료count불변 | a-no-synchronous-signal, b-no-synchronous-signal, no-manual-completion, manual-target.* / R. 동기비교만으로장시간신호부재를주장하지마십시오. |
| CT 처리중 재무효화 pending보존/PARK | TC122 K17.passive-park / prepare, arm-passive-observation |37×19정착후producer자체Measure무효화;수동처리에서 producer 진입;1초settle뒤10초quiet구간에서 이Window의 layout 완료 0회, producer불변 | settled.*, producer-entered, not-saturated, quiet.layout-passes, quiet.producer-unchanged, quiet.not-saturated; TC122_PARK_ARMED/TC122_PARK_QUIET/TC122_PARK producer / W+R |
| CT 외부event진행과 producerdisarm | TC122 같은scenario / external-event-and-stop | action입구before와Invalidate+Process후after비교;callback제거 | external-progress, disarmed, captured-before-positive, captured-after-positive, TC122_PARK_STOP / R. quiet구간은앞arm step안에서끝납니다. |
| CT 자연episode완료와 idlewake퇴장 | TC122 같은scenario / recover-window-and-drain | disarm뒤AfterLayoutarm→Invalidate,수동Process없이 37×19;자연fence뒤one-shot100ms에서 probe 재측정 없음 | recovery.disarmed, recovery.callback-cleared, recovery.geometry.*, recovery.park-producer-stopped, drained.no-producer, drained.park-producer-stopped, drained.not-saturated / W+R |
| V MeasurePassGuard/Arrange pass rollback | TC123 K18.measure-throw, K18.arrange-throw | 완료 37×19또는 1,2,37,19후producerthrow;완료record보존. Arrange Actor에는provisional7,9,53,29가보임;retry후producer3,동일retrycache에서도 3 | exception-propagated, provisional-input.*, last-completed.*, retry.*, retry-producer, retry-cache.*, retry-cache-producer / R. Arrange의last-completed는ViewImpl::GetArrangedBounds()로읽습니다. |
| V Measure 같은View reentry poison guard | TC123 K18.measure-reentry | callback안같은View Measure;무한producer재진입없이정상 37×19복구 | entered, bounded, recovered.* / R |
| V child snapshot/Add/Remove/RemoveAll invalidation | TC124 K19.add-remove, K20.mutate-during-producer | A제거+C추가후B.x0,C.x37;RemoveAlllogical0;Measure중B추가후후속passB.x20 | child-count, first, b.*, c.*, empty.*, empty-count, out-of-range, children, added, later-child.* / R |
| V 상속 RaiseAbove와 RaiseAbovePreservingLayout의 logical order | TC125 ORDER.update, ORDER.preserve | A20,B30,C40에서 A.RaiseAbove(C)는 B,C,A/A.x70; RaiseAbovePreservingLayout(A,C)는 A,B,C/A.x0 | first, last, a.*, b.* / R |
| V ...PreservingLayout() 6종의 ScopedSkipChildrenUpdate | TC125 ORDER.preserve-raise, ORDER.preserve-lower, ORDER.preserve-lower-below, ORDER.preserve-raise-to-top, ORDER.preserve-lower-to-bottom, ORDER.preserve-raise-above | 실제Actor순서B,A,C / A,C,B / C,A,B / B,C,A / C,A,B / B,C,A;logicalA,B,C와x0/20/50유지 | logical.count, actor.count, logical.order.{0.2}, actor.order.{0.2}, preserved.a.*, preserved.b.*, preserved.c.* / R |
| V OnChildOrderChanged의 SyncChildrenWithActorOrder: 보고된 순서 변경은 logical order 전체를 Actor 순서로 재구성 | TC125 ORDER.rebuild-reorder / lower-c, ORDER.rebuild-insert-existing / insert-c-above-b | RaiseToTopPreservingLayout(A) 뒤 C.Lower(): 두 순서 C,B,A, x0/40/70; RaisePreservingLayout(A) 뒤 InsertAbove(C,B): 두 순서 B,C,A, x0/30/70 | logical.count, actor.count, logical.order.{0.2}, actor.order.{0.2}, a.*, b.*, c.* / R |
| V OnChildAdded: 마지막이 아닌 위치에 삽입한 새 View의 SyncChildrenWithActorOrder | TC125 ORDER.rebuild-insert-new / insert-d-above-b | RaisePreservingLayout(A) 뒤 새 D15를 InsertAbove(D,B): 두 순서 B,D,A,C, x0/30/45/65(새 View만 끼우면 A,D,B,C) | logical.count, actor.count, logical.order.{0.3}, actor.order.{0.3}, a.*, b.*, c.*, d.* / R |
| V OnChildAdded의 마지막 child append와 OnChildRemoved: logical order 유지 | TC125 ORDER.keep-add / add-d, ORDER.keep-insert-last / insert-d-above-a, ORDER.keep-remove / remove-c | Add(D): Actor B,A,C,D; InsertAbove(D,A): Actor B,C,A,D; 둘 다 logical A,B,C,D, x0/20/50/90. Remove(C): Actor B,A, logical A,B, x0/20 | logical.count, actor.count, logical.order.{i}, actor.order.{i}, a.*, b.*, c.*, d.*(remove-c는 a.*, b.*) / R |
| V child filtering: non-ViewActor | TC125 ORDER.non-view | 장식Actor1+View2;actor3/logical2,logicalindex2empty,B.x20 | logical, actor, out-of-range, b.* / R |
| V parent변경과 old/newrootcache invalidation | TC126 L01.reparent | leftA20+gap7+B30,rightC40;B이동뒤left20×10,right40+5+30=75×20,B.x45 | old-root.*, new-root.*, old-count, new-count, moved.*, attached-to-root / R |
| V detached/off-scene 계산 | TC126 L02.detached-measure | child37×19요청 7,9;root44×28→detachroot0×0,child37×19→reattach44×28 | empty.*, detached-child.*, reattached.* / R |
| V window 없는 WRAP STANDALONE 축의 부모 extent, scene 재연결 뒤 window 측정(scene 해제의 cache drop(OnViewSceneDisconnection)·scale root subtree reset과 재등록은 내부 상태라 관측하지 않음) | TC126 L05.standalone-offscene (mount → detach → reattach) | window에 직접 붙인 stage200×110(부모 Layer) 안 parent200×80, WRAP probe300×90 at(7,9): window bound (7,9,min(300,W),min(90,H)); stage를 window에서 떼면(View 부모의 OnChildRemoved reset 없음) probe measured 유지, off-scene Arrange(0,0,200,80) →event-side (7,9,200,80), measured (200,80); stage를 다시 붙이면 window bound 복귀 | probe.*, probe.measured.*, kept.measured.*, offscene.placement.*, offscene.measured.* / R |
| CT per-Window Get/Remove/재생성 | TC127 L04.window-recreate / secondary-window, remove-and-recreate-controller | secondary220×140의child37×19,Remove/Get뒤child53×19를새fence로확인;main생존 | secondary-distinct, secondary-child.*, recreated-child.*, main-window-alive / W |
| CT Windowrootdetach와 publichandle정리 | TC127 같은scenario / detach-close | rootunparented,secondaryhandleReset,main유지 | root-unparented, window-handle-released, main-window-retained / R. publichandle해제를모든내부객체의즉시destruction증거로확대하지마십시오. |

## 고정 seed와 변환 불변식

| Source 의사결정 | TC / scenario, action | 입력 경계와 독립 oracle | 실제 check ID / 관측 |
|---|---|---|---|
| S nonweight prefix와 축교환 | TC151 R01.seed..{1,17,73,257,65537} | uint32 LCG s=s*1664525+1013904223;16child w=1+s%47,h=1+s%31,gap3;intrinsic(Σw+45,maxh),각prefix,verticaltranspose | intrinsic.*, horizontal.{0.15}.*, vertical.{0.15}.* / R |
| F equalgrow와reverse/RTL/translation/transpose | TC151 R02.flex-seed..{1,73,65537} |8childbasis=width,grow1,total=Σwidth+120→각width+15;reverse W-prefix-width;reverse+RTL두번반전;parent31,17이동local불변 | flex.forward..*, flex.reverse..*, flex.reverse-rtl..*, flex.translated-parent..*, flex.transposed..* / R |
| G ABSOLUTEprefix와RTL/translation/transpose | TC151 R04.grid-seed..{1,73,65537} |4rows20.39,3cols15.34,rowgap3,colgap5,12cell;widthΣcol+10,heightΣrow+9 | grid.ltr..*, grid.rtl..*, grid.translated-parent..*, grid.transposed..* / R |
| A finiteabsolute/proportional과RTL/translation/transpose | TC151 R05.absolute-seed..{1,73,65537} |12child,parent200×120;짝수absolutex=-10.39,홀수p=-.25.1.25에서x=(200-w)p;y=-5.24;transposeparent120×200 | absolute.ltr..*, absolute.translated-parent..*, absolute.rtl..*, absolute.transposed..* / R |

seed는 위의 제한된 유효 입력 영역을 재현합니다. 무작위 미실행 입력, 모든 params 조합, invalid-domain 전체, failure shrink의 성공을 이 TC의 PASS로 주장하지 마십시오. R06–R08의 축소·mutation·coverage 보조 작업은 별도 artifact와 재실행 증거가 필요합니다.

## 검토와 실행 결과를 연결하는 절차

1. 아래 CPP/MD 쌍에서 scenario의 실제 입력과 check를 읽으십시오. table 패턴을 계약 로그의 구체 ID로 펼치고 action의 모든 assertion을 검사하십시오.
2. 관심 source 함수·조건에 대해 해당 action 구간 coverage를 수집하십시오. 가능하면 action 시작 전 counter reset, 완료 후 dump 경계를 두고 root/child 역할을 기록하십시오. 지원되지 않는 reset/dump를 수행했다고 적지 마십시오.
3. launcher/HUD가 우연히 밟은 branch는 제외하십시오. 진입 coverage에 더해 fixture 입력, node/action, actual observer, oracle, 실패 가능한 assertion을 함께 연결해야 합니다.
4. observation buffer의 overflow check를 포함하여 검증하십시오. 내부 상태를 추정하여 성공/실패를 만들지 마십시오.
5. positive-control mutation은 원본의 같은 assertion PASS를 먼저 확보하고, 격리 mutant에서 그 assertion이 예상 이유로 FAIL하는지 대조하십시오. compile failure, crash, timeout, 원본부터 FAIL은 kill이 아닙니다.
6. 표에 없는 source 조건은 함수·조건·입력·필요 observer·후보 assertion을 누락 목록에 기록하십시오. 기존 행의 의미를 늘려 미실행 branch를 채우거나 coverage 비율만으로 완료 처리하지 마십시오.

전역 scale의 모든 reparent 조합, manager setter의 모든 route 조합, RaiseToTop/LowerToBottom 개별 경계, 깊은 ancestor stop 조합, observer가 바꾸는 모든 tree 형태는 위 대표 fixture만으로 각각 검증되었다고 볼 수 없습니다. 별도 신규 TC의 실제 assertion 연결이 있으면 교차 참조로 추가하고, 없으면 추가 fixture가 필요한 차이로 기록하십시오. Text/resource, Scroll/Recycler, hit/clip, RenderEffect, transition, controller 추가 경로와 성능은 [suite 책임표](layout-validation-coverage.md)의 대응 TC128–150/TC152–156 및 각 MD에서 추적하십시오.

## CPP/MD 찾아가기

- TC95: [tc-95-layout-core-constraints.cpp](tc/tc-95-layout-core-constraints.cpp) · [실행 명세](tc/tc-95-layout-core-constraints.md)
- TC96: [tc-96-layout-core-invalid-inputs.cpp](tc/tc-96-layout-core-invalid-inputs.cpp) · [실행 명세](tc/tc-96-layout-core-invalid-inputs.md)
- TC97: [tc-97-layout-default-view.cpp](tc/tc-97-layout-default-view.cpp) · [실행 명세](tc/tc-97-layout-default-view.md)
- TC98: [tc-98-layout-params-values.cpp](tc/tc-98-layout-params-values.cpp) · [실행 명세](tc/tc-98-layout-params-values.md)
- TC99: [tc-99-layout-dispatch.cpp](tc/tc-99-layout-dispatch.cpp) · [실행 명세](tc/tc-99-layout-dispatch.md)
- TC100: [tc-100-layout-tracked-setters.cpp](tc/tc-100-layout-tracked-setters.cpp) · [실행 명세](tc/tc-100-layout-tracked-setters.md)
- TC101: [tc-101-layout-numeric-boundaries.cpp](tc/tc-101-layout-numeric-boundaries.cpp) · [실행 명세](tc/tc-101-layout-numeric-boundaries.md)
- TC102: [tc-102-layout-scale.cpp](tc/tc-102-layout-scale.cpp) · [실행 명세](tc/tc-102-layout-scale.md)
- TC103: [tc-103-layout-direction.cpp](tc/tc-103-layout-direction.cpp) · [실행 명세](tc/tc-103-layout-direction.md)
- TC104: [tc-104-layout-stack-allocation.cpp](tc/tc-104-layout-stack-allocation.cpp) · [실행 명세](tc/tc-104-layout-stack-allocation.md)
- TC105: [tc-105-layout-stack-wrap-clamp.cpp](tc/tc-105-layout-stack-wrap-clamp.cpp) · [실행 명세](tc/tc-105-layout-stack-wrap-clamp.md)
- TC106: [tc-106-layout-flex-allocation.cpp](tc/tc-106-layout-flex-allocation.cpp) · [실행 명세](tc/tc-106-layout-flex-allocation.md)
- TC107: [tc-107-layout-flex-lines.cpp](tc/tc-107-layout-flex-lines.cpp) · [실행 명세](tc/tc-107-layout-flex-lines.md)
- TC108: [tc-108-layout-flex-alignment.cpp](tc/tc-108-layout-flex-alignment.cpp) · [실행 명세](tc/tc-108-layout-flex-alignment.md)
- TC109: [tc-109-layout-grid-tracks.cpp](tc/tc-109-layout-grid-tracks.cpp) · [실행 명세](tc/tc-109-layout-grid-tracks.md)
- TC110: [tc-110-layout-grid-spans.cpp](tc/tc-110-layout-grid-spans.cpp) · [실행 명세](tc/tc-110-layout-grid-spans.md)
- TC111: [tc-111-layout-grid-boundaries.cpp](tc/tc-111-layout-grid-boundaries.cpp) · [실행 명세](tc/tc-111-layout-grid-boundaries.md)
- TC112: [tc-112-layout-absolute-bounds.cpp](tc/tc-112-layout-absolute-bounds.cpp) · [실행 명세](tc/tc-112-layout-absolute-bounds.md)
- TC113: [tc-113-layout-absolute-proportions.cpp](tc/tc-113-layout-absolute-proportions.cpp) · [실행 명세](tc/tc-113-layout-absolute-proportions.md)
- TC114: [tc-114-layout-standalone.cpp](tc/tc-114-layout-standalone.cpp) · [실행 명세](tc/tc-114-layout-standalone.md)
- TC115: [tc-115-layout-measure-cache.cpp](tc/tc-115-layout-measure-cache.cpp) · [실행 명세](tc/tc-115-layout-measure-cache.md)
- TC116: [tc-116-layout-arrange-replay.cpp](tc/tc-116-layout-arrange-replay.cpp) · [실행 명세](tc/tc-116-layout-arrange-replay.md)
- TC117: [tc-117-layout-arrange-policy.cpp](tc/tc-117-layout-arrange-policy.cpp) · [실행 명세](tc/tc-117-layout-arrange-policy.md)
- TC118: [tc-118-layout-invalidation-generation.cpp](tc/tc-118-layout-invalidation-generation.cpp) · [실행 명세](tc/tc-118-layout-invalidation-generation.md)
- TC119: [tc-119-layout-dependency-ownership.cpp](tc/tc-119-layout-dependency-ownership.cpp) · [실행 명세](tc/tc-119-layout-dependency-ownership.md)
- TC120: [tc-120-layout-out-of-band.cpp](tc/tc-120-layout-out-of-band.cpp) · [실행 명세](tc/tc-120-layout-out-of-band.md)
- TC121: [tc-121-layout-view-completion.cpp](tc/tc-121-layout-view-completion.cpp) · [실행 명세](tc/tc-121-layout-view-completion.md)
- TC122: [tc-122-layout-park.cpp](tc/tc-122-layout-park.cpp) · [실행 명세](tc/tc-122-layout-park.md)
- TC123: [tc-123-layout-reentry-exceptions.cpp](tc/tc-123-layout-reentry-exceptions.cpp) · [실행 명세](tc/tc-123-layout-reentry-exceptions.md)
- TC124: [tc-124-layout-replay-mutation.cpp](tc/tc-124-layout-replay-mutation.cpp) · [실행 명세](tc/tc-124-layout-replay-mutation.md)
- TC125: [tc-125-layout-tree-order.cpp](tc/tc-125-layout-tree-order.cpp) · [실행 명세](tc/tc-125-layout-tree-order.md)
- TC126: [tc-126-layout-reparent-offscene.cpp](tc/tc-126-layout-reparent-offscene.cpp) · [실행 명세](tc/tc-126-layout-reparent-offscene.md)
- TC127: [tc-127-layout-window-cleanup.cpp](tc/tc-127-layout-window-cleanup.cpp) · [실행 명세](tc/tc-127-layout-window-cleanup.md)
- TC151: [tc-151-layout-seeds.cpp](tc/tc-151-layout-seeds.cpp) · [실행 명세](tc/tc-151-layout-seeds.md)
