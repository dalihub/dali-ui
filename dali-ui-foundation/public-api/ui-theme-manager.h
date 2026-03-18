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
#include <dali/public-api/signals/dali-signal.h>
#include <dali/public-api/common/dali-string.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{

namespace Integration
{
class UiThemeManagerImpl;
}

/**
 * @brief Singleton manager that provides access to the current theme.
 *
 * UiThemeManager is a Handle-based singleton. Theme-level queries (theme ID,
 * theme change notification) are exposed here. Color lookups are accessed
 * through UiColorManager, not directly through this class.
 *
 * The ThemeLoaderInterface instance is created via UiConfigImpl::CreateThemeLoader()
 * during UiConfig initialization. If CreateThemeLoader() returns nullptr,
 * a built-in default loader is used.
 */
class DALI_UI_API UiThemeManager : public BaseHandle
{
public:
  using ThemeChangedSignalType = Signal<void()>;

  UiThemeManager();
  ~UiThemeManager()                                        = default;
  UiThemeManager(const UiThemeManager& handle)             = default;
  UiThemeManager(UiThemeManager&& rhs) noexcept            = default;
  UiThemeManager& operator=(const UiThemeManager& handle)  = default;
  UiThemeManager& operator=(UiThemeManager&& rhs) noexcept = default;

  /**
   * @brief Returns the singleton UiThemeManager instance.
   *
   * @return A handle to the UiThemeManager singleton
   */
  static UiThemeManager Get();

  /**
   * @brief Downcasts a handle to a UiThemeManager handle.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UiThemeManager or an uninitialized handle
   */
  static UiThemeManager DownCast(BaseHandle handle);

  /**
   * @brief Returns the identifier of the current theme.
   *
   * @return The theme identifier, or an empty string if no theme is active
   */
  String GetCurrentThemeId() const;

  /**
   * @brief Returns a signal that is emitted when the theme changes.
   *
   * @return A reference to the theme-changed signal
   */
  ThemeChangedSignalType& ThemeChangedSignal();

public: // Not intended for application developers
  /**
   * @brief This constructor is used internally to wrap an implementation object.
   *
   * @param[in] impl A pointer to the internal UiThemeManager implementation
   */
  explicit UiThemeManager(Integration::UiThemeManagerImpl* impl);
};

} // namespace Ui
} // namespace Dali
