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

#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/views/web/web-cookie-manager.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebProfileImpl;
} // namespace Integration DALI_INTERNAL

/**
 * @brief Represents browsing data shared by WebViews in the same web profile.
 *
 * A profile can be shared by multiple WebViews. Cache and cookie operations
 * therefore affect every WebView associated with the same profile.
 * @SINCE_3_0.0
 */
class DALI_UI_API WebProfile : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized WebProfile handle.
   * @SINCE_3_0.0
   */
  WebProfile();

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~WebProfile();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  WebProfile(const WebProfile& profile);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  WebProfile(WebProfile&& profile) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  WebProfile& operator=(const WebProfile& profile);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  WebProfile& operator=(WebProfile&& profile) noexcept;

  /**
   * @brief Compares the underlying browsing profile identity.
   * @SINCE_3_0.0
   */
  bool operator==(const WebProfile& rhs) const;

  /**
   * @brief Compares the underlying browsing profile identity.
   * @SINCE_3_0.0
   */
  bool operator!=(const WebProfile& rhs) const;

  /**
   * @brief Downcasts a handle to WebProfile.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to downcast
   * @return An initialized WebProfile on success, otherwise an empty handle
   */
  static WebProfile DownCast(BaseHandle handle);

  /**
   * @brief Clears the resource cache for this profile.
   *
   * This method does nothing when the web engine does not provide a browsing
   * context.
   * @SINCE_3_0.0
   */
  void ClearCache();

  /**
   * @brief Gets the cookie manager for this profile.
   *
   * The returned handle is initialized even when the backend does not support
   * cookie management. Its operations then do nothing.
   *
   * @SINCE_3_0.0
   * @return The cookie manager associated with this profile
   */
  WebCookieManager GetCookieManager() const;

  /**
   * @brief Creates a named profile after application initialization and UiConfig::Apply().
   *
   * Call on the DALi event thread. This may initialize the web engine, fixing its
   * type and arguments. Names are case-sensitive; empty, Default and live duplicate
   * names are assertion errors. A released name may be reused. Incognito sessions
   * live as long as the profile or a dependent view/cookie manager is retained.
   * Normal profile destruction does not delete persistent browsing data.
   * @SINCE_3_0.0
   * @param[in] name The lookup key, not a native storage path
   * @param[in] incognito Whether browsing data is private
   * @return The new profile, or an empty handle on backend failure/unsupported profiles
   */
  static WebProfile New(const Dali::String& name, bool incognito = false);

  /**
   * @brief Creates a named profile with an injected bundle path.
   * @SINCE_3_0.0
   * @param[in] name The case-sensitive lookup key
   * @param[in] incognito Whether browsing data is private
   * @param[in] injectedBundlePath The bundle path, or an empty string for no bundle
   * @return The new profile, or an empty handle on failure; no default fallback
   * @see New(const Dali::String&, bool)
   */
  static WebProfile New(const Dali::String& name, bool incognito, const Dali::String& injectedBundlePath);

  /**
   * @brief Looks up a live profile without initializing the engine or creating a context.
   * @SINCE_3_0.0
   * @param[in] name The case-sensitive lookup key (Default for an existing default profile)
   * @return The existing profile, or an empty handle when absent
   */
  static WebProfile GetProfile(const Dali::String& name);

  /** @brief Gets the lookup key. @SINCE_3_0.0 */
  Dali::String GetName() const;

  /** @brief Gets the private browsing mode fixed at creation. @SINCE_3_0.0 */
  bool IsIncognito() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebProfile(Integration::WebProfileImpl& implementation);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE
