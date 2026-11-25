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

#include <dali-ui-foundation/integration-api/web-cookie-manager-impl.h>
#include <dali-ui-foundation/integration-api/web-profile-impl.h>
#include <dali-ui-foundation/integration-api/web-view-impl.h>
#include <dali-ui-foundation/internal/web-profile-registry.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-context.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

namespace
{
Internal::WebProfileRegistry& Profiles()
{
  static Internal::WebProfileRegistry profiles;
  return profiles;
}
} //namespace

WebProfile WebProfileImpl::New(const Dali::String& name, bool incognito, const Dali::String& bundlePath)
{
  DALI_ASSERT_ALWAYS(!name.Empty() && name != "Default" && "Profile name must be nonempty and not Default");
  DALI_ASSERT_ALWAYS(!GetProfile(name) && "A live profile already uses this name");
  if(!WebViewImpl::EnsureEngineType())
  {
    return {};
  }
  auto nativeProfile = Dali::WebEngineRuntime::CreateProfile(incognito, bundlePath.CStr());
  if(!nativeProfile)
  {
    return {};
  }
  WebProfile profile(*new WebProfileImpl(name, incognito, std::move(nativeProfile)));
  Internal::RegisterWebProfile(Profiles(), profile);
  return profile;
}

WebProfile WebProfileImpl::GetProfile(const Dali::String& name)
{
  auto& profiles = Profiles();
  auto  found    = profiles.find(name.CStr());
  if(found == profiles.end())
  {
    return {};
  }
  auto profile = found->second.GetHandle();
  if(!profile)
  {
    profiles.erase(found);
  }
  return profile;
}

WebProfile WebProfileImpl::GetDefaultProfile()
{
  auto profile = GetProfile("Default");
  if(profile)
  {
    return profile;
  }
  if(!WebViewImpl::EnsureEngineType())
  {
    return {};
  }
  auto nativeProfile = Dali::WebEngineRuntime::GetDefaultProfile();
  if(!nativeProfile)
  {
    return {};
  }
  profile = WebProfile(*new WebProfileImpl("Default", false, std::move(nativeProfile)));
  Internal::RegisterWebProfile(Profiles(), profile);
  return profile;
}

WebProfileImpl::WebProfileImpl(const Dali::String& name, bool incognito, Dali::WebEngineProfilePtr profile)
: mName(name),
  mIncognito(incognito),
  mProfile(std::move(profile))
{
}

WebProfileImpl::~WebProfileImpl() = default;

void WebProfileImpl::ClearCache()
{
  if(auto* context = mProfile->GetContext())
  {
    context->ClearCache();
  }
}

WebCookieManager WebProfileImpl::GetCookieManager() const
{
  auto cookies = mWebCookieManager.GetHandle();
  if(!cookies)
  {
    WebProfile profile(*const_cast<WebProfileImpl*>(this));
    auto       impl   = WebCookieManagerImpl::New(profile);
    cookies           = WebCookieManager(*impl);
    mWebCookieManager = Dali::WeakHandle<WebCookieManager>(cookies);
  }
  return cookies;
}

bool WebProfileImpl::IsEqual(const WebProfileImpl& rhs) const
{
  return mProfile == rhs.mProfile;
}

Dali::String WebProfileImpl::GetName() const
{
  return mName;
}
bool WebProfileImpl::IsIncognito() const
{
  return mIncognito;
}
const Dali::WebEngineProfilePtr& WebProfileImpl::GetEngineProfile() const
{
  return mProfile;
}

} // namespace Integration
} // namespace Ui
} // namespace DALI_NAMESPACE
