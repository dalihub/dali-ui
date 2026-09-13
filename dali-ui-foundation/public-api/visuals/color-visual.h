#ifndef DALI_UI_COLOR_VISUAL_OBJECT_H
#define DALI_UI_COLOR_VISUAL_OBJECT_H

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
#include <dali-ui-foundation/public-api/types/cutout-policy.h>
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
 * @brief ColorVisual renders a solid color.
 *
 * Its visual type is VisualType::COLOR. On top of the CornerRadius, CornerSquareness and
 * Borderline features of VisualBase, it adds a blur radius and a cutout policy.
 */
class DALI_UI_API ColorVisual : public VisualBase
{
public:
  /**
   * @brief Creates a ColorVisual object.
   *
   * @return The newly created color visual object.
   */
  static ColorVisual New();

  /**
   * @brief Downcasts a handle to ColorVisual handle.
   *
   * If handle points to a ColorVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a ColorVisual or an uninitialized handle
   */
  static ColorVisual DownCast(BaseHandle handle);

public: // Properties
  /**
   * @brief Gets the blur radius of the ColorVisual.
   *
   * @return The blur radius in pixels
   */
  float GetBlurRadius() const;

  /**
   * @brief Sets the blur radius of the ColorVisual.
   *
   * Zero, the default, leaves the edge sharp; the larger the value, the more the edge is
   * blurred. The area drawn grows by the blur radius, so a blurred visual reaches beyond
   * the View it belongs to.
   *
   * @param[in] blurRadius The blur radius in pixels
   * @note If the corner squareness is not zero, the blurred width may differ from this value.
   * @see SetCutoutPolicy()
   */
  void SetBlurRadius(float blurRadius);

  /**
   * @brief Gets the cutout policy of the ColorVisual.
   *
   * @return The cutout policy of the ColorVisual
   */
  CutoutPolicy GetCutoutPolicy() const;

  /**
   * @brief Sets the cutout policy of the ColorVisual.
   *
   * The default is CutoutPolicy::NONE, which draws the whole area. Cutting out the view is
   * how a visual enlarged by a blur radius is kept from drawing underneath its View.
   *
   * @param[in] cutoutPolicy The cutout policy to set
   * @note CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS and
   *       CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS follow the corner radius of the
   *       View the visual belongs to, so the visual must be added to a View before it is
   *       put on the scene.
   * @see CutoutPolicy
   */
  void SetCutoutPolicy(CutoutPolicy cutoutPolicy);

public:
  ColorVisual()                                      = default;
  ~ColorVisual()                                     = default;
  ColorVisual(const ColorVisual& rhs)                = default;
  ColorVisual& operator=(const ColorVisual& rhs)     = default;
  ColorVisual(ColorVisual&& rhs) noexcept            = default;
  ColorVisual& operator=(ColorVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ColorVisual(Dali::Ui::Internal::VisualBaseImpl* object);
  /// @endcond
};

/**
 * @}
 */
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_COLOR_VISUAL_OBJECT_H
