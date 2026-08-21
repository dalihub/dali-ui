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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/base-object.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/focus-indication-policy.h>
#include <dali-ui-foundation/extension-api/theme-loader-interface.h>
#include <dali-ui-foundation/public-api/configuration/ui-config.h>
#include <dali-ui-foundation/public-api/styles/ui-style-sheet.h>
#include <dali-ui-foundation/public-api/traits/trait-object.h>
#include <dali-ui-foundation/public-api/views/effects/state-effect.h>

namespace Dali
{

namespace Ui
{

namespace Extension
{

class UiConfigImpl;
using UiConfigImplPtr = IntrusivePtr<UiConfigImpl>;

/**
 * @brief Extension-facing implementation base for UiConfig presets.
 *
 * Module extension authors can derive from this class to distribute preconfigured
 * UiConfig handles. It supports a "freeze" mechanism: once Freeze() is called
 * by UiConfig::Apply(), all subsequent setter calls will trigger
 * DALI_ASSERT_ALWAYS.
 */
class DALI_UI_API UiConfigImpl : public BaseObject
{
public:
  /**
   * @brief Creates a new UiConfig implementation.
   *
   * @return An IntrusivePtr to the new UiConfigImpl
   */
  static UiConfigImplPtr New();

  /**
   * @brief Marks this config as frozen.
   *
   * Setter calls after this point will trigger an assertion failure.
   */
  void Freeze();

  /**
   * @brief Returns whether this config is frozen.
   *
   * @return True if frozen
   */
  bool IsFrozen() const;

  /**
   * @brief Retrieves the DPI factor (dpi / baselineDpi).
   *
   * This value is cached when Freeze() is called.
   *
   * @pre Must be frozen.
   * @return The computed DPI factor
   */
  float GetDpiFactor() const;

  /**
   * @brief Retrieves the scaled DPI factor (dpiFactor * scalingFactor).
   *
   * This value is cached when Freeze() is called.
   *
   * @pre Must be frozen.
   * @return The computed scaled DPI factor
   */
  float GetScaledDpiFactor() const;

  /**
   * @brief Sets the scaling factor.
   *
   * @pre Must not be frozen.
   * @param[in] scalingFactor The scaling factor
   */
  void SetScalingFactor(float scalingFactor);

  /**
   * @brief Retrieves the scaling factor.
   *
   * @return The scaling factor
   */
  float GetScalingFactor() const;

  /**
   * @brief Sets the screen DPI.
   *
   * @pre Must not be frozen.
   * @param[in] dpi The DPI value
   */
  void SetDpi(int dpi);

  /**
   * @brief Retrieves the screen DPI.
   *
   * @return The DPI value
   */
  int GetDpi() const;

  /**
   * @brief Sets the baseline DPI.
   *
   * @pre Must not be frozen.
   * @param[in] baselineDpi The baseline DPI
   */
  void SetBaselineDpi(int baselineDpi);

  /**
   * @brief Retrieves the baseline DPI.
   *
   * @return The baseline DPI value
   */
  int GetBaselineDpi() const;

  /**
   * @brief Sets the default key click policy.
   *
   * @pre Must not be frozen.
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
   * @brief Sets the execution key predicate.
   *
   * @pre Must not be frozen.
   * @param[in] predicate A function pointer with signature bool(const Dali::String&)
   */
  void SetExecutionKeyPredicate(ExecutionKeyPredicate predicate);

  /**
   * @brief Retrieves the execution key predicate.
   *
   * @return The current execution key predicate function pointer
   */
  ExecutionKeyPredicate GetExecutionKeyPredicate() const;

  /**
   * @copydoc UiConfig::SetLongPressKeyEventMinimumCount
   */
  void SetLongPressKeyEventMinimumCount(uint32_t count);

  /**
   * @copydoc UiConfig::GetLongPressKeyEventMinimumCount
   */
  uint32_t GetLongPressKeyEventMinimumCount() const;

  /**
   * @copydoc UiConfig::SetLongPressGestureMinimumHoldingTime
   */
  void SetLongPressGestureMinimumHoldingTime(uint32_t timeMs);

  /**
   * @copydoc UiConfig::GetLongPressGestureMinimumHoldingTime
   */
  uint32_t GetLongPressGestureMinimumHoldingTime() const;

