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
 *
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/view.h>
#include <functional>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/input-field-properties.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/text/text-style.h>
#include <dali-ui-foundation/public-api/ui-color.h>

// None

namespace Dali
{

namespace Ui
{

// Forward declarations
namespace Integration
{
class InputFieldImpl;
}

#include "input-field.autogen.h"
/**
 * @brief InputField is a single-line editable text view.
 *
 * It supports user interaction for text input and editing,
 * and handles text layout and rendering.
 */
class DALI_UI_API InputField : public View
{
public:
  // Typedefs

public: // Creation & Destruction
  /**
   * @brief Creates an uninitialized InputField handle.
   *
   * Only derived versions can be instantiated. Calling member
   * functions with an uninitialized Dali::Object is not allowed.
   */
  InputField();

  /**
   * @brief Creates an initialized InputField.
   *
   * @return A handle to a newly allocated Dali resource
   */
  static InputField New();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   * @param[in] inputField Handle to copy
   */
  InputField(const InputField& inputField);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  InputField(InputField&& rhs) noexcept;

  /**
   * @brief Virtual destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~InputField();

public: // Operators
  /**
   * @brief Copy assignment operator.
   *
   * Changes this handle to point to another real object.
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  InputField& operator=(const InputField& handle);

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  InputField& operator=(InputField&& rhs) noexcept;

public: // Static Methods
  /**
   * @brief Downcasts a handle to InputField handle.
   *
   * If handle points to a InputField, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a InputField or an uninitialized handle
   */
  static InputField DownCast(BaseHandle handle);

public: // Setters for chaining
  // @CHAIN_START(InputField, View)
  /**
   * @brief Sets the text.
   *
   * @param[in] text The text to display in UTF-8 format.
   */
  InputField& SetText(const Dali::String& text);

  /**
   * @brief Gets the text.
   *
   * @return The text currently set on the inputField in UTF-8 format.
   */
  Dali::String GetText() const;

  /**
   * @brief Sets the font family of the text.
   *
   * @param[in] fontFamily The requested font family to use.
   */
  InputField& SetFontFamily(const Dali::String& fontFamily);

  /**
   * @brief Gets the font family of the text.
   *
   * @return The font family currently set on the inputField.
   */
  Dali::String GetFontFamily() const;

  /**
   * @brief Sets the font size of the text.
   *
   * @param[in] fontSize The font size in pixels.
   */
  InputField& SetFontSize(float fontSize);

  /**
   * @brief Gets the font size of the text.
   *
   * @return The font size currently set on the inputField, in pixels.
   */
  float GetFontSize() const;

  /**
   * @brief Sets the color of the text.
   *
   * @param[in] color The required text color value.
   */
  InputField& SetTextColor(const UiColor& color);

  /**
   * @brief Gets the color of the text.
   *
   * @return The text color currently set on the inputField.
   */
  UiColor GetTextColor();

  /**
   * @brief Sets the horizontal alignment of the text within the inputField.
   *
   * @param[in] alignment The horizontal text alignment.
   */
  InputField& SetHorizontalTextAlignment(Text::Alignment alignment);

  /**
   * @brief Gets the horizontal text alignment.
   *
   * @return The horizontal text alignment.
   */
  Text::Alignment GetHorizontalTextAlignment() const;

  /**
   * @brief Sets the vertical alignment of the text within the inputField.
   *
   * @param[in] alignment The vertical text alignment.
   */
  InputField& SetVerticalTextAlignment(Text::Alignment alignment);

  /**
   * @brief Gets the vertical text alignment.
   *
   * @return The vertical text alignment.
   */
  Text::Alignment GetVerticalTextAlignment() const;

  /**
   * @brief Sets the overflow mode.
   *
   * @param[in] mode The overflow mode to apply.
   */
  InputField& SetOverflowMode(Text::OverflowMode mode);

