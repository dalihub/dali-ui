# 145. Layout: Scroll and page layout

이 TC는 ScrollView의 constraint 전달·scroll 보존·축별 padding/margin과 PageScrollView의 layout 기반 page 수·viewport 변경·page 삽입/삭제를 검증합니다. 대응 소스는 `tc-145-layout-scroll.cpp`입니다. 기존 TC 실행 결과는 사용하지 않습니다. **56 scenario, 157 action, 3347 required checks**를 실행하십시오.

## 실행 조건

- foundation manual-test 앱에서 `145. Layout`을 검색하고 `Scroll and page layout`에 진입하십시오.
- `Reset run`을 한 번 누르고 새 `run_id`를 기록하십시오. scale=1, LTR, parent-local 좌표이며 extent 단위는 visual unit입니다.
- pan은 fixture가 끕니다. 새 partial-page 시나리오는 명시적으로 animated `ScrollToPage`를 실행하며, 앱이 `IsScrolling()==false`와 새 frame 완료를 기다립니다. 에이전트는 별도 drag, resize 또는 animation을 시작하지 마십시오.
- 이 TC는 공개 API만으로 관측합니다. 숫자는 결과 행 또는 같은 run의 stdout 원문에서 읽으십시오. screenshot으로 위치를 추정하지 마십시오.

## 조작 및 판정

1. HUD의 `scenario k/N`을 확인하고 `Run scenario`를 누르십시오(step 하나씩 진행하려면 `Run step`).
2. 동기 action은 그 호출에서 끝납니다. mount/resize/content 변경 action은 해당 View들의 `LayoutFinishedSignal`과 같은 action의 Window 완료 fence를 기다립니다. 신규 page 회귀 action은 그 뒤 `Run::AfterFrame`의 새 `REFRESH_ONCE` render task 완료를 기다리고 current geometry를 독립 expected와 비교합니다. animation 대기 조건은 `IsScrolling()==false`이며 expected geometry를 대기 조건으로 사용하지 않습니다. 기본 layout fence 10초, fresh frame 4초 안에 완료되지 않거나 필요한 snapshot이 빠지면 FAIL입니다. 이 frame 관측은 Window compositor 전체 screenshot이 아닙니다.
3. 각 step이 끝나면 `LR_READY`와 그 step의 모든 행이 자동 출력됩니다.
4. 아래 표의 `required_checks`와 실행 행 수가 정확히 같고 모든 필수 행이 PASS여야 action을 통과시킵니다. `Next scenario`는 현재 scenario의 모든 step이 끝난 후에 누르거나 `Run all`로 이어서 실행하십시오. `Run all` 뒤에는 마지막 `LR_RESULT`의 `completed_scenarios`가 `required_scenarios`와 같아질 때까지 기다리십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.
5. 아래 56 scenario, 157 action을 모두 실행하고 3347개 검사 행을 보존하십시오. 실패·누락·timeout이 하나라도 있으면 TC를 PASS로 기록하지 마십시오. 알려진 문제도 xfail로 통과시키지 마십시오.
6. run/scenario/action ID, 필수·실행 행 수, 실패 actual/expected/tolerance, 원문 로그를 저장하십시오. 동일 fixture의 후속 실패가 앞 단계 오류에 따른 것인지도 기록하되 검사를 생략하지 마십시오.

`Rect`는 x/y/width/height 네 행, `Size`는 width/height 두 행입니다. geometry 허용오차는 0.001이며 정수·bool·producer 수는 정확히 일치해야 합니다. explicit constraint 비교는 오차 0입니다. NaN/Inf는 실패합니다. unconstrained callback 입력은 infinity가 아닌 `std::numeric_limits<float>::max()`라는 유한값입니다. candidate 출력으로 expected를 갱신하지 마십시오.

## I05.scroll-preservation — 7 action / 49 checks

| action | 독립 기대값 | 필수 행 수 |
|---|---|---:|
| mount | viewport100×80, content240×200 | 8 |
| scroll-to | scroll(50,60), content(−50,−60,240,200) | 6 |
| settled-layout-replay | 다음 실제 Window layout pass에서도 같은 scroll과 content target 유지 | 6 |
| upper-clamp | max=(240−100,200−80)=(140,120), content(−140,−120,240,200) | 6 |
| resize-viewport | viewport160×120, max=(80,80), 실제 content(−80,−80,240,200) | 10 |

