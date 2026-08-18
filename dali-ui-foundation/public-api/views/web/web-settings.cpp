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

#include <dali-ui-foundation/integration-api/web-settings-impl.h>
#include <dali-ui-foundation/public-api/views/web/web-settings.h>

namespace Dali
{
namespace Ui
{

WebSettings::WebSettings()  = default;
WebSettings::~WebSettings() = default;

WebSettings::WebSettings(const WebSettings& settings)                = default;
WebSettings::WebSettings(WebSettings&& settings) noexcept            = default;
WebSettings& WebSettings::operator=(const WebSettings& settings)     = default;
WebSettings& WebSettings::operator=(WebSettings&& settings) noexcept = default;

WebSettings WebSettings::DownCast(BaseHandle handle)
{
  auto* impl = dynamic_cast<Integration::WebSettingsImpl*>(handle.GetObjectPtr());
  return impl ? WebSettings(*impl) : WebSettings();
}

bool WebSettings::SetExtraFeatureValue(const Dali::String& feature, const Dali::String& value)
{
  return Integration::GetImpl(*this).SetExtraFeatureValue(feature, value);
}

Dali::String WebSettings::GetExtraFeatureValue(const Dali::String& feature) const
{
  return Integration::GetImpl(*this).GetExtraFeatureValue(feature);
}

void WebSettings::SetSpatialNavigationEnabled(bool enabled)
{
  Integration::GetImpl(*this).SetSpatialNavigationEnabled(enabled);
}

uint32_t WebSettings::GetDefaultFontSize() const
{
  return Integration::GetImpl(*this).GetDefaultFontSize();
}

void WebSettings::SetDefaultFontSize(uint32_t defaultFontSize)
{
  Integration::GetImpl(*this).SetDefaultFontSize(defaultFontSize);
}

void WebSettings::SetWebSecurityEnabled(bool enabled)
{
  Integration::GetImpl(*this).SetWebSecurityEnabled(enabled);
}

void WebSettings::SetExtraFeatureEnabled(const Dali::String& feature, bool enabled)
{
  Integration::GetImpl(*this).SetExtraFeatureEnabled(feature, enabled);
}

bool WebSettings::IsExtraFeatureEnabled(const Dali::String& feature) const
{
  return Integration::GetImpl(*this).IsExtraFeatureEnabled(feature);
}

WebSettings::WebSettings(Integration::WebSettingsImpl& implementation)
: BaseHandle(&implementation)
{
}

} // namespace Ui
} // namespace Dali
