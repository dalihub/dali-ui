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
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>
#include <dali-ui-foundation/public-api/text/input-field-properties.h>
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
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
  /**
   * @brief Property indices for InbputField.
   *
   * @note See Dali::Ui::Text::InputFieldPropertyIndex for the underlying property definitions.
   */
  struct Property
  {
    enum
    {
      TEXT                           = Text::InputFieldPropertyIndex::TEXT,
      FONT_FAMILY                    = Text::InputFieldPropertyIndex::FONT_FAMILY,
      FONT_SIZE                      = Text::InputFieldPropertyIndex::FONT_SIZE,
      TEXT_COLOR                     = Text::InputFieldPropertyIndex::TEXT_COLOR,
      HORIZONTAL_ALIGNMENT           = Text::InputFieldPropertyIndex::HORIZONTAL_ALIGNMENT,
      VERTICAL_ALIGNMENT             = Text::InputFieldPropertyIndex::VERTICAL_ALIGNMENT,
      OVERFLOW_MODE                  = Text::InputFieldPropertyIndex::OVERFLOW_MODE,
      PLACEHOLDER                    = Text::InputFieldPropertyIndex::PLACEHOLDER,
      PLACEHOLDER_COLOR              = Text::InputFieldPropertyIndex::PLACEHOLDER_COLOR,
      SHOW_PLACEHOLDER_ON_FOCUS      = Text::InputFieldPropertyIndex::SHOW_PLACEHOLDER_ON_FOCUS,
      CURSOR_WIDTH                   = Text::InputFieldPropertyIndex::CURSOR_WIDTH,
      CURSOR_COLOR                   = Text::InputFieldPropertyIndex::CURSOR_COLOR,
      CURSOR_BLINK_ENABLED           = Text::InputFieldPropertyIndex::CURSOR_BLINK_ENABLED,
      CURSOR_BLINK_INTERVAL          = Text::InputFieldPropertyIndex::CURSOR_BLINK_INTERVAL,
      CURSOR_POSITION                = Text::InputFieldPropertyIndex::CURSOR_POSITION,
      SELECTION_ENABLED              = Text::InputFieldPropertyIndex::SELECTION_ENABLED,
      SELECTION_COLOR                = Text::InputFieldPropertyIndex::SELECTION_COLOR,
      SELECTED_TEXT                  = Text::InputFieldPropertyIndex::SELECTED_TEXT,
      SELECTED_TEXT_START            = Text::InputFieldPropertyIndex::SELECTED_TEXT_START,
      SELECTED_TEXT_END              = Text::InputFieldPropertyIndex::SELECTED_TEXT_END,
      MAXIMUM_LENGTH                 = Text::InputFieldPropertyIndex::MAXIMUM_LENGTH,
      EDITABLE                       = Text::InputFieldPropertyIndex::EDITABLE,
      LAYOUT_DIRECTION_MODE          = Text::InputFieldPropertyIndex::LAYOUT_DIRECTION_MODE,
      FONT_WEIGHT                    = Text::InputFieldPropertyIndex::FONT_WEIGHT,
      FONT_WIDTH                     = Text::InputFieldPropertyIndex::FONT_WIDTH,
      FONT_SLANT                     = Text::InputFieldPropertyIndex::FONT_SLANT,
      TEXT_BACKGROUND_COLOR          = Text::InputFieldPropertyIndex::TEXT_BACKGROUND_COLOR,
      FONT_SIZE_SCALE                = Text::InputFieldPropertyIndex::FONT_SIZE_SCALE,
      MINIMUM_FONT_SIZE_SCALE        = Text::InputFieldPropertyIndex::MINIMUM_FONT_SIZE_SCALE,
      MAXIMUM_FONT_SIZE_SCALE        = Text::InputFieldPropertyIndex::MAXIMUM_FONT_SIZE_SCALE,
      SYSTEM_FONT_SIZE_SCALE_ENABLED = Text::InputFieldPropertyIndex::SYSTEM_FONT_SIZE_SCALE_ENABLED
    };
  };

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
   */
  InputField& SetPlaceholderColor(const UiColor& color);

  /**
   * @brief Gets the color of the placeholder text.
   *
   * @return The placeholder text color as a UiColor.
   */
  UiColor GetPlaceholderColor();

  /**
   * @brief Sets whether the placeholder text is shown when the input field has focus.
   *
   * @param[in] enabled True to show the placeholder text when focused, false otherwise.
   */
  InputField& SetShowPlaceholderOnFocus(bool enabled);

  /**
   * @brief Returns whether the placeholder text is shown when the input field has focus.
   *
   * @return True if the placeholder text is shown when focused, false otherwise.
   */
  bool IsPlaceholderShownOnFocus() const;

  /**
   * @brief Sets the width of the text cursor.
   *
   * @param[in] width The cursor width in pixels.
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
   */
  InputField& SetCursorColor(const UiColor& color);

  /**
   * @brief Gets the color of the text cursor.
   *
   * @return The cursor color as a UiColor.
   */
  UiColor GetCursorColor();

  /**
   * @brief Sets whether the cursor should blink.
   *
   * @param[in] enabled True to enable cursor blinking, false otherwise.
   */
  InputField& SetCursorBlinkEnabled(bool enabled);

  /**
   * @brief Returns whether the cursor is set to blink.
   *
   * @return True if cursor blinking is enabled, false otherwise.
   */
  bool IsCursorBlinkEnabled() const;

  /**
   * @brief Sets the time interval in seconds between cursor on and off states.
   *
   * @param[in] interval The cursor blink interval in seconds.
   */
  InputField& SetCursorBlinkInterval(float interval);

  /**
   * @brief Returns the time interval in seconds between cursor on and off states.
   *
   * @return The cursor blink interval in seconds.
   */
  float GetCursorBlinkInterval() const;

  /**
   * @brief Sets the cursor position.
   *
   * The position is specified as a character index in the current text.
   *
   * @param[in] position The cursor position.
   */
  InputField& SetCursorPosition(uint32_t position);

  /**
   * @brief Returns the current cursor position.
   *
   * The returned value is the character index in the current text.
   *
   * @return The current cursor position.
   */
  uint32_t GetCursorPosition() const;

  /**
   * @brief Sets whether text selection is enabled.
   *
   * @param[in] enabled True to enable text selection, false otherwise.
   */
  InputField& SetSelectionEnabled(bool enabled);

  /**
   * @brief Returns whether text selection is enabled.
   *
   * @return True if text selection is enabled, false otherwise.
   */
  bool IsSelectionEnabled() const;

  /**
   * @brief Sets the highlight color of the selected text region.
   *
   * @param[in] color The selection highlight color as a UiColor).
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
   * @brief Sets whether the InputField can be edited by user interaction.
   *
   * @param[in] editable True to allow editing, false otherwise.
   */
  InputField& SetEditable(bool editable);

  /**
   * @brief Returns whether the InputField can be edited by user interaction.
   *
   * @return True if the InputField is editable, false otherwise.
   */
  bool IsEditable() const;

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
   * @brief Sets the background color behind the text.
   *
   * The background is rendered behind the glyphs of the text.
   *
   * @param[in] color The text background color.
   *
   * @return This input field.
   */
  InputField& SetTextBackgroundColor(const UiColor& color);

  /**
   * @brief Gets the background color behind the text.
   *
   * @return The current text background color.
   */
  UiColor GetTextBackgroundColor() const;

  /**
   * @brief Clears the text background color.
   *
   * Disables the text background and removes the previously set color.
   */
  void ClearTextBackgroundColor();

  /**
   * @brief Sets the underline style.
   *
   * @param[in] underline The underline configuration.
   */
  InputField& SetUnderline(const Text::Underline& underline);

  /**
   * @brief Clears the underline style.
   */
  void ClearUnderline();

  /**
   * @brief Sets the shadow style.
   *
   * @param[in] shadow The shadow configuration.
   */
  InputField& SetShadow(const Text::Shadow& shadow);

  /**
   * @brief Clears the shadow style.
   */
  void ClearShadow();

  /**
   * @brief Sets the outline style.
   *
   * @param[in] outline The outline configuration.
   */
  InputField& SetOutline(const Text::Outline& outline);

  /**
   * @brief Clears the outline style.
   */
  void ClearOutline();

  /**
   * @brief Sets the line-through style.
   *
   * @param[in] lineThrough The line-through configuration.
   */
  InputField& SetLineThrough(const Text::LineThrough& lineThrough);

  /**
   * @brief Clears the line-through style.
   */
  void ClearLineThrough();

  /**
   * @brief Sets the font size scale.
   *
   * The scaled font size is calculated from the current font size
   * multiplied by this scale value.
   *
   * @param[in] scale The font size scale.
   */
  InputField& SetFontSizeScale(float scale);

  /**
   * @brief Gets the font size scale.
   *
   * @return The font size scale.
   */
  float GetFontSizeScale() const;

  /**
   * @brief Sets the minimum font size scale.
   *
   * If this value is greater than the maximum font size scale,
   * the adjusted font size scale follows this minimum value.
   *
   * @param[in] scale The minimum font size scale.
   */
  InputField& SetMinimumFontSizeScale(float scale);

  /**
   * @brief Gets the minimum font size scale.
   *
   * @return The minimum font size scale.
   */
  float GetMinimumFontSizeScale() const;

  /**
   * @brief Sets the maximum font size scale.
   *
   * If this value is less than the minimum font size scale,
   * the adjusted font size scale follows the minimum font size scale.
   *
   * @param[in] scale The maximum font size scale.
   */
  InputField& SetMaximumFontSizeScale(float scale);

  /**
   * @brief Gets the maximum font size scale.
   *
   * @return The maximum font size scale.
   */
  float GetMaximumFontSizeScale() const;

  /**
   * @brief Sets whether the system font size scale is applied.
   *
   * When enabled, the system font size scale is used instead of
   * the user-defined font size scale before applying the minimum
   * and maximum constraints.
   *
   * @param[in] enabled True to apply the system font size scale, false otherwise.
   */
  InputField& SetSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @brief Gets whether the system font size scale is applied.
   *
   * @return True if the system font size scale is applied, otherwise false.
   */
  bool IsSystemFontSizeScaleEnabled() const;

  /**
   * @brief Sets the font variation axes.
   *
   * This replaces all previously set font variation axes.
   *
   * If duplicate axis tags are provided, the last value is used.
   *
   * Unsupported axis tags may be ignored depending on the selected font.
   *
   * @param[in] axes The font variation axes.
   */
  InputField& SetFontVariation(const Dali::Vector<Text::FontVariationAxis>& axes);

  /**
   * @brief Sets the font variation from a settings string.
   *
   * The settings string consists of one or more pairs of axis tags and
   * numeric values separated by commas.
   *
   * Supported formats include:
   * - wght=700,wdth=90 (recommended)
   * - "wght" 700, "wdth" 90
   * - 'wght' 700, 'wdth' 90
   *
   * In quoted formats, the axis tag must be wrapped with single quotes
   * (U+0027) or double quotes (U+0022).
   *
   * Each axis tag must contain exactly four printable ASCII characters
   * in the range U+0020..U+007E. Space is allowed only as trailing
   * characters in the axis tag.
   *
   * If duplicate axis tags are specified, the last value is used.
   *
   * If the input string is invalid, the font variation is not changed.
   *
   * Unsupported axis tags may be ignored depending on the selected font.
   *
   * @param[in] settings The font variation settings string.
   */
  InputField& SetFontVariation(const Dali::String& settings);

  /**
   * @brief Returns the font variation axes.
   *
   * @return The font variation axes.
   */
  Dali::Vector<Text::FontVariationAxis> GetFontVariation() const;

  /**
   * @brief Clears the font variation.
   *
   * This removes all previously set font variation axes.
   */
  InputField& ClearFontVariation();

  // @CHAIN_END

  // Read Only
  /**
   * @brief Gets the adjusted font size scale used for rendering.
   *
   * The adjusted font size scale is resolved after applying the current
   * minimum and maximum font size scale constraints and, if enabled,
   * the system font size scale.
   *
   * If the minimum font size scale is greater than the maximum font size scale,
   * the minimum font size scale takes precedence and is used as the adjusted scale.
   *
   * @return The adjusted font size scale used for rendering.
   */
  float GetAdjustedFontSizeScale() const;

  /**
   * @brief Gets the currently selected text.
   *
   * @return The selected text in UTF-8 format.
   */
  Dali::String GetSelectedText() const;

  /**
   * @brief Gets the start index of the currently selected text.
   *
   * @return The start index of the selected text.
   */
  uint32_t GetSelectedTextStart() const;

  /**
   * @brief Gets the end index of the currently selected text.
   *
   * @return The end index of the selected text.
   */
  uint32_t GetSelectedTextEnd() const;

  // Method
  /**
   * @brief Selects the text within the specified index range.
   *
   * @param[in] startIndex The start index of the selection.
   * @param[in] endIndex The end index of the selection.
   * @return This input field.
   */
  InputField& SelectText(uint32_t startIndex, uint32_t endIndex);

  /**
   * @brief Selects the whole text.
   *
   * @return This input field.
   */
  InputField& SelectWholeText();

  /**
   * @brief Clears the current selection.
   *
   * @return This input field.
   */
  InputField& ClearSelection();

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

  /**
   * @brief This signal is emitted when the cursor position changes.
   *
   * @code
   *   void OnCursorPositionChanged(View view, uint32_t position);
   * @endcode
   *
   * @param[in] view The view whose cursor position changed.
   * @param[in] position The current cursor position.
   *
   * @return The signal to connect to.
   */
  Signal<void(View, uint32_t)>& CursorPositionChangedSignal();

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
  DALI_UI_CHAIN_VIEW_METHODS(InputField)
};

} // namespace Ui

} // namespace Dali
