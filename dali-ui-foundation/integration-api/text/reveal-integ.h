#ifndef DALI_UI_INTEGRATION_TEXT_REVEAL_INTEG_H
#define DALI_UI_INTEGRATION_TEXT_REVEAL_INTEG_H

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

#include <dali-ui-foundation/public-api/text/style/reveal.h>
#include <cstdint>

namespace DALI_NAMESPACE::Ui::Integration::Text::Reveal
{
/**
 * @brief Selects the rendering preference for Reveal blur.
 */
enum class BlurMode : uint8_t
{
  /**
   * @brief Prioritizes rendering performance over blur fidelity.
   */
  PERFORMANCE = 0,

  /**
   * @brief Prioritizes blur fidelity and may require more rendering resources.
   */
  QUALITY = 1
};

/**
 * @brief Sets the Reveal blur radius in logical pixels.
 *
 * Zero disables blur. Values outside [0.0, 64.0] are clamped, and NaN is
 * normalized to 0.0. The default is 0.0. The getter returns the authored value.
 *
 * Blur affects synchronous and asynchronous text foreground. Each sequence
 * shares one blur strength; it is not a separate blur for each reveal unit.
 * Inline ImageSpan content shares its sequence's blur and reveal timing.
 * Image loading does not delay or restart the timeline; a ready image uses
 * the current progress. Text decorations are not blurred.
 *
 * Blur uses TextRevealProgress and does not create or extend an Animation.
 * Reveal and blur timing are normalized together to finish at progress 1.0.
 * Animation alpha functions affect both.
 *
 * @note Blur is substantially more expensive than Reveal without blur. Setup
 * and rendering require additional offscreen processing, textures and render
 * tasks. Cost depends on rendered area, radius, mode, and the number of active
 * Labels and sequences. Holding progress still or reaching an endpoint does
 * not stop this processing or release its resources. Disable blur or Reveal
 * when no longer needed.
 *
 * @note Embossed text, height-tiled text textures and configurations exceeding
 * supported resource limits retain Reveal without blur. Authored settings
 * are preserved. Blur can extend outside Label bounds; use Label clipping to
 * restrict it. ImageSpan edge antialiasing may differ from rendering without
 * blur, including at progress 1.0.
 *
 * @param[in,out] reveal The configuration to modify. Apply it to a Label to take effect.
 * @param[in] radius The blur radius in [0.0, 64.0].
 * @pre reveal must not be None() or a moved-from value.
 */
DALI_UI_API void SetBlurRadius(Ui::Text::Reveal& reveal, float radius);

/**
 * @brief Gets the authored blur radius in logical pixels.
 *
 * @param[in] reveal The Reveal configuration.
 * @return The authored blur radius. The default is 0.0.
 * @pre reveal must not be None() or a moved-from value.
 */
DALI_UI_API float GetBlurRadius(const Ui::Text::Reveal& reveal);

/**
 * @brief Sets the blur duration relative to the common sequence interval.
 *
 * All active sequences share the reference interval used for staggering.
 * The ratio is not directly a fraction of the total Animation duration or
 * each individual line's reveal span.
 *
 * Zero disables blur. A value of 1.0 uses the complete reference interval.
 * Blur strength decreases from its maximum at each sequence start to zero
 * at the end of its blur interval. Units revealed later in the sequence can
 * therefore appear after its blur has ended. Reveal and blur timing are
 * normalized together to finish at progress 1.0.
 *
 * Values outside [0.0, 1.0] are clamped, and NaN is normalized to 0.0.
 * The default is 1.0. A nonzero radius is also required to enable blur.
 *
 * @param[in,out] reveal The configuration to modify.
 * @param[in] ratio The blur duration ratio in [0.0, 1.0].
 * @pre reveal must not be None() or a moved-from value.
 */
DALI_UI_API void SetBlurDurationRatio(Ui::Text::Reveal& reveal, float ratio);

/**
 * @brief Gets the authored blur duration ratio.
 *
 * @param[in] reveal The Reveal configuration.
 * @return The authored ratio in [0.0, 1.0]. The default is 1.0.
 * @pre reveal must not be None() or a moved-from value.
 */
DALI_UI_API float GetBlurDurationRatio(const Ui::Text::Reveal& reveal);

/**
 * @brief Selects the rendering preference without changing the Reveal timeline.
 *
 * The default is PERFORMANCE. The mode does not change the authored radius,
 * duration ratio or progress, and has no visual effect when blur is disabled.
 * Unrecognized values use PERFORMANCE rendering.
 *
 * @param[in,out] reveal The configuration to modify.
 * @param[in] mode The blur rendering preference.
 * @pre reveal must not be None() or a moved-from value.
 */
DALI_UI_API void SetBlurMode(Ui::Text::Reveal& reveal, BlurMode mode);

/**
 * @brief Gets the authored blur rendering preference.
 *
 * @param[in] reveal The Reveal configuration.
 * @return The authored mode. The default is PERFORMANCE.
 * @pre reveal must not be None() or a moved-from value.
 */
DALI_UI_API BlurMode GetBlurMode(const Ui::Text::Reveal& reveal);

} //namespace DALI_NAMESPACE::Ui::Integration::Text::Reveal

#endif // DALI_UI_INTEGRATION_TEXT_REVEAL_INTEG_H
