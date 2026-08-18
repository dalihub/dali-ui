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
#include <dali-ui-foundation/internal/ui-localization-manager-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali/devel-api/common/singleton-service.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/string-utils.h>

#include <dali-ui-foundation/internal/localization/gettext-wrapper.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace
{

struct ApplyingGuard
{
  bool& mFlag;

  explicit ApplyingGuard(bool& flag)
  : mFlag(flag)
  {
    mFlag = true;
  }

  ~ApplyingGuard()
  {
    mFlag = false;
  }
};

} // unnamed namespace

UiLocalizationManagerImpl::UiLocalizationManagerImpl() = default;

UiLocalizationManagerImpl::~UiLocalizationManagerImpl() = default;

UiLocalizationManager UiLocalizationManagerImpl::Get()
{
  UiLocalizationManager manager;

  SingletonService service(SingletonService::Get());
  if(service)
  {
    // Check whether the singleton is already created
    BaseHandle handle = service.GetSingleton(typeid(UiLocalizationManager));
    if(handle)
    {
      // If so, downcast the handle
      manager = UiLocalizationManager(static_cast<UiLocalizationManagerImpl*>(handle.GetObjectPtr()));
    }
    else
    {
      manager = UiLocalizationManager(new UiLocalizationManagerImpl());
      service.Register(typeid(manager), manager);
    }
  }

  return manager;
}

// ---- Domain management ----

bool UiLocalizationManagerImpl::RegisterDomain(StringView domainView, StringView localePathView)
{
  std::string domain     = ToStdString(domainView);
  std::string localePath = ToStdString(localePathView);

  if(domain.empty() || localePath.empty())
  {
    return false;
  }

  // Do not pass StringView::Data() directly to gettext.
  // StringView may not be null-terminated. Use std::string::c_str().
  const char* result = Localization::BindTextDomain(domain.c_str(), localePath.c_str());
  if(result == nullptr)
  {
    return false;
  }

  Localization::BindTextDomainCodeset(domain.c_str(), "UTF-8");

  mRegisteredDomains[domain] = localePath;

  RefreshBindings();
  return true;
}

void UiLocalizationManagerImpl::SetDefaultDomain(StringView domain)
{
  std::string newDomain = ToStdString(domain);

  if(mDefaultDomain == newDomain)
  {
    return;
  }

  mDefaultDomain = newDomain;
  RefreshBindings();
}

Dali::String UiLocalizationManagerImpl::GetDefaultDomain() const
{
  return ToDaliString(mDefaultDomain);
}

// ---- Localized string lookup ----

std::string UiLocalizationManagerImpl::GetEffectiveDomain(StringView domain) const
{
  if(domain.Size() > 0)
  {
    return ToStdString(domain);
  }

  return mDefaultDomain;
}

Dali::String UiLocalizationManagerImpl::GetLocalizedString(StringView resourceId) const
{
  return GetLocalizedStringInternal(resourceId, StringView());
}

Dali::String UiLocalizationManagerImpl::GetLocalizedString(StringView resourceId, StringView domain) const
{
  return GetLocalizedStringInternal(resourceId, domain);
}

Dali::String UiLocalizationManagerImpl::GetLocalizedPluralString(StringView resourceId,
                                                                 StringView pluralResourceId,
                                                                 uint32_t   quantity) const
{
  return GetLocalizedPluralStringInternal(resourceId, pluralResourceId, quantity, StringView());
}

Dali::String UiLocalizationManagerImpl::GetLocalizedPluralString(StringView resourceId,
                                                                 StringView pluralResourceId,
                                                                 uint32_t   quantity,
                                                                 StringView domain) const
{
  return GetLocalizedPluralStringInternal(resourceId, pluralResourceId, quantity, domain);
}

