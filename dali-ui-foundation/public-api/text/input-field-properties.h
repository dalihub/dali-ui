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
     * @note See also: InputField::SetText(), InputField::GetText().
     */
    TEXT = PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @note See also: InputField::SetFontFamily(), InputField::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @note See also: InputField::SetFontSize(), InputField::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The color of the text.
     * @details Name "textColor", type Property::VECTOR4.
     * @note See also: InputField::SetTextColor(), InputField::GetTextColor().
     */
    TEXT_COLOR,

    /**
     * @brief The horizontal alignment.
     * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @note See also: InputField::SetHorizontalTextAlignment(), InputField::GetHorizontalTextAlignment().
     */
    HORIZONTAL_ALIGNMENT,

    /**
     * @brief The vertical alignment.
     * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @note See also: InputField::SetVerticalTextAlignment(), InputField::GetVerticalTextAlignment().
     */
    VERTICAL_ALIGNMENT,

    /**
     * @brief The overflow mode.
     * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::OverflowMode (Property::INTEGER).
     * @note See also: InputField::SetOverflowMode(), InputField::GetOverflowMode().
     */
    OVERFLOW_MODE,

    /**
     * @brief The placeholder text displayed when the input field is empty.
     * @details Name "placeholder", type Property::STRING.
     * @note See also: InputField::SetPlaceholder(), InputField::GetPlaceholder().
     */
    PLACEHOLDER,

    /**
     * @brief The color of the placeholder text.
     * @details Name "placeholderColor", type Property::VECTOR4.
     * @note See also: InputField::SetPlaceholderColor(), InputField::GetPlaceholderColor().
     */
    PLACEHOLDER_COLOR,

    /**
     * @brief The width of the text cursor in pixels.
     * @details Name "cursorWidth", type Property::INTEGER.
     * @note See also: InputField::SetCursorWidth(), InputField::GetCursorWidth().
     */
    CURSOR_WIDTH,

    /**
     * @brief The color of the text cursor.
     * @details Name "cursorColor", type Property::VECTOR4.
     * @note See also: InputField::SetCursorColor(), InputField::GetCursorColor().
     */
    CURSOR_COLOR,

    /**
     * @brief The highlight color of the selected text region.
     * @details Name "selectionColor", type Property::VECTOR4.
     * @note See also: InputField::SetSelectionColor(), InputField::GetSelectionColor().
     */
    SELECTION_COLOR,

    /**
     * @brief The maximum number of characters that can be entered.
     * @details Name "maximumLength", type Property::INTEGER.
     * @note See also: InputField::SetMaximumLength(), InputField::GetMaximumLength().
     */
    MAXIMUM_LENGTH,

    /**
     * @brief The layout direction mode.
     * @details Name "layoutDirectionMode", type Text::LayoutDirectionMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LayoutDirectionMode (Property::INTEGER).
     * @note See also: InputField::SetLayoutDirectionMode(), InputField::GetLayoutDirectionMode().
     */
    LAYOUT_DIRECTION_MODE,

    /**
     * @brief The font weight.
     * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @note See also: InputField::SetFontWeight(), InputField::GetFontWeight().
     */
    FONT_WEIGHT,

    /**
     * @brief The font width.
     * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @note See also: InputField::SetFontWidth(), InputField::GetFontWidth().
     */
    FONT_WIDTH,

    /**
     * @brief The font slant.
     * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @note See also: InputField::SetFontSlant(), InputField::GetFontSlant().
     */
    FONT_SLANT,

    /**
     * @brief The background color behind the text.
     * @details Name "textBackgroundColor", type Property::VECTOR4.
     * @note The background is rendered behind the text glyphs.
     * @note See also: InputField::SetTextBackgroundColor(), InputField::GetTextBackgroundColor(), InputField::ClearTextBackgroundColor().
     */
    TEXT_BACKGROUND_COLOR,

    /**
     * @brief The font size scale.
     * @details Name "fontSizeScale", type Property::FLOAT.
     * @note See also: InputField::SetFontSizeScale(), InputField::GetFontSizeScale().
     */
    FONT_SIZE_SCALE,

    /**
     * @brief The minimum font size scale.
     * @details Name "minimumFontSizeScale", type Property::FLOAT.
     * @note See also: InputField::SetMinimumFontSizeScale(), InputField::GetMinimumFontSizeScale().
     */
    MINIMUM_FONT_SIZE_SCALE,

    /**
     * @brief The maximum font size scale.
     * @details Name "maximumFontSizeScale", type Property::FLOAT.
     * @note See also: InputField::SetMaximumFontSizeScale(), InputField::GetMaximumFontSizeScale().
     */
    MAXIMUM_FONT_SIZE_SCALE,

    /**
     * @brief Whether the system font size scale is applied.
     * @details Name "systemFontSizeScaleEnabled", type Property::BOOLEAN.
     * @note See also: InputField::SetSystemFontSizeScaleEnabled(), InputField::IsSystemFontSizeScaleEnabled().
     */
    SYSTEM_FONT_SIZE_SCALE_ENABLED,
  };
};

} // namespace Text
} // namespace Ui
} // namespace Dali
