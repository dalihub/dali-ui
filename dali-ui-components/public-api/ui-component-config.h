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
#include <dali-ui-foundation/public-api/ui-config.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class UiComponentConfigImpl;
}

/**
 * @brief Configuration for dali-ui-components default values.
 *
 * UiComponentConfig extends UiConfig and serves as a placeholder
 * for component-specific configuration. As the dali-ui-components
 * library grows, component-specific settings can be added here.
 *
 * @code
 *  UiComponentConfig config = UiComponentConfig::New();
 *  config.SetScalingFactor(1.5f);
 *  config.SetDefaultFontSize(24.0f);
 *  config.SetDefaultTextColor(Color::BLACK);
 *  config.Apply();
 * @endcode
 */
class DALI_UI_API UiComponentConfig : public UiConfig
{
public:
  /**
   * @brief Creates an uninitialized UiComponentConfig handle.
   */
  UiComponentConfig() = default;

  /**
   * @brief Destructor.
   */
  ~UiComponentConfig() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UiComponentConfig(const UiComponentConfig& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UiComponentConfig(UiComponentConfig&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UiComponentConfig& operator=(const UiComponentConfig& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UiComponentConfig& operator=(UiComponentConfig&& rhs) noexcept = default;

  /**
   * @brief Creates a new UiComponentConfig with default values.
   *
   * Inherits all UiConfig defaults including defaultFontSize=16.0f
   * and defaultTextColor=Color::BLACK.
   *
   * @return An initialized UiComponentConfig handle
   */
  static UiComponentConfig New();

  /**
   * @brief Downcasts a handle to a UiComponentConfig handle.
   *
   * If handle points to a UiComponentConfig, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UiComponentConfig or an uninitialized handle
   */
  static UiComponentConfig DownCast(BaseHandle handle);

public: // Not intended for Application developers
  /**
   * @brief This constructor is used internally to wrap an implementation object.
   *
   * @param[in] impl A pointer to the internal UiComponentConfig implementation
   */
  explicit UiComponentConfig(Integration::UiComponentConfigImpl* impl);
};

} // namespace Ui
} // namespace Dali
