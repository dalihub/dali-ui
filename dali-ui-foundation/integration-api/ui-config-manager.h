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
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/ui-config-impl.h>
#include <dali-ui-foundation/public-api/ui-config.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

/**
 * @brief Singleton manager that holds the global UiConfig instance.
 *
 * UiConfigManager provides process-wide access to the UiConfig values
 * used by the unit system (spx, dp, sdp) and other dali-ui internals.
 *
 * Must be initialized exactly once before the application main loop starts.
 */
class DALI_UI_API UiConfigManager
{
public:
  /**
   * @brief Returns the singleton UiConfigManager instance.
   *
   * @return The singleton instance
   */
  static UiConfigManager& Get();

  /**
   * @brief Initializes the manager with the given UiConfig.
   *
   * This method must be called exactly once before the application main loop starts.
   * After this call, the UiConfig is frozen and its setter methods can no longer be used.
   * Calling Initialize() more than once triggers an assertion failure.
   *
   * @param[in] config The UiConfig to use globally
   */
  void Initialize(const UiConfig& config);

  /**
   * @brief Returns whether Init() has been called.
   *
   * @return True if the manager has been initialized
   */
  bool IsInitialized() const;

  /**
   * @brief Retrieves the scaling factor from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The scaling factor
   */
  float GetScalingFactor() const;

  /**
   * @brief Retrieves the DPI factor (dpi / baselineDpi) from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The computed DPI factor
   */
  float GetDpiFactor() const;

  /**
   * @brief Retrieves the scaled DPI factor (dpiFactor * scalingFactor) from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The computed scaled DPI factor
   */
  float GetScaledDpiFactor() const;

  /**
   * @brief Retrieves the screen DPI from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The DPI value
   */
  int GetDpi() const;

  /**
   * @brief Retrieves the baseline DPI from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The baseline DPI value
   */
  int GetBaselineDpi() const;

  /**
   * @brief Retrieves the default key click policy from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The key click policy
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @brief Retrieves the execution key predicate from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The execution key predicate function pointer
   */
  ExecutionKeyPredicate GetExecutionKeyPredicate() const;

  /**
   * @brief Retrieves the minimum key repeat count for long-press from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The minimum repeat count
   */
  uint32_t GetMinLongPressKeyCount() const;

  /**
   * @brief Retrieves the tap recognizer time limit from the current config.
   *
   * @pre Init() must have been called. Triggers assertion failure otherwise.
   * @return The time limit in milliseconds
   */
  uint32_t GetTapRecognizerTime() const;

  /**
   * @brief Retrieves the list of broken image URLs for all types.
   *
   * Returns a vector containing broken image URLs in the order of SMALL, NORMAL, and LARGE types.
   * Empty URL strings (not set values) are included in the list to maintain the order.
   *
   * For example, if only NORMAL type is set with "broken_normal.png", the list would be:
   * - Index 0: "" (SMALL not set)
   * - Index 1: "broken_normal.png" (NORMAL set)
   * - Index 2: "" (LARGE not set)
   *
   * @pre Initialize() must be called. Triggers assertion failure otherwise.
   * @return A vector of broken image URLs in order of SMALL, NORMAL, LARGE
   */
  std::vector<std::string> GetBrokenImageUrlList() const;

  /**
   * @brief Retrieves whether to clear focus when the Escape key is pressed.
   *
   * @pre Initialize() must be called. Triggers assertion failure otherwise.
   * @return True if focus clearing on Escape key is enabled
   */
  bool IsFocusClearOnEscapeEnabled() const;

  /**
   * @brief Retrieves whether to always show the keyboard focus indicator.
   *
   *pre Initialize() must be called. Triggers assertion failure otherwise.
   * @return True if focus indicator is always shown
   */
  bool IsFocusIndicatorAlwaysShown() const;

  /**
   * @brief Retrieves the cached default font size.
   *
   * @pre Initialize() must have been called.
   * @return The default font size in points
   */
  float GetDefaultFontSize() const;

  /**
   * @brief Retrieves the cached default text color.
   *
   * @pre Initialize() must have been called.
   * @return The default text color
   */
  Vector4 GetDefaultTextColor() const;

  /*
   * @brief Creates a ThemeLoaderInterface instance from the current config.
   *
   * Forwards to UiConfigImpl::CreateThemeLoader(). Called internally by
   * UiThemeManagerImpl during on-demand loader creation.
   *
   * @pre Init() must have been called.
   * @return A new ThemeLoaderInterface instance. Caller takes ownership.
   */
  ThemeLoaderInterface* CreateThemeLoader();

private:
  UiConfigManager()  = default;
  ~UiConfigManager() = default;

  UiConfigManager(const UiConfigManager&)            = delete;
  UiConfigManager& operator=(const UiConfigManager&) = delete;

private:
  UiConfig              mConfig;
  // Cached derived factors for the unit system (spx/dp/sdp).
  // These are frequently queried in hot paths (e.g. unit literals and layout scaling).
  // UiConfig becomes immutable after Initialize() (Freeze()), so caching is safe.
  float mCachedScalingFactor{1.0f};
  float mCachedDpiFactor{1.0f};
  float mCachedScaledDpiFactor{1.0f};
  bool                  mInitialized{false};
};

} // namespace Integration

} // namespace Ui

} // namespace Dali
