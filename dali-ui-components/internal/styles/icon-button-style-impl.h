#pragma once

#include <dali-ui-components/public-api/styles/icon-button-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>

namespace DALI_NAMESPACE::Ui::Internal
{
class IconButtonStyleImpl : public Extension::UiStyleImpl
{
public:
  IconButtonStyleImpl() = default;
  IconButtonStyleImpl(Dali::String iconUrl, float iconWidth, float iconHeight)
  : mIconUrl(std::move(iconUrl)),
    mIconWidth(iconWidth),
    mIconHeight(iconHeight)
  {
  }

  void SetIconUrl(const Dali::String& iconUrl)
  {
    mIconUrl = iconUrl;
  }
  void SetIconWidth(float iconWidth)
  {
    mIconWidth = iconWidth;
  }
  void SetIconHeight(float iconHeight)
  {
    mIconHeight = iconHeight;
  }
  void SetIconColor(const UiColor& color)
  {
    mIconColor = color;
  }
  void SetPadding(const Insets& padding)
  {
    mPadding = padding;
  }
  Dali::String GetIconUrl() const
  {
    return mIconUrl;
  }
  float GetIconWidth() const
  {
    return mIconWidth;
  }
  float GetIconHeight() const
  {
    return mIconHeight;
  }
  UiColor GetIconColor() const
  {
    return mIconColor;
  }
  Insets GetPadding() const
  {
    return mPadding;
  }

protected:
  ~IconButtonStyleImpl() override = default;

private:
  Dali::String mIconUrl;
  float        mIconWidth{0.0f};
  float        mIconHeight{0.0f};
  UiColor      mIconColor{UiColor::ON_SURFACE_CONTAINER_HIGHEST};
  Insets       mPadding;
};

inline const IconButtonStyleImpl& GetImpl(const Ui::IconButtonStyle& style)
{
  return static_cast<const IconButtonStyleImpl&>(style.GetBaseObject());
}
} //namespace DALI_NAMESPACE::Ui::Internal
