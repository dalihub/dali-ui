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
#include <dali-ui-foundation/public-api/ui-config.h>
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>

namespace Dali
{

namespace UI
{

UIConfig::UIConfig(Integration::UIConfigImpl* impl)
  : BaseHandle(impl)
{
}

UIConfig UIConfig::New()
{
  return Integration::UIConfigImpl::New();
}

UIConfig UIConfig::DownCast(BaseHandle handle)
{
  return UIConfig(dynamic_cast<Integration::UIConfigImpl*>(handle.GetObjectPtr()));
}

void UIConfig::Apply()
{
  Integration::UIConfigManager::Get().Initialize(std::move(*this));
}

UIConfig& UIConfig::SetScalingFactor(float scalingFactor)
{
  GetImpl(*this).SetScalingFactor(scalingFactor);
  return *this;
}

float UIConfig::GetScalingFactor() const
{
  return GetImpl(*this).GetScalingFactor();
}

UIConfig& UIConfig::SetDpi(int dpi)
{
  GetImpl(*this).SetDpi(dpi);
  return *this;
}

int UIConfig::GetDpi() const
{
  return GetImpl(*this).GetDpi();
}

UIConfig& UIConfig::SetBaselineDpi(int baselineDpi)
{
  GetImpl(*this).SetBaselineDpi(baselineDpi);
  return *this;
}

int UIConfig::GetBaselineDpi() const
{
  return GetImpl(*this).GetBaselineDpi();
}

UIConfig& UIConfig::SetKeyClickPolicy(KeyClickPolicy policy)
{
  GetImpl(*this).SetKeyClickPolicy(policy);
  return *this;
}

KeyClickPolicy UIConfig::GetKeyClickPolicy() const
{
  return GetImpl(*this).GetKeyClickPolicy();
}

UIConfig& UIConfig::SetExecutionKeyPredicate(ExecutionKeyPredicate predicate)
{
  GetImpl(*this).SetExecutionKeyPredicate(predicate);
  return *this;
}

ExecutionKeyPredicate UIConfig::GetExecutionKeyPredicate() const
{
  return GetImpl(*this).GetExecutionKeyPredicate();
}

UIConfig& UIConfig::SetMinLongPressKeyCount(uint32_t count)
{
  GetImpl(*this).SetMinLongPressKeyCount(count);
  return *this;
}

uint32_t UIConfig::GetMinLongPressKeyCount() const
{
  return GetImpl(*this).GetMinLongPressKeyCount();
}

UIConfig& UIConfig::SetTapRecognizerTime(uint32_t timeMs)
{
  GetImpl(*this).SetTapRecognizerTime(timeMs);
  return *this;
}

uint32_t UIConfig::GetTapRecognizerTime() const
{
  return GetImpl(*this).GetTapRecognizerTime();
}

} // namespace UI
} // namespace Dali
