# Trait 저장소의 BaseObject 일반화 제안

## 상태와 범위

- 상태: TODO / 설계 제안, 미구현
- 대상: `dali-ui-foundation`의 View trait 저장소와 integration API
- 주요 대상 파일:
  - `dali-ui-foundation/internal/views/view/view-data-impl.h`
  - `dali-ui-foundation/internal/views/view/view-data-impl.cpp`
  - `dali-ui-foundation/integration-api/view-integ.h`
  - `dali-ui-foundation/integration-api/view-integ.cpp`
- 배경: View를 상속할 수 없는 사용처에 per-instance callback 같은 선택적 동작을
  제공할 때, 모든 payload가 lifecycle hook을 가진 `TraitObject`일 필요는 없다.

이 문서는 View의 trait slot이 저장할 수 있는 최소 타입을 `TraitObject`에서
`BaseObject`로 넓히는 방안과, typed trait 조회 과정에서 생길 수 있는 반복적인 RTTI
비용을 피하기 위한 `GetTraitStaticCast<T>()` 방안을 기록한다.

## 현재 구조

현재 trait entry는 다음 형태다.

```cpp
using TraitEntries =
  std::vector<std::pair<TraitId, IntrusivePtr<TraitObject>>>;
```

`SetTrait()`, `GetTrait()`도 `IntrusivePtr<TraitObject>`를 사용한다. 따라서 View에
붙이는 모든 ref-counted payload는 lifecycle hook이 필요하지 않더라도
`TraitObject`를 상속해야 한다.

현재 `ViewDataImpl`은 모든 trait에 대해 다음 hook을 호출한다.

- attach 후 `OnAttached()`
- 교체 또는 제거 전 `OnDetaching()`
- View 파괴 중 `OnViewDestroying()`

기본 `TraitObject` 구현은 아무 작업도 하지 않으므로, lifecycle이 필요 없는 객체도
빈 virtual hook 호출과 `TraitObject` 타입 의존성을 가진다.

## 확인된 lifecycle hook 사용자

현재 production 코드에서 hook을 실제 override하여 사용하는 객체는 다음 세 종류다.

1. `CoreInteractionObject`
   - attach 시 interactive/selectable/group-selectable 구현에 owner View 전달
   - View 파괴 시 하위 interaction 구현의 owner 및 group 관계 정리
   - core interaction trait 자체는 정상 경로에서 교체하거나 제거할 수 없음
2. `StateHandlerTrait`
   - attach 시 owner 저장 및 `StateChangedSignal` 연결
   - detach/View 파괴 시 signal, callback, owner 정리
3. `OverlayEffectImpl`
   - detach 시 overlay와 recoil 상태 정리
   - View 파괴 시 effect attachment 정리
   - 현재 `OnAttached()` 구현은 비어 있음

다음 객체는 `TraitObject`를 상속하지만 lifecycle hook을 override하지 않는다.

- `LayoutCallbacksObject`
- `LayoutManagerObject`
- `AbsoluteLayoutParamsImpl`
- `FlexLayoutParamsImpl`
- `GridLayoutParamsImpl`
- `StackLayoutParamsImpl`
- `StateEffectTargetTraitImpl`
- `StateEffectImpl`
- `NoneStateEffectImpl`

현재 accessibility activation callback holder는 기존 trait 계약을 유지하기 위해
얇은 `TraitObject` wrapper로 구현한다. 이 객체도 lifecycle hook을 사용하지 않으므로
이 제안이 구현되면 `BaseObject`로 단순화할 수 있는 후속 정리 대상이다.

## 제안 1: trait 저장 타입을 BaseObject로 일반화

Trait entry와 integration API의 최소 저장 타입을 다음과 같이 변경한다.

```cpp
using TraitEntries =
  std::vector<std::pair<TraitId, IntrusivePtr<BaseObject>>>;
```

```cpp
void SetTrait(
  ViewImpl& viewImpl,
  TraitId id,
  IntrusivePtr<BaseObject> object);

IntrusivePtr<BaseObject> GetTrait(
  const ViewImpl& viewImpl,
  TraitId id);
```

역할은 다음처럼 구분한다.

- `BaseObject`: trait slot에 저장할 수 있는 최소 타입
- `TraitObject : BaseObject`: View lifecycle hook이 필요한 객체가 선택적으로 상속하는
  타입
- `TraitId`: slot의 identity. ID와 실제 payload 타입의 대응 관계는 등록자와 조회자의
  계약
- attachment: `UniqueAny` 기반의 View 소유 임의 데이터 저장소. ref-counted object
  slot과는 별도 용도로 유지

