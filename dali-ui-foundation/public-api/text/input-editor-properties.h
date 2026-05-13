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
struct InputEditorPropertyIndex
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
   * @brief Enumeration for the instance of properties belonging to the InputEditor class.
   */
  enum
  {
    ///////////////////////////////////////////////////////////////////////////////
    // Event side (non-animatable) properties
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The text to display in UTF-8 format.
     * @details Name "text", type Property::STRING.
     * @see InputEditor::SetText(), InputEditor::GetText().
     */
    TEXT = PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @see InputEditor::SetFontFamily(), InputEditor::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @see InputEditor::SetFontSize(), InputEditor::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The color of the text.
     * @details Name "textColor", type Property::VECTOR4.
     * @see InputEditor::SetTextColor(), InputEditor::GetTextColor().
     */
    TEXT_COLOR,

    /**
     * @brief Line wrap mode when text lines are greater than the layout width.
     * @details Name "lineWrapMode", type Text::LineWrapMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LineWrapMode (Property::INTEGER).
     * @see InputEditor::SetLineWrapMode(), InputEditor::GetLineWrapMode().
     */
    LINE_WRAP_MODE,

    /**
     * @brief The horizontal alignment.
     * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see InputEditor::SetHorizontalTextAlignment(), InputEditor::GetHorizontalTextAlignment().
     */
    HORIZONTAL_ALIGNMENT,

    /**
     * @brief The vertical alignment.
     * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see InputEditor::SetVerticalTextAlignment(), InputEditor::GetVerticalTextAlignment().
     */
    VERTICAL_ALIGNMENT,

    /**
     * @brief The overflow mode.
     * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::OverflowMode (Property::INTEGER).
     * @see InputEditor::SetOverflowMode(), InputEditor::GetOverflowMode().
     */
    OVERFLOW_MODE,

    /**
     * @brief The placeholder text displayed when the input editor is empty.
     * @details Name "placeholder", type Property::STRING.
     * @see InputEditor::SetPlaceholder(), InputEditor::GetPlaceholder().
     */
    PLACEHOLDER,

    /**
     * @brief The color of the placeholder text.
     * @details Name "placeholderColor", type Property::VECTOR4.
     * @see InputEditor::SetPlaceholderColor(), InputEditor::GetPlaceholderColor().
     */
    PLACEHOLDER_COLOR,

    /**
     * @brief Whether the placeholder text is shown when the input editor has focus.
     * @details Name "showPlaceholderOnFocus", type Property::BOOLEAN.
     * @see InputEditor::SetShowPlaceholderOnFocus(), InputEditor::IsPlaceholderShownOnFocus().
     */
    SHOW_PLACEHOLDER_ON_FOCUS,

    /**
     * @brief The width of the text cursor in pixels.
     * @details Name "cursorWidth", type Property::INTEGER.
     * @see InputEditor::SetCursorWidth(), InputEditor::GetCursorWidth().
     */
    CURSOR_WIDTH,

    /**
     * @brief The color of the text cursor.
     * @details Name "cursorColor", type Property::VECTOR4.
     * @see InputEditor::SetCursorColor(), InputEditor::GetCursorColor().
     */
    CURSOR_COLOR,

    /**
     * @brief Whether the cursor should blink.
     * @details Name "cursorBlinkEnabled", type Property::BOOLEAN.
     * @see InputEditor::SetCursorBlinkEnabled(), InputEditor::IsCursorBlinkEnabled().
     */
    CURSOR_BLINK_ENABLED,

    /**
     * @brief The time interval in seconds between cursor on and off states.
     * @details Name "cursorBlinkInterval", type Property::FLOAT.
     * @see InputEditor::SetCursorBlinkInterval(), InputEditor::GetCursorBlinkInterval().
     */
    CURSOR_BLINK_INTERVAL,

    /**
     * @brief The current cursor position.
     * @details Name "cursorPosition", type Property::INTEGER.
     * @see InputEditor::SetCursorPosition(), InputEditor::GetCursorPosition().
     */
    CURSOR_POSITION,

    /**
     * @brief Whether text selection is enabled.
     * @details Name "selectionEnabled", type Property::BOOLEAN.
     * @see InputEditor::SetSelectionEnabled(), InputEditor::IsSelectionEnabled().
     */
    SELECTION_ENABLED,

    /**
     * @brief The highlight color of the selected text region.
     * @details Name "selectionColor", type Property::VECTOR4.
     * @see InputEditor::SetSelectionColor(), InputEditor::GetSelectionColor().
     */
    SELECTION_COLOR,

    /**
     * @brief The currently selected text.
     * @details Name "selectedText", type Property::STRING.
     * @note This property is read-only.
     * @see InputEditor::GetSelectedText().
     */
    SELECTED_TEXT,

    /**
     * @brief The start position of the selected text range.
     * @details Name "selectedTextStart", type Property::INTEGER.
     * @note This property is read-only.
     * @see InputEditor::GetSelectedTextStart().
     */
    SELECTED_TEXT_START,

    /**
     * @brief The end position of the selected text range.
     * @details Name "selectedTextEnd", type Property::INTEGER.
     * @note This property is read-only.
     * @see InputEditor::GetSelectedTextEnd().
     */
    SELECTED_TEXT_END,

    /**
     * @brief Whether text editing handles are enabled.
     * @details Name "textHandleEnabled", type Property::BOOLEAN.
     * @see InputEditor::SetTextHandleEnabled(), InputEditor::IsTextHandleEnabled().
     */
    TEXT_HANDLE_ENABLED,

    /**
     * @brief The color of the text editing handles.
     * @details Name "textHandleColor", type Property::VECTOR4.
     * @see InputEditor::SetTextHandleColor(), InputEditor::GetTextHandleColor().
     */
    TEXT_HANDLE_COLOR,

    /**
     * @brief The cursor handle image.
     * @details Name "cursorHandleImage", type Property::STRING.
     * @see InputEditor::SetCursorHandleImage(), InputEditor::GetCursorHandleImage().
     */
    CURSOR_HANDLE_IMAGE,

    /**
     * @brief The pressed cursor handle image.
     * @details Name "cursorHandlePressedImage", type Property::STRING.
     * @see InputEditor::SetCursorHandlePressedImage(), InputEditor::GetCursorHandlePressedImage().
     */
    CURSOR_HANDLE_PRESSED_IMAGE,

    /**
     * @brief The left selection handle image.
     * @details Name "selectionHandleImageLeft", type Property::STRING.
     * @see InputEditor::SetSelectionHandleImageLeft(), InputEditor::GetSelectionHandleImageLeft().
     */
    SELECTION_HANDLE_IMAGE_LEFT,

    /**
     * @brief The right selection handle image.
     * @details Name "selectionHandleImageRight", type Property::STRING.
     * @see InputEditor::SetSelectionHandleImageRight(), InputEditor::GetSelectionHandleImageRight().
     */
    SELECTION_HANDLE_IMAGE_RIGHT,

    /**
     * @brief The pressed left selection handle image.
     * @details Name "selectionHandlePressedImageLeft", type Property::STRING.
     * @see InputEditor::SetSelectionHandlePressedImageLeft(), InputEditor::GetSelectionHandlePressedImageLeft().
     */
    SELECTION_HANDLE_PRESSED_IMAGE_LEFT,

    /**
     * @brief The pressed right selection handle image.
     * @details Name "selectionHandlePressedImageRight", type Property::STRING.
     * @see InputEditor::SetSelectionHandlePressedImageRight(), InputEditor::GetSelectionHandlePressedImageRight().
     */
    SELECTION_HANDLE_PRESSED_IMAGE_RIGHT,

    /**
     * @brief The maximum number of characters that can be entered.
     * @details Name "maximumLength", type Property::INTEGER.
     * @see InputEditor::SetMaximumLength(), InputEditor::GetMaximumLength().
     */
    MAXIMUM_LENGTH,

    /**
     * @brief Whether the input editor can be edited by user interaction.
     * @details Name "editable", type Property::BOOLEAN.
     * @see InputEditor::SetEditable(), InputEditor::IsEditable().
     */
    EDITABLE,

    /**
     * @brief The layout direction mode.
     * @details Name "layoutDirectionMode", type Text::LayoutDirectionMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LayoutDirectionMode (Property::INTEGER).
     * @see InputEditor::SetLayoutDirectionMode(), InputEditor::GetLayoutDirectionMode().
     */
    LAYOUT_DIRECTION_MODE,

    /**
     * @brief Whether mark-up processing is enabled for the text.
     * @details Name "markupEnabled", type Property::BOOLEAN.
     * @see InputEditor::SetMarkupEnabled(), InputEditor::IsMarkupEnabled().
     */
    MARKUP_ENABLED,

    /**
     * @brief The font weight.
     * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @see InputEditor::SetFontWeight(), InputEditor::GetFontWeight().
     */
    FONT_WEIGHT,

    /**
     * @brief The font width.
     * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @see InputEditor::SetFontWidth(), InputEditor::GetFontWidth().
     */
    FONT_WIDTH,

    /**
     * @brief The font slant.
     * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @see InputEditor::SetFontSlant(), InputEditor::GetFontSlant().
     */
    FONT_SLANT,

    /**
     * @brief The background color behind the text.
     * @details Name "textBackgroundColor", type Property::VECTOR4.
     * @note The background is rendered behind the text glyphs.
     * @see InputEditor::SetTextBackgroundColor(), InputEditor::GetTextBackgroundColor(), InputEditor::ClearTextBackgroundColor().
     */
    TEXT_BACKGROUND_COLOR,

    /**
     * @brief The font size scale.
     * @details Name "fontSizeScale", type Property::FLOAT.
     * @see InputEditor::SetFontSizeScale(), InputEditor::GetFontSizeScale().
     */
    FONT_SIZE_SCALE,

    /**
     * @brief The minimum font size scale.
     * @details Name "minimumFontSizeScale", type Property::FLOAT.
     * @see InputEditor::SetMinimumFontSizeScale(), InputEditor::GetMinimumFontSizeScale().
     */
    MINIMUM_FONT_SIZE_SCALE,

    /**
     * @brief The maximum font size scale.
     * @details Name "maximumFontSizeScale", type Property::FLOAT.
     * @see InputEditor::SetMaximumFontSizeScale(), InputEditor::GetMaximumFontSizeScale().
     */
    MAXIMUM_FONT_SIZE_SCALE,

    /**
     * @brief Whether the system font size scale is applied.
     * @details Name "systemFontSizeScaleEnabled", type Property::BOOLEAN.
     * @see InputEditor::SetSystemFontSizeScaleEnabled(), InputEditor::IsSystemFontSizeScaleEnabled().
     */
    SYSTEM_FONT_SIZE_SCALE_ENABLED,

    /**
     * @brief The text color used for typing.
     * @details Name "typingTextColor", type Property::VECTOR4.
     * @see InputEditor::SetTypingTextColor(), InputEditor::GetTypingTextColor().
     */
    TYPING_TEXT_COLOR,

    /**
     * @brief The font family used for typing.
     * @details Name "typingFontFamily", type Property::STRING.
     * @see InputEditor::SetTypingFontFamily(), InputEditor::GetTypingFontFamily().
     */
    TYPING_FONT_FAMILY,

    /**
     * @brief The font size in pixels used for typing.
     * @details Name "typingFontSize", type Property::FLOAT.
     * @see InputEditor::SetTypingFontSize(), InputEditor::GetTypingFontSize().
     */
    TYPING_FONT_SIZE,

    /**
     * @brief The font weight used for typing.
     * @details Name "typingFontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @see InputEditor::SetTypingFontWeight(), InputEditor::GetTypingFontWeight().
     */
    TYPING_FONT_WEIGHT,

    /**
     * @brief The font width used for typing.
     * @details Name "typingFontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @see InputEditor::SetTypingFontWidth(), InputEditor::GetTypingFontWidth().
     */
    TYPING_FONT_WIDTH,

    /**
     * @brief The font slant used for typing.
     * @details Name "typingFontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @see InputEditor::SetTypingFontSlant(), InputEditor::GetTypingFontSlant().
     */
    TYPING_FONT_SLANT
  };
};

} // namespace Text
} // namespace Ui
} // namespace Dali
