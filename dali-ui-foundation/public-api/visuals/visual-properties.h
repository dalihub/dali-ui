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
 * @brief All the visual types.
 */
namespace Visual
{
/**
 * @brief All the visual types.
 */
enum Type
{
  BORDER,         ///< Renders a solid color as an internal border to the control's quad.
  COLOR,          ///< Renders a solid color to the control's quad.
  GRADIENT,       ///< Renders a smooth transition of colors to the control's quad.
  IMAGE,          ///< Renders an image into the control's quad.
  MESH,           ///< Renders a mesh using an "obj" file, optionally with textures provided by an "mtl" file.
  PRIMITIVE,      ///< Renders a simple 3D shape, such as a cube or sphere.
  WIREFRAME,      ///< Renders a simple wire-frame outlining a quad.
  TEXT,           ///< Renders text
  N_PATCH,        ///< Renders an n-patch image.
  SVG,            ///< Renders an SVG image.
  ANIMATED_IMAGE, ///< Renders a animated image.
};

/**
 * @brief Visual Property.
 */
namespace Property
{
/**
 * @brief Enumeration for the instance of properties belonging to the Visual Property.
 */
enum
{
  /**
   * @brief The index for the visual type.
   * @details Name "visualType", type [Type](Dali::Ui::Visual::Type) (Property::INTEGER) or Property::STRING.
   * @note Mandatory.
   * @see Type
   */
  TYPE = VISUAL_PROPERTY_BASE_START_INDEX,

  /**
   * @brief The shader to use in the visual.
   * @details Name "shader", type Property::MAP.
   * @note Optional.
   * @note Will override the existing shaders.
   * @see Shader::Property
   */
  SHADER,

  /**
   * @brief The transform used by the visual.
   * @details Name "transform", type Property::MAP.
   * @note Optional.
   * @see Ui::Visual::Transform::Property
   */
  TRANSFORM,

  /**
   * @brief Enables/disables premultiplied alpha.
   * @details Name "premultipliedAlpha", type Property::BOOLEAN.
   * @note Optional.
   * @note The premultiplied alpha is false by default unless this behaviour is modified
   * by the derived Visual type.
   */
  PREMULTIPLIED_ALPHA,

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
   * @brief The anchor-point of the visual
   * @details Name "pivot", type Align::Type (Property::INTEGER) or Property::STRING.
   * @see Ui::Align
   * @note The default is Align::TOP_BEGIN.
   */
  ANCHOR_POINT,

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
   *                                     .Add( Visual::Property::TRANSFORM,
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

} // namespace Visual

/**
 * @}
 */

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_VISUAL_PROPERTIES_H