  /**
   * @copydoc UiConfig::SetTapGestureMaximumMultiTapInterval
   */
  void SetTapGestureMaximumMultiTapInterval(uint32_t intervalMs);

  /**
   * @copydoc UiConfig::GetTapGestureMaximumMultiTapInterval
   */
  uint32_t GetTapGestureMaximumMultiTapInterval() const;

  /**
   * @copydoc UiConfig::SetTapGestureMaximumHoldingTime
   */
  void SetTapGestureMaximumHoldingTime(uint32_t timeMs);

  /**
   * @copydoc UiConfig::GetTapGestureMaximumHoldingTime
   */
  uint32_t GetTapGestureMaximumHoldingTime() const;

  /**
   * @copydoc UiConfig::SetTapGestureMaximumMotionDistance
   */
  void SetTapGestureMaximumMotionDistance(float distance);

  /**
   * @copydoc UiConfig::GetTapGestureMaximumMotionDistance
   */
  float GetTapGestureMaximumMotionDistance() const;

  /**
   * @copydoc UiConfig::SetBrokenImageUrl
   */
  void SetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType, const Dali::String& brokenImageUrl);

  /**
   * @copydoc UiConfig::GetBrokenImageUrl
   */
  const Dali::String& GetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType) const;

  /**
   * @brief Sets whether to clear focus when the Escape key is pressed.
   *
   * @pre Must not be frozen.
   * @param[in] enable True to enable focus clearing on Escape key
   */
  void SetClearFocusOnEscapeEnabled(bool enable);

  /**
   * @brief Retrieves whether to clear focus when the Escape key is pressed.
   *
   * @return True if focus clearing on Escape key is enabled
   */
  bool IsClearFocusOnEscapeEnabled() const;

  /**
   * @copydoc UiConfig::SetClearFocusIndicationOnTouch
   */
  void SetClearFocusIndicationOnTouch(bool clear);

  /**
   * @copydoc UiConfig::IsClearFocusIndicationOnTouchEnabled
   */
  bool IsClearFocusIndicationOnTouchEnabled() const;

  /**
   * @copydoc UiConfig::SetClearFocusIndicationOnHover
   */
  void SetClearFocusIndicationOnHover(bool clear);

  /**
   * @copydoc UiConfig::IsClearFocusIndicationOnHoverEnabled
   */
  bool IsClearFocusIndicationOnHoverEnabled() const;

  /**
   * @copydoc UiConfig::SetDefaultFocusIndicatorEnabled
   */
  void SetDefaultFocusIndicatorEnabled(bool enabled);

  /**
   * @copydoc UiConfig::IsDefaultFocusIndicatorEnabled
   */
  bool IsDefaultFocusIndicatorEnabled() const;

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
   * @copydoc UiConfig::SetDefaultSystemFontSizeScaleEnabled
   */
  void SetDefaultSystemFontSizeScaleEnabled(bool enabled);

  /**
   * @copydoc UiConfig::IsDefaultSystemFontSizeScaleEnabled
   */
  bool IsDefaultSystemFontSizeScaleEnabled() const;

  /**
   * @copydoc UiConfig::SetDefaultMinimumFontSizeScale
   */
  void SetDefaultMinimumFontSizeScale(float scale);

  /**
   * @copydoc UiConfig::GetDefaultMinimumFontSizeScale
   */
  float GetDefaultMinimumFontSizeScale() const;

  /**
   * @copydoc UiConfig::SetDefaultMaximumFontSizeScale
   */
  void SetDefaultMaximumFontSizeScale(float scale);

  /**
   * @copydoc UiConfig::GetDefaultMaximumFontSizeScale
   */
  float GetDefaultMaximumFontSizeScale() const;

  /**
   * @copydoc UiConfig::SetScaleForSystemFontSize
   */
  void SetScaleForSystemFontSize(UiConfig::SystemFontSize fontSize, float scale);

  /**
   * @copydoc UiConfig::GetScaleForSystemFontSize
   */
  float GetScaleForSystemFontSize(UiConfig::SystemFontSize fontSize) const;

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
   * @pre Must not be frozen.
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
   * @pre Must not be frozen.
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
   * @pre Must not be frozen.
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
   * @pre Must not be frozen.
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
   * @pre Must not be frozen.
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
   * @pre Must not be frozen.
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
   * @copydoc UiConfig::SetViewInitializer
   */
  void SetViewInitializer(ViewInitializer initializer);

