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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/styles/ui-style-key.h>
#include <dali-ui-foundation/public-api/styles/ui-style.h>
#include <dali-ui-foundation/public-api/types/callback.h>
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali/public-api/common/intrusive-ptr.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class RadioButtonStyleImpl;
}

/**
 * @addtogroup dali_ui_view
 * @{
 */

/**
 * @brief Style values used to initialize RadioButton appearance and layout.
 *
 * RadioButtonStyle is an immutable handle. Use Configure() or Builder to create a modified
 * style, then pass that style to RadioButton::New().
 */
class DALI_UI_COMPONENTS_API RadioButtonStyle : public UiStyle
{
public:
  class Builder;

  /**
   * @brief ABI-safe factory that creates the selectable RadioButton icon.
   *
   * The callback is invoked once for each RadioButton constructed from the style. Each
   * invocation must return a newly created SelectableImageInterface whose GetView() is also
   * initialized; a live selectable image or View must not be shared by multiple controls. A
   * stateless free function is the preferred callback target because Ui::Callback is move-only
   * and the callback is shared by style copies.
   */
  using IconGenerator = Ui::Callback<SelectableImageInterface()>;

  /**
   * @brief Creates an uninitialized style handle.
   */
  RadioButtonStyle();

  /**
   * @brief Destructor.
   */
  ~RadioButtonStyle();

  /**
   * @brief Creates a style handle that shares the implementation of @p rhs.
   * @param[in] rhs The style handle to copy
   */
  RadioButtonStyle(const RadioButtonStyle& rhs);

  /**
   * @brief Moves a style handle.
   * @param[in] rhs The style handle to move
   */
  RadioButtonStyle(RadioButtonStyle&& rhs) noexcept;

  /**
   * @brief Assigns another style handle to this handle.
   * @param[in] rhs The style handle to copy
   * @return A reference to this handle
   */
  RadioButtonStyle& operator=(const RadioButtonStyle& rhs);

  /**
   * @brief Moves another style handle into this handle.
   * @param[in] rhs The style handle to move
   * @return A reference to this handle
   */
  RadioButtonStyle& operator=(RadioButtonStyle&& rhs) noexcept;

  /**
   * @brief Returns the theme key for the default RadioButton style.
   * @return The RadioButton style key
   */
  static UiStyleKey<RadioButtonStyle> DefaultKey();

  /**
   * @brief Returns the built-in RadioButton style preset.
   * @return An initialized built-in style
   */
  static RadioButtonStyle DefaultPreset();

  /**
   * @brief Returns the current themed RadioButton style.
   *
   * The built-in preset is returned when the current theme has no override for DefaultKey().
   *
   * @return An initialized themed or built-in style
   */
  static RadioButtonStyle Default();

  /**
   * @brief Downcasts a base handle to RadioButtonStyle.
   * @param[in] handle The handle to downcast
   * @return A RadioButtonStyle handle, or an uninitialized handle if the types do not match
   */
  static RadioButtonStyle DownCast(BaseHandle handle);

  /**
   * @brief Statically downcasts a UiStyle known to contain RadioButtonStyle.
   * @param[in] style The RadioButtonStyle-compatible handle to downcast
   * @return A RadioButtonStyle handle sharing the same implementation
   * @pre @p style must be initialized and must contain a RadioButtonStyle implementation.
   */
  static RadioButtonStyle StaticDownCast(UiStyle style);

  /**
   * @brief Creates a builder initialized with a copy of this style's values.
   * @return A mutable builder independent of this style
   * @pre This style handle must be initialized.
   */
  Builder Configure() const;

  /**
   * @brief Returns the minimum width in logical pixels.
   * @return The minimum width
   * @pre This style handle must be initialized.
   */
  float GetMinimumWidth() const;

  /**
   * @brief Returns the minimum height in logical pixels.
   * @return The minimum height
   * @pre This style handle must be initialized.
   */
  float GetMinimumHeight() const;

  /**
   * @brief Returns the padding around the icon.
   * @return The content padding in logical pixels
   * @pre This style handle must be initialized.
   */
  Insets GetPadding() const;

  /**
   * @brief Returns the requested icon width.
   * @return The width in logical pixels, or zero when unset
   * @pre This style handle must be initialized.
   */
  float GetIconWidth() const;

  /**
   * @brief Returns the requested icon height.
   * @return The height in logical pixels, or zero when unset
   * @pre This style handle must be initialized.
   */
  float GetIconHeight() const;

  /**
   * @brief Invokes the configured generator to create a selectable icon.
   * @return The generated icon, or an uninitialized handle when no generator is configured
   * @pre This style handle must be initialized.
   * @note RadioButton::New(RadioButtonStyle) requires this function to return an initialized
   * SelectableImageInterface with an initialized View that is not shared by another control.
   */
  SelectableImageInterface CreateIcon() const;

  /**
   * @brief Returns the deselected icon color.
   * @return The deselected-state color
   * @pre This style handle must be initialized.
   */
  UiColor GetIconColor() const;

  /**
   * @brief Returns the selected icon color.
   * @return The selected-state color
   * @pre This style handle must be initialized.
   */
  UiColor GetSelectedIconColor() const;

  /**
   * @brief Returns the interaction state effect.
   * @return The effect applied to the RadioButton root
   * @pre This style handle must be initialized.
   */
  StateEffect GetStateEffect() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL RadioButtonStyle(Internal::RadioButtonStyleImpl* impl);
  /// @endcond
};

