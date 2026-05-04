#ifndef DALI_UI_INTERNAL_FOCUS_FINDER_H
#define DALI_UI_INTERNAL_FOCUS_FINDER_H

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
#include <dali-ui-foundation/public-api/view-focus-enums.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace FocusFinder
{

/**
 * Get the nearest focusable view.
 * @param [in] root The root actor to search within (may be a Layer or any Actor).
 * @param [in] focusedView The current focused view.
 * @param [in] direction The direction.
 * @return The nearest focusable view, or an empty handle if none exists.
 */
View GetNearestFocusableView(Actor root, View focusedView, Ui::FocusDirection direction);

/**
 * Get the next focusable view in reading order (top-to-bottom, left-to-right).
 * Used for FORWARD/BACKWARD (Tab/Shift+Tab) navigation.
 * @param [in] root The root actor to search within.
 * @param [in] focusedView The current focused view.
 * @param [in] direction Must be FORWARD or BACKWARD.
 * @return The next focusable view in reading order, or an empty handle if none exists.
 */
View GetNextFocusableViewInOrder(Actor root, View focusedView, Ui::FocusDirection direction);

} // namespace FocusFinder
} // namespace Internal
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_FOCUS_FINDER_H
