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
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali
{
namespace Ui
{
namespace Integration
{
class UiElementConfigImpl;
}

#include "ui-element-config.autogen.h"
/**
 * @brief Configuration for dali-ui-elements default values.
 *
 * UiElementConfig extends UiConfig with element-specific defaults
 * such as font size and text color that are applied when creating
 * primitive UI elements like TextLabel or ImageView.
 *
 * @code
 *  UiElementConfig::New()
 *       .SetScalingFactor(1.5f)
 *       .SetDefaultFontSize(24.0f)
 *       .SetDefaultTextColor(Color::BLACK);
 *       .Apply();
 * @endcode
 */
class DALI_UI_API UiElementConfig : public UiConfig
{
public:
  /**
   * @brief Creates an uninitialized UiElementConfig handle.
   */
  UiElementConfig() = default;

  /**
   * @brief Destructor.
   */
  ~UiElementConfig() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UiElementConfig(const UiElementConfig& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UiElementConfig(UiElementConfig&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UiElementConfig& operator=(const UiElementConfig& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UiElementConfig& operator=(UiElementConfig&& rhs) noexcept = default;

  /**
   * @brief Creates a new UiElementConfig with default values.
   *
   * Inherits UiConfig defaults and adds: defaultFontSize=16.0f,
   * defaultTextColor=Color::BLACK.
   *
   * @return An initialized UiElementConfig handle
   */
  static UiElementConfig New();

  /**
   * @brief Downcasts a handle to a UiElementConfig handle.
   *
   * If handle points to a UiElementConfig, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UiElementConfig or an uninitialized handle
   */
  static UiElementConfig DownCast(BaseHandle handle);

public: // Properties

  // @CHAIN_START(UiElementConfig, UiConfig)
  /**
   * @brief Sets the default font point-size for text elements.
   *
   * @pre The config must not be frozen.
   * @param[in] fontSize The default font size in points
   */
  UiElementConfig& SetDefaultFontSize(float fontSize);

  /**
   * @brief Retrieves the default font point-size.
   *
   * @return The default font size in points
   */
  float GetDefaultFontSize() const;

  /**
   * @brief Sets the default text color for text elements.
   *
   * @pre The config must not be frozen.
   * @param[in] color The default text color
   */
  UiElementConfig& SetDefaultTextColor(const Vector4& color);

  /**
   * @brief Retrieves the default text color.
   *
   * @return The default text color
   */
  Vector4 GetDefaultTextColor() const;

  // @CHAIN_END

public: // Not intended for Application developers

  /**
   * @brief This constructor is used internally to wrap an implementation object.
   *
   * @param[in] impl A pointer to the internal UiElementConfig implementation
   */
  explicit UiElementConfig(Integration::UiElementConfigImpl* impl);

public:
  DALI_UI_CHAIN_UICONFIG_METHODS(UiElementConfig)
};

} // namespace Ui
} // namespace Dali
