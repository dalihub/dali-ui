[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration-(kr))

# Configuration

dali-ui requires you to create a Config object and call `Apply()` at application startup.
Until `Apply()` is called, key features including unit literals (`_spx`, `_dp`, `_sdp`) will
not work correctly. Once `Apply()` is called, the configuration is frozen and cannot be
changed at runtime.

<br/>

## Which Config should I use?

Use the config preset from the highest DALi UI library your application depends
on. A components application should apply `Components::UiConfig`, because it
contains the foundation configuration plus the components style sheet preset.
Apply only one config. Do not apply both `UiConfig` and `Components::UiConfig`
in the same application.

| Library | Config Class | Header |
|---|---|---|
| `dali-ui-foundation` | `UiConfig` | `<dali-ui-foundation/public-api/ui-config.h>` |
| `dali-ui-components` | `Components::UiConfig` | `<dali-ui-components/dali-ui-components.h>` |

<br/>

## Basic Usage

<ins>Example</ins>

```cpp
int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Apply() must be called before application.MainLoop().
  UiConfig config = UiConfig::New();
  config.SetDpi(320);
  config.SetScalingFactor(1.5f);
  config.Apply();

  MyAppController controller(application);
  application.MainLoop();
  return 0;
}
```

When using `dali-ui-components`, apply a single `Components::UiConfig`.
Do not apply `UiConfig` separately; `Components::UiConfig` already covers the
foundation configuration:

```cpp
Components::UiConfig config = Components::UiConfig::New();
config.SetDpi(320);
config.SetScalingFactor(1.5f);
config.Apply();
```

For a full working example, see the [hello-world sample](https://github.com/dalihub/dali-ui/tree/main/samples/hello-world).

<br/>

## Configuration Options

The following describes key configuration options. For the full API, see [UiConfig](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1UiConfig.html).

| Option | Method | Default | Description |
|---|---|---|---|
| Scaling Factor | `SetScalingFactor(float)` | `1.0f` | Scale multiplier applied to `spx` and `sdp` units |
| DPI | `SetDpi(int)` | `160` | Physical DPI of the target display |
| Baseline DPI | `SetBaselineDpi(int)` | `160` | Reference DPI used for `dp` calculations |
| Default Font Size | `SetDefaultFontSize(float)` | `16.0f` | Default font size for text elements (pt) |
| Default Text Color | `SetDefaultTextColor(Vector4)` | `Color::BLACK` | Default color for text elements |
| Key Click Policy | `SetKeyClickPolicy(KeyClickPolicy)` | — | Determines whether the Clicked event fires on key press or key release |
| Execution Key Predicate | `SetExecutionKeyPredicate(fn)` | `"Return"` | Function that determines which key triggers a click |
| Min Long Press Key Count | `SetKeyLongPressThreshold(uint32_t)` | — | Minimum key repeat count to trigger a long-press |
| Tap Recognizer Time | `SetTapRecognizerTime(uint32_t)` | — | Time limit for tap recognition (ms) |
| Broken Image URL | `SetBrokenImageUrl(type, url)` | — | Fallback image shown on load failure (SMALL/NORMAL/LARGE) |
| Clear Focus on Escape | `SetClearFocusOnEscapeEnabled(bool)` | — | Whether to clear focus when Escape is pressed |
| Default Focus Indicator | `SetDefaultFocusIndicatorEnabled(bool)` | `IsDefaultFocusIndicatorEnabled()` | Whether the default focus indicator is enabled |
| Clear Focus Indication On Touch | `SetClearFocusIndicationOnTouch(bool)` | `IsClearFocusIndicationOnTouchEnabled()` | Whether touch interaction clears focus indication |
| Clear Focus Indication On Hover | `SetClearFocusIndicationOnHover(bool)` | `IsClearFocusIndicationOnHoverEnabled()` | Whether hovering outside the focused view clears focus indication |
| Style Sheet | `ResetStyleSheet(UiStyleSheet)`, `StyleSheet()`, `GetStyle(UiStyleKey<T>)` | Component style overrides | Component style creator lookup table and style resolver |

<br/>

## Component Style Sheets

`UiConfig` may carry a `UiStyleSheet`. `Components::UiConfig` provides the
components style sheet by default. Components resolve configured styles through
`UiConfig::GetStyle(UiStyleKey<T>)`. Style creators can be registered before
`Apply()`, but initialized style objects are created or accessed only after
`UiConfig::Apply()`.

Only the selected config should be applied. A components application should
modify the style sheet on `Components::UiConfig` and then call `Apply()` on that
same config, not apply a foundation `UiConfig` first.

Applications and product libraries usually access the style sheet on the config
preset and override only the entries they need before applying the config:

```cpp
Components::UiConfig config = Components::UiConfig::New();
config.StyleSheet()
  .SetStyle(MyButtonStyle::DefaultKey(), ProvideMyButtonStyle);
config.Apply(); // Freezes both config and its style sheet.
```

`ResetStyleSheet()` is still available when a provider wants to replace the whole
sheet. `StyleSheet::New()` is also available for that advanced provider path,
but application code should normally start from the sheet already owned by the
config preset.
Do not call component style accessors such as `TextButtonStyle::Default()` or
`TextButtonStyle::DefaultPreset()` before `Apply()`.

<br/>

## For Framework Library Developers

If you are building a platform- or device-specific framework library, you can subclass `UiConfig`
to define a pre-configured Config class and distribute it to app developers.

```cpp
// TVConfig.h (defined and distributed by the Framework developer)
class TVConfig : public Dali::Ui::UiConfig
{
public:
  explicit TVConfig(const Dali::Ui::UiConfig& config)
  : Dali::Ui::UiConfig(config)
  {
  }

  static TVConfig New()
  {
    TVConfig config(Dali::Ui::UiConfig::New());
    config.SetDpi(72);
    config.SetScalingFactor(2.0f);
    config.SetDefaultFontSize(28.0f);
    return config;
  }
};
```

App developers can then apply the platform configuration with a single line:

```cpp
TVConfig::New().Apply();
```

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
