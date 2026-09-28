# Transition / controller / TC155의 source–assertion 연결표

이 문서는 신규 TC128–TC142 및 TC155의 **정적 연결표**입니다. source 조건, fixture 입력, 실행할 scenario/action, 독립 기대값, 실제 check ID를 연결합니다. 아래 `MAPPED_STATIC`은 assertion이 코드에 정의되었다는 뜻이며 실행 성공, branch true/false 도달, mutation 검출을 뜻하지 않습니다. 현재 문서에는 실행 evidence를 붙이지 않았습니다. 기존 TC나 launcher 동작을 이 표의 검증 실적으로 합산하지 마십시오.

각 TC의 대응 MD를 따라 실제 UI로 실행하십시오. 결과의 `(tc,pid,run,scenario,step,action_seq,row)`와 source revision/build-ID를 연결하고, 필수 전체 행 및 마지막 `LR_CASE_END`의 cleanup 성공을 보존해야 실행 증거가 됩니다. 같은 action의 서로 다른 입력은 고유 check ID를 사용합니다. 같은 ID를 재출력한 행은 중복 관측이며 별도 검사로 세지 마십시오. 모든 관측은 공개 API와 lifecycle·animator callback으로만 수행합니다.

## Source와 관측 규칙

아래 `파일 별칭:line`은 이 문서 작성 시 읽은 위치입니다. 함수명과 조건식을 함께 사용하십시오. 변경 후 line만 일치하는 다른 코드에 연결하지 마십시오.

| 별칭 | 파일 |
|---|---|
| D | [layout-transition-dispatcher.cpp](../../dali-ui-foundation/internal/layouts/layout-transition-dispatcher.cpp) |
| V | [layout-transition-validation.cpp](../../dali-ui-foundation/internal/layouts/layout-transition-validation.cpp) |
| I | [layout-transition-impl.cpp](../../dali-ui-foundation/internal/layouts/layout-transition-impl.cpp) |
| R | [layout-reflow-resolver.h](../../dali-ui-foundation/internal/layouts/layout-reflow-resolver.h) |
| B | [layout-bounds-effects.cpp](../../dali-ui-foundation/public-api/layouts/layout-bounds-effects.cpp) |
| T | [layout-transition-types.h](../../dali-ui-foundation/public-api/layouts/layout-transition-types.h) |
| C | [layout-controller.cpp](../../dali-ui-foundation/public-api/layouts/layout-controller.cpp) |
| F | [layout-validation-transition-fixtures.h](tc/layout-validation-transition-fixtures.h) |
| P | [tc-155-layout-transition-performance.cpp](tc/tc-155-layout-transition-performance.cpp) |

- `rect(id)`는 실제 check ID `id.x`, `id.y`, `id.width`, `id.height`의 4행입니다. `size(id)`는 `id.width`, `id.height`의 2행입니다. 기본 좌표 허용오차는 절대 `0.001`, Spec rendered rect는 `0.01`, Spec progress는 `0.0001`, Animator 곡선/시간은 `0.00001`입니다. 정수·bool·문자열은 exact이며 finite 조건이 먼저입니다.
- `mount.target`은 F:327 `Mount`가 해당 Window의 완료 fence에서 읽은 child target입니다. 일반 fixture는 parent 300×200, child `(40,30,80,40)`입니다. `Snapshot`의 target과 `Current`의 실제 animation current property는 다른 관측입니다.
- F:364 `CheckObservation`은 `context.slot`, `context.cause`, `rect(context.from)`, `rect(context.to)`입니다. production Animator callback이 전달한 context를 복사하며 callback이 직접 쓴 Actor 좌표를 정답 근거로 사용하지 않습니다.
- 진행 중 geometry는 표본화하지 않습니다. transition은 시작한 action에서 동기 사실(start 수, 수동 `ProcessLayouts()`의 마지막 animator tick이 새 animator를 raw 0으로 dispatch한 첫 context)을 검사하고, 자연 완료는 `Run::AfterFrame`의 readiness로 lifecycle event(`OnFinished`)를 기다린 뒤 새 render task 완료 후의 끝 상태를 검사합니다. readiness는 event만 확인하며 expected geometry 일치를 기다리지 않습니다. `FRAME_DEADLINE_MS`(4초)는 liveness 한도이며 시간 판정이 아닙니다. Spec EXIT는 Animation이 끝난 뒤 ghost를 분리하고 `OnFinished(EXIT)`를 emit하므로 F:143 `State::OnFinished`가 그때 기록한 current geometry(`State::exitEnd`)가 bounds effect의 endpoint입니다. interruption은 시작과 중단을 한 동기 action에 넣어, 그 사이에 frame이 없으므로 중단 대상이 확실히 진행 중입니다.
- `ExpectException(id)`의 실제 두 ID는 `id.DaliException`, `id.condition`입니다. 예외 메시지 exact 값은 해당 CPP 문자열과 대조하십시오.
- `scenario 접두사 + 변수` 표기는 아래 contract 부록의 **구체 ID 집합**만 뜻합니다. `std::to_string(float)` 값은 6자리 소수이며, 표의 `0.49`를 로그에서 임의의 문자열 `0.49`로 찾지 마십시오.

## 등록 / 값 / validation

| decision ID | Source 조건 | TC · scenario / action | 입력 및 독립 expected | 실제 check ID / observer | 상태 |
|---|---|---|---|---|---|
| TV01 | T:140/375 timing 기본값, T:264 effect flags·size·anchor | TC128 `TR01.defaults-values` / `run` | timing duration=.3, delay=0; flags=false, size=(1,1), anchor=(.5,.5) | `spec.default.*`, `animator.default.*`, `effect.offset.absent`, `effect.size.absent`, `effect.size.x/y`, `effect.anchor.x/y` | MAPPED_STATIC |
| TV02 | T:277–327 effect setter/clear, View children/self attachment | TC128 `TR01.defaults-values` / `run` | offset/size를 변경 후 Clear; 같은 handle을 children/self에 부착 후 각각 비움 | `clear.offset`, `clear.size`, `clear.offset.x`, `clear.size.x`, `handle.children_role`, `handle.self_role`, `clear.children_role`, `clear.self_role`, `handle.downcast`, `handle.invalid_downcast` | MAPPED_STATIC |
| TV03 | V:57/83 builtin REVERSE, BOUNCE, SIN 거부 | TC128 `TR13.registration-rejection` / `run` | CHANGE timing과 bounds effect에 3개 alpha 각각 등록 → DaliException; REVERSE와 terminal 조건 메시지 구별 | `change.alpha.{reverse,bounce,sin}.DaliException/.condition`, `bounds.alpha.{reverse,bounce,sin}.DaliException/.condition` | MAPPED_STATIC |
| TV04 | V:111 sizeFactorX/Y<0; anchor 각 축 <0 또는 >1 | TC128 `TR13.registration-rejection` / `run` | X/Y 음수 각각, anchor (-.1,.5)/(1.1,.5)/(.5,-.1)/(.5,1.1) → 정확한 거부 | `bounds.negative-size.{x,y}.DaliException/.condition`; `bounds.anchor.{0,1,2,3}.DaliException/.condition` | MAPPED_STATIC |
| TV05 | V:95/103 visual spec REVERSE/layout-owned bounds, I:219 Animator REVERSE | TC128 `TR13.registration-rejection` / `run` | Opacity+REVERSE, PositionX spec, Animator REVERSE → 각 거부; 정상 재등록 후 Measure 80×40 | `visual.reverse.*`, `visual.bounds.*`, `animator.reverse.*`, `registration.valid-after-rejection`, `size(reuse.measure)` | MAPPED_STATIC |
| TV06 | V:111 finite guard 누락 여부를 탐지하는 robustness 요구 | TC128 `TR13.nonfinite-robustness` / `run` | NaN/+Inf × sizeX,sizeY,anchorX,anchorY,offsetX,duration(bounds effect) 및 CHANGE timing duration/delay, animator timing duration/delay의 20개 입력은 등록 거부되어야 함 | `robustness.nonfinite.{nan,positive-infinity}.{size-x,size-y,anchor-x,anchor-y,offset-x,duration,change-duration,change-delay,animator-duration,animator-delay}.rejected` 20행 | MAPPED_STATIC; 현 코드 수용을 expected로 바꾸지 않음 |
| TV07 | B:35/42/48/76 SlideFrom/SlideTo, 4 edge, 기본 SELF_FRACTION=1 | TC128 `TR05.factory-values` / `run` | TOP/BOTTOM/LEFT/RIGHT offset=(0,-1)/(0,1)/(-1,0)/(1,0); 두 alias가 같은 descriptor | `factory.slide.e{0.3}.{from,to}.{x,y,present,unit}` | MAPPED_STATIC |
| TV08 | B:85/110 ExpandFrom/ShrinkTo, 4 anchor | TC128 `TR05.factory-values` / `run` | T/B: size=(1,0), anchor=(.5,0/1); L/R: size=(0,1), anchor=(0/1,.5) | `factory.size.e{0.3}.{expand,shrink}.{x,y,anchor.x,anchor.y}` | MAPPED_STATIC |
| TV09 | D:1607 재사용 spec의 apply-time validation | TC137 `TR13.apply-time-revalidation` / `mutate-registered-spec`, `recovery` | 정상 Opacity spec 등록 후 같은 handle에 SizeWidth 추가 → 거부/start0; clear 후 target=(42,30,80,40) | `apply.bounds-rejection.*`, `rejected.no-start`, `rect(recovery.target)` | MAPPED_STATIC |

