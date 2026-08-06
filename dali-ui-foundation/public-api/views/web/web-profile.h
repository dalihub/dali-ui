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
#include <dali/public-api/object/base-handle.h>

namespace Dali
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
 */
class DALI_UI_API WebProfile : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized WebProfile handle.
   */
  WebProfile();

  /**
   * @brief Destructor.
   */
  ~WebProfile();

  /**
   * @brief Copy constructor.
   */
  WebProfile(const WebProfile& profile);

  /**
   * @brief Move constructor.
   */
  WebProfile(WebProfile&& profile) noexcept;

  /**
   * @brief Copy assignment operator.
   */
  WebProfile& operator=(const WebProfile& profile);

  /**
   * @brief Move assignment operator.
   */
  WebProfile& operator=(WebProfile&& profile) noexcept;

  /**
   * @brief Compares the underlying browsing profile identity.
   */
  bool operator==(const WebProfile& rhs) const;

  /**
   * @brief Compares the underlying browsing profile identity.
   */
  bool operator!=(const WebProfile& rhs) const;

  /**
   * @brief Downcasts a handle to WebProfile.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized WebProfile on success, otherwise an empty handle
   */
  static WebProfile DownCast(BaseHandle handle);

  /**
   * @brief Clears the resource cache for this profile.
   *
   * This method does nothing when the web engine does not provide a browsing
   * context.
   */
  void ClearCache();

  /**
   * @brief Gets the cookie manager for this profile.
   *
   * The returned handle is initialized even when the backend does not support
   * cookie management. Its operations then do nothing.
   *
   * @return The cookie manager associated with this profile
   */
  WebCookieManager GetCookieManager() const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebProfile(Integration::WebProfileImpl& implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
