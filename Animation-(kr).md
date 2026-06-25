# Animation

[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/Animation)

DALi UI는 자주 사용하는 `View` 및 component property를 쉽게 animation하기 위한
method-chain helper를 제공합니다. 이 helper는 `dali-core` `Animation` 위에서
동작하며, `Dali::Animation`을 대체하지 않습니다.

Raw `Property`, `TimePeriod`, `AnimateTo` 호출을 반복해서 작성하는 대신,
읽기 쉬운 typed method로 일반적인 UI property animation을 구성할 때 사용합니다.

Sample: [animation-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/animation/animation-example.cpp)

<br/>

## Basic Usage

### Bridge Pattern

`View::Animate(animation)`은 lightweight bridge object를 반환합니다. Chained
method를 호출할 때마다 전달한 `Animation`에 animation entry가 즉시 추가됩니다.

~~~cpp
auto animation = Animation::New();

view.Animate(animation)
  .BackgroundColor(UiColor::PRIMARY, 500_ms)
  .PositionY(50.0f, 300_ms, AlphaFunction::EASE_OUT);

animation.Play();
~~~

**사용 시점**: click, focus, press, local state change처럼 한 번만 실행하는
interaction animation에 적합합니다.

<br/>

### Spec Pattern

`View::NewAnimationSpec()`은 재사용 가능한 animation specification을 만듭니다.
Spec은 entry를 먼저 저장하고, 이후 `Animation`과 target `View`에 적용됩니다.

~~~cpp
ViewAnimationSpec fadeIn = View::NewAnimationSpec()
  .Opacity(1.0f, 500_ms, AlphaFunction::EASE_IN_OUT);

auto animation = Animation::New();
fadeIn.ApplyTo(animation, view);
animation.Play();
~~~

**사용 시점**: 공유 motion definition, 반복되는 effect, `LayoutTransition` 같은
기능의 declarative 설정에 적합합니다.

<br/>

## dali-core Animation과의 차이

`dali-core` `Animation`에서는 target property와 time period를 직접 지정합니다.

~~~cpp
Animation animation = Animation::New(0.5f);
animation.AnimateTo(Property(view, Actor::Property::OPACITY),
                    0.0f,
                    AlphaFunction::EASE_IN_OUT,
                    TimePeriod(0.0f, 0.5f));
animation.Play();
~~~

DALi UI method-chain animation에서는 일반적인 UI property가 typed method로
제공됩니다.

~~~cpp
Animation animation = Animation::New();
view.Animate(animation)
  .Opacity(0.0f, 500_ms, AlphaFunction::EASE_IN_OUT);
animation.Play();
~~~

| 구분 | dali-core `Animation` | DALi UI method-chain API |
|---|---|---|
| 대상 | `Property(handle, index)` | `Opacity`, `PositionY`, `BackgroundColor` 같은 typed method |
| 시간 | `TimePeriod(delaySeconds, durationSeconds)` | `300_ms`, `1.5_s` 같은 `Duration` 값 |
| 재사용 | 보통 custom helper code 필요 | `ViewAnimationSpec` / `LabelAnimationSpec` |
| 타입 안정성 | property index와 value를 수동으로 맞춤 | method signature가 value type을 정의 |
| 실행 | `dali-core` `Animation` | 내부적으로 `dali-core` `Animation` 사용 |

Custom property, key frame, path, orientation 등 typed DALi UI API가 노출하지
않는 low-level animation에는 raw `Dali::Animation`을 사용합니다.

<br/>

## Duration

DALi UI animation method는 `Dali::Ui::Duration`을 사용합니다.

~~~cpp
300_ms   // 0.3 seconds
1.5_s    // 1.5 seconds
~~~

`alpha`와 `delay`는 optional parameter입니다.

~~~cpp
view.Animate(animation)
  .Opacity(0.0f, 250_ms)                                  // linear, no delay
  .ScaleX(1.2f, 200_ms, AlphaFunction::EASE_OUT, 100_ms); // delayed
~~~

<br/>

## Supported APIs

### View

| API | Value type | 설명 |
|---|---|---|
| `View::Animate(Animation)` | - | `ViewAnimationBridge` 생성 |
| `View::NewAnimationSpec()` | - | `ViewAnimationSpec` 생성 |

지원되는 `View` animation method family는 다음과 같습니다.

