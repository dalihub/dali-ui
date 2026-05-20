#ifndef DALI_UI_TEXT_VISUAL_PROPERTIES_H
#define DALI_UI_TEXT_VISUAL_PROPERTIES_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief TextVisual is to render a text.
 */
struct TextVisualPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this visual.
   */
  enum PropertyRange
  {
    MUTABLE_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::MUTABLE_PROPERTY_END_INDEX + 1,
    MUTABLE_PROPERTY_END_INDEX   = MUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

    IMMUTABLE_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::IMMUTABLE_PROPERTY_END_INDEX + 1,
    IMMUTABLE_PROPERTY_END_INDEX   = IMMUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

    READ_ONLY_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::READ_ONLY_PROPERTY_END_INDEX + 1,
    READ_ONLY_PROPERTY_END_INDEX   = READ_ONLY_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the TextVisual.
   */
  enum
  {
    /**
     * @brief The text to display in UTF-8 format.
     * @details Name "text", type Property::STRING.
     * @see TextVisual::SetText(), TextVisual::GetText().
     */
    TEXT = MUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @see TextVisual::SetFontFamily(), TextVisual::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @see TextVisual::SetFontSize(), TextVisual::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The font weight.
     * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @see TextVisual::SetFontWeight(), TextVisual::GetFontWeight().
     */
    FONT_WEIGHT,

    /**
     * @brief The font width.
     * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @see TextVisual::SetFontWidth(), TextVisual::GetFontWidth().
     */
    FONT_WIDTH,

    /**
     * @brief The font slant.
     * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @see TextVisual::SetFontSlant(), TextVisual::GetFontSlant().
     */
    FONT_SLANT,

    /**
     * @brief The single-line or multi-line layout option.
     * @details Name "multiLine", type Property::BOOLEAN.
     * @see TextVisual::SetMultiLine(), TextVisual::IsMultiLine().
     */
    MULTI_LINE,

    /**
     * @brief Line wrap mode when text lines are greater than the layout width.
     * @details Name "lineWrapMode", type Text::LineWrapMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LineWrapMode (Property::INTEGER).
     * @see TextVisual::SetLineWrapMode(), TextVisual::GetLineWrapMode().
     */
    LINE_WRAP_MODE,

    /**
     * @brief The horizontal alignment.
     * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see TextVisual::SetHorizontalAlignment(), TextVisual::GetHorizontalAlignment().
     */
    HORIZONTAL_ALIGNMENT,

    /**
     * @brief The vertical alignment.
     * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see TextVisual::SetVerticalAlignment(), TextVisual::GetVerticalAlignment().
     */
    VERTICAL_ALIGNMENT,

    /**
     * @brief The overflow mode.
     * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::OverflowMode (Property::INTEGER).
     * @see TextVisual::SetOverflowMode(), TextVisual::GetOverflowMode().
     */
    OVERFLOW_MODE,

    /**
     * @brief The line height.
     * @details Name "lineHeight", type Property::FLOAT.
     * @see TextVisual::SetLineHeight(), TextVisual::GetLineHeight().
     */
    LINE_HEIGHT,

    /**
     * @brief The line height mode.
     * @details Name "lineHeightMode", type Text::LineHeightMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LineHeightMode (Property::INTEGER).
     * @see TextVisual::SetLineHeightMode(), TextVisual::GetLineHeightMode().
     */
    LINE_HEIGHT_MODE,

    /**
     * @brief The color of the text.
     * @details Name "textColor", type Property::VECTOR4.
     * @see TextVisual::SetTextColor(), TextVisual::GetTextColor().
     */
    TEXT_COLOR,

    /**
     * @brief Whether mark-up processing is enabled for the text.
     * @details Name "markupEnabled", type Property::BOOLEAN.
     * @see TextVisual::SetMarkupEnabled(), TextVisual::IsMarkupEnabled().
     */
    MARKUP_ENABLED,

    /**
     * @brief Whether cutout rendering is enabled for the text.
     * @details Name "cutoutEnabled", type Property::BOOLEAN.
     * @see TextVisual::SetCutoutEnabled(), TextVisual::IsCutoutEnabled().
     */
    CUTOUT_ENABLED,

    /**
     * @brief The shadow parameters.
     * @details Name "shadow", type Property::MAP.
     * @see TextVisual::SetShadow(), TextVisual::GetShadow().
     */
    SHADOW,

    /**
     * @brief The underline parameters.
     * @details Name "underline", type Property::MAP.
     * @see TextVisual::SetUnderline(), TextVisual::GetUnderline().
     */
    UNDERLINE,

    /**
     * @brief The outline parameters.
     * @details Name "outline", type Property::MAP.
     * @see TextVisual::SetOutline(), TextVisual::GetOutline().
     */
    OUTLINE,

    /**
     * @brief The line-through parameters.
     * @details Name "lineThrough", type Property::MAP.
     * @see TextVisual::SetLineThrough(), TextVisual::GetLineThrough().
     */
    LINE_THROUGH,

    /**
     * @brief The background parameters.
     * @details Name "textBackground", type Property::MAP.
     * @see TextVisual::SetBackground(), TextVisual::GetBackground().
     */
    BACKGROUND,
  };

}; // struct TextVisualPropertyIndex

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_VISUAL_PROPERTIES_H