이 step은 `ScrollViewImpl::RefreshViewport()`를 검증합니다. ScrollView 자신의 크기만 바뀐 pass에서 viewport·clamp가 갱신되고(`new-maximum-x/y`, `actual-content.x/y`) 그 통지로 viewport 파생 상태(`PAGE.*`의 `viewport-resize`)까지 재도출되어야 하므로, 이 행들은 모두 PASS여야 합니다. expected를 현재 값으로 바꾸지 마십시오. 실패하면 viewport 갱신 또는 그 통지의 회귀입니다.
| negative-clamp | scroll(0,0), content(0,0,240,200) | 6 |
| replace-smaller-content | 새 content60×40, old content parent 없음, scroll(0,0) | 7 |

`settled-layout-replay`는 ScrollView의 producer가 현재 scroll 위치를 반영하는지 검사합니다. viewport resize는 geometry가 같은 content에서도 clamp가 갱신되어야 합니다. Window snapshot은 callback 당시 layout target이고 실제 Actor position은 component의 post-layout scroll 적용 결과입니다. 각 행에서 정한 관측 시점을 혼합하지 마십시오.

## SC.match-constraints.0…3 — 각 1 action / 17 checks

각 scenario에서 `run`을 실행하십시오. viewport 요청 100×80, ScrollView padding(start,end,top,bottom)=(11,17,7,13), inner constraint=(72,60)입니다. custom content producer의 자연 크기는 240×200입니다. requested 크기가 MATCH_PARENT인 축만 inner constraint를 받습니다.

| scenario | width / height 요청 | 최초 producer constraint | measured 및 arranged content 크기 | Arrange 재측정 constraint | 누적 Measure producer 수 |
|---|---|---|---|---|---:|
| SC.match-constraints.0 | WRAP / WRAP | FLT_MAX / FLT_MAX | 240×200 | 재측정 없음 | 1 |
| SC.match-constraints.1 | MATCH / WRAP | 72 / FLT_MAX | 72×200 | 72 / 200 | 2 |
| SC.match-constraints.2 | WRAP / MATCH | FLT_MAX / 60 | 240×60 | 240 / 60 | 2 |
| SC.match-constraints.3 | MATCH / MATCH | 72 / 60 | 72×60 | 72 / 60, cache hit | 1 |

17행은 owner Measure2, 최초 callback constraint2, content measured2, owner 렌더 geometry4, content 렌더 geometry4, 최종 callback constraint2, 누적 producer1입니다. owner Arrange 반환은(0,0,100,80), content origin은 padding에 따른(11,7)입니다. callback 카운터는 이 synthetic producer만 계수합니다.

## SC.axis-padding-margin.0…2 — 각 4 action / 76 checks

각 scenario에서 `mount → maximum → resize → padding-change` 순서로 실행하십시오. **각 action 19행**입니다. mode0=Horizontal, mode1=Vertical, mode2=Both입니다. axis는 getter와도 정확히 대조합니다. `viewport.*` 4행은 scroll view의 **arrange된 actor rect**이고 `cached-viewport.width/height` 2행은 `ScrollViewImpl`이 **저장한 viewport**입니다(`RefreshViewport`가 둘을 같게 유지해야 합니다). 두 관측을 같은 ID로 묶지 마십시오.

| action | viewport | ScrollView padding(start,end,top,bottom) | mode0 content | mode1 content | mode2 content |
|---|---|---|---|---|---|
| mount | 100×80 | 11,17,7,13 | 240×60 | 72×200 | 72×60 |
| maximum | 100×80 | 11,17,7,13 | 240×60 | 72×200 | 72×60 |
| resize | 130×110 | 11,17,7,13 | 240×90 | 102×200 | 102×90 |
| padding-change | 130×110 | 2,4,3,5 | 240×102 | 124×200 | 124×102 |

mount의 scroll은(0,0)입니다. maximum은 `ScrollTo(999,999,false)`이고 이후 resize/padding에서도 유효한 최대 위치로 clamp됩니다. maxScroll=(max(contentWidth−viewportWidth,0), max(contentHeight−viewportHeight,0))이므로 mode0 x는 140→110→110, mode1 y는 120→90→90, mode2는 항상(0,0)입니다. content origin은 `(padding.start−scroll.x, padding.top−scroll.y)`입니다.

content 내부에는 padding(3,5,7,11)을, 내부 MATCH child에는 margin(2,4,6,8)을 적용합니다. 내부 child의 local rect는 `(5,13,max(contentWidth−14,0),max(contentHeight−32,0))`입니다. 이는 content 내부 기본 Layout의 margin/padding 결합 검사이며 ScrollView가 content 자체의 margin을 처리한다는 별도 계약을 가정하지 않습니다.

19행은 viewport Rect4, content Rect4, 내부 margin child Rect4, scroll2, integration API의 viewport2·scrollable size2, direction1입니다. mount/resize/padding의 실제 Actor event property는 필요한 View snapshot을 모두 받은 Window fence에서 읽습니다.

