#pragma once

#include <dali-ui-components/internal/styles/style-validation.h>

#include <dali-ui-components/public-api/styles/icon-button-style.h>
#include <dali-ui-foundation/extension-api/styles/ui-style-impl.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>

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
    DALI_ASSERT_ALWAYS(StyleValidation::IsRequestedDimension(iconWidth) &&
                       "IconButtonStyle icon dimension must be finite and non-negative or a layout sentinel");
    mIconWidth = iconWidth;
  }
  void SetIconHeight(float iconHeight)
  {
    DALI_ASSERT_ALWAYS(StyleValidation::IsRequestedDimension(iconHeight) &&
                       "IconButtonStyle icon dimension must be finite and non-negative or a layout sentinel");
    mIconHeight = iconHeight;
  }
  void SetIconColor(const UiColor& color)
  {
    mIconColor = color;
  }
  void SetPadding(const Insets& padding)
  {
    DALI_ASSERT_ALWAYS(StyleValidation::IsNonNegative(padding) && "IconButtonStyle padding must be finite and non-negative");
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
  float        mIconWidth{WRAP_CONTENT};
  float        mIconHeight{WRAP_CONTENT};
  UiColor      mIconColor{UiColor::ON_SURFACE_CONTAINER_HIGHEST};
  Insets       mPadding;
};

inline const IconButtonStyleImpl& GetImpl(const Ui::IconButtonStyle& style)
{
  return static_cast<const IconButtonStyleImpl&>(style.GetBaseObject());
}
} //namespace DALI_NAMESPACE::Ui::Internal
