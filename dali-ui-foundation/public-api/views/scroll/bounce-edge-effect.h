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

#ifndef DALI_UI_FOUNDATION_BOUNCE_EDGE_EFFECT_H
#define DALI_UI_FOUNDATION_BOUNCE_EDGE_EFFECT_H

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/scroll/edge-effect.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>

namespace Dali
{

namespace Ui
{

namespace Internal
{
class BounceEdgeEffectImpl;
}

/**
 * @brief An EdgeEffect that displaces the scroll content beyond the boundary
 *        and springs it back.
 *
 * During a drag past the edge:
 *   - Content is moved by @p deltaDistance × pullResistance, clamped to an
 *     internal maximum, giving the appearance of stretching.
 *
 * On release:
 *   - Content animates back to the boundary position with an EASE_OUT_BACK
 *     curve (slight overshoot), controlled by bounceDuration.
 *
 * On a fling hitting the edge (OnAbsorb):
 *   - Content first pushes out a distance proportional to the fling velocity,
 *     then springs back, producing a "thud and bounce" feel.
 *
 * Example:
 * @code
 * auto effect = BounceEdgeEffect::New(ScrollDirection::Vertical);
 * effect.SetSource(contentView)
 *       .SetPullResistance(0.3f)
 *       .SetBounceDuration(0.4f);
 * @endcode
 */
class DALI_UI_API BounceEdgeEffect : public EdgeEffect
{
public:
  BounceEdgeEffect()                                   = default;
  ~BounceEdgeEffect()                                  = default;
  BounceEdgeEffect(const BounceEdgeEffect&)            = default;
  BounceEdgeEffect(BounceEdgeEffect&&)                 = default;
  BounceEdgeEffect& operator=(const BounceEdgeEffect&) = default;
  BounceEdgeEffect& operator=(BounceEdgeEffect&&)      = default;

  /**
   * @brief Creates an initialized BounceEdgeEffect.
   *
   * @param[in] axis The scroll axis this effect operates on.
   */
  static BounceEdgeEffect New(ScrollDirection axis = ScrollDirection::Vertical);

  /**
   * @brief Downcasts an EdgeEffect handle to BounceEdgeEffect.
   *
   * @return A valid BounceEdgeEffect if the underlying impl is BounceEdgeEffectImpl,
   *         otherwise an uninitialized handle.
   */
  static BounceEdgeEffect DownCast(EdgeEffect effect);

  /**
   * @brief Returns the axis this effect operates on.
   */
  ScrollDirection GetAxis() const;

  /**
   * @brief Sets the pull resistance factor.
   *
   * A value of 1.0 moves the content 1:1 with the finger.
   * Lower values (< 1.0) create the rubber-band feel. Default: 0.4.
   *
   * @param[in] resistance Factor in [0.0, 1.0].
   */
  BounceEdgeEffect& SetPullResistance(float resistance);

  /**
   * @brief Gets the pull resistance factor.
   */
  float GetPullResistance() const;

  /**
   * @brief Sets the duration of the spring-back animation.
   *
   * @param[in] durationSec Duration in seconds. Default: 0.35.
   */
  BounceEdgeEffect& SetBounceDuration(float durationSec);

  /**
   * @brief Gets the duration of the spring-back animation (seconds).
   */
  float GetBounceDuration() const;

protected:
  explicit BounceEdgeEffect(Internal::BounceEdgeEffectImpl* impl);

private:
  Internal::BounceEdgeEffectImpl&       GetBounceImpl();
  const Internal::BounceEdgeEffectImpl& GetBounceImpl() const;
};

} // namespace Ui

} // namespace Dali

#endif // DALI_UI_FOUNDATION_BOUNCE_EDGE_EFFECT_H