`BaseHandle`을 저장 타입으로 사용하지 않는다. `IntrusivePtr<BaseObject>`를 사용하면
public handle/body가 필요 없는 내부 payload도 불필요한 handle wrapper 없이 저장할 수
있다.

## 선택적 lifecycle dispatch

저장 객체가 `TraitObject`인 경우에만 lifecycle hook을 호출한다.

```cpp
TraitObject* GetTraitLifecycleObject(BaseObject* object)
{
  return object ? dynamic_cast<TraitObject*>(object) : nullptr;
}
```

### Attach와 교체

```cpp
if(auto* lifecycle = GetTraitLifecycleObject(object.Get()))
{
  lifecycle->OnAttached(id, self);
}
```

기존 객체를 교체할 때는 이전 `IntrusivePtr<BaseObject>`를 hook 호출이 끝날 때까지
지역 변수로 유지한다. `OnDetaching()` 실행 중 entry가 바뀌거나 마지막 외부 참조가
사라져도 실행 중인 객체가 파괴되지 않아야 한다.

### Remove

entry를 제거하기 전에 저장 객체가 `TraitObject`인지 확인하고 `OnDetaching()`을
호출한다. hook 호출이 끝난 뒤 entry의 strong reference를 해제한다.

### View destruction

View 파괴 중 각 entry에 대해 `TraitObject` 여부를 확인하고
`OnViewDestroying()`을 호출한다. 일반 `BaseObject` payload는 별도 hook 없이 strong
reference만 해제한다.

`CoreInteractionObject`처럼 특정 reserved ID에 구체 타입이 반드시 필요한 기존
검증은 유지한다. 이 검증은 lifecycle 여부 검사와 다른 ID-specific invariant다.

## RTTI 비용의 범위

`BaseObject` 저장소에서 optional lifecycle을 판별하려면
`dynamic_cast<TraitObject*>`가 필요하다. 이 검사는 다음 lifecycle 경계에만 둔다.

- `SetTrait()`
- trait 교체
- `RemoveTrait()`
- View 파괴

이 경로들은 layout measure/arrange, rendering, input dispatch처럼 매 frame 또는 입력
샘플마다 반복되는 hot path가 아니다. View 하나에 붙는 trait 수도 일반적으로 적기
때문에 초기 구현에서는 lifecycle 포인터를 별도로 캐시하지 않는다.

프로파일링 결과 lifecycle RTTI가 문제가 될 경우에만 다음과 같은 entry 구조를
검토한다.

```cpp
struct TraitEntry
{
  TraitId id;
  IntrusivePtr<BaseObject> object;
  TraitObject* lifecycleObject;
};
```

이 구조는 등록 시 RTTI를 한 번만 수행하지만 trait entry마다 포인터 하나만큼 메모리가
증가한다. 확인된 사용 빈도와 trait 개수를 고려하면 사전 적용하지 않는다.

## 현재 typed 조회의 RTTI 사용

현재 코드도 `GetTrait()` 후 구체 타입을 얻기 위해 여러 방식의 cast를 사용한다.

- `LayoutCallbacksObject`: 항상 `dynamic_cast`
- 네 종류 layout params의 `TryGetLayoutParams()`: 항상 `dynamic_cast` 후
  `DALI_ASSERT_ALWAYS`
- `StateHandlerTrait`: 항상 `dynamic_cast`
- `StateEffect`, `StateEffectTargetTrait`: handle `DownCast()` 내부에서
  `dynamic_cast`
- layout params의 internal `Get()`: debug에서 `dynamic_cast`로 검증하고 release에서
  `static_cast`
- `LayoutManagerObject`: ID와 타입 대응을 신뢰하고 `static_cast`

저장 타입을 `BaseObject`로 넓히는 것 자체보다, typed 조회마다 무조건 RTTI를
사용하도록 API를 설계할 때 반복 비용과 코드 중복이 생길 수 있다.

## 제안 2: GetTraitStaticCast 제공

호출자가 `TraitId`와 payload 타입의 대응을 보장할 수 있는 경우를 위해 명시적인
unchecked typed getter를 제공한다.

```cpp
template<typename ObjectType>
IntrusivePtr<ObjectType> GetTraitStaticCast(
  const ViewImpl& viewImpl,
  TraitId id)
{
  static_assert(std::is_base_of_v<BaseObject, ObjectType>);

  IntrusivePtr<BaseObject> object = GetTrait(viewImpl, id);
  return IntrusivePtr<ObjectType>(
    object ? static_cast<ObjectType*>(object.Get()) : nullptr);
}
```

