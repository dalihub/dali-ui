#include <dali-ui-components/internal/icon-button-impl.h>
#include <dali-ui-components/public-api/icon-button.h>

namespace DALI_NAMESPACE::Ui
{
IconButton::IconButton()  = default;
IconButton::~IconButton() = default;
IconButton IconButton::New()
{
  return New(IconButtonStyle::Default());
}
IconButton IconButton::New(IconButtonStyle style)
{
  return Internal::IconButtonImpl::New(style);
}
IconButton IconButton::New(const Dali::String& iconUrl)
{
  return New(iconUrl, IconButtonStyle::Default());
}
IconButton IconButton::New(const Dali::String& iconUrl, IconButtonStyle style)
{
  IconButton button = New(style);
  button.SetIconUrl(iconUrl);
  return button;
}
IconButton IconButton::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<IconButton, Internal::IconButtonImpl>(handle);
}
IconButton::IconButton(const IconButton& handle)
: InteractiveView(handle)
{
}
IconButton::IconButton(IconButton&& rhs) noexcept = default;
IconButton& IconButton::operator=(const IconButton& handle)
{
  InteractiveView::operator=(handle);
  return *this;
}
IconButton& IconButton::operator=(IconButton&& rhs) noexcept = default;
void        IconButton::SetIconUrl(const Dali::String& iconUrl)
{
  Internal::GetImpl(*this).SetIconUrl(iconUrl);
}
Dali::String IconButton::GetIconUrl() const
{
  return Internal::GetImpl(*this).GetIconUrl();
}
void IconButton::SetIconWidth(float width)
{
  Internal::GetImpl(*this).SetIconWidth(width);
}
float IconButton::GetIconWidth() const
{
  return Internal::GetImpl(*this).GetIconWidth();
}
void IconButton::SetIconHeight(float height)
{
  Internal::GetImpl(*this).SetIconHeight(height);
}
float IconButton::GetIconHeight() const
{
  return Internal::GetImpl(*this).GetIconHeight();
}
void IconButton::SetIconColor(const UiColor& color)
{
  Internal::GetImpl(*this).SetIconColor(color);
}
UiColor IconButton::GetIconColor() const
{
  return Internal::GetImpl(*this).GetIconColor();
}
void IconButton::SetIconCornerRadius(const Vector4& radius)
{
  Internal::GetImpl(*this).SetIconCornerRadius(radius);
}
Vector4 IconButton::GetIconCornerRadius() const
{
  return Internal::GetImpl(*this).GetIconCornerRadius();
}
IconButton::IconButton(Internal::IconButtonImpl& implementation)
: InteractiveView(implementation)
{
}
IconButton::IconButton(Dali::Internal::CustomActor* internal)
: InteractiveView(internal)
{
  VerifyCustomActorPointer<Internal::IconButtonImpl>(internal);
}
} //namespace DALI_NAMESPACE::Ui
