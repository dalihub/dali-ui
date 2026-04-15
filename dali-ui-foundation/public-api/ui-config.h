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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/key-click-policy.h>
#include <dali-ui-foundation/public-api/trait.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Function pointer type for determining whether a key triggers click execution.
 *
 * @param[in] keyName The name of the key
 * @return True if the key should trigger click execution
 */
using ExecutionKeyPredicate = bool (*)(const Dali::String& keyName);

namespace Integration
{
class UiConfigImpl;
}

#include "ui-config.autogen.h"
/**
 * @brief An immutable-after-init configuration object for dali-ui global settings.
 *
 * UiConfig holds display and scaling parameters that affect unit calculations
 * (spx, dp, sdp) across the entire dali-ui framework.
 *
 * Setter methods may only be called before the config is applied via Apply().
 * After that point, any setter call will trigger an assertion failure.
 *
 * Setters return a reference to this object to support fluent method chaining:
 * @code
 *  int main(int argc, char** argv)
 *  {
 *    Application application = Application::New(&argc, &argv);
 *
 *    UiConfig::New()
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
class DALI_UI_API UiConfig : public BaseHandle
{
public:
  /**
   * @brief The type of broken image for image loading failures.
   *
   * Different broken image types allow different broken images to be used
   * based on the size of the view that needs to display them.
   * For example, a small icon view can use a compact broken image (SMALL),
   * while a large image view can use a more detailed broken image (LARGE).
   */
  enum class BrokenImageType
  {
    SMALL  = 0, ///< Broken image for small-sized views
    NORMAL = 1, ///< Broken image for normal-sized views
    LARGE  = 2  ///< Broken image for large-sized views
  };

public:
  /**
   * @brief Creates an uninitialized UiConfig handle.
   */
  UiConfig() = default;

  /**
   * @brief Destructor.
   */
  ~UiConfig() = default;

  /**
   * @brief Copy constructor.
   *
   * @param[in] handle Handle to copy
   */
  UiConfig(const UiConfig& handle) = default;

  /**
   * @brief Move constructor.
   *
   * @param[in] rhs Handle to move
   */
  UiConfig(UiConfig&& rhs) noexcept = default;

  /**
   * @brief Copy assignment operator.
   *
   * @param[in] handle Object to assign this to
   * @return Reference to this
   */
  UiConfig& operator=(const UiConfig& handle) = default;

  /**
   * @brief Move assignment operator.
   *
   * @param[in] rhs Object to assign this to
   * @return Reference to this
   */
  UiConfig& operator=(UiConfig&& rhs) noexcept = default;

  /**
   * @brief Creates a new UiConfig with default values.
   *
   * Default: scalingFactor=1.0f, dpi=160, baselineDpi=160.
   *
   * @return An initialized UiConfig handle
   */
  static UiConfig New();

  /**
   * @brief Downcasts a handle to a UiConfig handle.
   *
   * If handle points to a UiConfig, the downcast produces a valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a UiConfig or an uninitialized handle
   */
  static UiConfig DownCast(BaseHandle handle);

