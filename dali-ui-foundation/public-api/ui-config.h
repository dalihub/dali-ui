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
#include <dali-ui-foundation/public-api/state-effect.h>
#include <dali-ui-foundation/public-api/text/text-enumerations.h>
#include <dali-ui-foundation/public-api/trait-object.h>

namespace Dali
{
namespace Ui
{
class View;

/**
 * @brief Function pointer type for determining whether a key triggers click execution.
 *
 * @param[in] keyName The name of the key
 * @return True if the key should trigger click execution
 */
using ExecutionKeyPredicate = bool (*)(const Dali::String& keyName);

/**
 * @brief Function pointer type for initializing View defaults.
 *
 * @param[in] view The View to initialize
 */
using ViewInitializer = void (*)(View view);

namespace Integration
{
class UiConfigImpl;
}

/**
 * @brief An immutable-after-init configuration object for dali-ui global settings.
 *
 * UiConfig holds display and scaling parameters that affect unit calculations
 * (spx, dp, sdp) across the entire dali-ui framework.
 *
 * Setter methods may only be called before the config is applied via Apply().
 * After that point, any setter call will trigger an assertion failure.
 *
 * @code
 *  int main(int argc, char** argv)
 *  {
 *    Application application = Application::New(&argc, &argv);
 *
 *    UiConfig config = UiConfig::New();
 *    config.SetScalingFactor(1.5f);
 *    config.SetDpi(320);
 *    config.Apply();
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

  /**
   * @brief Sets the scaling factor applied to spx and sdp units.
   *
   * @pre The config must not be frozen (i.e. not yet call UiConfig::Apply()).
   * @param[in] scalingFactor The scaling factor value
   */
  void SetScalingFactor(float scalingFactor);

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
  void SetDpi(int dpi);

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
  void SetBaselineDpi(int baselineDpi);

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
  void SetKeyClickPolicy(KeyClickPolicy policy);

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
   * @code
   *   bool MyKeyPredicate(const Dali::String& keyName) {
   *     return keyName == "Return" || keyName == "KP_Enter";
   *   }
   *   UiConfig config = UiConfig::New();
   *   config.SetExecutionKeyPredicate(MyKeyPredicate);
   *   config.Apply();
   * @endcode
   */
  void SetExecutionKeyPredicate(ExecutionKeyPredicate predicate);

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
  void SetKeyLongPressThreshold(uint32_t threshold);

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
  void SetTapRecognizerTime(uint32_t timeMs);

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
  void SetBrokenImageUrl(BrokenImageType brokenImageType, const Dali::String& brokenImageUrl);

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

  /**
   * @brief Sets whether to clear focus when the Escape key is pressed.
   *
   * @pre The config must not be frozen.
   * @param[in] enable True to enable focus clearing on Escape key
   */
  void EnableFocusClearOnEscape(bool enable);

  /**
   * @brief Retrieves whether to clear focus when the Escape key is pressed.
   *
   * @return True if focus clearing on Escape key is enabled
   */
  bool IsFocusClearOnEscapeEnabled() const;

  /**
   * @brief Sets whether touch interaction clears focus indication by default.
   *
   * @pre The config must not be frozen.
   * @param[in] clear Whether touch interaction clears focus indication from the focused view
   */
  void SetClearFocusIndicationOnTouch(bool clear);

  /**
   * @brief Gets whether touch interaction clears focus indication by default.
   *
   * @return True if touch interaction clears focus indication from the focused view
   */
  bool IsClearFocusIndicationOnTouchEnabled() const;

  /**
   * @brief Sets whether hover outside the focused view clears focus indication by default.
   *
   * @pre The config must not be frozen.
   * @param[in] clear Whether hover outside the focused view clears focus indication
   */
  void SetClearFocusIndicationOnHover(bool clear);

  /**
   * @brief Gets whether hover outside the focused view clears focus indication by default.
   *
   * @return True if hover outside the focused view clears focus indication
   */
  bool IsClearFocusIndicationOnHoverEnabled() const;

  /**
   * @brief Sets whether FocusManager's default focus indicator is enabled by default.
   *
   * @pre The config must not be frozen.
   * @param[in] enabled Whether the default focus indicator is enabled
   */
  void SetDefaultFocusIndicatorEnabled(bool enabled);

