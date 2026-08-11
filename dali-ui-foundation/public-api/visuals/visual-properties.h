#ifndef DALI_UI_VISUAL_PROPERTIES_H
#define DALI_UI_VISUAL_PROPERTIES_H

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
#include <cstdint> ///< uint8_t

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/ui-property-index-ranges.h>

namespace Dali
{
namespace Ui
{
/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief Visual Property.
 */
struct VisualBasePropertyIndex
{
  /**
   * @brief Enumeration for the start and end property ranges for visual base.
   */
  enum PropertyRange
  {
    MUTABLE_PROPERTY_START_INDEX = Ui::VISUAL_MUTABLE_PROPERTY_START_INDEX,
    MUTABLE_PROPERTY_END_INDEX   = static_cast<int>(MUTABLE_PROPERTY_START_INDEX) + static_cast<int>(Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION) - 1, ///< Reserve property indices.

    IMMUTABLE_PROPERTY_START_INDEX = Ui::VISUAL_IMMUTABLE_PROPERTY_START_INDEX,
    IMMUTABLE_PROPERTY_END_INDEX   = static_cast<int>(IMMUTABLE_PROPERTY_START_INDEX) + static_cast<int>(Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION) - 1, ///< Reserve property indices.

    READ_ONLY_PROPERTY_START_INDEX = Ui::VISUAL_READ_ONLY_PROPERTY_START_INDEX,
    READ_ONLY_PROPERTY_END_INDEX   = static_cast<int>(READ_ONLY_PROPERTY_START_INDEX) + static_cast<int>(Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION) - 1, ///< Reserve property indices.
  };

  /**
   * @brief Enumeration for the instance of properties belonging to the Visual Property.
   */
  enum
  {
    /**
     * @brief The transform used by the visual.
     * @details Name "transform", type Property::MAP.
     * @note Optional.
     * @see Ui::Visual::Transform::Property
     */
    TRANSFORM = MUTABLE_PROPERTY_START_INDEX,

    /**
     * @brief The shader to use in the visual.
     * @details Name "shader", type Property::MAP.
     * @note Optional.
     * @note Will override the existing shaders.
     * @see Shader::Property
     */
    SHADER,

    /**
     * @brief Mix color is a blend color for any visual.
     * @details Name "mixColor", type Property::VECTOR3 or Property::VECTOR4, animatable
     * @note Optional
     * @note Animate support for Property::VECTOR4, which OPACITY property included.
     */
    MIX_COLOR,

    /**
     * @brief Opacity is the alpha component of the mixColor, above.
     * @details Name "opacity", type Property::FLOAT, animatable
     * @note Optional
     */
    OPACITY,

    /**
     * @brief The index for the visual type.
     * @details Name "visualType", type [Type](Dali::Ui::VisualType) (Property::INTEGER) or Property::STRING.
     * @note Mandatory.
     * @see Type
     */
    TYPE = READ_ONLY_PROPERTY_START_INDEX,
  };

}; // struct VisualBasePropertyIndex

/**
 * @brief All the visual types.
 */
enum class VisualType
{
  INVALID = -1,

  BORDER = 0,            ///< Renders a solid color as an internal border to the control's quad.
  COLOR,                 ///< Renders a solid color to the control's quad.
  GRADIENT,              ///< Renders a smooth transition of colors to the control's quad.
  TEXT,                  ///< Renders text
  IMAGE,                 ///< Renders an image into the control's quad.
  ANIMATED_IMAGE,        ///< Renders a animated image.
  ANIMATED_VECTOR_IMAGE, ///< Renders an animated vector image.
};

namespace Visual
{
/**
 * @brief Visual Transform for the offset or size.
 */
namespace Transform
{
/**
 * @brief Enumeration for transform proportional flags.
 */
enum class ProportionFlags : uint8_t
{
  /**
   * @brief No flags set
   */
  NONE = 0x0,

  /**
   * @brief Offset X is proportional to attached view size
   */
  X_PROPORTIONAL = 0x1,

  /**
   * @brief Offset Y is proportional to attached view size
   */
  Y_PROPORTIONAL = 0x2,

