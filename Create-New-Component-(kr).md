# Create New Component

이 문서는 DALi UI 기반 컴포넌트를 제작하고 배포할 때 반드시 따라야 하는 컴포넌트 개발 패턴을 설명합니다.

<br/>

## Base Class And Impl

Public component handle은 컴포넌트의 동작에 맞는 foundation class를 상속합니다. Internal implementation은 반드시 대응되는 impl class를 상속해야 합니다.

| 사용 목적 | Public base | Internal impl base |
|---|---|---|
| 기본 visual/container component | `View` | `ViewImpl` |
| click/focus 등 interaction이 필요한 component | `InteractiveView` | `Provider::InteractiveViewImpl` |
| selection 상태가 필요한 component | `SelectableView` | `Provider::SelectableViewImpl` |
| single-selection(radio) group 멤버 component | `GroupSelectableView` | `Provider::GroupSelectableViewImpl` |

Public handle과 internal impl의 상속 계층을 맞춥니다.

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

이 foundation class들은 단계마다 이전 단계 위에 동작을 더하는 계층을 이룹니다. `InteractiveView`는 click/focus 처리를, `SelectableView`는 selected 상태를, `GroupSelectableView`는 멤버 중 하나만 선택되는 single-selection(radio) grouping을 더합니다. 하위 class는 상위 class의 API를 그대로 상속·노출합니다.

컴포넌트 고유 상태와 동작은 impl class에 둡니다. Public component class는 lightweight handle로 유지합니다.

<br/>

## Component Style

스타일은 컴포넌트의 visual/layout 기본값을 하나의 재사용 가능한 객체로 묶는 방식입니다. Provider는 미리 정의된 스타일을 배포할 수 있고, 제품이나 앱은 필요한 스타일을 선택하여 일관된 컴포넌트 appearance를 만들 수 있습니다.
또한 Provider는 `UiConfig`의 style sheet를 통해 컴포넌트의 기본 스타일을 설정할 수 있습니다.

<br/>

### Style Class Pattern

컴포넌트 style class는 immutable `UiStyle` handle로 정의합니다. Style 관련 public/provider/integration/internal 파일은 각 API level의 `styles/` 디렉터리 아래에 둡니다.

`DefaultKey()`와 `Default()`는 뒤에서 설명할 `StyleSheet` override를 지원하기 위한 API입니다. `DefaultPreset()`은 override가 없을 때 사용할 cached built-in 기본 style을 제공합니다.

Style class는 다음 형태를 따릅니다.

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

각 API의 의미는 다음과 같습니다.

| API | 의미 |
|---|---|
| `DefaultKey()` | `StyleSheet`에서 이 컴포넌트의 default style entry 식별 |
| `DefaultPreset()` | `StyleSheet` override가 없을 때 사용하는 cached built-in 기본 preset |
| `Default()` | `StyleSheet` override를 조회하고 없으면 `DefaultPreset()` 반환 |
| `DownCast()` | generic handle이 이 concrete style type인지 type check |
| `StaticDownCast()` | `UiStyleSheet`에서 이미 type check가 끝난 style을 반복 `dynamic_cast` 없이 concrete handle로 생성 |
| `Configure()` | 기존 immutable style을 mutable builder로 clone |
| `Builder()` | built-in 기본값에서 시작해 complete style 생성 |

`DefaultKey()`는 이 컴포넌트의 default style entry에 사용할 typed style sheet key를 반환합니다. Key는 static storage에서 한 번만 할당합니다.

```cpp
UiStyleKey<TextButtonStyle> TextButtonStyle::DefaultKey()
{
  static UiStyleKey<TextButtonStyle> key = UiStyleKey<TextButtonStyle>::Alloc();
  return key;
}
```

`DefaultPreset()`은 컴포넌트의 built-in 기본 style을 반환하는 static cached accessor입니다. 호출할 때마다 새 style 객체를 만들어서는 안 됩니다. 초기화된 style 객체는 모두 `UiConfig::Apply()` 이후에만 생성하거나 접근할 수 있습니다.

```cpp
TextButtonStyle TextButtonStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();

  static TextButtonStyle style = TextButtonStyle::Builder().Build();
  return style;
}
```

`Default()`는 현재 config에서 override style을 조회하고, 등록된 override가 없으면 `DefaultPreset()`을 반환합니다. 이 함수는 `UiConfig::Apply()` 이후에만 호출할 수 있습니다.

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