## PAGE.horizontal / PAGE.vertical — 각 8 action / 156 checks

두 축에서 아래 모든 action을 실행하십시오. cross viewport는 80이며 main viewport는 처음 100, resize 후 125입니다. content는 같은 축의 StackLayout이며 page 길이는 100,100,50입니다. 마지막 부분 page를 포함한 실제 content 길이는 250입니다. page 크기 기본값(0,0)은 viewport를 사용하므로 pageCount=ceil(contentLength/pageLength)입니다. scroll main은 `[0,max(contentLength−viewportLength,0)]`로 clamp됩니다.

| action | 주요 독립 기대값 | 필수 행 수 |
|---|---|---:|
| mount-default-size | pageCount3, current0, scroll0, content250, 최초 PageChanged(0,3) 1회 | 14 |
| page-boundaries | ScrollToPage(−7,false)→page0/scroll0; ScrollToPage(99,false)→page2/scroll150, 마지막 signal(2,3) | 23 |
| viewport-resize | main viewport125, pageCount2, current1, scroll125, content250, signal(1,2) | 13 |
| explicit-page-size | main page100/cross17, pageCount3, current1, 기존 scroll125 유지, signal(1,3), page size getter 정확 일치 | 15 |
| insert-before-current | 앞에 길이 100 page를 추가한 뒤 NotifyPagesInserted(0,1); 즉시 count4/current2/scroll200/signal(2,4) 1회; layout 후 content350, 기존 page identity·local 위치 200·viewport 내 위치 0 유지 | 30 |
| remove-before-current | 앞 page 제거 후 NotifyPagesRemoved(0,1); 즉시 count3/current1/scroll100/signal(1,3) 1회; layout 후 content250 | 24 |
| remove-all | 실제 page 모두 제거 후 NotifyPagesRemoved(0,3); 즉시 count0/current−1/scroll0/signal(−1,0) 1회; layout 후 content main extent0 | 24 |
| restore-default | page size(0,0) 복원, 길이 100 page 추가; pageCount1/current0/scroll0/content100/signal(0,1) | 13 |

각 기본 `Check`는 count/current2, scroll main/cross2, content Rect4, integration API의 scrollable main extent/viewport main extent2로 **10행**입니다. cross scroll은 0입니다. `SignalCheck`는 고정 64개 event buffer에 overflow가 없고 event가 존재한다는 조건 1, 마지막 page/count2로 **3행**입니다. insert에서는 추가로 기존 child identity1, 기존 현재 page local Rect4, viewport 내 위치 1을 검사합니다.

Notify 직후에는 예상 page 수가 즉시 반영되지만 실제 content Arrange는 다음 layout에서 이루어집니다. 이 시점을 분리해서 읽으십시오.

| 관측 | insert 직후 | remove 직후 | empty 직후 |
|---|---:|---:|---:|
| 공개 expected page 수 | 4 | 3 | 0 |
| 선반영 scrollable main extent(page100×expectedCount) | 400 | 300 | 0 |
| 아직 재배치 전 Actor content main extent | 250 | 350 | 250 |
| 다음 layout 후 content·scrollable main extent | 350 | 250 | 0 |

부분 마지막 page 때문에 선반영 range와 최종 geometry가 다릅니다. page count·현재 identity 보존을 검사하면서 이 임시 range를 최종 geometry로 오인하지 마십시오. viewport만 바뀌고 content 크기는 그대로인 resize도 count/current/scroll 갱신을 검사하므로 누락된 invalidation을 드러낼 수 있습니다.

## PAGE.notify-horizontal/vertical — 추가 24 scenario / 48 action / 1104 checks

아래 suffix11개를 두 축에서 각각 실행하십시오. 정확한 scenario ID는 `PAGE.notify-horizontal.SUFFIX`, `PAGE.notify-vertical.SUFFIX`입니다. 각 scenario는 독립적인 길이 100 page5개로 시작하며 cross extent80, viewport main100입니다. `mount`는 14행이고 initial current를 아래 표의 값으로 즉시 이동시킨 뒤 count5·scroll=`100×current`·geometry·signal을 검증합니다.

`notify-and-settle`은 실제 child tree를 먼저 바꾸고 API를 호출합니다. 각 행의 actual index/count는 TC가 tree를 수정할 위치이며 API 인수와 구별됩니다. clamp 기대는 표에 고정되어 있고 candidate getter에서 생성하지 않습니다.

