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

namespace Dali
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
  TRANSFORM = Dali::Ui::VisualBasePropertyIndex::TRANSFORM,
  SHADER    = Dali::Ui::VisualBasePropertyIndex::SHADER,
  MIX_COLOR = Dali::Ui::VisualBasePropertyIndex::MIX_COLOR,
  OPACITY   = Dali::Ui::VisualBasePropertyIndex::OPACITY,
  TYPE      = Dali::Ui::VisualBasePropertyIndex::TYPE,

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
 * @brief Visual Transform Property.
 */
namespace Property
{
/**
 * @brief Enumeration for the type of Transform Property.
 */
enum Type
{
  SIZE_POLICY = Dali::Ui::Visual::Transform::Property::SIZE_POLICY,

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
 * @brief Internal enumeration for the container range type.
 *
 * This enum be used when internal view want to use VisualBase class.
 *
 * @note This enum is for internal use only.
 */
enum class InternalContainerRangeType
{
  UNDER_BACKGROUND_EFFECT                  = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::UNDER_BACKGROUND_EFFECT),
  BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_EFFECT_AND_BACKGROUND),
  BETWEEN_BACKGROUND_AND_CONTENT           = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT),
  BETWEEN_CONTENT_AND_DECORATION           = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::BETWEEN_CONTENT_AND_DECORATION),
  BETWEEN_DECORATION_AND_FOREGROUND_EFFECT = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::BETWEEN_DECORATION_AND_FOREGROUND_EFFECT),
  OVER_FOREGROUND_EFFECT                   = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT),

  BACKGROUND_EFFECT = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::MAX_COUNT), ///< Matches Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND_EFFECT
  BACKGROUND,                                                                            ///< Matches Dali::Ui::Integration::DepthIndex::Ranges::BACKGROUND
  CONTENT,                                                                               ///< Matches Dali::Ui::Integration::DepthIndex::Ranges::CONTENT
  DECORATION,                                                                            ///< Matches Dali::Ui::Integration::DepthIndex::Ranges::DECORATION
  FOREGROUND_EFFECT,                                                                     ///< Matches Dali::Ui::Integration::DepthIndex::Ranges::FOREGROUND_EFFECT

  MAX_COUNT,

  INVALID = static_cast<int>(Dali::Ui::Visual::ContainerRangeType::INVALID),
};

} // namespace Visual

} // namespace Integration
} // namespace Ui

} // namespace Dali

#endif // DALI_UI_INTEGRATION_API_VISUALS_VISUAL_PROPERTIES_INTEG_H
