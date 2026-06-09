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
#include <dali-ui-foundation/public-api/visuals/color-visual-properties.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

namespace Dali
{
namespace Ui
{

/**
 * @addtogroup dali_ui_visuals
 * @{
 */

/**
 * @brief ColorVisual is a owner of Visual::Base with Visual::COLOR, to render simple color.
 *
 * It can use CornerRadius / CornerSquareness / Borderline feature.
 */
class DALI_UI_API ColorVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for ColorVisual.
   *
   * These can be used with Dali::Ui::VisualBase::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      BLUR_RADIUS   = ColorVisualPropertyIndex::BLUR_RADIUS,
      CUTOUT_POLICY = ColorVisualPropertyIndex::CUTOUT_POLICY,
    };
  };

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

public: // Setters
  /**
   * @brief Gets the blur radius of the ColorVisual.
   *
   * @return The blur radius of the ColorVisual
   */
  float GetBlurRadius() const;

  /**
   * @brief Sets the blur radius of the ColorVisual.
   *
   * @param[in] blurRadius The blur radius to set
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
   * @param[in] cutoutPolicy The cutout policy to set
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
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ColorVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_VISUAL_OBJECT_H