| SUFFIX | initial current | API index/count | 실제 tree index/count | 최종 count/current/scroll | notify-and-settle 필수 행 |
|---|---:|---|---|---|---:|
| insert-at-current | 2 | 2/1 | 2/1 삽입 | 6/3/300 | 31 |
| insert-after | 2 | 4/1 | 4/1 삽입 | 6/2/200 | 31 |
| insert-negative-index | 2 | −7/1 | 0/1 삽입 | 6/3/300 | 31 |
| insert-past-end | 2 | 99/1 | 5/1 삽입 | 6/2/200 | 31 |
| remove-after | 2 | 4/1 | 4/1 삭제 | 4/2/200 | 31 |
| remove-current | 2 | 2/1 | 2/1 삭제 | 4/2/200 | 31 |
| remove-through-current | 2 | 1/3 | 1/3 삭제 | 2/1/100 | 31 |
| remove-last-current | 4 | 4/99 | 4/1 삭제 | 4/3/300 | 31 |
| remove-negative-index | 2 | −7/1 | 0/1 삭제 | 4/1/100 | 31 |
| remove-past-end | 2 | 99/99 | 4/1 삭제 | 4/2/200 | 31 |
| remove-all-clamped | 2 | −7/99 | 0/5 삭제 | 0/−1/0 | 26 |

`notify.immediate.*`10행은 count/current, scroll2, 아직 이전 extent500인 content Rect4, 새 예상 scrollable extent=`100×count`, viewport100을 검사합니다. `page.signal.*`3행은 overflow 없는 buffer와 최종 page/count, `notify.signal-delta=1` 및 `notify.logical-count`가 뒤따릅니다.

Window fence 뒤 `notify.settled.*`10행은 실제 content extent=`100×count`를 검사합니다. 비어 있지 않으면 `notify.visible-identity`, `notify.visible-local` Rect4, `notify.visible-origin=0`도 필수입니다. 현재 page를 삭제한 경우에는 그 자리에 남은 다음 page 또는 마지막 유효 page identity가 기대값입니다. empty는 `notify.empty-children` 한 행을 사용합니다. old page identity를 삭제 후에도 보존하라고 요구하지 않습니다.

두 축에 각각 `PAGE.notify-AXIS.nonpositive-count`도 있습니다. `mount`14행 뒤 `notify-noop`48행을 실행하십시오. 삽입 count0/−3, 삭제 count0/−3 네 호출은 tree 변경 없이 count5/current2/scroll200/content500을 그대로 유지해야 하며 각각 signal delta0입니다. `noop.0…3.*` 각 11행, 마지막 `page.signal.*`3행 및 `noop.logical-count=5`가 필요합니다. no-op도 signal을 발생시키는 회귀를 검출합니다.

## PAGE.pre-layout-horizontal/vertical — 추가 6 scenario / 12 action / 370 checks

각 축에서 `insert-before`, `insert-after`, `remove-empty` suffix를 실행하십시오. 정확한 ID는 `PAGE.pre-layout-horizontal.SUFFIX`, `PAGE.pre-layout-vertical.SUFFIX`입니다. 첫 action `notify-before-layout`은 fixture를 Window에 **붙이지 않습니다**. 요청 viewport100과 page100 하나를 구성해도 실제 viewport·scrollable은 아직 0이어야 합니다. 다음 action `attach-and-settle`에서만 붙입니다. agent가 그 사이 fixture를 별도로 attach하거나 임의 Measure/Arrange로 준비하지 마십시오.

| SUFFIX | layout 전 실제 tree와 Notify | 즉시 count/current/scroll | attach 이후 기대 | action별 필수 행 |
|---|---|---|---|---|
| insert-before | 기존 page 앞에 page100 추가, NotifyPagesInserted(0,1) | 2/1/0 | count2/current1/scroll100, 기존 page가 viewport origin에 표시 | 41 → 20 |
| insert-after | 기존 page 뒤에 page100 추가, NotifyPagesInserted(1,1) | 2/0/0 | count2/current0/scroll0 | 41 → 20 |
| remove-empty | 유일한 page 삭제, NotifyPagesRemoved(0,1); 이어 빈 상태에서 NotifyPagesRemoved(99,7) | 0/−1/0, 두 번째 호출은 signal 추가 없음 | count0/current−1/scroll0/content main0 | 48 → 15 |

모든 첫 action은 `pending.initial.*`에서 default page size의 보수적 count1/current0을, `pending.explicit.*`에서 main100/cross17의 명시 size를 적용한 count0/current−1을 검사합니다. 실제 scrollable0이므로 이 시점에는 signal이 없어야 합니다. size getter2행도 필요합니다. 같은 size 재설정은 `pending.same-size.*` 상태를 보존해야 합니다. (0,0)으로 복원한 `pending.default.*`는 다시 count1/current0입니다.