/**
 * @brief Mutable builder used to create RadioButtonStyle handles.
 */
class DALI_UI_COMPONENTS_API RadioButtonStyle::Builder
{
public:
  /**
   * @brief Creates a builder initialized with the built-in RadioButton defaults.
   */
  Builder();

  /**
   * @brief Moves a builder.
   * @param[in] rhs The builder to move
   */
  Builder(Builder&& rhs) noexcept;

  /**
   * @brief Moves another builder into this builder.
   * @param[in] rhs The builder to move
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
   */
  Builder& operator=(const Builder& rhs) = delete;

  /**
   * @brief Destructor.
   */
  ~Builder();

  /**
   * @brief Sets the minimum width.
   * @param[in] width The minimum width in logical pixels
   * @return A reference to this builder
   */
  Builder& SetMinimumWidth(float width) &;

  /**
   * @brief Sets the minimum width on a temporary builder.
   * @param[in] width The minimum width in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetMinimumWidth(float width) &&;

  /**
   * @brief Sets the minimum height.
   * @param[in] height The minimum height in logical pixels
   * @return A reference to this builder
   */
  Builder& SetMinimumHeight(float height) &;

  /**
   * @brief Sets the minimum height on a temporary builder.
   * @param[in] height The minimum height in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetMinimumHeight(float height) &&;

  /**
   * @brief Sets the padding around the icon.
   * @param[in] padding The content padding in logical pixels
   * @return A reference to this builder
   */
  Builder& SetPadding(const Insets& padding) &;

  /**
   * @brief Sets the padding around the icon on a temporary builder.
   * @param[in] padding The content padding in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetPadding(const Insets& padding) &&;

  /**
   * @brief Sets the requested icon width.
   *
   * A non-positive or non-finite value is normalized to zero, which unsets the explicit
   * width. An unset width follows the effective icon height.
   *
   * @param[in] width The requested width in logical pixels
   * @return A reference to this builder
   */
  Builder& SetIconWidth(float width) &;

  /**
   * @brief Sets the requested icon width on a temporary builder.
   *
   * A non-positive or non-finite value is normalized to zero, which unsets the explicit
   * width. An unset width follows the effective icon height.
   *
   * @param[in] width The requested width in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconWidth(float width) &&;

  /**
   * @brief Sets the requested icon height.
   *
   * A non-positive or non-finite value is normalized to zero, which unsets the explicit
   * height. An unset height follows a definite content height, or the minimum height when
   * wrapping content.
   *
   * @param[in] height The requested height in logical pixels
   * @return A reference to this builder
   */
  Builder& SetIconHeight(float height) &;

  /**
   * @brief Sets the requested icon height on a temporary builder.
   *
   * A non-positive or non-finite value is normalized to zero, which unsets the explicit
   * height. An unset height follows a definite content height, or the minimum height when
   * wrapping content.
   *
   * @param[in] height The requested height in logical pixels
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconHeight(float height) &&;

  /**
   * @brief Sets the selectable icon generator.
   * @param[in] generator The generator to move into this builder
   * @return A reference to this builder
   * @note A RadioButton cannot be created from a style whose generator is empty or returns
   * an invalid selectable image or View.
   */
  Builder& SetIconGenerator(IconGenerator&& generator) &;

  /**
   * @brief Sets the selectable icon generator on a temporary builder.
   * @param[in] generator The generator to move into this builder
   * @return An rvalue reference to this builder
   * @note A RadioButton cannot be created from a style whose generator is empty or returns
   * an invalid selectable image or View.
   */
  Builder&& SetIconGenerator(IconGenerator&& generator) &&;

  /**
   * @brief Sets the deselected icon color.
   * @param[in] color The deselected-state color
   * @return A reference to this builder
   */
  Builder& SetIconColor(const UiColor& color) &;

  /**
   * @brief Sets the deselected icon color on a temporary builder.
   * @param[in] color The deselected-state color
   * @return An rvalue reference to this builder
   */
  Builder&& SetIconColor(const UiColor& color) &&;

  /**
   * @brief Sets the selected icon color.
   * @param[in] color The selected-state color
   * @return A reference to this builder
   */
  Builder& SetSelectedIconColor(const UiColor& color) &;

  /**
   * @brief Sets the selected icon color on a temporary builder.
   * @param[in] color The selected-state color
   * @return An rvalue reference to this builder
   */
  Builder&& SetSelectedIconColor(const UiColor& color) &&;

  /**
   * @brief Sets the interaction state effect.
   * @param[in] effect The effect to apply, or an uninitialized handle to use StateEffect::None()
   * @return A reference to this builder
   */
  Builder& SetStateEffect(StateEffect effect) &;

  /**
   * @brief Sets the interaction state effect on a temporary builder.
   * @param[in] effect The effect to apply, or an uninitialized handle to use StateEffect::None()
   * @return An rvalue reference to this builder
   */
  Builder&& SetStateEffect(StateEffect effect) &&;

  /**
   * @brief Consumes this builder and returns an immutable style handle.
   * @return An initialized RadioButtonStyle
   * @pre This builder must not have been moved from or consumed by an earlier Build() call.
   */
  RadioButtonStyle Build() &&;

private:
  explicit Builder(Internal::RadioButtonStyleImpl* impl);
  friend class RadioButtonStyle;

private:
  IntrusivePtr<Internal::RadioButtonStyleImpl> mImpl;
};

/** @} */

} // namespace Ui
} // namespace Dali
