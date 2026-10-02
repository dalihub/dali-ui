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
#include <dali/public-api/adaptor-framework/window.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class KeyInputFocusManagerImpl;

/**
 * KeyInputFocusManager
 * This class provides the functionality of registering for keyboard events for views.
 * One key-input target is retained for the primary Window and for each explicitly
 * independent Window. Events select a target by the delivering SceneHolder and
 * bubble through its ancestors in that Window. Previous targets are not stacked.
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
   * Replaces the target in the View's focus scope and notifies actual focus
   * loss/gain. A request for the existing target is a no-op.
   * @param[in] view The View to receive keyboard input
   */
  void SetFocus(View view);

  /**
   * Queries the primary Window's current key-input target.
   * @return Pointer to the view set to receive keyboard inputs.
   */
  View GetCurrentFocusView() const;

  /**
   * @brief Gets the actual key input target in the specified Window.
   * @param[in] window The Window to query
   * @return Its independent or primary target, or an empty View if none exists
   */
  View GetCurrentFocusView(Dali::Window window) const;

  /**
   * @brief Registers or removes independent key input target storage.
   * Enabling adopts an existing primary target in the Window without notifying
   * focus gain. The caller clears inactive targets before disabling storage.
   * @param[in] window The Window whose storage is changed
   * @param[in] enabled Whether independent storage is enabled
   */
  void SetIndependentWindow(Window window, bool enabled);

  /**
   * @brief Promotes the Window's existing key target to the primary role.
   * Retains targets owned by other independent Windows and releases an ordinary
   * displaced target. A loss callback's newer target is preserved.
   * @param[in] window The new primary Window; empty clears the primary role
   */
  void SetPrimaryWindow(Window window);

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

} //namespace DALI_NAMESPACE

#endif // DALI_UI_KEYINPUT_FOCUS_MANAGER_H