TV06은 문서화한 정상 입력 계약과 구분한 robustness assertion입니다. 해당 guard는 현재 source에 모두 존재합니다: bounds effect는 `AbortIfInvalidBoundsEffect`가 범위 검사 앞에서 `LayoutBoundsEffect sizeFactor, anchor, offset and timing must be finite`로, CHANGE timing은 `AbortIfNonFiniteTiming`이 `LayoutTransitionTiming duration and delay must be finite`로, animator timing은 `AbortIfNonFiniteAnimatorTiming`이 `LayoutAnimatorTiming duration and delay must be finite`로 거부합니다(각 setter의 첫 문장, alpha 검사보다 앞). 따라서 이 20행은 모두 PASS여야 하며 어느 한 행이라도 실패하면 **회귀**입니다. 예상된 실패로 취급하거나 xfail·행 삭제로 PASS 처리하지 마십시오.

## 초기 진입 / CHANGE 원인 / owner

| decision ID | Source 조건 | TC · scenario / action | 입력 및 독립 expected | 실제 check ID / observer | 상태 |
|---|---|---|---|---|---|
| TD01 | D:634/841 first arrange 및 `wasInitialMount && !optIn` | TC129 `TR02.mount-{0,1,2,3}` / `mount`, `settled` | mode<2 Spec, mode>=2 Animator; odd mode opt-in. lifecycle start/finish=odd?1:0; geometry=(40,30,80,40) | `rect(mount.target)`, `rect(initial.rendered)`, `initial.started`, `initial.finished` | MAPPED_STATIC |
| TD02 | D:1733 SettleInitialEnter의 Spec bake / Animator skip | 위 TC129 / `settled` | initial Opacity=0; Spec은 suppress 여부와 무관 최종 1, Animator-only는 0 | `initial.opacity` | MAPPED_STATIC |
| TD03 | D:1062 runtime ENTER vs initial marker, stale initial cleanup | 위 TC129 / `runtime-add`; `TR02.attach-after-add` / `attach-and-invalidate` | 새 sibling=(140,30,60,20), start/finish odd?2:1; 이미 settled한 child에 뒤늦은 transition 부착은 ENTER0 | `rect(runtime.rendered)`, `runtime.started/finished`, `runtime.capture.overflow`; `late.attach.enter/finish`, `rect(late.attach.geometry)` | MAPPED_STATIC |
| TD04 | D:335 ResolveChangeCause의 5개 return | TC130 `TR03.cause-{0,1,2,3,4}` / `change` | kind0 add→ADDED,to=(0,20,120,40);1 remove→REMOVED,to=(0,0,120,40);2 reorder→REORDERED,to=(0,0,120,40);3 resize→WINDOW_RESIZED;4 width only→OTHER. from=(0,20,80,40) | `cause.context.present`, `context.slot=CHANGE`, `context.cause`, `rect(context.from/to)`, `cause.first.raw=0`, `cause.capture.complete` | MAPPED_STATIC |
| TD05 | D:346 cause precedence REORDERED 우선 | TC130 `TR03.cause-6` / `change` | add+remove+raise 동시; REORDERED, target=(0,120,120,40) | 위 TD04의 `context.cause`, `rect(context.to)` | MAPPED_STATIC; 다른 동시 조합은 TG03 참조 |
| TD06 | D:1142/1479 WINDOW_RESIZED && !opt-in | TC130 `TR03.cause-5` / `change` | resize opt-out; target=(0,20,120,40), callback/start/finish0 | `resize.opt-out.no-context/start/finish/capture`, `rect(resize.opt-out.target/logical)` | MAPPED_STATIC |
| TD07 | D:105/1136 bounds epsilon .5 | TC130 `TR14.threshold-{0.490000,0.500000,0.510000}` / `threshold` | x=40+delta를 항상 layout에 반영; CHANGE start=delta>.5 | `rect(threshold.layout)`, `threshold.start` | MAPPED_STATIC; y/width/height 경계는 TG04 |
| TD08 | I:194 override→default→disabled; D:1509 cancel-and-snap | TC130 `TR01.timing-enable-override` / `default-disabled`, `override-and-clear` | ClearChangeTiming 후 x80/start0; 한 action에서 OTHER override .6s로 x120/start1, 진행 중 override clear 후 x160/old finish0 | `rect(disabled.target)`, `disabled.start`, `override.start`, `rect(cleared.target)`, `cancel.no-finish` | MAPPED_STATIC |
| TD09 | D:1524 CHANGE duration<=0 ignores delay | 위 TC130 / `override-and-clear` | duration0, delay10, x200은 즉시 200 | `zero-duration.ignore-delay` | MAPPED_STATIC |
| TD10 | D:603 capture recurse / closest children owner boundary | TC131 `TR04.scope-{0,1,2,3,4,6}` / `change-grandchild` | nested parent(200,100), child x10→40. mode0 DIRECT 없음;1 SUBTREE ancestor1;2 ISOLATE 없음;3 PASS_THROUGH ancestor1;4 closest secondary1;6 STANDALONE 없음 | `owner.ancestor.calls`, `owner.closest-or-self.calls`, `rect(owner.logical.target)=(40,15,80,40)` | MAPPED_STATIC |
| TD11 | R ResolveGoverningTransition level0/SELF, D:662/1348 self capture/dispatch | TC131 `TR04.scope-{5,7}` / `change-grandchild` | self animator mode5 secondary1; child PASS_THROUGH mode7 secondary0; ancestor0 | `owner.closest-or-self.calls`, `owner.ancestor.calls`, `rect(owner.logical.target)` | MAPPED_STATIC; structural SELF slots는 TG02 |
| TD12 | D:662 no View parent → no self frame | TC131 `TR04.self-root-inert` / `self-root` | 실제 extra Window root, Self transition, width280 → bounds=(0,0,280,200), start0 | `rect(self.root.target)`, `self.root.start` | MAPPED_STATIC |
| TD13 | R inherited owner / D:1246 actual direct-parent frame | TC131 `TR04.inherited-enter-exit-parent-units` / `inherited-enter`, `inherited-exit`, `inherited-exit-finish` | owner root vs direct parent nested(200×100), child(10,15,80,40), parent-fraction(.5,.5). EXIT endpoint=(110,65,80,40)(root extent라면 (160,115)) | `inherited.enter.start`, `inherited.ghost.logical=0`, `inherited.ghost.direct-parent`, `inherited.exit.start`, `rect(inherited.exit.end)` | MAPPED_STATIC |
| TD14 | D:2718 normal inherited completion / direct-parent unparent | 위 TC131 / `inherited-enter-finish`, `inherited-exit-finish` | ENTER final=(10,15,80,40), start/finish1; EXIT start/finish1, child detached | `rect(inherited.enter.final)`, `inherited.enter.start/finish`, `inherited.exit.start/finish/unparented` | MAPPED_STATIC |

