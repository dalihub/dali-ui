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
#include <dali-ui-foundation/integration-api/ui-config-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/provider-api/ui-config-impl.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
BaseHandle Create()
{
  BaseHandle handle = UiConfigManager::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Ui::Integration::UiConfigManager, Dali::BaseHandle, Create, true)
DALI_TYPE_REGISTRATION_END()
} // unnamed namespace

UiConfigManagerImplPtr UiConfigManagerImpl::New()
{
  return new UiConfigManagerImpl();
}

UiConfigManagerImpl::UiConfigManagerImpl() = default;

UiConfigManagerImpl::~UiConfigManagerImpl()
{
}

void UiConfigManagerImpl::Initialize(const UiConfig& config)
{
  UiConfig current = config;
  current.Apply();
}

bool UiConfigManagerImpl::IsInitialized() const
{
  return UiConfig::HasCurrent();
}

const UiConfig& UiConfigManagerImpl::GetConfig() const
{
  mConfig = UiConfig::GetCurrent();
  return mConfig;
}

ThemeLoaderInterface* UiConfigManagerImpl::CreateThemeLoader()
{
  UiConfig config = UiConfig::GetCurrent();
  return GetImpl(config).CreateThemeLoader();
}

bool UiConfigManagerImpl::RegisterLifecycleControllerCallback()
{
  return true;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
