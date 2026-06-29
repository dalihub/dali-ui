[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Create-New-Component-(kr))

# Create New Component

This document describes the component development pattern that must be followed when building and distributing components based on DALi UI.

<br/>

## Base Class And Impl

A public component handle should inherit from the foundation class that matches the component's behavior. The internal implementation must inherit from the matching impl class.

| Use case | Public base | Internal impl base |
|---|---|---|
| Basic visual/container component | `View` | `ViewImpl` |
| Clickable/focusable interactive component | `InteractiveView` | `Provider::InteractiveViewImpl` |
| Component with selectable state | `SelectableView` | `Provider::SelectableViewImpl` |
| Single-selection (radio) group member component | `GroupSelectableView` | `Provider::GroupSelectableViewImpl` |

Keep the public handle and internal impl hierarchy aligned:

```cpp
class TextButton : public InteractiveView
{
  ...
};

class TextButtonImpl : public Provider::InteractiveViewImpl
{
  ...
};
```

These foundation classes form a hierarchy where each level adds behavior on top of the previous one: `InteractiveView` adds click/focus handling, `SelectableView` adds a selected state, and `GroupSelectableView` adds single-selection (radio) grouping for members where only one can be selected. A subclass inherits and exposes its parent classes' APIs directly.

Component-specific state and behavior belong in the impl class. Public component classes remain lightweight handles.

<br/>

## Component Style

A component style groups a component's visual and layout defaults into a reusable object. Providers can distribute predefined styles, while products or applications can choose the style they need to keep component appearance consistent.
Providers can also set a component's default style through the style sheet in `UiConfig`.

<br/>

### Style Class Pattern

A component style class is an immutable `UiStyle` handle. Style-related public, provider, integration, and internal files live under the `styles/` directory of their API level.

`DefaultKey()` and `Default()` exist to support `StyleSheet` overrides described later. `DefaultPreset()` provides the component's cached built-in default style when no override is registered.

A style class should provide:

```cpp
class TextButtonStyle : public UiStyle
{
public:
  class Builder;

  static UiStyleKey<TextButtonStyle> DefaultKey();

  static TextButtonStyle DefaultPreset();
  static TextButtonStyle Default();
  static TextButtonStyle DownCast(BaseHandle handle);
  static TextButtonStyle StaticDownCast(UiStyle style);

  Builder Configure() const;
};
```

Use this meaning for each API:

| API | Meaning |
|---|---|
| `DefaultKey()` | Identifies this component's default style entry in a `StyleSheet` |
| `DefaultPreset()` | Cached built-in default preset used when there is no `StyleSheet` override |
| `Default()` | Resolves a `StyleSheet` override, falling back to `DefaultPreset()` |
| `DownCast()` | Type-checks a generic handle as this concrete style type |
| `StaticDownCast()` | Creates a concrete handle from a style already type-checked by `UiStyleSheet` without repeated `dynamic_cast` |
| `Configure()` | Clones an existing immutable style into a mutable builder |
| `Builder()` | Starts from built-in defaults and creates a complete style |

`DefaultKey()` returns the typed style sheet key for this component's default style entry. Allocate it once in static storage.

```cpp
UiStyleKey<TextButtonStyle> TextButtonStyle::DefaultKey()
{
  static UiStyleKey<TextButtonStyle> key = UiStyleKey<TextButtonStyle>::Alloc();
  return key;
}
```

`DefaultPreset()` is a static cached accessor for the component's built-in default style. It must not create a new style object on every call. Like all initialized style objects, the preset may be created or accessed only after `UiConfig::Apply()`.

```cpp
TextButtonStyle TextButtonStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();

  static TextButtonStyle style = TextButtonStyle::Builder().Build();
  return style;
}
```

`Default()` resolves an override from the current config. If no override is registered, it returns `DefaultPreset()`. `UiConfig::Apply()` must have happened before this function is called.

```cpp
TextButtonStyle TextButtonStyle::Default()
{
  DebugAssertStyleConfigApplied();

  TextButtonStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  if(style)
  {
    return style;
  }
  return DefaultPreset();
}
```

