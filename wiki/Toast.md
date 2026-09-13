[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Toast-(kr))

# DALi UI Components - Toast

`Dali::Ui::Toast` displays brief, non-modal feedback above a specific `Window`.
It follows the One UI Toast structure with message text and an optional icon,
requires no response, and disappears after a finite duration.

Toast does not provide Snackbar actions or an expanded state. Use another
component when the user must make a choice or invoke an action.

## 1. Setup and basic use

Apply the component configuration before creating styles or entering the main
loop:

```cpp
#include <dali-ui-components/dali-ui-components.h>

Components::UiConfig::New().Apply();

Toast toast = Toast::New("Saved", Toast::Duration::SHORT);
toast.Post(window);
```

`Post()` requires an initialized `Window` and attaches the Toast to that
Window's overlay layer. An empty Window triggers a DALi assertion.

## 2. Content and duration

A Toast contains one text message and may also contain an icon. Text wraps
naturally without a fixed line-count limit. The configured maximum Toast height
constrains visible content, and overflowing text is ellipsized.

```cpp
toast.SetText("Network connection restored");
toast.SetDuration(Toast::Duration::LONG);
toast.Post(window);
```

Set an icon URL to use the icon-and-text variant. The icon is created lazily
and appears before the text. Setting an empty URL removes it from layout.

```cpp
toast.SetIconResourceUrl("images/saved.png");
toast.SetIconColor(UiColor(Color::WHITE));
toast.SetIconSynchronousLoading(false);

toast.SetIconResourceUrl(""); // Return to the text-only variant.
```

The predefined durations are:

| Value | Duration |
|---|---:|
| `Toast::Duration::SHORT` | 1500 ms |
| `Toast::Duration::LONG` | 2500 ms |

All durations are finite and must be greater than zero. As in One UI Components,
the timeout starts when `Post()` is called, in parallel with the entrance
transition. Changing the duration while a Toast is posted restarts its timeout.

## 3. Dismissal and lifecycle signals

```cpp
toast.ShownSignal().Connect(this, &Controller::OnToastShown);
toast.HiddenSignal().Connect(this, &Controller::OnToastHidden);

toast.Post(window);
toast.Dismiss();
```

- `ShownSignal()` is emitted after the entrance transition finishes.
- `HiddenSignal()` is emitted after timeout, explicit dismissal, or external
  removal cleanup finishes.
- `Dismiss()` has no effect when the Toast is already detached or dismissing.
- Reposting the same Toast replaces its active presentation without emitting
  `HiddenSignal()` for the superseded presentation.

Different Toast instances may coexist because the component does not provide a
global queue or manager.

## 4. ToastStyle

`ToastStyle` holds the visual and layout values used by Toast:

- maximum width ratio
- maximum height and Window-relative bottom offset ratio
- content padding, icon/text spacing, and corner radius
- background and text colors
- font size and family
- icon size, corner radius, and color
- shadow and borderline

Create a custom immutable style by configuring an existing style:

```cpp
ToastStyle style = ToastStyle::Default()
                     .Configure()
                     .SetMaximumWidthRatio(0.75f)
                     .SetMaximumHeight(120.0f)
                     .SetBottomOffsetRatio(0.08f)
                     .SetItemSpacing(12.0f)
                     .SetPadding(Insets(24.0f, 24.0f, 12.0f, 12.0f))
                     .SetBackgroundColor(UiColor(0x23465Du))
                     .SetTextColor(UiColor(0xFFFFFFu))
                     .SetCornerRadius(Vector4(20.0f, 20.0f, 20.0f, 20.0f))
                     .Build();

Toast toast = Toast::New("Custom style", Toast::Duration::LONG, style);
```

The built-in One UI values are:

| Value | Default |
|---|---:|
| Maximum width | 68% of Window width |
| Maximum height | 112 |
| Bottom offset | 5% of Window height |
| Padding | horizontal 32, vertical 16 |
| Icon/text spacing | 16 |
| Corner radius | 36 |
| Icon size / radius | 36 × 36 / 0 |
| Font | SamsungOneUI400, 28 |
| Borderline | width 2, offset -1, `OutlineACC` |
| Background / text | `SurfaceFixed` / `OnSurfaceContainerFixedVariantBright` |

The Toast recomputes its position and size when the Window is resized. It also
moves above a visible input panel when the focused view is an `InputField` or
`InputEditor`. Entrance and exit each use the One UI 300 ms opacity-and-vertical
motion curve; transition timing is intrinsic behavior rather than a style
property.

## 5. Accessibility and localization

The Toast uses the `NOTIFICATION` accessibility role. Its accessible name is
the message text; the internal Label is hidden from the accessibility tree to
avoid duplicate announcements.

Toast has no Toast-specific translation type. Bind application resources to
`SetText()` with `UiLocalizationManager`:

```cpp
UiLocalizationManager::Get().SetBindingResource(
  toast,
  "ToastText",
  "IDS_SAVED",
  LocalizedStringCallback::New(this, &MyApp::ApplyToastText));

void MyApp::ApplyToastText(BaseHandle target, const Dali::String& text)
{
  Toast toast = Toast::DownCast(target);
  if(toast)
  {
    toast.SetText(text);
  }
}
```

## 6. Sample and GBS package

The interactive sample covers text-only, optional-icon, short, long,
long-text, custom-style, explicit-dismissal, and lifecycle-signal paths.

```sh
cmake -S samples/toast -B samples/toast/build
cmake --build samples/toast/build

gbs build -A armv7l --include-all --packaging-dir samples/toast/packaging
```

The package id is `com.samsung.dali.toast`; the executable is `toast.example`.

---

[← Back to Components](Components.md)
