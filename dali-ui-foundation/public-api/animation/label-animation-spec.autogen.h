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
#include <dali-ui-foundation/public-api/animation/view-animation-spec.autogen.h>

namespace Dali
{
namespace Ui
{

class Label;

namespace Internal DALI_INTERNAL
{
class LabelAnimationSpecImpl;
}

/**
 * @brief Defines a reusable, typed animation specification for Label properties.
 *
 * LabelAnimationSpec extends ViewAnimationSpec with Label-specific
 * animation properties. All parent properties are also available
 * with proper return type for fluent chaining.
 *
 * @code
 *   auto spec = Label::NewAnimationSpec()
 *     .Opacity(1.0f, 300_ms);
 *   spec.ApplyTo(anim, label);
 * @endcode
 */
class DALI_UI_API LabelAnimationSpec : public ViewAnimationSpec
{
public:
  LabelAnimationSpec();
  static LabelAnimationSpec New();
  static LabelAnimationSpec DownCast(BaseHandle handle); // LCOV_EXCL_LINE

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
  LabelAnimationSpec& BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the background color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the background gradient start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BackgroundGradientStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the background gradient start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BackgroundGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the first shadow blur radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the first shadow blur radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the first shadow opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ShadowOpacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the first shadow opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& Size(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& SizeWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& SizeWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size height.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& SizeHeight(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size height by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& SizeHeightBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& Position(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PositionX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PositionXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PositionY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PositionYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& Scale(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ScaleX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ScaleXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ScaleY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ScaleYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& Color(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& Opacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& OpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner squareness.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner squareness by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BorderlineWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BorderlineOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

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
  LabelAnimationSpec& TextColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the text gradient start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text gradient start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the text gradient overlay start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextGradientOverlayStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text gradient overlay start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextGradientOverlayStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the text reveal progress.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextRevealProgress(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text reveal progress by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& TextRevealProgressBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the pixel snap factor.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PixelSnapFactor(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the pixel snap factor by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  LabelAnimationSpec& PixelSnapFactorBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());


public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL explicit LabelAnimationSpec(Internal::LabelAnimationSpecImpl* impl);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
