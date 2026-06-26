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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/signals/slot-delegate.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/provider-api/theme-loader-interface.h>
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-theme-manager.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{

/**
 * @brief Internal implementation of UiThemeManager.
 *
 * Holds the ThemeLoaderInterface instance and exposes theme-level
 * queries. The loader is created on demand via UiConfigImpl::CreateThemeLoader()
 * and cannot be replaced once created.
 */
class DALI_UI_API UiThemeManagerImpl : public BaseObject
{
public:
  using ThemeChangedSignalType = Signal<void()>;

  /**
   * @brief Returns the singleton UiThemeManager handle.
   *
   * @return A UiThemeManager handle wrapping the singleton impl
   */
  static UiThemeManager Get();

  /**
   * @brief Ensures the ThemeLoaderInterface is created.
   *
   * On the first call, creates the loader via UiConfigImpl::CreateThemeLoader().
   * If CreateThemeLoader() returns nullptr, a built-in DefaultThemeLoader is used.
   * Subsequent calls are no-ops.
   *
   * @pre UiConfig::Apply() must have been called. Triggers assertion failure otherwise.
   */
  void EnsureThemeLoader();

  /**
   * @brief Returns the ThemeLoaderInterface instance, creating it if necessary.
   *
   * @pre UiConfig::Apply() must have been called. Triggers assertion failure otherwise.
   * @return A reference to the ThemeLoaderInterface
   */
  ThemeLoaderInterface& GetLoader();

  /**
   * @brief Returns the identifier of the current theme.
   *
   * @return The theme identifier, or an empty string if no theme is active
   */
  String GetCurrentThemeId() const;

  /**
   * @brief Returns the signal emitted when the theme changes.
   *
   * @return A reference to the theme-changed signal
   */
  ThemeChangedSignalType& ThemeChangedSignal();

protected:
  /**
   * @brief Constructor. Used internally to create the singleton impl.
   */
  UiThemeManagerImpl();

  /**
   * @brief Destructor.
   */
  ~UiThemeManagerImpl() override;

private:
  UiThemeManagerImpl(const UiThemeManagerImpl&)            = delete;
  UiThemeManagerImpl(UiThemeManagerImpl&&)                 = delete;
  UiThemeManagerImpl& operator=(const UiThemeManagerImpl&) = delete;
  UiThemeManagerImpl& operator=(UiThemeManagerImpl&&)      = delete;

  void OnLoaderThemeChanged();

private:
  ThemeLoaderInterface*            mLoader{nullptr};
  ThemeChangedSignalType           mThemeChangedSignal;
  SlotDelegate<UiThemeManagerImpl> mSlotDelegate{this};
};

} // namespace Integration

/**
 * @brief Retrieves the UiThemeManagerImpl from a UiThemeManager handle.
 */
inline Integration::UiThemeManagerImpl& GetImpl(UiThemeManager& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UiThemeManagerImpl&>(handle);
}

/**
 * @brief Retrieves the UiThemeManagerImpl from a const UiThemeManager handle.
 */
inline const Integration::UiThemeManagerImpl& GetImpl(const UiThemeManager& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UiThemeManagerImpl&>(handle);
}

} // namespace Ui
} // namespace Dali
