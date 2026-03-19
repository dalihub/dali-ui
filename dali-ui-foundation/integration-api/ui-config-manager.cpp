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

void UiConfigManager::Initialize(const UiConfig& config)
{
  DALI_ASSERT_ALWAYS(!mUiConfigInitialized && "UiConfigManager::Initialize() must be called only once");
  mConfig = config;
  GetImpl(mConfig).Freeze();

  const auto& impl = GetImpl(mConfig);
  // Cache derived unit factors once at initialization time.
  // Unit literals (_spx/_dp/_sdp) and layout scaling paths query these frequently.
  // UiConfig is frozen after Initialize(), so caching these derived values is safe.
  mCachedScalingFactor   = impl.GetScalingFactor();
  const int dpi          = impl.GetDpi();
  const int baselineDpi  = impl.GetBaselineDpi();
  mCachedDpiFactor       = static_cast<float>(dpi) / static_cast<float>(baselineDpi);
  mCachedScaledDpiFactor = mCachedDpiFactor * mCachedScalingFactor;
  mUiConfigInitialized   = true;

  UiThemeManager themeManager = UiThemeManager::Get();
  GetImpl(themeManager).EnsureThemeLoader();

  GetImpl(mConfig).OnApplied();

  if(mApplicationCreated)
  {
    GetImpl(mConfig).OnApplicationCreated();
  }
}

bool UiConfigManager::IsInitialized() const
{
  return mUiConfigInitialized;
}

float UiConfigManager::GetScalingFactor() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedScalingFactor;
}

float UiConfigManager::GetDpiFactor() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedDpiFactor;
}

float UiConfigManager::GetScaledDpiFactor() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedScaledDpiFactor;
}

int UiConfigManager::GetDpi() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetDpi();
}

int UiConfigManager::GetBaselineDpi() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetBaselineDpi();
}

KeyClickPolicy UiConfigManager::GetKeyClickPolicy() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetKeyClickPolicy();
}

ExecutionKeyPredicate UiConfigManager::GetExecutionKeyPredicate() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetExecutionKeyPredicate();
}

uint32_t UiConfigManager::GetMinLongPressKeyCount() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetMinLongPressKeyCount();
}

uint32_t UiConfigManager::GetTapRecognizerTime() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetTapRecognizerTime();
}

std::vector<std::string> UiConfigManager::GetBrokenImageUrlList() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetBrokenImageUrlList();
}

bool UiConfigManager::IsFocusClearOnEscapeEnabled() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).IsFocusClearOnEscapeEnabled();
}

bool UiConfigManager::IsFocusIndicatorAlwaysShown() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).IsFocusIndicatorAlwaysShown();
}

float UiConfigManager::GetDefaultFontSize() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetDefaultFontSize();
}

Vector4 UiConfigManager::GetDefaultTextColor() const
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).GetDefaultTextColor();
}

ThemeLoaderInterface* UiConfigManager::CreateThemeLoader()
{
  DALI_ASSERT_ALWAYS(mUiConfigInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return GetImpl(mConfig).CreateThemeLoader();
}

void UiConfigManager::OnApplicationCreated()
{
  // FIXME This method is temporary solution to detect the ready state of the dali-adaptor
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
