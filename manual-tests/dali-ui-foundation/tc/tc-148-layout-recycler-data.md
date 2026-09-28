# 148. Layout: Recycler data mutations

이 문서는 [tc-148-layout-recycler-data.cpp](tc-148-layout-recycler-data.cpp)의 실행 절차입니다. 기존 Layout TC 결과는 합산하지 마십시오. 전체 실행 조건은 [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오.

## 실행 및 완료 확인

1. app와 실제 resolve된 library의 build-ID, revision, build type을 기록하고 stdout을 저장하십시오. launcher에서 `148. Layout`을 검색하여 해당 TC에 진입하십시오.
2. `TC148.Reset run`을 한 번 실행하고 새 `run`을 기록하십시오. scenario 내 step 사이에는 Reset하지 마십시오.
3. `Run scenario`(step 하나씩 진행하려면 `Run step`)를 누른 뒤 `LR_ACTION`의 scenario/step/action_seq/required_checks를 기록하십시오. 동기 step은 즉시 완료됩니다. Window/resource step은 해당 episode의 완료까지 기다리십시오. 10초 내 완료되지 않으면 PENDING을 PASS로 처리하지 마십시오. 성능 sample step만 최대 120초를 허용합니다.
4. 각 step 완료 시 자동 출력되는 **모든** `LR_CHECK` 행을 읽으십시오. 행의 tc/pid/run/scenario/step/action_seq를 연결하여 다른 실행의 값과 혼합하지 마십시오. 같은 행을 다시 읽으면 값이 같아야 합니다.
5. 해당 step의 실제 행 수와 required_checks가 정확히 같아야 합니다. 일반 rect 허용오차는 0.001, 정수/bool/ID는 exact입니다. NaN/Inf, 누락/잘림, 중복 읽기로 채운 행 수, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. `passed`만 믿지 말고 actual/expected/tolerance를 재계산하십시오.
6. 모든 step 후 `Next scenario`로 이동하고 모든 scenario를 실행하십시오. 마지막 `LR_RESULT`의 completed_scenarios=required_scenarios, failures=0 및 전체 행 증거가 필요합니다. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오. 실패 후 Reset으로 실패 이력을 지우지 마십시오.

## 입력과 독립 기대값

| scenario / step | 주요 기대값 | 검사 수 |
|---|---|---:|
| adapter-lifecycle / initial | 12개, range262, viewport50, item0.2 materialized |26|
| content-only | position1 stable.7777, bind 증가 1, create 증가 0, geometry 불변 |14|
| insert | 앞에 stable.8888 삽입, count13/range284 |9|
| move | position0→2, count13/range284, 각 holder ID가 새 source 위치와 일치 |9|
| remove | 첫 item 제거, count12/range262 |9|
| size-change | 첫 height30, item0=(0,0,120,30), item1=(0,32,120,20), estimated range272 |9|
| full-data-empty | count0/range0/bound0, recycle 발생 |4|
| replace-clear | 새 adapter2개/range42 → ClearAdapter/empty/range0 |4|
| window-viewport / attach | 화면의 120×50 stage에 부착한 root(0,0,120,50), range262, first0/last2 |8|
| window-viewport / resize | stage와 target height94, viewport94, first0/last4 |7|

초기 item rect는 (0,22*i,120,20)입니다. 모든 bound holder의 NAME `stable.<id>`를 실제 adapter source와 비교하십시오. 위치만 맞고 다른 item ID가 표시되는 경우 FAIL입니다. content-only 변경은 크기를 재설정하지 않고 새 ID binding만 수행합니다. insert/move/remove는 같은 adapter 인스턴스에서 순서대로 실행하며 중간 Reset을 금지합니다.

window-viewport는 실제 View/Window 완료 snapshot을 사용합니다. RecyclerView는 주어진 constraint를 그대로 채우므로 120×50 stage 아래에 부착하며, resize step은 stage와 target의 height를 함께 94로 바꿉니다. direct adapter-lifecycle의 동기 Arrange 반환으로 Window resize 검증을 대신하지 마십시오. RecyclerView의 estimated range는 item offset과 같은 항목별 규칙(측정된 항목은 측정값, 미측정 항목은 SetItemExtent 추정값, 그리고 spacing)으로 계산하므로, 미측정 item의 실제 크기가 추정값과 다르면 실제 합과 다를 수 있습니다.


## 실제 position scroll과 focus/peek

세 번째 scenario window-scroll-focus를 아래 순서대로 모두 실행하십시오. 화면의 120×50 stage에 동일한 12개 item을 가진 RecyclerView를 붙이며 cache before/after를 1000으로 두어 focus 대상 holder가 먼저 존재하도록 합니다. 이 설정은 virtualization 성능 검증용이 아니며, target 미생성 때문에 focus 경로가 실행되지 않는 것을 방지합니다. 각 item은 focusable입니다.

Window에 platform input focus가 있어야 합니다. 서버는 실제 앱 창을 활성 상태로 유지하고 step의 PENDING 동안 다른 창이나 조작 대상에 focus를 옮기지 마십시오. inactive Window에 저장된 focus 예약을 실제 focus 이동의 PASS로 처리하지 마십시오. focus.actual.target과 이동에 필요한 started/finished 신호가 이를 검출합니다.

독립 입력은 item i의 content rect=(0,22×i,120,20), viewport50, 전체 range12×20+11×2=262입니다. 최대 offset=max(0,262−50)=212입니다.

- ScrollToPosition(i,false)의 offset은 clamp(22×i,0,212)입니다.
- focus 대상의 시작/끝은 start=22×i, end=start+20입니다. peek=max(0,요청값)입니다.
- start−peek<현재 offset이면 target=start−peek입니다. 그렇지 않고 end+peek>현재 offset+50이면 target=end+peek−50입니다. 둘 다 아니면 offset을 유지합니다.
- 마지막으로 target을[0,212]로 clamp합니다. SetScrollOnFocus(false)이면 focus 이동으로 offset이 변하면 안 됩니다.

| step | 입력 | expected offset | required_checks |
|---|---|---:|---:|
| attach | Window120×50,12개 holder materialized |0|18|
| position-middle | ScrollToPosition(3,false) |66|13|
| position-end-clamp | ScrollToPosition(11,false) |212|13|
| position-start | ScrollToPosition(0,false) |0|13|
| offset-low-clamp | SetScrollOffset(-99) |0|13|
| offset-high-clamp | SetScrollOffset(999) |212|13|
| focus-end | offset0,item3,peek0 |36|18|
| focus-start | offset80,item3,peek0 |66|18|
| focus-already-visible | offset60,item3,peek0 |60|18|
| peek-end | offset0,item3,peek7 |43|18|
| peek-start | offset60,item3,peek7 |59|18|
| peek-head-clamp | offset100,item0,peek7 |0|18|
| peek-tail-clamp | offset0,item11,peek7 |212|18|
| negative-peek-normalize | offset0,item3,peek-7→0 |36|18|
| focus-scroll-disabled | offset0,item3,peek7,SetScrollOnFocus(false) |0|18|

각 step에서 필수 13개 행은 scroll.offset/range/viewport, scroll.target.materialized, scroll.content.rect.{x,y,width,height}, scroll.scroller.x/y, scroll.viewport.item.y, scroll.render.scroller.y, scroll.animation.idle입니다. content rect는 scroll 후에도(0,22×i,120,20)이어야 하며 실제 parent scroller의 event/render y는 둘 다−offset, item의 viewport y는 22×i−offset입니다. layout 계산값만 맞고 실제 content가 이동하지 않는 경우에도 FAIL입니다.

attach에는 실제 Window snapshot rect4행과 cached holder count12 한 행이 추가됩니다. focus step에는 focus.request.accepted, focus.peek.normalized, focus.actual.target, focus.scroll.started.delta, focus.scroll.finished.delta가 추가됩니다. offset을 이동하는 focus step은 started/finished가 각각 1이어야 합니다. already-visible 및 focus-scroll-disabled는 각각 0이어야 합니다.

position/offset 단계는 실제 render property 반영을 위해 100ms 후 관측합니다. focus 단계는 public FocusManager::SetCurrentFocusView를 호출하고 2000ms 후 관측합니다. native scroll animation의 configured 최대 1200ms보다 길게 기다리되 가상 clock이나 animation 내부 값을 바꾸지 않습니다. 2000ms가 지났다는 사실만으로 PASS가 아니며 실제 finished 신호, IsScrolling=false, target focus와 모든 좌표가 함께 맞아야 합니다. 10초 안에 결과가 고정되지 않으면 timeout으로 기록하십시오.

각 focus 단계 시작 시 ClearFocus 후 초기 offset/peek를 설정하여 이전 target과 같아서 FocusChangedSignal이 생략되는 경우를 배제합니다. 종료 시 ScrollOnFocus를 끄고 진행 중 scroll을 취소한 뒤 이전 focus를 복원합니다. 필요한 signal callback은 counter만 수정합니다.

근거 경로는 integration-api/recycler/recycler-view-impl.cpp의 ScrollToPosition/ScrollToItemMakeVisible/OnFocusManagerChanged/OnScrollAnimationFinished/ApplyScrollerPosition과 public-api/focus-manager/focus-manager.h의 SetCurrentFocusView 계약입니다.

## 종료와 증거

`< Back`으로 나간 뒤 다시 진입하여 첫 step을 재실행하십시오. 이전 timer/callback/fixture/추가 Window가 남아 있으면 FAIL입니다. 결과에는 scenario/input/step, 실제값과 기대값, 최초 실패 행, 전체 stdout, artifact fingerprint를 첨부하십시오. `python3 layout-validation-tools.py check-log LOG --tc TC148`은 저장된 행의 누락과 수치를 다시 검사하는 보조 수단입니다. 이 도구는 앱을 실행하거나 서버의 UI 자동 조작을 대체하지 않습니다.

size-change 직전 cached measurement는 item0/1/2의 20/20/20입니다. 변경 뒤 30/20/20이고 미측정 9개는 추정값 20이므로 70+9×20=250에 gap22를 더해 range=272입니다(실제 합과 같음).

## 단계 구성

- `adapter-lifecycle`의 각 step은 RecyclerView를 120×50 stage에 부착한 뒤 adapter 통지 + 명시 invalidate로 controller pass를 일으키고 fence 뒤 검사합니다. `replace-clear`는 `replace`(adapter 교체: count 2, range 42) → `clear`(adapter 제거)의 두 step으로 나뉘며 item rect는 렌더 rect입니다.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 3 scenario, 26 action, 344 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `adapter-lifecycle` | `initial` : 26 → `content-only` : 14 → `insert` : 9 → `move` : 9 → `remove` : 9 → `size-change` : 9 → `full-data-empty` : 4 → `replace` : 2 → `clear` : 2 |
| `window-viewport` | `attach` : 8 → `resize` : 7 |
| `window-scroll-focus` | `attach` : 18 → `position-middle` : 13 → `position-end-clamp` : 13 → `position-start` : 13 → `offset-low-clamp` : 13 → `offset-high-clamp` : 13 → `focus-end` : 18 → `focus-start` : 18 → `focus-already-visible` : 18 → `peek-end` : 18 → `peek-start` : 18 → `peek-head-clamp` : 18 → `peek-tail-clamp` : 18 → `negative-peek-normalize` : 18 → `focus-scroll-disabled` : 18 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC148 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
