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
struct LabelPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = PROPERTY_REGISTRATION_START_INDEX,
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000, ///< Reserve property indices.

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX   = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000 ///< Reserve animatable property indices.
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the Label class.
   */
  enum
  {
    ///////////////////////////////////////////////////////////////////////////////
    // Event side (non-animatable) properties
    ///////////////////////////////////////////////////////////////////////////////
    /**
     * @brief The text to display in UTF-8 format.
     * @details Name "text", type Property::STRING.
     * @note See also: Label::SetText(), Label::GetText().
     */
    TEXT = PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @note See also: Label::SetFontFamily(), Label::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @note See also: Label::SetFontSize(), Label::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The single-line or multi-line layout option.
     * @details Name "multiLine", type Property::BOOLEAN.
     * @note See also: Label::SetMultiLine(), Label::IsMultiLine().
     */
    MULTI_LINE,

    /**
     * @brief Line wrap mode when text lines are greater than the layout width.
     * @details Name "lineWrapMode", type Text::LineWrapMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LineWrapMode (Property::INTEGER).
     * @note See also: Label::SetLineWrapMode(), Label::GetLineWrapMode().
     */
    LINE_WRAP_MODE,

    /**
     * @brief The horizontal alignment.
     * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @note See also: Label::SetHorizontalTextAlignment(), Label::GetHorizontalTextAlignment().
     */
    HORIZONTAL_ALIGNMENT,

    /**
     * @brief The vertical alignment.
     * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @note See also: Label::SetVerticalTextAlignment(), Label::GetVerticalTextAlignment().
     */
    VERTICAL_ALIGNMENT,

    /**
     * @brief The overflow mode.
     * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::OverflowMode (Property::INTEGER).
     * @note See also: Label::SetOverflowMode(), Label::GetOverflowMode().
     */
    OVERFLOW_MODE,

    /**
     * @brief The line height.
     * @details Name "lineHeight", type Property::FLOAT.
     * @note See also: Label::SetLineHeight(), Label::GetLineHeight().
     */
    LINE_HEIGHT,

    /**
     * @brief The line height mode.
     * @details Name "lineHeightMode", type Text::LineHeightMode (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::LineHeightMode (Property::INTEGER).
     * @note See also: Label::SetLineHeightMode(), Label::GetLineHeightMode().
     */
    LINE_HEIGHT_MODE,

    /**
     * @brief The layout direction mode.
     * @details Name "layoutDirectionMode", type Text::LayoutDirectionMode (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::LayoutDirectionMode (Property::INTEGER).
     * @note See also: Label::SetLayoutDirectionMode(), Label::GetLayoutDirectionMode().
     */
    LAYOUT_DIRECTION_MODE,

    /**
     * @brief Whether mark-up processing is enabled for the text.
     * @details Name "markupEnabled", type Property::BOOLEAN.
     * @note See also: Label::SetMarkupEnabled(), Label::IsMarkupEnabled().
     */
    MARKUP_ENABLED,

    /**
     * @brief The color of the anchor.
     * @details Name "anchorColor", type Property::VECTOR4.
     * @note See also: Label::SetAnchorColor(), Label::GetAnchorColor().
     */
    ANCHOR_COLOR,

    /**
     * @brief The color of anchors when they are clicked.
     * @details Name "anchorClickedColor", type Property::VECTOR4.
     * @note See also: Label::SetAnchorClickedColor(), Label::GetAnchorClickedColor().
     */
    ANCHOR_CLICKED_COLOR,

    /**
     * @brief Sets the marquee speed in pixels per second.
     * @details Name "marqueeSpeed", type Property::INTEGER.
     * @note See also: Label::SetMarqueeSpeed(), Label::GetMarqueeSpeed().
     */
    MARQUEE_SPEED,

    /**
     * @brief Number of complete loops for marquee.
     * @details Name "marqueeLoopCount", type Property::INTEGER.
     * @note See also: Label::SetMarqueeLoopCount(), Label::GetMarqueeLoopCount().
     */
    MARQUEE_LOOP_COUNT,

    /**
     * @brief The amount of time to delay the start of marquee and further loops.
     * @details Name "marqueeLoopDelay", type Property::FLOAT.
     * @note See also: Label::SetMarqueeLoopDelay(), Label::GetMarqueeLoopDelay().
     */
    MARQUEE_LOOP_DELAY,

    /**
     * @brief Gap before marquee wraps.
     * @details Name "marqueeGap", type Property::INTEGER.
     * @note See also: Label::SetMarqueeGap(), Label::GetMarqueeGap().
     */
    MARQUEE_GAP,

    /**
     * @brief The marquee stop behaviour.
     * @details Name "marqueeStopMode", type Text::MarqueeStopMode (Property::INTEGER) or Property::STRING.
     * @note See also: Label::SetMarqueeStopMode(), Label::GetMarqueeStopMode().
     */
    MARQUEE_STOP_MODE,

    /**
     * @brief The marquee orientation.
     * @details Name "marqueeOrientation", type Text::MarqueeOrientation (Property::INTEGER) or Property::STRING.
     * @note See also: Label::SetMarqueeOrientation(), Label::GetMarqueeOrientation().
     */
    MARQUEE_ORIENTATION,

    /**
     * @brief The font weight.
     * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @note See also: Label::SetFontWeight(), Label::GetFontWeight().
     */
    FONT_WEIGHT,

    /**
     * @brief The font width.
     * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @note See also: Label::SetFontWidth(), Label::GetFontWidth().
     */
    FONT_WIDTH,

    /**
     * @brief The font slant.
     * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @note See also: Label::SetFontSlant(), Label::GetFontSlant().
     */
    FONT_SLANT,

    ///////////////////////////////////////////////////////////////////////////////
    // Animatable Properties
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief The color of the text.
     * @details Name "textColor", type Property::VECTOR4.
     */
    TEXT_COLOR = ANIMATABLE_PROPERTY_START_INDEX,

    /**
     * @brief The red component of the text color.
     * @details Name "textColorRed", type Property::FLOAT.
     */
    TEXT_COLOR_RED,

    /**
     * @brief The green component of the text color.
     * @details Name "textColorGreen", type Property::FLOAT.
     */
    TEXT_COLOR_GREEN,

    /**
     * @brief The blue component of the text color.
     * @details Name "textColorBlue", type Property::FLOAT.
     */
    TEXT_COLOR_BLUE,

    /**
     * @brief The alpha component of the text color.
     * @details Name "textColorAlpha", type Property::FLOAT.
     */
    TEXT_COLOR_ALPHA
  };
};
} // namespace Text
} // namespace Ui
} // namespace Dali