`default-disabled`라는 action 이름은 생성자의 기본값을 뜻하지 않습니다. 이 action은 명시적으로 `ClearChangeTiming()`을 호출합니다. I:45 생성자의 `mChangeTimingEnabled=true`는 별도 사실이며, action 이름만으로 기본 disabled라는 결론을 내리지 마십시오.

## Bounds / clip / native Spec

TC132의 기본 33개 scenario는 HUD View tree 밖의 같은 Window 최상위 root를 사용하며, 모두 `mount-parent`, `enter`, `enter-natural-finish`, `exit`, `exit-natural-finish` 순서입니다. B는 최종 base, E는 변형 endpoint입니다. ENTER는 자연 완료 뒤 current geometry가 B여야 하고(`rect(enter.final)`), EXIT는 `OnFinished(EXIT)` 시점의 current geometry가 E여야 합니다(`rect(exit.endpoint)`). 진행 중 geometry는 표본화하지 않습니다. 실제 current 값을 검사하며 logical target만으로 native frame 성공을 대신하지 않습니다.

| decision ID | Source 조건 | TC · scenario / action | 입력 및 독립 expected | 실제 check ID / observer | 상태 |
|---|---|---|---|---|---|
| TB01 | D:204 PIXEL/SELF_FRACTION/PARENT_FRACTION, x/y 축 | TC132 `TR05.slide-e{0.3}-u{0.2}-s{-1,1}` / start·natural finish·EXIT endpoint | B=(40,30,80,40), parent300×200. PIXEL magnitude17, SELF .25×(80 또는 40), PARENT .25×(300 또는 200). TOP/LEFT 음수, BOTTOM/RIGHT 양수; s=-1이면 반전 | `rect(enter.final)`, `rect(exit.endpoint)`; 총 24개의 구체 scenario | MAPPED_STATIC |
| TB02 | D:221 hasSizeFactor, anchor 보정; B:85 4 edge | TC132 `TR05.expand-e{0.3}` / start·natural finish·EXIT endpoint | E는 TOP(40,30,80,0), BOTTOM(40,70,80,0), LEFT(40,30,0,40), RIGHT(120,30,0,40) | `rect(enter.final)`, `rect(exit.endpoint)` | MAPPED_STATIC |
| TB03 | D:221 factor <1/=1/>1 및 각축 독립 | TC132 `TR05.factor-{0.500000,1.000000,1.500000}` / start·natural finish·EXIT endpoint | E=(40+(80−80f)/2,40,80f,20), y factor=.5 고정이므로 f=1도 timed path | `rect(enter.final)`, `rect(exit.endpoint)` | MAPPED_STATIC |
| TB04 | D:221 size→offset 합성, offset SELF 기준은 base | TC132 `TR05.mixed-units-anchor` / start·natural finish·EXIT endpoint | size(.5,1.5), anchor(1,.5), offset(parentX*.1,selfY*−.5) → E=(110,0,40,60) | `rect(enter.final)`, `rect(exit.endpoint)` | MAPPED_STATIC |
| TB05 | D:444 VisualBoundsOf의 post-RTL 좌표 / 물리 edge | TC132 `TR14.physical-left-rtl` / mount·natural finish·EXIT endpoint | parent300, requestedx40,width80 → B.x180; physical LEFT E.x100. sibling anchor.x240 | `rect(anchor.target)`, `rect(enter.final)`, `rect(exit.endpoint)` | MAPPED_STATIC |
| TB06 | D:1607/2390/2718 ENTER vs EXIT 및 완료순서 | TC132 전체 / `enter`, `enter-natural-finish`, `exit`, `exit-natural-finish` | ENTER 시작 start1, 최종 B, start/finish1; EXIT 시작 시 logical count1/Actor parent 유지/start1, 종료 시 endpoint E·detached 및 finish callback parent empty | `enter.start.once/finish.once`, `rect(enter.final)`, `exit.logical.count`, `exit.actor.parent`, `rect(exit.endpoint)`, `exit.unparented/start.once/finish.once/finish.parent.empty` | MAPPED_STATIC |
| TB07 | D:246 max(bounds delay+duration,visual duration) | TC133 `TR06.clip-{0,1,2}-{size,offset}` / `start-composite`, `natural-finish` | bounds .2s + delay.1 vs opacity .5s → composite는 더 긴 .5s track까지; opacity0→1(완료 시 1), final=(40,30,80,40) | `composite.start.once/finish.pending`, `rect(composite.final.bounds)`, `composite.final.opacity`, `composite.finish.once/start.still.once` | MAPPED_STATIC; 역 duration 관계는 TG05 |
| TB08 | D:291 AUTO && timedSize, forced && timedEffect, DISABLED | 위 TC133 / `start-composite`, `natural-finish` | clip0=AUTO: size만 clip; clip1=DISABLED: 없음; clip2=CLIP_TO_BOUNDING_BOX: 둘 다 clip. 원래 DISABLED로 복구 | `composite.transient.clip`, `composite.clip.restored` | MAPPED_STATIC; 다른 원래 clip 값·취소 복구는 TG05 |
| TB09 | V:127/140/154 noop/untimed, D:1702/2535 즉시경로 | TC133 `TR06.noop-and-instant` / `instant-enter` | ENTER size0,duration0,delay10 → 최종 base 즉시/start0/finish0; empty visual+identity EXIT→즉시 unparent/start0 | `rect(instant.enter.endpoint)`, `instant.enter.start/finish`, `empty.exit.immediate/start` | MAPPED_STATIC |
| TB10 | D:1136 equal-target skip와 ViewDataImpl::ReplayArrangeSubtreeFromCache / ApplySelfBoundsIfChanged의 exact `!=` write 억제 | TC132 `TR14.ancestor-replay-during-enter` / `enter-and-replay`, `enter-natural-finish` | HUD tree에서 spec ENTER를 시작한 같은 action에서 host Arrange invalidate+manual drain(ENTER가 확실히 진행 중). event-side y=target 30 유지, 재시작 없음; 취소는 finish 없이 정리되므로 다음 action의 finish1·base 정착이 비취소를 보여 줌 | `ancestor-replay.before.event-y`, `ancestor-replay.after.event-y`, `ancestor-replay.no-restart`, `enter.finish.once`, `rect(enter.final)` | MAPPED_STATIC; geometry write 수·cancel 수·Animation 동일성은 관측하지 않음 |

## Animator 시간 / 교체 / 수명

TC135의 기본 interruption fixture는 HUD View tree 밖 같은 Window의 `SENSITIVE=false` 최상위 root입니다. TC135는 transition의 시작과 중단을 한 동기 action에서 수행하므로 그 사이에 frame이 없고, 중단 대상은 확실히 진행 중이며 visual 상태는 시작점입니다. 진행 중(PLAYING) spec ENTER와 bounds가 같은 조상 pass의 계약은 TB10이 검사합니다. 앱이 `LayoutAnimatorCallback`으로 보간하는 transition의 같은 경우는 검사 범위 밖입니다.

