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

namespace Dali
{
namespace Ui
{
namespace Integration DALI_INTERNAL
{
class WebSettingsImpl;
} // namespace Integration DALI_INTERNAL

/**
 * @brief Provides settings associated with a WebView.
 *
 * A WebSettings handle remains safe to retain after its WebView is destroyed.
 * Operations then return their documented default result without affecting an
 * engine.
 */
class DALI_UI_API WebSettings : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized WebSettings handle.
   */
  WebSettings();

  /**
   * @brief Destructor.
   */
  ~WebSettings();

  /**
   * @brief Copy constructor.
   */
  WebSettings(const WebSettings& settings);

  /**
   * @brief Move constructor.
   */
  WebSettings(WebSettings&& settings) noexcept;

  /**
   * @brief Copy assignment operator.
   */
  WebSettings& operator=(const WebSettings& settings);

  /**
   * @brief Move assignment operator.
   */
  WebSettings& operator=(WebSettings&& settings) noexcept;

  /**
   * @brief Downcasts a handle to WebSettings.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized WebSettings on success, otherwise an empty handle
   */
  static WebSettings DownCast(BaseHandle handle);

  /**
   * @brief Sets a string value for an extra web engine feature.
   *
   * This generic extension point allows additional settings to be exposed
   * without adding a dedicated API for every feature.
   *
   * @param[in] feature The feature name
   * @param[in] value The feature value
   * @return True on success, or false when the feature or engine is unavailable
   */
  bool SetExtraFeatureValue(const Dali::String& feature, const Dali::String& value);

  /**
   * @brief Gets a string value for an extra web engine feature.
   *
   * @param[in] feature The feature name
   * @return The feature value, or an empty string when unavailable
   */
  Dali::String GetExtraFeatureValue(const Dali::String& feature) const;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL WebSettings(Integration::WebSettingsImpl& implementation);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
