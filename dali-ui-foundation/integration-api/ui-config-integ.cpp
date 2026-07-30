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

#include <dali-ui-foundation/integration-api/ui-config-integ.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/ui-config-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
namespace UiConfig
{

void SetFocusIndicationPolicy(Dali::Ui::UiConfig config, FocusIndicationPolicy::Function policy)
{
  GetImpl(config).SetFocusIndicationPolicy(policy);
}

FocusIndicationPolicy::Function GetFocusIndicationPolicy(const Dali::Ui::UiConfig& config)
{
  return GetImpl(config).GetFocusIndicationPolicy();
}

} // namespace UiConfig
} // namespace Integration
} // namespace Ui
} // namespace Dali