| decision ID | Source 조건 | TC · scenario / action | 입력 및 독립 expected | 실제 check ID / observer | 상태 |
|---|---|---|---|---|---|
| TA01 | D:126 builtin curve branches, cubic-inout t<.5 양쪽 | TC134 `TR07.curve-{default,linear,square-in,square-out,cubic-in,cubic-out,cubic-inout,sine-in,sine-out,sine-inout}` / `first-tick`, `natural-finish` | duration.4; 수동 pass의 첫 dispatch raw0, 이후 실제 clock; 모든 표본에서 progress=f(raw), f는 CPP의 닫힌 식 | `first.count/raw/alpha`, `first.context.*`, `samples.count/raw-monotonic/alpha-mismatches/context-mismatches`, `last.raw/alpha`, `lifecycle.start/finish`, `capture.complete` | MAPPED_STATIC |
| TA02 | D:126 builtin 미지원 / BEZIER·SPRING default | TC134 `TR07.curve-{bounce-fallback,sin-fallback,back-fallback,bezier-fallback,spring-fallback,custom-spring-fallback}` / `first-tick`, `natural-finish` | callback alpha는 linear fallback f(t)=t | `samples.alpha-mismatches`, 나머지 TA01 check | MAPPED_STATIC; native Spec alpha와 동일하다고 주장하지 않음 |
| TA03 | D:126 CUSTOM_FUNCTION 반환값 unclamped | TC134 `TR07.curve-custom-nonterminal` / `first-tick`, `natural-finish` | 함수 2−.5t → first.alpha 2, last.alpha 1.5 | `first.alpha`, `samples.alpha-mismatches`, `last.alpha` 및 context/lifecycle | MAPPED_STATIC |
| TA04 | D:2077 elapsed−delay clamp; D:2252 first tick delta0; D:2237 delta cap=.1 | TC134 `TR07.delay-cap-restart` / `delay-and-cap` | duration=.2,delay=.2, 실제 clock; first raw0, raw 단조, 표본 간 Δraw ≤ .1/.2(cap, timing 무관), last raw1, finish1, finish 뒤 context 없음 | `first.raw`, `raw-monotonic`, `raw-step-bound`, `last.raw`, `finish.once`, `drained.no-late-context` | MAPPED_STATIC |
| TA05 | D:2328/2353 tick driver restart와 actual timer 완료 | 위 TC134 / `restore-natural-timer` | drained 뒤 새 .15s CHANGE, first raw0; 실제 clock으로 자연 완료(event 대기) → finish1, last raw1 | `restart.first.raw`, `restart.natural.finish`, `restart.natural.raw-one` | MAPPED_STATIC; wake/allocation 수는 별도, 자체 timer 재시작 여부는 frame 대기와 구분되지 않음 |
| TA06 | D:2089 duration>0 ternary와 D:2290 atEnd | TC134 `TR07.nonpositive-{0.000000,-0.100000}` / `instant-tick` | delay10이어도 수동 pass의 첫 tick에서 callback1/raw1/start1/finish1 | `instant.callbacks/raw/start/finish` | MAPPED_STATIC |
| TA07 | D:1496/1869 기존 Spec/Animator 선택 후 from 보존 | TC135 `TR08.same-slot-{0,1,2,3}` / `replace` | mode bit0=이전Animator,bit1=새Animator. mount는 render settle까지 대기(current x40 확정). 같은 action에서 40→140 시작 직후 240으로 교체(frame 없음) → 새 Animator는 from40,to240, 새 Spec은 Play target240; start2, oldfinish0 | `first.start`, `rect(replacement.from/to)` 또는 `rect(replacement.target)`, `replacement.start`, `cancelled.finish.silent` | MAPPED_STATIC |
| TA08 | D:1790/1852 cancel silent, D:2144/2718 정상만 finish | 위 TC135 / `complete-replacement` | 새 transition만 finish1(event 대기), child parent 유지, Animator는 last raw1·Spec은 current (240,30,80,40) | `normal.finish.only-new`, `replacement.not-exit`, `replacement.last-raw` 또는 `rect(replacement.final)` | MAPPED_STATIC |
| TA09 | D:502/1990 CurrentVisualBoundsForExit, cross-slot | TC135 `TR08.change-to-exit`, `TR08.enter-to-exit` / `cross-slot`, `exit-finish` | Animator CHANGE 또는 ENTER를 시작한 같은 action에서 EXIT(진행 중 lastLerped x40에서 시작, CHANGE의 arranged x140 아님); oldfinish0, EXIT start1/finish1(event 대기), detached | `rect(exit.continuity)`, `exit.slot`, `old.finish.silent/remains-silent`, `exit.start.once/finish.once/detached` | MAPPED_STATIC; Spec ENTER의 visual settle/cancel은 TG06 |
| TA10 | D:2390 repeated EXIT guard / View logical-child exclusion | TC136 `TR09.ghost-{0,1,2,3}` / `start-and-mutate` | Animator EXIT(시작과 변이를 한 action에서); logical0,Actor1,parent유지, interaction3속성false. 반복Remove/Add-same/Reparent/RemoveAll에 start는 1 유지 | `ghost.logical.count/actor.count/parent.retained/start.once/no-duplicate-start/cancel.no-finish/expected-parent/logical-after-action`, `ghost.interaction.disabled.{sensitive,focusable,touch}` | MAPPED_STATIC |
| TA11 | D:1852/2144/2592 EXIT 취소·복구·다른 parent 보호 | 위 TC136 / `finish-or-cancel` | operation2 reparent만 finish0/newparent유지; 나머지 finish1/detached(EXIT 완료 event 뒤); 세 interaction true복구 | `ghost.finish/final-parent`, `ghost.interaction.restored.{sensitive,focusable,touch}` | MAPPED_STATIC |
| TA12 | D:409/2049 focus clear 후 synchronous reparent | TC136 `TR10.focus-reentrant-reparent` / `focused-remove` | 실제 focus child→clear listener에서 other로reparent; focused child 아님, 취소 start0/finish0, flags복구 | `focus.request/actual/listener.reparent/removed-view.not-focused/cancel.no-start/cancel.no-finish/restore.focusable/restore.touch` | MAPPED_STATIC |
| TA13 | D:2144 restore-before-Remove 및 listener변경 보존 | TC136 `TR10.restore-before-remove-listener` / `remove`, `listener-result` | remove listener 관측bitmask7; listener가false설정후 finish에서 0, finalfalse3속성, finish1/detached | `listener.exit.start/restored-before-remove/changes-visible-at-finish/parent.empty/finish.once`, `listener.change-preserved.{sensitive,focusable,touch}` | MAPPED_STATIC |
| TA14 | D:186 EmitLifecycle / D:2610 scene detach cancel | TC137 `TR11.lifecycle-{0.5}` / `start-and-mutate`, `observe-natural-completion` | OnStart remove(0),OnFinish add(1),handle replace(2),rootunparent(3),childPASS_THROUGH(4),handleclear(5). 취소 0/3만 finish0; 나머지finish1,start1 | `lifecycle.started/normal-vs-cancelled/no-second-start/target-parent/finish-added-child` | MAPPED_STATIC |
| TA15 | D:881/1172 mode gate와 pending-enter 소비 | TC137 `TR12.mode-return-no-retro-enter` / `add-suppressed`, `fresh-add` | PASS_THROUGH 중 add후AUTO복귀+invalidate는ENTER0; 별도의 fresh add는 start1/finish1 | `mode.no-retro-enter/no-retro-finish/handle-kept`, `fresh.enter.once/finish.once` | MAPPED_STATIC |
| TA16 | D:688 pin/동기수명변경, D:869 ChildStillPresent | TC137 `TR11.remove-later-target-on-start` / `remove-later`, `live-target-finish` | 두 target40→60,140→160; 첫 OnStart가 뒤 target 제거 → live start1/finish1, 최종(60,30,80,40) | `later.listener.executed/target.removed/only-live-start/only-live-finish`, `rect(later.live.final)` | MAPPED_STATIC; last-handle destruction은 TG08 |

## Controller root / batch / completion / rollback

