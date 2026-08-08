# Focus Navigation 리팩토링 결정 기록

## 상태와 범위

- 상태: 구현 결정 승인
- 대상: `dali-ui-foundation` focus navigation API/구현, automated tests,
  Wiki, `samples/focus-navigation`
- 배경: `dali-toolkit`의 application 전역 custom algorithm 사용성을
  `dali-ui`의 View/container 중심 모델 안에서 제공

이 문서는 설계 논의에서 검토한 선택지와 최종 판단 근거를 보존한다. Focus 자동
복원, scene 연결 시 자동 focus, navigation cluster는 이번 범위에 포함하지 않는다.

## 기존 구조와 확인된 공백

기존 `dali-ui`의 탐색 순서는 parent View policy, 명시적 방향 target, 기본
`FocusFinder`였다. View callback과 virtual override는 빈 `View`로 "처리하지 않음"을
표현했기 때문에 다음 세 상태를 구분할 수 없었다.

1. 이 policy가 처리하지 않았으므로 다음 policy로 진행
2. 특정 candidate로 이동
3. 요청은 처리했지만 현재 focus 유지

또한 current focus가 없으면 parent chain과 명시 target이 존재하지 않아 application
code가 첫 focus를 선택할 방법이 없었다. Focus Group containment도 기본 finder에만
적용되어 local policy나 명시 target이 경계를 벗어날 수 있었고, Focus Group View
자체 policy는 parent traversal에서 호출되지 않았다.

기존 Toolkit의 `CustomAlgorithmInterface::GetNextFocusableActor(current, proposed,
direction)`에 대응하는 application-level 지점이 필요하되, raw pointer와 모호한 빈
handle 의미는 그대로 복제하지 않는 것으로 결정했다. Toolkit 경로에서 `proposed`는
실질적으로 비어 전달되므로 새 API에 유지할 가치가 없다고 판단했다.

## 최종 public model

### FocusNavigationResult

모든 navigation policy는 다음 중 하나를 반환한다.

- `NotHandled()`: 다음 parent 또는 낮은 우선순위 policy로 진행
- `MoveTo(view)`: 탐색 종료 후 candidate 검증, `RequestFocus()` resolve, commit 시도
- `Stay()`: 탐색 종료, focus 유지, `MoveFocus()`는 이동하지 않았으므로 `false`

`MoveTo()`의 candidate가 비었거나 연결되지 않았거나 Window/Focus Group 경계를
위반하면 요청은 실패한다. 이미 처리한 결정을 다른 algorithm으로 대체하지 않도록
낮은 우선순위로 fallback하지 않는다. 명시 target이 설정됐지만 resolve할 수 없는
경우도 같은 원칙을 적용한다.

### FocusNavigationContext

Context는 public data struct가 아니라 `BaseHandle` 기반 immutable object다. Public
factory를 두지 않고 `FocusManager`가 요청마다 한 번 생성하여 모든 policy에 같은
context를 전달한다. 이 구조는 향후 getter 추가 시 callback signature와 handle 크기를
바꾸지 않으며 DALi binding 관례에도 맞는다.

제공 정보:

- `GetDirection()`
- `GetDevice()` / `GetDeviceName()`
- `GetInputEvent()`
- `GetWindow()`
- `GetFocusGroup()`

Window를 manager callback의 별도 인자로 중복 전달하지 않고 context에 통합했다.
따라서 View-level과 manager-level callback이 다음 한 가지 signature를 공유한다.

```cpp
FocusNavigationResult GetNextFocusableView(
  View current,
  FocusNavigationContext context);
```

### View-level customization

`FocusNavigationCallback`과 `ViewImpl::OnFocusNavigationRequested()`는 위 signature를
사용한다. 한 View 인스턴스에 callback이 있으면 callback이 virtual override보다
우선하며 virtual은 호출되지 않는다. 빈 callback 설정은 virtual 경로를 복원한다.

Callback은 move-only callback object를 보관하지만 member-function target object의
lifetime을 연장하지 않는다. Target이 파괴되기 전에 callback을 교체하거나 해제해야
한다.

### Application fallback

