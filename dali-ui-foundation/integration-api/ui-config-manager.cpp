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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// CLASS HEADER
#include <dali-ui-foundation/integration-api/ui-config-manager.h>

namespace Dali
{

namespace UI
{

namespace Integration
{

UIConfigManager& UIConfigManager::Get()
{
  static UIConfigManager instance;
  return instance;
}

namespace
{
const char* const UICONFIG_NOT_INITIALIZED_MESSAGE =
    "UIConfig has not been initialized. "
    "Call UIConfig::New().Apply() in main() before the application main loop starts. "
    "UIConfig provides global settings for the entire dali-ui framework. "
    "Do NOT access UIConfig-dependent features in static/global variable initializers.";
} // unnamed namespace

void UIConfigManager::Initialize(UIConfig config)
{
  DALI_ASSERT_ALWAYS(!mInitialized && "UIConfigManager::Init() must be called only once");
  mConfig = std::move(config);
  GetImpl(mConfig).Freeze();

  const auto& impl = GetImpl(mConfig);
  mCachedScalingFactor = impl.GetScalingFactor();
  mCachedDpiFactor = impl.GetDpiFactor();
  mCachedScaledDpiFactor = impl.GetScaledDpiFactor();
  mCachedDpi = impl.GetDpi();
  mCachedBaselineDpi = impl.GetBaselineDpi();
  mCachedKeyClickPolicy = impl.GetKeyClickPolicy();
  mCachedExecutionKeyPredicate = impl.GetExecutionKeyPredicate();
  mCachedMinLongPressKeyCount = impl.GetMinLongPressKeyCount();
  mCachedTapRecognizerTime = impl.GetTapRecognizerTime();
  mInitialized = true;

  GetImpl(mConfig).OnInitialized();
}

bool UIConfigManager::IsInitialized() const
{
  return mInitialized;
}

float UIConfigManager::GetScalingFactor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedScalingFactor;
}

float UIConfigManager::GetDpiFactor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedDpiFactor;
}

float UIConfigManager::GetScaledDpiFactor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedScaledDpiFactor;
}

int UIConfigManager::GetDpi() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedDpi;
}

int UIConfigManager::GetBaselineDpi() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedBaselineDpi;
}

KeyClickPolicy UIConfigManager::GetKeyClickPolicy() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedKeyClickPolicy;
}

ExecutionKeyPredicate UIConfigManager::GetExecutionKeyPredicate() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedExecutionKeyPredicate;
}

uint32_t UIConfigManager::GetMinLongPressKeyCount() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedMinLongPressKeyCount;
}

uint32_t UIConfigManager::GetTapRecognizerTime() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UICONFIG_NOT_INITIALIZED_MESSAGE);
  return mCachedTapRecognizerTime;
}

} // namespace Integration

} // namespace UI

} // namespace Dali
