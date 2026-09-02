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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Span payload that applies a gradient to the foreground text fill.
 *
 * Attach the span to a half-open UTF-32 range with StyledTextBuilder. The
 * authored gradient is copied when the span is created, so later changes to
 * the source gradient do not change this payload. When foreground color and
 * gradient spans overlap, the later-applied span determines the foreground
 * paint for the overlapping characters. Color glyphs, such as emoji, retain
 * their native colors.
 *
 * @note This is a static paint payload. To change the gradient, replace the
 * span with a newly created one.
 */
class DALI_UI_API GradientSpan : public Span
{
public:
  /**
   * @brief Selects the coordinate bounds used to evaluate the gradient.
   *
   * The gradient's Units determine how authored coordinates are interpreted
   * within the selected bounds.
   */
  enum class BoundsMode : uint8_t
  {
    SPAN_BOUND = 0u, ///< The union of visible glyph ink bounds owned by this span.
    CONTENT_BOUND,   ///< The laid-out bounds of the complete text content.
    VIEW_BOUND       ///< The bounds of the text view.
  };

  /**
   * @brief Creates an uninitialized GradientSpan handle.
   */
  GradientSpan() = default;

  /**
   * @brief Creates a GradientSpan with an immutable authored gradient snapshot.
   *
   * @param[in] gradient The foreground gradient
   * @return A new SPAN_BOUND GradientSpan
   */
  static GradientSpan New(const Gradient::Base& gradient);

  /**
   * @brief Creates a GradientSpan with an immutable authored gradient snapshot.
   *
   * @param[in] gradient The foreground gradient
   * @param[in] boundsMode The coordinate bounds used to evaluate the gradient
   * @return A new GradientSpan
   */
  static GradientSpan New(const Gradient::Base& gradient, BoundsMode boundsMode);

  /**
   * @brief Downcasts a handle to GradientSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized GradientSpan on success, otherwise empty
   */
  static GradientSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets a copy of the authored gradient snapshot.
   *
   * @return The foreground gradient
   */
  Gradient::Base GetGradient() const;

  /**
   * @brief Gets the coordinate bounds mode.
   *
   * @return The coordinate bounds mode
   */
  BoundsMode GetBoundsMode() const;

private:
  /**
   * @brief Creates a GradientSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL GradientSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
