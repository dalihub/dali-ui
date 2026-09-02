[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Switch-(kr))

# DALi UI Components - Switch

`Dali::Ui::Switch` is a binary selectable control in
`dali-ui-components`. It displays a track and a movable thumb and supports tap,
horizontal drag, and focused Enter-key operation. Selection APIs and signals
come from its `SelectableView` base class.

The default style follows the DALi UI component theme while retaining the
OneUI Switch concepts: themed variables are represented by `SwitchStyle`, the
selection transition is animated, and the touch overlay is centered on the
thumb.

---

## 1. Basic Setup

Include the components umbrella header and apply `Components::UiConfig` before
creating component styles or entering the main loop.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Includes both the foundation configuration and component styles.
  Components::UiConfig::New().Apply();

  MyController controller(application);
  application.MainLoop();
  return 0;
}
```

Apply only `Components::UiConfig` in an application using
`dali-ui-components`; a separate foundation `UiConfig` is not required.

---

## 2. Creating and Placing a Switch

Create a Switch with the configured default style:

```cpp
Switch wifiSwitch = Switch::New();
wifiSwitch.SetAccessibilityName("Wi-Fi");
rootLayout.Add(wifiSwitch);
```

Set the initial state before adding it to the scene when it should start on:

```cpp
Switch bluetoothSwitch = Switch::New();
bluetoothSwitch.SetSelected(true);
rootLayout.Add(bluetoothSwitch);
```

The default track is `56 x 32` units. Its `10`-unit outer padding contains the
expanded touch-effect target, so its default natural size is `76 x 52` units.
Normal `View` sizing and layout APIs can override the requested or minimum
size.

---

## 3. Input and Interaction

The same selected state is controlled by every supported input path.

| Input | Behavior |
|---|---|
| Tap/click | Toggles between on and off. |
| Horizontal drag | Follows the pointer and settles to the nearest state on release. |
| Enter while focused | Toggles through the inherited key-click path. |
| Interrupted drag | Returns to the state held before the interrupted gesture. |
| RTL layout | Mirrors thumb placement and horizontal drag direction. |

`Switch` also follows the inherited interaction policies:

```cpp
wifiSwitch.SetEnabled(false);              // Ignores pointer and Enter input.
wifiSwitch.SetClickable(false);            // Prevents user activation.
wifiSwitch.SetToggleByClickEnabled(false); // Prevents user state toggling.
```

These policies block user-driven changes, including Switch dragging, but do
not prevent an application from calling `SetSelected()`.

---

## 4. Selection State and Signal

Read or change the state through the inherited `SelectableView` API:

```cpp
bool enabled = wifiSwitch.IsSelected();
wifiSwitch.SetSelected(!enabled);
```

Connect `SelectionChangedSignal()` to observe actual state changes:

```cpp
wifiSwitch.SelectionChangedSignal().Connect(
  this,
  [this](View /*view*/, bool selected, InputEvent event) {
    if(event.IsProgrammatic())
    {
      // Changed by an application SetSelected() call or a completed drag.
    }
    else
    {
      // Changed by tap or Enter input.
    }

    UpdateWifi(selected);
  });
```

The signal is emitted only when the selected value changes. Calling
`SetSelected()` with the current value does not emit it. Tap and Enter changes
carry their originating input event. A drag commits through `SetSelected()`, so
its signal event is programmatic even though AUTO mode still animates the drag.

---

## 5. Selection Animation

`SetSelectionAnimationMode()` controls when the thumb and track transition is
animated.

| Mode | Behavior |
|---|---|
| `SelectionAnimationMode::AUTO` | Default. Animates user input; programmatic changes snap. |
| `SelectionAnimationMode::ENABLED` | Animates both user and programmatic changes while visible and on scene. |
| `SelectionAnimationMode::DISABLED` | Always snaps directly to the requested state. |

```cpp
wifiSwitch.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);
wifiSwitch.SetSelected(true); // Animates when visible and connected to a scene.
```

When an animation is enabled, its duration is scaled by the remaining travel
distance. Reversing the state or starting a drag during a transition therefore
continues from the currently displayed position.

The full transition uses the fixed OneUI timing of `0.3` seconds and cubic
control points `(0.22, 0.25)` and `(0.0, 1.0)`. These values are intrinsic
Switch behavior and are not part of `SwitchStyle`.

---

## 6. Customizing SwitchStyle

`SwitchStyle` is the DALi UI equivalent of OneUI Switch variables. Clone the
configured default, change only the required values, build an immutable style,
and pass it to `Switch::New(style)`.

```cpp
SwitchStyle compactGreen = SwitchStyle::Default()
                             .Configure()
                             .SetTrackSize(48.0f, 28.0f)
                             .SetThumbPadding(Insets(3.0f, 3.0f, 3.0f, 3.0f))
                             .SetTrackColors(UiColor(0xB7B7BBu),
                                             UiColor(0x0BB075u))
                             .Build();