함수명에 `StaticCast`를 포함하여 checked lookup이 아니며 호출자가 타입 안전성을
보장해야 한다는 점을 호출부에 드러낸다.

```cpp
auto callbacks =
  Integration::View::GetTraitStaticCast<LayoutCallbacksObject>(
    viewImpl,
    ReservedTraitId::LAYOUT_SIGNALS);
```

### 호출 계약

`GetTraitStaticCast<ObjectType>()`에는 다음 precondition을 둔다.

> 해당 `TraitId`에 객체가 저장되어 있다면 그 객체의 dynamic type은
> `ObjectType`이거나 그 파생 타입이어야 한다. 이 조건을 위반하면 동작은 정의되지
> 않는다.

다음 조건을 만족하는 framework 내부 코드에서만 사용한다.

1. trait을 저장하는 모든 경로가 통제됨
2. 해당 ID에 저장되는 구체 타입이 하나로 합의됨
3. 다른 extension library가 같은 ID에 다른 타입을 넣을 수 없거나, 그렇게 하는 것이
   명백한 API 계약 위반임

custom `TraitId`, 외부 입력으로 결정되는 타입, 실패 가능한 probing에는 사용하지
않는다.

### Checked 조회와의 구분

불확실한 타입 조회가 필요한 경우에는 별도의 checked helper를 둘 수 있다.

```cpp
template<typename ObjectType>
IntrusivePtr<ObjectType> GetTraitDynamicCast(
  const ViewImpl& viewImpl,
  TraitId id)
{
  static_assert(std::is_base_of_v<BaseObject, ObjectType>);

  IntrusivePtr<BaseObject> object = GetTrait(viewImpl, id);
  return IntrusivePtr<ObjectType>(
    object ? dynamic_cast<ObjectType*>(object.Get()) : nullptr);
}
```

API별 의미는 다음과 같다.

- `GetTrait()`: `IntrusivePtr<BaseObject>` 반환, cast하지 않음
- `GetTraitDynamicCast<T>()`: checked lookup, 타입 불일치 시 빈 pointer 반환
- `GetTraitStaticCast<T>()`: unchecked lookup, 호출자가 타입 invariant 보장

`GetTraitAs<T>()`처럼 cast 방식이 드러나지 않는 이름은 사용하지 않는다.

## Debug 검증 정책

`GetTraitStaticCast<T>()` 안에서 debug-only `dynamic_cast` assertion을 수행할지는 구현
전에 결정해야 한다.

선택지 1은 순수 static cast다.

- 이름과 문서의 precondition으로 책임을 호출자에게 완전히 이전
- debug/release 모두 typed 조회 RTTI 없음
- 잘못된 ID/type 조합을 조기에 발견하지 못할 수 있음

선택지 2는 debug-only 검증 후 static cast다.

```cpp
DALI_ASSERT_DEBUG(
  !object || dynamic_cast<ObjectType*>(object.Get()));
```

- release hot path에는 RTTI 없음
- 개발과 테스트 중 계약 위반을 조기에 발견
- debug 성능 측정에서는 RTTI 비용이 포함됨

초기 권고안은 선택지 2다. 이 검증은 API의 runtime safety 보장이 아니라 개발 중
precondition 검증이며, release에서 호출자 책임이라는 계약은 동일하다.

## 적용 후보

저장 경로가 통제되는 다음 framework reserved trait은
`GetTraitStaticCast<T>()` 적용 후보로 검토한다.

- `LAYOUT_SIGNALS` / `LayoutCallbacksObject`
- `LAYOUT_MANAGER` / `LayoutManagerObject`
- `ABSOLUTE_LAYOUT_PARAMS` / `AbsoluteLayoutParamsImpl`
- `FLEX_LAYOUT_PARAMS` / `FlexLayoutParamsImpl`
- `GRID_LAYOUT_PARAMS` / `GridLayoutParamsImpl`
- `STACK_LAYOUT_PARAMS` / `StackLayoutParamsImpl`
- `STATE_HANDLER_TRAIT` / `StateHandlerTrait`
- `STATE_EFFECT` / `StateEffectImpl`
- `STATE_EFFECT_TARGET` / `StateEffectTargetTraitImpl`

각 후보는 실제 저장 경로와 integration API를 통한 외부 교체 가능성을 다시 확인한 뒤
전환한다. 단순히 reserved ID라는 이유만으로 unchecked getter를 사용하지 않는다.

public handle 반환이 필요한 `StateEffect`, `StateEffectTargetTrait`은 object용
`GetTraitStaticCast<T>()`와 별도로 handle 생성 helper가 필요할 수 있다. 기존
`GetKnownTraitHandle()`의 역할과 중복되지 않도록 정리한다.

