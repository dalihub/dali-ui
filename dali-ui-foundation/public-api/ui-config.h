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
#include <cstdint>
#include <string>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/key-click-policy.h>

namespace Dali
{

namespace UI
{

/**
 * @brief Function pointer type for determining whether a key triggers click execution.
 *
 * @param[in] keyName The name of the key
 * @return True if the key should trigger click execution
 */
using ExecutionKeyPredicate = bool (*)(const std::string& keyName);

namespace Integration
{
class UIConfigImpl;
}

#include "ui-config.autogen.h"
/**
 * @brief An immutable-after-init configuration object for dali-ui global settings.
 *
 * UIConfig holds display and scaling parameters that affect unit calculations
 * (spx, dp, sdp) across the entire dali-ui framework.
 *
 * Setter methods may only be called before the config is applied via Init().
 * After that point, any setter call will trigger an assertion failure.
 *
 * Setters return a reference to this object to support fluent method chaining:
 * @code
 *  int main(int argc, char** argv)
 *  {
 *    Application application = Application::New(&argc, &argv);
 *
 *    UIConfig::New()
 *       .SetScalingFactor(1.5f)
 *       .SetDpi(320)
 *       .Apply();
 *
 *    HelloWorldController test(application);
 *    application.MainLoop();
 *
 *    return 0;
 *  }
 * @endcode
 */
class DALI_UI_API UIConfig : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized UIConfig handle.
   */
  UIConfig() = default;

  /**
   * @brief Destructor.
   */
  ~UIConfig() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UIConfig(const UIConfig& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UIConfig(UIConfig&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UIConfig& operator=(const UIConfig& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UIConfig& operator=(UIConfig&& rhs) noexcept = default;

  /**
   * @brief Creates a new UIConfig with default values.
   *
   * Default: scalingFactor=1.0f, dpi=160, baselineDpi=160.
   *
   * @return An initialized UIConfig handle
   */
  static UIConfig New();

  /**
   * @brief Downcasts a handle to a UIConfig handle.
   *
   * If handle points to a UIConfig, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UIConfig or an uninitialized handle
   */
  static UIConfig DownCast(BaseHandle handle);

public: // Properties

  /**
   * @brief Applies the UIConfig as the global configuration for given application.
   *
   * This must be called exactly once, typically in main() before the application
   * main loop starts. After this call, the config is frozen and its setter methods
   * can no longer be used. Unit literals (_spx, _dp, _sdp) require this to be
   * called before they can be used.
   *
   * Calling Apply() more than once triggers an assertion failure.
   */
  void Apply();

  // @CHAIN_START(UIConfig)
  /**
   * @brief Sets the scaling factor applied to spx and sdp units.
   *
   * @pre The config must not be frozen (i.e. not yet call UIConfig::Apply()).
   * @param[in] scalingFactor The scaling factor value
   */
  UIConfig& SetScalingFactor(float scalingFactor);

  /**
   * @brief Retrieves the scaling factor.
   *
   * @return The scaling factor applied to spx and sdp units
   */
  float GetScalingFactor() const;

  /**
   * @brief Sets the screen DPI used for dp and sdp unit calculations.
   *
   * @pre The config must not be frozen.
   * @param[in] dpi The dots-per-inch value of the target display
   */
  UIConfig& SetDpi(int dpi);

  /**
   * @brief Retrieves the screen DPI.
   *
   * @return The dots-per-inch value of the target display
   */
  int GetDpi() const;

  /**
   * @brief Sets the baseline DPI used as the reference for dp calculations.
   *
   * @pre The config must not be frozen.
   * @param[in] baselineDpi The baseline dots-per-inch value
   */
  UIConfig& SetBaselineDpi(int baselineDpi);

  /**
   * @brief Retrieves the baseline DPI.
   *
   * @return The baseline dots-per-inch used as the reference for dp calculations
   */
  int GetBaselineDpi() const;

  /**
   * @brief Sets the default key click policy for clickable views.
   *
   * @pre The config must not be frozen.
   * @param[in] policy The key click policy
   */
  UIConfig& SetKeyClickPolicy(KeyClickPolicy policy);

  /**
   * @brief Retrieves the default key click policy.
   *
   * @return The key click policy
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @brief Sets the predicate function that determines which keys trigger click execution.
   *
   * The predicate receives a key name and returns true if the key should act as an
   * execution key (e.g. "Return", "KP_Enter"). The default predicate matches "Return".
   *
   * @pre The config must not be frozen.
   * @param[in] predicate A function pointer with signature bool(const std::string&)
   */
  UIConfig& SetExecutionKeyPredicate(ExecutionKeyPredicate predicate);

  /**
   * @brief Retrieves the execution key predicate.
   *
   * @return The current execution key predicate function pointer
   */
  ExecutionKeyPredicate GetExecutionKeyPredicate() const;

  /**
   * @brief Sets the minimum key repeat count to trigger a long-press via key input.
   *
   * @pre The config must not be frozen.
   * @param[in] count The minimum repeat count
   */
  UIConfig& SetMinLongPressKeyCount(uint32_t count);

  /**
   * @brief Retrieves the minimum key repeat count for long-press.
   *
   * @return The minimum repeat count
   */
  uint32_t GetMinLongPressKeyCount() const;

  /**
   * @brief Sets the tap recognizer time limit in milliseconds.
   *
   * @pre The config must not be frozen.
   * @param[in] timeMs The time limit in milliseconds
   */
  UIConfig& SetTapRecognizerTime(uint32_t timeMs);

  /**
   * @brief Retrieves the tap recognizer time limit.
   *
   * @return The time limit in milliseconds
   */
  uint32_t GetTapRecognizerTime() const;

  // @CHAIN_END

public: // Not intended for Application developers

  /**
   * @brief This constructor is used internally to wrap an implementation object.
   *
   * @param[in] impl A pointer to the internal UIConfig implementation
   */
  explicit UIConfig(Integration::UIConfigImpl* impl);
};

} // namespace UI
} // namespace Dali
