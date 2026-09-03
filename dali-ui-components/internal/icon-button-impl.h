#pragma once

#include <dali-ui-components/public-api/icon-button.h>
#include <dali-ui-foundation/extension-api/interactive-view-impl.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>

namespace Dali::Ui::Internal
{
class IconButtonImpl : public Extension::InteractiveViewImpl
{
public:
  static Ui::IconButton New(IconButtonStyle style);
  void                  SetIconUrl(const Dali::String& iconUrl);
  Dali::String          GetIconUrl() const;
  void                  SetIconWidth(float width);
  float                 GetIconWidth() const;
  void                  SetIconHeight(float height);
  float                 GetIconHeight() const;
  void                  SetIconColor(const UiColor& color);
  UiColor               GetIconColor() const;
  void                  SetIconCornerRadius(const Vector4& radius);
  Vector4               GetIconCornerRadius() const;

protected:
  void OnInitialize() override;
  IconButtonImpl()           = default;
  ~IconButtonImpl() override = default;

private:
  void          ApplyInitialStyle(IconButtonStyle style);
  Ui::ImageView mIcon;
};

inline IconButtonImpl& GetImpl(Ui::IconButton& button)
{
  return static_cast<IconButtonImpl&>(button.GetImplementation());
}
inline const IconButtonImpl& GetImpl(const Ui::IconButton& button)
{
  return static_cast<const IconButtonImpl&>(button.GetImplementation());
}
} // namespace Dali::Ui::Internal
