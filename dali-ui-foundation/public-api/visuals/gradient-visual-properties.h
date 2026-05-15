#ifndef DALI_UI_GRADIENT_VISUAL_PROPERTIES_H
#define DALI_UI_GRADIENT_VISUAL_PROPERTIES_H

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
#include <dali-ui-foundation/public-api/gradient/gradient-enumerations.h>
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
 * @brief GradientVisual is to render a smooth transition of colors to the control's quad.
 */
struct GradientVisualPropertyIndex
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
   * @brief Enumeration for the instance of properties belonging to the GradientVisual.
   */
  enum
  {
    /**
     * @brief The offset value that shifts the starting value of offset of the gradient.
     * It will decide the offset value at START_POSITION (for Linear) or CENTER (for Radial) or START_ANGLE (or Conic)
     * @see STOP_OFFSET
     * @details Name "startOffset", type Property::FLOAT.
     * @note Optional. If not supplied, default is 0.
     */
    START_OFFSET = MUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief The start position of a linear gradient.
     * @details Name "startPosition", type Property::VECTOR2.
     * Gradient is drawn in a rectangular area, depend on Units type.
     * For Units::OBJECT_BOUNDING_BOX case, TopLeft is (-0.5, -0.5) and BottomRight is (0.5, 0.5).
     * The outside of the entered Start Position and End Position is extended to the value of the border.
     * Even if the range between Start Position and End Position is greater than (-0.5, -0.5) to (0.5, 0.5),
     * only the corresponding area is drawn.
     * @note Mandatory for Linear.
     */
    START_POSITION = IMMUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief The end position of a linear gradient.
     * @details Name "endPosition", type Property::VECTOR2.
     * Gradient is drawn in a rectangular area, depend on Units type.
     * For Units::OBJECT_BOUNDING_BOX case, TopLeft is (-0.5, -0.5) and BottomRight is (0.5, 0.5).
     * The outside of the entered Start Position and End Position is extended to the value of the border.
     * Even if the range between Start Position and End Position is greater than (-0.5, -0.5) to (0.5, 0.5),
     * only the corresponding area is drawn.
     * @note Mandatory for Linear.
     */
    END_POSITION,

    /**
     * @brief The center point of a radial gradient.
     * @details Name "center", type Property::VECTOR2.
     * @note Mandatory for Radial and Conic.
     */
    CENTER,

    /**
     * @brief The size of the radius of a radial gradient.
     * @details Name "radius", type Property::FLOAT.
     * @note Mandatory for Radial.
     */
    RADIUS,

    /**
     * @brief The initial angle from which the conic gradient begins.
     * @details Name "startAngle", type Property::FLOAT.
     * @note Mandatory for Conic.
     */
    START_ANGLE,

    /**
     * @brief All the stop offsets.
     * @details Name "stopOffset", type Property::ARRAY of Property::FLOAT.
     * @note Optional. If not supplied, default is 0.0 and 1.0.
     */
    STOP_OFFSET,

    /**
     * @brief The color at the stop offsets.
     * @details Name "stopColor", type Property::ARRAY of Property::VECTOR4.
     * @note Mandatory. At least 2 values required to show a gradient.
     */
    STOP_COLOR,

    /**
     * @brief Defines the coordinate system for certain attributes of the points in a gradient.
     * @details Name "gradientUnits", type Units (Property::INTEGER) or Property::STRING.
     * @note Optional. If not supplied, default is Units::OBJECT_BOUNDING_BOX.
     * @see Units
     */
    UNITS,

    /**
     * @brief Indicates what happens if the gradient starts or ends inside the bounds of the target rectangle.
     * @details Name "spreadMethod", type SpreadMethod (Property::INTEGER) or Property::STRING.
     * @note Optional. If not supplied, default is SpreadMethod::PAD.
     * @see SpreadMethod
     */
    SPREAD_METHOD,
  };

}; // struct GradientVisualPropertyIndex

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_GRADIENT_VISUAL_PROPERTIES_H
