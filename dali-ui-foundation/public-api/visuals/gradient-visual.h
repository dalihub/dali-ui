#ifndef DALI_UI_GRADIENT_VISUAL_OBJECT_H
#define DALI_UI_GRADIENT_VISUAL_OBJECT_H

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
#include <dali-ui-foundation/public-api/dali-ui-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief GradientVisual renders a smooth transition of colors.
 *
 * The gradient is described by a Gradient::Base value. The concrete value passed to
 * SetGradient() decides which kind of gradient is rendered, and carries the stop nodes,
 * the coordinate units and the spread method along with the type specific geometry.
 *
 * @code
 * GradientVisual visual = GradientVisual::New();
 *
 * Gradient::Radial gradient(Vector2::ZERO, 0.5f);
 * gradient.SetStopNodes({{0.0f, Color::RED}, {1.0f, Color::BLUE}});
 * gradient.SetStartOffset(0.25f);
 * visual.SetGradient(gradient);
 * @endcode
 *
 * Its visual type is VisualType::GRADIENT. It can use the CornerRadius, CornerSquareness
 * and Borderline features of VisualBase.
 */
class DALI_UI_API GradientVisual : public VisualBase
{
public:
  /**
   * @brief Creates a GradientVisual object.
   *
   * @return The newly created gradient visual object.
   */
  static GradientVisual New();

  /**
   * @brief Downcasts a handle to GradientVisual handle.
   *
   * If handle points to a GradientVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a GradientVisual or an uninitialized handle
   */
  static GradientVisual DownCast(BaseHandle handle);

public: // Properties
  /**
   * @brief Sets the gradient of the GradientVisual.
   *
   * The gradient type is decided by the value that is passed in, i.e. pass a
   * Gradient::Linear, Gradient::Radial or Gradient::Conic to render that kind of gradient.
   * Setting a gradient of a different type discards the geometry of the previous type.
   *
   * @param[in] gradient The gradient to set
   * @note A gradient whose type is Gradient::Type::NONE is ignored, since a gradient
   * visual has no geometry to fall back to. The visual keeps the gradient it already has.
   * @note At least 2 stop nodes are required to render a gradient.
   */
  void SetGradient(const Gradient::Base& gradient);

  /**
   * @brief Gets the gradient of the GradientVisual.
   *
   * Use Gradient::Linear::DownCast(), Gradient::Radial::DownCast() or
   * Gradient::Conic::DownCast() on the returned value to read the type specific geometry.
   *
   * @return The gradient of the GradientVisual, or a gradient whose GetType() is
   * Gradient::Type::NONE if no gradient has been set
   */
  Gradient::Base GetGradient() const;

public:
  GradientVisual()                                         = default;
  ~GradientVisual()                                        = default;
  GradientVisual(const GradientVisual& rhs)                = default;
  GradientVisual& operator=(const GradientVisual& rhs)     = default;
  GradientVisual(GradientVisual&& rhs) noexcept            = default;
  GradientVisual& operator=(GradientVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL GradientVisual(Dali::Ui::Internal::VisualBaseImpl* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_VISUAL_OBJECT_H
