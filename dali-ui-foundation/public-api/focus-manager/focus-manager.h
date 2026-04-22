#pragma once

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/view-focus-enums.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class FocusManager;
}
/**
 * @addtogroup dali_ui_managers
 * @{
 */

/**
 * @brief Provides the functionality of handling focus navigation
 * and maintaining the two dimensional focus chain.
 *
 * It provides functionality of setting the focus and moving the focus
 * in four directions (i.e. Left, Right, Up and Down). It also draws a
 * highlight for the focused view and emits a signal when the focus
 * is changed.
 *
 * Signals
 * | %Signal Name            | Method                             |
 * |-------------------------|------------------------------------|
 * | preFocusChange          | @ref PreFocusChangeSignal()        |
 * | focusChanged            | @ref FocusChangedSignal()          |
 * | focusGroupChanged       | @ref FocusGroupChangedSignal()     |
 * | focusedViewEnterKey     | @ref FocusedViewEnterKeySignal()   |
 */
class DALI_UI_API FocusManager : public BaseHandle
{
public:
  /// @brief Pre focus change signal
  typedef Signal<View(View, View, Ui::FocusDirection)> PreFocusChangeSignalType;

  /// @brief Focus changed signal
  typedef Signal<void(View, View)> FocusChangedSignalType;

  /// @brief Focus group changed signal
  typedef Signal<void(View, bool)> FocusGroupChangedSignalType;

  /// @brief Focused view has the enter key pressed signal
  typedef Signal<void(View)> FocusedViewEnterKeySignalType;

  /**
   * @brief Creates a FocusManager handle; this can be initialized with FocusManager::New().
   *
   * Calling member functions with an uninitialized handle is not allowed.
   */
  FocusManager();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FocusManager();

  /**
   * @brief Gets the singleton of FocusManager object.
   *
   * @return A handle to the FocusManager view
   */
  static FocusManager Get();

  /**
   * @brief Moves the focus to the given view.
   *
   * Only one view can be focused at the same time.  The view must
   * be in the stage already and focusable.
   *
   * @param view The view to be focused
   * @return Whether the focus is successful or not
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   * @note If the parent of this view has the KEYBOARD FOCUSABLE CHILDREN property set to false, it will not be focused.
   */
  bool SetCurrentFocusView(View view);

  /**
   * @brief Gets the current focused view.
   *
   * @return A handle to the current focused view or an empty handle if no view is focused
   * @pre The FocusManager has been initialized.
   */
  View GetCurrentFocusView();

  /**
   * @brief Moves the focus to the next focusable actor in the focus
   * chain in the given direction (according to the focus traversal
   * order).
   *
   * @param direction The direction of focus movement
   * @return true if the movement was successful
   * @pre The FocusManager has been initialized.
   */
  bool MoveFocus(FocusDirection direction);

  /**
   * @brief Clears the focus from the current focused actor if any, so
   * that no actor is focused in the focus chain.
   *
   * It will emit focus changed signal without current focused actor.
   * @pre The FocusManager has been initialized.
   */
  void ClearFocus();

  /**
   * @brief Sets whether the focus movement should be looped within the same focus group.
   *
   * The focus movement is not looped by default.
   * @param enabled Whether the focus movement should be looped
   * @pre The FocusManager has been initialized.
   */
  void SetFocusGroupLoop(bool enabled);

  /**
   * @brief Gets whether the focus movement should be looped within the same focus group.
   *
   * @return Whether the focus movement should be looped
   * @pre The FocusManager has been initialized.
   */
  bool GetFocusGroupLoop() const;

  /**
   * @brief Sets whether a view is a focus group that can limit the
   * scope of focus movement to its child views in the focus chain.
   *
   * Layout views set themselves as focus groups by default.
   *
   * @param view The view to be set as a focus group
   * @param isFocusGroup Whether to set the view as a focus group or not
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   */
  void SetAsFocusGroup(View view, bool isFocusGroup);

  /**
   * @brief Checks whether the view is set as a focus group or not.
   *
   * @param view The view to be checked
   * @return Whether the view is set as a focus group
   * @pre The FocusManager has been initialized.
   * @pre The View has been initialized.
   */
  bool IsFocusGroup(View view) const;

