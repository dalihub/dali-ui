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
#include <dali/public-api/object/property-index-ranges.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
struct InputFieldPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Ui::View::VIEW_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000 ///< Reserve property indices.
  };

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
     * @see InputField::SetText(), InputField::GetText().
     */
    TEXT = PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @see InputField::SetFontFamily(), InputField::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @see InputField::SetFontSize(), InputField::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The color of the text.
     * @details Name "textColor", type Property::VECTOR4.
     * @see InputField::SetTextColor(), InputField::GetTextColor().
     */
    TEXT_COLOR,

    /**
     * @brief The horizontal alignment.
     * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see InputField::SetHorizontalTextAlignment(), InputField::GetHorizontalTextAlignment().
     */
    HORIZONTAL_ALIGNMENT,

    /**
     * @brief The vertical alignment.
     * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see InputField::SetVerticalTextAlignment(), InputField::GetVerticalTextAlignment().
     */
    VERTICAL_ALIGNMENT,

    /**
     * @brief The overflow mode.
     * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::OverflowMode (Property::INTEGER).
     * @see InputField::SetOverflowMode(), InputField::GetOverflowMode().
     */
    OVERFLOW_MODE,

    /**
     * @brief The placeholder text displayed when the input field is empty.
     * @details Name "placeholder", type Property::STRING.
     * @see InputField::SetPlaceholder(), InputField::GetPlaceholder().
     */
    PLACEHOLDER,

    /**
     * @brief The color of the placeholder text.
     * @details Name "placeholderColor", type Property::VECTOR4.
     * @see InputField::SetPlaceholderColor(), InputField::GetPlaceholderColor().
     */
    PLACEHOLDER_COLOR,

    /**
     * @brief Whether the placeholder text is shown when the input field has focus.
     * @details Name "showPlaceholderOnFocus", type Property::BOOLEAN.
     * @see InputField::SetShowPlaceholderOnFocus(), InputField::IsPlaceholderShownOnFocus().
     */
    SHOW_PLACEHOLDER_ON_FOCUS,

    /**
     * @brief The width of the text cursor in pixels.
     * @details Name "cursorWidth", type Property::INTEGER.
     * @see InputField::SetCursorWidth(), InputField::GetCursorWidth().
     */
    CURSOR_WIDTH,

    /**
     * @brief The color of the text cursor.
     * @details Name "cursorColor", type Property::VECTOR4.
     * @see InputField::SetCursorColor(), InputField::GetCursorColor().
     */
    CURSOR_COLOR,

    /**
     * @brief Whether the cursor should blink.
     * @details Name "cursorBlinkEnabled", type Property::BOOLEAN.
     * @see InputField::SetCursorBlinkEnabled(), InputField::IsCursorBlinkEnabled().
     */
    CURSOR_BLINK_ENABLED,

    /**
     * @brief The time interval in seconds between cursor on and off states.
     * @details Name "cursorBlinkInterval", type Property::FLOAT.
     * @see InputField::SetCursorBlinkInterval(), InputField::GetCursorBlinkInterval().
     */
    CURSOR_BLINK_INTERVAL,

    /**
     * @brief The current cursor position.
     * @details Name "cursorPosition", type Property::INTEGER.
     * @see InputField::SetCursorPosition(), InputField::GetCursorPosition().
     */
    CURSOR_POSITION,

    /**
     * @brief Whether text selection is enabled.
     * @details Name "selectionEnabled", type Property::BOOLEAN.
     * @see InputField::SetSelectionEnabled(), InputField::IsSelectionEnabled().
     */
    SELECTION_ENABLED,

    /**
     * @brief The highlight color of the selected text region.
     * @details Name "selectionColor", type Property::VECTOR4.
     * @see InputField::SetSelectionColor(), InputField::GetSelectionColor().
     */
    SELECTION_COLOR,

    /**
     * @brief The currently selected text.
     * @details Name "selectedText", type Property::STRING.
     * @note This property is read-only.
     * @see InputField::GetSelectedText().
     */
    SELECTED_TEXT,

    /**
     * @brief The start position of the selected text range.
     * @details Name "selectedTextStart", type Property::INTEGER.
     * @note This property is read-only.
     * @see InputField::GetSelectedTextStart().
     */
    SELECTED_TEXT_START,

    /**
     * @brief The end position of the selected text range.
     * @details Name "selectedTextEnd", type Property::INTEGER.
     * @note This property is read-only.
     * @see InputField::GetSelectedTextEnd().
     */
    SELECTED_TEXT_END,

    /**
     * @brief The maximum number of characters that can be entered.
     * @details Name "maximumLength", type Property::INTEGER.
     * @see InputField::SetMaximumLength(), InputField::GetMaximumLength().
     */
    MAXIMUM_LENGTH,

    /**
     * @brief Whether the input field can be edited by user interaction.
     * @details Name "editable", type Property::BOOLEAN.
     * @see InputField::SetEditable(), InputField::IsEditable().
     */
    EDITABLE,

    /**
     * @brief The layout direction mode.
     * @details Name "layoutDirectionMode", type Text::LayoutDirectionMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LayoutDirectionMode (Property::INTEGER).
     * @see InputField::SetLayoutDirectionMode(), InputField::GetLayoutDirectionMode().
     */
    LAYOUT_DIRECTION_MODE,

    /**
     * @brief The font weight.
     * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @see InputField::SetFontWeight(), InputField::GetFontWeight().
     */
    FONT_WEIGHT,

    /**
     * @brief The font width.
     * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @see InputField::SetFontWidth(), InputField::GetFontWidth().
     */
    FONT_WIDTH,

    /**
     * @brief The font slant.
     * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @see InputField::SetFontSlant(), InputField::GetFontSlant().
     */
    FONT_SLANT,

    /**
     * @brief The background color behind the text.
     * @details Name "textBackgroundColor", type Property::VECTOR4.
     * @note The background is rendered behind the text glyphs.
     * @see InputField::SetTextBackgroundColor(), InputField::GetTextBackgroundColor(), InputField::ClearTextBackgroundColor().
     */
    TEXT_BACKGROUND_COLOR,

    /**
     * @brief The font size scale.
     * @details Name "fontSizeScale", type Property::FLOAT.
     * @see InputField::SetFontSizeScale(), InputField::GetFontSizeScale().
     */
    FONT_SIZE_SCALE,

    /**
     * @brief The minimum font size scale.
     * @details Name "minimumFontSizeScale", type Property::FLOAT.
     * @see InputField::SetMinimumFontSizeScale(), InputField::GetMinimumFontSizeScale().
     */
    MINIMUM_FONT_SIZE_SCALE,

    /**
     * @brief The maximum font size scale.
     * @details Name "maximumFontSizeScale", type Property::FLOAT.
     * @see InputField::SetMaximumFontSizeScale(), InputField::GetMaximumFontSizeScale().
     */
    MAXIMUM_FONT_SIZE_SCALE,

    /**
     * @brief Whether the system font size scale is applied.
     * @details Name "systemFontSizeScaleEnabled", type Property::BOOLEAN.
     * @see InputField::SetSystemFontSizeScaleEnabled(), InputField::IsSystemFontSizeScaleEnabled().
     */
    SYSTEM_FONT_SIZE_SCALE_ENABLED,
  };
};

} // namespace Text
} // namespace Ui
} // namespace Dali
