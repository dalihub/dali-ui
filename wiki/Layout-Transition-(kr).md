[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Transition)

# Layout Transition

`LayoutTransition`은 연속된 layout pass 결과 사이에서 `View`의 자식들을 애니메이션합니다. 자식이 추가, 제거, 재정렬되거나 부모 layout이 기존 자식의 bounds를 다르게 계산하면, framework가 해당 slot에 맞는 애니메이션을 실행합니다.

<img src="assets/Layout/dali-ui-layout-transition-grid-item.gif" height="400"/>


---

## 개념

### Slot

| Slot | 발생 조건 |
|---|---|
| `ENTER` | 부모가 initial arrange를 완료한 뒤 새 자식이 추가됨 |
| `EXIT` | `View::Remove(child, RemovePolicy::ANIMATE_EXIT)`가 호출되고 EXIT slot이 설정됨 |
| `CHANGE` | 기존 자식의 arranged bounds가 이전 pass와 달라짐 |

각 slot은 독립적인 설정을 가집니다. 모든 slot은 frame 0에서 독립적으로 시작됩니다. 같은 child에 대해 EXIT -> CHANGE -> ENTER 같은 엄격한 순서가 필요하면 `OnFinished` lifecycle callback에서 다음 동작을 연결하세요.

### Mode

Slot은 **spec mode** 또는 **animator mode**로 설정할 수 있습니다.

- **Spec mode**: 애플리케이션이 `ViewAnimationSpec`(ENTER / EXIT) 또는 `LayoutTransitionTiming`(CHANGE)을 제공하고, framework가 `dali-core` `Animation`으로 보간을 수행합니다.
- **Animator mode**: 애플리케이션이 `LayoutAnimatorCallback`을 등록합니다. framework가 설정된 timing 동안 progress 0 -> 1을 sweep하며 매 frame callback을 호출하고, callback이 `Actor` property를 직접 씁니다.

같은 slot에 둘 다 설정되어 있으면 **animator mode가 우선**합니다.

### Cause

`LayoutAnimatorContext::changeCause`는 CHANGE 상황을 구분합니다. `slot == CHANGE`일 때만 의미가 있으며, ENTER / EXIT에서는 `OTHER`로 기본 설정됩니다.

| Cause | 의미 |
|---|---|
| `SIBLING_ADDED` | sibling 추가로 기존 child에 CHANGE 발생 |
| `SIBLING_REMOVED` | sibling 제거로 기존 child에 CHANGE 발생 |
| `REORDERED` | sibling 순서 변경으로 CHANGE 발생 |
| `WINDOW_RESIZED` | top-level window resize로 CHANGE 발생 |
| `OTHER` | 그 외 layout-driven change |

---

## Quick Start: Spec Mode

```cpp
StackLayout layout = StackLayout::New();

ViewAnimationSpec enter = ViewAnimationSpec::New();
enter.Opacity(1.0f, Duration(0.25f), AlphaFunction(AlphaFunction::EASE_OUT));

ViewAnimationSpec exit = ViewAnimationSpec::New();
exit.Opacity(0.0f, Duration(0.2f), AlphaFunction(AlphaFunction::EASE_IN));

LayoutTransitionTiming change{Duration(0.3f),
                              AlphaFunction(AlphaFunction::EASE_IN_OUT),
                              Duration()};

LayoutTransition transition = LayoutTransition::New();
transition.SetEnterVisualSpec(enter)
          .SetExitVisualSpec(exit)
          .SetEnterBoundsEffect(LayoutBoundsEffects::SlideFrom(
            LayoutBoundsEdge::BOTTOM,
            {Duration(0.3f), AlphaFunction(AlphaFunction::EASE_OUT), Duration()}))
          .SetExitBoundsEffect(LayoutBoundsEffects::SlideTo(
            LayoutBoundsEdge::BOTTOM,
            {Duration(0.3f), AlphaFunction(AlphaFunction::EASE_IN), Duration()}))
          .SetChangeTiming(change);

layout.SetLayoutTransition(transition);

View child = View::New();
child.SetProperty(Actor::Property::OPACITY, 0.0f);
layout.Add(child);
```

Bounds effect channel은 mirror semantics를 사용합니다. 하나의 `LayoutBoundsEffect` descriptor가 ENTER에서는 endpoint -> base, EXIT에서는 base -> endpoint로 해석됩니다.

