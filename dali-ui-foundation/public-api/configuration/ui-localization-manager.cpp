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
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/ui-localization-manager-impl.h>

namespace Dali
{
namespace Ui
{

UiLocalizationManager::UiLocalizationManager() = default;

UiLocalizationManager::UiLocalizationManager(Internal::UiLocalizationManagerImpl* impl)
: BaseHandle(impl)
{
}

UiLocalizationManager UiLocalizationManager::Get()
{
  return Internal::UiLocalizationManagerImpl::Get();
}

UiLocalizationManager UiLocalizationManager::DownCast(BaseHandle handle)
{
  return UiLocalizationManager(dynamic_cast<Internal::UiLocalizationManagerImpl*>(handle.GetObjectPtr()));
}

bool UiLocalizationManager::RegisterDomain(StringView domain, StringView localePath)
{
  return GetImpl(*this).RegisterDomain(domain, localePath);
}

void UiLocalizationManager::SetDefaultDomain(StringView domain)
{
  GetImpl(*this).SetDefaultDomain(domain);
}

Dali::String UiLocalizationManager::GetDefaultDomain() const
{
  return GetImpl(*this).GetDefaultDomain();
}

Dali::String UiLocalizationManager::GetLocalizedString(StringView resourceId) const
{
  return GetImpl(*this).GetLocalizedString(resourceId);
}

Dali::String UiLocalizationManager::GetLocalizedString(StringView resourceId, StringView domain) const
{
  return GetImpl(*this).GetLocalizedString(resourceId, domain);
}

Dali::String UiLocalizationManager::GetLocalizedPluralString(StringView resourceId,
                                                             StringView pluralResourceId,
                                                             uint32_t   quantity) const
{
  return GetImpl(*this).GetLocalizedPluralString(resourceId, pluralResourceId, quantity);
}

Dali::String UiLocalizationManager::GetLocalizedPluralString(StringView resourceId,
                                                             StringView pluralResourceId,
                                                             uint32_t   quantity,
                                                             StringView domain) const
{
  return GetImpl(*this).GetLocalizedPluralString(resourceId, pluralResourceId, quantity, domain);
}

void UiLocalizationManager::SetBindingResource(BaseHandle              target,
                                               StringView              bindingId,
                                               StringView              resourceId,
                                               LocalizedStringCallback callback)
{
  GetImpl(*this).SetBindingResource(target, bindingId, resourceId, std::move(callback));
}

void UiLocalizationManager::SetBindingResource(BaseHandle              target,
                                               StringView              bindingId,
                                               StringView              resourceId,
                                               StringView              domain,
                                               LocalizedStringCallback callback)
{
  GetImpl(*this).SetBindingResource(target, bindingId, resourceId, domain, std::move(callback));
}

bool UiLocalizationManager::HasBinding(BaseHandle target, StringView bindingId) const
{
  return GetImpl(*this).HasBinding(target, bindingId);
}

void UiLocalizationManager::ClearBinding(BaseHandle target, StringView bindingId)
{
  GetImpl(*this).ClearBinding(target, bindingId);
}

void UiLocalizationManager::ClearBindings(BaseHandle target)
{
  GetImpl(*this).ClearBindings(target);
}

void UiLocalizationManager::SetLocalizedStringOverride(LocalizedStringOverrideFunc func)
{
  GetImpl(*this).SetLocalizedStringOverride(func);
}

void UiLocalizationManager::ClearLocalizedStringOverride()
{
  GetImpl(*this).ClearLocalizedStringOverride();
}

void UiLocalizationManager::SetBypassEnabled(bool enabled)
{
  GetImpl(*this).SetBypassEnabled(enabled);
}

bool UiLocalizationManager::IsBypassEnabled() const
{
  return GetImpl(*this).IsBypassEnabled();
}

void UiLocalizationManager::RefreshBindings()
{
  GetImpl(*this).RefreshBindings();
}

} // namespace Ui
} // namespace Dali
