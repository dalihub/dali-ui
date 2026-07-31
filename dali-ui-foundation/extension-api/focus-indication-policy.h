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
#include <dali-ui-foundation/public-api/input/input-event.h>
#include <dali-ui-foundation/public-api/views/view-focus-enums.h>
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Extension
{

/**
 * @brief Describes an automatic focus indication decision.
 *
 * When focus is changing, @p previousFocusView is the view losing focus and
 * @p focusedView is the view gaining focus. When only focus indication is
 * changing, both handles refer to the currently focused view.
 *
 * If @p previousFocusView is empty, @p previousFocusIndicated is always false.
 */
struct FocusIndicationContext
{
  Dali::Ui::View previousFocusView;                      ///< The focused view before this decision, or an empty handle if none
  Dali::Ui::View focusedView;                            ///< The view that will be focused after this decision
  FocusDevice    device{FocusDevice::UNKNOWN};           ///< The device associated with the change
  InputEvent     inputEvent{InputEvent::Programmatic()}; ///< The input event associated with the change
  bool           previousFocusIndicated{false};          ///< Whether previousFocusView was focus indicated
  bool           proposedIndicated{false};               ///< The framework's default focus indication decision
};

namespace FocusIndicationPolicy
{
/**
 * @brief Function pointer type for resolving automatic focus indication changes.
 *
 * The returned value becomes the final FOCUS_INDICATED state.
 *
 * @note This is a plain function pointer. Capturing lambdas and non-static
 *       member functions are not accepted.
 */
using Function = bool (*)(const FocusIndicationContext& context);

/**
 * @brief Uses the framework's proposed focus indication state.
 *
 * @param[in] context Information about the focus indication decision
 * @return FocusIndicationContext::proposedIndicated
 */
DALI_UI_API bool Default(const FocusIndicationContext& context);

/**
 * @brief Selects focus indication for every automatic decision while a view is focused.
 *
 * Explicit focus indication changes, such as
 * FocusManager::ClearFocusIndication(), are not overridden.
 *
 * @param[in] context Information about the focus indication decision
 * @return True
 */
DALI_UI_API bool FollowFocus(const FocusIndicationContext& context);

} // namespace FocusIndicationPolicy
} // namespace Extension
} // namespace Ui
} // namespace Dali
