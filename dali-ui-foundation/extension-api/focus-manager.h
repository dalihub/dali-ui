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
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{
namespace FocusManager
{
/**
 * @brief Sets the exclusive View that receives key input.
 *
 * The target receives key events before its ancestors and is notified through
 * its focus-change lifecycle. For an editable View, this notification may
 * start its editing session and activate its input method context.
 *
 * This is a low-level input-routing API. It does not update the navigation
 * focus maintained by Dali::Ui::FocusManager, including focus indication,
 * focus history, or accessibility focus. Consequently, the key-input target
 * may differ from Dali::Ui::FocusManager::GetCurrentFocusView().
 *
 * Setting a new target causes the previous target to receive a focus-lost
 * notification. Only one target is retained; previous targets are not stacked
 * or restored automatically.
 *
 * @pre This function must be called from the UI thread.
 * @pre @p view must be connected to a valid scene.
 * @param[in] view The View to receive key input
 * @return true if @p view became the key-input target; false otherwise
 */
DALI_UI_API bool SetKeyInputTarget(Ui::View view);

/**
 * @brief Clears the key-input target if @p view is still the current target.
 *
 * This operation is conditional so that delayed cleanup by a previous owner
 * cannot clear a newer target. It does not update navigation or accessibility
 * focus, and it does not restore a previous key-input target.
 *
 * Clearing an editable target sends its focus-lost notification and may end
 * its editing session or deactivate its input method context.
 *
 * @pre This function must be called from the UI thread.
 * @param[in] view The expected current key-input target
 * @return true if @p view was the current target and was cleared; false otherwise
 */
DALI_UI_API bool ClearKeyInputTarget(Ui::View view);

/**
 * @brief Checks whether @p view is the current key-input target.
 *
 * This queries current key-input routing ownership only; it does not indicate
 * whether a View is capable of becoming a key-input target. It also does not
 * indicate navigation focus, focus indication, or accessibility focus.
 *
 * @pre This function must be called from the UI thread.
 * @param[in] view The View to check
 * @return true if @p view is the current key-input target; false otherwise
 */
DALI_UI_API bool IsKeyInputTarget(Ui::View view);

} // namespace FocusManager
} // namespace Extension
} // namespace Ui
} // namespace Dali