각 `CheckUnmeasuredPage` prefix의 7행은 count/current2, scroll x/y2, scrollable main/viewport main2, signal-count1입니다. Notify 후 `pending.notified.*`7행, `page.signal.*`3행, `pending.logical-count`1행을 검사합니다. remove-empty는 `pending.already-empty.*`7행이 추가됩니다. insert-before의 layout 전 scroll0은 아직 range가 없기 때문이며, 실제 첫 layout 이후에도 pending current page의 표시 위치를 잃어도 된다는 뜻이 아닙니다.

attach action은 지정 View snapshot과 Window fence 후 `pending.settled.*`10행, `page.signal.*`3행, logical count1행을 읽습니다. 비어 있지 않으면 expected visible identity1, local Rect4, `pending.visible-origin=0`1행, empty이면 `pending.empty-children`1행을 추가합니다. page current index와 실제 보이는 page가 서로 어긋나면 FAIL입니다.

## 근거와 검증 경계

- `public-api/layouts/scroll-view-layout-manager.cpp`의 MATCH 축 constraint, 자연 축 unconstrained Measure, Arrange-owned 재측정, ALWAYS Arrange와 실제 scroll 위치 보존을 검사합니다.
- `integration-api/scroll-view-impl.cpp`의 viewport·scrollable size·padding origin·AdjustScrollPosition·ApplyScrollPosition을 수치로 대조합니다.
- `public-api/views/scroll/page-scroll-view.h`의 page 크기·count·index·Notify 계약과 `integration-api/page-scroll-view-impl.cpp`의 즉시 expected count/다음 layout 갱신 경로를 검사합니다.

이 TC는 scroll과 Layout의 결합을 대상으로 하며 fling 물리·gesture 인식·scrollbar GUI 전체를 검증한다는 의미는 아닙니다. timing은 측정하지 않습니다. `TC152/153/156`의 성능 결과를 이 TC의 정확성 통과로 대체하지 마십시오.

## Page resize와 명시 selection 회귀 — 양축 각각 13 action / 271 checks

아래 세 scenario를 horizontal과 vertical 모두 실행하십시오. 축을 바꾸면 main/cross 좌표만 교환되며 독립 기대값의 page, count, main offset은 같습니다. content는 기본 100 길이 page 5개입니다. `CheckPageFrame`의 19행은 page/count/scroll/cached viewport/event-side content 10행, current scene-graph content와 viewport 각 4행, animation idle 1행입니다. signal 검사는 그 밖의 별도 필수 행입니다.

| Scenario | action | 독립 기대값 | 필수 행 수 |
| --- | --- | --- | ---: |
| `PAGE.same-count-resize-{axis}` | `mount-last` | viewport100, content500, page4, offset400, count5 | 14 |
| 동일 | `resize-same-count` | viewport124, count=ceil(500/124)=5 유지, offset=500−124=376, page=round(376/124)=3, signal 정확히 1회 | 23 |
| 동일 | `same-offset-scroll` | `ScrollTo(GetScrollPosition(),false)` 후 page3/offset376 유지, 추가 signal 0 | 20 |
| 동일 | `repeat-layout` | 실제 controller의 반복 Arrange 후 geometry/page 유지, 추가 signal 0 | 20 |
| `PAGE.explicit-partial-last-{axis}` | `mount` | viewport100, page0, offset0 | 14 |
| 동일 | `select-explicit-last` | explicit page length124, animated page4 선택, count5, offset=min(4×124,500−100)=400, signal 1회 | 24 |
| 동일 | `cross-axis-resize` | cross viewport80→90, main viewport100 유지, page4/offset400/count5 유지, signal 0 | 20 |
| 동일 | `repeat-layout` | page4와 current geometry 유지, 추가 signal 0 | 20 |
| `PAGE.notify-partial-last-{axis}` | `mount-partial-last` | viewport124, animated page4, count5, offset376, 초기 count 및 selection signal 합계2 | 24 |
| 동일 | `insert-before-last` | 앞에 길이124 page 삽입 및 `NotifyPagesInserted(0,1)`: immediate count6/page5, layout 후 content624/offset500/page5, 전체 signal 증가1 | 26 |
| 동일 | `insert-repeat-callback` | cross80→90으로 반복 area callback 유도, count6/page5/offset500 유지, signal 0 | 20 |
| 동일 | `remove-before-last` | 삽입한 page 제거 및 `NotifyPagesRemoved(0,1)`: immediate count5/page4, layout 후 content500/offset376/page4, 전체 signal 증가1 | 26 |
| 동일 | `remove-repeat-callback` | cross90→80으로 반복 area callback 유도, count5/page4/offset376 유지, signal 0 | 20 |

