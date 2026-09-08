#ifndef DALI_UI_INTEGRATION_API_VISUALS_VISUAL_PROPERTIES_INTEG_H
#define DALI_UI_INTEGRATION_API_VISUALS_VISUAL_PROPERTIES_INTEG_H

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
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{
/**
 * @brief All the visual types.
 */
enum class InternalVisualType
{
  INVALID = static_cast<int>(Dali::Ui::VisualType::INVALID),

  BORDER                = static_cast<int>(Dali::Ui::VisualType::BORDER),
  COLOR                 = static_cast<int>(Dali::Ui::VisualType::COLOR),
  GRADIENT              = static_cast<int>(Dali::Ui::VisualType::GRADIENT),
  TEXT                  = static_cast<int>(Dali::Ui::VisualType::TEXT),
  IMAGE                 = static_cast<int>(Dali::Ui::VisualType::IMAGE),
  ANIMATED_IMAGE        = static_cast<int>(Dali::Ui::VisualType::ANIMATED_IMAGE),
  ANIMATED_VECTOR_IMAGE = static_cast<int>(Dali::Ui::VisualType::ANIMATED_VECTOR_IMAGE),

  MESH,      ///< Renders a mesh using an "obj" file, optionally with textures provided by an "mtl" file.
  PRIMITIVE, ///< Renders a simple 3D shape, such as a cube or sphere.
  WIREFRAME, ///< Renders a simple wire-frame outlining a quad.
  N_PATCH,   ///< Renders an n-patch image.
  SVG,       ///< Renders an SVG image.
  ARC,       ///< Renders an arc.
};

namespace Visual
{
/**
 * @brief Visual Properties
 */
namespace Property
{
enum Type
{
  MUTABLE_PROPERTY_START_INDEX = Dali::Ui::VISUAL_MUTABLE_PROPERTY_START_INDEX,
  MUTABLE_PROPERTY_END_INDEX   = static_cast<int>(MUTABLE_PROPERTY_START_INDEX) + static_cast<int>(Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION) - 1, ///< Reserve property indices.

  IMMUTABLE_PROPERTY_START_INDEX = Dali::Ui::VISUAL_IMMUTABLE_PROPERTY_START_INDEX,
  IMMUTABLE_PROPERTY_END_INDEX   = static_cast<int>(IMMUTABLE_PROPERTY_START_INDEX) + static_cast<int>(Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION) - 1, ///< Reserve property indices.

  READ_ONLY_PROPERTY_START_INDEX = Dali::Ui::VISUAL_READ_ONLY_PROPERTY_START_INDEX,
  READ_ONLY_PROPERTY_END_INDEX   = static_cast<int>(READ_ONLY_PROPERTY_START_INDEX) + static_cast<int>(Dali::PropertyRanges::DEFAULT_PROPERTY_MAX_COUNT_PER_DERIVATION) - 1, ///< Reserve property indices.

  TRANSFORM = MUTABLE_PROPERTY_START_INDEX, ///< The transform of the visual. Name "transform", type Property::MAP.
  SHADER,                                   ///< The custom shader of the visual. Name "shader", type Property::MAP.
  MIX_COLOR,                                ///< The mix colour of the visual. Name "mixColor", type Property::VECTOR4, animatable.
  OPACITY,                                  ///< The opacity of the visual. Name "opacity", type Property::FLOAT, animatable.

  TYPE = READ_ONLY_PROPERTY_START_INDEX, ///< The type of the visual. Name "visualType", type Ui::VisualType.

  /**
   * @brief The radius for the rounded corners of the visual.
   * @details Name "cornerRadius", type Property::FLOAT or Property::VECTOR4, animatable
   * @note By default, it is Vector::ZERO.
   * @note Only Property::Vector4 can be animated.
   * @note Each radius will clamp internally to the half of smaller of the visual width and visual height.
   * @note Their may exist some alias when you use it as ClippingMode::CLIP_CHILDREN.
   * @note Radius value are used in clockwise order from top-left-corner to bottom-left-corner.
   *       When radius is Vector4(x, y, z, w)
   *       x    y
   *        +--+
   *        |  |
   *        +--+
   *       w    z
   */
  CORNER_RADIUS = OPACITY + 2,

  /**
   * @brief Whether the corner radius value is relative (percentage [0.0f to 0.5f] of the visual size) or absolute (in
   * world units).
   * @details Name "cornerRadiusPolicy", type Property::INTEGER.
   * @see Policy::Type
   * @note By default, it is ABSOLUTE to the visual's size.
   *       If it is RELATIVE, the corner radius value is relative to the smaller of the visual width and visual height.
   */
  CORNER_RADIUS_POLICY = OPACITY + 3,

  /**
   * @brief The width for the borderline of the visual.
   * @details Name "borderlineWidth", type Property::FLOAT, animatable
   * @note Optional. Default value is 0.0f.
   */
  BORDERLINE_WIDTH = OPACITY + 4,

  /**
   * @brief The color for the borderline of the visual.
   * @details Name "borderlineColor", type Property::VECTOR4, animatable
   * @note Default value is Color::BLACK.
   */
  BORDERLINE_COLOR = OPACITY + 5,

  /**
   * @brief The offset from the visual borderline (recommend [-1.0f to 1.0f]).
   * @details Name "borderlineOffset", type Property::FLOAT, animatable
   * @note Default value is 0.0f.
   * @note This value will clamp internally to [-1.0f to 1.0f].
   */
  BORDERLINE_OFFSET = OPACITY + 6,

  /**
   * @brief The squareness for the rounded corners of the visual.
   * @details Name "cornerSquareness", type Property::FLOAT or Property::VECTOR4, animatable
   * @note By default, it is Vector::ZERO.
   * @note Each squareness will clamp internally as [0.0 1.0].
   * @note Squareness value are used in clockwise order from top-left-corner to bottom-left-corner.
   *       When squareness is Vector4(x, y, z, w)
   *       x    y
   *        +--+
   *        |  |
   *        +--+
   *       w    z
   */
  CORNER_SQUARENESS = OPACITY + 7,
};

} // namespace Property

/**
 * @brief Visual Transform for the offset or size.
 */
namespace Transform
{
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

  /**
   * @brief Extra size value that will be added to the computed visual size.
   * @details Name "extraSize", type Vector2.
   *
   * @note It is an absolute value.
   *       The property can be used when a user want to set a visual size as a combined value of `relative` and
   * `absolute`. For example, when a user want to set a visual size to (ControlSize * 2 + 10), The transform map will
   * be, transformMap.Add( Transform::Property::SIZE, Vector2( 2.0f, 2.0f ) ) .Add( Transform::Property::SIZE_POLICY,
   * Vector2( Transform::Policy::Relative, Transform::Policy::Relative ) ) .Add( Transform::Property::EXTRA_SIZE,
   * Vector2( 10.0f, 10.0f ) );
   */
  EXTRA_SIZE = SIZE_POLICY + 1,
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

} // namespace Visual

} // namespace Integration
} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_INTEGRATION_API_VISUALS_VISUAL_PROPERTIES_INTEG_H
