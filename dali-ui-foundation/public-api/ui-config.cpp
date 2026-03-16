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
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/public-api/ui-config.h>

namespace Dali
{

namespace Ui
{

UiConfig::UiConfig(Integration::UiConfigImpl* impl)
: BaseHandle(impl)
{
}

UiConfig UiConfig::New()
{
  Integration::UiConfigImplPtr impl = Integration::UiConfigImpl::New();

  // Pass ownership to handle
  UiConfig handle(impl.Get());

  return handle;
}

UiConfig UiConfig::DownCast(BaseHandle handle)
{
  return UiConfig(dynamic_cast<Integration::UiConfigImpl*>(handle.GetObjectPtr()));
}

void UiConfig::Apply()
{
  Integration::UiConfigManager::Get().Initialize(std::move(*this));
}

UiConfig& UiConfig::SetScalingFactor(float scalingFactor)
{
  GetImpl(*this).SetScalingFactor(scalingFactor);
  return *this;
}

float UiConfig::GetScalingFactor() const
{
  return GetImpl(*this).GetScalingFactor();
}

UiConfig& UiConfig::SetDpi(int dpi)
{
  GetImpl(*this).SetDpi(dpi);
  return *this;
}

int UiConfig::GetDpi() const
{
  return GetImpl(*this).GetDpi();
}

UiConfig& UiConfig::SetBaselineDpi(int baselineDpi)
{
  GetImpl(*this).SetBaselineDpi(baselineDpi);
  return *this;
}

int UiConfig::GetBaselineDpi() const
{
  return GetImpl(*this).GetBaselineDpi();
}

UiConfig& UiConfig::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetImpl(*this).SetKeyClickPolicy(policy);
  return *this;
}

KeyClickPolicy UiConfig::GetKeyClickPolicy() const
{
  return GetImpl(*this).GetKeyClickPolicy();
}

UiConfig& UiConfig::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  GetImpl(*this).SetExecutionKeyPredicate(predicate);
  return *this;
}

ExecutionKeyPredicate UiConfig::GetExecutionKeyPredicate() const
{
  return GetImpl(*this).GetExecutionKeyPredicate();
}

UiConfig& UiConfig::SetMinLongPressKeyCount(uint32_t count)
{
  GetImpl(*this).SetMinLongPressKeyCount(count);
  return *this;
}

uint32_t UiConfig::GetMinLongPressKeyCount() const
{
  return GetImpl(*this).GetMinLongPressKeyCount();
}

UiConfig& UiConfig::SetTapRecognizerTime(uint32_t timeMs)
{
  GetImpl(*this).SetTapRecognizerTime(timeMs);
  return *this;
}

uint32_t UiConfig::GetTapRecognizerTime() const
{
  return GetImpl(*this).GetTapRecognizerTime();
}

UiConfig& UiConfig::SetBrokenImageUrl(BrokenImageType brokenImageType, const std::string& brokenImageUrl)
{
  GetImpl(*this).SetBrokenImageUrl(brokenImageType, brokenImageUrl);
  return *this;
}

const std::string& UiConfig::GetBrokenImageUrl(BrokenImageType brokenImageType) const
{
  return GetImpl(*this).GetBrokenImageUrl(brokenImageType);
}

UiConfig& UiConfig::EnableFocusClearOnEscape(bool enable)
{
  GetImpl(*this).EnableFocusClearOnEscape(enable);
  return *this;
}

bool UiConfig::IsFocusClearOnEscapeEnabled() const
{
  return GetImpl(*this).IsFocusClearOnEscapeEnabled();
}

UiConfig& UiConfig::SetAlwaysShowFocus(bool alwaysShow)
{
  GetImpl(*this).SetAlwaysShowFocus(alwaysShow);
  return *this;
}

bool UiConfig::IsFocusIndicatorAlwaysShown() const
{
  return GetImpl(*this).IsFocusIndicatorAlwaysShown();
}

} // namespace Ui
} // namespace Dali
