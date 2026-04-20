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
#include <dali/public-api/object/base-object.h>
#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/integration-api/theme-loader-interface.h>
#include <dali-ui-foundation/public-api/trait.h>
#include <dali-ui-foundation/public-api/ui-config.h>

namespace Dali
{

namespace Ui
{

namespace Integration
{

class UiConfigImpl;
using UiConfigImplPtr = IntrusivePtr<UiConfigImpl>;

/**
 * @brief Internal implementation of UiConfig.
 *
 * Supports a "freeze" mechanism: once Freeze() is called (by UiConfigManager::Init()),
 * all subsequent setter calls will trigger DALI_ASSERT_ALWAYS.
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
   * @brief Sets the minimum key repeat count for long-press.
   *
   * @pre Must not be frozen.
   * @param[in] count The minimum repeat count
   */
  void SetKeyLongPressThreshold(uint32_t count);

  /**
   * @brief Retrieves the minimum key repeat count for long-press.
   *
   * @return The minimum repeat count
   */
  uint32_t GetKeyLongPressThreshold() const;

  /**
   * @brief Sets the tap recognizer time limit in milliseconds.
   *
   * @pre Must not be frozen.
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
   * @copydoc UiConfig::SetBrokenImageUrl
   */
  void SetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType, const Dali::String& brokenImageUrl);

  /**
   * @copydoc UiConfig::GetBrokenImageUrl
   */
  const Dali::String& GetBrokenImageUrl(UiConfig::BrokenImageType brokenImageType) const;

  /**
   * @copydoc UiConfigManager::GetBrokenImageUrlList
   */
  std::vector<Dali::String> GetBrokenImageUrlList() const;

  /**
   * @brief Sets whether to clear focus when the Escape key is pressed.
   *
   * @pre Must not be frozen.
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
   * @brief Sets whether to always show the keyboard focus indicator.
   *
   * @pre Must not be frozen.
   * @param[in] alwaysShow True to always show focus indicator
   */
  void SetAlwaysShowFocus(bool alwaysShow);

  /**
   * @brief Retrieves whether to always show the keyboard focus indicator.
   *
   * @return True if focus indicator is always shown
   */
  bool IsFocusIndicatorAlwaysShown() const;

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
   * @brief Sets the default interaction effect applied to views when AsInteractive() is called.
   *
   * @pre Must not be frozen.
   * @param[in] effect A Trait implementing InteractionEffectInterface, or Trait{} to disable the default
   */
  void SetDefaultInteractionEffect(Trait effect);

  /**
   * @brief Returns the default interaction effect.
   *
   * @return The default interaction effect handle, or an uninitialized Trait if none is set
   */
  Trait GetDefaultInteractionEffect() const;

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
   * Called internally by UiConfigManager during initialization. Derived
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
  UiConfigImpl(const UiConfigImpl&)            = delete;
  UiConfigImpl(UiConfigImpl&&)                 = delete;
  UiConfigImpl& operator=(const UiConfigImpl&) = delete;
  UiConfigImpl& operator=(UiConfigImpl&&)      = delete;

private:
  Dali::String mBrokenImageUrls[3]{}; ///< Broken image URLs for SMALL, NORMAL, LARGE

  Trait                    mDefaultInteractionEffect; ///< Default effect for interactive views
  ExecutionKeyPredicate    mExecutionKeyPredicate;
  Vector4                  mDefaultTextColor;
  float                    mScalingFactor;
  float                    mDefaultFontSize;
  int                      mDpi;
  int                      mBaselineDpi;
  int                      mMarqueeSpeed;
  int                      mMarqueeLoopCount;
  float                    mMarqueeLoopDelay;
  float                    mMarqueeGap;
  Text::MarqueeStopMode    mMarqueeStopMode;
  Text::MarqueeOrientation mMarqueeOrientation;
  KeyClickPolicy           mKeyClickPolicy;
  uint32_t                 mKeyLongPressThreshold;
  uint32_t                 mTapRecognizerTime;
  float                    mCachedDpiFactor{1.0f};
  float                    mCachedScaledDpiFactor{1.0f};
  bool                     mClearFocusOnEscape;
  bool                     mAlwaysShowFocus;
  bool                     mLabelAsyncRendering;
  bool                     mFrozen;
};

} // namespace Integration

/**
 * @brief Retrieves the UiConfigImpl from a UiConfig handle.
 *
 * @param[in] obj The UiConfig handle
 * @return A reference to the internal implementation
 */
inline Integration::UiConfigImpl& GetImpl(UiConfig& obj)
{
  BaseObject& handle = obj.GetBaseObject();
  return static_cast<Integration::UiConfigImpl&>(handle);
}

/**
 * @brief Retrieves the UiConfigImpl from a const UiConfig handle.
 *
 * @param[in] obj The UiConfig handle
 * @return A const reference to the internal implementation
 */
inline const Integration::UiConfigImpl& GetImpl(const UiConfig& obj)
{
  const BaseObject& handle = obj.GetBaseObject();
  return static_cast<const Integration::UiConfigImpl&>(handle);
}

} // namespace Ui

} // namespace Dali
