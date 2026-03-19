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

namespace Dali::Ui
{

/**
 * @brief Keyboard focus direction.
 * @SINCE_1_0.0
 */
enum FocusDirection
{
  LEFT,              ///< Move keyboard focus towards the left direction @SINCE_1_0.0
  RIGHT,             ///< Move keyboard focus towards the right direction @SINCE_1_0.0
  UP,                ///< Move keyboard focus towards the up direction @SINCE_1_0.0
  DOWN,              ///< Move keyboard focus towards the down direction @SINCE_1_0.0
  PAGE_UP,           ///< Move keyboard focus towards the previous page direction @SINCE_1_2.14
  PAGE_DOWN,         ///< Move keyboard focus towards the next page direction @SINCE_1_2.14
  FORWARD,           ///< Move keyboard focus towards the forward direction @SINCE_2_1.10
  BACKWARD,          ///< Move keyboard focus towards the backward direction @SINCE_2_1.10
  CLOCKWISE,         ///< Move keyboard focus towards the clockwise direction @SINCE_2_1.14
  COUNTER_CLOCKWISE, ///< Move keyboard focus towards the counter clockwise direction @SINCE_2_1.14
};

/**
 * @brief Keyboard focus device.
 * @SINCE_2_5.5
 */
enum FocusDevice
{
  UNKNOWN,     ///< Unknown device. @SINCE_2_5.5
  KEYBOARD,    ///< A regular keyboard, numberpad or attached buttons. @SINCE_2_5.5
  MOUSE,       ///< A mouse, trackball or touchpad relative motion device. @SINCE_2_5.5
  TOUCH,       ///< A touchscreen with fingers or stylus. @SINCE_2_5.5
  PEN,         ///< A special pen device. @SINCE_2_5.5
  POINTER,     ///< A pointing device based on laser, infrared or similar technology. @SINCE_2_5.5
  GAMEPAD,     ///< A gamepad Viewler or joystick. @SINCE_2_5.5
  WHEEL,       ///< A mouse device. @SINCE_2_5.5
  PROGRAMMATIC ///< Set to API call, not device. @SINCE_2_5.5
};
} // namespace Dali::Ui

#endif // DALI_UI_VIEW_FOCUS_ENUMS_H