Switch compactSwitch = Switch::New(compactGreen);
```

The style is applied when the Switch is created. `Switch` does not provide a
runtime `SetStyle()` API.

### Default values

| Style value | Default |
|---|---|
| Minimum width / height | `0 / 0` |
| Outer padding | `Insets(10, 10, 10, 10)` |
| Track width / height | `56 / 32` |
| Thumb padding | `Insets(3.25, 3.25, 3.25, 3.25)` |
| Off track color | Theme color `SurfaceContainerHigher` |
| On track color | `UiColor::PRIMARY` |
| Thumb color | Theme color `OnSurfaceContainerFixed` |
| Overlay effect | Round overlay with `Insets(10)` overlay padding and child recoil at scale `0.96` |
| Drag threshold | `5` units |

### Builder API groups

| Purpose | APIs |
|---|---|
| Size | `SetMinimumWidth()`, `SetMinimumHeight()`, `SetPadding()` |
| Track | `SetTrackWidth()`, `SetTrackHeight()`, `SetTrackSize()` |
| Thumb geometry | `SetThumbPadding()` |
| Colors | `SetTrackColor()`, `SetSelectedTrackColor()`, `SetTrackColors()`, `SetThumbColor()` |
| Interaction effect | `SetOverlayEffect()` |
| Drag | `SetDragThreshold()` |

### Style constraints

`Build()` validates geometry so the thumb remains valid:

- Track width and height must be finite and positive, and width must be at
  least the height.
- Insets must be finite and non-negative.
- Opposing thumb-padding values must leave a positive thumb size within the
  track height.
- Minimum size and drag threshold must be finite and non-negative.

Violating these contracts triggers a DALi assertion.

---

## 7. Touch Overlay and Recoil

The default style uses a round `OverlayEffect`. Its `OverlayPadding` expands the
visual by 10 units around the 32x32 primary target without changing Switch
layout or touch bounds. The visible thumb is the target's child, so it recoils
to `0.96` scale while pressed.

Provide a different `OverlayEffect` through the style when a product needs a
different response:

```cpp
OverlayEffect effect = OverlayEffect::Round()
                         .Configure()
                         .SetOverlayPadding(Insets(8.0f, 8.0f, 8.0f, 8.0f))
                         .SetRecoilScope(RecoilScope::OVERLAY_TARGET_CHILDREN)
                         .SetRecoilScaleFactor(0.94f)
                         .Build();

SwitchStyle style = SwitchStyle::Default()
                      .Configure()
                      .SetOverlayEffect(effect)
                      .Build();
```

Passing an empty `OverlayEffect` to `SetOverlayEffect()` disables the Switch
overlay effect.

---

## 8. Accessibility and RTL

`Switch` exposes the accessibility role `SWITCH` and keeps its checked and
enabled accessibility states synchronized with its selected and enabled
states. Give each control a name describing the setting it controls:

```cpp
Switch notifications = Switch::New();
notifications.SetAccessibilityName("Notifications");
```

Do not put the current value (`on` or `off`) in the accessibility name; the
checked state communicates that value. Focused Enter-key activation uses the
same selection path as pointer input.

The thumb endpoint and drag direction follow the effective layout direction,
so application code does not need separate LTR and RTL Switch styles.

---

## 9. Sample and Packaging

The interactive sample demonstrates default, initially selected, custom-style,
and disabled Switches. Labels show callback counts and whether the last change
was programmatic or user-originated.

Build it standalone from the repository root after loading the DALi environment:

```sh
source ~/setenv
cmake -S samples/switch -B /tmp/dali-ui-switch-build -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/dali-ui-switch-build
```

The sample executable is written to:

```text
samples/switch/bin/switch.example
```

Tizen packaging metadata is provided with the sample:

| File | Purpose |
|---|---|
| `samples/switch/packaging/com.samsung.dali.switch.spec` | Package build and install rules |
| `samples/switch/com.samsung.dali.switch.xml` | Tizen application metadata |
| `samples/switch/com.samsung.dali.switch.manifest` | Package manifest and privileges |

See
[`samples/switch`](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/switch)
for the complete source.

---

## 10. Usage Notes

- Apply `Components::UiConfig` before calling `Switch::New()` or
  `SwitchStyle::Default()`.
- Use `SelectionChangedSignal()` rather than `ClickedSignal()` when application
  behavior depends on the selected value; dragging changes selection without
  being a simple click.
- `InputEvent::IsProgrammatic()` distinguishes tap/Enter from programmatic
  changes, but drag commits are also reported as programmatic.
- Use `SelectionAnimationMode::ENABLED` when programmatic changes should be
  visible as transitions.
- Keep outer padding large enough to contain the configured OverlayEffect
  padding when creating a compact custom style.
- Set an accessibility name that describes the controlled setting.

<br/>

---

[← Back to Components](Components.md)
