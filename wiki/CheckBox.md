[한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/CheckBox-%28kr%29) · [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)

# CheckBox

`Dali::Ui::CheckBox` is a `SelectableView` with checked/unchecked state and an optional trailing label. The default glyph uses the installed `checkbox.json` Lottie resource.

## Setup and creation

Apply the Components configuration during application initialization. The example then adds an initially selected option to an initialized parent View.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

void ConfigureComponents()
{
  Components::UiConfig::New().Apply();
}
```

```cpp
void OnOptionChanged(View sender, bool selected, InputEvent event)
{
  // Store the selected state in the application model.
}

CheckBox AddOption(View parent)
{
  auto style = CheckBoxStyle::Default().Configure()
    .SetIconWidth(28.0f).SetIconHeight(28.0f)
    .SetLabelGap(12.0f).SetFontSize(18.0f)
    .Build();
  auto option = CheckBox::New("Receive updates", style);
  option.SetSelected(true);
  option.SelectionChangedSignal().Connect(&OnOptionChanged);
  parent.Add(option);
  return option;
}
```

Factories are `New()`, `New(style)`, `New(text)`, and `New(text, style)`. Omitting a style uses `CheckBoxStyle::Default()`. `SetText("")` switches to the box-only form; changing label presence invalidates measurement.

## Selection and animation

`IsSelected()`, `SetSelected(bool)`, `SelectionChangedSignal()`, and `SetToggleByClickEnabled(bool)` are inherited from SelectableView. The selection callback is `void(View, bool, InputEvent)` and can report programmatic changes as well as user interaction. The example initializes selection before connecting the callback.

Use `SetToggleByClickEnabled(false)` to prevent click-based toggling. This does not disable the entire control or prevent `SetSelected()` calls; use `SetEnabled(false)` for disabled interaction. For single-selection groups rather than independent options, see RadioButton.

`SetSelectionAnimationMode()` selects the policy below. Off-scene or invisible controls snap to the final state.

| Value | Behavior |
|---|---|
| `AUTO` (default) | Animate user changes while on-scene and visible |
| `ENABLED` | Animate programmatic changes too, while on-scene and visible |
| `DISABLED` | Apply all changes immediately |

## Style and sizing

Customize a separate copy with `CheckBoxStyle::Default().Configure()`; use `DefaultPreset()` for built-in values. Builders are move-only and must not be reused after `Build()`. Instance setters support text, typography, underline, and icon size updates.

| Style field | Built-in value |
|---|---|
| Minimum width/height | 0 / 0 |
| Padding / label gap | 8 on each side / 8 |
| Icon width/height | 36×36 |
| Unselected / selected color | `OUTLINE` / `PRIMARY` |
| Label color / font | `ON_SURFACE` / `SamsungOneUI600`, 16 |
| State effect | Round overlay |

Minimum sizes, padding, icon dimensions, label gap, and font size must be finite and non-negative. Icon zero means unset: width follows the resolved height, and height follows content height. This differs from IconButton's explicit-zero sizing. `GetIconWidth/Height()` report requested values, not arranged sizes. Overall size also accounts for the label, padding, and parent layout constraints.

`SetIconGenerator(IconGenerator&&)` accepts a factory returning `SelectableImageInterface`. Prefer a stateless function because a generator may be shared by style copies. Custom Lottie frame ranges and recoloring key paths must match the asset. See the buildable sample for a custom generator.

## Accessibility and resources

The role is `CHECK_BOX`, and selection updates the accessibility `CHECKED` state. Label text supplies the default name; an explicit `SetAccessibilityName()` wins. Provide a meaningful name for box-only controls. Internal label/icon views are hidden from accessibility, and disabled state is synchronized.

Install the component Lottie assets and any custom icon resources. A missing glyph asset is distinct from a selection-state failure. Validate screen-reader speech and visible animation in the target runtime.

## References

- [CheckBox API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/check-box.h)
- [CheckBoxStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/check-box-style.h)
- [SelectableView API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-foundation/public-api/views/selectable-view.h)
- [Sample and custom icon generator](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/check-box)
- [Manual scenario (Korean)](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-check-box-basics.md)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-CheckBox.cpp)
- [RadioButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/RadioButton)
