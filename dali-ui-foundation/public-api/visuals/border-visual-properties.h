#ifndef DALI_UI_BORDER_VISUAL_PROPERTIES_H
#define DALI_UI_BORDER_VISUAL_PROPERTIES_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Ui
{
/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief BorderVisual is to render a solid color as an internal border to the control's quad.
 */
struct BorderVisualPropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for this visual.
   */
  enum PropertyRange
  {
    MUTABLE_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::MUTABLE_PROPERTY_END_INDEX + 1,
    MUTABLE_PROPERTY_END_INDEX   = MUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

    IMMUTABLE_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::IMMUTABLE_PROPERTY_END_INDEX + 1,
    IMMUTABLE_PROPERTY_END_INDEX   = IMMUTABLE_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,

    READ_ONLY_PROPERTY_START_INDEX = Ui::VisualBasePropertyIndex::READ_ONLY_PROPERTY_END_INDEX + 1,
    READ_ONLY_PROPERTY_END_INDEX   = READ_ONLY_PROPERTY_START_INDEX + Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION - 1,
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the BorderVisual.
   */
  enum
  {
    /**
     * @brief The width of the border (in pixels).
     * @details Name "borderSize", type Property::FLOAT.
     * @note Mandatory.
     */
    BORDER_SIZE = MUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief Whether anti-aliasing of the border is required.
     * @details Name "antiAliasing", type Property::BOOLEAN.
     * @note Optional. If not supplied, default is false.
     */
    ANTI_ALIASING,
  };
}; // struct BorderVisualPropertyIndex

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_BORDER_VISUAL_PROPERTIES_H
