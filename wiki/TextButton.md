[한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/TextButton-%28kr%29) · [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)

# TextButton

`Dali::Ui::TextButton` is an `InteractiveView` for actions represented by text. It is not a persistent selection control: use CheckBox for checked options and IconButton for icon-only actions.

## Setup and basic use

Apply the Components configuration once during application initialization, before resolving styles or creating controls. `AddSaveButton` below adds a button to an initialized parent View.

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
void OnSaveClicked(View sender, InputEvent event)
{
  // Start the application action here.
}

TextButton AddSaveButton(View parent)
{
  auto style = TextButtonStyle::Default().Configure()
    .SetMinimumHeight(48.0f)
    .SetPadding(Insets(20.0f, 20.0f, 12.0f, 12.0f))
    .SetFontSize(18.0f)
    .Build();
  auto button = TextButton::New("Save", style);
  button.ClickedSignal().Connect(&OnSaveClicked);
  parent.Add(button);
  return button;
}
```

Factories are `New()`, `New(style)`, `New(text)`, and `New(text, style)`. Omitting the style resolves `TextButtonStyle::Default()` from the current configuration. `TextButton()` alone creates an empty handle; use `New()` before accessing the control.

## Styling and runtime APIs

`Default()` resolves the configured style; `DefaultPreset()` returns the built-in preset. `Configure()` creates an independent builder. A successful `Build()` consumes the move-only builder. A creation style is not a live binding that overwrites subsequent instance changes.

| Concern | API |
|---|---|
| Button size limits | Builder `SetMinimumWidth/Height`, `SetMaximumWidth/Height`, `SetMinimumSize`, `SetMaximumSize` |
| Appearance | Builder `SetPadding`, `SetBackgroundColor`, `SetCornerRadius`, `SetCornerRadiusPolicy`, `SetStateEffect` |
| Text updates | Button `SetText`, `GetText` |
| Typography | Style or button `SetTextColor`, `SetFontSize`, `SetFontFamily`, `SetTextUnderline` |
| Label alignment | Style or button `SetHorizontalAlignment`, `SetVerticalAlignment` |

Font size, minimum sizes, padding, and radii must be finite and non-negative. Maximum sizes accept finite non-negative values or `UNCONSTRAINED_MAX_SIZE`. `Build()` asserts if an axis minimum exceeds its constrained maximum; this validation failure leaves the builder available for correction. Font size zero is passed unchanged to the internal Label, not replaced with the preset size.

Built-in defaults are minimum 10×10, no maximum constraint, horizontal padding 16 and vertical padding 12, relative corner radius 0.5, centered text, and `SamsungOneUI600` at 16. Background/text use the `PRIMARY`/`ON_PRIMARY` color tokens. Configured defaults may differ, and unavailable fonts may fall back.

## Events and accessibility

The inherited `ClickedSignal()` callback is `void(View, InputEvent)`. Connect member callbacks using a connection-tracked controller. The application owns the action; TextButton has no separate toggle-state API. Disable it with `SetEnabled(false)`.

The accessibility role is `BUTTON`. Visible text supplies the default name; an explicit `SetAccessibilityName()` takes precedence. The internal Label is hidden from the accessibility tree to avoid duplicate reading, and enabled-state changes are synchronized. Verify actual focus navigation and screen-reader speech in the application's runtime environment.

## References

- [Public API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/text-button.h)
- [TextButtonStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/text-button-style.h)
- [Manual example source](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-text-button-basics.cpp)
- [Manual test scenario (Korean)](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-text-button-basics.md)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-TextButton.cpp)
- [Shared style contracts](https://github.sec.samsung.net/NUI/dali-ui/wiki/ComponentConsistency)