| Method | Value type | 설명 |
|---|---|---|
| `BackgroundColor`, `BackgroundColorBy` | `UiColor` | Background visual mix color |
| `SizeWidth`, `SizeWidthBy` | `float` | Actor width |
| `SizeHeight`, `SizeHeightBy` | `float` | Actor height |
| `PositionX`, `PositionXBy` | `float` | Actor X position |
| `PositionY`, `PositionYBy` | `float` | Actor Y position |
| `ScaleX`, `ScaleXBy` | `float` | Actor X scale |
| `ScaleY`, `ScaleYBy` | `float` | Actor Y scale |
| `Opacity`, `OpacityBy` | `float` | Actor opacity |
| `CornerRadius`, `CornerRadiusBy` | `Vector4` | Corner radius |
| `CornerSquareness`, `CornerSquarenessBy` | `Vector4` | Corner squareness |
| `BorderlineWidth`, `BorderlineWidthBy` | `float` | Borderline width |
| `BorderlineColor`, `BorderlineColorBy` | `UiColor` | Borderline color |
| `BorderlineOffset`, `BorderlineOffsetBy` | `float` | Borderline offset |

<br/>

### Label

| API | Value type | 설명 |
|---|---|---|
| `Label::Animate(Animation)` | - | `LabelAnimationBridge` 생성 |
| `Label::NewAnimationSpec()` | - | `LabelAnimationSpec` 생성 |

`LabelAnimationBridge`와 `LabelAnimationSpec`은 모든 `View` animation method를
포함하고, 다음 method를 추가로 제공합니다.

| Method | Value type | 설명 |
|---|---|---|
| `TextColor`, `TextColorBy` | `UiColor` | Label text color |

<br/>

## Examples

### Interaction Animation

~~~cpp
void OnClicked(View view, InputEvent event)
{
  auto animation = Animation::New();

  view.Animate(animation)
    .Opacity(0.0f, 200_ms, AlphaFunction::EASE_IN)
    .PositionY(50.0f, 300_ms, AlphaFunction::EASE_OUT);

  animation.Play();
}
~~~

<br/>

### Reusable Spec

~~~cpp
ViewAnimationSpec fadeIn = View::NewAnimationSpec()
  .Opacity(1.0f, 500_ms, AlphaFunction::EASE_IN_OUT);

void PlayFadeIn(View view)
{
  auto animation = Animation::New();
  fadeIn.ApplyTo(animation, view);
  animation.Play();
}
~~~

<br/>

### Label Animation

~~~cpp
auto animation = Animation::New();

label.Animate(animation)
  .TextColor(UiColor::PRIMARY, 250_ms)
  .Opacity(1.0f, 250_ms);

animation.Play();
~~~

<br/>

### raw dali-core Animation과 함께 사용

Method-chain API는 같은 `Animation` handle에 entry를 추가하므로, 필요한 경우
raw `dali-core` animation call을 함께 추가할 수 있습니다.

~~~cpp
auto animation = Animation::New();

view.Animate(animation)
  .Opacity(1.0f, 250_ms);

animation.AnimateTo(Property(view, Actor::Property::ORIENTATION),
                    targetQuaternion,
                    AlphaFunction::EASE_OUT,
                    TimePeriod(0.0f, 0.25f));

animation.Play();
~~~

## Guidelines

* Local one-time animation code에는 bridge pattern을 사용합니다.
* 재사용 가능한 motion definition에는 spec pattern을 사용합니다.
* `Opacity(1.0f, 300_ms)`처럼 `By`가 없는 method는 absolute target value에
  사용합니다.
* `PositionYBy(20.0f, 300_ms)` 같은 `By` method는 relative change에
  사용합니다.
* Layout에 의해 결정되는 movement나 size change에는 가능한 경우
  `LayoutTransition` bounds effect 또는 change timing을 사용합니다.
* Typed DALi UI API가 노출하지 않는 property나 animation type에는 raw
  `Dali::Animation`을 사용합니다.

<br/>

## Caveats

* Bridge는 기존 `Animation`과 target view를 감싸는 임시 helper입니다.
  `Animation` handle lifetime은 기존 방식과 동일하게 관리해야 합니다.
* Spec은 재사용 가능한 definition일 뿐 실행 중인 animation이 아닙니다.
* Empty spec은 visual effect가 없습니다.
* `LayoutTransition` visual spec에서는 `PositionX`, `PositionY`,
  `SizeWidth`, `SizeHeight` 같은 layout-owned bounds property가 거부됩니다.
  Bounds animation은 layout bounds-effect channel에서 처리해야 합니다.
* `AlphaFunction::REVERSE`는 `LayoutTransition` visual spec validation에서
  거부됩니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
