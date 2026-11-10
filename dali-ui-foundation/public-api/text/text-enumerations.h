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
#include <dali/public-api/common/dali-namespace.h>
#include <cstdint>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace Text
{
/**
 * @brief Enumeration for text alignment options.
 * @SINCE_3_0.0
 */
enum class Alignment : uint8_t
{
  /**
   * @brief Align to the start (left/top)
   * @SINCE_3_0.0
   */
  START = 0,
  /**
   * @brief Align to the center
   * @SINCE_3_0.0
   */
  CENTER = 1,
  /**
   * @brief Align to the end (right/bottom)
   * @SINCE_3_0.0
   */
  END = 2
};

/**
 * @brief Enumeration for line wrapping strategies.
 *
 * Specifies how text is wrapped when the available layout width
 * is insufficient to display the entire text on a single line.
 * @SINCE_3_0.0
 */
enum class LineWrapMode : uint8_t
{
  /**
   * @brief Wraps at word boundaries.
   * @SINCE_3_0.0
   */
  WORD = 0,
  /**
   * @brief Wraps at individual characters.
   * @SINCE_3_0.0
   */
  CHARACTER = 1,
  /**
   * @brief Wraps using hyphenation when possible.
   * @SINCE_3_0.0
   */
  HYPHENATION = 2,
  /**
   * @brief Tries WORD wrapping first, then HYPHENATION, and falls back to CHARACTER.
   * @SINCE_3_0.0
   */
  MIXED = 3
};

/**
 * @brief Enumeration for determining how the text layout direction is resolved.
 * @SINCE_3_0.0
 */
enum class LayoutDirectionMode : uint8_t
{
  /**
   * @brief Determines the layout direction from the text content itself.
   * @SINCE_3_0.0
   */
  CONTENTS = 0,
  /**
   * @brief Inherits the layout direction from the parent.
   * @SINCE_3_0.0
   */
  INHERIT = 1,
  /**
   * @brief Determines the layout direction based on the system locale.
   * @SINCE_3_0.0
   */
  LOCALE = 2
};

/**
 * @brief Enumeration for determining how the line height is interpreted.
 * @SINCE_3_0.0
 */
enum class LineHeightMode : uint8_t
{
  /**
   * @brief The line height is calculated relative to the font size.
   * The specified value is treated as a multiplier of the font pixel size.
   * @SINCE_3_0.0
   */
  RELATIVE = 0,
  /**
   * @brief The line height is specified as an absolute pixel value.
   * The specified value is treated as the exact line height in pixels.
   * @SINCE_3_0.0
   */
  ABSOLUTE = 1
};

/**
 * @brief Special value for automatic line height.
 *
 * When used with SetLineHeight(), it uses the natural line height
 * derived from font metrics.
 * @SINCE_3_0.0
 */
constexpr float LINE_HEIGHT_AUTO = -1.0f;

/**
 * @brief Special value for unlimited text layout lines.
 *
 * When used with Label::SetMaximumLines(), no maximum line-count constraint is applied.
 * @SINCE_3_0.0
 */
constexpr int MAXIMUM_LINES_UNLIMITED = 0;

/**
 * @brief Special value for infinite marquee looping.
 *
 * When used with SetMarqueeLoopCount(), the marquee repeats indefinitely.
 * @SINCE_3_0.0
 */
constexpr int MARQUEE_LOOP_COUNT_INFINITE = 0;

/**
 * @brief Enumeration for defining when the marquee animation is triggered.
 * Specifies the condition under which the marquee starts.
 * @SINCE_3_0.0
 */
enum class MarqueeTriggerPolicy : uint8_t
{
  /**
   * @brief Starts the marquee only when explicitly requested.
   * The animation is triggered by calling StartMarquee().
   * @SINCE_3_0.0
   */
  MANUAL = 0,
  /**
   * @brief Starts the marquee automatically when the text overflows.
   * The animation is triggered during layout if the content exceeds
   * the available space.
   * @SINCE_3_0.0
   */
  ON_OVERFLOW = 1
};

/**
 * @brief Enumeration for selecting the orientation of the marquee animation.
 * Defines whether the marquee scrolls horizontally or vertically.
 * @SINCE_3_0.0
 */
enum class MarqueeOrientation : uint8_t
{
  /**
   * @brief Scrolls horizontally.
   * The effective scroll direction is determined by the layout direction
   * and the text direction (e.g., left-to-right or right-to-left).
   * @SINCE_3_0.0
   */
  HORIZONTAL = 0,
  /**
   * @brief Scrolls vertically.
   * The content scrolls from bottom to top.
   * @SINCE_3_0.0
   */
  VERTICAL = 1
};

/**
 * @brief Enumeration for defining how the marquee stops.
 * Specifies the behavior when a stop request is issued.
 * @SINCE_3_0.0
 */
enum class MarqueeStopMode : uint8_t
{
  /**
   * @brief Stops the marquee immediately.
   * The current scrolling position is not preserved.
   * @SINCE_3_0.0
   */
  IMMEDIATE = 0,
  /**
   * @brief Stops the marquee after the current loop finishes.
   * The marquee continues until the current cycle completes,
   * then stops at the loop boundary.
   * @SINCE_3_0.0
   */
  FINISH_LOOP = 1
};

/**
 * @brief Enumeration for text overflow handling.
 * Specifies how text is rendered when it exceeds the available layout bounds.
 * @SINCE_3_0.0
 */
enum class OverflowMode : uint8_t
{
  /**
   * @brief Clips the overflowing text.
   * Text outside the layout bounds is not rendered.
   * @SINCE_3_0.0
   */
  CLIP = 0,
  /**
   * @brief Truncates the text with an ellipsis.
   * Displays "..." at the end of the visible text.
   * @SINCE_3_0.0
   */
  ELLIPSIS = 1
};

/**
 * @brief Enumeration for password text display modes.
 * Specifies how input text is displayed when password-style masking is used.
 * @SINCE_3_0.0
 */
enum class PasswordMode : uint8_t
{
  /**
   * @brief Displays the input text without password masking.
   * @SINCE_3_0.0
   */
  NONE = 0,

  /**
   * @brief Masks all input text.
   * Each input character is displayed using the password mask character.
   * @SINCE_3_0.0
   */
  HIDE_ALL = 1,

  /**
   * @brief Temporarily reveals the last entered character before masking it.
   * The character remains visible for the password reveal duration,
   * then is displayed using the password mask character.
   * @SINCE_3_0.0
   */
  REVEAL_LAST_CHARACTER = 2
};

/**
 * @brief Enumeration for selecting the bounds used to evaluate text gradient coordinates.
 *
 * The selected bounds define the target rectangle for text view TextGradient rendering.
 * Gradient::Units then defines whether coordinates inside that rectangle are
 * normalized or pixel-based.
 * @SINCE_3_0.0
 */
enum class GradientBoundsMode : uint8_t
{
  /**
   * @brief Use laid-out text content bounds.
   *
   * This is the default mode. The gradient belongs to the laid-out content, so
   * scrolling content keeps the same position inside the gradient. Repeated
   * marquee copies repeat the same content-relative gradient.
   * @SINCE_3_0.0
   */
  CONTENT_BOUND = 0,

  /**
   * @brief Use the text view bounds.
   *
   * The full text view size is used, including padding. The gradient remains
   * fixed to the view while text content scrolls through it.
   * @SINCE_3_0.0
   */
  VIEW_BOUND = 1
};

/**
 * @brief Enumeration for selecting how a text gradient overlay is applied to
 * the resolved text glyph fill color.
 *
 * The overlay gradient is treated as the source, and the resolved text glyph
 * fill is treated as the destination. The mode is evaluated inside the text
 * shader for visible glyph fill pixels. It does not change framebuffer blending
 * state, and the resolved glyph alpha is preserved.
 *
 * Text decorations such as shadow, underline, strikethrough, outline, and
 * background are not affected.
 * @SINCE_3_0.0
 */
enum class GradientOverlayMode : uint8_t
{
  /**
   * @brief Draw the overlay gradient source over the resolved text glyph fill.
   *
   * Transparent overlay stops leave the glyph fill unchanged. Opaque overlay
   * stops replace the glyph fill color at that position. The resolved glyph
   * alpha is preserved.
   * @SINCE_3_0.0
   */
  SRC_OVER = 0,

  /**
   * @brief Screen the overlay gradient source with the resolved text glyph fill.
   *
   * This generally lightens the glyph fill. Black overlay stops leave the glyph
   * fill unchanged, while white overlay stops produce white. The resolved glyph
   * alpha is preserved.
   * @SINCE_3_0.0
   */
  SCREEN = 1
};

/**
 * @brief Enumeration for font weight.
 * @SINCE_3_0.0
 */
enum class FontWeight : uint8_t
{
  /**
   * @brief Thin font weight. Equivalent to weight 100.
   * @SINCE_3_0.0
   */
  THIN = 0,
  /**
   * @brief Extra light font weight. Equivalent to weight 200.
   * @SINCE_3_0.0
   */
  EXTRA_LIGHT = 1,
  /**
   * @brief Ultra light font weight. Alias of EXTRA_LIGHT.
   * @SINCE_3_0.0
   */
  ULTRA_LIGHT = EXTRA_LIGHT,
  /**
   * @brief Light font weight. Equivalent to weight 300.
   * @SINCE_3_0.0
   */
  LIGHT = 2,
  /**
   * @brief Demi light font weight.
   * @SINCE_3_0.0
   */
  DEMI_LIGHT = 3,
  /**
   * @brief Semi light font weight. Alias of DEMI_LIGHT.
   * @SINCE_3_0.0
   */
  SEMI_LIGHT = DEMI_LIGHT,
  /**
   * @brief Book font weight.
   * @SINCE_3_0.0
   */
  BOOK = 4,
  /**
   * @brief Normal font weight. Equivalent to weight 400.
   * @SINCE_3_0.0
   */
  NORMAL = 5,
  /**
   * @brief Regular font weight. Alias of NORMAL.
   * @SINCE_3_0.0
   */
  REGULAR = NORMAL,
  /**
   * @brief Medium font weight. Equivalent to weight 500.
   * @SINCE_3_0.0
   */
  MEDIUM = 6,
  /**
   * @brief Semi bold font weight. Equivalent to weight 600.
   * @SINCE_3_0.0
   */
  SEMI_BOLD = 7,
  /**
   * @brief Demi bold font weight. Alias of SEMI_BOLD.
   * @SINCE_3_0.0
   */
  DEMI_BOLD = SEMI_BOLD,
  /**
   * @brief Bold font weight. Equivalent to weight 700.
   * @SINCE_3_0.0
   */
  BOLD = 8,
  /**
   * @brief Extra bold font weight. Equivalent to weight 800.
   * @SINCE_3_0.0
   */
  EXTRA_BOLD = 9,
  /**
   * @brief Ultra bold font weight. Alias of EXTRA_BOLD.
   * @SINCE_3_0.0
   */
  ULTRA_BOLD = EXTRA_BOLD,
  /**
   * @brief Black font weight. Equivalent to weight 900.
   * @SINCE_3_0.0
   */
  BLACK = 10,
  /**
   * @brief Heavy font weight. Alias of BLACK.
   * @SINCE_3_0.0
   */
  HEAVY = BLACK
};

/**
 * @brief Enumeration for font width.
 * @SINCE_3_0.0
 */
enum class FontWidth : uint8_t
{
  /**
   * @brief Ultra condensed font width.
   * @SINCE_3_0.0
   */
  ULTRA_CONDENSED = 0,
  /**
   * @brief Extra condensed font width.
   * @SINCE_3_0.0
   */
  EXTRA_CONDENSED = 1,
  /**
   * @brief Condensed font width.
   * @SINCE_3_0.0
   */
  CONDENSED = 2,
  /**
   * @brief Semi condensed font width.
   * @SINCE_3_0.0
   */
  SEMI_CONDENSED = 3,
  /**
   * @brief Normal font width.
   * @SINCE_3_0.0
   */
  NORMAL = 4,
  /**
   * @brief Semi expanded font width.
   * @SINCE_3_0.0
   */
  SEMI_EXPANDED = 5,
  /**
   * @brief Expanded font width.
   * @SINCE_3_0.0
   */
  EXPANDED = 6,
  /**
   * @brief Extra expanded font width.
   * @SINCE_3_0.0
   */
  EXTRA_EXPANDED = 7,
  /**
   * @brief Ultra expanded font width.
   * @SINCE_3_0.0
   */
  ULTRA_EXPANDED = 8
};

/**
 * @brief Enumeration for font slant.
 * @SINCE_3_0.0
 */
enum class FontSlant : uint8_t
{
  /**
   * @brief Normal upright font style.
   * @SINCE_3_0.0
   */
  NORMAL = 0,
  /**
   * @brief Roman upright font style. Alias of NORMAL.
   * @SINCE_3_0.0
   */
  ROMAN = NORMAL,
  /**
   * @brief Italic font style.
   * @SINCE_3_0.0
   */
  ITALIC = 1,
  /**
   * @brief Oblique font style.
   * @SINCE_3_0.0
   */
  OBLIQUE = 2
};

/**
 * @brief Provides mask values for typing style change notifications.
 *
 * The mask is used by TypingStyleChangedSignal to indicate which
 * typing style attributes have changed at the current cursor position or
 * selected text range.
 * @SINCE_3_0.0
 */
namespace TypingStyle
{
enum Mask
{
  /**
   * @brief No typing style has changed.
   * @SINCE_3_0.0
   */
  NONE = 0x0000,
  /**
   * @brief The typing text color has changed.
   * @SINCE_3_0.0
   */
  TEXT_COLOR = 0x0001,
  /**
   * @brief The typing font family has changed.
   * @SINCE_3_0.0
   */
  FONT_FAMILY = 0x0002,
  /**
   * @brief The typing font size has changed.
   * @SINCE_3_0.0
   */
  FONT_SIZE = 0x0004,
  /**
   * @brief The typing font weight has changed.
   * @SINCE_3_0.0
   */
  FONT_WEIGHT = 0x0008,
  /**
   * @brief The typing font width has changed.
   * @SINCE_3_0.0
   */
  FONT_WIDTH = 0x0010,
  /**
   * @brief The typing font slant has changed.
   * @SINCE_3_0.0
   */
  FONT_SLANT = 0x0020
};
}

} // namespace Text

} // namespace Ui

} //namespace DALI_NAMESPACE
