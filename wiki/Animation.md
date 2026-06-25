[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Animation-(kr))

# Animation

DALi UI provides method-chain animation helpers for common `View` and component
properties. These helpers are built on top of `dali-core` `Animation`; they do
not replace `Dali::Animation`.

Use them when you want to animate common UI properties with readable, typed
methods instead of repeatedly writing raw `Property`, `TimePeriod`, and
`AnimateTo` calls.

Sample: [animation-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/animation/animation-example.cpp)

<br/>

## Basic Usage

### Bridge Pattern

`View::Animate(animation)` returns a lightweight bridge object. Each chained
method immediately adds an animation entry to the given `Animation`.

~~~cpp
auto animation = Animation::New();

view.Animate(animation)
  .BackgroundColor(UiColor::PRIMARY, 500_ms)
  .PositionY(50.0f, 300_ms, AlphaFunction::EASE_OUT);

animation.Play();
~~~

**When to use**: one-off interaction animations such as click, focus, press, or
local state changes.

<br/>

### Spec Pattern

`View::NewAnimationSpec()` creates a reusable animation specification. The spec
stores entries first, then applies them to an `Animation` and target `View`.

~~~cpp
ViewAnimationSpec fadeIn = View::NewAnimationSpec()
  .Opacity(1.0f, 500_ms, AlphaFunction::EASE_IN_OUT);

auto animation = Animation::New();
fadeIn.ApplyTo(animation, view);
animation.Play();
~~~

**When to use**: shared motion definitions, repeated effects, or declarative
configuration for features such as `LayoutTransition`.

<br/>

## Difference from dali-core Animation

With `dali-core` `Animation`, applications specify the target property and time
period directly.

~~~cpp
Animation animation = Animation::New(0.5f);
animation.AnimateTo(Property(view, Actor::Property::OPACITY),
                    0.0f,
                    AlphaFunction::EASE_IN_OUT,
                    TimePeriod(0.0f, 0.5f));
animation.Play();
~~~

With DALi UI method-chain animation, common UI properties are exposed as typed
methods.

~~~cpp
Animation animation = Animation::New();
view.Animate(animation)
  .Opacity(0.0f, 500_ms, AlphaFunction::EASE_IN_OUT);
animation.Play();
~~~

| Area | dali-core `Animation` | DALi UI method-chain API |
|---|---|---|
| Target | `Property(handle, index)` | Typed methods such as `Opacity`, `PositionY`, `BackgroundColor` |
| Time | `TimePeriod(delaySeconds, durationSeconds)` | `Duration` values such as `300_ms`, `1.5_s` |
| Reuse | Usually custom helper code | `ViewAnimationSpec` / `LabelAnimationSpec` |
| Type safety | Property index and value are paired manually | Method signature defines the value type |
| Runtime | `dali-core` `Animation` | Uses `dali-core` `Animation` internally |

Use raw `Dali::Animation` for custom properties, key frames, paths, orientation,
or low-level animation features that are not exposed by the typed DALi UI API.

<br/>

## Duration

DALi UI animation methods use `Dali::Ui::Duration`.

~~~cpp
300_ms   // 0.3 seconds
1.5_s    // 1.5 seconds
~~~

`alpha` and `delay` are optional parameters.

~~~cpp
view.Animate(animation)
  .Opacity(0.0f, 250_ms)                                  // linear, no delay
  .ScaleX(1.2f, 200_ms, AlphaFunction::EASE_OUT, 100_ms); // delayed
~~~

<br/>

## Supported APIs

### View

| API | Value type | Description |
|---|---|---|
| `View::Animate(Animation)` | - | Create `ViewAnimationBridge` |
| `View::NewAnimationSpec()` | - | Create `ViewAnimationSpec` |

Supported `View` animation method families:

| Method | Value type | Description |
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

| API | Value type | Description |
|---|---|---|
| `Label::Animate(Animation)` | - | Create `LabelAnimationBridge` |
| `Label::NewAnimationSpec()` | - | Create `LabelAnimationSpec` |

`LabelAnimationBridge` and `LabelAnimationSpec` include all `View` animation
methods and add:

| Method | Value type | Description |
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

### Mixing with raw dali-core Animation

The method-chain API writes entries into the same `Animation` handle, so raw
`dali-core` animation calls can be added when needed.

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

* Use the bridge pattern for local one-time animation code.
* Use the spec pattern for reusable motion definitions.
* Use methods without `By`, such as `Opacity(1.0f, 300_ms)`, for absolute
  target values.
* Use `By` methods, such as `PositionYBy(20.0f, 300_ms)`, for relative changes.
* For layout-driven movement or size changes, prefer `LayoutTransition` bounds
  effects or change timing where applicable.
* Use raw `Dali::Animation` for properties or animation types not exposed by
  the typed DALi UI API.

<br/>

## Caveats

* A bridge is a temporary helper around an existing `Animation` and target view.
  Keep the `Animation` handle alive as usual.
* A spec is only a reusable definition. It is not a running animation.
* Empty specs have no visual effect.
* In `LayoutTransition` visual specs, layout-owned bounds properties such as
  `PositionX`, `PositionY`, `SizeWidth`, and `SizeHeight` are rejected. Bounds
  animation belongs to the layout bounds-effect channel.
* `AlphaFunction::REVERSE` is rejected by `LayoutTransition` visual spec
  validation.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
