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

Component-specific state and behavior belong in the impl class. Public component classes remain lightweight handles.

<br/>

## Component Style

A component style groups a component's visual and layout defaults into a reusable object. Providers can distribute predefined styles, while products or applications can choose the style they need to keep component appearance consistent.
Providers can also set a component's default style through the style sheet in `UiConfig`.

<br/>

### Style Class Pattern

A component style class is an immutable `UiStyle` handle. A style is a complete object and is created after `UiConfig::Apply()`.

`DefaultKey()` and `Default()` exist to support `StyleSheet` overrides described later. `Builtin()` provides the component's default style when no override is registered.

A style class should provide:

```cpp
class TextButtonStyle : public UiStyle
{
public:
  class Builder;

  static UiStyleKey DefaultKey();

  static TextButtonStyle Builtin();
  static TextButtonStyle Default();
  static TextButtonStyle DownCast(BaseHandle handle);

  Builder Configure() const;
};
```

Use this meaning for each API:

| API | Meaning |
|---|---|
| `DefaultKey()` | Identifies this component's default style entry in a `StyleSheet` |
| `Builtin()` | Built-in default style used when there is no `StyleSheet` override |
| `Default()` | Resolves a `StyleSheet` override, falling back to `Builtin()` |
| `Configure()` | Clones an existing immutable style into a mutable builder |
| `Builder()` | Starts from built-in defaults and creates a complete style |

`Builtin()`, `Default()`, and `Builder()` require `UiConfig::Apply()` to have already completed.

`Builtin()` is a static cached accessor. It must not create a new style object on every call.

```cpp
TextButtonStyle TextButtonStyle::Builtin()
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle::Builtin() requires UiConfig::Apply()");

  static TextButtonStyle style = TextButtonStyle::Builder().Build();
  return style;
}
```

`Default()` resolves an override from the current config. If no override is registered, it returns `Builtin()`.

```cpp
TextButtonStyle TextButtonStyle::Default()
{
  DALI_ASSERT_ALWAYS(UiConfig::HasCurrent() && "TextButtonStyle::Default() requires UiConfig::Apply()");

  TextButtonStyle style = TextButtonStyle::DownCast(UiConfig::GetCurrent().GetStyle(DefaultKey()));
  if(style)
  {
    return style;
  }
  return Builtin();
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

Users can write components' default styles in a style sheet and set them through `UiConfig`.

The following code shows how to use a style sheet:

```cpp
auto styleSheet = Components::StyleSheet::New();
styleSheet.SetStyle(TextButtonStyle::DefaultKey(), []() -> UiStyle
{
  return TextButtonStyle::Builder()
           .SetBackgroundColor(UiColor::SURFACE)
           .SetTextColor(UiColor::ON_SURFACE)
           .Build();
});

UiConfig config = UiConfig::New();
config.SetStyleSheet(styleSheet);
config.Apply();
```

After that, `TextButton` gets the default style registered in the config through `TextButtonStyle::Default()`, and uses this `Default()` style for default `TextButton` creation.

> [!WARNING]
> The style sheet set on `UiConfig` is frozen when `UiConfig::Apply()` is called. A frozen style sheet cannot be changed, so register all required style entries before applying the config.

> [!WARNING]
> `UiStyleSheet` stores style creators, not style objects. A style creator is used to create a `Default()` style object. Calling the same style's `Default()` from inside its style creator causes recursion. dali-ui asserts when it detects recursion inside a style sheet.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
