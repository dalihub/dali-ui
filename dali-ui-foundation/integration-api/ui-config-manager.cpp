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
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-theme-manager-impl.h>
#include <dali-ui-foundation/public-api/ui-theme-manager.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

UiConfigManager& UiConfigManager::Get()
{
  static UiConfigManager instance;
  return instance;
}

namespace
{
const char* const UICONFIG_NOT_INITIALIZED_MESSAGE =
  "UiConfig has not been initialized. "
  "Call UiConfig::New().Apply() in main() before the application main loop starts. "
  "UiConfig provides global settings for the entire dali-ui framework. "
  "Do NOT access UiConfig-dependent features in static/global variable initializers.";
} // unnamed namespace

void UiConfigManager::Initialize(UiConfig config)
{
  DALI_ASSERT_ALWAYS(!mInitialized && "UiConfigManager::Initialize() must be called only once");
  mConfig = std::move(config);
  GetImpl(mConfig).Freeze();

  const auto& impl             = GetImpl(mConfig);
  mCachedScalingFactor         = impl.GetScalingFactor();
  mCachedDpiFactor             = impl.GetDpiFactor();
  mCachedScaledDpiFactor       = impl.GetScaledDpiFactor();
  mCachedDpi                   = impl.GetDpi();
  mCachedBaselineDpi           = impl.GetBaselineDpi();
  mCachedKeyClickPolicy        = impl.GetKeyClickPolicy();
  mCachedExecutionKeyPredicate = impl.GetExecutionKeyPredicate();
  mCachedMinLongPressKeyCount  = impl.GetMinLongPressKeyCount();
  mCachedTapRecognizerTime     = impl.GetTapRecognizerTime();
  mInitialized                 = true;

  UiThemeManager themeManager = UiThemeManager::Get();
  GetImpl(themeManager).EnsureThemeLoader();

  GetImpl(mConfig).OnInitialized();
}

bool UiConfigManager::IsInitialized() const
{
  return mInitialized;
}

float UiConfigManager::GetScalingFactor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedScalingFactor;
}

float UiConfigManager::GetDpiFactor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedDpiFactor;
}

float UiConfigManager::GetScaledDpiFactor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedScaledDpiFactor;
}

int UiConfigManager::GetDpi() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedDpi;
}

int UiConfigManager::GetBaselineDpi() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedBaselineDpi;
}

KeyClickPolicy UiConfigManager::GetKeyClickPolicy() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedKeyClickPolicy;
}

ExecutionKeyPredicate UiConfigManager::GetExecutionKeyPredicate() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedExecutionKeyPredicate;
}

uint32_t UiConfigManager::GetMinLongPressKeyCount() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedMinLongPressKeyCount;
}

uint32_t UiConfigManager::GetTapRecognizerTime() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedTapRecognizerTime;
}

std::vector<std::string> UiConfigManager::GetBrokenImageUrlList() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetBrokenImageUrlList();
}

bool UiConfigManager::IsFocusClearOnEscapeEnabled() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).IsFocusClearOnEscapeEnabled();
}

bool UiConfigManager::IsFocusIndicatorAlwaysShown() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).IsFocusIndicatorAlwaysShown();
}

ThemeLoaderInterface* UiConfigManager::CreateThemeLoader()
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).CreateThemeLoader();
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
