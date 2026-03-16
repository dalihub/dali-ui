#ifndef DALI_UI_TEXT_ENUMERATIONS_H
#define DALI_UI_TEXT_ENUMERATIONS_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 * @brief The available underline types for text.
 * @SINCE_1_2.60
 */
namespace Underline
{
/**
 * @brief Enumerations specifying the underline type.
 */
enum Type
{
  SOLID,
  DASHED,
  DOUBLE
};

} // namespace Underline

} // namespace Text

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_ENUMERATIONS_H
