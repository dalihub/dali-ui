#ifndef DALI_UI_FOCUS_FINDER_H
#define DALI_UI_FOCUS_FINDER_H

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
#include <dali-ui-foundation/public-api/controls/control.h>
#include <dali-ui-foundation/public-api/view.h>

namespace Dali
{
namespace Ui
{
namespace FocusFinder
{

/**
 * Get the nearest focusable actor.
 * @param [in] rootActor The root actor.
 * @param [in] focusedActor The current focused actor.
 * @param [in] direction The direction.
 * @return The nearest focusable actor, or an empty handle if none exists.
 */
DALI_UI_API Actor GetNearestFocusableActor(Actor rootActor, Actor focusedActor,
                                           Ui::Control::KeyboardFocus::Direction direction);
DALI_UI_API Actor GetNearestFocusableActor(Actor rootActor, Actor focusedActor,
                                           Ui::View::KeyboardFocus::Direction direction);

} // namespace FocusFinder

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOCUS_FINDER_H
