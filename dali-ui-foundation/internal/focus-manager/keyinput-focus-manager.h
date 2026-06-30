#ifndef DALI_UI_KEYINPUT_FOCUS_MANAGER_H
#define DALI_UI_KEYINPUT_FOCUS_MANAGER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class KeyInputFocusManagerImpl;

/**
 * KeyInputFocusManager
 * This class provides the functionality of registering for keyboard events for views.
 * The keyinput focus manager maintains a stack of views, With the last added view receiving
 * all the keyboard events first. And if the conrol doesn't consume the event it is passed to
 * the next view in the stack. If none of the views in the stack consume the key event then
 * UnhandledKeyEventSignal() is emitted.
 *
 * Signals
 * | %Signal Name         | Method                            |
 * |----------------------|-----------------------------------|
 * | keyInputFocusChanged | @ref KeyInputFocusChangedSignal() |
 */
class KeyInputFocusManager : public BaseHandle
{
public:
  // KeyInputFocusChanged
  typedef Signal<void(View, View)> KeyInputFocusChangedSignalType;

public:
  /**
   * Create a KeyInputFocusManager handle; this can be initialised with KeyInputFocusManager::Get()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  KeyInputFocusManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~KeyInputFocusManager();

  /**
   * Get the singleton of KeyInputFocusManager object.
   * @return A handle to the KeyInputFocusManager view.
   */
  static KeyInputFocusManager Get();

  /**
   * Sets keyboard focus for a view.
   * Note: A view can be set to be in focus and still not receive all the key events if another view has over
   * ridden it. As the key input focus mechanism works like a stack, the top most view receives all the key events,
   * and passes on the unhandled events to the views below in the stack. A view in the stack will regain key input
   * focus when there are no more views above it in the focus stack.
   *
   * @pre The View is not in the focus stack. If it is allready present in the top of the stack it results in a
   * no-op, If it is present in the stack but not on the top of the stack, then the view is moved to the top of the
   * focus stack.
   * @param[in] view The View to receive keyboard input
   */
  void SetFocus(View view);

  /**
   * Query for the view that is currently set to be on top of the fcous stack and receives all
   * keyboard input events first.
   * @return Pointer to the view set to receive keyboard inputs.
   */
  View GetCurrentFocusView() const;

  /**
   * Removes focus for the given view, The view will no longer receive events from keyboard.
   * @param [in] view which should be removed from focus.
   */
  void RemoveFocus(View view);

public: // Signals
  /**
   * This signal is emitted when the key input focus view changes.
   * Two view parameters are sent as part of this signal, the first being the signal that now has the focus, the
   * second being the one that has lost focus. A callback of the following type may be connected:
   * @code
   *   void YourCallback(View focusGainedView, View focusLostActor);
   * @endcode
   * @return The signal to connect to.
   */
  KeyInputFocusChangedSignalType& KeyInputFocusChangedSignal();

private:
  explicit DALI_INTERNAL KeyInputFocusManager(KeyInputFocusManagerImpl* impl);

}; // class KeyInputFocusManager

} // namespace Internal

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_KEYINPUT_FOCUS_MANAGER_H