---

## Quick Start: Animator Mode

```cpp
auto onChange = [](const LayoutAnimatorContext& ctx)
{
  Actor actor = ctx.view;
  if(!actor) return;

  const float p = ctx.progress;
  const float x = ctx.fromBounds.x + (ctx.toBounds.x - ctx.fromBounds.x) * p;
  const float y = ctx.fromBounds.y + (ctx.toBounds.y - ctx.fromBounds.y) * p;
  actor.SetPositionX(x);
  actor.SetPositionY(y);
};

LayoutAnimatorTiming timing;
timing.duration = Duration(0.3f);
timing.alpha = AlphaFunction(AlphaFunction::EASE_IN_OUT);

LayoutTransition transition = LayoutTransition::New();
transition.SetChangeAnimator(LayoutAnimatorCallback::New(onChange), timing);
layout.SetLayoutTransition(transition);
```

---

## 주요 옵션

| Option | 기본값 | 효과 |
|---|---|---|
| `SetChangeOnWindowResize` | `false` | `true`이면 window resize로 발생한 layout change에도 CHANGE 실행 |
| `SetEnterOnInitialMount` | `false` | `true`이면 부모의 첫 arrange pass에 이미 존재하던 자식에도 ENTER 실행 |
| `SetReflowScope` | `DIRECT_CHILDREN` | CHANGE slot 적용 범위 선택. `SUBTREE`는 transition이 없는 descendant까지 reflow |
| `SetOnStart` | unset | per-(view, slot) transition 시작 시 호출 |
| `SetOnFinished` | unset | transition이 정상 완료될 때 호출 |

### Reflow Scope

기본적으로 transition은 해당 view의 **direct children**만 애니메이션합니다. 손자 view는 자신의 직접 부모에도 transition이 있어야 애니메이션됩니다.

`SetReflowScope(LayoutReflowScope::SUBTREE)`를 설정하면 **CHANGE slot**이 subtree 전체까지 확장됩니다. 가장 가까운 transition 보유 ancestor가 해당 node의 애니메이션을 담당하며, descendant가 자체 transition을 가지면 그 지점에서 scope가 끊깁니다.

```cpp
LayoutTransition t = LayoutTransition::New();
t.SetChangeTiming(LayoutTransitionTiming{Duration(0.25f), AlphaFunction(AlphaFunction::EASE_IN_OUT), Duration()})
 .SetReflowScope(LayoutReflowScope::SUBTREE);
container.SetLayoutTransition(t);
```

---

## 자기 적용 Transition (self transition)

`View::SetSelfLayoutTransition(t)`는 **그 view 자신**이 layout child로서 적용받을 transition을 부착합니다. 부모가 `SetLayoutTransition`으로 부착한 transition보다, 그리고 `SUBTREE` scope ancestor의 transition보다 우선합니다.

우선순위는 (view, event)마다 dispatch 시점에 결정됩니다.

1. 그 view 자신의 self transition — 여기서 종결
2. 직접 부모의 transition — 여기서 종결
3. 해당 slot 효과를 가진 가장 가까운 `SUBTREE` scope ancestor

각 단계는 slot별이 아니라 **부착 여부만으로 종결**됩니다. CHANGE만 설정한 self transition이 부모의 ENTER를 상속하지는 않으며, 설정되지 않은 slot은 그냥 애니메이션되지 않습니다(CHANGE는 즉시 최종 bounds로 snap, EXIT는 즉시 unparent, ENTER는 skip). 이는 2단계와 3단계가 이미 사용하는 것과 동일한 도매(wholesale) 규칙입니다.

### 3가지 상태

| 상태 | 표기 | 의미 |
|---|---|---|
| 미설정(기본) | 호출하지 않거나 uninitialized handle로 해제 | 상속 — 부모 / ancestor 규칙이 적용됨 |
| 오버라이드 | `SetSelfLayoutTransition(t)` | `t`가 이 view의 모든 slot을 단독 지배 |
| 명시적 제외 | `SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH)` | Layout transition이 이 view를 통과: 어떤 slot도 애니메이션되지 않음 — CHANGE는 snap, EXIT는 즉시 unparent, ENTER는 skip되고 아무것도 settle되지 않음 |

Uninitialized handle은 "다시 상속"이지 **"애니메이션 금지"가 아닙니다**.

