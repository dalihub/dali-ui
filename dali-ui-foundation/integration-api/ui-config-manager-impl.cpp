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
#include <dali/devel-api/adaptor-framework/lifecycle-controller.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/default-theme-loader.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/integration-api/ui-theme-manager-impl.h>
#include <dali-ui-foundation/public-api/ui-theme-manager.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

namespace
{
const char* const UICONFIG_NOT_INITIALIZED_MESSAGE =
  "UiConfig has not been initialized. "
  "Call UiConfig::New().Apply() in main() before the application main loop starts. "
  "UiConfig provides global settings for the entire dali-ui framework. "
  "Do NOT access UiConfig-dependent features in static/global variable initializers.";

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

UiConfigManagerImpl::UiConfigManagerImpl()
{
  RegisterLifecycleControllerCallback();
}

UiConfigManagerImpl::~UiConfigManagerImpl()
{
}

void UiConfigManagerImpl::Initialize(const UiConfig& config)
{
  DALI_ASSERT_ALWAYS(!mUiConfigInitialized && "UiConfigManager::Initialize() must be called only once");
  mConfig = config;
  GetImpl(mConfig).Freeze();

  mUiConfigInitialized = true;

  UiThemeManager themeManager = UiThemeManager::Get();
  GetImpl(themeManager).EnsureThemeLoader();

  GetImpl(mConfig).OnApplied();

  if(mApplicationCreated)
  {
    GetImpl(mConfig).OnApplicationCreated();
  }
}

bool UiConfigManagerImpl::IsInitialized() const
{
  return mUiConfigInitialized;
}

const UiConfig& UiConfigManagerImpl::GetConfig() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mConfig;
}

ThemeLoaderInterface* UiConfigManagerImpl::CreateThemeLoader()
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).CreateThemeLoader();
}

bool UiConfigManagerImpl::RegisterLifecycleControllerCallback()
{
  if(!mLifecycleControllerCallbackConnected)
  {
    Dali::LifecycleController lifecycleController = Dali::LifecycleController::Get();
    if(DALI_LIKELY(lifecycleController))
    {
      mLifecycleControllerCallbackConnected = true;
      lifecycleController.PreInitSignal().Connect(this, &UiConfigManagerImpl::OnApplicationCreated);
    }
  }
  return mLifecycleControllerCallbackConnected;
}

void UiConfigManagerImpl::OnApplicationCreated()
{
  if(mApplicationCreated)
  {
    return;
  }

  mApplicationCreated = true;

  if(mUiConfigInitialized)
  {
    GetImpl(mConfig).OnApplicationCreated();
  }
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
