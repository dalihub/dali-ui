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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{

namespace Ui
{

class View;
class InputEvent;

namespace Integration
{

/**
 * @brief Interface for receiving interactive events from InteractiveTrait.
 *
 * When InteractiveTrait is attached to a View whose ViewImpl implements this
 * interface, the trait calls these methods directly before emitting the
 * corresponding signals. This avoids the overhead of adding signal connections
 * for subclass event handling.
 *
 * Typically implemented by InteractiveViewImpl and its subclasses.
 */
class DALI_UI_API InteractiveEventReceiverInterface
{
public:
  /**
   * @brief Called when the view is clicked.
   *
   * @param[in] view  The view that was clicked
   * @param[in] event The input event that triggered the click
   */
  virtual void OnClicked(View view, const InputEvent& event) = 0;

  /**
   * @brief Called when the pressed state changes.
   *
   * @param[in] view    The view whose pressed state changed
   * @param[in] pressed True when pressed, false when released
   * @param[in] event   The input event that caused the state change
   */
  virtual void OnPressedChanged(View view, bool pressed, const InputEvent& event) = 0;

  /**
   * @brief Called when a long press is detected.
   *
   * @param[in] view  The view that received the long press
   * @param[in] event The input event that triggered the long press
   * @return True to suppress the subsequent click event, false to allow it
   */
  virtual bool OnLongPressed(View view, const InputEvent& event) = 0;

protected:
  ~InteractiveEventReceiverInterface() = default;
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
