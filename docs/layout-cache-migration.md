# Layout Cache Migration Notes

이 문서는 Measure/Arrange 캐시 도입으로 바뀐 **관찰 가능한 동작**과, 기존 코드에서
확인해야 할 항목을 정리한다. 캐시의 설계와 계약 전문은
[layout-structure.md](layout-structure.md)의 "Layout caching and the
measure/arrange contract" 및 "Invalidation" 절에 있다.

이 문서에서 **producer**는 measure/arrange 구현을 가리키는 약칭이다. 즉
`OnMeasure` / `OnArrange` override, attach된 `LayoutManager`, 그리고
`MeasureCallback` / `ArrangeCallback`이다.

원칙은 하나다. **캐시는 계산을 생략할 뿐 결과를 바꾸지 않는다.** 아래 목록에서
"결과가 달라진다"고 표시된 항목은 모두 기존 버그의 수정이며, 그 외에는 layout이
적용된 뒤의 Position/Size가 이전과 동일하다.

---

## 1. 반드시 확인해야 할 항목

### 1.1 `LayoutManager` 파생 클래스가 자기 상태를 가지고 있는가

manager에 보관된 상태(orientation, spacing, 정의 테이블 등)는 owner의 measure
캐시 키에도 arrange 캐시 키에도 들어가지 않는다. 해당 상태를 바꾸는 setter는
반드시 다음 중 하나를 호출해야 한다.

```cpp
void MyManager::SetGap(float gap)
{
  if(mGap == gap) { return; }   // 같은 값이면 아무것도 예약하지 않는다
  mGap = gap;
  InvalidateOwnerMeasure();     // 배치만 바뀐다면 InvalidateOwnerArrange()
}
```

`InvalidateOwnerMeasure()` / `InvalidateOwnerArrange()`는 `LayoutManager`의
protected 멤버로 새로 추가되었다. attach 전에 호출해도 안전하다(no-op). 다만
**setter에서만** 호출해야 하며, manager 자신의 `Measure()` / `Arrange()` producer
안에서 호출하면 계약 위반으로 경고 후 무시된다(§1.4).

호출이 없는 setter가 실제로 잃는 것:

- **Measure가 읽는 상태**: measure 캐시는 무조건 동작하므로, 무효화 없는 변경은
  이 view에 무효화가 닿거나 이 view의 키 입력(정규화된 constraint, 유효
  스케일)이 바뀌기 전까지 반영되지 않는다. 무관한 pass가 도는 것만으로는
  회복되지 않는다. 조상이 miss해도 조상 구현이 이 view를 같은 입력으로 다시
  측정하므로 이 view는 여전히 hit하고, 형제의 무효화는 위로만 전파되어 이
  view에 닿지 않는다. 이 위험은 이전부터 있었고 이제 계약으로 명문화되었다.
- **Arrange만 읽는 상태**: 기본 정책인 `ArrangePolicy::IF_CHANGED`에서는 이전
  결과가 재사용될 수 있으므로 무효화 없는 변경이 반영되지 않을 수 있다.
  `ArrangePolicy::ALWAYS`도
  pass 자체를 예약하지는 않으므로 setter의 무효화 호출은 어느 정책에서나 필요하다.

라이브러리 내장 manager(Stack/Grid/Flex)는 이번 변경으로 모두 배선되어, **직접
setter 호출만으로** 재배치가 예약되고 반영된다. 이전에는 이 직접 호출이
unspecified였다(아무것도 pass를 예약하지 않음).

### 1.2 커스텀 arrange producer의 실행 정책을 확인한다

arrange producer의 기본값은 `ArrangePolicy::IF_CHANGED`다. 이전 결과를
재사용할 수 있으면 `OnArrange()`, 1인자 `SetArrangeCallback(callback)`, 그리고
`LayoutManager::Arrange()`가 호출되지 않을 수 있다.

