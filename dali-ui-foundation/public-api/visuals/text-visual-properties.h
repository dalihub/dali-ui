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
namespace TextVisual
{
/**
 * @brief TextVisual Property.
 */
namespace Property
{
/**
 * @brief Enumeration for the instance of properties belonging to the TextVisual.
 */
enum
{
  /**
   * @brief The text to display in UTF-8 format.
   * @details name "text", type Property::STRING.
   */
  TEXT = VISUAL_IMMUTABLE_PROPERTY_START_INDEX,

  /**
   * @brief The requested font family to use.
   * @details name "fontFamily", type Property::STRING.
   */
  FONT_FAMILY,

  /**
   * @brief The requested font style to use.
   * @details name "fontStyle", type Property::MAP.
   */
  FONT_STYLE,

  /**
   * @brief The size of font in points.
   * @details name "pointSize", type Property::FLOAT.
   */
  POINT_SIZE,

  /**
   * @brief The single-line or multi-line layout option.
   * @details name "multiLine", type Property::BOOLEAN, default false.
   */
  MULTI_LINE,

  /**
   * @brief The line horizontal alignment.
   * @details Name "horizontalAlignment", type HorizontalAlignment::Type (Property::INTEGER) or Property::STRING.
   * @note Optional. If not specified, the default is HorizontalAlignment::BEGIN
   * @note Return type is HorizontalAlignment::Type (Property::INTEGER)
   */
  HORIZONTAL_ALIGNMENT,

  /**
   * @brief The line vertical alignment.
   * @details name "verticalAlignment", VerticalAlignment::Type (Property::INTEGER) or  Property::STRING
   * @note Optional. If not specified, the default is VerticalAlignment::TOP
   * @note Return type is VerticalAlignment::Type (Property::INTEGER)`
   */
  VERTICAL_ALIGNMENT,

  /**
   * @brief The color of the text.
   * @details name "textColor", type Property::VECTOR4.
   */
  TEXT_COLOR,

  /**
   * @brief  Whether the mark-up processing is enabled.
   * @details name "enableMarkup", type Property::BOOLEAN.
   */
  ENABLE_MARKUP,

  /**
   * @brief The shadow parameters.
   * @details name "shadow", type Property::MAP.
   */
  SHADOW,

  /**
   * @brief The default underline parameters.
   * @details name "underline", type Property::MAP.
   */
  UNDERLINE,

  /**
   * @brief The default outline parameters.
   * @details name "outline", type Property::MAP.
   */
  OUTLINE,

  /**
   * @brief The default lineThrough parameters.
   * @details name "lineThrough", type Property::MAP.
   */
  LINE_THROUGH,

  /**
   * @brief The default background parameters.
   * @details name "background", type Property::MAP.
   */
  BACKGROUND
};

} // namespace Property

} // namespace TextVisual

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_TEXT_VISUAL_PROPERTIES_H
