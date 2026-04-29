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
#include <cstdint>

namespace Dali
{

namespace Ui
{

namespace Text
{
/**
 * @brief Enumeration for text alignment options.
 */
enum class Alignment : uint8_t
{
  /**
   * @brief Align to the start (left/top)
   */
  START = 0,
  /**
   * @brief Align to the center
   */
  CENTER = 1,
  /**
   * @brief Align to the end (right/bottom)
   */
  END = 2
};

/**
 * @brief Enumeration for line wrapping strategies.
 *
 * Specifies how text is wrapped when the available layout width
 * is insufficient to display the entire text on a single line.
 */
enum class LineWrapMode : uint8_t
{
  /**
   * @brief Wraps at word boundaries.
   */
  WORD = 0,
  /**
   * @brief Wraps at individual characters.
   */
  CHARACTER = 1,
  /**
   * @brief Wraps using hyphenation when possible.
   */
  HYPHENATION = 2,
  /**
   * @brief Tries WORD wrapping first, then HYPHENATION, and falls back to CHARACTER.
   */
  MIXED = 3
};

/**
 * @brief Enumeration for determining how the text layout direction is resolved.
 */
enum class LayoutDirectionMode : uint8_t
{
  /**
   * @brief Determines the layout direction from the text content itself.
   */
  CONTENTS = 0,
  /**
   * @brief Inherits the layout direction from the parent.
   */
  INHERIT = 1,
  /**
   * @brief Determines the layout direction based on the system locale.
   */
  LOCALE = 2
};

/**
 * @brief Enumeration for determining how the line height is interpreted.
 */
enum class LineHeightMode : uint8_t
{
  /**
   * @brief The line height is calculated relative to the font size.
   * The specified value is treated as a multiplier of the font pixel size.
   */
  RELATIVE = 0,
  /**
   * @brief The line height is specified as an absolute pixel value.
   * The specified value is treated as the exact line height in pixels.
   */
  ABSOLUTE = 1
};

/**
 * @brief Special value for automatic line height.
 *
 * When used with SetLineHeight(), it uses the natural line height
 * derived from font metrics.
 */
constexpr float LINE_HEIGHT_AUTO = -1.0f;

/**
 * @brief Enumeration for defining when the marquee animation is triggered.
 * Specifies the condition under which the marquee starts.
 */
enum class MarqueeTriggerPolicy : uint8_t
{
  /**
   * @brief Starts the marquee only when explicitly requested.
   * The animation is triggered by calling StartMarquee().
   */
  MANUAL = 0,
  /**
   * @brief Starts the marquee automatically when the text overflows.
   * The animation is triggered during layout if the content exceeds
   * the available space.
   */
  ON_OVERFLOW = 1
};

/**
 * @brief Enumeration for selecting the orientation of the marquee animation.
 * Defines whether the marquee scrolls horizontally or vertically.
 */
enum class MarqueeOrientation : uint8_t
{
  /**
   * @brief Scrolls horizontally.
   * The effective scroll direction is determined by the layout direction
   * and the text direction (e.g., left-to-right or right-to-left).
   */
  HORIZONTAL = 0,
  /**
   * @brief Scrolls vertically.
   * The content scrolls from bottom to top.
   */
  VERTICAL = 1
};

/**
 * @brief Enumeration for defining how the marquee stops.
 * Specifies the behavior when a stop request is issued.
 */
enum class MarqueeStopMode : uint8_t
{
  /**
   * @brief Stops the marquee immediately.
   * The current scrolling position is not preserved.
   */
  IMMEDIATE = 0,
  /**
   * @brief Stops the marquee after the current loop finishes.
   * The marquee continues until the current cycle completes,
   * then stops at the loop boundary.
   */
  FINISH_LOOP = 1
};

/**
 * @brief Enumeration for text overflow handling.
 * Specifies how text is rendered when it exceeds the available layout bounds.
 */
enum class OverflowMode : uint8_t
{
  /**
   * @brief Clips the overflowing text.
   * Text outside the layout bounds is not rendered.
   */
  CLIP = 0,
  /**
   * @brief Truncates the text with an ellipsis.
   * Displays "..." at the end of the visible text.
   */
  ELLIPSIS = 1
};

/**
 * @brief Enumeration for password text display modes.
 * Specifies how input text is displayed when password-style masking is used.
 */
enum class PasswordMode : uint8_t
{
  /**
   * @brief Displays the input text without password masking.
   */
  NONE = 0,