```cpp
LayoutTransition slow = LayoutTransition::New();
slow.SetChangeTiming(LayoutTransitionTiming{Duration(0.8f), AlphaFunction(AlphaFunction::EASE_IN_OUT), Duration()});
item.SetSelfLayoutTransition(slow);                                    // 오버라이드
other.SetLayoutTransitionMode(LayoutTransitionMode::PASS_THROUGH);     // 통과(정책)
other.SetLayoutTransitionMode(LayoutTransitionMode::AUTO);             // 상속으로 복귀
other.SetSelfLayoutTransition(LayoutTransition());                     // handle 해제
```

### 뷰별 정책 (`LayoutTransitionMode`)

`View::SetLayoutTransitionMode`는 뷰별 **정책**을 선언합니다. 어떤 transition handle을 해석하기 **전에** 읽히므로, **같은 view에서는 정책이 값을 이깁니다**. 즉, 모드가 `AUTO`가 아닌 view는 자기 자신이 부착한 handle로도 애니메이션되지 않습니다.

| 모드 | 효과 |
|---|---|
| `AUTO`(기본) | 일반 해석: 자신의 self transition > 직접 부모의 transition > 가장 가까운 `SUBTREE` ancestor의 transition |
| `PASS_THROUGH` | Layout transition이 이 view를 통과합니다: 이 view는 결코 transition의 대상이 되지 않으며, 자기 자신이 부착한 handle의 대상도 아닙니다. 상속은 자손으로 계속 흘러가고, 이 view 자신의 **children 역할** transition도 그 자식들을 계속 지배합니다 |
| `ISOLATE_SUBTREE` | 이 view와 그 subtree 전체를 이 view **자신을 포함해 그 위쪽**의 모든 owner로부터 격리합니다 — 이 view 자신의 children 역할 transition도 포함해서, 위에서 내려오는 어떤 것도 그 안의 무엇도 애니메이션하지 못합니다 |

`ISOLATE_SUBTREE`가 끊는 것은 게이트를 지나가거나 게이트에서 시작하는, **위에서 내려오는** 지배 사슬뿐입니다. 게이트보다 **아래에서** 선언한 것은 그대로 동작합니다. Descendant의 self transition은 그대로 그 view를 애니메이션하고, descendant의 children 역할 transition도 그 descendant의 자식들을 그대로 지배합니다.

- **Mode 변경으로 handle이 해제되지는 않습니다.** 모드와 무관하게 `GetSelfLayoutTransition()`은 부착된 handle을 그대로 반환하며, `AUTO`로 돌아가면 재부착 없이 효과가 복구됩니다.
- **Mode 변경은 취소가 아닙니다.** In-flight transition은 handle을 교체할 때와 똑같이 각자의 타이밍으로 끝까지 진행되고, 새 정책은 다음 (view, slot) 이벤트부터 적용됩니다. `AUTO`로 되돌아가도 이전 모드에서 발생한 add에 대해 ENTER가 소급 발생하지는 않습니다.
- Uninitialized handle은 여전히 "다시 상속"이지 **"애니메이션 금지"가 아닙니다**. 그것은 `PASS_THROUGH`의 역할입니다.

### 주의 사항

- **Standalone 동작.** Ancestor에 transition이 하나도 없어도 self transition은 단독으로 동작합니다.
- **한 view가 두 역할을 동시에 가질 수 있음.** `SetLayoutTransition`은 그 view의 children을, `SetSelfLayoutTransition`은 그 view 자신을 지배합니다.
- **Self 역할에서 `SetReflowScope`는 무시됩니다.**
- **Lifecycle callback은 승자에서만 발생.** Self transition이 지배하는 view는 부모의 `OnStart` / `OnFinished`를 발생시키지 않으므로, 부모에서 자식 완료 수를 세는 코드는 이를 감안해야 합니다.
- **EXIT geometry는 변하지 않음.** Ghost는 여전히 직접 부모 아래에 남고 unparent 대상도 직접 부모입니다. 바뀌는 것은 효과의 출처뿐입니다.
- **CHANGE cause.** Transition을 가진 부모의 self 지배 **직속** 자식은 cause를 그대로 유지합니다(`REORDERED` / `SIBLING_*` / `WINDOW_RESIZED`). Ancestor에 transition이 없으면 `SUBTREE` 상속 descendant와 동일하게 `OTHER`(resize 중에는 `WINDOW_RESIZED`)로 강등되므로, standalone self transition에는 **기본** CHANGE timing을 설정하세요.
- **Initial mount 기준은 children 역할과 동일**하며(직접 부모의 첫 arrange), opt-in은 **self** transition의 `SetEnterOnInitialMount`에서 읽습니다.
- **부모 View가 없는 동안 self transition은 동작하지 않습니다.**
- **Handle 교체나 해제는 in-flight transition을 취소하지 않습니다.**

