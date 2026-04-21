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
    PROPERTY_START_INDEX = Ui::View::VIEW_PROPERTY_END_INDEX + 1,
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
     * @see Label::SetText(), Label::GetText().
     */
    TEXT = PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @see Label::SetFontFamily(), Label::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @see Label::SetFontSize(), Label::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The single-line or multi-line layout option.
     * @details Name "multiLine", type Property::BOOLEAN.
     * @see Label::SetMultiLine(), Label::IsMultiLine().
     */
    MULTI_LINE,

    /**
     * @brief Line wrap mode when text lines are greater than the layout width.
     * @details Name "lineWrapMode", type Text::LineWrapMode (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::LineWrapMode (Property::INTEGER).
     * @see Label::SetLineWrapMode(), Label::GetLineWrapMode().
     */
    LINE_WRAP_MODE,

    /**
     * @brief The horizontal alignment.
     * @details Name "horizontalAlignment", type Text::Alignment (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see Label::SetHorizontalTextAlignment(), Label::GetHorizontalTextAlignment().
     */
    HORIZONTAL_ALIGNMENT,

    /**
     * @brief The vertical alignment.
     * @details Name "verticalAlignment", type Text::Alignment (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::Alignment (Property::INTEGER).
     * @see Label::SetVerticalTextAlignment(), Label::GetVerticalTextAlignment().
     */
    VERTICAL_ALIGNMENT,

    /**
     * @brief The overflow mode.
     * @details Name "overflowMode", type Text::OverflowMode (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::OverflowMode (Property::INTEGER).
     * @see Label::SetOverflowMode(), Label::GetOverflowMode().
     */
    OVERFLOW_MODE,

    /**
     * @brief The line height.
     * @details Name "lineHeight", type Property::FLOAT.
     * @see Label::SetLineHeight(), Label::GetLineHeight().
     */
    LINE_HEIGHT,

    /**
     * @brief The line height mode.
     * @details Name "lineHeightMode", type Text::LineHeightMode (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::LineHeightMode (Property::INTEGER).
     * @see Label::SetLineHeightMode(), Label::GetLineHeightMode().
     */
    LINE_HEIGHT_MODE,

    /**
     * @brief The layout direction mode.
     * @details Name "layoutDirectionMode", type Text::LayoutDirectionMode (Property::INTEGER) or type Property::STRING.
     * @note Return type is Text::LayoutDirectionMode (Property::INTEGER).
     * @see Label::SetLayoutDirectionMode(), Label::GetLayoutDirectionMode().
     */
    LAYOUT_DIRECTION_MODE,

    /**
     * @brief Whether mark-up processing is enabled for the text.
     * @details Name "markupEnabled", type Property::BOOLEAN.
     * @see Label::SetMarkupEnabled(), Label::IsMarkupEnabled().
     */
    MARKUP_ENABLED,

    /**
     * @brief The color of the anchor.
     * @details Name "anchorColor", type Property::VECTOR4.
     * @see Label::SetAnchorColor(), Label::GetAnchorColor().
     */
    ANCHOR_COLOR,

    /**
     * @brief The color of anchors when they are clicked.
     * @details Name "anchorClickedColor", type Property::VECTOR4.
     * @see Label::SetAnchorClickedColor(), Label::GetAnchorClickedColor().
     */
    ANCHOR_CLICKED_COLOR,

    /**
     * @brief Defines when the marquee animation is triggered.
     * @details Name "marqueeTriggerPolicy", type Text::MarqueeTriggerPolicy (Property::INTEGER) or Property::STRING.
     * @see Label::SetMarqueeTriggerPolicy(), Label::GetMarqueeTriggerPolicy().
     */
    MARQUEE_TRIGGER_POLICY,

    /**
     * @brief Sets the marquee speed in pixels per second.
     * @details Name "marqueeSpeed", type Property::INTEGER.
     * @see Label::SetMarqueeSpeed(), Label::GetMarqueeSpeed().
     */
    MARQUEE_SPEED,

    /**
     * @brief Number of complete loops for marquee.
     * @details Name "marqueeLoopCount", type Property::INTEGER.
     * @see Label::SetMarqueeLoopCount(), Label::GetMarqueeLoopCount().
     */
    MARQUEE_LOOP_COUNT,

    /**
     * @brief The amount of time to delay the start of marquee and further loops.
     * @details Name "marqueeLoopDelay", type Property::FLOAT.
     * @see Label::SetMarqueeLoopDelay(), Label::GetMarqueeLoopDelay().
     */
    MARQUEE_LOOP_DELAY,

    /**
     * @brief Gap before marquee wraps.
     * @details Name "marqueeGap", type Property::INTEGER.
     * @see Label::SetMarqueeGap(), Label::GetMarqueeGap().
     */
    MARQUEE_GAP,

    /**
     * @brief The marquee orientation.
     * @details Name "marqueeOrientation", type Text::MarqueeOrientation (Property::INTEGER) or Property::STRING.
     * @see Label::SetMarqueeOrientation(), Label::GetMarqueeOrientation().
     */
    MARQUEE_ORIENTATION,

    /**
     * @brief The marquee stop behaviour.
     * @details Name "marqueeStopMode", type Text::MarqueeStopMode (Property::INTEGER) or Property::STRING.
     * @see Label::SetMarqueeStopMode(), Label::GetMarqueeStopMode().
     */
    MARQUEE_STOP_MODE,

    /**
     * @brief The font weight.
     * @details Name "fontWeight", type Text::FontWeight (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWeight (Property::INTEGER).
     * @see Label::SetFontWeight(), Label::GetFontWeight().
     */
    FONT_WEIGHT,

    /**
     * @brief The font width.
     * @details Name "fontWidth", type Text::FontWidth (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontWidth (Property::INTEGER).
     * @see Label::SetFontWidth(), Label::GetFontWidth().
     */
    FONT_WIDTH,

    /**
     * @brief The font slant.
     * @details Name "fontSlant", type Text::FontSlant (Property::INTEGER) or Property::STRING.
     * @note Return type is Text::FontSlant (Property::INTEGER).
     * @see Label::SetFontSlant(), Label::GetFontSlant().
     */
    FONT_SLANT,

    /**
     * @brief The background color behind the text.
     * @details Name "textBackgroundColor", type Property::VECTOR4.
     * @note The background is rendered behind the text glyphs.
     * @see Label::SetTextBackgroundColor(), Label::GetTextBackgroundColor(), Label::ClearTextBackgroundColor().
     */
    TEXT_BACKGROUND_COLOR,

    /**
     * @brief The font size scale.
     * @details Name "fontSizeScale", type Property::FLOAT.
     * @see Label::SetFontSizeScale(), Label::GetFontSizeScale().
     */
    FONT_SIZE_SCALE,

    /**
     * @brief The minimum font size scale.
     * @details Name "minimumFontSizeScale", type Property::FLOAT.
     * @see Label::SetMinimumFontSizeScale(), Label::GetMinimumFontSizeScale().
     */
    MINIMUM_FONT_SIZE_SCALE,

    /**
     * @brief The maximum font size scale.
     * @details Name "maximumFontSizeScale", type Property::FLOAT.
     * @see Label::SetMaximumFontSizeScale(), Label::GetMaximumFontSizeScale().
     */
    MAXIMUM_FONT_SIZE_SCALE,

    /**
     * @brief Whether the system font size scale is applied.
     * @details Name "systemFontSizeScaleEnabled", type Property::BOOLEAN.
     * @see Label::SetSystemFontSizeScaleEnabled(), Label::IsSystemFontSizeScaleEnabled().
     */
    SYSTEM_FONT_SIZE_SCALE_ENABLED,

    /**
     * @brief Whether cutout rendering is enabled for the text.
     * @details Name "cutoutEnabled", type Property::BOOLEAN.
     * @see Label::SetCutoutEnabled(), Label::IsCutoutEnabled().
     */
    CUTOUT_ENABLED,

    /**
     * @brief Whether asynchronous text rendering is enabled.
     * @details Name "asyncRendering", type Property::BOOLEAN.
     * @see Label::SetAsyncRendering(), Label::IsAsyncRendering().
     */
    ASYNC_RENDERING,

    /**
     * @brief The render scale of the text.
     * @details Name "renderScale", type Property::FLOAT.
     * Renders text by rasterizing glyphs at a larger scale and downscaling the result.
     * This improves rendering quality when the view is visually scaled, by reducing
     * quality loss caused by texture upscaling.
     * The layout size of the view is not affected.
     * Valid only when async rendering is enabled, and the value must be 1.0f or greater.
     * @see Label::SetRenderScale(), Label::GetRenderScale().
     */
    RENDER_SCALE,

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
    TEXT_COLOR_ALPHA,

    /**
     * @brief The pixel snap factor.
     * @details Name "pixelSnapFactor", type Property::FLOAT.
     * Controls the degree of pixel snapping applied to the visual position.
     * A value of 0.0f disables snapping (original position is preserved),
     * while 1.0f applies full pixel alignment. Intermediate values blend
     * smoothly between the original and snapped positions.
     *
     * This property is typically animated to balance smooth motion and
     * crisp alignment. Use 0.0f during animations to avoid snapping artifacts,
     * and gradually increase to 1.0f as the animation settles for sharp rendering.
     *
     * The value must be in the range [0.0f, 1.0f].
     */
    PIXEL_SNAP_FACTOR
  };
};
} // namespace Text
} // namespace Ui
} // namespace Dali
