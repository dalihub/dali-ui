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

// EXTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/styles/text-button-style.h>
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>
#include <dali-ui-foundation/public-api/views/view-types.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class ToastStyleImpl;
}

/**
 * @brief Immutable visual and layout values used to initialize Toast.
 *
 * Transition timing and easing are intrinsic Toast behavior and are not
 * exposed as style values. Window-relative values are resolved when Toast is
 * posted to an explicit Window.
 */
class DALI_UI_COMPONENTS_API ToastStyle : public UiStyle
{
public:
  /** @brief Mutable builder used to create ToastStyle handles. */
  class Builder;

  /** @brief Creates an uninitialized ToastStyle handle. */
  ToastStyle() = default;

  /** @brief Returns the style key for the default Toast style. */
  static UiStyleKey<ToastStyle> DefaultKey();
  /** @brief Gets the cached built-in default Toast style.
   * @note Requires UiConfig::Apply().
   */
  static ToastStyle DefaultPreset();
  /** @brief Gets the default Toast style from the current UiConfig.
   * @note Requires UiConfig::Apply().
   */
  static ToastStyle Default();
  /** @brief Downcasts a base handle to ToastStyle.
   * @param[in] handle The handle to downcast
   * @return A ToastStyle handle, or an uninitialized handle if the types do not match
   */
  static ToastStyle DownCast(BaseHandle handle);
  /** @brief Casts a UiStyle handle to ToastStyle.
   * @param[in] style The style to cast
   * @pre @p style must contain a ToastStyle.
   */
  static ToastStyle StaticDownCast(UiStyle style);

  /** @brief Creates a builder initialized with this style's values.
   * @return A ToastStyle builder
   */
  Builder Configure() const;

  /** @name Geometry and layout values
   * @{
   */
  /** @brief Returns the initial width in logical pixels. */
  float GetInitialWidth() const;
  /** @brief Returns the initial height in logical pixels. */
  float GetInitialHeight() const;
  /** @brief Returns the minimum width in logical pixels. */
  float GetMinimumWidth() const;
  /** @brief Returns the minimum height in logical pixels. */
  float GetMinimumHeight() const;
  /** @brief Returns the expanded height in logical pixels. */
  float GetExpandedHeight() const;
  /** @brief Returns the maximum height in logical pixels. */
  float GetMaximumHeight() const;
  /** @brief Returns the maximum Window-width ratio. */
  float GetMaximumWidthRatio() const;
  /** @brief Returns the maximum width in logical pixels, or zero when ratio-based. */
  float GetMaximumWidth() const;
  /** @brief Returns the bottom offset as a Window-height ratio. */
  float GetBottomOffsetRatio() const;
  /** @brief Returns the bottom offset in logical pixels. */
  float GetBottomOffset() const;
  /** @} */

  /** @name Content layout values
   * @{
   */
  /** @brief Returns the Toast corner radii. */
  Vector4 GetCornerRadius() const;
  /** @brief Returns the corner-radius interpretation policy. */
  CornerRadiusPolicy GetCornerRadiusPolicy() const;
  /** @brief Returns padding for the default presentation. */
  Insets GetPadding() const;
  /** @brief Returns padding for the expanded presentation. */
  Insets GetExpandedPadding() const;
  /** @brief Returns item spacing for the default presentation. */
  float GetItemSpacing() const;
  /** @brief Returns item spacing for the expanded presentation. */
  float GetExpandedItemSpacing() const;
  /** @} */

  /** @name Visual and text values
   * @{
   */
  /** @brief Returns the Toast background color. */
  UiColor GetBackgroundColor() const;
  /** @brief Returns the message text color. */
  UiColor GetTextColor() const;
  /** @brief Returns the message font size in logical pixels. */
  float GetFontSize() const;
  /** @brief Returns the message font family. */
  Dali::String GetFontFamily() const;
  /** @brief Returns the Toast shadow stack. */
  ShadowStack GetShadow() const;
  /** @} */

  /** @name Border values
   * @{
   */
  /** @brief Returns the borderline width in logical pixels. */
  float GetBorderlineWidth() const;
  /** @brief Returns the borderline color. */
  UiColor GetBorderlineColor() const;
  /** @brief Returns the borderline offset. */
  float GetBorderlineOffset() const;
  /** @} */