일반적인 settled viewport resize는 page를 실제 offset에서 재도출합니다. 반면 명시적으로 완료된 animated snap 또는 pending notification은 선택한 page를 유지해야 합니다. partial last page의 물리 offset을 반올림한 값으로 이를 덮으면 FAIL입니다. 예를 들어 explicit page4의 offset400은 round(400/124)=3이지만 명시 selection은 4이고, pending insert의 page5/offset500도 round(500/124)=4로 바뀌면 안 됩니다. `SetPageSize` 뒤 cross-axis layout callback만으로 selection이 바뀌는 것도 FAIL입니다.

## Animation 도중 viewport resize — 양축·count 조건별 각 3 action / 64 checks

`PAGE.inflight-resize-{horizontal,vertical}.{same-count,smaller-count,same-count-fractional,smaller-count-partial}`의 여덟 scenario에서 `mount`(14행) → `resize-during-snap`(30행) → `repeat-settled-layout`(20행)을 실행하십시오. mount는 viewport100, content500, page0입니다. 다음 action은 원래 page4/offset400을 향해 animated snap을 시작하고, `IsScrolling()==true`인 동안 viewport를 바꾼 뒤 실제 LayoutController를 동기 drain합니다. `snap.animation-started`와 `snap.resized-while-scrolling`이 모두 true여야 합니다.

| mode | 새 viewport | 완료 offset | 완료 count/page | PageChanged 증가 |
| --- | ---: | ---: | --- | ---: |
| `same-count` | 124 | 500−124=376 | count5/page4 | 1 |
| `smaller-count` | 250 | 500−250=250 | count2/page1 | 1 |
| `same-count-fractional` | 124.4 | 500−124.4=375.6 | count5/page4 | 1 |
| `smaller-count-partial` | 160 | 500−160=340 | count4/page3 | 1 |

same-count와 same-count-fractional에서는 명시 snap target4가 유효하므로 round(376/124)=3이나 round(375.6/124.4)=3으로 덮지 않습니다. smaller-count에서는 원래 target4가 현재 범위를 벗어나므로 count−1=1로, smaller-count-partial에서는 count−1=3으로 clamp해야 합니다. 네 mode 모두 PageChanged는 1회입니다.

count가 바뀌는 통지가 보고하는 page는 **그 view가 정착할 page**입니다. snap이 진행 중이면 새 count로 clamp한 snap target이며, 이는 완료 처리가 commit하는 값과 같으므로 통지는 한 번으로 끝납니다(완료 처리는 같은 page를 보고 조용히 지나갑니다). 마지막 page가 부분인 smaller-count-partial에서 물리 offset340을 반올림하면 round(340/160)=2이지만 실제로 정착할 page는 3이므로, 처음부터 (page3,count4)를 보고해야 하며 (page2,count4) 뒤에 (page3,count4)가 이어지는 두 번의 통지는 FAIL입니다. smaller-count의 통지도 처음부터 (page1,count2)입니다. snap target이 없는 자유 scroll이나 정지 상태에서는 종전대로 scroll 위치에서 page를 다시 유도하며, 그 위치는 animation 중에는 목표가 아니라 현재 위치입니다. same-count와 same-count-fractional은 count가 바뀌지 않아 resize 시점에는 통지가 없고 완료 시 page0→4의 1회만 발생합니다.

same-count-fractional의 최대 offset은 분수 375.6입니다. animation 완료 후의 범위 복구는 clamp한 **float**을 그대로 적용해야 하며, 반올림하여 376에 놓으면 view가 스크롤로는 도달할 수 없는 위치이므로 FAIL입니다. content actor의 main 좌표도 −375.6이어야 하고, 허용오차는 0.001입니다.

네 mode 모두 `ScrollFinishedSignal`은 정확히 1회이며, callback 시점에 이미 보정된 main/cross offset과 유효한 page/count를 관측해야 합니다. 이후 새 frame 완료에서 content/viewport current geometry를 별도로 검사합니다. 반복 layout은 추가 signal 없이 같은 상태를 유지해야 합니다.

이 검사는 animation **완료 후** 범위 복구와 signal의 관측 순서를 보장합니다. 여덟 scenario 모두 같은 3 action / 64행 구성입니다. animation 진행 중의 모든 frame을 새 endpoint로 retarget하거나 overshoot를 제거했다는 의미는 아닙니다. 대기 조건은 완료 여부뿐이며 geometry expected를 기다림 조건으로 사용하지 않습니다. 빈 content의 별도 page-count 통지 동작도 이 여덟 scenario의 범위에 포함하지 않습니다.