최종 이름은 `FocusManager::SetFocusNavigationFallback()`이다. DALi setter 이름에
`Callback` suffix가 필수적인 관례는 없고, 매개변수 타입과 설명으로 callback임이
충분히 드러난다. `Default`라는 이름은 framework `FocusFinder`와 혼동되므로 쓰지
않는다.

- FocusManager가 하나의 callback을 저장
- 새 callback은 기존 callback 교체
- 빈 callback은 해제
- navigation Window를 결정할 수 있으면 current focus 유무와 무관하게 호출
- current focus가 없으면 application fallback을 첫 policy로 호출
- default algorithm 활성화 여부와 독립적으로 호출
- `NotHandled()`이며 default algorithm이 켜진 경우에만 Finder로 진행

이는 기존 Toolkit algorithm을 새 모델에 맞게 재설계한 layer다. 앱은 과거처럼
`GetNextFocusableView` 성격의 algorithm class/member method를 만들고 해당 method를
callback으로 등록할 수 있다. 실행 가능한 예제는 `samples/focus-navigation`에 둔다.

Signal은 여러 listener 중 누가 navigation 결정을 소유하는지 불명확하고 반환값
결합 규칙이 부적합하므로 사용하지 않는다.

## 탐색 순서와 경계

Current focus가 있을 때:

1. 가장 가까운 parent View의 local policy
2. 바깥 parent들의 local policy
3. 가장 가까운 Focus Group View 자체의 local policy
4. 명시적 방향 target
5. FocusManager application fallback
6. framework `FocusFinder`

Current focus가 없을 때:

1. FocusManager application fallback
2. fallback이 `NotHandled()`이고 default algorithm이 켜져 있으면 `FocusFinder`

Window는 key/wheel event의 source `SceneHolder`에서 보존한다. Programmatic
`MoveFocus()`는 current focus의 Window, 그 다음 마지막 focused Window를 사용한다.
어느 것도 결정할 수 없다면 callback을 잘못된 scope로 호출하지 않고 요청을
실패시킨다.

Navigation으로 선택된 candidate와 `RequestFocus()`로 resolve된 최종 View 모두
요청 Window에 연결되어야 한다. Current focus가 Focus Group 안에 있으면 둘 모두
가장 가까운 Focus Group subtree 안에 있어야 한다. 중첩 group에서는 가장 가까운
group이 경계다.

Focus Group은 user navigation trap이지 programmatic focus 권한 경계가 아니다.
따라서 `RequestFocus(outside)`와 `SetCurrentFocusView(outside)`는 허용한다. Popup
종료 후 focus 복원도 이 명시적 경로를 사용하며 별도 `ExitFocusGroup()`은 추가하지
않는다.

## Callback 실행 계약

Policy callback은 candidate를 계산하여 결과만 반환한다. Callback 실행 중 다음
mutation은 거부한다.

- nested `MoveFocus()`
- `RequestFocus()` / `SetCurrentFocusView()`
- 실행 중인 manager fallback 교체 또는 해제

Callback 동안 외부 요인으로 current focus가 바뀌었거나 candidate tree가 바뀌면
snapshot과 최종 상태를 재검증하여 stale result를 commit하지 않는다. 이 계약은
재진입으로 인한 중첩 signal, callback self-destruction, 잘못된 Window commit을
방지한다.

## API 선택 가이드

1. 고정된 한두 View 관계: 방향별 focusable View setter
2. 기존 container 인스턴스에만 적용: `SetFocusNavigationCallback()`
3. Custom View 종류가 소유: `OnFocusNavigationRequested()` override
4. 앱 전체 공통 rule 또는 첫 focus: `SetFocusNavigationFallback()`
5. 별도 rule이 없는 일반 탐색: framework `FocusFinder`

`OnFocusRequested()`는 navigation policy가 아니다. `MoveTo(container)` 이후 실제
focusable self/descendant를 resolve하는 hook이다.

## Toolkit migration

- Component/화면 영역 rule은 owning container policy로 이동
- App 전역 rule은 manager fallback으로 이동
- 첫 navigation은 같은 fallback에서 빈 `current`를 검사
- 기본 탐색 중단은 `Stay()`
- Framework에 위임은 `NotHandled()`
- Window별 분기는 `context.GetWindow()`

