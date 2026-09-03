#include <dali-ui-components/internal/component-image-path.h>
#include <dali-ui-components/internal/styles/icon-button-style-impl.h>
#include <dali-ui-components/public-api/styles/icon-button-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-debug.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

#include <utility>

namespace Dali::Ui
{
namespace
{
IconButtonStyle CreateStyle(Dali::String iconUrl, float width, float height)
{
  return IconButtonStyle(new Internal::IconButtonStyleImpl(std::move(iconUrl), width, height));
}
} //namespace

UiStyleKey<IconButtonStyle> IconButtonStyle::DefaultKey()
{
  static UiStyleKey<IconButtonStyle> key = UiStyleKey<IconButtonStyle>::Alloc();
  return key;
}
IconButtonStyle IconButtonStyle::DefaultPreset()
{
  DebugAssertStyleConfigApplied();
  static IconButtonStyle style = CreateStyle({}, 56.0f, 56.0f);
  return style;
}
IconButtonStyle IconButtonStyle::Default()
{
  DebugAssertStyleConfigApplied();
  IconButtonStyle style = UiConfig::GetCurrent().GetStyle(DefaultKey());
  return style ? style : DefaultPreset();
}
IconButtonStyle IconButtonStyle::AddPreset()
{
  DebugAssertStyleConfigApplied();
  static IconButtonStyle style = CreateStyle(Internal::ResolveComponentImageUrl("ic_add.svg"), 56.0f, 56.0f);
  return style;
}
IconButtonStyle IconButtonStyle::BackPreset()
{
  DebugAssertStyleConfigApplied();
  static IconButtonStyle style = IconButtonStyle::Builder()
                                   .SetIconUrl(Internal::ResolveComponentImageUrl("ic_navi_back.svg"))
                                   .SetIconWidth(40.0f)
                                   .SetIconHeight(40.0f)
                                   .SetPadding(Insets(6.0f, 6.0f))
                                   .Build();
  return style;
}
IconButtonStyle IconButtonStyle::MorePreset()
{
  DebugAssertStyleConfigApplied();
  static IconButtonStyle style = IconButtonStyle::Builder()
                                   .SetIconUrl(Internal::ResolveComponentImageUrl("ic_more.svg"))
                                   .SetIconWidth(40.0f)
                                   .SetIconHeight(40.0f)
                                   .SetPadding(Insets(6.0f, 6.0f))
                                   .Build();
  return style;
}
IconButtonStyle IconButtonStyle::DownCast(BaseHandle handle)
{
  return IconButtonStyle(dynamic_cast<Internal::IconButtonStyleImpl*>(handle.GetObjectPtr()));
}
IconButtonStyle IconButtonStyle::StaticDownCast(UiStyle style)
{
  return IconButtonStyle(static_cast<Internal::IconButtonStyleImpl*>(style.GetObjectPtr()));
}
Dali::String IconButtonStyle::GetIconUrl() const
{
  return Internal::GetImpl(*this).GetIconUrl();
}
float IconButtonStyle::GetIconWidth() const
{
  return Internal::GetImpl(*this).GetIconWidth();
}
float IconButtonStyle::GetIconHeight() const
{
  return Internal::GetImpl(*this).GetIconHeight();
}
UiColor IconButtonStyle::GetIconColor() const
{
  return Internal::GetImpl(*this).GetIconColor();
}
Insets IconButtonStyle::GetPadding() const
{
  return Internal::GetImpl(*this).GetPadding();
}
IconButtonStyle::IconButtonStyle(Internal::IconButtonStyleImpl* impl)
: UiStyle(impl)
{
}

IconButtonStyle::Builder::Builder()
: mImpl(new Internal::IconButtonStyleImpl())
{
}
IconButtonStyle::Builder::Builder(Builder&& rhs) noexcept                             = default;
IconButtonStyle::Builder& IconButtonStyle::Builder::operator=(Builder&& rhs) noexcept = default;
IconButtonStyle::Builder::~Builder()                                                  = default;

#define IBS_SETTER(Name, Type)                                         \
  IconButtonStyle::Builder& IconButtonStyle::Builder::Name(Type v) &   \
  {                                                                    \
    mImpl->Name(v);                                                    \
    return *this;                                                      \
  }                                                                    \
  IconButtonStyle::Builder&& IconButtonStyle::Builder::Name(Type v) && \
  {                                                                    \
    Name(v);                                                           \
    return std::move(*this);                                           \
  }

IBS_SETTER(SetIconUrl, const Dali::String&)
IBS_SETTER(SetIconWidth, float)
IBS_SETTER(SetIconHeight, float)
IBS_SETTER(SetIconColor, const UiColor&)
IBS_SETTER(SetPadding, const Insets&)
#undef IBS_SETTER

IconButtonStyle IconButtonStyle::Builder::Build() &&
{
  DALI_ASSERT_ALWAYS(mImpl && "IconButtonStyle::Builder has already been consumed");
  IconButtonStyle style(mImpl.Get());
  mImpl.Reset();
  return style;
}
} // namespace Dali::Ui