public: // Properties
  /**
   * @brief Applies the UiConfig as the global configuration for given application.
   *
   * This must be called exactly once, typically in main() before the application
   * main loop starts. After this call, the config is frozen and its setter methods
   * can no longer be used. Unit literals (_spx, _dp, _sdp) require this to be
   * called before they can be used.
   *
   * Calling Apply() more than once triggers an assertion failure.
   */
  void Apply();

  // @CHAIN_START(UiConfig)
  /**
   * @brief Sets the scaling factor applied to spx and sdp units.
   *
   * @pre The config must not be frozen (i.e. not yet call UiConfig::Apply()).
   * @param[in] scalingFactor The scaling factor value
   */
  UiConfig& SetScalingFactor(float scalingFactor);

  /**
   * @brief Retrieves the scaling factor.
   *
   * @return The scaling factor applied to spx and sdp units
   */
  float GetScalingFactor() const;

  /**
   * @brief Retrieves the DPI factor (dpi / baselineDpi).
   *
   * @pre The config must be frozen (i.e. Apply() must have been called).
   * @return The computed DPI factor
   */
  float GetDpiFactor() const;

  /**
   * @brief Retrieves the scaled DPI factor (dpiFactor * scalingFactor).
   *
   * @pre The config must be frozen (i.e. Apply() must have been called).
   * @return The computed scaled DPI factor
   */
  float GetScaledDpiFactor() const;

  /**
   * @brief Sets the screen DPI used for dp and sdp unit calculations.
   *
   * @pre The config must not be frozen.
   * @param[in] dpi The dots-per-inch value of the target display
   */
  UiConfig& SetDpi(int dpi);

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
  UiConfig& SetBaselineDpi(int baselineDpi);

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
  UiConfig& SetKeyClickPolicy(KeyClickPolicy policy);

  /**
   * @brief Retrieves the default key click policy.
   *
   * @return The key click policy
   */
  KeyClickPolicy GetKeyClickPolicy() const;

  /**
   * @brief Sets the predicate used to determine whether a key event triggers
   * click execution (e.g. "Return", "KP_Enter").
   *
   * When a key event is received by a clickable View, this predicate is called
   * with the key name. If it returns @c true, the View treats the key as a
   * click execution key and fires the click action. The default predicate
   * matches "Return" only.
   *
   * @note This parameter is a plain function pointer (ExecutionKeyPredicate).
   * Lambdas with captures and member function pointers are not accepted.
   * Only free functions or stateless lambdas may be used.
   *
   * @note Passing @c nullptr restores the default predicate that matches "Return".
   *
   * @pre The config must not be frozen (i.e. Apply() must not have been called).
   * Calling this method after Apply() triggers an assert in debug builds and
   * is ignored in release builds.
   *
   * @param[in] predicate A function pointer matching the ExecutionKeyPredicate
   *                      signature, or @c nullptr to restore the default
   * @return Reference to this UiConfig for method chaining
   *
   * @code
   *   bool MyKeyPredicate(const Dali::String& keyName) {
   *     return keyName == "Return" || keyName == "KP_Enter";
   *   }
   *   UiConfig::New()
   *     .SetExecutionKeyPredicate(MyKeyPredicate)
   *     .Apply();
   * @endcode
   */
  UiConfig& SetExecutionKeyPredicate(ExecutionKeyPredicate predicate);

  /**
   * @brief Retrieves the execution key predicate.
   *
   * @return The current execution key predicate function pointer
   */
  ExecutionKeyPredicate GetExecutionKeyPredicate() const;

  /**
   * @brief Sets the threshold for recognizing a key long-press.
   *
   * When a key is held down, the system generates repeated key press events.
   * This threshold defines how many consecutive key press events must be
   * received before the input is recognized as a long-press action.
   *
   * @pre The config must not be frozen.
   * @param[in] threshold The minimum number of consecutive key press events
   *                      required to trigger a long-press
   */
  UiConfig& SetKeyLongPressThreshold(uint32_t threshold);

  /**
   * @brief Retrieves the threshold for recognizing a key long-press.
   *
   * @return The minimum number of consecutive key press events required
   *         to trigger a long-press
   */
  uint32_t GetKeyLongPressThreshold() const;

  /**
   * @brief Sets the tap recognizer time limit in milliseconds.
   *
   * @pre The config must not be frozen.
   * @param[in] timeMs The time limit in milliseconds
   */
  UiConfig& SetTapRecognizerTime(uint32_t timeMs);

  /**
   * @brief Retrieves the tap recognizer time limit.
   *
   * @return The time limit in milliseconds
   */
  uint32_t GetTapRecognizerTime() const;

  /**
   * @brief Sets an image to be displayed when image loading fails.
   *
   * This method configures a broken image that will be shown in image views (e.g., ImageView)
   * when the requested image fails to load properly. Using different broken image types
   * allows for appropriate broken images based on the target view size.
   *
   * @pre The config must not be frozen.
   * @param[in] brokenImageType The type of broken image (SMALL, NORMAL, or LARGE)
   * @param[in] brokenImageUrl The URL of the broken image to use
   */
  UiConfig& SetBrokenImageUrl(BrokenImageType brokenImageType, const Dali::String& brokenImageUrl);

  /**
   * @brief Gets the image URL to be displayed when image loading fails.
   *
   * Retrieves the broken image URL that was previously set for the specified broken image type.
   * This image will be shown in image views when the requested image fails to load.
   *
   * @param[in] brokenImageType The type of broken image (SMALL, NORMAL, or LARGE)
   * @return A reference to the broken image URL string
   */
  const Dali::String& GetBrokenImageUrl(BrokenImageType brokenImageType) const;

  // @CHAIN_MANUAL
  /**
   * @brief Sets whether to clear focus when the Escape key is pressed.
   *
   * @pre The config must not be frozen.
   * @param[in] enable True to enable focus clearing on Escape key
   * @return Reference to this for method chaining
   */
  UiConfig& EnableFocusClearOnEscape(bool enable);

  /**
   * @brief Retrieves whether to clear focus when the Escape key is pressed.
   *
   * @return True if focus clearing on Escape key is enabled
   */
  bool IsFocusClearOnEscapeEnabled() const;

  /**
   * @brief Sets whether to always show the keyboard focus indicator.
   *
   * @pre The config must not be frozen.
   * @param[in] alwaysShow True to always show focus indicator
   * @return Reference to this for method chaining
   */
  UiConfig& SetAlwaysShowFocus(bool alwaysShow);

  /**
   * @brief Retrieves whether to always show the keyboard focus indicator.
   *
   * @return True if focus indicator is always shown
   */
  bool IsFocusIndicatorAlwaysShown() const;

  /**
   * @brief Sets the default font point-size for text elements.
   *
   * @pre The config must not be frozen.
   * @param[in] fontSize The default font size in points
   */
  UiConfig& SetDefaultFontSize(float fontSize);

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
  UiConfig& SetDefaultTextColor(const Vector4& color);

  /**
   * @brief Retrieves the default text color.
   *
   * @return The default text color
   */
  Vector4 GetDefaultTextColor() const;

  /**
   * @brief Sets the default interaction effect applied to views when AsInteractive() is called.
   *
   * @pre The config must not be frozen.
   * @param[in] effect A Trait implementing InteractionEffectInterface, or Trait{} to disable the default
   * @return Reference to this for method chaining
   */
  UiConfig& SetDefaultInteractionEffect(Trait effect);

  /**
   * @brief Returns the default interaction effect.
   *
   * @return The default interaction effect handle, or an uninitialized Trait if none is set
   */
  Trait GetDefaultInteractionEffect() const;

  // @CHAIN_END

public: // Not intended for Application developers
  /**
   * @brief This constructor is used internally to wrap an implementation object.
   *
   * @param[in] impl A pointer to the internal UiConfig implementation
   */
  explicit UiConfig(Integration::UiConfigImpl* impl);
};

} // namespace Ui
} // namespace Dali
