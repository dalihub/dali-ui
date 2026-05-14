#ifndef DALI_UI_COLOR_VISUAL_PROPERTIES_H
#define DALI_UI_COLOR_VISUAL_PROPERTIES_H

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
#include <dali-ui-foundation/public-api/ui-property-index-ranges.h>
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
 * @brief ColorVisual Property.
 */
struct ColorVisualPropertyIndex
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
   * @brief Enumeration for the instance of properties belonging to the ColorVisual.
   */
  enum
  {
    /**
     * @brief The blur radius of the visual.
     * @details Name "blurRadius", type Property::FLOAT, animatable.
     *          If the value is 0, the edge is sharp. Otherwise, the larger the value, the more the edge is blurred.
     * @note Optional.
     * @note The default is 0.
     * @note The visual size increases by the blur radius.
     * @note If squareness is not zero, the width of blur radius might not equal with it's real value.
     */
    BLUR_RADIUS = MUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief Policy of cutout the color render result.
     * @details Name "cutoutPolicy", type Property::INTEGER.
     * @note Optional.
     * @note The default is CutoutPolicy::NONE.
     */
    CUTOUT_POLICY = IMMUTABLE_PROPERTY_START_INDEX,
  };

}; // struct ColorVisualPropertyIndex

/**
 * @brief Enumeration for cutout policy.
 */
enum class CutoutPolicy
{
  NONE,                              ///< Fully render the visual area (Default)
  CUTOUT_VIEW,                       ///< Cutout the area of the view.
  CUTOUT_VIEW_WITH_CORNER_RADIUS,    ///< Cutout the area of the view include view's corner radius.
  CUTOUT_OUTSIDE,                    ///< Cutout the outside area of the view.
  CUTOUT_OUTSIDE_WITH_CORNER_RADIUS, ///< Cutout the outside area of the view include view's corner radius.
};

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_COLOR_VISUAL_PROPERTIES_H
