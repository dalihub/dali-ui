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
#include <dali/public-api/math/vector4.h>

namespace Dali
{

namespace UI
{

namespace Integration
{

class UIElementConfigImpl;

/**
 * @brief Singleton manager that caches UIElementConfig values for
 *        efficient access by element classes (TextLabel, ImageView, etc.).
 *
 * Populated automatically when Apply() is called for a
 * UIElementConfig instance. Element classes should NOT call Initialize() directly;
 * it is invoked by UIElementConfigImpl::OnInitialized().
 */
class UIElementConfigManager
{
public:
  /**
   * @brief Returns the singleton UIElementConfigManager instance.
   *
   * @return The singleton instance
   */
  static UIElementConfigManager& Get();

  /**
   * @brief Called by UIElementConfigImpl::OnInitialized() to cache values.
   *
   * @param[in] impl The initialized UIElementConfigImpl
   */
  void Init(const UIElementConfigImpl& impl);

  /**
   * @brief Returns whether Init() has been called.
   *
   * @return True if initialized
   */
  bool IsInitialized() const;

  /**
   * @brief Retrieves the cached default font size.
   *
   * @pre Init() must have been called.
   * @return The default font size in points
   */
  float GetDefaultFontSize() const;

  /**
   * @brief Retrieves the cached default text color.
   *
   * @pre Init() must have been called.
   * @return The default text color
   */
  Vector4 GetDefaultTextColor() const;

private:
  UIElementConfigManager() = default;
  ~UIElementConfigManager() = default;
  UIElementConfigManager(const UIElementConfigManager&) = delete;
  UIElementConfigManager& operator=(const UIElementConfigManager&) = delete;

private:
  float mDefaultFontSize;
  Vector4 mDefaultTextColor;
  bool mInitialized{false};
};

} // namespace Integration

} // namespace UI

} // namespace Dali
