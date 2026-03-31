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
 * @brief Enumeration for text layout direction.
 */
enum class Direction : uint8_t
{
  /**
   * @brief Left-to-right text direction.
   */
  LEFT_TO_RIGHT = 0,
  /**
   * @brief Right-to-left text direction.
   */
  RIGHT_TO_LEFT = 1
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

} // namespace Text

} // namespace Ui

} // namespace Dali