| decision ID | Source 조건 | TC · scenario / action | 입력 및 독립 expected | 실제 check ID / observer | 상태 |
|---|---|---|---|---|---|
| TC01 | C:1407 axis별 requested>=0 우선, parent유무, parent0 | TC138 `CT01.constraint-{0.3}` / `run` | Window320×240,requestedpos11/13,margin3/7/5/11. mode0fixed300×200→constraint290×184,producer80×40;1 parent250×180→240×164;2noParent→실제 window 크기 기준 (W−10)×(H−16), 요청 320×240이 그대로 실현되면 310×224;3parent0→0×0 | `size(root.producer.constraint)`, `rect(root.bounds)` x14,y18,크기는mode별; `root.producer.called`, `root.parent.branch` | MAPPED_STATIC |
| TC02 | C:781 actual Window resize→all roots invalidate | TC138 `CT01.real-window-resize` / `create`,`resize` | MATCH_PARENT Window320×240→400×300; root(0,0,W,H) | `rect(window.initial.root)`, `rect(window.resized.root)` 해당 extraWindow fence | MAPPED_STATIC |
| TC03 | C:616 pending set dedup, C:1222 depth sort ancestor-first | TC139 `CT02.depth-and-dedup` / `queue-reverse-order` | standalone child→root→child 중복request 뒤 수동 pass 한 번; child logical=(40,30,80,40) | `rect(batch.child.target)` | MAPPED_STATIC; 처리 순서·중복 제거는 내부 순서라 관측하지 않음(부모 pass가 standalone child도 배치) |
| TC04 | C:704 UnregisterView의 pending/allroots 제거 | TC139 `CT02.deleted-pending-root` / `unregister` | offscene synthetic root request뒤 unregister→다음 pass에서 producer0 | `unregister.no-producer` | MAPPED_STATIC; pending 수는 관측하지 않음, 실제weak-expiry branch는 TG09 |
| TC05 | C:866 pre 계산 / post emit / View first Window last | TC140 `CT03.view-before-window` / `mount-observed-tree` | child(40,30,80,40),sibling(150,30,60,20); View ID2/3 후Window marker100,Window1회 | `rect(completion.child.target/sibling.target)`, `completion.window.last/children.present/window.once` | MAPPED_STATIC |
| TC06 | C:835 manual flag 및 C:866 deferred emit | 위 TC140 / `manual-process-defers-emit`,`noop-no-new-signal` | manual Process 즉시신호 0변화,추후fence+1,targetx60; no-op manual은새신호강요없음 | `manual.no-synchronous-emit`, `manual.eventual.fence`, `rect(manual.latest.target)`, `noop.immediate.signal-count`, `noop.geometry.x`, `noop.settled.signal-count`, `noop.settled.window-count`(200ms 뒤에도 View/Window 완료 수 불변) | MAPPED_STATIC |
| TC07 | C:1052 latest-wins index overwrite | TC140 `CT03.latest-and-unsubscribe` / `two-passes-one-delivery` | x70후 90 두manualpass,동기delivery0,최종x90 한회 | `coalesced.no-sync-delivery`, `rect(coalesced.latest)`, `coalesced.view.once` | MAPPED_STATIC; emit 중 b1 stale-skip은 TG10 |
| TC08 | C:1100 connection check false | 위 TC140 / `unsubscribe-before-post` | targetx100queued후DisconnectAll,delay100ms→기존listener증가 0,currentx100 | `unsubscribe.silent`, `unsubscribe.geometry` | MAPPED_STATIC |
| TC09 | C:528/568 detached guard, C:ProcessDepthScope deferred destroy | TC141 `CT04.remove-in-completion` / `remove-during-fence`,`recreate-and-process` | oldWindowfence중Remove→old1회(event 대기),Windowalive;새Get/controller에width280→newtarget,oldlistener증가 0 | `lifetime.old-fence.once/window.alive/old.listener.inert`, `rect(lifetime.new.target)` | MAPPED_STATIC |
| TC10 | C:1510 Window-key isolation / Remove idempotence | TC141 `CT04.window-isolation` / `mount-other`,`close-other-then-main-change` | secondWindow200×160에root160×120;otherRemove2회후mainchildx55정상 | `rect(second.window.root)`, `windows.distinct`, `rect(surviving.window.target)` | MAPPED_STATIC |
| TC11 | C:ActiveLayoutFinishedScope/ProcessDepthScope/ManualProcessScope RAII, rollback currentroot | TC142 `CT05.measure-throws`, `CT05.arrange-throws` / `throw-and-recover`,`recovered-fence` | syntheticproducer의정확runtime_error;예외off 뒤 복구 pass에서 Measure·Arrange producer 재실행(guard 누수 없음),새target65 | `rollback.exception.caught/message`, `rollback.recovered.measure-producer/arrange-producer`, `rect(rollback.recovery.target)` | MAPPED_STATIC |
| TC12 | C:1222 nested processing과 collector frame복구 | TC142 `CT05.nested-process` / `nested-call` | Measurecallback안 ProcessLayouts 1회재진입→producer1,outertarget(40,30,80,40) | `nested.callback.executed/once`, `rect(nested.final.bounds)` | MAPPED_STATIC |
| TC13 | C:PendingBatchRollbackScope BeginTurn/remainingroots/Commit | TC142 `CT05.remaining-root-rollback` / `abort-earlier-root`,`drain-restored-batch` | ancestorproducer예외;동일action에서주입해제;다음action에서 각root invalidate후 복구child(40,30,80,40),boundary(5,7,160,120) | `remaining.exception/exception.message`, `rect(remaining.recovered.child/boundary)` | MAPPED_STATIC; 뒤쪽 root의 rollback 기록·pending·depth는 관측하지 않음 |
| TC14 | C:1222 StartTransitionsAfterLayout의lifecycle예외도rollback | TC142 `CT05.lifecycle-throws` / `throw-lifecycle`,`recover-lifecycle` | OnStart정확runtime_error→start1;clear후target100,oldfinish0 | `lifecycle.exception/exception.message/failed-start.once/cancel.silent`, `rect(lifecycle.recovered)` | MAPPED_STATIC |
| TC15 | C:LayoutPassScope, D:718/723 outermost resizeflag해제 | TC142 `CT05.resize-flag-abort` / `abort-resize-then-change` | resize640×480중Measure예외→다음ordinaryx90의causeOTHER,from40,to90,oldfinish0 | `resize-abort.exception/context/no-finish`, `context.slot/cause`, `rect(context.from/to)` | MAPPED_STATIC |

TC01은 scale=1·고정 min/max 기본값 fixture입니다. producer가 scale/margin/requested를 바꾸는 도중에도 `SnapshotStandaloneSlotInputs`의 같은 입력을 유지하는 분기는 이 네 fixture만으로 닫히지 않습니다. 다른 신규 TC의 action/assertion 연결 또는 추가 fixture가 필요합니다.

## TC155 정확성 선행 조건과 work/sample 분리

scenario는 두 계열 30개입니다. 반복 sample 계열 `TR15.mode-{0.4}-n{1,32,128}` 15개는 `mount` 뒤 `samples`를, work-count 계열 `TR15W.mode-{0.4}-n{1,32,128}` 15개는 `mount` 뒤 `work-count`를 실행합니다. `work-count`가 다시 연결하는 transition callback과 fixture 폭 토글이 반복 sample을 오염시키지 않도록 두 계열은 별도 scenario로 분리되어 있습니다. mode0=transition없음,1=부착하되무변경dormant,2=ClearChangeTiming,3=activeSpec,4=activeAnimator입니다. 신규 leaf의 독립 target은 `(0,i,width,1)`이며 identity/순서와 logical child count도 검사합니다.

