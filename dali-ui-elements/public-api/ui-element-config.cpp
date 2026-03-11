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
#include <dali-ui-elements/public-api/ui-element-config.h>
#include <dali-ui-elements/integration-api/ui-element-config-impl.h>

namespace Dali
{

namespace Ui
{

UiElementConfig::UiElementConfig(Integration::UiElementConfigImpl* impl)
  : UiConfig(impl)
{
}

UiElementConfig UiElementConfig::New()
{
  Integration::UiElementConfigImplPtr impl = Integration::UiElementConfigImpl::New();

  // Pass ownership to handle
  return UiElementConfig(impl.Get());
}

UiElementConfig UiElementConfig::DownCast(BaseHandle handle)
{
  return UiElementConfig(dynamic_cast<Integration::UiElementConfigImpl*>(handle.GetObjectPtr()));
}

UiElementConfig& UiElementConfig::SetDefaultFontSize(float fontSize)
{
  GetImpl(*this).SetDefaultFontSize(fontSize);
  return *this;
}

float UiElementConfig::GetDefaultFontSize() const
{
  return GetImpl(*this).GetDefaultFontSize();
}

UiElementConfig& UiElementConfig::SetDefaultTextColor(const Vector4& color)
{
  GetImpl(*this).SetDefaultTextColor(color);
  return *this;
}

Vector4 UiElementConfig::GetDefaultTextColor() const
{
  return GetImpl(*this).GetDefaultTextColor();
}

} // namespace Ui
} // namespace Dali