다음과 같이 layout invalidation이 추적하지 않는 상태를 읽거나 매 pass 외부 작업을
수행해야 하는 producer만 명시적으로 opt-out한다.

```cpp
// ViewImpl 파생 클래스
MyViewImpl::MyViewImpl()
{
  SetArrangePolicy(ArrangePolicy::ALWAYS);
}

// 콜백
view.SetArrangeCallback(ArrangeCallback::New(&MyArrange),
                        ArrangePolicy::ALWAYS);

// LayoutManager 파생 클래스
MyManager::MyManager()
{
  SetArrangePolicy(ArrangePolicy::ALWAYS);
}
```

`ArrangePolicy::ALWAYS`가 필요한 경우:

- 조상/월드 좌표(`SCREEN_POSITION`, `WORLD_POSITION`, `WORLD_SCALE`, 윈도우 좌표)를
  읽는다
- actor 트리 밖의 표면(네이티브 플레이어, 웹 엔진 등)에 상태를 밀어 넣는다
- 입력이 같아도 배치하는 자식 집합이나 외부 작업이 달라진다
- `InvalidateArrange()`가 따라붙지 않는 상태에 의존한다

정책은 구현 인스턴스에 저장되고 파생 클래스에도 상속된다. 파생 클래스는 생성자에서
다시 정책을 설정할 수 있다. 기존 1인자 `SetArrangeCallback(callback)`은 이제
`ArrangePolicy::IF_CHANGED`를 사용하므로, callback 호출 횟수나 외부 부수 효과에
의존하던 코드는 2인자 overload로 `ArrangePolicy::ALWAYS`를 지정해야 한다.

### 1.3 measure producer가 매 프레임 호출된다고 가정하고 있지 않은가

measure 캐시는 이전부터 **무조건** 동작했으므로 이 위험 자체는 새로 생긴 것이
아니다. 이번 변경으로 달라진 것은 캐시가 **언제 비워지는지**다. 누락되어 있던
무효화가 메워지고 조상 캐시를 정리하는 경로가 새로 들어오면서, measure producer는
이전보다 **더 자주** 호출된다(적중률은 올라가지 않고 내려간다). 대신 우연한
무효화에 기대어 producer가 다시 호출되던 코드는 그 보장을 잃는다. `OnMeasure` /
`MeasureCallback` / `LayoutManager::Measure`를 per-frame tick으로 쓰고 있었다면
지금 드러난다. producer 밖의 상태를 읽는다면 그 상태를 바꾸는 쪽에서
`InvalidateMeasure()`를 호출해야 한다. 단, 그 호출은 **producer 안이 아니라 이벤트
시점**(pass 이전/이후)에 이루어져야 한다. 자세한 내용은 §1.4를 참고한다.

### 1.4 레이아웃 처리 중에 무효화를 호출하고 있지 않은가

**레이아웃 처리 창(layout processing window)** 은 Measure/Arrange pass가 스택에 있는
동안, 그리고 `LayoutFinished` emit이 진행 중인 동안 열려 있다. 이 창이 열려 있을 때
공개 진입점 `View::InvalidateMeasure()` / `View::InvalidateArrange()` /
`LayoutController::RequestLayout()`를 직접 호출하는 것은 계약 위반이며 View당 한 번
로그로 경고된다(`DALI_LOG_ERROR`, View에 latch되므로 로그 폭주 없음). 하지만 경고는
요청이 폐기됐다는 뜻이 아니다.

- 무효화는 **전부 보존**된다. 관련 캐시 유효성이 철회되고 dirty가 기록되며, 조상
  chain의 상태가 갱신되고 layout root가 `LayoutController`의 pending set에 남는다.
- 진행 중 producer가 이미 소비한 상태가 있으면 그 pass의 캐시 publish도 차단된다.
- 단, 처리 창 안에서 생긴 pending work는 **PARK**되어 스스로 idle
  `ProcessEvents` wake를 요청하지 않는다.

