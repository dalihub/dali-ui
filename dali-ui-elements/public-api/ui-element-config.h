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
namespace UI
{
namespace Integration
{
class UIElementConfigImpl;
}

#include "ui-element-config.autogen.h"
/**
 * @brief Configuration for dali-ui-elements default values.
 *
 * UIElementConfig extends UIConfig with element-specific defaults
 * such as font size and text color that are applied when creating
 * primitive UI elements like TextLabel or ImageView.
 *
 * @code
 *  UIElementConfig::New()
 *       .SetScalingFactor(1.5f)
 *       .SetDefaultFontSize(24.0f)
 *       .SetDefaultTextColor(Color::BLACK);
 *       .Apply();
 * @endcode
 */
class DALI_UI_API UIElementConfig : public UIConfig
{
public:
  /**
   * @brief Creates an uninitialized UIElementConfig handle.
   */
  UIElementConfig() = default;

  /**
   * @brief Destructor.
   */
  ~UIElementConfig() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UIElementConfig(const UIElementConfig& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UIElementConfig(UIElementConfig&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UIElementConfig& operator=(const UIElementConfig& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UIElementConfig& operator=(UIElementConfig&& rhs) noexcept = default;

  /**
   * @brief Creates a new UIElementConfig with default values.
   *
   * Inherits UIConfig defaults and adds: defaultFontSize=16.0f,
   * defaultTextColor=Color::BLACK.
   *
   * @return An initialized UIElementConfig handle
   */
  static UIElementConfig New();

  /**
   * @brief Downcasts a handle to a UIElementConfig handle.
   *
   * If handle points to a UIElementConfig, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UIElementConfig or an uninitialized handle
   */
  static UIElementConfig DownCast(BaseHandle handle);

public: // Properties

  // @CHAIN_START(UIElementConfig, UIConfig)
  /**
   * @brief Sets the default font point-size for text elements.
   *
   * @pre The config must not be frozen.
   * @param[in] fontSize The default font size in points
   */
  UIElementConfig& SetDefaultFontSize(float fontSize);

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
  UIElementConfig& SetDefaultTextColor(const Vector4& color);

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
   * @param[in] impl A pointer to the internal UIElementConfig implementation
   */
  explicit UIElementConfig(Integration::UIElementConfigImpl* impl);

public:
  DALI_UI_CHAIN_UICONFIG_METHODS(UIElementConfig)
};

} // namespace UI
} // namespace Dali
