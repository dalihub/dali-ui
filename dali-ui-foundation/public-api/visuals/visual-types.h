#ifndef DALI_UI_VISUAL_TYPES_H
#define DALI_UI_VISUAL_TYPES_H

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
#include <dali/public-api/common/dali-namespace.h>
#include <cstdint> ///< uint8_t

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
 *
 * Returned by VisualBase::GetVisualType() to identify which kind of visual a handle owns.
 * @SINCE_3_0.0
 */
enum class VisualType
{
  INVALID = -1, ///< Not a valid visual type. @SINCE_3_0.0

  BORDER = 0,       ///< Renders a solid color as a border inside the visual's quad. @see BorderVisual @SINCE_3_0.0
  COLOR,            ///< Renders a solid color over the visual's quad. @see ColorVisual @SINCE_3_0.0
  GRADIENT,         ///< Renders a smooth transition of colors over the visual's quad. @see GradientVisual @SINCE_3_0.0
  TEXT,             ///< Renders text. @see TextVisual @SINCE_3_0.0
  IMAGE,            ///< Renders an image. @see ImageVisual @SINCE_3_0.0
  ANIMATED_IMAGE,   ///< Renders an animated image or a sequence of images. @see AnimatedImageVisual @SINCE_3_0.0
  LOTTIE_ANIMATION, ///< Renders a Lottie animation. @see LottieAnimationVisual @SINCE_3_0.0
};

/**
 * @brief The point on the attached View that a visual is positioned from.
 *
 * The point is absolute and is never mirrored: LEFT is always the left edge, whatever the effective
 * layout direction.
 *
 * @SINCE_3_0.0
 * @see VisualBase::SetOrigin()
 * @see VisualPivot
 */
enum class VisualOrigin : uint8_t
{
  TOP_LEFT = 0,  ///< The top-left corner of the View. @SINCE_3_0.0
  TOP_CENTER,    ///< The midpoint of the View's top edge. @SINCE_3_0.0
  TOP_RIGHT,     ///< The top-right corner of the View. @SINCE_3_0.0
  CENTER_LEFT,   ///< The midpoint of the View's left edge. @SINCE_3_0.0
  CENTER,        ///< The centre of the View. @SINCE_3_0.0
  CENTER_RIGHT,  ///< The midpoint of the View's right edge. @SINCE_3_0.0
  BOTTOM_LEFT,   ///< The bottom-left corner of the View. @SINCE_3_0.0
  BOTTOM_CENTER, ///< The midpoint of the View's bottom edge. @SINCE_3_0.0
  BOTTOM_RIGHT   ///< The bottom-right corner of the View. @SINCE_3_0.0
};

/**
 * @brief The point on the visual itself that is placed at the origin.
 *
 * Like VisualOrigin, the point is absolute and is never mirrored.
 *
 * @SINCE_3_0.0
 * @see VisualBase::SetPivot()
 * @see VisualOrigin
 */
enum class VisualPivot : uint8_t
{
  TOP_LEFT = 0,  ///< The top-left corner of the visual. @SINCE_3_0.0
  TOP_CENTER,    ///< The midpoint of the visual's top edge. @SINCE_3_0.0
  TOP_RIGHT,     ///< The top-right corner of the visual. @SINCE_3_0.0
  CENTER_LEFT,   ///< The midpoint of the visual's left edge. @SINCE_3_0.0
  CENTER,        ///< The centre of the visual. @SINCE_3_0.0
  CENTER_RIGHT,  ///< The midpoint of the visual's right edge. @SINCE_3_0.0
  BOTTOM_LEFT,   ///< The bottom-left corner of the visual. @SINCE_3_0.0
  BOTTOM_CENTER, ///< The midpoint of the visual's bottom edge. @SINCE_3_0.0
  BOTTOM_RIGHT   ///< The bottom-right corner of the visual. @SINCE_3_0.0
};

namespace Visual
{
/**
 * @brief Controls whether the offset and size of a visual are proportional to
 * the attached View's size, or absolute.
 * @SINCE_3_0.0
 */
namespace Transform
{
/**
 * @brief Selects which of a visual's transform values are proportional to the attached View's size.
 *
 * A value whose flag is set is a fraction of the attached View's size, where 1.0f is the full
 * size. A value whose flag is unset is an absolute length in pixels.
 *
 * The default is ALL, so every offset and size value is proportional unless changed.
 *
 * @SINCE_3_0.0
 * @see VisualBase::SetTransformProportionFlags()
 */
enum class ProportionFlags : uint8_t
{
  /**
   * @brief Every offset and size value is an absolute length in pixels.
   * @SINCE_3_0.0
   */
  NONE = 0x0,

  /**
   * @brief Offset X is a fraction of the attached View's width; without this flag it is absolute.
   * @SINCE_3_0.0
   */
  X_PROPORTIONAL = 0x1,

  /**
   * @brief Offset Y is a fraction of the attached View's height; without this flag it is absolute.
   * @SINCE_3_0.0
   */
  Y_PROPORTIONAL = 0x2,

  /**
   * @brief Both offset values are fractions of the attached View's size.
   * @SINCE_3_0.0
   */
  OFFSET_PROPORTIONAL = X_PROPORTIONAL | Y_PROPORTIONAL,

  /**
   * @brief Width is a fraction of the attached View's width; without this flag it is absolute.
   * @SINCE_3_0.0
   */
  WIDTH_PROPORTIONAL = 0x4,

  /**
   * @brief Height is a fraction of the attached View's height; without this flag it is absolute.
   * @SINCE_3_0.0
   */
  HEIGHT_PROPORTIONAL = 0x8,

  /**
   * @brief Both size values are fractions of the attached View's size.
   * @SINCE_3_0.0
   */
  SIZE_PROPORTIONAL = WIDTH_PROPORTIONAL | HEIGHT_PROPORTIONAL,

  /**
   * @brief Every offset and size value is a fraction of the attached View's size. (Default)
   * @SINCE_3_0.0
   */
  ALL = OFFSET_PROPORTIONAL | SIZE_PROPORTIONAL
};

/**
 * @brief Bitwise OR operator for ProportionFlags.
 * @SINCE_3_0.0
 */
inline ProportionFlags operator|(ProportionFlags lhs, ProportionFlags rhs)
{
  return static_cast<ProportionFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise AND operator for ProportionFlags.
 * @SINCE_3_0.0
 */
inline ProportionFlags operator&(ProportionFlags lhs, ProportionFlags rhs)
{
  return static_cast<ProportionFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

} // namespace Transform

/**
 * @brief Status of resource which is used for visual.
 * @SINCE_3_0.0
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
 * @SINCE_3_0.0
 * @note Sibling order reorders visuals sharing the same DepthLayer. A visual can never be drawn
 * outside of the layer it is attached to.
 */
enum class DepthLayer
{
  BACKGROUND_EFFECT, ///< The background effect layer. The bottom-most layer. @SINCE_3_0.0
  BACKGROUND,        ///< The background layer. @SINCE_3_0.0
  CONTENT,           ///< The content layer. @SINCE_3_0.0
  DECORATION,        ///< The decoration layer. @SINCE_3_0.0
  FOREGROUND_EFFECT, ///< The foreground effect layer. The top-most layer. @SINCE_3_0.0

  MAX_COUNT, ///< Sentinel value used to validate depth layer values. Not a layer itself. @SINCE_3_0.0

  NONE = -1, ///< The visual is not attached to any View. @SINCE_3_0.0
};

} // namespace Visual

/**
 * @}
 */

} // namespace Ui

} //namespace DALI_NAMESPACE

#endif // DALI_UI_VISUAL_TYPES_H
