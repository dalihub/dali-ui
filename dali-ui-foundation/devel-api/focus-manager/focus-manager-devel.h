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
#include <dali-ui-foundation/public-api/views/view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>

namespace Dali
{
namespace Ui
{
namespace DevelFocusManager
{
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

} // namespace DevelFocusManager

} // namespace Ui

} // namespace Dali
