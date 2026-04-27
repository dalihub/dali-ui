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
#include <dali-ui-foundation/integration-api/ui-config-manager.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-manager-impl.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{
UiConfigManager gPreInitializedUiConfigManager;
}

UiConfigManager UiConfigManager::Get()
{
  UiConfigManager uiConfigManager;

  // Check whether the UiConfigManager is already created
  SingletonService singletonService(SingletonService::Get());
  if(singletonService)
  {
    BaseHandle handle = singletonService.GetSingleton(typeid(UiConfigManager));
    if(handle)
    {
      // If so, downcast the handle of singleton to UiConfigManager
      uiConfigManager = UiConfigManager(static_cast<UiConfigManagerImpl*>(handle.GetObjectPtr()));
    }

    if(!uiConfigManager) // If not, create the UiConfigManager and register it as a singleton
    {
      if(gPreInitializedUiConfigManager)
      {
        uiConfigManager = std::move(gPreInitializedUiConfigManager);
        gPreInitializedUiConfigManager.Reset();

        // Register lifecycle controller callback now for pre-initialized UiConfigManager use case.
        GetImpl(uiConfigManager).RegisterLifecycleControllerCallback();
      }
      else
      {
        auto impl       = UiConfigManagerImpl::New();
        uiConfigManager = UiConfigManager(impl.Get());
      }
      singletonService.Register(typeid(UiConfigManager), uiConfigManager);
    }
  }
  else
  {
    if(!gPreInitializedUiConfigManager)
    {
      auto impl                      = UiConfigManagerImpl::New();
      gPreInitializedUiConfigManager = UiConfigManager(impl.Get());
    }
    uiConfigManager = gPreInitializedUiConfigManager;
  }

  return uiConfigManager;
}

UiConfigManager::UiConfigManager() = default;

UiConfigManager::~UiConfigManager() = default;

UiConfigManager::UiConfigManager(const UiConfigManager& rhs) = default;

UiConfigManager& UiConfigManager::operator=(const UiConfigManager& rhs) = default;

UiConfigManager::UiConfigManager(UiConfigManager&& rhs) noexcept = default;

UiConfigManager& UiConfigManager::operator=(UiConfigManager&& rhs) noexcept = default;

void UiConfigManager::Initialize(const UiConfig& config)
{
  GetImpl(*this).Initialize(config);
}

bool UiConfigManager::IsInitialized() const
{
  return GetImpl(*this).IsInitialized();
}

const UiConfig& UiConfigManager::GetConfig() const
{
  return GetImpl(*this).GetConfig();
}

ThemeLoaderInterface* UiConfigManager::CreateThemeLoader()
{
  return GetImpl(*this).CreateThemeLoader();
}

UiConfigManager::UiConfigManager(UiConfigManagerImpl* impl)
: BaseHandle(impl)
{
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