현재 layout batch에 해당 root의 아직 시작하지 않은 turn이 이미 들어 있다면 그 turn이
pending 상태를 바로 소비할 수 있다. 현재 batch가 소비하지 못하고 끝난 작업만 자체
wake 없이 PARK된 채 다음 processing 기회를 기다린다.

레이아웃 처리 도중의 무효화는 매 프레임 레이아웃 펌프를 다시 무장시켜 메인 루프가
idle로 진입하지 못하게 만들 수 있기 때문이다. PARK된 작업은 이후 독립적으로 발생한
`ProcessEvents` 또는 명시적 `LayoutController::ProcessLayouts()`가 실행되면 처리된다.
처리 창 밖에서 들어온 event-time 요청은 이미 PARK된 root까지 포함해 controller에
**한 개의 coalesced outstanding wake**만 무장시킨다.

Layout transition lifecycle 콜백은 Measure/Arrange pass 이후이자 이 창 밖에서
실행되므로, 기존에 문서화된 mutation과 transition chaining 경로는 wakeable하다.

이 정책은 public/internal 호출 경로를 구분하지 않는다. property setter, resource 경로,
`LayoutFinished` 슬롯에서의 트리 변형(`Add()` / `Remove()`)도 full invalidation과 root
pending은 유지하지만 자체 idle wake는 만들지 않는다. 빠른 후속 레이아웃이 필요하다면
상태 변경과 무효화를 event 시점으로 옮기거나 별도의 idle callback/timer를 예약해야 한다.

캐시 유효성이 철회된다는 것은 그 entry가 이후 cache hit에 사용되지 않는다는 뜻이다.
즉시 재계산된다는 뜻은 아니므로, parked work가 drain되기 전까지 `GetMeasuredSize()`나
actor bounds에는 마지막으로 완료된 pass의 geometry가 계속 보일 수 있다.

**계약을 명시하면 다음과 같다.** 레이아웃 처리 중의 무효화는 **원칙적으로 금지**이며,
dali-core의 relayout 정책(처리 중 `RequestRelayout()`은 보존되지만 wake를 만들지
않음)과 동일하게 **best-effort로만 지원**된다. PARK된 작업은 다음에 외부 요인으로
발생하는 `ProcessEvents`에서 처리되는데, 입력·애니메이션·타이머가 전혀 없는 정지
상태 앱에서는 그 시점이 **무기한 뒤**일 수 있고 `LayoutFinished`도 그때까지 함께
보류된다. 따라서 컴포넌트와 앱은 **현재 프레임의 정확성을 in-pass 무효화에 의존해서는
안 된다**. 처리 프레임이 wake 없이 parked work를 남기고 끝나면 controller가 에피소드당
한 번 `DALI_LOG_ERROR`를 남긴다(공개 API 위반 경고가 볼 수 없는 framework 내부 경로
기인 파킹까지 포함; pending set이 비워지면 latch가 풀려 다음 에피소드에 다시 1회
기록된다).

---

## 2. 결과가 달라지는 항목 (버그 수정)

### 2.1 CheckBox의 RIGHT_TO_LEFT 배치

`CheckBox`는 자체적으로 한 번 미러링하고 프레임워크가 다시 한 번 미러링해서, 결과가
서로 상쇄되어 **RIGHT_TO_LEFT에서도 LEFT_TO_RIGHT처럼** 그려지고 있었다. 자체
미러링을 제거해 이제 아이콘이 오른쪽 가장자리에서 `padding.start`만큼 떨어진 곳에
놓인다(= "start"의 정의).

RTL 화면에서 CheckBox를 쓰고 있었다면 스크린샷 기준 테스트를 갱신해야 한다.

### 2.2 자식 `POSITION_X`를 직접 쓴 뒤의 RTL 미러링

RTL 미러링은 이제 자식의 **논리(logical) arranged bounds**에서 계산된다. 이전에는
자식 actor의 현재 `POSITION_X`를 다시 반전하는 방식이었고, 그것은 멱등이 아니라
같은 값에 두 번 적용하면 원래대로 돌아갔다.

