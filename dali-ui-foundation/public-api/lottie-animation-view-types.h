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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/signals/callback.h>
#include <cstdint>

namespace Dali
{
namespace Ui
{
class LottieAnimationView;

/**
 * @brief Enumeration for the current playback state.
 */
struct LottieAnimationViewPlayState
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
struct LottieAnimationViewStopBehavior
{
  enum Type
  {
    CURRENT_FRAME, ///< Show the current frame when stopped
    FIRST_FRAME,   ///< Show the first frame when stopped
    LAST_FRAME     ///< Show the last frame when stopped
  };
};

/**
 * @brief Enumeration for the looping mode.
 */
struct LottieAnimationViewLoopingMode
{
  enum Type
  {
    RESTART,     ///< Restart from the beginning when the end is reached
    AUTO_REVERSE ///< Reverse direction when the end is reached
  };
};

/**
 * @brief Enumeration for animatable vector properties used with SetDynamicProperty.
 */
struct LottieAnimationViewVectorProperty
{
  enum Type
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
};

/**
 * @brief Information for setting a dynamic (per-frame callback) property.
 *
 * Ownership of @p callback is transferred to the visual after SetDynamicProperty() is called.
 *
 * @note The callback is invoked on a worker thread. Do NOT call DALi APIs from it.
 */
struct LottieAnimationViewDynamicPropertyInfo
{
  int32_t       id;       ///< Unique identifier passed to the callback
  Dali::String  keyPath;  ///< Target layer/element path (e.g. "Layer 1.Ellipse 1.Fill 1", or "**" for all)
  int32_t       property; ///< The LottieAnimationViewVectorProperty::Type to animate
  CallbackBase* callback; ///< Callback returning a Property::Value each frame
};

} // namespace Ui
} // namespace Dali