## Accessibility activation callback과의 관계

현재 accessibility activation callback은 lifecycle이 필요 없는 얇은 `TraitObject`
wrapper에 저장한다. 이 변경이 적용되면 wrapper의 base class를 `BaseObject`로
단순화할 수 있다. View를 상속하지 못하는 사용처는 extension API로 callback을
등록하고, callback을 등록한 View만 trait entry와 callback payload 비용을 부담한다.

`ViewDataImpl`에 callback 멤버를 직접 추가하지 않으므로 callback을 사용하지 않는
View의 상시 메모리는 증가하지 않는다. 등록된 callback은 virtual
`OnAccessibilityActivate()`를 대체하며 callback의 반환값을 최종 action 결과로
사용한다. Dispatch가 wrapper의 strong reference를 실행 종료까지 유지하여 callback이
실행 중 자기 자신을 교체하거나 해제해도 안전해야 한다.

## API 및 ABI 영향

예상되는 integration API 변경:

- `SetTrait()` parameter를 `IntrusivePtr<TraitObject>`에서
  `IntrusivePtr<BaseObject>`로 변경
- `GetTrait()` return type을 `IntrusivePtr<TraitObject>`에서
  `IntrusivePtr<BaseObject>`로 변경
- `GetTraitStaticCast<T>()` template 추가
- 필요하면 `GetTraitDynamicCast<T>()` template 추가

이는 integration API의 source/API 변경이다. extension library는 재컴파일 및 일부
호출부 수정이 필요할 수 있다. `View` public handle의 크기와 `ViewDataImpl`의
per-instance 고정 멤버는 변경하지 않는다.

`TraitObject`를 즉시 삭제하지 않는다. lifecycle hook을 사용하는 기존 타입과 외부
extension 구현을 위한 선택적 base class로 유지한다.

## 구현 시 수정 범위

1. `TraitEntries`를 `IntrusivePtr<BaseObject>` 기반으로 변경
2. `ViewDataImpl::SetTrait()`, `GetTrait()`, `RemoveTrait()` signature와 구현 변경
3. integration `View::SetTrait()`, `GetTrait()` signature 변경
4. lifecycle hook 호출부에 optional `TraitObject` 판별 추가
5. lifecycle을 사용하지 않는 내부 payload의 base class를 `BaseObject`로 축소
6. `GetTraitStaticCast<T>()`와 선택적 `GetTraitDynamicCast<T>()` 추가
7. 기존 typed 조회 호출부를 안전성 계약에 따라 static/dynamic getter로 분류
8. 기존 trait tests를 `BaseObject` payload와 lifecycle payload 양쪽으로 확장
9. accessibility activation callback wrapper를 `TraitObject`에서 `BaseObject`로 단순화

## 검증 항목

1. 일반 `BaseObject`를 trait으로 저장, 조회, 교체, 제거할 수 있음
2. 일반 `BaseObject`에는 lifecycle hook을 시도하지 않음
3. `TraitObject`에는 attach, detach, View destruction hook이 정확히 한 번 호출됨
4. trait 교체 및 hook 재진입 중 객체 lifetime이 유지됨
5. `CoreInteractionObject`, `StateHandlerTrait`, `OverlayEffectImpl`의 기존 cleanup 동작
   유지
6. `GetTraitDynamicCast<T>()` 타입 불일치 시 빈 pointer 반환
7. `GetTraitStaticCast<T>()`의 null entry 처리
8. debug 검증을 선택할 경우 잘못된 static getter 사용이 test에서 검출됨
9. layout manager/callback/params와 state effect의 기존 동작 유지
10. callback을 사용하지 않는 View의 `sizeof(ViewDataImpl)` 증가 없음

## 미결 사항

1. `GetTraitStaticCast<T>()`에 debug-only RTTI assertion을 포함할지
2. `GetTraitDynamicCast<T>()`를 공용 integration helper로 함께 제공할지
3. lifecycle 사용이 없는 기존 `TraitObject` 파생 타입을 한 번에 `BaseObject`로
   변경할지, 새 payload부터 적용한 뒤 점진적으로 정리할지
4. reserved ID별 payload 타입 계약을 어디에 문서화할지
5. external extension이 reserved ID를 직접 설정하는 것을 지원 계약으로 볼지
6. object typed getter와 public handle typed getter를 별도 API로 유지할지

이 문서는 위 미결 사항을 결정하고 구현 승인을 받기 전까지 TODO 상태로 유지한다.
