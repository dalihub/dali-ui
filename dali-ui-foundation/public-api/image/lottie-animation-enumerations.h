#pragma once

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
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/image/animated-image-enumerations.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>

namespace Dali
{

namespace Ui
{

namespace LottieAnimation
{
/**
 * @brief Enumeration for what looping mode is in.
 */
enum class LoopingMode : uint8_t
{
  RESTART,     ///< When the animation arrives at the end in looping mode, the animation restarts from the beginning.
  AUTO_REVERSE ///< When the animation arrives at the end in looping mode, the animation reverses direction and runs
               ///< backwards again.
};

/**
 * @brief Enumeration for animatable vector properties used with SetDynamicProperty.
 */
enum class VectorProperty : uint8_t
{
  FILL_COLOR = 0,     ///< Fill color of a shape
  FILL_OPACITY,       ///< Fill opacity of a shape
  STROKE_COLOR,       ///< Stroke color of a shape
  STROKE_OPACITY,     ///< Stroke opacity of a shape
  STROKE_WIDTH,       ///< Stroke width of a shape
  TRANSFORM_ANCHOR,   ///< Anchor point of a layer
  TRANSFORM_POSITION, ///< Position of a layer
  TRANSFORM_SCALE,    ///< Scale of a layer
  TRANSFORM_ROTATION, ///< Rotation of a layer
  TRANSFORM_OPACITY   ///< Opacity of a layer
};

} // namespace LottieAnimation

} // namespace Ui

} // namespace Dali
