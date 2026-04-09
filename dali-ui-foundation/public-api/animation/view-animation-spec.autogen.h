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
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{

class View;

namespace Internal DALI_INTERNAL
{
class ViewAnimationSpecImpl;
}

/**
 * @brief Defines a reusable, typed animation specification for View properties.
 *
 * @code
 *   auto spec = View::NewAnimationSpec()
 *     .Opacity(1.0f, 300_ms);
 *   spec.ApplyTo(anim, view);
 * @endcode
 */
class DALI_UI_API ViewAnimationSpec : public BaseHandle
{
public:
  ViewAnimationSpec();
  static ViewAnimationSpec New();

  ViewAnimationSpec(const ViewAnimationSpec& other);
  ViewAnimationSpec(ViewAnimationSpec&& rhs) noexcept;
  ~ViewAnimationSpec();
  ViewAnimationSpec& operator=(const ViewAnimationSpec& other);
  ViewAnimationSpec& operator=(ViewAnimationSpec&& rhs) noexcept;

  ////////////////////////////////////////////////////////////////////////////
  // Own properties
  ////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Animates the background color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BackgroundColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the background color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BackgroundColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& SizeWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& SizeWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the size height.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& SizeHeight(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the size height by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& SizeHeightBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& ScaleX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& ScaleXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the scale y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& ScaleY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the scale y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& ScaleYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the opacity.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& Opacity(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the opacity by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& OpacityBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position x.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& PositionX(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position x by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& PositionXBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the position y.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& PositionY(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the position y by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& PositionYBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner radius.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& CornerRadius(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner radius by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& CornerRadiusBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the corner squareness.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& CornerSquareness(const Vector4& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the corner squareness by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& CornerSquarenessBy(const Vector4& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline width.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BorderlineWidth(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline width by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BorderlineWidthBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline color.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BorderlineColor(const UiColor& target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline color by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BorderlineColorBy(const UiColor& relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());

  /**
   * @brief Animates the borderline offset.
   *
   * @param[in] target The target value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BorderlineOffset(float target, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());
  /**
   * @brief Animates the borderline offset by a relative amount.
   *
   * @param[in] relative The relative value
   * @param[in] duration The animation duration
   * @param[in] alpha The alpha function (default: linear)
   * @param[in] delay The delay before starting (default: 0)
   */
  ViewAnimationSpec& BorderlineOffsetBy(float relative, Duration duration, AlphaFunction alpha = AlphaFunction(), Duration delay = Duration());


  /**
   * @brief Applies all stored entries to the given Animation and View.
   *
   * @param[in] animation The Animation to apply to
   * @param[in] view The target View
   */
  void ApplyTo(Animation animation, View view) const;

public: // Not intended for application developers
  /// @cond internal
  DALI_INTERNAL explicit ViewAnimationSpec(Internal::ViewAnimationSpecImpl* impl);
  /// @endcond
};

} // namespace Ui
} // namespace Dali
