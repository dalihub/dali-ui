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
#include <dali-ui-foundation/public-api/image-view-types.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Ui
{
class AnimatedImageView;

/**
 * @brief Enumeration for the current playback state.
 */
struct AnimatedImageViewPlayState
{
  enum Type
  {
    STOPPED, ///< Animation has stopped
    PLAYING, ///< The animation is playing
    PAUSED   ///< The animation is paused
  };
};

/**
 * @brief Enumeration for what to do when the animation is stopped.
 */
struct AnimatedImageViewStopBehavior
{
  enum Type
  {
    CURRENT_FRAME, ///< Show the current frame when stopped
    FIRST_FRAME,   ///< Show the first frame when stopped
    LAST_FRAME     ///< Show the last frame when stopped
  };
};

} // namespace Ui
} // namespace Dali
