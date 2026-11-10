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

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace LottieAnimation
{
/**
 * @brief Enumeration for what looping mode is in.
 * @SINCE_3_0.0
 */
enum class LoopingMode : uint8_t
{
  RESTART,     ///< When the animation arrives at the end in looping mode, the animation restarts from the beginning. @SINCE_3_0.0
  AUTO_REVERSE ///< When the animation arrives at the end in looping mode, the animation reverses direction and runs
               ///< backwards again. @SINCE_3_0.0
};

/**
 * @brief Identifies which property inside the Lottie content a dynamic property drives.
 *
 * These name properties of the shapes and layers in the animation file. They are not DALi
 * properties: they carry no property index and cannot be set with SetProperty(). The only way
 * to give one a value is the callback passed to SetDynamicProperty().
 * @SINCE_3_0.0
 */
enum class ContentProperty : uint8_t
{
  FILL_COLOR = 0,     ///< Fill color of a shape @SINCE_3_0.0
  FILL_OPACITY,       ///< Fill opacity of a shape @SINCE_3_0.0
  STROKE_COLOR,       ///< Stroke color of a shape @SINCE_3_0.0
  STROKE_OPACITY,     ///< Stroke opacity of a shape @SINCE_3_0.0
  STROKE_WIDTH,       ///< Stroke width of a shape @SINCE_3_0.0
  TRANSFORM_ANCHOR,   ///< Anchor point of a layer @SINCE_3_0.0
  TRANSFORM_POSITION, ///< Position of a layer @SINCE_3_0.0
  TRANSFORM_SCALE,    ///< Scale of a layer @SINCE_3_0.0
  TRANSFORM_ROTATION, ///< Rotation of a layer @SINCE_3_0.0
  TRANSFORM_OPACITY   ///< Opacity of a layer @SINCE_3_0.0
};

} // namespace LottieAnimation

} // namespace Ui

} //namespace DALI_NAMESPACE
