#pragma once

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class ToastStyleImpl;
}

/**
 * @brief Immutable visual and layout values for a Toast.
 *
 * Use Builder to create a custom style, or Configure() to derive one from an
 * existing style.
 */
class DALI_UI_COMPONENTS_API ToastStyle : public UiStyle
{
public:
  /** @brief Builder type used to create a ToastStyle. */
  class Builder;

  /**
   * @brief Creates an uninitialized ToastStyle handle.
   */
  ToastStyle() = default;

  /**
   * @brief Returns the style key for the default Toast style.
   * @return The default Toast style key
   */
  static UiStyleKey<ToastStyle> DefaultKey();

  /**
   * @brief Gets the cached built-in default Toast style.
   * @return The built-in default Toast style
   */
  static ToastStyle DefaultPreset();

  /**
   * @brief Gets the default Toast style from the current UiConfig.
   * @return The configured default Toast style
   */
  static ToastStyle Default();

  /**
   * @brief Downcasts a base handle to ToastStyle.
   * @param[in] handle The handle to downcast
   * @return A valid ToastStyle on success, or an uninitialized handle
   */
  static ToastStyle DownCast(BaseHandle handle);

  /**
   * @brief Casts a UiStyle handle to ToastStyle without a runtime type check.
   * @param[in] style The ToastStyle-compatible UiStyle handle
   * @return The ToastStyle handle
   */
  static ToastStyle StaticDownCast(UiStyle style);

  /**
   * @brief Creates a builder initialized with this style's values.
   * @return A mutable builder containing a copy of this style
   */
  Builder Configure() const;

  /**
   * @brief Returns the maximum Toast width as a ratio of the Window width.
   * @return A value greater than zero and no greater than one
   */
  float GetMaximumWidthRatio() const;

  /**
   * @brief Returns the maximum Toast height used to constrain wrapped content.
   *
   * Toast does not impose a separate fixed line-count limit.
   * @return The non-negative maximum height in logical pixels
   */
  float GetMaximumHeight() const;

  /**
   * @brief Returns the Toast bottom offset as a ratio of the Window height.
   * @return A value greater than zero and no greater than one
   */
  float GetBottomOffsetRatio() const;

  /**
   * @brief Returns the spacing between the optional icon and message text.
   * @return The non-negative spacing in logical pixels
   */
  float GetItemSpacing() const;

  /**
   * @brief Returns the Toast corner radii.
   * @return The non-negative radius of each corner
   */
  Vector4 GetCornerRadius() const;

  /**
   * @brief Returns the padding around the Toast content.
   * @return The non-negative content padding
   */
  Insets GetPadding() const;

  /**
   * @brief Returns the Toast background color.
   * @return The background color
   */
  UiColor GetBackgroundColor() const;

  /**
   * @brief Returns the message text color.
   * @return The text color
   */
  UiColor GetTextColor() const;

  /**
   * @brief Returns the message font size.
   * @return The non-negative font size in logical pixels
   */
  float GetFontSize() const;

  /**
   * @brief Returns the message font family.
   * @return The font family name
   */
  Dali::String GetFontFamily() const;

  /**
   * @brief Returns the optional icon width.
   * @return The non-negative width in logical pixels
   */
  float GetIconWidth() const;

  /**
   * @brief Returns the optional icon height.
   * @return The non-negative height in logical pixels
   */
  float GetIconHeight() const;

  /**
   * @brief Returns the optional icon corner radii.
   * @return The non-negative radius of each corner
   */
  Vector4 GetIconCornerRadius() const;

  /**
   * @brief Returns the optional icon color.
   * @return The icon color
   */
  UiColor GetIconColor() const;

  /**
   * @brief Returns the Toast shadow stack.
   * @return The shadow stack
   */
  ShadowStack GetShadow() const;

  /**
   * @brief Returns the Toast borderline width.
   * @return The non-negative width in logical pixels
   */
  float GetBorderlineWidth() const;

  /**
   * @brief Returns the Toast borderline offset.
   * @return The borderline offset, where -1 places it fully inside
   */
  float GetBorderlineOffset() const;

