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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
struct TextVisualizerPropertyIndex
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
   * @brief Enumeration for the instance of properties belonging to the TextVisualizer class.
   */
  enum
  {
    /**
     * @brief The text to display in UTF-8 format.
     * @details Name "text", type Property::STRING.
     * @see TextVisualizer::SetText(), TextVisualizer::GetText().
     */
    TEXT = PROPERTY_START_INDEX,

    /**
     * @brief The font family of the text.
     * @details Name "fontFamily", type Property::STRING.
     * @see TextVisualizer::SetFontFamily(), TextVisualizer::GetFontFamily().
     */
    FONT_FAMILY,

    /**
     * @brief The size of font in pixels.
     * @details Name "fontSize", type Property::FLOAT.
     * @see TextVisualizer::SetFontSize(), TextVisualizer::GetFontSize().
     */
    FONT_SIZE,

    /**
     * @brief The color of the text.
     * @details Name "textColor", type Property::VECTOR4.
     * @see TextVisualizer::SetTextColor(), TextVisualizer::GetTextColor().
     */
    TEXT_COLOR,

    /**
     * @brief The relative line height multiplier.
     * @details Name "lineHeight", type Property::FLOAT.
     * A positive value is interpreted as a multiplier of the font pixel size.
     * A value of -1.0f uses the natural line height.
     * @see TextVisualizer::SetLineHeight(), TextVisualizer::GetLineHeight(), TextVisualizer::ClearLineHeight().
     */
    LINE_HEIGHT
  };
};

} // namespace Text
} // namespace Ui
} // namespace Dali