  /**
   * @brief Offset is proportional to attached view size
   */
  OFFSET_PROPORTIONAL = X_PROPORTIONAL | Y_PROPORTIONAL,

  /**
   * @brief Width is proportional to attached view size
   */
  WIDTH_PROPORTIONAL = 0x4,

  /**
   * @brief Height is proportional to attached view size
   */
  HEIGHT_PROPORTIONAL = 0x8,

  /**
   * @brief Size is proportional to attached view size
   */
  SIZE_PROPORTIONAL = WIDTH_PROPORTIONAL | HEIGHT_PROPORTIONAL,

  /**
   * @brief All bits set (all current and future proportional axes)
   */
  ALL = static_cast<uint8_t>(~0)
};

/**
 * @brief Bitwise OR operator for ProportionFlags.
 */
inline ProportionFlags operator|(ProportionFlags lhs, ProportionFlags rhs)
{
  return static_cast<ProportionFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise AND operator for ProportionFlags.
 */
inline ProportionFlags operator&(ProportionFlags lhs, ProportionFlags rhs)
{
  return static_cast<ProportionFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

/**
 * @brief Policies used by the transform for the offset or size.
 */
namespace Policy
{
/**
 * @brief Enumeration for the type of Transform Policy.
 */
enum Type
{
  RELATIVE = 0, ///< Relative to the control (percentage [0.0f to 1.0f] of the control).
  ABSOLUTE = 1  ///< Absolute value in world units.
};

} // namespace Policy

/**
 * @brief Visual Transform Property.
 */
namespace Property
{
/**
 * @brief Enumeration for the type of Transform Property.
 */
enum Type
{
  /**
   * @brief Offset of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in
   * world units).
   * @details Name "offset", type Property::VECTOR2, animatable.
   *
   * @see OFFSET_POLICY
   */
  OFFSET,

  /**
   * @brief Size of the visual, which can be either relative (percentage [0.0f to 1.0f] of the parent) or absolute (in
   * world units).
   * @details Name "size", type Property::VECTOR2, animatable.
   * @see SIZE_POLICY
   */
  SIZE,

  /**
   * @brief The origin of the visual within its control area.
   * @details Name "origin", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Ui::Align
   * @note The default is Align::TOP_BEGIN.
   */
  ORIGIN,

  /**
   * @brief The pivot of the visual
   * @details Name "pivot", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Ui::Align
   * @note The default is Align::TOP_BEGIN.
   */
  PIVOT,

  /**
   * @brief Whether the x or y OFFSET values are relative (percentage [0.0f to 1.0f] of the control) or absolute (in
   * world units).
   * @details Name "offsetPolicy", type Vector2 or Property::ARRAY of Property::STRING.
   *          If Property::ARRAY then 2 strings expected for the x and y.
   *
   * C++:
   * @code
   * control.SetProperty( ..., // Some visual based property
   *                      Property::Map().Add( ... ) // Properties to set up visual
   *                                     .Add( Ui::VisualBasePropertyIndex::TRANSFORM,
   *                                           Property::Array().Add(
   * Ui::Visual::Transform::Property::OFFSET_POLICY, Vector2( Policy::ABSOLUTE, Policy::RELATIVE ) ) ) .Add(
   * Ui::Visual::Transform::Property::OFFSET, Vector2( 10, 1.0f ) ) );
   * @endcode
   *
   * JSON:
   * @code
   * {
   *   ...
   *   "transition":
   *   {
   *     "offsetPolicy" : [ "ABSOLUTE", "RELATIVE" ],
   *     "offset" : [ 10, 1.0 ]
   *   }
   *   ...
   * }
   *
   * @endcode
   * @see Policy::Type
   * @note By default, both the x and the y offset is RELATIVE.
   */
  OFFSET_POLICY,

  /**
   * @brief Whether the width or height SIZE values are relative (percentage [0.0f to 1.0f] of the control) or absolute
   * (in world units).
   * @details Name "sizePolicy", type Vector2 or Property::ARRAY of Property::STRING.
   *          If Property::ARRAY then 2 strings expected for the width and height.
   *
   * @see Policy::Type
   * @see OFFSET_POLICY for example
   * @note By default, both the width and the height is RELATIVE to the control's size.
   */
  SIZE_POLICY,
};

} // namespace Property

} // namespace Transform

/**
 * @brief Shader for Visuals.
 */
namespace Shader
{
/**
 * @brief Shader Property.
 */
namespace Property
{
/**
 * @brief The type of Shader.
 */
enum
{
  /**
   * @brief The vertex shader.
   * @details Name "vertexShader", type Property::STRING or Property::ARRAY of Property::STRING.
   *          A Property::ARRAY of Property::STRING values can be used to split the shader string over multiple lines.
   * @note Optional
   * @note If not supplied, the visual's already set vertex shader is used.
   */
  VERTEX_SHADER,

  /**
   * @brief The fragment shader.
   * @details Name "fragmentShader", type Property::STRING or Property::ARRAY of Property::STRING.
   *          A Property::ARRAY of Property::STRING values can be used to split the shader string over multiple lines.
   * @note Optional
   * @note If not supplied, the visual's already set fragment shader is used.
   */
  FRAGMENT_SHADER,

  /**
   * @brief How to subdivide the grid along the X-Axis.
   * @details Name "subdivideGridX", type Property::INTEGER.
   * @note Optional
   * @note If not supplied, the default is 1.
   * @note Value should be greater than or equal to 1.
   */
  SUBDIVIDE_GRID_X,

  /**
   * @brief How to subdivide the grid along the Y-Axis.
   * @details Name "subdivideGridY", type Property::INTEGER.
   * @note Optional
   * @note If not supplied, the default is 1.
   * @note Value should be greater than or equal to 1.
   */
  SUBDIVIDE_GRID_Y,

  /**
   * @brief Render Pass key to synchonize Shader and RenderTask.
   * @details Name "renderPassTag", type Property::INTEGER.
   * @note Optional
   * @note If not supplied, the default is 0.
   * @note Value should be greater than or equal to 0.
   */
  RENDER_PASS_TAG,

  /**
   * @brief Hints for rendering.
   * @details Name "hints", type Dali::Shader::Hint (Property::INTEGER), Property::STRING or Property::ARRAY of
   * Property::STRING.
   * @note Optional
   * @note If not supplied, the default is Dali::Shader::Hint::NONE.
   */
  HINTS,

  /**
   * @brief Name for shader.
   * @details Name "name", type Property::STRING. Once set, the value should not change afterwards(for caching).
   * @note Optional
   * @note If not supplied, the default is empty string.
   */
  NAME,
};

} // namespace Property

} // namespace Shader

/**
 * @brief Status of resource which is used for visual.
 */
enum class ResourceStatus
{
  PREPARING, /// Resource is prepared.
  READY,     /// Resource is ready.
  FAILED     /// Resource is fail to load
};

/**
 * @brief Enumeration for the visual range when attached to the View.
 * It will be used when we determine the visual's depth index.
 */
enum class ContainerRangeType
{
  UNDER_BACKGROUND_EFFECT,                  ///< The visual object exist under Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT
  BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND, ///< The visual object exist between Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT and Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND
  BETWEEN_BACKGROUND_AND_CONTENT,           ///< The visual object exist between Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND and Dali::Ui::Integration::DepthIndex::Ranges::CONTENT
  BETWEEN_CONTENT_AND_DECORATION,           ///< The visual object exist between Dali::Ui::Integration::DepthIndex::Ranges::CONTENT and Dali::Ui::Integration::DepthIndex::Ranges::DECORATION
  BETWEEN_DECORATION_AND_FOREGROUND_EFFECT, ///< The visual object exist between Dali::Ui::Integration::DepthIndex::Ranges::DECORATION and Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT
  OVER_FOREGROUND_EFFECT,                   ///< The visual object exist over Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT

  MAX_COUNT,

  INVALID = -1,
};

} // namespace Visual

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_VISUAL_PROPERTIES_H