---

## Lifecycle과 제거

`View::Remove(child, RemovePolicy::ANIMATE_EXIT)`는 EXIT slot이 설정된 경우 **deferred remove**를 수행합니다. child는 layout tracking list에서는 즉시 제거되어 sibling이 빈 공간으로 reflow되지만, actor는 EXIT 애니메이션이 끝날 때까지 붙어 있습니다. EXIT 완료 후 actor는 자동으로 unparent됩니다.

상속된 one-argument `Actor::Remove`, `RemovePolicy::IMMEDIATE`, 또는 `Self().Remove`를 직접 사용하면 actor가 즉시 unparent되고 EXIT는 실행되지 않습니다.

`OnFinished`는 transition이 정상 완료될 때만 호출됩니다. 새 transition에 의해 대체되거나, reparent되거나, view destruction / scene disconnection으로 취소되는 경우에는 호출되지 않습니다.

---

## 주의 사항

- **Callback reference cycle 금지.** Animator 또는 lifecycle closure 안에서 target `View`를 값으로 capture하면 cycle이 생길 수 있습니다. 필요한 경우 `WeakHandle<View>`를 사용하거나 callback 안의 `ctx.view`를 읽으세요.
- **Animator callback에서 view tree 변경 금지.** `Add`, `Remove`, `Unparent`, `InvalidateMeasure`는 animator callback에서 호출하지 마세요. Tree mutation은 lifecycle callback에서만 수행하세요.
- **Transition 교체는 in-flight transition을 취소하지 않음.** `SetLayoutTransition`으로 새 handle을 설정해도 이미 실행 중인 transition은 시작 당시 handle의 timing으로 끝까지 진행됩니다.
- **기본 ENTER / EXIT 없음.** Slot에 spec이나 animator가 없으면 해당 slot은 건너뜁니다.
- **ENTER / EXIT visual spec의 bounds entry 금지.** Bounds 관련 property는 bounds-effect API 또는 animator mode로 처리하세요.
- **Zero-duration 또는 empty spec은 transition 없음으로 처리.** Lifecycle callback도 호출되지 않습니다.
- **Cancellation은 silent.** `OnFinished`는 정상 완료된 transition에만 호출됩니다.
- **EXIT `OnFinished`는 unparent 이후 호출.** EXIT 완료 callback 안에서 `view.GetParent()`는 uninitialized handle입니다.
- **EXIT ghost는 되살릴 수 없음.** EXIT 중인 child를 같은 parent에 다시 추가해도 무시됩니다. 취소하려면 다른 parent로 reparent하세요.

---

## AlphaFunction 제한

`AlphaFunction::REVERSE`는 `LayoutTransition`에서 지원되지 않습니다. Transition 방향은 ENTER / EXIT / CHANGE slot semantics와 captured bounds로 결정되며, alpha function은 그 transition의 진행 형태만 조정합니다.

Layout bounds timing(`SetChangeTiming`, `LayoutBoundsEffect::timing`)에서는 최종 bounds가 layout target과 일치해야 하므로 `BOUNCE`, `SIN`도 거부됩니다. `CUSTOM_FUNCTION`의 출력은 애플리케이션 책임입니다.

Animator mode는 일부 alpha만 직접 평가합니다. `BOUNCE`, `SIN`, `EASE_OUT_BACK`, `BEZIER`, `SPRING`, `CUSTOM_SPRING`은 현재 linear fallback으로 처리됩니다.

---

## 함께 보기

- [API Reference - LayoutTransition](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1LayoutTransition.html)
- `dali-ui-foundation/public-api/layouts/layout-transition.h`
- `dali-ui-foundation/public-api/layouts/layout-transition-types.h`
- [samples/layout-transition](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/layout-transition)
- `automated-tests/src/dali-ui-foundation/utc-Dali-LayoutTransition.cpp`

---

[← Layout으로 돌아가기](Layout-(kr).md)
