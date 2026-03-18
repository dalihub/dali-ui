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

// CLASS HEADER
#include <dali-ui-components/integration-api/ui-component-config-impl.h>
#include <dali-ui-components/public-api/ui-component-config.h>

namespace Dali
{

namespace Ui
{

UiComponentConfig::UiComponentConfig(Integration::UiComponentConfigImpl* impl)
: UiConfig(impl)
{
}

UiComponentConfig UiComponentConfig::New()
{
  Integration::UiComponentConfigImplPtr impl = Integration::UiComponentConfigImpl::New();

  // Pass ownership to handle
  return UiComponentConfig(impl.Get());
}

UiComponentConfig UiComponentConfig::DownCast(BaseHandle handle)
{
  return UiComponentConfig(dynamic_cast<Integration::UiComponentConfigImpl*>(handle.GetObjectPtr()));
}

} // namespace Ui
} // namespace Dali
