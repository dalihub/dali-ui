#ifndef DALI_UI_TEXT_ENUMERATIONS_INTERNAL_H
#define DALI_UI_TEXT_ENUMERATIONS_INTERNAL_H

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

#include <cstdint>

namespace Dali
{
namespace Ui
{
namespace Text
{

namespace EllipsisPosition
{
/**
 * @brief Enumerations specifying where to position the ellipsis glyph.
 * @see EllipsisPosition
 */
enum Type
{
  END = 0, ///< END position will truncate tail of text that exceeds the layout size then replace it by Ellipsis glyph
           ///< in the end of the last appeared line.
  START,   ///< START position will truncate head of text that exceeds the layout size then replace it by Ellipsis glyph
           ///< in the start of the first appeared line. In multilines, the lines are removed from top until the text fit
           ///< into layout height.
  MIDDLE   ///< MIDDLE position will truncate middle of text that exceeds the layout size then replace it by Ellipsis
           ///< glyph in the middle of line. In multilines, the lines are removed from middle until the text fit into
           ///< layout height then add ellipsis glyph in the end of the last line appeared before removed lines.
};

} // namespace EllipsisPosition

namespace Ellipsize
{
/**
 * @brief Enumerations specifying ellipsize mode.
 * @see EllipsisMode.
 */
enum Mode
{
  TRUNCATE = 0, ///< If the text exceeds the layout, it will be truncated with an ellipsis.
  AUTO_SCROLL   ///< If the text exceeds the layout, it will be auto scroll animated.
};

} // namespace Ellipsize

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

} // namespace Text

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_ENUMERATIONS_INTERNAL_H
