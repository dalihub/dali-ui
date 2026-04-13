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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>

namespace Dali
{
namespace Ui
{
namespace DevelFocusManager
{
/**
 * @brief Interface used to provide custom focus algorithm for retrieving the next focusable actor.
 *
 * The application / ui can implement the interface and override the focus behaviour.
 * Upon providing an implementation of this interface, the PreFocusChangeSignal is no longer emitted.
 * If focus is changing within a layout container, then the layout container is queried first to provide
 * the next focusable actor. If this does not provide a valid actor, then the FocusManager will
 * check focusable properties to determine next focusable actor. If focusable properties are not set,
 * then the FocusManager calls the GetNextFocusableActor() method of this interface.
 */
class CustomAlgorithmInterface
{
public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~CustomAlgorithmInterface() {};

  /**
   * @brief Called by the FocusManager to get the next focusable actor.
   *
   * @param[in] current The current focused actor
   * @param[in] proposed The proposed focused actor
   * @param[in] direction The direction of focus movement
   * @param[in] deviceName The name of the device where the key event occurred.
   * @return A handle to the next focusable actor
   */
  virtual Actor GetNextFocusableActor(Actor current, Actor proposed, Ui::FocusDirection direction, const Dali::String& deviceName = "") = 0;
};

/**
 * @brief Provide the implementation of custom Focus algorithm interface
 *
 * @param[in] focusManager The instance of FocusManager
 * @param[in] interface The user's implementation of CustomAlgorithmInterface
 * @see DevelFocusManager::CustomAlgorithmInterface
 */
DALI_UI_API void SetCustomAlgorithm(FocusManager focusManager, CustomAlgorithmInterface& interface);

/**
 * @brief Decide using focus indicator or not
 *
 * @param[in] focusManager The instance of FocusManager
 * @param[in] enable Whether using focus indicator or not
 */
DALI_UI_API void EnableFocusIndicator(FocusManager focusManager, bool enable);

/**
 * @brief Check focus indicator is enabled or not
 *
 * @param[in] focusManager The instance of FocusManager
 * @return True when focus indicator is enabled
 */
DALI_UI_API bool IsFocusIndicatorEnabled(FocusManager focusManager);

/**
 * @brief Decide using default focus algorithm or not
 *
 * @param[in] focusManager The instance of FocusManager
 * @param[in] enable Whether using default focus algorithm or not
 */
DALI_UI_API void EnableDefaultAlgorithm(FocusManager focusManager, bool enable);

/**
 * @brief Check default focus algorithm is enabled or not
 *
 * @param[in] focusManager The instance of FocusManager
 * @return True when default focus algorithm is enabled
 */
DALI_UI_API bool IsDefaultAlgorithmEnabled(FocusManager focusManager);

/**
 * @brief Moves the focus to the next focusable actor in the focus
 * chain in the given direction (according to the focus traversal
 * order).
 *
 * @param[in] focusManager The instance of FocusManager
 * @param direction The direction of focus movement
 * @param deviceName The device name
 * @return true if the movement was successful
 * @pre The FocusManager has been initialized.
 */
DALI_UI_API bool MoveFocus(FocusManager focusManager, Ui::FocusDirection direction, const Dali::String& deviceName);

/**
 * @brief Sets the root actor to start moving focus when DefaultAlgorithm is enabled.
 *
 * @param[in] focusManager The instance of FocusManager
 * @param[in] actor The root actor
 */
DALI_UI_API void SetFocusFinderRootActor(FocusManager focusManager, Actor actor);

/**
 * @brief Resets the root actor that starts moving focus when DefaultAlgorithm is enabled.
 * When reset, the window becomes root.
 *
 * @param[in] focusManager The instance of FocusManager
 */
DALI_UI_API void ResetFocusFinderRootActor(FocusManager focusManager);

} // namespace DevelFocusManager

} // namespace Ui

} // namespace Dali
