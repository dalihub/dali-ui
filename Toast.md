[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Toast-(kr))

# DALi UI Components - Toast

`Dali::Ui::Toast` presents a transient, non-modal message above a specific
`Window`. Its behavior and visual proportions follow the OneUI Snackbar model,
while its API, immutable `ToastStyle`, and handle/body implementation follow
DALi UI component conventions.

Toast is not a `Navigator` page or a dialog. `Post(window)` attaches it to that
Window's overlay layer; `Dismiss()` removes it after the exit animation.

## 1. Setup and basic use

Apply the component configuration before creating styles or entering the main
loop:

```cpp
#include <dali-ui-components/dali-ui-components.h>

Components::UiConfig::New().Apply();

Toast toast = Toast::New("Saved", Toast::Duration::SHORT);
toast.Post(window);
```

`Post()` intentionally requires an initialized `Window`. There is no no-argument
overload and an empty Window triggers a DALi assertion.

## 2. Duration and dismissal

The built-in millisecond values are:

| Value | Duration |
|---|---:|
| `Toast::Duration::SHORT` | 1500 ms |
| `Toast::Duration::LONG` | 2500 ms |
| `Toast::Duration::INDEFINITE` | No automatic timeout |

```cpp
toast.SetDuration(Toast::Duration::LONG);
toast.Post(window);

// May be called while posting, shown, or expanding.
toast.Dismiss();
```

The timeout starts when posting begins and the first action click stops it.
Calling `Post(window)` again on the same Toast replaces its active presentation
without emitting `HiddenSignal()` for the superseded presentation.

## 3. Optional two-stage action

The action button defaults to the text `Button`. Set an empty string to hide it.

```cpp
toast.SetActionButtonText("Details");
toast.ActionButtonClickedSignal().Connect(
  this,
  [](Toast toast, Toast::ActionStage stage) {
    if(stage == Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE)
    {
      // The first click is followed by expansion.
    }
    else
    {
      // The second click is followed by dismissal.
    }
  });
```

The first action click emits `CONFIRM_IN_DEFAULT_MODE`, stops the current
timeout, and expands the Toast. The second emits
`CONFIRM_IN_EXPANDED_MODE` and dismisses it.

## 4. Lifecycle signals

```cpp
toast.ShownSignal().Connect(this, &Controller::OnToastShown);
toast.HiddenSignal().Connect(this, &Controller::OnToastHidden);
```

- `ShownSignal()` is emitted once per post lifecycle after visible content is
  ready. If expansion interrupts posting, expansion completion emits it.
- `HiddenSignal()` is emitted after explicit, timeout, action, or external
  removal cleanup completes.
- Destroying an already detached Toast does not emit lifecycle signals.

Different Toast instances may coexist because this component does not provide
a global queue or manager.

## 5. Content properties

The following properties can be changed before or during presentation:

```cpp
toast.SetText("Network connection restored");
toast.SetTextColor(UiColor(0xFFFFFFu));
toast.SetFontSize(28.0f);
toast.SetFontFamily("SamsungOneUI400");
toast.SetActionButtonText("Open");
toast.SetActionButtonTextColor(UiColor(0x010102u));
toast.SetItemSpacing(20.0f);
```

## 6. ToastStyle

Clone the configured default and build an immutable style:

```cpp
ToastStyle style = ToastStyle::Default()
                     .Configure()
                     .SetBackgroundColor(UiColor(0x23465Du))
                     .SetTextColor(UiColor(0xFFFFFFu))
                     .SetCornerRadius(Vector4(20.0f, 20.0f, 20.0f, 20.0f))
                     .SetMaximumWidthRatio(0.75f)
                     .Build();

Toast toast = Toast::New("Custom style", Toast::Duration::LONG, style);
```

`ToastStyle` contains geometry, padding, spacing, color, typography, shadow,
border, and action-button style values. Transition durations and easing are
intrinsic Toast behavior and are deliberately not style properties.

The default maximum width is 68% of the Window width. A non-zero absolute
maximum additionally caps it. The default bottom offset is 5% of Window
height. Window resize recomputes these values.

## 7. Accessibility and localization

The root uses the `NOTIFICATION` accessibility role and is not independently
highlightable. The message Label is hidden from the accessibility tree while
the optional action remains operable.

Toast has no Toast-specific translation type. Bind application resources to
its ordinary setters with `UiLocalizationManager`:

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

The same mechanism can bind the action-button text.

## 8. Sample and GBS package

The interactive sample covers short, long, indefinite, action-less, custom
style, explicit dismissal, and signal-count paths.

```sh
cmake -S samples/toast -B samples/toast/build
cmake --build samples/toast/build

gbs build -A armv7l --include-all --packaging-dir samples/toast/packaging
```

The package id is `com.samsung.dali.toast`; the executable is `toast.example`.

---

[← Back to Components](Components.md)