| decision ID | Source 조건 | scenario 계열 / action | 독립 expected와 관측 | 실제 check ID | 상태 |
|---|---|---|---|---|---|
| TP01 | C:1222 실제Windowmount·D:556 attachment관측 | 두 계열 / `mount` | 모든N개leaf Windowfence target=(0,i,80,1),rootlogicalcount=N,handlecount=N,child[i]identity일치 | `performance.fixture.handles/logical-children`, `performance.fixture.leaf-i.identity-order`, `rect(performance.fixture.leaf-i.target)` | MAPPED_STATIC |
| TP02 | D:1496/1869 active Spec/Animator 생성 vs absent/disabled/dormant | `TR15W` / `work-count` | lifecycle callback 연결(mode4는 계수 animator); mode1 Process(no-op),나머지width81변경. 같은 action의 start 수=mode>=3?N:0, 첫 animator dispatch(raw0) 수=mode4?N:0 | `performance.transition.start-count`, `performance.first-tick-count` | MAPPED_STATIC; traversal/capacity 비용은 이 2행이 측정하지 않음 |
| TP03 | C:1407/Arrange완료 logicaltarget 보존 | `TR15W` / `work-count` | 모든leaf target=(0,i,mode1?80:81,1), count/identity일치 | `performance.work.handles/logical-children`, `performance.work.leaf-i.identity-order`, `rect(performance.work.leaf-i.target)` | MAPPED_STATIC |
| TP04 | P:CheckSample의실제 9회update vs 갱신누락 회귀 | `TR15` / `samples` | mount상태80에서시작. sample j=0.30마다 9회Update직후 targetwidth=evenj?81:80. **다음Process/Delay전**, 기존 const GetArrangedBounds로전체leaf확인 | `performance.sample-j.handles/logical-children`, `performance.sample-j.leaf-i.identity-order`, `rect(performance.sample-j.leaf-i.target)` | MAPPED_STATIC |
| TP05 | C:1222 empty-pending earlyreturn | `TR15` / mode1 `samples` | sample마다 128회no-op,31samples;모든leafwidth80유지 | TP04와같은ID family | MAPPED_STATIC |

`GeometryChecks(N)=5N+2`입니다. mount checks는 N=1/32/128에서 7/162/642, `TR15W`의 work-count는 9/164/644, `TR15`의 samples는 `31*(5N+2)`=217/5022/19902입니다(sample 수·operation 수 같은 컴파일 상수 비교 행은 제거). geometry는 각 sample의 마지막 update 직후에 읽습니다. TC155의 빈 `PerformanceAnimator`는 application callback 작업이 없는 기준 workload이며 실제 animated Actor current oracle을 대표하지 않습니다. Spec/Animator rendered progression은 TC132–135의 해당 action과 별도로 판정하십시오.

## 직접 연결이 아직 없는 분기 / 관측 한계

다음은 이 16개 TC의 **직접 assertion 공백**입니다. 다른 신규 TC가 해당 분기를 검증한다면 그 구체 action/check/evidence를 추가하여 통합하십시오. 함수가 실행되었거나 관련 TC 이름이 있다는 이유로 이 목록을 제거하지 마십시오. 아래 행은 suite 전체의 최종 누락 판정도, 지원 제외 선언도 아닙니다.

| gap ID | Source 분기/조건 | 현재 근접 fixture의 한계 | 닫기 위해 필요한 독립 관측 |
|---|---|---|---|
| TG01 | V:127 noop epsilon1e-5, 각 offset/size축; 무효 enum과 timing delay의finite/음수조합 | TC128/133은identity/일부NaN/Inf만검사; epsilon전후·각axis전체는없음 | 경계양쪽descriptor와timed active/lifecycle/geometry;무효domain은계약을명확히표시한robustnessassertion |
| TG02 | R의구조적SELF ENTER/EXIT,closestowner에요청slot없음,ownerdetach/scope변경/gate변경후pendingENTER재검증(D:1246) | TC131 scope0.7은주로CHANGE이고inheritedENTER/EXIT는정상한경로 | add와dispatch사이에각조건변경,owner/role/parentID·start0/1·실제bounds/finalparent를별도체크 |
| TG03 | D:335 동시cause우선순위의ADDED>REMOVED>RESIZE 및self/inherited원인전달 | TC130 kind6은REORDERED+ADDED+REMOVED만;resize와동시조합없음 | 각인접우선순위pair의명시input과context.cause/target;두rootresize에서도같은cause보존 |
| TG04 | D:105 epsilon y/width/height 및 VisualBoundsOf의미배치 fallback | TC130 threshold는x만;TC132는이미배치된rect중심 | 각축.49/.5/.51과미배치→첫layout의독립좌표/slot/lifecycle |
| TG05 | D:246 bounds channel이더길어Animationduration확장;D:323 clipping취소복구·기존clip값보존 | TC133은visual.5가bounds.3보다김,원래clipDISABLED·정상완료만 | bounds더긴fixture에서duration정확값/중간frame;기존다른clip값으로취소후원복 |
| TG06 | D:1790 Spec ENTER→CHANGE BAKE_FINAL / ENTER→EXIT PRESERVE_CURRENT | TC135 cross-slot은Animator이고same-slot은CHANGE; spec-enter successor는 완료 시 opacity와 CHANGE 최종 rect만 확인(진행 중 값 미표본) | 실제Spec opacity/size중간값→후속slot의독립endpoint·visualstate·oldfinish0·새finish1 |
| TG07 | D:2130 Animator callback의동기self삭제/새Animator생성, D:2280 freshentry finalize skip | TC134는정상callback,TC137은lifecyclecallback변경 | tickcallback에서교체된새entry의raw0/elapsed0/생존및후속raw1,취소/정상finish개수 |
| TG08 | D:2610 offscene inheritedghost direct-parent의last-handle파괴 및selfcaptured rawparent prune | TC136 reparent와TC137 Unparent는handle을State가보유 | weak lifetime/부모실제파괴후state배출·interaction복구·finishsilent;stale rawparent dereference없음 |
| TG09 | C:781 dead weakroot 정리, C:1222 invalid/dead batchentry,rollback중bad_alloc catch | TC139는명시Unregister이고TC142는producer/lifecycle예외 | weakexpired분기의독립trace·pending제거;allocationfailure는승인된국소faultseam과원래예외보존 |
| TG10 | C:1100 tombstone / emit도중WeakHandle만료 / nested b1 stale-skip | TC140 latest-wins는emit전 2pass이고unsubscribe만직접검사 | 앞Viewlistener가뒤View파괴·재layout하는episode,뒤view최신 1회/소멸 0회 및 Windowfence순서 |
| TG11 | C:Process의Viewemit중Remove→Windowemit억제,동일callback내Remove→Get→새controller보호 | TC141은Windowfence에서Remove후다음action에Get | 같은callback내교체후newcontroller생존·oldWindowemit0·freshroot처리evidence |
| TG12 | C:1407 SnapshotStandaloneSlotInputs의mid-Measure입력변경,axis별scale/minmax;C:766 ReplaceCurrentWindow | TC138 고정scale1·기본clamp·동일Window만 | 입력snapshot전후의독립constraint/slot·해당rootownership,교체된Window의root/fence정합 |
| TG13 | D:373/2328/2353 실제timer수명·driver wake·capacity 및C:1146 no-self-wake | TC134 자연완료·TC155 start/첫tick 수만으로 전체wake/storage량은확정못함 | 다른 신규 PARK/work-count TC의 정확 예산과 passive idle 증거 연결 |
| TG14 | null/empty handle·missing actor·no callback 방어 return,동일handle양role 동시부착 | 실제fixture에없는모든early-return을정상경로로커버했다고할수없음 | 도달가능조건별명시fixture/observer,정확무동작/복구assertion;도달불가판정은근거검토필요 |

## 정적 contract 대조

아래는 검토용 metadata와 CPP의 scenario 생성식을 대조한 체크리스트입니다. TC132의 `TR14.ancestor-replay-during-enter`는 초기 metadata 수집 뒤 추가된 source delta이며 아래 합계/행에는 포함했습니다. 최종 실행 전 metadata/manifest를 다시 추출·검토해야 합니다. metadata는 실행 성공의 증거가 아닙니다. 실행 로그에서 계약을 축소해도 통과하지 않도록 최종 manifest는 별도로 검토하고 외부 SHA pin을 고정하십시오. 이 문서는 manifest를 수정하지 않습니다.

표의 수치는 단일 계약(`layout-validation-manifest.json`, SHA-256 `3b90f79fc2cb651ce3a818374ed13584e2d677307f12d6de8199a1cc3ff79706`)에서 생성했습니다. manifest에는 `release` profile 하나만 있습니다.

