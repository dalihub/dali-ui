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

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/ui-component-config.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class UiComponentConfigImpl;
using UiComponentConfigImplPtr = IntrusivePtr<UiComponentConfigImpl>;

/**
 * @brief Internal implementation of UiComponentConfig.
 *
 * Extends UiConfigImpl as a placeholder for component-specific
 * configuration. As the dali-ui-components library grows,
 * component-specific settings can be added here.
 */
class UiComponentConfigImpl : public UiConfigImpl
{
public:
  /**
   * @brief Creates a new UiComponentConfig handle with default values.
   *
   * @return A fully initialized UiComponentConfig handle
   */
  static UiComponentConfigImplPtr New();

protected:
  /**
   * @brief Constructor with default configuration values.
   */
  UiComponentConfigImpl();

  /**
   * @brief Destructor.
   */
  ~UiComponentConfigImpl() override;

private:
  UiComponentConfigImpl(const UiComponentConfigImpl&)            = delete;
  UiComponentConfigImpl(UiComponentConfigImpl&&)                 = delete;
  UiComponentConfigImpl& operator=(const UiComponentConfigImpl&) = delete;
  UiComponentConfigImpl& operator=(UiComponentConfigImpl&&)      = delete;
};

} // namespace Integration

/**
 * @brief Retrieves the UiComponentConfigImpl from a UiComponentConfig handle.
 *
 * @param[in] obj The UiComponentConfig handle
 * @return A reference to the internal implementation
 */
inline Integration::UiComponentConfigImpl& GetImpl(UiComponentConfig& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UiComponentConfigImpl&>(handle);
}

/**
 * @brief Retrieves the UiComponentConfigImpl from a const UiComponentConfig handle.
 *
 * @param[in] obj The UiComponentConfig handle
 * @return A const reference to the internal implementation
 */
inline const Integration::UiComponentConfigImpl& GetImpl(const UiComponentConfig& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UiComponentConfigImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
