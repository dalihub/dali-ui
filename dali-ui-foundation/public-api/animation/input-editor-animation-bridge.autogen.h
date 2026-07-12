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

class InputEditor;

/**
 * @brief A lightweight bridge for applying animations to a InputEditor.
 *
 * InputEditorAnimationBridge extends ViewAnimationBridge with InputEditor-specific
 * animation properties. All parent properties are also available
 * with proper return type for fluent chaining.
 *
 * Typically created via InputEditor::Animate():
 * @code
 *   auto anim = Animation::New();
 *   inputEditor.Animate(anim)
 *     .Opacity(0.5f, 300_ms);
 *   anim.Play();
 * @endcode
 */
class DALI_UI_API InputEditorAnimationBridge : public ViewAnimationBridge
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
  InputEditorAnimationBridge& BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the background color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the first shadow blur radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ShadowBlurRadius(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the first shadow blur radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ShadowBlurRadiusBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the first shadow opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ShadowOpacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the first shadow opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ShadowOpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& Size(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& SizeBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& SizeWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& SizeWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size height.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& SizeHeight(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size height by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& SizeHeightBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& Position(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PositionBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PositionX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PositionXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PositionY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PositionYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& Scale(const Vector3& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ScaleBy(const Vector3& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ScaleX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ScaleXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ScaleY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ScaleYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& Color(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& ColorBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& Opacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& OpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner squareness.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner squareness by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BorderlineWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BorderlineOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  ////////////////////////////////////////////////////////////////////////////
  // Own properties
  ////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Animates the text gradient start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& TextGradientStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the text gradient start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& TextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the placeholder text gradient start offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PlaceholderTextGradientStartOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the placeholder text gradient start offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  InputEditorAnimationBridge& PlaceholderTextGradientStartOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());


private:
  friend class InputEditor;
  InputEditorAnimationBridge(Animation animation, InputEditor view);
};

} // namespace Ui
} // namespace Dali