  /**
   * @brief Gets whether FocusManager's default focus indicator is enabled by default.
   *
   * @return True if the default focus indicator is enabled
   */
  bool IsDefaultFocusIndicatorEnabled() const;

  /**
   * @brief Sets the default font point-size for text elements.
   *
   * @pre The config must not be frozen.
   * @param[in] fontSize The default font size in points
   */
  void SetDefaultFontSize(float fontSize);

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
  void SetDefaultTextColor(const Vector4& color);

  /**
   * @brief Retrieves the default text color.
   *
   * @return The default text color
   */
  Vector4 GetDefaultTextColor() const;

  /**
   * @brief Sets the default placeholder text color.
   *
   * @pre The config must not be frozen.
   * @param[in] color The default placeholder text color.
   */
  void SetDefaultPlaceholderTextColor(const Vector4& color);

  /**
   * @brief Returns the default placeholder text color.
   *
   * @return The default placeholder text color.
   */
  Vector4 GetDefaultPlaceholderTextColor() const;

  /**
   * @brief Sets whether the placeholder text is shown when the control has focus.
   *
   * @pre The config must not be frozen.
   * @param[in] enabled True to show the placeholder text when focused, false otherwise.
   */
  void SetShowPlaceholderTextOnFocus(bool enabled);

  /**
   * @brief Returns whether the placeholder text is shown when the control has focus.
   *
   * @return True if the placeholder text is shown when focused, false otherwise.
   */
  bool IsPlaceholderTextShownOnFocus() const;

  /**
   * @brief Sets the marquee speed in pixels per second.
   *
   * @pre The config must not be frozen.
   * @param[in] speed The marquee speed in pixels per second
   */
  void SetMarqueeSpeed(int speed);

  /**
   * @brief Retrieves the marquee speed.
   *
   * @return The marquee speed in pixels per second
   */
  int GetMarqueeSpeed() const;

  /**
   * @brief Sets the number of complete loops for marquee.
   *
   * @pre The config must not be frozen.
   * @param[in] loopCount The number of loops
   */
  void SetMarqueeLoopCount(int loopCount);

  /**
   * @brief Retrieves the number of complete loops for marquee.
   *
   * @return The number of loops
   */
  int GetMarqueeLoopCount() const;

  /**
   * @brief Sets the amount of time to delay the start of marquee and further loops.
   *
   * @pre The config must not be frozen.
   * @param[in] delay The delay time in seconds
   */
  void SetMarqueeLoopDelay(float delay);

  /**
   * @brief Retrieves the amount of time to delay the start of marquee and further loops.
   *
   * @return The delay time in seconds
   */
  float GetMarqueeLoopDelay() const;

  /**
   * @brief Sets the gap before marquee wraps.
   *
   * @pre The config must not be frozen.
   * @param[in] gap The gap in pixels
   */
  void SetMarqueeGap(float gap);

  /**
   * @brief Retrieves the gap before marquee wraps.
   *
   * @return The gap in pixels
   */
  float GetMarqueeGap() const;

  /**
   * @brief Sets the marquee stop behaviour.
   *
   * @pre The config must not be frozen.
   * @param[in] stopMode The marquee stop mode
   */
  void SetMarqueeStopMode(Text::MarqueeStopMode stopMode);

  /**
   * @brief Retrieves the marquee stop behaviour.
   *
   * @return The marquee stop mode
   */
  Text::MarqueeStopMode GetMarqueeStopMode() const;

  /**
   * @brief Sets the marquee orientation.
   *
   * @pre The config must not be frozen.
   * @param[in] orientation The marquee orientation
   */
  void SetMarqueeOrientation(Text::MarqueeOrientation orientation);

  /**
   * @brief Retrieves the marquee orientation.
   *
   * @return The marquee orientation
   */
  Text::MarqueeOrientation GetMarqueeOrientation() const;

  /**
   * @brief Sets the default mode for resolving text layout direction.
   *
   * Text-based views use this mode unless their layout direction mode is
   * explicitly set.
   *
   * - Text::LayoutDirectionMode::CONTENTS:
   *   The layout direction is determined from the text content itself.
   *
   * - Text::LayoutDirectionMode::INHERIT:
   *   The layout direction is inherited from the parent view.
   *
   * - Text::LayoutDirectionMode::LOCALE:
   *   The layout direction is determined based on the system locale.
   *
   * @pre The config must not be frozen.
   * @param[in] mode The default LayoutDirectionMode used to determine text layout direction.
   */
  void SetTextLayoutDirectionMode(Text::LayoutDirectionMode mode);

