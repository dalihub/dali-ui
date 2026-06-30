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
#include <dali-ui-foundation/public-api/configuration/ui-config.h>

namespace Dali
{
namespace Ui
{
namespace Components
{

namespace Internal
{
class UiConfigImpl;
}

/**
 * @brief UiConfig preset for applications using dali-ui-components.
 *
 * The config includes the components default style sheet and is the recommended
 * config entry point for applications using dali-ui-components. It also covers
 * the foundation configuration, so applications should apply only this config
 * and should not apply a separate Dali::Ui::UiConfig as well. Applications may
 * call StyleSheet() before Apply() to override individual style entries.
 */
class DALI_UI_API UiConfig : public Dali::Ui::UiConfig
{
public:
  /**
   * @brief Creates an initialized components UiConfig.
   *
   * @return A components UiConfig
   */
  static UiConfig New();

  /**
   * @brief Creates an uninitialized UiConfig handle.
   */
  UiConfig() = default;

public: // Not intended for application developers
  /// @cond internal
  explicit DALI_INTERNAL UiConfig(Internal::UiConfigImpl* impl);
  /// @endcond
};

} // namespace Components
} // namespace Ui
} // namespace Dali
