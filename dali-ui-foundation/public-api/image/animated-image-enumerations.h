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
#include <dali-ui-foundation/public-api/image/image-enumerations.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

/**
 * @brief Enumerations shared by every image that animates.
 *
 * That covers both kinds: a raster animation, meaning a GIF, a WebP or a list of images drawn
 * by AnimatedImageVisual, and a vector animation, meaning a Lottie file drawn by
 * LottieAnimationVisual. A still image has none of these.
 * @SINCE_3_0.0
 */
namespace AnimatedImage
{
/**
 * @brief Enumeration for what state the animation is in.
 * @SINCE_3_0.0
 */
enum class PlayState : uint8_t
{
  STOPPED, ///< Animation has stopped @SINCE_3_0.0
  PLAYING, ///< The animation is playing @SINCE_3_0.0
  PAUSED   ///< The animation is paused @SINCE_3_0.0
};

/**
 * @brief Enumeration for what to do when the animation is stopped.
 * @SINCE_3_0.0
 */
enum class StopBehavior : uint8_t
{
  CURRENT_FRAME, ///< When the animation is stopped, the current frame is shown. @SINCE_3_0.0
  FIRST_FRAME,   ///< When the animation is stopped, the first frame is shown. @SINCE_3_0.0
  LAST_FRAME     ///< When the animation is stopped, the last frame is shown. @SINCE_3_0.0
};

} // namespace AnimatedImage

} // namespace Ui

} //namespace DALI_NAMESPACE
