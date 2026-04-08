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

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali-ui-foundation/public-api/ui-config.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

/**
 * @brief Singleton manager that holds the global UiConfig instance.
 *
 * UiConfigManager provides process-wide access to the UiConfig.
 * Configuration values should be read directly from the UiConfig
 * returned by GetConfig().
 *
 * Must be initialized exactly once before the application main loop starts.
 */
class DALI_UI_API UiConfigManager
{
public:
  /**
   * @brief Returns the singleton UiConfigManager instance.
   *
   * @return The singleton instance
   */
  static UiConfigManager& Get();

  /**
   * @brief Initializes the manager with the given UiConfig.
   *
   * This method must be called exactly once before the application main loop starts.
   * After this call, the UiConfig is frozen and its setter methods can no longer be used.
   * Calling Initialize() more than once triggers an assertion failure.
   *
   * @param[in] config The UiConfig to use globally
   */
  void Initialize(const UiConfig& config);

  /**
   * @brief Returns whether Initialize() has been called.
   *
   * @return True if the manager has been initialized
   */
  bool IsInitialized() const;

  /**
   * @brief Returns the global UiConfig instance.
   *
   * @pre Initialize() must have been called. Triggers assertion failure otherwise.
   * @return The global UiConfig handle
   */
  const UiConfig& GetConfig() const;

  /*
   * @brief Creates a ThemeLoaderInterface instance from the current config.
   *
   * Forwards to UiConfigImpl::CreateThemeLoader(). Called internally by
   * UiThemeManagerImpl during on-demand loader creation.
   *
   * @pre Initialize() must have been called.
   * @return A new ThemeLoaderInterface instance. Caller takes ownership.
   */
  ThemeLoaderInterface* CreateThemeLoader();

  /*
   * @brief Called when the adaptor is ready.
   */
  void OnApplicationCreated();

private:
  UiConfigManager()  = default;
  ~UiConfigManager() = default;

  UiConfigManager(const UiConfigManager&)            = delete;
  UiConfigManager& operator=(const UiConfigManager&) = delete;

private:
  UiConfig mConfig;
  bool     mUiConfigInitialized{false};
  bool     mApplicationCreated{false};
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
