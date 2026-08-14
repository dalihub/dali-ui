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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>

namespace Dali
{
namespace Ui
{

class Label;

/**
 * @brief A lightweight bridge for applying animations to a Label.
 *
 * LabelAnimationBridge extends ViewAnimationBridge with Label-specific
 * animation properties. All parent properties are also available
 * with proper return type for fluent chaining.
 *
 * Typically created via Label::Animate():
 * @code
 *   auto anim = Animation::New();
 *   label.Animate(anim)
 *     .Opacity(0.5f, 300_ms);
 *   anim.Play();
 * @endcode
 */
class DALI_UI_API LabelAnimationBridge : public ViewAnimationBridge
{
public:
  ////////////////////////////////////////////////////////////////////////////
  // Parent property overrides
  ////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Animates the background color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the background color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the background gradient start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BackgroundGradientStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the background gradient start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BackgroundGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the first shadow blur radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the first shadow blur radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the first shadow opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ShadowOpacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the first shadow opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& Size(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& SizeWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& SizeWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size height.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& SizeHeight(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size height by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& SizeHeightBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& Position(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PositionX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PositionXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PositionY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PositionYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& Scale(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ScaleX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ScaleXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ScaleY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ScaleYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& Color(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& Opacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& OpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner squareness.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner squareness by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BorderlineWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BorderlineOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  ////////////////////////////////////////////////////////////////////////////
  // Own properties
  ////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Animates the text color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the text gradient start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text gradient start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the text gradient overlay start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextGradientOverlayStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text gradient overlay start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextGradientOverlayStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the text reveal progress.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextRevealProgress(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text reveal progress by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& TextRevealProgressBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the pixel snap factor.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PixelSnapFactor(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the pixel snap factor by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationBridge& PixelSnapFactorBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());


private:
  friend class Label;
  LabelAnimationBridge(Animation animation, Label view);
};

} // namespace Ui
} // namespace Dali
