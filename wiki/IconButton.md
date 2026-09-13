[한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/IconButton-%28kr%29) · [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)

# IconButton

`Dali::Ui::IconButton` is an `InteractiveView` that represents an action with one icon. It has no visible text label, so the application must provide an accessible name describing the action. Use TextButton for a labeled button.

## Setup and basic use

Apply the Components configuration once during application initialization before resolving presets or creating controls. The parent View in this example must already be initialized. The back action itself belongs to the application's navigation policy.

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

void ConfigureComponents()
{
  Components::UiConfig::New().Apply();
}

void OnBackClicked(View sender, InputEvent event)
{
  // Ask the application navigator to go back.
}

IconButton AddBackButton(View parent)
{
  auto button = IconButton::New(IconButtonStyle::BackPreset());
  button.SetAccessibilityName("Back");
  button.ClickedSignal().Connect(&OnBackClicked);
  parent.Add(button);
  return button;
}
```

Factories are `New()`, `New(style)`, `New(iconUrl)`, and `New(iconUrl, style)`. Omitting the style resolves `IconButtonStyle::Default()` from the current configuration. `IconButton()` creates an empty handle; `DownCast(BaseHandle)` returns empty when the type does not match.

## Presets and styles

| Factory | Icon | Requested icon dimensions | Padding |
|---|---|---|---|
| `DefaultPreset()` | Empty URL | 56×56 | 0 |
| `AddPreset()` | Built-in add icon | 56×56 | 0 |
| `BackPreset()` | Built-in back icon | 40×40 | 6 per side |
| `MorePreset()` | Built-in more icon | 40×40 | 6 per side |
| `Builder()` | Empty URL | `WRAP_CONTENT` on both axes | 0 |

These are built-in values; `Default()` can return an application-registered style. Icon dimensions are not the total control size: padding, parent constraints, and UI scaling also contribute. Built-in icons require the installed component image assets.

`Configure()` returns an independent builder without modifying the original. Builders are move-only and consumed by `Build()`. Style fields are icon URL, width, height, color, and control padding. Runtime icon rounding uses `SetIconCornerRadius()`; it is not an IconButtonStyle builder field.

## Sizing and image updates

Each icon axis follows **explicit size → valid natural size → 56 logical-unit fallback**. Presets already specify explicit sizes; changing only the URL does not enable natural sizing.

```cpp
IconButton AddNaturalIcon(View parent, const Dali::String& iconUrl)
{
  auto style = IconButtonStyle::Default().Configure()
    .SetIconUrl(iconUrl)
    .SetIconWidth(WRAP_CONTENT)
    .SetIconHeight(WRAP_CONTENT)
    .SetPadding(Insets(4.0f, 4.0f))
    .Build();
  auto button = IconButton::New(style);
  button.SetAccessibilityName("Open details");
  parent.Add(button);
  return button;
}

void UpdateIcon(IconButton button, const Dali::String& iconUrl)
{
  button.SetIconUrl(iconUrl);
  button.SetIconWidth(32.0f);
  button.SetIconHeight(WRAP_CONTENT);
  button.SetIconColor(UiColor(UiColor::PRIMARY));
  button.SetIconCornerRadius(Vector4(2.0f, 2.0f, 2.0f, 2.0f));
}
```

- Omitted dimensions in a raw `Builder()` are `WRAP_CONTENT`. When copying a preset, explicitly reset the desired axes as shown above.
- Use the image's valid natural size when ready. The 56 fallback is not a minimum: a ready 32×16 image remains 32×16 before padding and scaling.
- One positive fixed axis and a `WRAP_CONTENT` axis preserve the image ratio. A 32×16 image requested at width 64 resolves height 32.
- Explicit zero is a zero size, not an unset value. This differs from CheckBox's icon-zero semantics.
- `MATCH_PARENT` retains its parent-layout meaning.
- Asynchronous resource completion and URL changes invalidate measurement so the temporary fallback does not become a permanent fixed size.
- `GetIconWidth/Height()` return requested values or sentinels, not measured dimensions.

Dimensions must be finite non-negative values or `WRAP_CONTENT`/`MATCH_PARENT`. Padding and corner-radius components must be finite and non-negative; invalid input asserts. Pass a resource URL that is actually deployed with the application.

## Events and accessibility

`ClickedSignal()` is inherited from InteractiveView with callback `void(View, InputEvent)`. The application performs the action; IconButton has no built-in selected state or navigation command. `SetEnabled(false)` disables activation and clears the accessibility enabled state; re-enabling restores it.

The accessibility role is `BUTTON`; the internal image is hidden from the accessibility tree. Set an action-oriented name such as `SetAccessibilityName("Back")` and localize it through application resources. Check touch-target size, focus navigation, and actual screen-reader speech in the target environment.

## References

- [IconButton API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/icon-button.h)
- [IconButtonStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/icon-button-style.h)
- [Sample](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/icon-button)
- [Sample instructions (Korean)](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/icon-button/README.md)
- [UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-IconButton.cpp)
- [TextButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/TextButton)