  /**
   * @brief Masks all input text.
   * Each input character is displayed using the password mask character.
   */
  HIDE_ALL = 1,

  /**
   * @brief Temporarily reveals the last entered character before masking it.
   * The character remains visible for the password reveal duration,
   * then is displayed using the password mask character.
   */
  REVEAL_LAST_CHARACTER = 2
};

/**
 * @brief Enumeration for font weight.
 */
enum class FontWeight : uint8_t
{
  /**
   * @brief Thin font weight. Equivalent to weight 100.
   */
  THIN = 0,
  /**
   * @brief Extra light font weight. Equivalent to weight 200.
   */
  EXTRA_LIGHT = 1,
  /**
   * @brief Ultra light font weight. Alias of EXTRA_LIGHT.
   */
  ULTRA_LIGHT = EXTRA_LIGHT,
  /**
   * @brief Light font weight. Equivalent to weight 300.
   */
  LIGHT = 2,
  /**
   * @brief Demi light font weight.
   */
  DEMI_LIGHT = 3,
  /**
   * @brief Semi light font weight. Alias of DEMI_LIGHT.
   */
  SEMI_LIGHT = DEMI_LIGHT,
  /**
   * @brief Book font weight.
   */
  BOOK = 4,
  /**
   * @brief Normal font weight. Equivalent to weight 400.
   */
  NORMAL = 5,
  /**
   * @brief Regular font weight. Alias of NORMAL.
   */
  REGULAR = NORMAL,
  /**
   * @brief Medium font weight. Equivalent to weight 500.
   */
  MEDIUM = 6,
  /**
   * @brief Semi bold font weight. Equivalent to weight 600.
   */
  SEMI_BOLD = 7,
  /**
   * @brief Demi bold font weight. Alias of SEMI_BOLD.
   */
  DEMI_BOLD = SEMI_BOLD,
  /**
   * @brief Bold font weight. Equivalent to weight 700.
   */
  BOLD = 8,
  /**
   * @brief Extra bold font weight. Equivalent to weight 800.
   */
  EXTRA_BOLD = 9,
  /**
   * @brief Ultra bold font weight. Alias of EXTRA_BOLD.
   */
  ULTRA_BOLD = EXTRA_BOLD,
  /**
   * @brief Black font weight. Equivalent to weight 900.
   */
  BLACK = 10,
  /**
   * @brief Heavy font weight. Alias of BLACK.
   */
  HEAVY = BLACK
};

/**
 * @brief Enumeration for font width.
 */
enum class FontWidth : uint8_t
{
  /**
   * @brief Ultra condensed font width.
   */
  ULTRA_CONDENSED = 0,
  /**
   * @brief Extra condensed font width.
   */
  EXTRA_CONDENSED = 1,
  /**
   * @brief Condensed font width.
   */
  CONDENSED = 2,
  /**
   * @brief Semi condensed font width.
   */
  SEMI_CONDENSED = 3,
  /**
   * @brief Normal font width.
   */
  NORMAL = 4,
  /**
   * @brief Semi expanded font width.
   */
  SEMI_EXPANDED = 5,
  /**
   * @brief Expanded font width.
   */
  EXPANDED = 6,
  /**
   * @brief Extra expanded font width.
   */
  EXTRA_EXPANDED = 7,
  /**
   * @brief Ultra expanded font width.
   */
  ULTRA_EXPANDED = 8
};

/**
 * @brief Enumeration for font slant.
 */
enum class FontSlant : uint8_t
{
  /**
   * @brief Normal upright font style.
   */
  NORMAL = 0,
  /**
   * @brief Roman upright font style. Alias of NORMAL.
   */
  ROMAN = NORMAL,
  /**
   * @brief Italic font style.
   */
  ITALIC = 1,
  /**
   * @brief Oblique font style.
   */
  OBLIQUE = 2
};

} // namespace Text

} // namespace Ui

} // namespace Dali
