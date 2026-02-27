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
#include <dali-ui-foundation/public-api/ui-config.h>
#include <dali-ui-foundation/integration-api/ui-config-impl.h>

namespace Dali
{

namespace UI
{

namespace Integration
{

/**
 * @brief Singleton manager that holds the global UIConfig instance.
 *
 * UIConfigManager provides process-wide access to the UIConfig values
 * used by the unit system (spx, dp, sdp) and other dali-ui internals.
 *
 * Must be initialized exactly once before the application main loop starts.
 */
class DALI_UI_API UIConfigManager
{
public:
  /**
   * @brief Returns the singleton UIConfigManager instance.
   *
   * @return The singleton instance
   */
  static UIConfigManager& Get();

  /**
   * @brief Initializes the manager with the given UIConfig.
   *
   * This method must be called exactly once before the application main loop starts.
   * After this call, the UIConfig is frozen and its setter methods can no longer be used.
   * Calling Initialize() more than once triggers an assertion failure.
   *
   * @param[in] config The UIConfig to use globally
   */
  void Initialize(UIConfig config);

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

private:
  UIConfigManager() = default;
  ~UIConfigManager() = default;

  UIConfigManager(const UIConfigManager&) = delete;
  UIConfigManager& operator=(const UIConfigManager&) = delete;

private:
  UIConfig mConfig;
  ExecutionKeyPredicate mCachedExecutionKeyPredicate;
  float mCachedScalingFactor;
  float mCachedDpiFactor;
  float mCachedScaledDpiFactor;
  int mCachedDpi;
  int mCachedBaselineDpi;
  KeyClickPolicy mCachedKeyClickPolicy;
  uint32_t mCachedMinLongPressKeyCount;
  uint32_t mCachedTapRecognizerTime;
  bool mInitialized{false};
};

} // namespace Integration

} // namespace UI

} // namespace Dali
