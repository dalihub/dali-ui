#pragma once

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/slot-delegate.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/callback.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{

/**
 * @brief Internal implementation of UiLocalizationManager.
 *
 * Manages localized string lookups via dgettext/dngettext and maintains View bindings
 * so that resourceId/domain-based strings are automatically refreshed when
 * the locale or default domain changes.
 */
class DALI_UI_API UiLocalizationManagerImpl : public BaseObject
{
public:
  /**
   * @brief Returns the singleton UiLocalizationManager handle.
   *
   * Creates the implementation on first call. Returns an uninitialized
   * handle if called after the SingletonService has been destroyed.
   *
   * @return A UiLocalizationManager handle wrapping the singleton impl
   */
  static UiLocalizationManager Get();

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::RegisterDomain
   */
  bool RegisterDomain(StringView domain, StringView localePath);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::SetDefaultDomain
   */
  void SetDefaultDomain(StringView domain);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::GetDefaultDomain
   */
  Dali::String GetDefaultDomain() const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::GetLocalizedString(StringView) const
   */
  Dali::String GetLocalizedString(StringView resourceId) const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::GetLocalizedString(StringView, StringView) const
   */
  Dali::String GetLocalizedString(StringView resourceId, StringView domain) const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::GetLocalizedPluralString(StringView, StringView, uint32_t) const
   */
  Dali::String GetLocalizedPluralString(StringView resourceId,
                                        StringView pluralResourceId,
                                        uint32_t   quantity) const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::GetLocalizedPluralString(StringView, StringView, uint32_t, StringView) const
   */
  Dali::String GetLocalizedPluralString(StringView resourceId,
                                        StringView pluralResourceId,
                                        uint32_t   quantity,
                                        StringView domain) const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::SetBindingResource(BaseHandle, StringView, StringView, LocalizedStringCallback)
   */
  void SetBindingResource(BaseHandle              target,
                          StringView              bindingId,
                          StringView              resourceId,
                          LocalizedStringCallback callback);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::SetBindingResource(BaseHandle, StringView, StringView, StringView, LocalizedStringCallback)
   */
  void SetBindingResource(BaseHandle              target,
                          StringView              bindingId,
                          StringView              resourceId,
                          StringView              domain,
                          LocalizedStringCallback callback);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::HasBinding
   */
  [[nodiscard]] bool HasBinding(BaseHandle target,
                                StringView bindingId) const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::ClearBinding
   */
  void ClearBinding(BaseHandle target,
                    StringView bindingId);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::ClearBindings(BaseHandle)
   */
  void ClearBindings(BaseHandle target);

  /**
   * @brief Removes all bindings associated with a raw object pointer.
   *
   * This is for internal destructor usage where Self() may not be available.
   * The pointer must match BaseHandle::GetObjectPtr() used at registration.
   *
   * @param[in] objectPtr The target object pointer to unbind, or nullptr (no-op)
   */
  void ClearBindings(void* objectPtr);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::SetLocalizedStringOverride
   */
  void SetLocalizedStringOverride(LocalizedStringOverrideFunc func);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::ClearLocalizedStringOverride
   */
  void ClearLocalizedStringOverride();

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::SetBypassEnabled
   */
  void SetBypassEnabled(bool enabled);

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::IsBypassEnabled
   */
  bool IsBypassEnabled() const;

  /**
   * @copydoc Dali::Ui::UiLocalizationManager::RefreshBindings
   */
  void RefreshBindings();

protected:
  /**
   * @brief Constructs a new UiLocalizationManagerImpl.
   */
  UiLocalizationManagerImpl();

  /**
   * @brief Destructor.
   */
  ~UiLocalizationManagerImpl() override;

private:
  UiLocalizationManagerImpl(const UiLocalizationManagerImpl&)            = delete;
  UiLocalizationManagerImpl(UiLocalizationManagerImpl&&)                 = delete;
  UiLocalizationManagerImpl& operator=(const UiLocalizationManagerImpl&) = delete;
  UiLocalizationManagerImpl& operator=(UiLocalizationManagerImpl&&)      = delete;

private:
  /**
   * @brief Stores callback and localization info for a single binding.
   */
  struct BindingInfo
  {
    LocalizedStringCallback applyFunc;
    std::string             resourceId;
    std::string             domain;               ///< empty means use current default domain
    bool                    hasResourceId{false}; ///< true once resourceId has been explicitly set
  };

  /**
   * @brief Stores all bindings for a single target object.
   */
  struct ViewBinding
  {
    WeakHandle<BaseHandle>                           weakTarget;
    std::vector<std::pair<std::string, BindingInfo>> bindings;
  };

  /**
   * @brief Snapshot entry used during RefreshBindings to avoid iterator invalidation.
   */
  struct BindingSnapshot
  {
    RefObject*  targetPtr{nullptr};
    std::string bindingId;
  };

private:
  /// @brief Connects to the platform locale changed signal if not already connected.
  void EnsureLocaleSignalConnected();

  /// @brief Handler for locale changed signal.
  void OnLocaleChanged(std::string locale);

  /// @brief Gets or creates a ViewBinding entry for the given target.
  ViewBinding& GetOrCreateViewBinding(BaseHandle target);

  /// @brief Finds a BindingInfo for the given target and bindingId.
  BindingInfo*       FindBinding(BaseHandle target, StringView bindingId);
  const BindingInfo* FindBinding(BaseHandle target, StringView bindingId) const;

  /// @brief Finds a BindingInfo by raw pointer and string bindingId.
  BindingInfo*       FindBinding(RefObject* targetPtr, const std::string& bindingId);
  const BindingInfo* FindBinding(RefObject* targetPtr, const std::string& bindingId) const;

  /// @brief Applies a single binding if not currently in a refresh cycle.
  void ApplyBindingIfPossible(BaseHandle target, StringView bindingId);
  void ApplyBindingIfPossible(RefObject* targetPtr, const std::string& bindingId);

  /**
   * @brief Gets a target handle from the stored weak target.
   *
   * Returns an empty handle if the entry does not exist or the weak target has expired.
   *
   * @param[in] targetPtr Raw object pointer used as the binding map key
   * @return A temporary target handle for callback invocation, or empty
   */
  BaseHandle GetTargetHandle(RefObject* targetPtr) const;

  /// @brief Applies a binding by looking up the localized string and invoking the callback.
  void ApplyBinding(BaseHandle target, BindingInfo& info);

  /// @brief Builds a snapshot of all current bindings for safe iteration during refresh.
  std::vector<BindingSnapshot> BuildRefreshSnapshot();

  /// @brief Internal lookup implementation with full fallback chain.
  Dali::String GetLocalizedStringInternal(StringView resourceId,
                                          StringView domain) const;

  /// @brief Internal plural lookup implementation with its fallback chain.
  Dali::String GetLocalizedPluralStringInternal(StringView resourceId,
                                                StringView pluralResourceId,
                                                uint32_t   quantity,
                                                StringView domain) const;

  /// @brief Determines the effective domain, falling back to mDefaultDomain if empty.
  std::string GetEffectiveDomain(StringView domain) const;

private:
  std::unordered_map<RefObject*, ViewBinding>  mBindings;
  std::unordered_map<std::string, std::string> mRegisteredDomains;

  std::string                 mDefaultDomain;
  LocalizedStringOverrideFunc mLocalizedStringOverride{nullptr};

  SlotDelegate<UiLocalizationManagerImpl> mSlotDelegate{this};

  bool mBypassEnabled{false};
  bool mIsApplying{false};
  bool mPendingRefresh{false};
  bool mConnectedToLocaleSignal{false};
};

} // namespace Internal

/**
 * @brief Retrieves the UiLocalizationManagerImpl from a UiLocalizationManager handle.
 *
 * @param[in] obj The UiLocalizationManager handle
 * @return A reference to the internal implementation
 */
inline Internal::UiLocalizationManagerImpl& GetImpl(UiLocalizationManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Internal::UiLocalizationManagerImpl&>(handle);
}

/**
 * @brief Retrieves the UiLocalizationManagerImpl from a const UiLocalizationManager handle.
 *
 * @param[in] obj The UiLocalizationManager handle
 * @return A const reference to the internal implementation
 */
inline const Internal::UiLocalizationManagerImpl& GetImpl(const UiLocalizationManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Internal::UiLocalizationManagerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
