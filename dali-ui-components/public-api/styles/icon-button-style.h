#pragma once

#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/insets.h>

namespace Dali::Ui
{
namespace Internal
{
class IconButtonStyleImpl;
}

/**
 * @brief Style values used to initialize IconButton appearance and layout.
 */
class DALI_UI_COMPONENTS_API IconButtonStyle : public UiStyle
{
public:
  /**
   * @brief Mutable builder used to create IconButtonStyle handles.
   */
  class Builder;

  /**
   * @brief Creates an uninitialized IconButtonStyle handle.
   */
  IconButtonStyle() = default;

  /**
   * @brief Returns the style key for the default IconButton style.
   * @return The default IconButton style key
   */
  static UiStyleKey<IconButtonStyle> DefaultKey();

  /**
   * @brief Gets the cached built-in default IconButton style.
   *
   * This function requires UiConfig::Apply().
   *
   * @return The built-in default preset
   */
  static IconButtonStyle DefaultPreset();

  /**
   * @brief Gets the default IconButton style from the current UiConfig.
   *
   * This function requires UiConfig::Apply().
   *
   * @return The default IconButton style
   */
  static IconButtonStyle Default();

  /**
   * @brief Gets the built-in add-icon preset.
   *
   * This function requires UiConfig::Apply().
   *
   * @return The add-icon preset
   */
  static IconButtonStyle AddPreset();

  /**
   * @brief Gets the built-in back-icon preset.
   *
   * This function requires UiConfig::Apply().
   *
   * @return The back-icon preset
   */
  static IconButtonStyle BackPreset();

  /**
   * @brief Gets the built-in more-icon preset.
   *
   * This function requires UiConfig::Apply().
   *
   * @return The more-icon preset
   */
  static IconButtonStyle MorePreset();

  /**
   * @brief Downcasts a base handle to IconButtonStyle.
   * @param[in] handle The handle to downcast
   * @return An IconButtonStyle handle, or an uninitialized handle if the types do not match
   */
  static IconButtonStyle DownCast(BaseHandle handle);

  /**
   * @brief Casts a UiStyle handle to IconButtonStyle.
   * @param[in] style The style to cast
   * @return The IconButtonStyle handle
   * @pre @p style must contain an IconButtonStyle.
   */
  static IconButtonStyle StaticDownCast(UiStyle style);

  /**
   * @brief Returns the resource URL of the icon.
   * @return The icon resource URL
   */
  Dali::String GetIconUrl() const;

  /**
   * @brief Returns the requested width of the icon.
   * @return The requested width in logical pixels
   */
  float GetIconWidth() const;

  /**
   * @brief Returns the requested height of the icon.
   * @return The requested height in logical pixels
   */
  float GetIconHeight() const;

  /**
   * @brief Returns the icon color.
   * @return The icon color
   */
  UiColor GetIconColor() const;

  /**
   * @brief Returns the padding around the icon.
   * @return The icon padding
   */
  Insets GetPadding() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL IconButtonStyle(Internal::IconButtonStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create IconButtonStyle handles.
 */
class DALI_UI_COMPONENTS_API IconButtonStyle::Builder
{
public:
  /**
   * @brief Creates an IconButtonStyle builder.
   */
  Builder();

  /**
   * @brief Move constructor.
   * @param[in] rhs The builder to move
   */
  Builder(Builder&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   * @param[in] rhs The builder to move
   * @return A reference to this builder
   */
  Builder& operator=(Builder&& rhs) noexcept;

  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;

  /**
   * @brief Destructor.
   */
  ~Builder();

  /**
   * @brief Sets the resource URL of the icon.
   * @param[in] iconUrl The resource URL of the icon
   * @return A reference to this builder
   */
  Builder& SetIconUrl(const Dali::String& iconUrl) &;

  /**
   * @brief Sets the resource URL of the icon.
   * @param[in] iconUrl The resource URL of the icon
   * @return This builder
   */
  Builder&& SetIconUrl(const Dali::String& iconUrl) &&;

  /**
   * @brief Sets the requested width of the icon.
   * @param[in] width The requested width in logical pixels
   * @return A reference to this builder
   */
  Builder& SetIconWidth(float width) &;

  /**
   * @brief Sets the requested width of the icon.
   * @param[in] width The requested width in logical pixels
   * @return This builder
   */
  Builder&& SetIconWidth(float width) &&;

  /**
   * @brief Sets the requested height of the icon.
   * @param[in] height The requested height in logical pixels
   * @return A reference to this builder
   */
  Builder& SetIconHeight(float height) &;

  /**
   * @brief Sets the requested height of the icon.
   * @param[in] height The requested height in logical pixels
   * @return This builder
   */
  Builder&& SetIconHeight(float height) &&;

  /**
   * @brief Sets the color of the icon.
   * @param[in] color The color to apply to the icon
   * @return A reference to this builder
   */
  Builder& SetIconColor(const UiColor& color) &;

  /**
   * @brief Sets the color of the icon.
   * @param[in] color The color to apply to the icon
   * @return This builder
   */
  Builder&& SetIconColor(const UiColor& color) &&;

  /**
   * @brief Sets the padding around the icon.
   * @param[in] padding The padding to apply
   * @return A reference to this builder
   */
  Builder& SetPadding(const Insets& padding) &;

  /**
   * @brief Sets the padding around the icon.
   * @param[in] padding The padding to apply
   * @return This builder
   */
  Builder&& SetPadding(const Insets& padding) &&;

  /**
   * @brief Creates an IconButtonStyle from the configured values.
   * @return The configured IconButtonStyle
   */
  IconButtonStyle Build() &&;

private:
  explicit Builder(Internal::IconButtonStyleImpl* impl);
  friend class IconButtonStyle;

private:
  IntrusivePtr<Internal::IconButtonStyleImpl> mImpl;
};
} // namespace Dali::Ui