## 정지 상태 no-op animated 요청 뒤 page 수 변경 — 양축 각각 4 action / 81 checks

`PAGE.noop-snap-resize-{horizontal,vertical}`에서 `mount`(14행) → `instant-select`(23행) → `animated-same-page`(21행) → `resize-smaller-count`(23행)을 실행하십시오. content는 길이 100 page 5개(content500)이고 cross viewport는 80, main viewport는 처음 100이며 page 크기 기본값(0,0)은 viewport를 사용합니다. 축을 바꾸면 main/cross 좌표만 교환되며 독립 기대값의 page, count, main offset은 같습니다.

| action | 조작 | 독립 기대값 | 필수 행 수 |
| --- | --- | --- | ---: |
| `mount` | page0으로 mount | viewport100, count5, page0, offset0, 마지막 signal(0,5) | 14 |
| `instant-select` | `ScrollToPage(2,false)` 뒤 새 frame | count5, page2, offset=2×100=200, event-side와 current scene-graph의 content main −200, 마지막 signal(2,5), signal 증가 정확히 1 | 23 |
| `animated-same-page` | `ScrollToPage(2,true)` 뒤 새 frame | 목표 offset200이 현재 위치와 같으므로 요청 직후 `IsScrolling()==false`(`noop-snap.animation-not-started`), page2/offset200/count5와 content −200 유지, signal 증가 0 | 21 |
| `resize-smaller-count` | main viewport 100→160, layout 뒤 새 frame | count=ceil(500/160)=4, offset200 유지(최대 500−160=340 이내), page=round(200/160)=1, 마지막 signal(1,4), signal 증가 정확히 1, content main −200 | 23 |

`instant-select`와 `resize-smaller-count`는 `CheckPageFrame`19행, `page.signal.*`3행, signal 증가1행이고, `animated-same-page`는 `noop-snap.animation-not-started`1행, `CheckPageFrame`19행, signal 증가1행입니다.

scroll을 시작하지 않은 animated 요청은 snap target을 남기면 안 됩니다. 이 요청에는 완료 통지가 없으므로 target이 남으면 다음 page 수 변경이 그 target인 page2를 보고합니다. 실제 offset은 200이고 가장 가까운 page는 1이므로 `resize-smaller-count`의 `noop-resized.current=1`과 `page.signal.page=1`이 기대값이며, target이 남는 결함에서는 두 행이 2를 보고합니다. content 위치는 두 경우 모두 −200이므로 geometry로 page2를 정당화할 수 없고, signal 증가도 두 경우 모두 1이므로 횟수만으로 판정하지 마십시오. 이 행들이 실패하면 제품 회귀로 보존하고 expected를 바꾸지 마십시오.

instant scroll과 no-op 요청은 layout pass를 요청하지 않으므로 `instant-select`와 `animated-same-page`는 `AfterLayout`을 기다리지 않고 `Run::AfterFrame`의 새 render task 완료 뒤에 관측합니다. `animated-same-page`는 `IsScrolling()==false`를 기다리지 않고 요청 직후 한 번 검사합니다. 대기 조건은 frame 완료뿐이며 expected geometry를 기다림 조건으로 사용하지 않습니다.

## 종료와 재현

`< Back`으로 나간 뒤 같은 TC에 다시 진입하고 `Reset run` 이후 첫 scenario를 반복하십시오. run ID와 counter가 새로 시작하고 초기 기대값이 같아야 합니다. 반복 실행은 위 3347행 집계와 별도로 기록하십시오.

## 단계 구성

