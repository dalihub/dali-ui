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
#include <dali-ui-elements/integration-api/ui-element-config-manager.h>
#include <dali-ui-elements/integration-api/ui-element-config-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

namespace
{
const char* const UIELEMENT_CONFIG_NOT_INITIALIZED_MESSAGE =
    "UiElementConfigManager has not been initialized. "
    "Call UiConfig.Apply() to enable element defaults.";
} // unnamed namespace

UiElementConfigManager& UiElementConfigManager::Get()
{
  static UiElementConfigManager instance;
  return instance;
}

void UiElementConfigManager::Init(const UiElementConfigImpl& impl)
{
  mDefaultFontSize = impl.GetDefaultFontSize();
  mDefaultTextColor = impl.GetDefaultTextColor();
  mInitialized = true;
}

bool UiElementConfigManager::IsInitialized() const
{
  return mInitialized;
}

float UiElementConfigManager::GetDefaultFontSize() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UIELEMENT_CONFIG_NOT_INITIALIZED_MESSAGE);
  return mDefaultFontSize;
}

Vector4 UiElementConfigManager::GetDefaultTextColor() const
{
  DALI_ASSERT_ALWAYS(mInitialized && UIELEMENT_CONFIG_NOT_INITIALIZED_MESSAGE);
  return mDefaultTextColor;
}

} // namespace Integration

} // namespace Ui

} // namespace Dali
