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
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/web-engine/web-engine-cookie-manager.h>
#include <dali/integration-api/debug.h>
#include <mutex>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration
{

namespace
{
Dali::WebEngineCookieManager* GetNativeCookieManager(const WebProfile& profile, const char* operation)
{
  if(auto* manager = profile ? GetImpl(profile).GetEngineProfile()->GetCookieManager() : nullptr)
  {
    return manager;
  }

  DALI_LOG_ERROR("WebCookieManager::%s: cookie store is unavailable\n", operation);
  return nullptr;
}
} // namespace

WebCookieManagerImplPtr WebCookieManagerImpl::New(const WebProfile& profile)
{
  return new WebCookieManagerImpl(profile);
}

WebCookieManagerImpl::WebCookieManagerImpl(const WebProfile& profile)
{
  auto handle = profile;
  mProfile    = Dali::WeakHandle<WebProfile>(handle);
}

struct WebCookieManagerImpl::WatchState
{
  std::mutex                           mutex;
  std::unique_ptr<EventThreadCallback> trigger;
};

WebCookieManagerImpl::~WebCookieManagerImpl() = default;

void WebCookieManagerImpl::DetachFromProfile(Dali::WebEngineCookieManager* manager)
{
  if(mWatch)
  {
    // A native notification can still hold the shared state after teardown.
    std::unique_ptr<EventThreadCallback> trigger;
    {
      std::lock_guard<std::mutex> lock(mWatch->mutex);
      trigger = std::move(mWatch->trigger);
    }
    if(manager)
    {
      manager->ChangesWatch({});
    }
    mWatch.reset();
  }
  mCookieChangedSignal.reset();
}

Extension::WebCookieManager::CookieChangedSignalType& WebCookieManagerImpl::CookieChangedSignal()
{
  auto  profile = mProfile.GetHandle();
  auto* manager = GetNativeCookieManager(profile, "CookieChangedSignal");
  if(!manager)
  {
    return mInactiveSignal;
  }

  if(!mWatch)
  {
    mCookieChangedSignal = std::make_unique<Extension::WebCookieManager::CookieChangedSignalType>();
    mWatch               = std::make_shared<WatchState>();
    mWatch->trigger      = std::make_unique<EventThreadCallback>(MakeCallback(this, &WebCookieManagerImpl::EmitCookieChanged));
    manager->ChangesWatch([watch = mWatch]()
    {
      std::lock_guard<std::mutex> lock(watch->mutex);
      if(watch->trigger)
      {
        watch->trigger->Trigger();
      }
    });
  }
  return *mCookieChangedSignal;
}

void WebCookieManagerImpl::EmitCookieChanged()
{
  // A slot can release the last external profile handle during Emit().
  auto profile = mProfile.GetHandle();
  if(profile && GetImpl(profile).GetEngineProfile()->GetCookieManager())
  {
    mCookieChangedSignal->Emit();
  }
}

void WebCookieManagerImpl::ClearAllCookies()
{
  auto profile = mProfile.GetHandle();
  if(auto* manager = GetNativeCookieManager(profile, "ClearAllCookies"))
  {
    manager->ClearCookies();
  }
}

void WebCookieManagerImpl::SetCookieAcceptPolicy(WebCookieAcceptPolicy policy)
{
  auto  profile = mProfile.GetHandle();
  auto* manager = GetNativeCookieManager(profile, "SetCookieAcceptPolicy");
  if(!manager)
  {
    return;
  }

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
  manager->SetCookieAcceptPolicy(nativePolicy);
}

void WebCookieManagerImpl::SetPersistentStorage(const Dali::String& directory)
{
  auto profile = mProfile.GetHandle();
  if(auto* manager = GetNativeCookieManager(profile, "SetPersistentStorage"); manager && !profile.IsIncognito())
  {
    manager->SetPersistentStorage(directory.CStr(), Dali::WebEngineCookieManager::CookiePersistentStorage::SQLITE);
  }
}

bool WebCookieManagerImpl::IsEqual(const WebCookieManagerImpl& rhs) const
{
  if(this == &rhs)
  {
    return true;
  }

  auto profile = mProfile.GetHandle();
  return profile && profile == rhs.mProfile.GetHandle();
}

} // namespace Integration
} // namespace Ui
} //namespace DALI_NAMESPACE