- `SC.match-constraints.{bits}`(17행)는 100×80 ScrollView를 stage에 부착하고 controller pass 뒤 owner measured (100,80), 첫 producer 호출의 제약(`measure.constraint.*`), content measured, owner/content 렌더 rect, 마지막 producer 호출의 제약(`arrange.constraint.*`), producer 횟수(단일 MATCH 축은 2, 그 외 1)를 검사합니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 56 scenario, 157 action, 3347 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `I05.scroll-preservation` | `mount` : 8 → `scroll-to` : 6 → `settled-layout-replay` : 6 → `upper-clamp` : 6 → `resize-viewport` : 10 → `negative-clamp` : 6 → `replace-smaller-content` : 7 |
| `SC.match-constraints.0` | `run` : 17 |
| `SC.match-constraints.1` | `run` : 17 |
| `SC.match-constraints.2` | `run` : 17 |
| `SC.match-constraints.3` | `run` : 17 |
| `SC.axis-padding-margin.0` | `mount` : 19 → `maximum` : 19 → `resize` : 19 → `padding-change` : 19 |
| `SC.axis-padding-margin.1` | `mount` : 19 → `maximum` : 19 → `resize` : 19 → `padding-change` : 19 |
| `SC.axis-padding-margin.2` | `mount` : 19 → `maximum` : 19 → `resize` : 19 → `padding-change` : 19 |
| `PAGE.horizontal` | `mount-default-size` : 14 → `page-boundaries` : 23 → `viewport-resize` : 13 → `explicit-page-size` : 15 → `insert-before-current` : 30 → `remove-before-current` : 24 → `remove-all` : 24 → `restore-default` : 13 |
| `PAGE.vertical` | `mount-default-size` : 14 → `page-boundaries` : 23 → `viewport-resize` : 13 → `explicit-page-size` : 15 → `insert-before-current` : 30 → `remove-before-current` : 24 → `remove-all` : 24 → `restore-default` : 13 |
| `PAGE.notify-horizontal.insert-at-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.insert-after` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.insert-negative-index` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.insert-past-end` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-after` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-through-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-last-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-negative-index` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-past-end` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-horizontal.remove-all-clamped` | `mount` : 14 → `notify-and-settle` : 26 |
| `PAGE.notify-horizontal.nonpositive-count` | `mount` : 14 → `notify-noop` : 48 |
| `PAGE.notify-vertical.insert-at-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.insert-after` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.insert-negative-index` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.insert-past-end` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-after` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-through-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-last-current` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-negative-index` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-past-end` | `mount` : 14 → `notify-and-settle` : 31 |
| `PAGE.notify-vertical.remove-all-clamped` | `mount` : 14 → `notify-and-settle` : 26 |
| `PAGE.notify-vertical.nonpositive-count` | `mount` : 14 → `notify-noop` : 48 |
| `PAGE.pre-layout-horizontal.insert-before` | `notify-before-layout` : 41 → `attach-and-settle` : 20 |
| `PAGE.pre-layout-horizontal.insert-after` | `notify-before-layout` : 41 → `attach-and-settle` : 20 |
| `PAGE.pre-layout-horizontal.remove-empty` | `notify-before-layout` : 48 → `attach-and-settle` : 15 |
| `PAGE.pre-layout-vertical.insert-before` | `notify-before-layout` : 41 → `attach-and-settle` : 20 |
| `PAGE.pre-layout-vertical.insert-after` | `notify-before-layout` : 41 → `attach-and-settle` : 20 |
| `PAGE.pre-layout-vertical.remove-empty` | `notify-before-layout` : 48 → `attach-and-settle` : 15 |
| `PAGE.same-count-resize-horizontal` | `mount-last` : 14 → `resize-same-count` : 23 → `same-offset-scroll` : 20 → `repeat-layout` : 20 |
| `PAGE.explicit-partial-last-horizontal` | `mount` : 14 → `select-explicit-last` : 24 → `cross-axis-resize` : 20 → `repeat-layout` : 20 |
| `PAGE.notify-partial-last-horizontal` | `mount-partial-last` : 24 → `insert-before-last` : 26 → `insert-repeat-callback` : 20 → `remove-before-last` : 26 → `remove-repeat-callback` : 20 |
| `PAGE.same-count-resize-vertical` | `mount-last` : 14 → `resize-same-count` : 23 → `same-offset-scroll` : 20 → `repeat-layout` : 20 |
| `PAGE.explicit-partial-last-vertical` | `mount` : 14 → `select-explicit-last` : 24 → `cross-axis-resize` : 20 → `repeat-layout` : 20 |
| `PAGE.notify-partial-last-vertical` | `mount-partial-last` : 24 → `insert-before-last` : 26 → `insert-repeat-callback` : 20 → `remove-before-last` : 26 → `remove-repeat-callback` : 20 |
| `PAGE.inflight-resize-horizontal.same-count` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-horizontal.smaller-count` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-horizontal.same-count-fractional` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-horizontal.smaller-count-partial` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-vertical.same-count` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-vertical.smaller-count` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-vertical.same-count-fractional` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.inflight-resize-vertical.smaller-count-partial` | `mount` : 14 → `resize-during-snap` : 30 → `repeat-settled-layout` : 20 |
| `PAGE.noop-snap-resize-horizontal` | `mount` : 14 → `instant-select` : 23 → `animated-same-page` : 21 → `resize-smaller-count` : 23 |
| `PAGE.noop-snap-resize-vertical` | `mount` : 14 → `instant-select` : 23 → `animated-same-page` : 21 → `resize-smaller-count` : 23 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC145 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
