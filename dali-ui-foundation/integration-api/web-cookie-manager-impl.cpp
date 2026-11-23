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
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-cookie-manager.h>
#include <mutex>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

WebCookieManagerImplPtr WebCookieManagerImpl::New(const WebProfile& profile)
{
  return new WebCookieManagerImpl(profile);
}

WebCookieManagerImpl::WebCookieManagerImpl(const WebProfile& profile)
: mProfile(profile)
{
}

struct WebCookieManagerImpl::WatchState
{
  std::mutex                                               mutex;
  std::shared_ptr<WebCookieManager::CookieChangesCallback> callback;
};

WebCookieManagerImpl::~WebCookieManagerImpl()
{
  if(mWatch)
  {
    SetCookieChangesCallback({});
  }
}

void WebCookieManagerImpl::SetCookieChangesCallback(WebCookieManager::CookieChangesCallback callback)
{
  if(mWatch)
  {
    std::shared_ptr<WebCookieManager::CookieChangesCallback> previous;
    {
      std::lock_guard<std::mutex> lock(mWatch->mutex);
      previous = std::move(mWatch->callback);
    }
    mWatch.reset();
  }
  if(auto* manager = GetImpl(mProfile).GetEngineProfile()->GetCookieManager())
  {
    if(callback)
    {
      auto watch      = std::make_shared<WatchState>();
      watch->callback = std::make_shared<WebCookieManager::CookieChangesCallback>(std::move(callback));
      mWatch          = watch;
      manager->ChangesWatch([watch]()
      {
        std::shared_ptr<WebCookieManager::CookieChangesCallback> current;
        {
          std::lock_guard<std::mutex> lock(watch->mutex);
          current = watch->callback;
        }
        if(current)
        {
          current->Invoke();
        }
      });
    }
    else
    {
      manager->ChangesWatch({});
    }
  }
}

void WebCookieManagerImpl::ClearAllCookies()
{
  if(auto* cookieManager = GetImpl(mProfile).GetEngineProfile()->GetCookieManager())
  {
    cookieManager->ClearCookies();
  }
}

void WebCookieManagerImpl::SetCookieAcceptPolicy(WebCookieAcceptPolicy policy)
{
  Dali::WebEngineCookieManager::CookieAcceptPolicy nativePolicy;
  switch(policy)
  {
    case WebCookieAcceptPolicy::ALWAYS:
      nativePolicy = Dali::WebEngineCookieManager::CookieAcceptPolicy::ALWAYS;
      break;
    case WebCookieAcceptPolicy::NEVER:
      nativePolicy = Dali::WebEngineCookieManager::CookieAcceptPolicy::NEVER;
      break;
    case WebCookieAcceptPolicy::BLOCK_THIRD_PARTY:
      nativePolicy = Dali::WebEngineCookieManager::CookieAcceptPolicy::NO_THIRD_PARTY;
      break;
    default:
      DALI_ASSERT_ALWAYS(false && "Invalid cookie acceptance policy");
      return;
  }
  if(auto* manager = GetImpl(mProfile).GetEngineProfile()->GetCookieManager())
  {
    manager->SetCookieAcceptPolicy(nativePolicy);
  }
}

void WebCookieManagerImpl::SetPersistentStorage(const Dali::String& path)
{
  if(mProfile.IsIncognito())
  {
    return;
  }
  if(auto* manager = GetImpl(mProfile).GetEngineProfile()->GetCookieManager())
  {
    manager->SetPersistentStorage(path.CStr(), Dali::WebEngineCookieManager::CookiePersistentStorage::SQLITE);
  }
}

bool WebCookieManagerImpl::IsEqual(const WebCookieManagerImpl& rhs) const
{
  return mProfile == rhs.mProfile;
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