결과 차이는 하나뿐이다. 부모가 자식을 arrange한 **뒤에** 그 자식의 `POSITION_X`를
바깥에서 덮어썼다면, RTL에서 그 덮어쓴 값이 더 이상 미러링 입력이 되지 않는다.
논리 좌표로 배치하고 방향 처리는 프레임워크에 맡기면 된다.

아직 한 번도 arrange된 적 없는 자식(= 논리 bounds가 없는 자식)은 이제 미러링
대상에서 제외되어 **그대로 두어진다**. 이전에는 actor의 현재 물리 좌표를 다시
반전하는 방식이라 같은 입력의 반복 pass에서 좌표가 좌우로 진동했는데(멱등이
아님), 프레임워크가 소유한 논리 좌표가 없는 자식을 건드리지 않는 것이 결정적인
동작이다. 자식을 직접 배치하고 arrange하지 않는 producer가 RTL을 원한다면
producer 자신이 방향을 처리해야 한다.

### 2.3 그 외 무효화 누락 수정

다음은 모두 "원래 갱신되었어야 하는데 갱신되지 않던" 경우다. 화면이 늦게 갱신되던
증상이 사라지는 방향이다.

- pass 도중 발생한 재무효화가 pass 종료 시 덮여 사라지던 문제
- 부모가 arrange하지 않는 자식의 재무효화가 영구히 삼켜지던 문제
- 자식 제거 / 오프스크린 루트 재연결 시 subtree의 effective scale이 낡은 값으로
  남던 문제(스케일이 섞여 보이던 증상)
- resource-ready / background visual 등록·교체·해제 후 natural size가 반영되지
  않던 문제
- fitting mode 갱신 요청이 처리 중에 다시 들어오면 유실되던 문제
- `RaiseToTop` / `LowerBelow` 등 child order 변경이 일부 View에서 누락되던 문제
- 레이아웃 방향 변경이 아무 재배치도 일으키지 않던 문제

---

## 3. 새로 추가된 API (전부 additive)

| API | 위치 | 용도 |
|---|---|---|
| `ArrangePolicy` | `layout-types.h` | arrange producer의 실행 정책 |
| `View::SetArrangeCallback(cb, policy)` | `view.h` | callback 실행 정책 지정 |
| `ViewImpl::SetArrangePolicy(policy)` | `view-impl.h` (protected) | `OnArrange` 실행 정책 지정 |
| `ViewImpl::GetArrangePolicy()` | `view-impl.h` (protected) | `OnArrange` 실행 정책 조회(설정값 미러) |
| `LayoutManager::SetArrangePolicy(policy)` | `layout-manager.h` (protected) | manager arrange 실행 정책 지정 |
| `LayoutManager::InvalidateOwnerMeasure()` | `layout-manager.h` (protected) | manager 자기 상태 변경 시 owner 무효화 |
| `LayoutManager::InvalidateOwnerArrange()` | `layout-manager.h` (protected) | 위와 같되 배치 축만 |

가상 함수는 추가되지 않았고, 공개 시그니처에 표준 라이브러리 타입은 사용하지
않았다.

---

## 4. 참고: 동작이 바뀌지 않는 것

- clean 상태에서 producer가 호출되지 않는 것은 캐시의 정상 동작이며, 그때도 actor
  geometry는 매 pass 재조정된다. 레이아웃 밖에서 덮어쓴 좌표는 캐시 적중 시에도
  똑같이 복구된다.
- `LayoutFinishedSignal()`은 pass 기준이다. 캐시 적중으로 처리된 View도 통지를
  받으며, 이는 "bounds가 바뀌었다"는 신호가 아니다.
- 무효화 전파의 coalescing은 ancestor chain을 얼마나 자주 걷는지만 바꾼다. 한 pass
  전에 쌓인 무효화는 전부 그 pass에서 처리된다.