Dali::String UiLocalizationManagerImpl::GetLocalizedStringInternal(StringView resourceIdView,
                                                                   StringView domainView) const
{
  // 1. Empty resourceId -> empty string
  if(resourceIdView.Size() == 0 || resourceIdView.Data() == nullptr)
  {
    return Dali::String();
  }

  // 2. Bypass enabled -> return resourceId as-is
  if(mBypassEnabled)
  {
    return Dali::String(resourceIdView);
  }

  // 3. Determine effective domain
  std::string resourceId      = ToStdString(resourceIdView);
  std::string effectiveDomain = GetEffectiveDomain(domainView);

  // 4. Try override function first
  if(mLocalizedStringOverride)
  {
    Dali::String overridden;

    StringView effectiveDomainView;
    if(!effectiveDomain.empty())
    {
      effectiveDomainView = StringView(effectiveDomain.c_str(), static_cast<uint32_t>(effectiveDomain.size()));
    }

    if(mLocalizedStringOverride(resourceIdView, effectiveDomainView, overridden))
    {
      return overridden;
    }
  }

  // 5. No effective domain -> return resourceId
  if(effectiveDomain.empty())
  {
    return ToDaliString(resourceId);
  }

  // 6. dgettext lookup.
  // Do not pass StringView::Data() directly to dgettext.
  // Always use null-terminated std::string::c_str().
  // If no translation is found, dgettext returns resourceId.
  const char* translated = Localization::GetText(effectiveDomain.c_str(), resourceId.c_str());

  if(translated == nullptr)
  {
    return ToDaliString(resourceId);
  }

  return Dali::String(translated);
}

Dali::String UiLocalizationManagerImpl::GetLocalizedPluralStringInternal(StringView resourceIdView,
                                                                         StringView pluralResourceIdView,
                                                                         uint32_t   quantity,
                                                                         StringView domainView) const
{
  // Both gettext source identifiers are required for plural lookup.
  if(resourceIdView.Size() == 0 || resourceIdView.Data() == nullptr ||
     pluralResourceIdView.Size() == 0 || pluralResourceIdView.Data() == nullptr)
  {
    return Dali::String();
  }

  // Bypass exposes the primary resource id without applying plural rules.
  if(mBypassEnabled)
  {
    return Dali::String(resourceIdView);
  }

  std::string resourceId       = ToStdString(resourceIdView);
  std::string pluralResourceId = ToStdString(pluralResourceIdView);
  std::string effectiveDomain  = GetEffectiveDomain(domainView);

  // LocalizedStringOverrideFunc has no plural id or quantity parameters and
  // therefore cannot represent a plural-aware lookup.
  if(effectiveDomain.empty())
  {
    return ToDaliString(resourceId);
  }

  // Always pass null-terminated storage to dngettext. Catalog-miss plural
  // selection is intentionally left to gettext.
  const char* translated = Localization::GetPluralText(effectiveDomain.c_str(),
                                                       resourceId.c_str(),
                                                       pluralResourceId.c_str(),
                                                       static_cast<unsigned long>(quantity));

  if(translated == nullptr)
  {
    return ToDaliString(resourceId);
  }

  return Dali::String(translated);
}

// ---- Binding registry ----

UiLocalizationManagerImpl::ViewBinding& UiLocalizationManagerImpl::GetOrCreateViewBinding(BaseHandle target)
{
  RefObject* targetPtr = target.GetObjectPtr();

  auto& viewBinding = mBindings[targetPtr];

  BaseHandle existing = viewBinding.weakTarget.GetBaseHandle();

  if(!existing)
  {
    // Important:
    // If a stale entry remains and a new object reuses the same address,
    // old callbacks must not be carried over.
    viewBinding.bindings.clear();
    viewBinding.weakTarget = WeakHandle<BaseHandle>(target);
  }

  return viewBinding;
}

UiLocalizationManagerImpl::BindingInfo* UiLocalizationManagerImpl::FindBinding(BaseHandle target,
                                                                               StringView bindingId)
{
  if(!target)
  {
    return nullptr;
  }

  RefObject* targetPtr = target.GetObjectPtr();
  return FindBinding(targetPtr, ToStdString(bindingId));
}

const UiLocalizationManagerImpl::BindingInfo* UiLocalizationManagerImpl::FindBinding(BaseHandle target,
                                                                                     StringView bindingId) const
{
  if(!target)
  {
    return nullptr;
  }

  RefObject* targetPtr = target.GetObjectPtr();
  return FindBinding(targetPtr, ToStdString(bindingId));
}

UiLocalizationManagerImpl::BindingInfo* UiLocalizationManagerImpl::FindBinding(RefObject*         targetPtr,
                                                                               const std::string& bindingId)
{
  auto it = mBindings.find(targetPtr);
  if(it == mBindings.end())
  {
    return nullptr;
  }

  for(auto& item : it->second.bindings)
  {
    if(item.first == bindingId)
    {
      return &item.second;
    }
  }

  return nullptr;
}

