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
#include <dali-ui-foundation/public-api/styles/ui-style-sheet.h>

namespace Dali
{
namespace Ui
{
namespace Components
{
namespace StyleSheet
{

/**
 * @brief Creates a new components style sheet.
 *
 * Components::UiConfig already provides a components style sheet. Applications
 * usually override entries through Components::UiConfig::StyleSheet().
 * Create a separate sheet only when a provider needs to replace the whole
 * style sheet through UiConfig::ResetStyleSheet().
 *
 * @return A new components style sheet
 */
DALI_UI_API UiStyleSheet New();

} // namespace StyleSheet
} // namespace Components
} // namespace Ui
} // namespace Dali
