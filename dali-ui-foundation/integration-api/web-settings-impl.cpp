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
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-settings.h>
#include <utility>

namespace Dali
{
namespace Ui
{
namespace Integration
{

WebSettingsImplPtr WebSettingsImpl::New(Dali::WebEngine webEngine)
{
  return new WebSettingsImpl(std::move(webEngine));
}

WebSettingsImpl::WebSettingsImpl(Dali::WebEngine webEngine)
: mWebEngine(webEngine)
{
}

WebSettingsImpl::~WebSettingsImpl() = default;

bool WebSettingsImpl::SetExtraFeatureValue(const Dali::String& feature, const Dali::String& value)
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  if(!webEngine || !webEngine.GetPlugin())
  {
    return false;
  }

  return webEngine.GetSettings().SetExtraFeatureValue(feature.CStr(), value.CStr());
}

Dali::String WebSettingsImpl::GetExtraFeatureValue(const Dali::String& feature) const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  if(!webEngine || !webEngine.GetPlugin())
  {
    return Dali::String();
  }

  const std::string value = webEngine.GetSettings().GetExtraFeatureValue(feature.CStr());
  return Dali::String(value.c_str());
}

void WebSettingsImpl::SetSpatialNavigationEnabled(bool enabled)
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  if(webEngine && webEngine.GetPlugin())
  {
    webEngine.GetSettings().EnableSpatialNavigation(enabled);
  }
}

uint32_t WebSettingsImpl::GetDefaultFontSize() const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? webEngine.GetSettings().GetDefaultFontSize() : 0u;
}

void WebSettingsImpl::SetDefaultFontSize(uint32_t defaultFontSize)
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  if(webEngine && webEngine.GetPlugin())
  {
    webEngine.GetSettings().SetDefaultFontSize(defaultFontSize);
  }
}

void WebSettingsImpl::SetWebSecurityEnabled(bool enabled)
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  if(webEngine && webEngine.GetPlugin())
  {
    webEngine.GetSettings().EnableWebSecurity(enabled);
  }
}

void WebSettingsImpl::SetExtraFeatureEnabled(const Dali::String& feature, bool enabled)
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  if(webEngine && webEngine.GetPlugin())
  {
    webEngine.GetSettings().SetExtraFeature(feature.CStr(), enabled);
  }
}

bool WebSettingsImpl::IsExtraFeatureEnabled(const Dali::String& feature) const
{
  Dali::WebEngine webEngine = mWebEngine.GetHandle();
  return (webEngine && webEngine.GetPlugin()) ? webEngine.GetSettings().IsExtraFeatureEnabled(feature.CStr()) : false;
}

} // namespace Integration
} // namespace Ui
} // namespace Dali
