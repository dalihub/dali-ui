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

#include <cstdint>

namespace Dali::Ui
{
/**
 * @brief Defines the type of input events.
 */
enum class InputEventType : uint32_t
{
  /**
   * @brief The change did not originate from a concrete input event.
   *
   * This includes explicit API calls, such as programmatic state changes,
   * and framework-triggered lifecycle changes, such as focus loss, disabling,
   * or scene disconnection.
   */
  NONE = 0,

  /**
   * @brief The input event is from touch event.
   */
  TOUCH_EVENT = 1,

  /**
   * @brief The input event is from key event.
   */
  KEY_EVENT = 2,

  /**
   * @brief The input event is from tap gesture.
   */
  TAP_GESTURE = 3,

  /**
   * @brief The input event is from long press gesture.
   */
  LONG_PRESS_GESTURE = 4,

  /**
   * @brief The input event is from wheel event.
   */
  WHEEL_EVENT = 5,

  /**
   * @brief Reserved for future expansion.
   */
  RESERVED = 99
};
} // namespace Dali::Ui
