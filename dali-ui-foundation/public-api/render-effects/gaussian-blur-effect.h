#ifndef DALI_UI_BLUR_EFFECT_H
#define DALI_UI_BLUR_EFFECT_H
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
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/animation/time-period.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/render-effects/render-effect.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class GaussianBlurEffectImpl;
} // namespace Internal DALI_INTERNAL

/**
 * @brief GaussianBlurEffect is a visual effect that blurs owner view and its children.
 *
 * @code
 * GaussianBlurEffect effect = GaussianBlurEffect::New();
 * view.SetRenderEffect(effect); // Set on the view, activated automatically
 * effect.Deactivate();          // Turn off, but stay set on the view
 * effect.Activate();            // Turn back on
 * view.ClearRenderEffect();     // Remove from the view, deactivated automatically
 * @endcode
 */
class DALI_UI_API GaussianBlurEffect : public RenderEffect
{
public:
  /**
   * @brief Typedef for finished signals sent by this class.
   */
  typedef Signal<void()> FinishedSignalType;

  /**
   * @brief Creates an initialized GaussianBlurEffect, with default blur radius 40u.
   * @return A handle to a newly allocated Dali resource
   */
  static GaussianBlurEffect New();

  /**
   * @brief Creates an initialized GaussianBlurEffect.
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @return A handle to a newly allocated Dali resource
   * @note Due to the downscale factor and kernel optimizations, the actual applied blur changes only in fixed
   * increments.
   * @note The increment size is (2 / downscaleFactor). For example, with downscaleFactor = 0.25, the effective blur
   * radius changes in steps of 8 pixels, so small adjustments below that threshold will have no visual effect.
   */
  static GaussianBlurEffect New(uint32_t blurRadius);

  /**
   * @brief Creates an uninitialized blur effect.
   */
  GaussianBlurEffect();

  /**
   * @brief Copy constructor
   */
  GaussianBlurEffect(const GaussianBlurEffect& handle);

  /**
   * @brief Destructor
   */
  ~GaussianBlurEffect();

  /**
   * @brief Set whether to refresh effect once or every frame.
   * @param[in] blurOnce If true, renders effect once, else updates effect rendering every frame.
   */
  void SetBlurOnce(bool blurOnce);

  /**
   * @brief Retrieves whether effect rendering is done once(true) or every frame(false)
   * @return Whether blur is rendered once or every frame.
   */
  bool GetBlurOnce() const;

  /**
   * @brief Set blur radius value.
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @note Due to the downscale factor and kernel optimizations, the actual applied blur changes only in fixed
   * increments.
   * @note The increment size is (2 / downscaleFactor). For example, with downscaleFactor = 0.25, the effective blur
   * radius changes in steps of 8 pixels, so small adjustments below that threshold will have no visual effect.
   */
  void SetBlurRadius(uint32_t blurRadius);

  /**
   * @brief Retrieves blur radius value.
   * @return The blur radius.
   */
  uint32_t GetBlurRadius() const;

  /**
   * @brief Set blur downscale factor. The value is clamped to the range [0.25f, 1.0f].
   * @param[in] downscaleFactor The downscale factor for input texture.
   */
  void SetBlurDownscaleFactor(float downscaleFactor);

  /**
   * @brief Retrieves blur downscale factor.
   * @return The blur downscale factor.
   */
  float GetBlurDownscaleFactor() const;

  /**
   * @brief Sets the dithering noise strength of the final blur output.
   *
   * The value is clamped to the range [0.0f, 1.0f].
   * 0.0f disables dithering noise, while 1.0f applies the maximum noise strength.
   * The default value is 0.1f.
   *
   * @param[in] strength The dithering noise strength.
   */
  void SetDitherNoiseStrength(float strength);

  /**
   * @brief Gets the dithering noise strength of the final blur output.
   *
   * @return The dithering noise strength.
   */
  float GetDitherNoiseStrength() const;

  /**
   * @brief Adds blur strength animation. Blurifies clear texture within given animation parameters.
   * @param[in] animation Animation instance to which we add blur strength animation.
   * @param[in] alphaFunction AlphaFunction of blur strength animation.
   * @param[in] timePeriod TimePeriod of blur strength animation. Default value is animation's duration.
   * @param[in] fromValue Starting value of blur strength. Must be in range of [0.0f, 1.0f]
   * @param[in] toValue End value of blur strength. Must be in range of [0.0f, 1.0f]
   * @note If toValue is smaller than fromValue, animation would show reversed(blurred->clarified) animation.
   * @note When choosing alpha function, note that gaussian curve itself is innately non-linear.
   */
  void AddBlurStrengthAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod,
                                float fromValue, float toValue);

  /**
   * @brief Adds blur opacity animation. Blurifies clear texture within given animation parameters.
   * @param[in] animation Animation instance to which we add blur opacity animation.
   * @param[in] alphaFunction AlphaFunction of blur opacity animation.
   * @param[in] timePeriod TimePeriod of blur opacity animation. Default value is animation's duration.
   * @param[in] fromValue Starting value of blur opacity. Must be in range of [0.0f, 1.0f]
   * @param[in] toValue End value of blur opacity. Must be in range of [0.0f, 1.0f]
   * @note If toValue is smaller than fromValue, animation would show reversed(blurred->clarified) animation.
   * @note When choosing alpha function, note that gaussian curve itself is innately non-linear.
   */
  void AddBlurOpacityAnimation(Animation& animation, AlphaFunction alphaFunction, TimePeriod timePeriod,
                               float fromValue, float toValue);

public: // Signals
  /**
   * @brief Connect to this signal to be notified when the blur has been rendered.
   *
   * Only emitted while blur-once is enabled and the effect is activated, since a blur that
   * refreshes every frame never finishes. The offscreen resources are released right after the
   * signal is emitted.
   *
   * @return The finished signal
   */
  FinishedSignalType& FinishedSignal();

public: // Not intended for use by Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Ui::Internal implementation.
   * @param[in] gaussianBlurEffectImpl The GaussianBlurEffect implementation.
   */
  explicit DALI_INTERNAL GaussianBlurEffect(Internal::GaussianBlurEffectImpl* gaussianBlurEffectImpl);
  ///@endcond
};
} // namespace Ui
} // namespace Dali
#endif // DALI_UI_BLUR_EFFECT_H