const UiLocalizationManagerImpl::BindingInfo* UiLocalizationManagerImpl::FindBinding(RefObject*         targetPtr,
                                                                                     const std::string& bindingId) const
{
  auto it = mBindings.find(targetPtr);
  if(it == mBindings.end())
  {
    return nullptr;
  }

  for(const auto& item : it->second.bindings)
  {
    if(item.first == bindingId)
    {
      return &item.second;
    }
  }

  return nullptr;
}

// ---- Binding registration ----

void UiLocalizationManagerImpl::SetBindingResource(BaseHandle              target,
                                                   StringView              bindingId,
                                                   StringView              resourceId,
                                                   LocalizedStringCallback callback)
{
  SetBindingResource(target, bindingId, resourceId, StringView(), std::move(callback));
}

void UiLocalizationManagerImpl::SetBindingResource(BaseHandle              target,
                                                   StringView              bindingIdView,
                                                   StringView              resourceIdView,
                                                   StringView              domainView,
                                                   LocalizedStringCallback callback)
{
  if(!target || bindingIdView.Size() == 0)
  {
    return;
  }

  EnsureLocaleSignalConnected();

  std::string bindingId  = ToStdString(bindingIdView);
  std::string resourceId = ToStdString(resourceIdView);
  std::string domain     = ToStdString(domainView);

  auto& viewBinding = GetOrCreateViewBinding(target);

  BindingInfo* targetInfo = nullptr;

  for(auto& item : viewBinding.bindings)
  {
    if(item.first == bindingId)
    {
      targetInfo = &item.second;
      break;
    }
  }

  if(!targetInfo)
  {
    BindingInfo info;
    viewBinding.bindings.emplace_back(bindingId, std::move(info));
    targetInfo = &viewBinding.bindings.back().second;
  }

  targetInfo->applyFunc     = std::move(callback);
  targetInfo->resourceId    = std::move(resourceId);
  targetInfo->domain        = std::move(domain);
  targetInfo->hasResourceId = true;

  // Apply only this binding immediately. Full refresh is reserved for
  // locale/default-domain/override/bypass changes or explicit RefreshBindings().
  ApplyBindingIfPossible(target.GetObjectPtr(), bindingId);
}

bool UiLocalizationManagerImpl::HasBinding(BaseHandle target,
                                           StringView bindingId) const
{
  if(!target)
  {
    return false;
  }

  return FindBinding(target, bindingId) != nullptr;
}

// ---- Binding removal ----

void UiLocalizationManagerImpl::ClearBinding(BaseHandle target,
                                             StringView bindingIdView)
{
  if(!target || bindingIdView.Size() == 0)
  {
    return;
  }

  RefObject* targetPtr = target.GetObjectPtr();

  auto it = mBindings.find(targetPtr);
  if(it == mBindings.end())
  {
    return;
  }

  std::string bindingId = ToStdString(bindingIdView);

  auto& bindings = it->second.bindings;
  bindings.erase(std::remove_if(bindings.begin(),
                                bindings.end(),
                                [&bindingId](const auto& entry)
  {
    return entry.first == bindingId;
  }),
                 bindings.end());

  if(bindings.empty())
  {
    mBindings.erase(it);
  }
}

void UiLocalizationManagerImpl::ClearBindings(BaseHandle target)
{
  if(!target)
  {
    return;
  }

  mBindings.erase(target.GetObjectPtr());
}

void UiLocalizationManagerImpl::ClearBindings(void* objectPtr)
{
  if(objectPtr == nullptr)
  {
    return;
  }

  mBindings.erase(static_cast<RefObject*>(objectPtr));
}

// ---- Override ----

void UiLocalizationManagerImpl::SetLocalizedStringOverride(LocalizedStringOverrideFunc func)
{
  if(mLocalizedStringOverride == func)
  {
    return;
  }

  mLocalizedStringOverride = func;
  RefreshBindings();
}

void UiLocalizationManagerImpl::ClearLocalizedStringOverride()
{
  SetLocalizedStringOverride(nullptr);
}

// ---- Bypass ----

void UiLocalizationManagerImpl::SetBypassEnabled(bool enabled)
{
  if(mBypassEnabled == enabled)
  {
    return;
  }

  mBypassEnabled = enabled;
  RefreshBindings();
}

bool UiLocalizationManagerImpl::IsBypassEnabled() const
{
  return mBypassEnabled;
}

// ---- Apply helpers ----

void UiLocalizationManagerImpl::ApplyBindingIfPossible(BaseHandle target,
                                                       StringView bindingId)
{
  if(!target)
  {
    return;
  }

  ApplyBindingIfPossible(target.GetObjectPtr(), ToStdString(bindingId));
}

