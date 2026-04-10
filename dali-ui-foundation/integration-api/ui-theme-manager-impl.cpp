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
#include <dali-ui-foundation/integration-api/ui-theme-manager-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/default-theme-loader.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>

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
} // unnamed namespace

UiThemeManagerImpl::UiThemeManagerImpl() = default;

UiThemeManagerImpl::~UiThemeManagerImpl()
{
  delete mLoader;
}

UiThemeManager UiThemeManagerImpl::Get()
{
  static IntrusivePtr<UiThemeManagerImpl> impl;

  if(!impl)
  {
    impl = new UiThemeManagerImpl();
  }

  return UiThemeManager(impl.Get());
}

void UiThemeManagerImpl::EnsureThemeLoader()
{
  if(!mLoader)
  {
    auto configManager = UiConfigManager::Get();
    DALI_ASSERT_ALWAYS(configManager.IsInitialized() && UICONFIG_NOT_INITIALIZED_MESSAGE);

    mLoader = configManager.CreateThemeLoader();
    if(!mLoader)
    {
      mLoader = new DefaultThemeLoader();
    }
    mLoader->ThemeChangedSignal().Connect(mSlotDelegate, &UiThemeManagerImpl::OnLoaderThemeChanged);
  }
}

ThemeLoaderInterface& UiThemeManagerImpl::GetLoader()
{
  EnsureThemeLoader();
  return *mLoader;
}

String UiThemeManagerImpl::GetCurrentThemeId() const
{
  if(mLoader)
  {
    return mLoader->GetCurrentThemeId();
  }
  return {};
}

UiThemeManagerImpl::ThemeChangedSignalType& UiThemeManagerImpl::ThemeChangedSignal()
{
  return mThemeChangedSignal;
}

void UiThemeManagerImpl::OnLoaderThemeChanged()
{
  mThemeChangedSignal.Emit();
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
