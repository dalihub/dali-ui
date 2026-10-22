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

// EXTERNAL INCLUDES
#include <dali/public-api/signals/callback.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-context.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-navigation-result.h>

namespace DALI_NAMESPACE
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
 * The result is borrowed for this invocation only and initially NOT_HANDLED.
 * Write a decision with result.MoveTo(), result.Stay() or result.NotHandled().
 * Do not retain its address or reference after returning.
 *
 * @note Signature: void(View currentFocusedView, FocusNavigationContext context, FocusNavigationResult& result)
 */
using FocusNavigationCallback = Callback<void(View, FocusNavigationContext, FocusNavigationResult&)>;

} // namespace Ui
} //namespace DALI_NAMESPACE