void UiLocalizationManagerImpl::ApplyBindingIfPossible(RefObject*         targetPtr,
                                                       const std::string& bindingId)
{
  if(mIsApplying)
  {
    mPendingRefresh = true;
    return;
  }

  BaseHandle target = GetTargetHandle(targetPtr);
  if(!target)
  {
    // The weak target has expired or the entry was removed while applying.
    mBindings.erase(targetPtr);
    return;
  }

  BindingInfo* info = FindBinding(targetPtr, bindingId);
  if(!info)
  {
    return;
  }

  {
    ApplyingGuard guard(mIsApplying);
    ApplyBinding(target, *info);
  }

  if(mPendingRefresh)
  {
    mPendingRefresh = false;
    RefreshBindings();
  }
}

BaseHandle UiLocalizationManagerImpl::GetTargetHandle(RefObject* targetPtr) const
{
  auto it = mBindings.find(targetPtr);
  if(it == mBindings.end())
  {
    return BaseHandle();
  }

  return it->second.weakTarget.GetBaseHandle();
}

void UiLocalizationManagerImpl::ApplyBinding(BaseHandle target, BindingInfo& info)
{
  if(!target || !info.applyFunc || !info.hasResourceId)
  {
    return;
  }

  Dali::String localized;
  if(info.resourceId.empty())
  {
    localized = Dali::String();
  }
  else
  {
    StringView resourceView(info.resourceId.c_str(), static_cast<uint32_t>(info.resourceId.size()));

    StringView domainView;
    if(!info.domain.empty())
    {
      domainView = StringView(info.domain.c_str(), static_cast<uint32_t>(info.domain.size()));
    }

    localized = GetLocalizedStringInternal(resourceView, domainView);
  }

  // The callback may mutate or clear bindings, including this BindingInfo.
  // Do not access info after Invoke().
  info.applyFunc.Invoke(target, localized);
}

// ---- Refresh ----

std::vector<UiLocalizationManagerImpl::BindingSnapshot> UiLocalizationManagerImpl::BuildRefreshSnapshot()
{
  std::vector<BindingSnapshot> snapshot;

  auto it = mBindings.begin();
  while(it != mBindings.end())
  {
    BaseHandle handle = it->second.weakTarget.GetBaseHandle();
    if(!handle)
    {
      it = mBindings.erase(it);
      continue;
    }

    for(const auto& item : it->second.bindings)
    {
      BindingSnapshot entry;
      entry.targetPtr = it->first;
      entry.bindingId = item.first;
      snapshot.emplace_back(std::move(entry));
    }

    ++it;
  }

  return snapshot;
}

void UiLocalizationManagerImpl::RefreshBindings()
{
  if(mIsApplying)
  {
    mPendingRefresh = true;
    return;
  }

  ApplyingGuard guard(mIsApplying);

  // Build a snapshot first.
  // This prevents iterator invalidation if callbacks mutate bindings.
  std::vector<BindingSnapshot> snapshot = BuildRefreshSnapshot();

  for(const auto& entry : snapshot)
  {
    BaseHandle target = GetTargetHandle(entry.targetPtr);
    if(!target)
    {
      // The binding may have been cleared or the weak target may have expired.
      mBindings.erase(entry.targetPtr);
      continue;
    }

    // Binding may have been cleared or changed by a previous callback.
    // Always re-check current binding state.
    BindingInfo* current = FindBinding(entry.targetPtr, entry.bindingId);
    if(!current)
    {
      continue;
    }

    ApplyBinding(target, *current);
  }

  if(mPendingRefresh)
  {
    mPendingRefresh = false;
    RefreshBindings();
  }
}

// ---- Locale signal ----

void UiLocalizationManagerImpl::EnsureLocaleSignalConnected()
{
  if(mConnectedToLocaleSignal)
  {
    return;
  }

  if(!Dali::Adaptor::IsAvailable())
  {
    return;
  }

  Dali::Adaptor::Get().LocaleChangedSignal().Connect(
    mSlotDelegate,
    &UiLocalizationManagerImpl::OnLocaleChanged);

  mConnectedToLocaleSignal = true;
}

void UiLocalizationManagerImpl::OnLocaleChanged(std::string locale)
{
  // The platform locale manager owns setlocale/category update.
  // UiLocalizationManager only refreshes bindings so that dgettext
  // lookups use the updated locale.
  RefreshBindings();
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