`DownCast()` and `StaticDownCast()` are required when a style class is used with `UiStyleSheet::GetStyle(UiStyleKey<T>)`. `DownCast()` performs the first runtime type check. `StaticDownCast()` is used after `UiStyleSheet` has already verified the type, so cached style lookups do not repeat `dynamic_cast`.

```cpp
TextButtonStyle TextButtonStyle::DownCast(BaseHandle handle)
{
  return TextButtonStyle(dynamic_cast<Internal::TextButtonStyleImpl*>(handle.GetObjectPtr()));
}

TextButtonStyle TextButtonStyle::StaticDownCast(UiStyle style)
{
  return TextButtonStyle(static_cast<Internal::TextButtonStyleImpl*>(style.GetObjectPtr()));
}
```

<br/>

### Builder Pattern

The nested `Builder` is a mutable builder, not a handle object.

Rules:

- Use a constructor, not `New()`.
- Do not add builder getters.
- `Builder()` starts from built-in default values.
- `Build()` returns a complete immutable style.
- `Configure()` clones an existing style for partial modification.

Example:

```cpp
auto style = TextButtonStyle::Builder()
               .SetTextColor(UiColor::ON_SURFACE)
               .Build();
```

<br/>

### Construction-Time Style Application

Style is applied to a component at construction time.

Prefer factory overloads such as:

```cpp
auto button = TextButton::New();
auto styledButton = TextButton::New("OK", style);
```

Apply the initial style inside the component implementation through a typed
private helper. Do not add an untyped `UiStyle` initialization hook to `ViewImpl`.
The style sheet already validates `UiStyleKey<T>` lookups, and the component
factory receives the concrete style type.

```cpp
TextButton TextButtonImpl::New(TextButtonStyle style)
{
  DALI_ASSERT_ALWAYS(style && "TextButtonStyle must be initialized");

  IntrusivePtr<TextButtonImpl> impl(new TextButtonImpl());
  TextButton handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}

void TextButtonImpl::ApplyInitialStyle(TextButtonStyle style)
{
  SetFontSize(style.GetFontSize());
  SetTextColor(style.GetTextColor());
}
```

Do not add a runtime `SetStyle()` API unless the component explicitly tracks which properties were set by the user after construction. Otherwise, a later style application cannot know whether it should preserve or overwrite those user-set values.

For most components:

- Style-only properties may include min/max size, padding, shape, and background.
- Frequently changed content properties may also have component setters.
- Runtime setters should directly update the component's actual state.

<br/>

## Stateful Style

Stateful style covers how to express style properties that change by component state. For example, a background color for the pressed state or a text color for the disabled state may use different values for the same style property.

TBD

<br/>

## Style Sheet

Users can override components' default styles through the style sheet owned by
`Components::UiConfig`.
Apply only `Components::UiConfig` in applications that use components; do not
apply a separate foundation `UiConfig`.

The following code shows how to use a style sheet:

```cpp
Components::UiConfig config = Components::UiConfig::New();
config.StyleSheet()
  .SetStyle(TextButtonStyle::DefaultKey(), []() -> UiStyle
  {
    return TextButtonStyle::Builder()
             .SetBackgroundColor(UiColor::SURFACE)
             .SetTextColor(UiColor::ON_SURFACE)
             .Build();
  });

config.Apply();
```

After that, `TextButton` gets the default style registered in the config through `TextButtonStyle::Default()`, and uses this `Default()` style for default `TextButton` creation.

`StyleSheet()` is the normal application override path: start from the sheet
already provided by the config preset and change selected entries before
`Apply()`. `ResetStyleSheet()` and `StyleSheet::New()` are still available when a
provider wants to replace the whole sheet.

> [!WARNING]
> The style sheet set on `UiConfig` is frozen when `UiConfig::Apply()` is called. A frozen style sheet cannot be changed, so register all required style entries before applying the config.

> [!WARNING]
> `UiStyleSheet` stores style creators, not style objects. A style creator is used to create a `Default()` style object. Calling the same style's `Default()` from inside its style creator causes recursion. dali-ui asserts when it detects recursion inside a style sheet.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
