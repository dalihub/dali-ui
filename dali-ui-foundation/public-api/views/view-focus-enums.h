#ifndef DALI_UI_VIEW_FOCUS_ENUMS_H
#define DALI_UI_VIEW_FOCUS_ENUMS_H

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
#include <dali/public-api/common/dali-namespace.h>

namespace DALI_NAMESPACE::Ui
{

/**
 * @brief Keyboard focus direction.
 * @SINCE_3_0.0
 */
enum FocusDirection
{
  LEFT,              ///< Move keyboard focus towards the left direction @SINCE_3_0.0
  RIGHT,             ///< Move keyboard focus towards the right direction @SINCE_3_0.0
  UP,                ///< Move keyboard focus towards the up direction @SINCE_3_0.0
  DOWN,              ///< Move keyboard focus towards the down direction @SINCE_3_0.0
  PAGE_UP,           ///< Move keyboard focus towards the previous page direction @SINCE_3_0.0
  PAGE_DOWN,         ///< Move keyboard focus towards the next page direction @SINCE_3_0.0
  FORWARD,           ///< Move keyboard focus towards the forward direction @SINCE_3_0.0
  BACKWARD,          ///< Move keyboard focus towards the backward direction @SINCE_3_0.0
  CLOCKWISE,         ///< Move keyboard focus towards the clockwise direction @SINCE_3_0.0
  COUNTER_CLOCKWISE, ///< Move keyboard focus towards the counter clockwise direction @SINCE_3_0.0
};

/**
 * @brief Keyboard focus device.
 * @SINCE_3_0.0
 */
enum FocusDevice
{
  UNKNOWN,     ///< Unknown device. @SINCE_3_0.0
  KEYBOARD,    ///< A regular keyboard, numberpad or attached buttons. @SINCE_3_0.0
  MOUSE,       ///< A mouse, trackball or touchpad relative motion device. @SINCE_3_0.0
  TOUCH,       ///< A touchscreen with fingers or stylus. @SINCE_3_0.0
  PEN,         ///< A special pen device. @SINCE_3_0.0
  POINTER,     ///< A pointing device based on laser, infrared or similar technology. @SINCE_3_0.0
  GAMEPAD,     ///< A gamepad Viewler or joystick. @SINCE_3_0.0
  WHEEL,       ///< A mouse device. @SINCE_3_0.0
  PROGRAMMATIC ///< Set to API call, not device. @SINCE_3_0.0
};
} //namespace DALI_NAMESPACE::Ui

#endif // DALI_UI_VIEW_FOCUS_ENUMS_H
