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
#include <dali/public-api/object/base-handle.h>

#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/callback.h>

namespace Dali
{
namespace Ui
{

namespace Internal
{
class UiLocalizationManagerImpl;
}

/**
 * @brief Function pointer type for overriding localized string lookups.
 *
 * This callback is invoked before the default gettext/dgettext lookup used by
 * GetLocalizedString() and localization bindings. It is not invoked by
 * GetLocalizedPluralString().
 *
 * If the function returns true:
 *   - outString is used as the final localized string.
 *   - dgettext() is skipped.
 *
 * If the function returns false:
 *   - normal gettext/dgettext lookup proceeds.
 *
 * The returned localized string may be display text, a resource path,
 * an image URL, placeholder text, tooltip text, or any other
 * locale-dependent string value.
 *
 * @note This is a plain function pointer, not std::function.
 *       Capturing lambdas and non-static member functions are not accepted.
 *       Use a free function or a static function. If state is required,
 *       route through an application singleton/resource manager.
 *
 * @note resourceId and domain StringViews are only valid during the call.
 *       The override implementation must copy them if it needs to store them.
 *
 * @param[in] resourceId The localization resource id / msgid
 * @param[in] domain The effective gettext domain. If a binding was created
 *                   without explicit domain, this is the current default domain.
 * @param[out] outString The overridden localized string
 * @return true if outString should be used, false to fall back to dgettext
 */
using LocalizedStringOverrideFunc = bool (*)(StringView    resourceId,
                                             StringView    domain,
                                             Dali::String& outString);

/**
 * @brief Typed callback for applying a localized string to a target object.
 *
 * This callback is used by localization bindings. The manager only resolves
 * resourceId/domain into a localized string; the callback decides how to apply it.
 *
 * The first argument is the target object that was registered with
 * SetBindingResource(). The handle is obtained from the manager's weak target
 * reference immediately before invocation.
 *
 * @note The target handle passed to the callback is intended for use during
 *       the callback. Do not store it unless you intentionally want to keep
 *       the target alive.
 * @note If created from a member function, the callback owner must outlive
 *       the binding.
 *
 * Examples:
 *   - Label text
 *   - ImageView image URL
 *   - placeholder text
 *   - tooltip text
 */
using LocalizedStringCallback = Callback<void(BaseHandle, const Dali::String&)>;

/**
 * @brief Provides public access to localized string lookup and binding.
 *
 * UiLocalizationManager is a Handle-based singleton.
 *
 * It supports:
 *   - gettext domain registration
 *   - default domain
 *   - dgettext/dngettext-based lookup
 *   - localized string override hook for non-plural lookup
 *   - bypass mode
 *   - View/BaseHandle binding refresh on locale change
 *
 * The core model is:
 *
 *   resourceId + domain -> localized string
 *
 * The localized string can be used as display text, resource path, image URL,
 * placeholder text, tooltip text, etc.
 *
 * @code
 *   // Register a gettext domain and set it as default
 *   UiLocalizationManager manager = UiLocalizationManager::Get();
 *   manager.RegisterDomain("myapp", "/usr/share/locale");
 *   manager.SetDefaultDomain("myapp");
 *
 *   // Direct lookup
 *   Dali::String title = manager.GetLocalizedString("IDS_TITLE");
 *
 *   // Binding: automatically refreshes on locale change
 *   manager.SetBindingResource(label, "Text", "IDS_TITLE",
 *       LocalizedStringCallback::New(this, &MyImpl::SetLabelText));
 *
 *   // Callback signature: void(BaseHandle target, const Dali::String& text)
 * @endcode
 */
class DALI_UI_API UiLocalizationManager : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized UiLocalizationManager handle.
   */
  UiLocalizationManager();

