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
 */
enum FocusDirection
{
  LEFT,              ///< Move keyboard focus towards the left direction
  RIGHT,             ///< Move keyboard focus towards the right direction
  UP,                ///< Move keyboard focus towards the up direction
  DOWN,              ///< Move keyboard focus towards the down direction
  PAGE_UP,           ///< Move keyboard focus towards the previous page direction
  PAGE_DOWN,         ///< Move keyboard focus towards the next page direction
  FORWARD,           ///< Move keyboard focus towards the forward direction
  BACKWARD,          ///< Move keyboard focus towards the backward direction
  CLOCKWISE,         ///< Move keyboard focus towards the clockwise direction
  COUNTER_CLOCKWISE, ///< Move keyboard focus towards the counter clockwise direction
};

/**
 * @brief Keyboard focus device.
 */
enum FocusDevice
{
  UNKNOWN,     ///< Unknown device.
  KEYBOARD,    ///< A regular keyboard, numberpad or attached buttons.
  MOUSE,       ///< A mouse, trackball or touchpad relative motion device.
  TOUCH,       ///< A touchscreen with fingers or stylus.
  PEN,         ///< A special pen device.
  POINTER,     ///< A pointing device based on laser, infrared or similar technology.
  GAMEPAD,     ///< A gamepad Viewler or joystick.
  WHEEL,       ///< A mouse device.
  PROGRAMMATIC ///< Set to API call, not device.
};
} // namespace Dali::Ui

#endif // DALI_UI_VIEW_FOCUS_ENUMS_H