`DownCast()`와 `StaticDownCast()`는 style class를 `UiStyleSheet::GetStyle(UiStyleKey<T>)`와 함께 사용하기 위해 필요합니다. `DownCast()`는 최초 runtime type check에 사용합니다. `StaticDownCast()`는 `UiStyleSheet`에서 이미 type check가 끝난 뒤 사용하므로 cached style 조회에서 `dynamic_cast`를 반복하지 않습니다.

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

중첩 `Builder`는 mutable builder이며 handle object가 아닙니다.

규칙:

- `New()`가 아니라 constructor를 사용합니다.
- builder getter는 제공하지 않습니다.
- `Builder()`는 built-in 기본값에서 시작합니다.
- `Build()`는 complete immutable style을 반환합니다.
- `Configure()`는 기존 style을 일부 수정하기 위해 builder로 clone합니다.

예:

```cpp
auto style = TextButtonStyle::Builder()
               .SetTextColor(UiColor::ON_SURFACE)
               .Build();
```

<br/>

### Construction-Time Style Application

Style은 컴포넌트 생성 시점에 적용합니다.

다음과 같은 factory overload를 선호합니다.

```cpp
auto button = TextButton::New();
auto styledButton = TextButton::New("OK", style);
```

초기 style은 컴포넌트 구현 내부의 typed private helper에서 적용합니다.
`ViewImpl`에 untyped `UiStyle` 초기화 hook을 추가하지 않습니다.
Style sheet는 이미 `UiStyleKey<T>` 조회 시 type을 검증하고, 컴포넌트 factory도
concrete style type을 받기 때문입니다.

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

컴포넌트가 생성 이후 사용자가 직접 설정한 property를 따로 추적하지 않는다면 runtime `SetStyle()` API를 추가하지 않습니다. 나중에 style을 다시 적용할 때 사용자가 설정한 값을 보존할지 덮어쓸지 판단하려면 property별 dirty flag가 필요하고, 메모리와 정책 복잡도가 커집니다.

대부분의 컴포넌트에서:

- min/max size, padding, shape, background는 style-only property가 될 수 있습니다.
- 자주 변경되는 content property는 component setter도 함께 제공할 수 있습니다.
- runtime setter는 컴포넌트의 실제 상태를 직접 업데이트해야 합니다.

<br/>

## Stateful Style

Stateful style은 컴포넌트 state에 따라 달라지는 style 속성을 표현하는 방식을 다룹니다. 예를 들어 pressed 상태의 background color, disabled 상태의 text color처럼 동일한 style property가 state별로 다른 값을 가질 수 있는 경우를 정의합니다.

TBD

<br/>

## Style Sheet

사용자는 `Components::UiConfig`가 가진 style sheet를 통해 컴포넌트들의 기본 스타일을 override할 수 있습니다.
Components를 사용하는 앱에서는 `Components::UiConfig` 하나만 적용하고, foundation
`UiConfig`를 따로 적용하지 않습니다.

아래 코드는 style sheet를 사용하는 방법을 보여줍니다.

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

이후 `TextButton`은 config에 등록된 기본 스타일을 `TextButtonStyle::Default()`로 가져오고, 기본 `TextButton` 생성 시 이 `Default()` 스타일을 사용합니다.

`StyleSheet()`가 일반 앱 override 경로입니다. Config preset이 이미 제공하는 sheet에서 시작해 `Apply()` 전에 필요한 entry만 바꿉니다. `ResetStyleSheet()`와 `StyleSheet::New()`는 provider가 sheet 전체를 교체해야 할 때 사용할 수 있습니다.

> [!WARNING]
> style sheet는 `UiConfig::Apply()` 시점에 freeze됩니다. Freeze된 style sheet는 더 이상 변경할 수 없으므로, 필요한 style entry를 모두 등록한 뒤 config에 적용해야 합니다.

> [!WARNING]
> `UiStyleSheet`는 style 객체가 아니라 style creator를 저장합니다. Style creator는 `Default()` 객체를 생성하는 역할로 쓰입니다. 그렇기 때문에 style creator 안에서 해당 style의 `Default()`를 호출하면 recursion이 발생합니다. dali-ui는 style sheet 안에서 recursion을 감지하면 assert를 발생시킵니다.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