| TC | scenario 수 | action 수 | required checks 합계 | 코드 |
|---|---:|---:|---:|---|
| TC128 | 4 | 4 | 139 | [tc-128-layout-transition-config-validation.cpp](tc/tc-128-layout-transition-config-validation.cpp) |
| TC129 | 5 | 14 | 82 | [tc-129-layout-transition-initial-mount.cpp](tc/tc-129-layout-transition-initial-mount.cpp) |
| TC130 | 11 | 23 | 161 | [tc-130-layout-transition-change-causes.cpp](tc/tc-130-layout-transition-change-causes.cpp) |
| TC131 | 10 | 22 | 106 | [tc-131-layout-transition-scope.cpp](tc/tc-131-layout-transition-scope.cpp) |
| TC132 | 34 | 170 | 751 | [tc-132-layout-transition-bounds.cpp](tc/tc-132-layout-transition-bounds.cpp) |
| TC133 | 7 | 20 | 102 | [tc-133-layout-transition-composite-clip.cpp](tc/tc-133-layout-transition-composite-clip.cpp) |
| TC134 | 20 | 58 | 471 | [tc-134-layout-transition-animator-clock.cpp](tc/tc-134-layout-transition-animator-clock.cpp) |
| TC135 | 8 | 24 | 126 | [tc-135-layout-transition-interruption.cpp](tc/tc-135-layout-transition-interruption.cpp) |
| TC136 | 6 | 17 | 104 | [tc-136-layout-transition-exit-focus.cpp](tc/tc-136-layout-transition-exit-focus.cpp) |
| TC137 | 13 | 39 | 149 | [tc-137-layout-transition-lifecycle.cpp](tc/tc-137-layout-transition-lifecycle.cpp) |
| TC138 | 5 | 6 | 40 | [tc-138-layout-controller-root-constraints.cpp](tc/tc-138-layout-controller-root-constraints.cpp) |
| TC139 | 2 | 3 | 9 | [tc-139-layout-controller-batch-order.cpp](tc/tc-139-layout-controller-batch-order.cpp) |
| TC140 | 2 | 6 | 33 | [tc-140-layout-controller-completion.cpp](tc/tc-140-layout-controller-completion.cpp) |
| TC141 | 2 | 6 | 24 | [tc-141-layout-controller-lifetime.cpp](tc/tc-141-layout-controller-lifetime.cpp) |
| TC142 | 6 | 16 | 81 | [tc-142-layout-controller-rollback.cpp](tc/tc-142-layout-controller-rollback.cpp) |
| TC155 | 30 | 60 | 137900 | [tc-155-layout-transition-performance.cpp](tc/tc-155-layout-transition-performance.cpp) |

다음 표의 `action:required_checks`는 컴파일된 단일 계약입니다. TC155는 반복 sample 계열과 work-count 계열이 모두 이 표에 있습니다.