  /**
   * @brief Gets the default mode for resolving text layout direction.
   *
   * @return The default LayoutDirectionMode used to resolve text layout direction.
   */
  Text::LayoutDirectionMode GetTextLayoutDirectionMode() const;

  /**
   * @brief Sets whether Label uses asynchronous text rendering by default.
   *
   * @pre The config must not be frozen.
   * @param[in] asyncRendering True to enable asynchronous rendering for Label by default,
   * false to use synchronous rendering.
   */
  void SetLabelAsyncRendering(bool asyncRendering);

  /**
   * @brief Retrieves whether Label uses asynchronous text rendering by default.
   *
   * @return True if asynchronous rendering is enabled for Label by default
   */
  bool IsLabelAsyncRendering() const;

  /**
   * @brief Sets the initializer called from ViewImpl::OnInitialize().
   *
   * The initializer is called for every View after the internal child-order
   * tracking signal has been connected. Passing @c nullptr restores the
   * default initializer.
   *
   * @pre The config must not be frozen.
   * @param[in] initializer The view initializer function
   */
  void SetViewInitializer(ViewInitializer initializer);

  /**
   * @brief Retrieves the initializer called from ViewImpl::OnInitialize().
   *
   * @return The current view initializer function pointer
   */
  ViewInitializer GetViewInitializer() const;

  /**
   * @brief The default View initializer.
   */
  static void DefaultViewInitializer(View view);

  /**
   * @brief Sets the default StateEffect for Views that become interactive.
   *
   * This effect is applied automatically when a View receives an interactive
   * trait, for example through View::AsInteractive() or during InteractiveView
   * initialization, unless the View already has an explicitly assigned
   * StateEffect.
   *
   * StateEffect is not limited to interactive Views. This setting only controls
   * the framework default used for Views that become interactive.
   *
   * The effect handle is stored as-is and may be shared by multiple Views. This
   * method does not clone or snapshot the effect. If an effect type exposes
   * mutable APIs, modifying the effect after setting it as the default may affect
   * Views sharing the same effect object.
   *
   * StateEffect implementations should keep per-View runtime state outside the
   * effect object.
   *
   * Pass StateEffect::None() to disable the automatic default effect for
   * interactive Views.
   *
   * @pre The config must not be frozen.
   * @param[in] effect A StateEffect, or StateEffect::None() to disable the default
   */
  void SetDefaultStateEffectForInteractive(StateEffect effect);

  /**
   * @brief Gets the default StateEffect for Views that become interactive.
   *
   * @return The default StateEffect. StateEffect::None() means no effect is
   * applied automatically when a View receives an interactive trait.
   */
  StateEffect GetDefaultStateEffectForInteractive() const;

  /**
   * @brief Sets the delay before an ambiguous touch press is applied.
   *
   * When a touch starts while a scrollable parent is still deciding whether the
   * gesture is a tap or a scroll, the pressed state of the touched interactive
   * View is delayed by this time.
   *
   * @pre The config must not be frozen.
   * @param[in] timeMs The delay in milliseconds
   */
  void SetAmbiguousPressDelay(uint32_t timeMs);

  /**
   * @brief Retrieves the delay before an ambiguous touch press is applied.
   *
   * @return The delay in milliseconds
   */
  uint32_t GetAmbiguousPressDelay() const;

  /**
   * @brief Sets how long a completed ambiguous press is kept visible.
   *
   * This duration is used only when an ambiguous press is still delayed and a
   * release or tap completes it before the press delay expires. In that case,
   * the pressed state is applied synchronously and then kept for this duration
   * before being released.
   *
   * @pre The config must not be frozen.
   * @param[in] timeMs The duration in milliseconds
   */
  void SetAmbiguousPressDuration(uint32_t timeMs);

  /**
   * @brief Retrieves how long a completed ambiguous press is kept visible.
   *
   * @return The duration in milliseconds
   */
  uint32_t GetAmbiguousPressDuration() const;

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
