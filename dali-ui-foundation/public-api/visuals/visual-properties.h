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

namespace DALI_NAMESPACE
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

} // namespace Transform

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
 * @brief Enumeration for the layer that a visual is drawn in when attached to a View.
 *
 * A View draws its own visuals in a fixed order: background effect, background, content,
 * decoration, then foreground effect. Each value selects one of those layers. A visual attached to
 * a layer is drawn above the View's own visual of that layer and below the next layer.
 *
 * @note Sibling order reorders visuals sharing the same DepthLayer. A visual can never be drawn
 * outside of the layer it is attached to.
 */
enum class DepthLayer
{
  BACKGROUND_EFFECT, ///< The background effect layer. The bottom-most layer.
  BACKGROUND,        ///< The background layer.
  CONTENT,           ///< The content layer.
  DECORATION,        ///< The decoration layer.
  FOREGROUND_EFFECT, ///< The foreground effect layer. The top-most layer.

  MAX_COUNT, ///< Sentinel value used to validate depth layer values. Not a layer itself.

  NONE = -1, ///< The visual is not attached to any View.
};

} // namespace Visual

/**
 * @}
 */

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_VISUAL_PROPERTIES_H
