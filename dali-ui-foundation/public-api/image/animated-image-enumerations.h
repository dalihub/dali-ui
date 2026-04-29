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

namespace Dali
{

namespace Ui
{

namespace AnimatedImage
{
/**
 * @brief Enumeration for what state the animation is in.
 */
enum class PlayState : uint8_t
{
  STOPPED, ///< Animation has stopped
  PLAYING, ///< The animation is playing
  PAUSED   ///< The animation is paused
};

/**
 * @brief Enumeration for what to do when the animation is stopped.
 */
enum class StopBehavior : uint8_t
{
  CURRENT_FRAME, ///< When the animation is stopped, the current frame is shown.
  FIRST_FRAME,   ///< When the animation is stopped, the first frame is shown.
  LAST_FRAME     ///< When the animation is stopped, the last frame is shown.
};

} // namespace AnimatedImage

} // namespace Ui

} // namespace Dali
