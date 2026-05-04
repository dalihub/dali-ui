[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Configuration-(kr))

# Configuration

dali-ui requires you to create a Config object and call `Apply()` at application startup.
Until `Apply()` is called, key features including unit literals (`_spx`, `_dp`, `_sdp`) will
not work correctly. Once `Apply()` is called, the configuration is frozen and cannot be
changed at runtime.

<br/>

## Which Config should I use?

The Config class depends on which library you are using.

| Library | Config Class | Header |
|---|---|---|
| `dali-ui-foundation` | `UiConfig` | `<dali-ui-foundation/public-api/ui-config.h>` |
| `dali-ui-components` | `UiComponentConfig` | `<dali-ui-components/public-api/ui-component-config.h>` |

`UiComponentConfig` inherits from `UiConfig` and supports all the same settings.
If you are using `dali-ui-components`, always use `UiComponentConfig`.

<br/>

## Basic Usage

<ins>Example</ins>

```cpp
int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  // Apply() must be called before application.MainLoop().
  UiConfig::New()
    .SetDpi(320)
    .SetScalingFactor(1.5f)
    .Apply();

  MyAppController controller(application);
  application.MainLoop();
  return 0;
}
```

When using `dali-ui-components`:

```cpp
UiComponentConfig::New()
  .SetDpi(320)
  .SetScalingFactor(1.5f)
  .Apply();
```

For a full working example, see the [hello-world sample](https://github.com/dalihub/dali-ui/tree/main/samples/hello-world).

<br/>

## Configuration Options

The following describes key configuration options. For the full API, see [UiConfig](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1UiConfig.html), [UiComponentConfig](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1UiComponentConfig.html).

| Option | Method | Default | Description |
|---|---|---|---|
| Scaling Factor | `SetScalingFactor(float)` | `1.0f` | Scale multiplier applied to `spx` and `sdp` units |
| DPI | `SetDpi(int)` | `160` | Physical DPI of the target display |
| Baseline DPI | `SetBaselineDpi(int)` | `160` | Reference DPI used for `dp` calculations |
| Default Font Size | `SetDefaultFontSize(float)` | `16.0f` | Default font size for text elements (pt) |
| Default Text Color | `SetDefaultTextColor(Vector4)` | `Color::BLACK` | Default color for text elements |
| Key Click Policy | `SetKeyClickPolicy(KeyClickPolicy)` | — | Determines whether the Clicked event fires on key press or key release |
| Execution Key Predicate | `SetExecutionKeyPredicate(fn)` | `"Return"` | Function that determines which key triggers a click |
| Min Long Press Key Count | `SetMinLongPressKeyCount(uint32_t)` | — | Minimum key repeat count to trigger a long-press |
| Tap Recognizer Time | `SetTapRecognizerTime(uint32_t)` | — | Time limit for tap recognition (ms) |
| Broken Image URL | `SetBrokenImageUrl(type, url)` | — | Fallback image shown on load failure (SMALL/NORMAL/LARGE) |
| Focus Clear on Escape | `EnableFocusClearOnEscape(bool)` | — | Whether to clear focus when Escape is pressed |
| Always Show Focus | `SetAlwaysShowFocus(bool)` | — | Whether to always show the focus indicator |

<br/>

## For Framework Library Developers

If you are building a platform- or device-specific framework library, you can subclass `UiConfig`
or `UiComponentConfig` to define a pre-configured Config class and distribute it to app developers.

```cpp
// TVConfig.h (defined and distributed by the Framework developer)
class TVConfig : public Dali::Ui::UiComponentConfig
{
public:
  static TVConfig New()
  {
    return UiComponentConfig::New()
      .SetDpi(72)
      .SetScalingFactor(2.0f)
      .SetDefaultFontSize(28.0f);
  }
};
```

App developers can then apply the platform configuration with a single line:

```cpp
TVConfig::New().Apply();
```

<br/>

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)