  /**
   * @brief Gets the overflow mode.
   *
   * @return The current overflow mode.
   */
  Text::OverflowMode GetOverflowMode() const;

  /**
   * @brief Sets the placeholder text displayed when the input field is empty.
   *
   * @param[in] text The placeholder text in UTF-8 encoding.
   */
  InputField& SetPlaceholder(const Dali::String& text);

  /**
   * @brief Gets the placeholder text.
   *
   * @return The placeholder text in UTF-8 encoding.
   */
  Dali::String GetPlaceholder() const;

  /**
   * @brief Sets the color of the placeholder text.
   *
   * @param[in] color The placeholder text color as a UiColor.
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetPlaceholderColor(const UiColor& color);

  /**
   * @brief Gets the color of the placeholder text.
   *
   * @return The placeholder text color as a UiColor.
   */
  UiColor GetPlaceholderColor();

  /**
   * @brief Sets the width of the text cursor.
   *
   * @param[in] width The cursor width in pixels.
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetCursorWidth(int width);

  /**
   * @brief Gets the width of the text cursor.
   *
   * @return The cursor width in pixels.
   */
  int GetCursorWidth() const;

  /**
   * @brief Sets the color of the text cursor
   *
   * This color is applied to both primary and secondary cursors
   * when a split cursor is shown in bidirectional text.
   *
   * @param[in] color The cursor color as a UiColor.
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetCursorColor(const UiColor& color);

  /**
   * @brief Gets the color of the text cursor.
   *
   * @return The cursor color as a UiColor.
   */
  UiColor GetCursorColor();

  /**
   * @brief Sets the highlight color of the selected text region.
   *
   * @param[in] color The selection highlight color as a UiColor).
   *
   * @return A reference to this InputField for method chaining.
   */
  InputField& SetSelectionColor(const UiColor& color);

  /**
   * @brief Gets the highlight color of the selected text region.
   *
   * @return The selection highlight color as a UiColor.
   */
  UiColor GetSelectionColor();

  /**
   * @brief Sets the maximum number of characters that can be entered into the InputField.
   *
   * @param[in] length The maximum number of characters allowed.
   */
  InputField& SetMaximumLength(int length);

  /**
   * @brief Gets the maximum number of characters allowed in the InputField.
   *
   * @return The maximum character count.
   */
  int GetMaximumLength() const;

  /**
   * @brief Sets how the layout direction of the text is resolved.
   *
   * - LayoutDirectionMode::CONTENTS:
   *   The layout direction is determined from the text content itself.
   *
   * - LayoutDirectionMode::INHERIT:
   *   The layout direction is inherited from the parent view.
   *
   * - LayoutDirectionMode::LOCALE:
   *   The layout direction is determined based on the system locale.
   *
   * @note The default layout direction mode of InputField is LayoutDirectionMode::INHERIT.
   *
   * @param[in] mode The LayoutDirectionMode used to determine the text layout direction.
   */
  InputField& SetLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @brief Gets the current layout direction mode.
   *
   * @return The LayoutDirectionMode used to resolve the text layout direction.
   */
  Text::LayoutDirectionMode GetLayoutDirectionMode() const;

  /**
   * @brief Sets the font weight.
   *
   * @param[in] weight The font weight.
   */
  InputField& SetFontWeight(Text::FontWeight weight);

  /**
   * @brief Returns the font weight.
   *
   * @return The font weight.
   */
  Text::FontWeight GetFontWeight() const;

  /**
   * @brief Sets the font width.
   *
   * @param[in] width The font width.
   */
  InputField& SetFontWidth(Text::FontWidth width);

  /**
   * @brief Returns the font width.
   *
   * @return The font width.
   */
  Text::FontWidth GetFontWidth() const;

  /**
   * @brief Sets the font slant.
   *
   * @param[in] slant The font slant.
   */
  InputField& SetFontSlant(Text::FontSlant slant);

  /**
   * @brief Returns the font slant.
   *
   * @return The font slant.
   */
  Text::FontSlant GetFontSlant() const;

