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
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-config.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{
class UiConfigManagerImpl;

/**
 * @brief Singleton manager that holds the global UiConfig instance.
 *
 * UiConfigManager provides process-wide access to the UiConfig.
 * Configuration values should be read directly from the UiConfig
 * returned by GetConfig().
 *
 * Must be initialized exactly once before the application main loop starts.
 */
class DALI_UI_API UiConfigManager : public BaseHandle
{
public:
  /**
   * @brief Returns the singleton UiConfigManager instance.
   *
   * @return The singleton instance
   */
  static UiConfigManager Get();

  /**
   * @brief Create a UiConfigManager handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  UiConfigManager();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~UiConfigManager();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] rhs A reference to the copied handle
   */
  UiConfigManager(const UiConfigManager& rhs);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * It makes this handle use the same BaseObject as the copied handle
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this handle
   */
  UiConfigManager& operator=(const UiConfigManager& rhs);

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs A reference to the moved handle
   */
  UiConfigManager(UiConfigManager&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  UiConfigManager& operator=(UiConfigManager&& rhs) noexcept;

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

private:
  explicit DALI_INTERNAL UiConfigManager(UiConfigManagerImpl* impl);
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
