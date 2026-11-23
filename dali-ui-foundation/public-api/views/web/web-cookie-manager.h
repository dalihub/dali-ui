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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebCookieManagerImpl;
} // namespace Integration DALI_INTERNAL

/**
 * @brief Cookie acceptance policy for the represented profile.
 * @SINCE_3_0.2
 */
enum class WebCookieAcceptPolicy
{
  ALWAYS,            ///< Accept cookies from any page. @SINCE_3_0.2
  NEVER,             ///< Reject cookies. @SINCE_3_0.2
  BLOCK_THIRD_PARTY, ///< Blocks third-party cookies. @SINCE_3_0.2
};

/**
 * @brief Manages cookies belonging to a WebProfile.
 * @SINCE_3_0.0
 */
class DALI_UI_API WebCookieManager : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized WebCookieManager handle.
   * @SINCE_3_0.0
   */
  WebCookieManager();

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~WebCookieManager();

  /**
   * @brief Copy constructor.
   * @SINCE_3_0.0
   */
  WebCookieManager(const WebCookieManager& manager);

  /**
   * @brief Move constructor.
   * @SINCE_3_0.0
   */
  WebCookieManager(WebCookieManager&& manager) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_3_0.0
   */
  WebCookieManager& operator=(const WebCookieManager& manager);

  /**
   * @brief Move assignment operator.
   * @SINCE_3_0.0
   */
  WebCookieManager& operator=(WebCookieManager&& manager) noexcept;

  /**
   * @brief Compares the underlying cookie store identity.
   * @SINCE_3_0.0
   */
  bool operator==(const WebCookieManager& rhs) const;

  /**
   * @brief Compares the underlying cookie store identity.
   * @SINCE_3_0.0
   */
  bool operator!=(const WebCookieManager& rhs) const;

  /**
   * @brief Downcasts a handle to WebCookieManager.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to downcast
   * @return An initialized WebCookieManager on success, otherwise an empty handle
   */
  static WebCookieManager DownCast(BaseHandle handle);

  /**
   * @brief Clears all cookies in this manager's cookie store.
   *
   * This method does nothing when the web engine does not provide a cookie
   * manager.
   * @SINCE_3_0.0
   */
  void ClearAllCookies();

  /**
   * @brief Owning callback invoked without arguments when cookies change.
   * @SINCE_3_0.2
   */
  using CookieChangesCallback = Dali::Callback<void()>;

  /**
   * @brief Sets the cookie acceptance policy for this profile.
   *
   * Call on the DALi event thread. Does nothing when the backend has no cookie
   * manager or the context has terminated.
   * @SINCE_3_0.2
   * @param[in] policy The policy applied to all views sharing this profile
   * @pre policy is a valid WebCookieAcceptPolicy enumerator
   */
  void SetCookieAcceptPolicy(WebCookieAcceptPolicy policy);

  /**
   * @brief Configures persistent cookie storage for a normal profile.
   *
   * Call on the DALi event thread. Incognito profiles ignore this operation and
   * never forward a persistent-storage request to the backend. Does nothing
   * when the backend has no cookie manager or the context has terminated.
   * The Chromium backend uses a Cookies database under the supplied directory
   * and may persist session cookies as well. Return does not imply completion
   * of database loading or writing.
   * @SINCE_3_0.2
   * @param[in] path Directory used for cookie storage
   */
  void SetPersistentStorage(const Dali::String& path);

  /**
   * @brief Replaces the cookie change callback for this profile.
   * @SINCE_3_0.2
   * @param[in] callback Callback whose ownership is transferred to this manager; empty to stop watching
   * @note Call on the main event thread. The callback may run on an engine
   * thread and must not access UI handles. It reports a change, without cookie
   * identity or change type. Do not capture this manager by value, as that creates a reference cycle.
   * @note Replacing, clearing, or destroying the manager prevents subsequent
   * dispatch of the old callback. A dispatch that already acquired the callback
   * may begin or finish invoking it after removal.
   * No operation is performed if the engine is unavailable or terminated.
   */
  void SetCookieChangesCallback(CookieChangesCallback callback);

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebCookieManager(Integration::WebCookieManagerImpl& implementation);
  /// @endcond
};

} // namespace Ui
} //namespace DALI_NAMESPACE