  /**
   * @brief Sets the underline style.
   *
   * @param[in] underline The underline configuration.
   */
  InputField& SetUnderline(const Text::Underline& underline);

  /**
   * @brief Resets the underline.
   */
  void ResetUnderline();

  /**
   * @brief Sets the shadow style.
   *
   * @param[in] shadow The shadow configuration.
   */
  InputField& SetShadow(const Text::Shadow& shadow);

  /**
   * @brief Resets the shadow.
   */
  void ResetShadow();

  /**
   * @brief Sets the outline style.
   *
   * @param[in] outline The outline configuration.
   */
  InputField& SetOutline(const Text::Outline& outline);

  /**
   * @brief Resets the outline.
   */
  void ResetOutline();

  /**
   * @brief Sets the line-through style.
   *
   * @param[in] lineThrough The line-through configuration.
   */
  InputField& SetLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @brief Resets the line-through.
   */
  void ResetLineThrough();

  // @CHAIN_END

public: // Signals
  /**
   * @brief This signal is emitted when the text content changes.
   *
   * @code
   *   void OnTextChanged(View view);
   * @endcode
   * @return The signal to connect to.
   */
  Signal<void(View)>& TextChangedSignal();

  /**
   * @brief This signal is emitted when the text input reaches the maximum allowed length.
   *
   * The signal is triggered when an attempt is made to insert additional
   * characters beyond the configured maximum length.
   *
   * @code
   *   void OnMaximumLengthReached(View view);
   * @endcode
   *
   * @return The signal to connect to.
   */
  Signal<void(View)>& MaximumLengthReachedSignal();

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief Creates a handle using the Internal implementation.
   *
   * @param[in] implementation The InputField implementation
   */
  explicit DALI_UI_API InputField(Integration::InputFieldImpl& implementation);

  /**
   * @brief Allows the creation of this InputField from an Internal::CustomActor pointer.
   *
   * @param[in] internal A pointer to the internal CustomActor
   */
  explicit DALI_UI_API InputField(Dali::Internal::CustomActor* internal);
  /// @endcond

public:
  // Property
  struct Property
  {
    /**
     * @brief Enumeration for the instance of properties belonging to the InputField class.
     */
    enum
    {
      ///////////////////////////////////////////////////////////////////////////////
      // Event side (non-animatable) properties
      ///////////////////////////////////////////////////////////////////////////////
      /**
       * @brief The text to display in UTF-8 format.
       * @details Name "text", type Property::STRING.
       * @note See also: InputField::SetText(), InputField::GetText().
       */
      TEXT = Text::InputFieldPropertyIndex::TEXT,

      /**
       * @brief The font family of the text.
       * @details Name "fontFamily", type Property::STRING.
       * @note See also: InputField::SetFontFamily(), InputField::GetFontFamily().
       */
      FONT_FAMILY = Text::InputFieldPropertyIndex::FONT_FAMILY,

      /**
       * @brief The size of font in pixels.
       * @details Name "fontSize", type Property::FLOAT.
       * @note See also: InputField::SetFontSize(), InputField::GetFontSize().
       */
      FONT_SIZE = Text::InputFieldPropertyIndex::FONT_SIZE,

      /**
       * @brief The color of the text.
       * @details Name "textColor", type Property::VECTOR4.
       * @note See also: InputField::SetTextColor(), InputField::GetTextColor().
       */
      TEXT_COLOR = Text::InputFieldPropertyIndex::TEXT_COLOR,

      /**
       * @brief The horizontal alignment.
       * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::Alignment (Property::INTEGER).
       * @note See also: InputField::SetHorizontalTextAlignment(), InputField::GetHorizontalTextAlignment().
       */
      HORIZONTAL_ALIGNMENT = Text::InputFieldPropertyIndex::HORIZONTAL_ALIGNMENT,

      /**
       * @brief The vertical alignment.
       * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::Alignment (Property::INTEGER).
       * @note See also: InputField::SetVerticalTextAlignment(), InputField::GetVerticalTextAlignment().
       */
      VERTICAL_ALIGNMENT = Text::InputFieldPropertyIndex::VERTICAL_ALIGNMENT,

      /**
       * @brief The overflow mode.
       * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::OverflowMode (Property::INTEGER).
       * @note See also: InputField::SetOverflowMode(), InputField::GetOverflowMode().
       */
      OVERFLOW_MODE = Text::InputFieldPropertyIndex::OVERFLOW_MODE,

      /**
       * @brief The placeholder text displayed when the input field is empty.
       * @details Name "placeholder", type Property::STRING.
       * @note See also: InputField::SetPlaceholder(), InputField::GetPlaceholder().
       */
      PLACEHOLDER = Text::InputFieldPropertyIndex::PLACEHOLDER,

      /**
       * @brief The color of the placeholder text.
       * @details Name "placeholderColor", type Property::VECTOR4.
       * @note See also: InputField::SetPlaceholderColor(), InputField::GetPlaceholderColor().
       */
      PLACEHOLDER_COLOR = Text::InputFieldPropertyIndex::PLACEHOLDER_COLOR,

      /**
       * @brief The width of the text cursor in pixels.
       * @details Name "cursorWidth", type Property::INTEGER.
       * @note See also: InputField::SetCursorWidth(), InputField::GetCursorWidth().
       */
      CURSOR_WIDTH = Text::InputFieldPropertyIndex::CURSOR_WIDTH,

      /**
       * @brief The color of the text cursor.
       * @details Name "cursorColor", type Property::VECTOR4.
       * @note See also: InputField::SetCursorColor(), InputField::GetCursorColor().
       */
      CURSOR_COLOR = Text::InputFieldPropertyIndex::CURSOR_COLOR,

      /**
       * @brief The highlight color of the selected text region.
       * @details Name "selectionColor", type Property::VECTOR4.
       * @note See also: InputField::SetSelectionColor(), InputField::GetSelectionColor().
       */
      SELECTION_COLOR = Text::InputFieldPropertyIndex::SELECTION_COLOR,

      /**
       * @brief The maximum number of characters that can be entered.
       * @details Name "maximumLength", type Property::INTEGER.
       * @note See also: InputField::SetMaximumLength(), InputField::GetMaximumLength().
       */
      MAXIMUM_LENGTH = Text::InputFieldPropertyIndex::MAXIMUM_LENGTH,

      /**
       * @brief The layout direction mode.
       * @details Name "layoutDirectionMode", type Text::LayoutDirectionMode (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::LayoutDirectionMode (Property::INTEGER).
       * @note See also: InputField::SetLayoutDirectionMode(), InputField::GetLayoutDirectionMode().
       */
      LAYOUT_DIRECTION_MODE = Text::InputFieldPropertyIndex::LAYOUT_DIRECTION_MODE,

      /**
       * @brief The font weight.
       * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::FontWeight (Property::INTEGER).
       * @note See also: InputField::SetFontWeight(), InputField::GetFontWeight().
       */
      FONT_WEIGHT = Text::InputFieldPropertyIndex::FONT_WEIGHT,

      /**
       * @brief The font width.
       * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::FontWidth (Property::INTEGER).
       * @note See also: InputField::SetFontWidth(), InputField::GetFontWidth().
       */
      FONT_WIDTH = Text::InputFieldPropertyIndex::FONT_WIDTH,

      /**
       * @brief The font slant.
       * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
       * @note Return type is Text::FontSlant (Property::INTEGER).
       * @note See also: InputField::SetFontSlant(), InputField::GetFontSlant().
       */
      FONT_SLANT = Text::InputFieldPropertyIndex::FONT_SLANT
    };
  };

public:
  DALI_UI_CHAIN_VIEW_METHODS(InputField)
};

} // namespace Ui

} // namespace Dali
