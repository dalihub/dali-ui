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
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebCookieManagerImpl;
} // namespace Integration DALI_INTERNAL

/**
 * @brief Manages cookies belonging to a WebProfile.
 */
class DALI_UI_API WebCookieManager : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized WebCookieManager handle.
   */
  WebCookieManager();

  /**
   * @brief Destructor.
   */
  ~WebCookieManager();

  /**
   * @brief Copy constructor.
   */
  WebCookieManager(const WebCookieManager& manager);

  /**
   * @brief Move constructor.
   */
  WebCookieManager(WebCookieManager&& manager) noexcept;

  /**
   * @brief Copy assignment operator.
   */
  WebCookieManager& operator=(const WebCookieManager& manager);

  /**
   * @brief Move assignment operator.
   */
  WebCookieManager& operator=(WebCookieManager&& manager) noexcept;

  /**
   * @brief Compares the underlying cookie store identity.
   */
  bool operator==(const WebCookieManager& rhs) const;

  /**
   * @brief Compares the underlying cookie store identity.
   */
  bool operator!=(const WebCookieManager& rhs) const;

  /**
   * @brief Downcasts a handle to WebCookieManager.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized WebCookieManager on success, otherwise an empty handle
   */
  static WebCookieManager DownCast(BaseHandle handle);

  /**
   * @brief Clears all cookies in this manager's cookie store.
   *
   * This method does nothing when the web engine does not provide a cookie
   * manager.
   */
  void ClearAllCookies();

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebCookieManager(Integration::WebCookieManagerImpl& implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