  /** @brief Returns the style used by the action button. */
  TextButtonStyle GetActionButtonStyle() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL ToastStyle(Internal::ToastStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create complete immutable ToastStyle handles.
 */
class DALI_UI_COMPONENTS_API ToastStyle::Builder
{
public:
  /** @brief Creates a ToastStyle builder with the default values. */
  Builder();
  /** @brief Move constructor.
   * @param[in] rhs The builder to move
   */
  Builder(Builder&& rhs) noexcept;
  /** @brief Move assignment operator.
   * @param[in] rhs The builder to move
   * @return A reference to this builder
   */
  Builder& operator=(Builder&& rhs) noexcept;
  Builder(const Builder&)            = delete;
  Builder& operator=(const Builder&) = delete;
  /** @brief Destructor. */
  ~Builder();

  /** @brief Sets a scalar Toast style value.
   *
   * Each overload returns this builder and validates the value for its named
   * property (for example, a size, ratio, or spacing).
   */
#define DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(Name) \
  /** @brief Sets the named scalar Toast style value.    \
   * @param[in] value The value to apply.                \
   * @return This builder.                               \
   */                                                    \
  Builder& Set##Name(float value) &;                     \
  /** @brief Sets the named scalar Toast style value.    \
   * @param[in] value The value to apply.                \
   * @return This builder.                               \
   */                                                    \
  Builder&& Set##Name(float value) &&;

  /** @brief Sets the initial width in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(InitialWidth)
  /** @brief Sets the initial height in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(InitialHeight)
  /** @brief Sets the minimum width in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(MinimumWidth)
  /** @brief Sets the minimum height in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(MinimumHeight)
  /** @brief Sets the expanded height in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(ExpandedHeight)
  /** @brief Sets the maximum height in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(MaximumHeight)
  /** @brief Sets the maximum Window-width ratio. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(MaximumWidthRatio)
  /** @brief Sets the maximum width in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(MaximumWidth)
  /** @brief Sets the bottom offset as a Window-height ratio. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(BottomOffsetRatio)
  /** @brief Sets the bottom offset in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(BottomOffset)
  /** @brief Sets item spacing for the default presentation. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(ItemSpacing)
  /** @brief Sets item spacing for the expanded presentation. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(ExpandedItemSpacing)
  /** @brief Sets the message font size in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(FontSize)
  /** @brief Sets the borderline width in logical pixels. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(BorderlineWidth)
  /** @brief Sets the borderline offset. */
  DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION(BorderlineOffset)

#undef DALI_TOAST_STYLE_FLOAT_BUILDER_DECLARATION

  /** @brief Sets the initial Toast size.
   * @param[in] width The width in logical pixels
   * @param[in] height The height in logical pixels
   * @return This builder
   */
  Builder& SetInitialSize(float width, float height) &;
  /** @copydoc SetInitialSize(float, float) */
  Builder&& SetInitialSize(float width, float height) &&;
  /** @brief Sets the minimum Toast size.
   * @param[in] width The width in logical pixels
   * @param[in] height The height in logical pixels
   * @return This builder
   */
  Builder& SetMinimumSize(float width, float height) &;
  /** @copydoc SetMinimumSize(float, float) */
  Builder&& SetMinimumSize(float width, float height) &&;

  /** @brief Sets the Toast corner radii.
   * @param[in] radius The corner radii
   * @return This builder
   */
  Builder& SetCornerRadius(const Vector4& radius) &;
  /** @copydoc SetCornerRadius(const Vector4&) */
  Builder&& SetCornerRadius(const Vector4& radius) &&;
  /** @brief Sets the corner-radius interpretation policy.
   * @param[in] policy The policy to apply
   * @return This builder
   */
  Builder& SetCornerRadiusPolicy(CornerRadiusPolicy policy) &;
  /** @copydoc SetCornerRadiusPolicy(CornerRadiusPolicy) */
  Builder&& SetCornerRadiusPolicy(CornerRadiusPolicy policy) &&;
  /** @brief Sets padding for the default Toast presentation.
   * @param[in] padding The padding to apply
   * @return This builder
   */
  Builder& SetPadding(const Insets& padding) &;
  /** @copydoc SetPadding(const Insets&) */
  Builder&& SetPadding(const Insets& padding) &&;
  /** @brief Sets padding for the expanded Toast presentation.
   * @param[in] padding The padding to apply
   * @return This builder
   */
  Builder& SetExpandedPadding(const Insets& padding) &;
  /** @copydoc SetExpandedPadding(const Insets&) */
  Builder&& SetExpandedPadding(const Insets& padding) &&;

  /** @brief Sets the Toast background color.
   * @param[in] color The color to apply
   * @return This builder
   */
  Builder& SetBackgroundColor(const UiColor& color) &;
  /** @copydoc SetBackgroundColor(const UiColor&) */
  Builder&& SetBackgroundColor(const UiColor& color) &&;
  /** @brief Sets the message text color.
   * @param[in] color The color to apply
   * @return This builder
   */
  Builder& SetTextColor(const UiColor& color) &;
  /** @copydoc SetTextColor(const UiColor&) */
  Builder&& SetTextColor(const UiColor& color) &&;
  /** @brief Sets the message font family.
   * @param[in] fontFamily The font family name
   * @return This builder
   */
  Builder& SetFontFamily(const Dali::String& fontFamily) &;
  /** @copydoc SetFontFamily(const Dali::String&) */
  Builder&& SetFontFamily(const Dali::String& fontFamily) &&;
  /** @brief Sets the Toast shadow stack.
   * @param[in] shadow The shadow stack to apply
   * @return This builder
   */
  Builder& SetShadow(const ShadowStack& shadow) &;
  /** @copydoc SetShadow(const ShadowStack&) */
  Builder&& SetShadow(const ShadowStack& shadow) &&;

  /** @brief Sets the borderline color.
   * @param[in] color The color to apply
   * @return This builder
   */
  Builder& SetBorderlineColor(const UiColor& color) &;
  /** @copydoc SetBorderlineColor(const UiColor&) */
  Builder&& SetBorderlineColor(const UiColor& color) &&;
  /** @brief Sets the action-button style.
   * @param[in] style The style to apply
   * @return This builder
   * @pre @p style must be initialized.
   */
  Builder& SetActionButtonStyle(TextButtonStyle style) &;
  /** @copydoc SetActionButtonStyle(TextButtonStyle) */
  Builder&& SetActionButtonStyle(TextButtonStyle style) &&;

  /** @brief Creates a ToastStyle from the configured values.
   * @return The configured ToastStyle
   */
  ToastStyle Build() &&;

private:
  explicit Builder(Internal::ToastStyleImpl* impl);
  friend class ToastStyle;

private:
  IntrusivePtr<Internal::ToastStyleImpl> mImpl;
};

} // namespace Ui
} //namespace DALI_NAMESPACE
