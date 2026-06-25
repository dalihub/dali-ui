[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Color-&-Theme-(kr))

## UiColor

`UiColor` represents a color value in two ways.

**Direct RGBA:**
```cpp
UiColor red(1.0f, 0.0f, 0.0f);          // RGB, alpha = 1.0
UiColor semi(1.0f, 0.0f, 0.0f, 0.5f);   // RGBA
UiColor hex(0xFF5733);                   // 0xRRGGBB, alpha = 1.0
UiColor hex2(0xFF5733, 0.8f);            // 0xRRGGBB + alpha
```

**Token-based (resolved from theme at runtime):**
```cpp
UiColor primary("Primary");  // looked up from the theme color table
```

When a token-based `UiColor` is passed to `SetBackgroundColor()` or similar methods,
the color is automatically refreshed whenever the theme changes.

<br/>

## Predefined Color IDs

dali-ui provides a small set of predefined color IDs common across platforms:

```cpp
view.SetBackgroundColor(UiColor::PRIMARY);     // "Primary"
view.SetBackgroundColor(UiColor::BACKGROUND);  // "Background"
view.SetBackgroundColor(UiColor::OUTLINE);     // "Outline"
```

Additional color IDs for specific platforms or devices should be defined in platform extension libraries.
See the [Framework Developer: Extending Color IDs](https://github.sec.samsung.net/NUI/dali-ui/wiki/Color-&-Theme#extending-color-ids-for-framework-developers) section.

<br/>

## Alpha Adjustment: ScaleAlpha / WithAlpha

You can adjust the alpha of a token color without defining a separate token.
Prefer these methods over registering multiple tokens for the same color with different alpha values.

```cpp
// "Primary" color at 50% alpha
view.SetBackgroundColor(UiColor::PRIMARY.ScaleAlpha(0.5f));

// "Primary" color with alpha fixed at 0.3
view.SetBackgroundColor(UiColor::PRIMARY.WithAlpha(0.3f));
```

> [!WARNING]
> **Recommendation**: Color table lookup has a cost that grows with table size.
> Avoid registering multiple tokens for the same color that differ only in alpha.
> Use `ScaleAlpha` / `WithAlpha` to keep the token count small.

<br/>

## Detecting Theme Changes

dali-ui does not provide an API to switch themes directly.
Theme changes are driven by the platform's `ThemeLoaderInterface` implementation,
which listens to system events (e.g. vconf, DBus) and emits `ThemeChangedSignal`.

To react to theme changes in your app, connect to `UiThemeManager`:

```cpp
UiThemeManager::Get().ThemeChangedSignal().Connect(this, &MyApp::OnThemeChanged);

void MyApp::OnThemeChanged()
{
  auto themeId = UiThemeManager::Get().GetCurrentThemeId();
  // handle theme-specific logic
}
```

Views that have token-based `UiColor` bindings are refreshed automatically on theme change
and do not require manual signal handling.

<br/>

## Providing a Custom Theme Loader (for Framework Developers)

To provide a device-specific theme, implement `ThemeLoaderInterface` and wire it up
by overriding `UiConfigImpl::CreateThemeLoader()`.

Below describes a simple example to show how it works. For the working sample, please refer this: [samples/color-controls](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/color-controls)

**1. Implement ThemeLoaderInterface (Example)**

```cpp
// tv-theme-loader.h
class TvThemeLoader : public Dali::Ui::ThemeLoaderInterface
{
public:
  TvThemeLoader()
  {
    Vconf.NotifyKeyChanged("db/menu/system/accessibility/highcontrast", &HighContrastChangedCallback);
  }

  bool GetColor(Dali::StringView colorId, Dali::Vector4& outColor) override
  {
    // e.g. read table from vconf, a DB, a file or a program data area and look up colorId
    return mColorTable.Lookup(colorId, outColor);
  }

  Dali::String GetCurrentThemeId() const override
  {
    return mCurrentThemeId;
  }

  ThemeChangedSignalType& ThemeChangedSignal() override
  {
    return mThemeChangedSignal;
  }

  void NotifyThemeChanged(const Dali::String& newThemeId)
  {
    mCurrentThemeId = newThemeId;
    // update color table, then notify
    mThemeChangedSignal.Emit();
  }

private:
  Dali::String            mCurrentThemeId;
  ThemeChangedSignalType  mThemeChangedSignal;
  // ...
};
```

**2. Subclass UiConfigImpl and override CreateThemeLoader()**

```cpp
// tv-config-impl.h (integration-api level)
class TvConfigImpl : public Dali::Ui::Integration::UiConfigImpl
{
public:
  static Dali::IntrusivePtr<TvConfigImpl> New() { return new TvConfigImpl(); }

  Dali::Ui::ThemeLoaderInterface* CreateThemeLoader() override
  {
    return new TvThemeLoader();
  }
};
```

**3. Connect TvConfigImpl to TVConfig**

Combining with the `TVConfig` pattern introduced in [Configuration](링크):

```cpp
// tv-config.h (public-api level)
class TVConfig : public Dali::Ui::UiConfig
{
public:
  static TVConfig New()
  {
    auto* impl = TvConfigImpl::New().Get();
    TVConfig config(impl);
    config.SetDpi(72);
    config.SetScalingFactor(2.0f);
    return config;
  }

private:
  explicit TVConfig(Dali::Ui::Integration::UiConfigImpl* impl)
  : UiConfig(impl) {}
};
```

App developers use it the same way:
```cpp
TVConfig::New().Apply();
```

<br/>

## Extending Color IDs (for Framework Developers)

Define platform-specific color IDs in your extension library and distribute them to app developers:

```cpp
// tv-colors.h
struct TVColors
{
  static const Dali::Ui::UiColor SURFACE_ON_FOCUSED;
  static const Dali::Ui::UiColor SURFACE_ON_SELECTED;
  // ...
};

// tv-colors.cpp
const Dali::Ui::UiColor TVColors::SURFACE_ON_FOCUSED(Dali::String("SurfaceOnFocused"));
const Dali::Ui::UiColor TVColors::SURFACE_ON_SELECTED(Dali::String("SurfaceOnSelected"));
```

App developers use them as follows:
```cpp
view.SetBackgroundColor(TVColors::SURFACE_ON_FOCUSED);
view.SetBackgroundColor(TVColors::SURFACE_ON_FOCUSED.ScaleAlpha(0.5f));
```

> [!WARNING]
> **Caution**: Every token color incurs a table lookup on each color application.
> Avoid registering multiple tokens for the same color that differ only in alpha.
> Use `ScaleAlpha` / `WithAlpha` to minimize the number of tokens.

<br/>

## Color Override (for App Developers)

To override specific colors at runtime regardless of the current theme,
use `UiColorManager::SetColorOverride()`:

```cpp
bool MyOverride(Dali::StringView id, Dali::Vector4& out)
{
  if(id == "Primary") { out = Dali::Vector4(1, 0, 0, 1); return true; }
  return false;  // fall through to theme for all other colors
}

UiColorManager::Get().SetColorOverride(MyOverride);

// Remove override
UiColorManager::Get().ClearColorOverride();
```

Setting an override immediately refreshes all existing View color bindings.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