별도 `FocusEntryCallback`, navigation root 등록, window별 callback registry는 현재
fallback 하나로 요구를 충족하므로 추가하지 않는다.

## 보류 및 향후 확장

Focus 자동 복원은 navigation algorithm과 다른 state/lifecycle 문제다. 향후 요구가
생기면 FocusManager impl에 Window/Focus Group별 history 또는 restoration policy를
별도 추가할 수 있다. 현재 callback signature는 immutable context handle이므로 필요
시 restoration 관련 getter를 확장할 수 있고, 이번 pipeline을 다시 깨뜨릴 필요가
없다. 다만 history의 저장 시점, View 파괴 처리, group 제거 semantics는 별도 설계가
필요하다.

이번 범위에서 제외:

- `focusedByDefault`와 scene 연결 시 자동 focus
- 자동 focus 복원
- navigation root 및 별도 entry callback
- Focus Group 간 Tab 이동과 navigation cluster
- Toolkit `PreFocusChangeSignal` 호환 layer
- window별 callback 등록 API

## 성능과 메모리 판단

메이저한 저하는 예상하지 않는다.

- Navigation request마다 작은 ref-counted context object 1개를 할당한다. 입력
  navigation 빈도에서는 layout/render 비용에 비해 작지만, 기존 대비 명확한 allocation
  1회와 handle ref/unref 비용은 추가된다.
- Candidate 검증은 parent chain과 Focus Group subtree membership을 확인하므로 depth에
  비례하는 짧은 traversal이 최대 두 번 추가된다.
- View별 callback storage는 기존 optional data를 계속 사용하므로 callback을 쓰지
  않는 View의 per-instance memory 증가는 없다.
- FocusManager에는 callback object와 reentrancy flag가 하나 추가된다.
- `FocusNavigationResult`는 callback 반환 시 작은 value와 handle copy 비용을 가진다.

따라서 극단적으로 깊은 tree에서 매우 높은 빈도로 navigation을 합성하는 경우만
micro-benchmark 대상이며 일반 key/wheel navigation에서 체감 가능한 regression은
예상하지 않는다.

## Public API 변화 카운트

추가 API는 요청한 카운트에서 제외한다. 기존 API 기준:

- 변경: 4개
  1. `FocusNavigationCallback` callback signature
  2. `View::SetFocusNavigationCallback()`의 실질 parameter type
  3. `ViewImpl::SetFocusNavigationCallback()`의 실질 parameter type
  4. `ViewImpl::OnFocusNavigationRequested()` signature/return type
- 삭제: 0개

추가되는 API는 `FocusNavigationResultType`, `FocusNavigationResult`,
`FocusNavigationContext`, `FocusManager::SetFocusNavigationFallback()` 및 각 type의
factory/getter다. 이 변경은 pre-release refactoring 정책에 따라 source/ABI migration을
수용하며 compatibility alias나 legacy virtual을 병존시키지 않는다.

## Patch 구성 제안

하나의 PR 안에서 의존 순서가 보이는 세 commit을 권장한다.

1. Result/Context 및 View policy signature 변경
2. FocusManager fallback, containment, reentrancy와 automated tests
3. Sample, API comments, Wiki, 이 결정 기록

세 부분은 하나의 public contract를 구성하므로 별도 PR로 나누면 중간 commit/PR에서
build 또는 문서 계약이 어긋날 가능성이 있다. 리뷰가 단일 patch를 선호하면 squash할
수 있지만 개발 과정의 논리적 세 commit은 유지하는 편이 검토에 유리하다.

## 검증 기준

- Result 세 상태와 invalid `MoveTo()`
- callback-over-virtual 및 inner-to-outer propagation
- Focus Group View policy 포함, 바깥 policy 제외
- local/explicit/fallback/Finder 우선순위
- source/current/last-focused Window별 current focus 없는 fallback entry와 Window 없는 실패
- default algorithm과 fallback 독립성
- callback 교체/해제 및 재진입 거부
- Window, direction, device, device name, InputEvent, Focus Group context
- policy/explicit/fallback/Finder candidate containment
- programmatic Focus Group escape
- sample과 foundation/automated-test build
