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
#include <dali-ui-foundation/integration-api/focus-indication-policy.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace UiConfig
{

/**
 * @brief Sets the policy used to resolve automatic focus indication changes.
 *
 * The policy is queried when FocusManager changes focus and when keyboard,
 * touch, or hover input proposes changing FOCUS_INDICATED. Explicit calls to
 * FocusManager::ClearFocusIndication() are not overridden by this policy.
 * The policy does not affect whether navigation input moves focus.
 * FOCUS_INDICATED is still removed when a view loses focus because a view
 * cannot remain focus indicated without being focused.
 *
 * Pass FocusIndicationPolicy::Default or nullptr to restore the framework's
 * default behavior. FocusIndicationPolicy::FollowFocus selects focus indication
 * for every automatic decision while a view is focused.
 *
 * @pre The config must not be frozen.
 * @param[in] config The UiConfig to update
 * @param[in] policy The focus indication policy
 */
DALI_UI_API void SetFocusIndicationPolicy(Dali::Ui::UiConfig config, FocusIndicationPolicy::Function policy);

/**
 * @brief Gets the configured focus indication policy.
 *
 * @param[in] config The UiConfig to query
 * @return The configured policy
 */
DALI_UI_API FocusIndicationPolicy::Function GetFocusIndicationPolicy(const Dali::Ui::UiConfig& config);

} // namespace UiConfig
} // namespace Integration
} // namespace Ui
} // namespace Dali