  /**
   * @brief Returns the closest ancestor of the given view that is a focus group.
   *
   * @param view The view to be checked for its focus group
   * @return The focus group the given view belongs to or an empty handle if the given view
   * doesn't belong to any focus group
   */
  View GetFocusGroup(View view);

  /**
   * @brief Sets the focus indicator actor.
   *
   * This will replace the default focus indicator actor in
   * FocusManager and will be added to the focused actor as a
   * highlight.
   *
   * @param indicator The indicator view to be added
   * @pre The FocusManager has been initialized.
   * @pre The indicator view has been initialized.
   */
  void SetFocusIndicatorActor(View indicator);

  /**
   * @brief Gets the focus indicator view.
   *
   * @return A handle to the focus indicator view
   * @pre The FocusManager has been initialized.
   */
  View GetFocusIndicatorView();

  /**
   * @brief Move the focus to prev focused actor
   */
  void MoveFocusBackward();

  /**
   * @brief Gets the device of the last focus change.
   *
   * This method returns what caused the most recent focus change,
   * allowing applications to differentiate between different input methods.
   *
   * @return The device of the last focus change
   * @pre The FocusManager has been initialized.
   */
  FocusDevice GetLastFocusChangeDevice() const;

  /**
   * @brief Gets the device name that caused the last focus change.
   *
   * This method returns the name of the input device that caused
   * the most recent focus change. For non-device inputs (like programmatic
   * focus changes), an empty string may be returned.
   *
   * @return The device name that caused the last focus change
   * @pre The FocusManager has been initialized.
   */
  const Dali::String& GetLastFocusChangeDeviceName() const;

  /**
   * @brief Sets whether to clear focus when window loses focus.
   *
   * By default, this is enabled.
   * When disabled, the focus state is preserved even when the window loses focus,
   *
   * @param enabled Whether to clear focus when window loses focus
   */
  void SetClearFocusOnWindowFocusLost(bool enabled);

  /**
   * @brief Gets whether to clear focus when window loses focus.
   *
   * @return Whether clear focus is enabled when window loses focus
   */
  bool GetClearFocusOnWindowFocusLost() const;

public: // Signals
  /**
   * @brief This signal is emitted before the focus is going to be changed.
   *
   * FocusManager makes the best guess for which view to
   * focus towards the given direction, but applications might want to
   * change that. By connecting with this signal, they can check the
   * proposed view to focus and return a different view if they
   * wish. This signal is only emitted when the navigation key is
   * pressed and FocusManager tries to move the focus
   * automatically. It won't be emitted for focus movement by calling
   * SetCurrentFocusView directly.
   *
   * A callback of the following type may be connected:
   * @code
   *   View YourCallbackName(View currentFocusedView, View proposedViewToFocus, FocusDirection::Direction direction);
   * @endcode
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  PreFocusChangeSignalType& PreFocusChangeSignal();

  /**
   * @brief This signal is emitted after the current focused view has been changed.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(View originalFocusedView, View currentFocusedView);
   * @endcode
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusChangedSignalType& FocusChangedSignal();

  /**
   * @brief This signal is emitted when the focus group has been changed.
   *
   * If the current focus group has a parent layout view,
   * FocusManager will make the best guess for the next focus
   * group to move the focus to in the given direction (forward or
   * backward). If not, the application has to set the new focus.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(View currentFocusedView, bool forward);
   * @endcode
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusGroupChangedSignalType& FocusGroupChangedSignal();

  /**
   * @brief This signal is emitted when the current focused view has the enter key pressed on it.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(View enterPressedView);
   * @endcode
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  FocusedViewEnterKeySignalType& FocusedViewEnterKeySignal();

  // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a new handle from the implementation.
   *
   * @param[in] impl A pointer to the object
   */
  explicit DALI_INTERNAL FocusManager(Internal::FocusManager* impl);
  /// @endcond

}; // class FocusManager

/**
 * @}
 */
} // namespace Ui

} // namespace Dali
