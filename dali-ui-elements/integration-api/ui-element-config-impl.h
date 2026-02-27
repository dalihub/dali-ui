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
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-elements/public-api/ui-element-config.h>

namespace Dali
{

namespace UI
{

namespace Integration
{

/**
 * @brief Internal implementation of UIElementConfig.
 *
 * Extends UIConfigImpl with element-specific defaults such as font size
 * and text color. Registers itself with UIElementConfigManager via
 * the OnInitialized() hook.
 */
class UIElementConfigImpl : public UIConfigImpl
{
public:
  /**
   * @brief Creates a new UIElementConfig handle with default values.
   *
   * @return A fully initialized UIElementConfig handle
   */
  static UIElementConfig New();

  /**
   * @brief Sets the default font size.
   *
   * @pre Must not be frozen.
   * @param[in] fontSize The default font size
   */
  void SetDefaultFontSize(float fontSize);

  /**
   * @brief Retrieves the default font size.
   *
   * @return The default font size
   */
  float GetDefaultFontSize() const;

  /**
   * @brief Sets the default text color.
   *
   * @pre Must not be frozen.
   * @param[in] color The default text color
   */
  void SetDefaultTextColor(const Vector4& color);

  /**
   * @brief Retrieves the default text color.
   *
   * @return The default text color
   */
  Vector4 GetDefaultTextColor() const;

protected:
  /**
   * @brief Constructor with default configuration values.
   */
  UIElementConfigImpl();

  /**
   * @brief Destructor.
   */
  ~UIElementConfigImpl() override;

  /**
   * @brief Registers this config with UIElementConfigManager.
   */
  void OnInitialized() override;

private:
  UIElementConfigImpl(const UIElementConfigImpl&) = delete;
  UIElementConfigImpl(UIElementConfigImpl&&) = delete;
  UIElementConfigImpl& operator=(const UIElementConfigImpl&) = delete;
  UIElementConfigImpl& operator=(UIElementConfigImpl&&) = delete;

private:
  float mDefaultFontSize;
  Vector4 mDefaultTextColor;
};

} // namespace Integration

/**
 * @brief Retrieves the UIElementConfigImpl from a UIElementConfig handle.
 *
 * @param[in] obj The UIElementConfig handle
 * @return A reference to the internal implementation
 */
inline Integration::UIElementConfigImpl& GetImpl(UIElementConfig& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UIElementConfigImpl&>(handle);
}

/**
 * @brief Retrieves the UIElementConfigImpl from a const UIElementConfig handle.
 *
 * @param[in] obj The UIElementConfig handle
 * @return A const reference to the internal implementation
 */
inline const Integration::UIElementConfigImpl& GetImpl(const UIElementConfig& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UIElementConfigImpl&>(handle);
}

} // namespace UI

} // namespace Dali
