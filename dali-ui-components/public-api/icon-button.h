#pragma once

#include <dali-ui-components/public-api/styles/icon-button-style.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali/public-api/common/dali-string.h>

namespace DALI_NAMESPACE::Ui
{
namespace Internal
{
class IconButtonImpl;
}

/**
 * @brief An interactive button that displays an icon.
 *
 * IconButton uses the current default IconButtonStyle unless a style is
 * supplied when it is created. Its icon appearance can also be updated after
 * creation with the icon setter methods.
 */
class DALI_UI_COMPONENTS_API IconButton : public InteractiveView
{
public:
  /**
   * @brief Creates an uninitialized IconButton handle.
   */
  IconButton();

  /**
   * @brief Destructor.
   */
  ~IconButton();

  /**
   * @brief Creates an IconButton using the current default style.
   * @return An initialized IconButton
   */
  static IconButton New();

  /**
   * @brief Creates an IconButton using @p style.
   * @param[in] style The style to apply
   * @return An initialized IconButton
   * @pre @p style must be initialized.
   */
  static IconButton New(IconButtonStyle style);

  /**
   * @brief Creates an IconButton using the current default style and @p iconUrl.
   * @param[in] iconUrl The resource URL of the icon
   * @return An initialized IconButton
   */
  static IconButton New(const Dali::String& iconUrl);

  /**
   * @brief Creates an IconButton using @p style and @p iconUrl.
   * @param[in] iconUrl The resource URL of the icon
   * @param[in] style The style to apply
   * @return An initialized IconButton
   * @pre @p style must be initialized.
   */
  static IconButton New(const Dali::String& iconUrl, IconButtonStyle style);

  /**
   * @brief Downcasts a base handle to IconButton.
   * @param[in] handle The handle to downcast
   * @return An IconButton handle, or an uninitialized handle if the types do not match
   */
  static IconButton DownCast(BaseHandle handle);

  /**
   * @brief Copy constructor.
   * @param[in] handle The handle to copy
   */
  IconButton(const IconButton& handle);

  /**
   * @brief Move constructor.
   * @param[in] rhs The handle to move
   */
  IconButton(IconButton&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @param[in] handle The handle to copy
   * @return A reference to this handle
   */
  IconButton& operator=(const IconButton& handle);

  /**
   * @brief Move assignment operator.
   * @param[in] rhs The handle to move
   * @return A reference to this handle
   */
  IconButton& operator=(IconButton&& rhs) noexcept;

  DALI_UI_VIEW_WITH(IconButton)

  /**
   * @brief Sets the resource URL of the icon.
   * @param[in] iconUrl The resource URL of the icon
   */
  void SetIconUrl(const Dali::String& iconUrl);

  /**
   * @brief Returns the resource URL of the icon.
   * @return The icon resource URL
   */
  Dali::String GetIconUrl() const;

  /**
   * @brief Sets the icon's requested width.
   * @param[in] width The size in logical pixels. Zero is an explicit size;
   * WRAP_CONTENT prefers the natural size (or 56 if unavailable), while MATCH_PARENT uses the parent constraint.
   * A positive size on one axis with WRAP_CONTENT on the other preserves the image's aspect ratio.
   * @pre The size must be finite and non-negative or WRAP_CONTENT/MATCH_PARENT; a violation triggers an assertion.
   */
  void SetIconWidth(float width);

  /**
   * @brief Returns the requested width of the icon.
   * @return The requested logical-pixel value or layout sentinel, not the actual measured width.
   */
  float GetIconWidth() const;

  /**
   * @brief Sets the icon's requested height.
   * @param[in] height The size in logical pixels. Zero is an explicit size;
   * WRAP_CONTENT prefers the natural size (or 56 if unavailable), while MATCH_PARENT uses the parent constraint.
   * @pre The size must be finite and non-negative or WRAP_CONTENT/MATCH_PARENT; a violation triggers an assertion.
   */
  void SetIconHeight(float height);

  /**
   * @brief Returns the requested height of the icon.
   * @return The requested logical-pixel value or layout sentinel, not the actual measured height.
   */
  float GetIconHeight() const;

  /**
   * @brief Sets the color of the icon.
   * @param[in] color The color to apply to the icon
   */
  void SetIconColor(const UiColor& color);

  /**
   * @brief Returns the color of the icon.
   * @return The icon color
   */
  UiColor GetIconColor() const;

  /**
   * @brief Sets the corner radii of the icon.
   * @param[in] radius The corner radii in top-left, top-right, bottom-right,
   * and bottom-left order
   * @pre Each component must be finite and non-negative; a violation triggers an assertion. Zero means no rounding.
   */
  void SetIconCornerRadius(const Vector4& radius);

  /**
   * @brief Returns the corner radii of the icon.
   * @return The icon corner radii
   */
  Vector4 GetIconCornerRadius() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL IconButton(Internal::IconButtonImpl& implementation);
  explicit DALI_INTERNAL IconButton(Dali::Internal::CustomActor* internal);
  /// @endcond
};
} //namespace DALI_NAMESPACE::Ui
