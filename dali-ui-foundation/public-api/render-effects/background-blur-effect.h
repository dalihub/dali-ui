#ifndef DALI_UI_BACKGROUND_BLUR_EFFECT_H
#define DALI_UI_BACKGROUND_BLUR_EFFECT_H
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
#include <dali-ui-foundation/public-api/views/view.h>

namespace Dali
{
namespace Ui
{
namespace Internal DALI_INTERNAL
{
class BackgroundBlurEffectImpl;
} // namespace Internal DALI_INTERNAL

/**
 * @brief BackgroundBlurEffect is a visual effect that blurs owner view's background.
 *
 * @code
 * BackgroundBlurEffect effect = BackgroundBlurEffect::New();
 * view.SetRenderEffect(effect); // Set on the view, activated automatically
 * effect.Deactivate();          // Turn off, but stay set on the view
 * effect.Activate();            // Turn back on
 * view.ClearRenderEffect();     // Remove from the view, deactivated automatically
 * @endcode
 *
 * @note The owner view owns at most one render effect.
 * @note Tree hierarchy matters for BackgroundBlurEffect. You should determine "what is the background".
 */
class DALI_UI_API BackgroundBlurEffect : public RenderEffect
{
public:
  /**
   * @brief Typedef for finished signals sent by this class.
   */
  typedef Signal<void()> FinishedSignalType;

  /**
   * @brief Creates an initialized BackgroundBlurEffect, using default settings. As default, blur radius is set to 40u.
   * @return A handle to a newly allocated Dali resource
   */
  static BackgroundBlurEffect New();

  /**
   * @brief Creates an initialized BackgroundBlurEffect.
   * @param[in] blurRadius The radius of Gaussian kernel.
   * @return A handle to a newly allocated Dali resource
   * @note Due to the downscale factor and kernel optimizations, the actual applied blur changes only in fixed
   * increments.
   * @note The increment size is (2 / downscaleFactor). For example, with downscaleFactor = 0.25, the effective blur
   * radius changes in steps of 8 pixels, so small adjustments below that threshold will have no visual effect.
   */
  static BackgroundBlurEffect New(uint32_t blurRadius);

  /**
   * @brief Creates an uninitialized blur effect.
   */
  BackgroundBlurEffect();

  /**
   * @brief Copy constructor.
   */
  BackgroundBlurEffect(const BackgroundBlurEffect& handle);

  /**
   * @brief Destructor
   */
  ~BackgroundBlurEffect();

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
   * @note Blur passes use full-resolution buffers while the animation is active so that low-strength output does not
   * expose an upscaled downsampled image. If the animation finishes at strength 1, the configured downscale factor is
   * restored. If it finishes at strength 0, blur rendering is bypassed until another strength animation is added.
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

  /**
   * @brief Set the view the background capture starts from.
   *
   * By default the whole ancestor chain above the owner view is captured as the background. Setting
   * a source view narrows that down, so only the subtree under it is blurred. Pass an empty handle
   * to go back to the default.
   *
   * @param[in] sourceView The view the background capture starts from.
   * @note The source view must be an ancestor of the owner view, otherwise it is ignored. An
   * ancestor with its own forward offscreen effect also cuts the search short, in which case this
   * setting has no effect either.
   * @warning The effect keeps only a weak reference to the source view, so the caller must keep the
   * view alive.
   */
  void SetSourceView(Ui::View sourceView);

  /**
   * @brief Set the view the background capture stops at.
   *
   * The background is captured in traversal order and stops once this view is reached, so this view
   * and everything drawn after it are left out of the blur. Pass an empty handle to capture the
   * whole background again.
   *
   * @param[in] stopperView The view the background capture stops at.
   * @warning The effect keeps only a weak reference to the stopper view, so the caller must keep the
   * view alive.
   */
  void SetStopperView(Ui::View stopperView);

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
   * @param[in] backgroundBlurEffectImpl The BackgroundBlurEffect implementation.
   */
  explicit DALI_INTERNAL BackgroundBlurEffect(Internal::BackgroundBlurEffectImpl* backgroundBlurEffectImpl);
  ///@endcond
};
} // namespace Ui
} // namespace Dali
#endif // DALI_UI_BACKGROUND_BLUR_EFFECT_H
