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
 * @brief Contains modes which specify how lines are wrapped.
 *
 * If the layout width is too short to show the full text, then a wrapping mode can be specified.
 *
 * LineWrap::WORD mode will move an entire word to the next line:
 * @code
 * +---------+
 * |HELLO    |
 * |WORLD    |
 * +---------+
 * @endcode
 *
 * LineWrap::CHARACTER mode will move character by character to the next line:
 * @code
 * +---------+
 * |HELLO WOR|
 * |LD       |
 * +---------+
 * @endcode
 *
 * @SINCE_1_2.60
 */
namespace LineWrap
{
/**
 * @brief Enumerations specifying how a line is wrapped.
 * @SINCE_1_2.60
 * @see LineWrap
 */
enum Mode
{
  WORD,     ///< @SINCE_1_2.60
  CHARACTER ///< @SINCE_1_2.60
};

} // namespace LineWrap

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