  /**
   * @brief Returns the Toast borderline color.
   * @return The borderline color
   */
  UiColor GetBorderlineColor() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL ToastStyle(Internal::ToastStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create immutable ToastStyle handles.
 */
class DALI_UI_COMPONENTS_API ToastStyle::Builder
{
public:
  /**
   * @brief Creates a builder initialized with the built-in Toast defaults.
   */
  Builder();

  /**
   * @brief Move constructor.
   * @param[in] rhs The builder to move from
   */
  Builder(Builder&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   * @param[in] rhs The builder to move from
   * @return A reference to this builder
   */
  Builder& operator=(Builder&& rhs) noexcept;

  /**
   * @brief Copy construction is not supported.
   * @param[in] rhs The builder that would be copied
   */
  Builder(const Builder& rhs) = delete;

  /**
   * @brief Copy assignment is not supported.
   * @param[in] rhs The builder that would be copied
   * @return A reference to this builder
   */
  Builder& operator=(const Builder& rhs) = delete;

  /**
   * @brief Destructor.
   */
  ~Builder();

  /**
   * @brief Sets the maximum Toast width as a ratio of the Window width.
   * @param[in] value A finite value greater than zero and no greater than one
   * @return A reference to this builder
   */
  Builder& SetMaximumWidthRatio(float value) &;

  /**
   * @brief Sets the maximum Toast width as a ratio of the Window width.
   * @param[in] value A finite value greater than zero and no greater than one
   * @return An rvalue reference to this builder
   */
  Builder&& SetMaximumWidthRatio(float value) &&;

  /**
   * @brief Sets the maximum Toast height used to constrain wrapped content.
   *
   * Toast does not impose a separate fixed line-count limit.
   * @param[in] value The finite, non-negative height in logical pixels
   * @return A reference to this builder
   */
  Builder& SetMaximumHeight(float value) &;

  /**
   * @brief Sets the maximum Toast height used to constrain wrapped content.
   *
   * Toast does not impose a separate fixed line-count limit.
   * @param[in] value The finite, non-negative height in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetMaximumHeight(float value) &&;

  /**
   * @brief Sets the Toast bottom offset as a ratio of the Window height.
   * @param[in] value A finite value greater than zero and no greater than one
   * @return A reference to this builder
   */
  Builder& SetBottomOffsetRatio(float value) &;

  /**
   * @brief Sets the Toast bottom offset as a ratio of the Window height.
   * @param[in] value A finite value greater than zero and no greater than one
   * @return An rvalue reference to this builder
   */
  Builder&& SetBottomOffsetRatio(float value) &&;

  /**
   * @brief Sets the spacing between the optional icon and message text.
   * @param[in] value The finite, non-negative spacing in logical pixels
   * @return A reference to this builder
   */
  Builder& SetItemSpacing(float value) &;

  /**
   * @brief Sets the spacing between the optional icon and message text.
   * @param[in] value The finite, non-negative spacing in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetItemSpacing(float value) &&;

  /**
   * @brief Sets the radius of each Toast corner.
   * @param[in] radius The finite, non-negative corner radii
   * @return A reference to this builder
   */
  Builder& SetCornerRadius(const Vector4& radius) &;

  /**
   * @brief Sets the radius of each Toast corner.
   * @param[in] radius The finite, non-negative corner radii
   * @return An rvalue reference to this builder
   */
  Builder&& SetCornerRadius(const Vector4& radius) &&;

  /**
   * @brief Sets the padding around the Toast content.
   * @param[in] padding The finite, non-negative content padding
   * @return A reference to this builder
   */
  Builder& SetPadding(const Insets& padding) &;

  /**
   * @brief Sets the padding around the Toast content.
   * @param[in] padding The finite, non-negative content padding
   * @return An rvalue reference to this builder
   */
  Builder&& SetPadding(const Insets& padding) &&;

  /**
   * @brief Sets the Toast background color.
   * @param[in] color The background color
   * @return A reference to this builder
   */
  Builder& SetBackgroundColor(const UiColor& color) &;

  /**
   * @brief Sets the Toast background color.
   * @param[in] color The background color
   * @return An rvalue reference to this builder
   */
  Builder&& SetBackgroundColor(const UiColor& color) &&;

  /**
   * @brief Sets the message text color.
   * @param[in] color The text color
   * @return A reference to this builder
   */
  Builder& SetTextColor(const UiColor& color) &;

  /**
   * @brief Sets the message text color.
   * @param[in] color The text color
   * @return An rvalue reference to this builder
   */
  Builder&& SetTextColor(const UiColor& color) &&;

  /**
   * @brief Sets the message font size.
   * @param[in] value The finite, non-negative size in logical pixels
   * @return A reference to this builder
   */
  Builder& SetFontSize(float value) &;

  /**
   * @brief Sets the message font size.
   * @param[in] value The finite, non-negative size in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetFontSize(float value) &&;

  /**
   * @brief Sets the message font family.
   * @param[in] fontFamily The font family name
   * @return A reference to this builder
   */
  Builder& SetFontFamily(const Dali::String& fontFamily) &;

  /**
   * @brief Sets the message font family.
   * @param[in] fontFamily The font family name
   * @return An rvalue reference to this builder
   */
  Builder&& SetFontFamily(const Dali::String& fontFamily) &&;

  /**
   * @brief Sets the optional icon width.
   * @param[in] value The finite, non-negative width in logical pixels
   * @return A reference to this builder
   */
  Builder& SetIconWidth(float value) &;

  /**
   * @brief Sets the optional icon width.
   * @param[in] value The finite, non-negative width in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconWidth(float value) &&;

  /**
   * @brief Sets the optional icon height.
   * @param[in] value The finite, non-negative height in logical pixels
   * @return A reference to this builder
   */
  Builder& SetIconHeight(float value) &;

  /**
   * @brief Sets the optional icon height.
   * @param[in] value The finite, non-negative height in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconHeight(float value) &&;

  /**
   * @brief Sets the optional icon corner radii.
   * @param[in] radius The finite, non-negative corner radii
   * @return A reference to this builder
   */
  Builder& SetIconCornerRadius(const Vector4& radius) &;

  /**
   * @brief Sets the optional icon corner radii.
   * @param[in] radius The finite, non-negative corner radii
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconCornerRadius(const Vector4& radius) &&;

  /**
   * @brief Sets the optional icon color.
   * @param[in] color The icon color
   * @return A reference to this builder
   */
  Builder& SetIconColor(const UiColor& color) &;

  /**
   * @brief Sets the optional icon color.
   * @param[in] color The icon color
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconColor(const UiColor& color) &&;

  /**
   * @brief Sets the Toast shadow stack.
   * @param[in] shadow The shadow stack
   * @return A reference to this builder
   */
  Builder& SetShadow(const ShadowStack& shadow) &;

  /**
   * @brief Sets the Toast shadow stack.
   * @param[in] shadow The shadow stack
   * @return An rvalue reference to this builder
   */
  Builder&& SetShadow(const ShadowStack& shadow) &&;

  /**
   * @brief Sets the Toast borderline width.
   * @param[in] value The finite, non-negative width in logical pixels
   * @return A reference to this builder
   */
  Builder& SetBorderlineWidth(float value) &;

  /**
   * @brief Sets the Toast borderline width.
   * @param[in] value The finite, non-negative width in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetBorderlineWidth(float value) &&;

  /**
   * @brief Sets the Toast borderline offset.
   * @param[in] value The finite offset, where -1 places it fully inside
   * @return A reference to this builder
   */
  Builder& SetBorderlineOffset(float value) &;

  /**
   * @brief Sets the Toast borderline offset.
   * @param[in] value The finite offset, where -1 places it fully inside
   * @return An rvalue reference to this builder
   */
  Builder&& SetBorderlineOffset(float value) &&;

  /**
   * @brief Sets the Toast borderline color.
   * @param[in] color The borderline color
   * @return A reference to this builder
   */
  Builder& SetBorderlineColor(const UiColor& color) &;

  /**
   * @brief Sets the Toast borderline color.
   * @param[in] color The borderline color
   * @return An rvalue reference to this builder
   */
  Builder&& SetBorderlineColor(const UiColor& color) &&;

  /**
   * @brief Creates a ToastStyle from the configured values.
   * @return The immutable ToastStyle
   */
  ToastStyle Build() &&;

private:
  explicit Builder(Internal::ToastStyleImpl* impl);
  friend class ToastStyle;
  IntrusivePtr<Internal::ToastStyleImpl> mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE
