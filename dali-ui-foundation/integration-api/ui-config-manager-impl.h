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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/connection-tracker.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/signals/slot-delegate.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/theme-loader-interface.h>
#include <dali-ui-foundation/integration-api/ui-config-manager.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class UiConfigManagerImpl;
using UiConfigManagerImplPtr = IntrusivePtr<UiConfigManagerImpl>;
/**
 * @brief Internal implementation of UiConfigManager.
 */
class DALI_UI_API UiConfigManagerImpl : public BaseObject, public ConnectionTracker
{
public:
  static UiConfigManagerImplPtr New();

public: ///< Method from UiConfigManager
  /**
   * @copydoc Dali::Ui::Integration::UiConfigManager::Initialize()
   */
  void Initialize(const UiConfig& config);

  /**
   * @copydoc Dali::Ui::Integration::UiConfigManager::IsInitialized()
   */
  bool IsInitialized() const;

  /**
   * @copydoc Dali::Ui::Integration::UiConfigManager::GetConfig()
   */
  const UiConfig& GetConfig() const;

  /**
   * @copydoc Dali::Ui::Integration::UiConfigManager::CreateThemeLoader()
   */
  ThemeLoaderInterface* CreateThemeLoader();

public: ///< Internal API for UiConfigManager
  /**
   * @brief Register callbacks to LifecycleController.
   * @note It could be failed if we try to register too ealy timing, like before Core::New().
   * @return True if register successed, or already registered. False if register failed.
   */
  bool RegisterLifecycleControllerCallback();

protected:
  /**
   * @brief Constructor. Used internally to create the singleton impl.
   */
  UiConfigManagerImpl();

  /**
   * @brief Destructor.
   */
  ~UiConfigManagerImpl() override;

  /*
   * @brief Called when the adaptor is ready.
   */
  void OnApplicationCreated();

private:
  UiConfigManagerImpl(const UiConfigManagerImpl&)            = delete;
  UiConfigManagerImpl(UiConfigManagerImpl&&)                 = delete;
  UiConfigManagerImpl& operator=(const UiConfigManagerImpl&) = delete;
  UiConfigManagerImpl& operator=(UiConfigManagerImpl&&)      = delete;

private:
  UiConfig mConfig;
  bool     mUiConfigInitialized{false};
  bool     mApplicationCreated{false};
  bool     mLifecycleControllerCallbackConnected{false};
};

} // namespace Integration

/**
 * @brief Retrieves the UiConfigManagerImpl from a UiConfigManager handle.
 */
inline Integration::UiConfigManagerImpl& GetImpl(Integration::UiConfigManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UiConfigManagerImpl&>(handle);
}

/**
 * @brief Retrieves the UiConfigManagerImpl from a const UiConfigManager handle.
 */
inline const Integration::UiConfigManagerImpl& GetImpl(const Integration::UiConfigManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UiConfigManagerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
