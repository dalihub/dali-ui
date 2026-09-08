#include <dali-ui-components/internal/icon-button-impl.h>
#include <dali-ui-foundation/public-api/views/effects/overlay-effect.h>
#include <dali/devel-api/object/type-registry-helper.h>

namespace DALI_NAMESPACE::Ui::Internal
{
namespace
{
BaseHandle Create()
{
  return BaseHandle();
}
DALI_TYPE_REGISTRATION_BEGIN(IconButtonImpl, Extension::InteractiveViewImpl, Create)
DALI_TYPE_REGISTRATION_END()
} //namespace
Ui::IconButton IconButtonImpl::New(IconButtonStyle style)
{
  DALI_ASSERT_ALWAYS(style && "IconButtonStyle must be initialized");

  IntrusivePtr<IconButtonImpl> impl(new IconButtonImpl());
  Ui::IconButton               handle(*impl);
  impl->Initialize();
  impl->ApplyInitialStyle(style);
  return handle;
}
void IconButtonImpl::OnInitialize()
{
  Ui::Extension::InteractiveViewImpl::OnInitialize();
  Ui::View self = Ui::View::DownCast(Self());
  self.SetAccessibilityRole(Accessibility::Role::BUTTON);
  self.SetStateEffect(OverlayEffect::Round());
  mIcon = Ui::ImageView::New();
  mIcon.SetAccessibilityHidden(true);
  Self().Add(mIcon);
}
void IconButtonImpl::ApplyInitialStyle(IconButtonStyle style)
{
  SetIconUrl(style.GetIconUrl());
  SetIconWidth(style.GetIconWidth());
  SetIconHeight(style.GetIconHeight());
  SetIconColor(style.GetIconColor());
  Ui::View::DownCast(Self()).SetPadding(style.GetPadding());
}
void IconButtonImpl::SetIconUrl(const Dali::String& iconUrl)
{
  mIcon.SetResourceUrl(iconUrl);
}
Dali::String IconButtonImpl::GetIconUrl() const
{
  return mIcon.GetResourceUrl();
}
void IconButtonImpl::SetIconWidth(float width)
{
  mIcon.SetRequestedWidth(width);
}
float IconButtonImpl::GetIconWidth() const
{
  return mIcon.GetRequestedWidth();
}
void IconButtonImpl::SetIconHeight(float height)
{
  mIcon.SetRequestedHeight(height);
}
float IconButtonImpl::GetIconHeight() const
{
  return mIcon.GetRequestedHeight();
}
void IconButtonImpl::SetIconColor(const UiColor& color)
{
  mIcon.SetImageColor(color);
}
UiColor IconButtonImpl::GetIconColor() const
{
  return const_cast<Ui::ImageView&>(mIcon).GetImageColor();
}
void IconButtonImpl::SetIconCornerRadius(const Vector4& radius)
{
  mIcon.SetCornerRadius(radius);
}
Vector4 IconButtonImpl::GetIconCornerRadius() const
{
  return mIcon.GetCornerRadius();
}
} //namespace DALI_NAMESPACE::Ui::Internal