  /**
   * @brief Destructor.
   */
  ~UiLocalizationManager() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UiLocalizationManager(const UiLocalizationManager& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UiLocalizationManager(UiLocalizationManager&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UiLocalizationManager& operator=(const UiLocalizationManager& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UiLocalizationManager& operator=(UiLocalizationManager&& rhs) noexcept = default;

  /**
   * @brief Returns the singleton UiLocalizationManager instance.
   *
   * Creates the instance on first call.
   *
   * @return A handle to the UiLocalizationManager singleton
   */
  static UiLocalizationManager Get();

  /**
   * @brief Downcasts a handle to a UiLocalizationManager handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to UiLocalizationManager or an uninitialized handle
   */
  static UiLocalizationManager DownCast(BaseHandle handle);

  /**
   * @brief Registers a gettext domain.
   *
   * This calls bindtextdomain(domain, localePath) internally and sets
   * the domain codeset to UTF-8.
   *
   * Re-registering the same domain updates the locale path and refreshes
   * existing bindings.
   *
   * @note This does not automatically set the default domain.
   *       Applications should explicitly call SetDefaultDomain().
   *       Dali UI internal/framework domains should generally be used
   *       explicitly and should not rely on the app default domain.
   *
   * @param[in] domain The gettext domain name
   * @param[in] localePath The root locale directory
   * @return true if the domain was registered successfully
   */
  bool RegisterDomain(StringView domain, StringView localePath);

  /**
   * @brief Sets the default domain.
   *
   * Bindings created without explicit domain use the current default domain.
   * Changing the default domain refreshes all existing bindings.
   *
   * @param[in] domain The default gettext domain
   */
  void SetDefaultDomain(StringView domain);

  /**
   * @brief Gets the current default domain.
   *
   * @return The default domain, or an empty string if not set
   */
  Dali::String GetDefaultDomain() const;

  /**
   * @brief Gets a localized string using the default domain.
   *
   * Fallback behavior:
   *   - empty resourceId -> empty string
   *   - bypass enabled -> resourceId
   *   - override returns true -> override result
   *   - no effective domain -> resourceId
   *   - dgettext result -> translated string
   *   - dgettext null result -> resourceId
   *
   * @param[in] resourceId The localization resource id / msgid
   * @return The localized string or fallback resourceId
   */
  Dali::String GetLocalizedString(StringView resourceId) const;

  /**
   * @brief Gets a localized string using an explicit domain.
   *
   * @param[in] resourceId The localization resource id / msgid
   * @param[in] domain The gettext domain
   * @return The localized string or fallback resourceId
   */
  Dali::String GetLocalizedString(StringView resourceId, StringView domain) const;

  /**
   * @brief Gets a localized plural string using the default domain.
   *
   * Uses dngettext() to select a localized plural form according to the
   * specified cardinal quantity and the plural rules of the message catalog.
   * The @a resourceId and @a pluralResourceId parameters correspond to
   * gettext msgid and msgid_plural, respectively.
   *
   * Fallback behavior:
   *   - empty resourceId or pluralResourceId -> empty string
   *   - bypass enabled -> resourceId
   *   - no effective domain -> resourceId
   *   - dngettext result -> selected string
   *   - dngettext null result -> resourceId
   *
   * @note The @a quantity is used to select the plural form. It is not
   *       substituted into the returned string.
   * @note LocalizedStringOverrideFunc is not applied to plural lookup.
   *
   * @param[in] resourceId The localization resource id / gettext msgid
   * @param[in] pluralResourceId The plural localization resource id / gettext msgid_plural
   * @param[in] quantity The cardinal quantity used to select the plural form
   * @return The localized plural string or fallback resource id
   */
  Dali::String GetLocalizedPluralString(StringView resourceId,
                                        StringView pluralResourceId,
                                        uint32_t   quantity) const;

  /**
   * @brief Gets a localized plural string using an explicit domain.
   *
   * Uses dngettext() to select a localized plural form according to the
   * specified cardinal quantity and the plural rules of the message catalog.
   * The @a resourceId and @a pluralResourceId parameters correspond to
   * gettext msgid and msgid_plural, respectively.
   *
   * Passing an empty @a domain uses the current default domain. If there is
   * no effective domain, @a resourceId is returned.
   * Empty-id, bypass, dngettext-result, and null-result handling are the same
   * as for the default-domain overload.
   *
   * @note The @a quantity is used to select the plural form. It is not
   *       substituted into the returned string.
   * @note LocalizedStringOverrideFunc is not applied to plural lookup.
   *
   * @param[in] resourceId The localization resource id / gettext msgid
   * @param[in] pluralResourceId The plural localization resource id / gettext msgid_plural
   * @param[in] quantity The cardinal quantity used to select the plural form
   * @param[in] domain The gettext domain, or empty for the default domain
   * @return The localized plural string or fallback resource id
   */
  Dali::String GetLocalizedPluralString(StringView resourceId,
                                        StringView pluralResourceId,
                                        uint32_t   quantity,
                                        StringView domain) const;

  /**
   * @brief Sets a localized string resource for a named binding and applies it immediately.
   *
   * This overload uses the current default domain when the binding is refreshed.
   *
   * If a binding with the same @a target and @a bindingId already exists,
   * the stored resourceId, domain, and callback are replaced.
   *
   * This can be called again to update the resourceId, domain, or callback
   * for an existing binding.
   *
   * Unlike UiColorManager::SetBindingColor(), this method also registers
   * the callback because localized string bindings are commonly created
   * in one step.
   *
   * @param[in] target The target object
   * @param[in] bindingId Caller-defined binding id, e.g. "Text", "ImageUrl"
   * @param[in] resourceId The localization resource id / msgid
   * @param[in] callback Callback invoked with the localized string
   */
  void SetBindingResource(BaseHandle              target,
                          StringView              bindingId,
                          StringView              resourceId,
                          LocalizedStringCallback callback);

  /**
   * @brief Sets a localized string resource and explicit domain for a named binding and applies it immediately.
   *
   * Passing an empty domain makes the binding use the current default domain.
   *
   * If a binding with the same @a target and @a bindingId already exists,
   * the stored resourceId, domain, and callback are replaced.
   *
   * This can be called again to update the resourceId, domain, or callback
   * for an existing binding.
   *
   * Unlike UiColorManager::SetBindingColor(), this method also registers
   * the callback because localized string bindings are commonly created
   * in one step.
   *
   * @param[in] target The target object
   * @param[in] bindingId Caller-defined binding id, e.g. "Text", "ImageUrl"
   * @param[in] resourceId The localization resource id / msgid
   * @param[in] domain The gettext domain, or empty for default domain
   * @param[in] callback Callback invoked with the localized string
   */
  void SetBindingResource(BaseHandle              target,
                          StringView              bindingId,
                          StringView              resourceId,
                          StringView              domain,
                          LocalizedStringCallback callback);

  /**
   * @brief Returns whether a named binding exists for an object.
   *
   * @param[in] target The target object
   * @param[in] bindingId Binding id
   * @return true if the binding exists
   */
  [[nodiscard]] bool HasBinding(BaseHandle target,
                                StringView bindingId) const;

  /**
   * @brief Removes a named binding.
   *
   * The current applied property value is not changed.
   *
   * @param[in] target The target object
   * @param[in] bindingId Binding id
   */
  void ClearBinding(BaseHandle target,
                    StringView bindingId);

  /**
   * @brief Removes all bindings associated with a target object.
   *
   * @param[in] target The target object
   */
  void ClearBindings(BaseHandle target);

  /**
   * @brief Sets a function that overrides localized string lookup.
   *
   * Setting the override refreshes all existing bindings immediately.
   *
   * @note This is a plain function pointer. Capturing lambdas and member
   *       functions are not accepted.
   *
   * @note Passing nullptr clears the override, equivalent to
   *       ClearLocalizedStringOverride().
   *
   * @note This override is used by GetLocalizedString() and localization
   *       bindings. It is not used by GetLocalizedPluralString().
   *
   * @param[in] func Override function or nullptr
   */
  void SetLocalizedStringOverride(LocalizedStringOverrideFunc func);

  /**
   * @brief Clears the localized string override.
   *
   * All bindings are refreshed to use gettext/dgettext values.
   */
  void ClearLocalizedStringOverride();

  /**
   * @brief Enables or disables localization bypass.
   *
   * When enabled, GetLocalizedString() and GetLocalizedPluralString() return
   * resourceId as-is and bindings apply resourceId as-is.
   *
   * Changing this value refreshes all bindings.
   *
   * @param[in] enabled true to enable bypass
   */
  void SetBypassEnabled(bool enabled);

  /**
   * @brief Returns whether localization bypass is enabled.
   *
   * @return true if bypass is enabled
   */
  bool IsBypassEnabled() const;

  /**
   * @brief Refreshes all registered bindings.
   *
   * Call this when application-owned resource tables used by the override
   * function changed.
   *
   * Locale changed signal should also call this internally.
   */
  void RefreshBindings();

public: // Not intended for application developers
  /// @cond internal
  explicit UiLocalizationManager(Internal::UiLocalizationManagerImpl* impl);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
