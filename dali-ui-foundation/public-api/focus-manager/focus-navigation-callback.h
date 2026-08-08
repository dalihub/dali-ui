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
 */

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-context.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-result.h>
#include <dali-ui-foundation/public-api/types/callback.h>

namespace Dali
{
namespace Ui
{
class View;

/**
 * @brief Move-only callback for custom focus navigation.
 *
 * @code
 * // Member function:
 * view.SetFocusNavigationCallback(FocusNavigationCallback::New(this, &MyClass::OnFocusNavigation));
 *
 * // Static function:
 * view.SetFocusNavigationCallback(FocusNavigationCallback::New(&MyFocusNavigationFunc));
 * @endcode
 *
 * @note Signature: FocusNavigationResult(View currentFocusedView, FocusNavigationContext context)
 */
using FocusNavigationCallback = Callback<FocusNavigationResult(View, FocusNavigationContext)>;

} // namespace Ui
} // namespace Dali
