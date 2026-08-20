#pragma once

/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{

/**
 * @brief Policy used to resolve the native state at application launch.
 */
enum class WindowFrameInitialStatePolicy
{
  AUTO             = 0, ///< Treats an initial target-sized window as maximized
  USE_CURRENT      = 1, ///< Uses the state already reported by Dali::Window
  REQUEST_MAXIMIZE = 2  ///< Requests maximize during initial attachment
};

/**
 * @brief Interactive window operations offered by a WindowFrame.
 *
 * These values are caller-configured feature gates. They do not report runtime
 * backend capabilities. An enabled feature allows WindowFrame to dispatch the
 * corresponding request; the window system may still ignore it. Launch state,
 * decoration, shadow, and signal observation use their dedicated APIs rather
 * than feature gates.
 */
enum class WindowFrameFeature
{
  MOVE             = 0, ///< Interactive move requests
  RESIZE           = 1, ///< Interactive resize requests
  MINIMIZE         = 2, ///< Minimize requests
  MAXIMIZE_RESTORE = 3  ///< Maximize and restore requests
};

/**
 * @brief Creation-time configuration for a WindowFrame.
 *
 * This type contains launch-state policy and operation gates that must be
 * resolved while WindowFrame is created. Mutable presentation and
 * interaction settings are configured on the created WindowFrame handle.
 * The value can be copied and extended without exposing its storage layout.
 */
class DALI_UI_COMPONENTS_API WindowFrameOptions
{
public:
  WindowFrameOptions();
  WindowFrameOptions(const WindowFrameOptions& rhs);
  WindowFrameOptions(WindowFrameOptions&& rhs) noexcept;
  WindowFrameOptions& operator=(const WindowFrameOptions& rhs);
  WindowFrameOptions& operator=(WindowFrameOptions&& rhs) noexcept;
  ~WindowFrameOptions();

  /**
   * @brief Sets launch-state resolution policy.
   * @param[in] policy Launch policy
   */
  void SetInitialStatePolicy(WindowFrameInitialStatePolicy policy);

  /**
   * @brief Returns launch-state resolution policy.
   * @return Current policy
   */
  WindowFrameInitialStatePolicy GetInitialStatePolicy() const;

  /**
   * @brief Sets normal frame size used for the first restore after an initial
   *        full-size launch.
   * @param[in] size Restore frame size
   */
  void SetInitialRestoreFrameSize(const Dali::Vector2& size);

  /**
   * @brief Removes the configured initial restore frame size.
   */
  void ClearInitialRestoreFrameSize();

  /**
   * @brief Reads the configured initial restore frame size.
   *
   * The output is not modified when no size is configured.
   * @param[out] size Configured restore size
   * @return true when configured
   */
  bool GetInitialRestoreFrameSize(Dali::Vector2& size) const;

  /**
   * @brief Enables or disables a WindowFrame feature.
   *
   * Every feature is enabled by default, whatever the backend supports, so this
   * declares what the product offers rather than what the window system can do.
   * The configured value is applied when a WindowFrame is created with these
   * options.
   *
   * @param[in] feature Feature to configure
   * @param[in] enabled Whether the feature is offered
   */
  void SetFeatureEnabled(WindowFrameFeature feature, bool enabled);

  /**
   * @brief Returns the configured value for a feature.
   * @param[in] feature Feature to query
   * @return true when the feature is enabled
   */
  bool IsFeatureEnabled(WindowFrameFeature feature) const;

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Dali::Ui