| TC | 구체 scenario ID | action:required_checks (실행 순서) |
|---|---|---|
| TC128 | `TR01.defaults-values` | `run:20` |
| TC128 | `TR13.registration-rejection` | `run:35` |
| TC128 | `TR05.factory-values` | `run:64` |
| TC128 | `TR13.nonfinite-robustness` | `run:20` |
| TC129 | `TR02.mount-0` | `mount:4` → `settled:7` → `runtime-add:7` |
| TC129 | `TR02.mount-1` | `mount:4` → `settled:7` → `runtime-add:7` |
| TC129 | `TR02.mount-2` | `mount:4` → `settled:7` → `runtime-add:7` |
| TC129 | `TR02.mount-3` | `mount:4` → `settled:7` → `runtime-add:7` |
| TC129 | `TR02.attach-after-add` | `mount:4` → `attach-and-invalidate:6` |
| TC130 | `TR03.cause-0` | `mount:4` → `change:13` |
| TC130 | `TR03.cause-1` | `mount:4` → `change:13` |
| TC130 | `TR03.cause-2` | `mount:4` → `change:13` |
| TC130 | `TR03.cause-3` | `mount:4` → `change:13` |
| TC130 | `TR03.cause-4` | `mount:4` → `change:13` |
| TC130 | `TR03.cause-5` | `mount:4` → `change:12` |
| TC130 | `TR03.cause-6` | `mount:4` → `change:13` |
| TC130 | `TR14.threshold-0.490000` | `mount:4` → `threshold:5` |
| TC130 | `TR14.threshold-0.500000` | `mount:4` → `threshold:5` |
| TC130 | `TR14.threshold-0.510000` | `mount:4` → `threshold:5` |
| TC130 | `TR01.timing-enable-override` | `mount:4` → `default-disabled:5` → `override-and-clear:7` |
| TC131 | `TR04.scope-0` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-1` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-2` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-3` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-4` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-5` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-6` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.scope-7` | `mount:4` → `change-grandchild:6` |
| TC131 | `TR04.self-root-inert` | `self-root:5` |
| TC131 | `TR04.inherited-enter-exit-parent-units` | `mount-inherited:4` → `inherited-enter:1` → `inherited-enter-finish:6` → `inherited-exit:3` → `inherited-exit-finish:7` |
| TC132 | `TR05.slide-e0-u0-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e0-u0-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e0-u1-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e0-u1-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e0-u2-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e0-u2-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e1-u0-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e1-u0-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e1-u1-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e1-u1-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e1-u2-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e1-u2-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e2-u0-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e2-u0-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e2-u1-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e2-u1-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e2-u2-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e2-u2-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e3-u0-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e3-u0-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e3-u1-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e3-u1-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e3-u2-s-1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.slide-e3-u2-s1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.expand-e0` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.expand-e1` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.expand-e2` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.expand-e3` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.factor-0.500000` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.factor-1.000000` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.factor-1.500000` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR05.mixed-units-anchor` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR14.physical-left-rtl` | `mount-parent:4` → `enter:1` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC132 | `TR14.ancestor-replay-during-enter` | `mount-parent:4` → `enter-and-replay:4` → `enter-natural-finish:6` → `exit:3` → `exit-natural-finish:8` |
| TC133 | `TR06.clip-0-offset` | `mount:4` → `start-composite:3` → `natural-finish:8` |
| TC133 | `TR06.clip-0-size` | `mount:4` → `start-composite:3` → `natural-finish:8` |
| TC133 | `TR06.clip-1-offset` | `mount:4` → `start-composite:3` → `natural-finish:8` |
| TC133 | `TR06.clip-1-size` | `mount:4` → `start-composite:3` → `natural-finish:8` |
| TC133 | `TR06.clip-2-offset` | `mount:4` → `start-composite:3` → `natural-finish:8` |
| TC133 | `TR06.clip-2-size` | `mount:4` → `start-composite:3` → `natural-finish:8` |
| TC133 | `TR06.noop-and-instant` | `mount:4` → `instant-enter:8` |
| TC134 | `TR07.curve-default` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-linear` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-square-in` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-square-out` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-cubic-in` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-cubic-out` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-cubic-inout` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-sine-in` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-sine-out` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-sine-inout` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-bounce-fallback` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-sin-fallback` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-back-fallback` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-bezier-fallback` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-spring-fallback` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-custom-spring-fallback` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.curve-custom-nonterminal` | `mount:4` → `first-tick:13` → `natural-finish:9` |
| TC134 | `TR07.delay-cap-restart` | `mount:4` → `delay-and-cap:6` → `restore-natural-timer:3` |
| TC134 | `TR07.nonpositive-0.000000` | `mount:4` → `instant-tick:4` |
| TC134 | `TR07.nonpositive--0.100000` | `mount:4` → `instant-tick:4` |
| TC135 | `TR08.same-slot-0` | `mount:4` → `replace:7` → `complete-replacement:6` |
| TC135 | `TR08.same-slot-1` | `mount:4` → `replace:7` → `complete-replacement:6` |
| TC135 | `TR08.same-slot-2` | `mount:4` → `replace:11` → `complete-replacement:3` |
| TC135 | `TR08.same-slot-3` | `mount:4` → `replace:11` → `complete-replacement:3` |
| TC135 | `TR08.change-to-exit` | `mount:4` → `cross-slot:7` → `exit-finish:3` |
| TC135 | `TR08.enter-to-exit` | `mount:4` → `cross-slot:7` → `exit-finish:3` |
| TC135 | `TR08.spec-enter-to-change` | `mount:4` → `enter-and-successor:4` → `successor-finish:8` |
| TC135 | `TR08.spec-enter-to-exit` | `mount:4` → `enter-and-successor:4` → `successor-finish:4` |
| TC136 | `TR09.ghost-0` | `mount:4` → `start-and-mutate:11` → `finish-or-cancel:5` |
| TC136 | `TR09.ghost-1` | `mount:4` → `start-and-mutate:11` → `finish-or-cancel:5` |
| TC136 | `TR09.ghost-2` | `mount:4` → `start-and-mutate:11` → `finish-or-cancel:5` |
| TC136 | `TR09.ghost-3` | `mount:4` → `start-and-mutate:11` → `finish-or-cancel:5` |
| TC136 | `TR10.focus-reentrant-reparent` | `mount:4` → `focused-remove:8` |
| TC136 | `TR10.restore-before-remove-listener` | `mount:4` → `remove:1` → `listener-result:7` |
| TC137 | `TR11.lifecycle-0` | `mount:4` → `start-and-mutate:1` → `observe-natural-completion:4` |
| TC137 | `TR11.lifecycle-1` | `mount:4` → `start-and-mutate:1` → `observe-natural-completion:4` |
| TC137 | `TR11.lifecycle-2` | `mount:4` → `start-and-mutate:1` → `observe-natural-completion:4` |
| TC137 | `TR11.lifecycle-3` | `mount:4` → `start-and-mutate:1` → `observe-natural-completion:4` |
| TC137 | `TR11.lifecycle-4` | `mount:4` → `start-and-mutate:1` → `observe-natural-completion:4` |
| TC137 | `TR11.lifecycle-5` | `mount:4` → `start-and-mutate:1` → `observe-natural-completion:4` |
| TC137 | `TR12.mode-return-no-retro-enter` | `mount:4` → `add-suppressed:3` → `fresh-add:2` |
| TC137 | `TR13.apply-time-revalidation` | `mount:4` → `mutate-registered-spec:3` → `recovery:4` |
| TC137 | `TR11.remove-later-target-on-start` | `mount-two-targets:8` → `remove-later:3` → `live-target-finish:5` |
| TC137 | `TR11.tick-removes-self` | `mount:4` → `mutating-tick:2` → `drain:6` |
| TC137 | `TR11.tick-replaces-with-exit` | `mount:4` → `mutating-tick:2` → `drain:7` |
| TC137 | `TR12.parent-last-handle-spec` | `mount:4` → `destroy-parent:10` → `late-finish-silent:3` |
| TC137 | `TR12.parent-last-handle-animator` | `mount:4` → `destroy-parent:10` → `late-finish-silent:3` |
| TC138 | `CT01.constraint-0` | `run:8` |
| TC138 | `CT01.constraint-1` | `run:8` |
| TC138 | `CT01.constraint-2` | `run:8` |
| TC138 | `CT01.constraint-3` | `run:8` |
| TC138 | `CT01.real-window-resize` | `create:4` → `resize:4` |
| TC139 | `CT02.depth-and-dedup` | `mount:4` → `queue-reverse-order:4` |
| TC139 | `CT02.deleted-pending-root` | `unregister:1` |
| TC140 | `CT03.view-before-window` | `mount-observed-tree:11` → `manual-process-defers-emit:6` → `noop-no-new-signal:4` |
| TC140 | `CT03.latest-and-unsubscribe` | `mount:4` → `two-passes-one-delivery:6` → `unsubscribe-before-post:2` |
| TC141 | `CT04.remove-in-completion` | `mount-extra-window:4` → `remove-during-fence:2` → `recreate-and-process:5` |
| TC141 | `CT04.window-isolation` | `mount-main:4` → `mount-other:5` → `close-other-then-main-change:4` |
| TC142 | `CT05.measure-throws` | `mount:4` → `throw-and-recover:4` → `recovered-fence:4` |
| TC142 | `CT05.arrange-throws` | `mount:4` → `throw-and-recover:4` → `recovered-fence:4` |
| TC142 | `CT05.nested-process` | `mount:4` → `nested-call:6` |
| TC142 | `CT05.remaining-root-rollback` | `mount-boundary-roots:8` → `abort-earlier-root:2` → `drain-restored-batch:8` |
| TC142 | `CT05.lifecycle-throws` | `mount:4` → `throw-lifecycle:3` → `recover-lifecycle:5` |
| TC142 | `CT05.resize-flag-abort` | `mount:4` → `abort-resize-then-change:13` |
| TC155 | `TR15.mode-0-n1` | `mount:7` → `samples:217` |
| TC155 | `TR15W.mode-0-n1` | `mount:7` → `work-count:9` |
| TC155 | `TR15.mode-0-n32` | `mount:162` → `samples:5022` |
| TC155 | `TR15W.mode-0-n32` | `mount:162` → `work-count:164` |
| TC155 | `TR15.mode-0-n128` | `mount:642` → `samples:19902` |
| TC155 | `TR15W.mode-0-n128` | `mount:642` → `work-count:644` |
| TC155 | `TR15.mode-1-n1` | `mount:7` → `samples:217` |
| TC155 | `TR15W.mode-1-n1` | `mount:7` → `work-count:9` |
| TC155 | `TR15.mode-1-n32` | `mount:162` → `samples:5022` |
| TC155 | `TR15W.mode-1-n32` | `mount:162` → `work-count:164` |
| TC155 | `TR15.mode-1-n128` | `mount:642` → `samples:19902` |
| TC155 | `TR15W.mode-1-n128` | `mount:642` → `work-count:644` |
| TC155 | `TR15.mode-2-n1` | `mount:7` → `samples:217` |
| TC155 | `TR15W.mode-2-n1` | `mount:7` → `work-count:9` |
| TC155 | `TR15.mode-2-n32` | `mount:162` → `samples:5022` |
| TC155 | `TR15W.mode-2-n32` | `mount:162` → `work-count:164` |
| TC155 | `TR15.mode-2-n128` | `mount:642` → `samples:19902` |
| TC155 | `TR15W.mode-2-n128` | `mount:642` → `work-count:644` |
| TC155 | `TR15.mode-3-n1` | `mount:7` → `samples:217` |
| TC155 | `TR15W.mode-3-n1` | `mount:7` → `work-count:9` |
| TC155 | `TR15.mode-3-n32` | `mount:162` → `samples:5022` |
| TC155 | `TR15W.mode-3-n32` | `mount:162` → `work-count:164` |
| TC155 | `TR15.mode-3-n128` | `mount:642` → `samples:19902` |
| TC155 | `TR15W.mode-3-n128` | `mount:642` → `work-count:644` |
| TC155 | `TR15.mode-4-n1` | `mount:7` → `samples:217` |
| TC155 | `TR15W.mode-4-n1` | `mount:7` → `work-count:9` |
| TC155 | `TR15.mode-4-n32` | `mount:162` → `samples:5022` |
| TC155 | `TR15W.mode-4-n32` | `mount:162` → `work-count:164` |
| TC155 | `TR15.mode-4-n128` | `mount:642` → `samples:19902` |
| TC155 | `TR15W.mode-4-n128` | `mount:642` → `work-count:644` |

## 실행 evidence 연결 시 필수 항목

각 decision ID에 `source revision + build type + resolved app/library build-ID + fixture hash + (tc,pid,run,scenario,step,action_seq,row)`를 붙이십시오. 실제 branch 도달은 분리한 coverage dump로 증명하고, assertion의 actual/expected/tolerance/finite 및 required/executed 개수를 보존하십시오. 정상 fixture PASS → 해당 계산/cause/취소/순서에 한정한 mutant의 예정 FAIL이 확인되어야 mutation kill로 기록할 수 있습니다. baseline FAIL, capability 실패, build 실패, missing row는 mutation kill이 아닙니다.

이 연결표의 생성, contract 개수 대조, code compilation만으로 execution coverage를 완료했다고 보고하지 마십시오. TG 공백과 실제 실행의 FAIL/미실행은 후속 검증 목록에 남겨야 합니다.