  /**
   * @copydoc UiConfig::GetViewInitializer
   */
  ViewInitializer GetViewInitializer() const;

  /**
   * @copydoc UiConfig::SetDefaultStateEffectForInteractive()
   */
  void SetDefaultStateEffectForInteractive(StateEffect effect);

  /**
   * @copydoc UiConfig::GetDefaultStateEffectForInteractive()
   */
  StateEffect GetDefaultStateEffectForInteractive() const;

  /**
   * @copydoc UiConfig::ResetStyleSheet()
   */
  void ResetStyleSheet(UiStyleSheet styleSheet);

  /**
   * @copydoc UiConfig::StyleSheet()
   */
  UiStyleSheet StyleSheet() const;

  /**
   * @copydoc UiConfig::SetAmbiguousPressDelay()
   */
  void SetAmbiguousPressDelay(uint32_t timeMs);

  /**
   * @copydoc UiConfig::GetAmbiguousPressDelay()
   */
  uint32_t GetAmbiguousPressDelay() const;

  /**
   * @copydoc UiConfig::SetAmbiguousPressDuration()
   */
  void SetAmbiguousPressDuration(uint32_t timeMs);

  /**
   * @copydoc UiConfig::GetAmbiguousPressDuration()
   */
  uint32_t GetAmbiguousPressDuration() const;

  /**
   * @copydoc UiConfig::SetWebEngineType()
   */
  void SetWebEngineType(WebEngineType type);

  /**
   * @copydoc UiConfig::GetWebEngineType()
   */
  WebEngineType GetWebEngineType() const;

  /**
   * @brief Sets the policy used to resolve automatic focus indication changes.
   *
   * @param[in] policy The policy function, or nullptr to restore the default policy
   */
  void SetFocusIndicationPolicy(FocusIndicationPolicy::Function policy);

  /**
   * @brief Gets the policy used to resolve automatic focus indication changes.
   *
   * @return The configured policy function
   */
  FocusIndicationPolicy::Function GetFocusIndicationPolicy() const;

  /**
   * @brief Called after this config is applied via UiConfig::Apply().
   *
   * Derived config implementations override this to register themselves
   * with their own layer-specific config manager.
   */
  virtual void OnApplied();

  /**
   * @brief Called after the application ready to apply some options using config.
   *
   * Derived config implementations override this to register themselves
   * with their own layer-specific config manager.
   */
  virtual void OnApplicationCreated();

  /**
   * @brief Creates a ThemeLoaderInterface instance.
   *
   * Called internally by UiThemeManagerImpl while creating the theme loader. Derived
   * implementations override this to provide a custom theme loader.
   * The framework takes ownership of the returned pointer.
   *
   * @return A ThemeLoaderInterface instance, or nullptr to use the default loader
   */
  virtual ThemeLoaderInterface* CreateThemeLoader();

protected:
  /**
   * @brief Constructor with default configuration values.
   */
  UiConfigImpl();

  /**
   * @brief Destructor.
   */
  ~UiConfigImpl() override;

private:
  /**
   * @brief Applies the gesture recognition options to DALi.
   *
   * Only the options that the application has explicitly set are applied; the others
   * keep the DALi default and are cached so that the getters report the effective value.
   */
  void ApplyGestureOptions();

private:
  UiConfigImpl(const UiConfigImpl&)            = delete;
  UiConfigImpl(UiConfigImpl&&)                 = delete;
  UiConfigImpl& operator=(const UiConfigImpl&) = delete;
  UiConfigImpl& operator=(UiConfigImpl&&)      = delete;

private:
  class Impl;
  Impl* mImpl;
};

} // namespace Extension

/**
 * @brief Retrieves the UiConfigImpl from a UiConfig handle.
 *
 * @param[in] obj The UiConfig handle
 * @return A reference to the internal implementation
 */
inline Extension::UiConfigImpl& GetImpl(UiConfig& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Extension::UiConfigImpl&>(handle);
}

/**
 * @brief Retrieves the UiConfigImpl from a const UiConfig handle.
 *
 * @param[in] obj The UiConfig handle
 * @return A const reference to the internal implementation
 */
inline const Extension::UiConfigImpl& GetImpl(const UiConfig& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Extension::UiConfigImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
