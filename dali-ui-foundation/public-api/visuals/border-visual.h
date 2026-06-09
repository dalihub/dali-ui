#ifndef DALI_UI_BORDER_VISUAL_OBJECT_H
#define DALI_UI_BORDER_VISUAL_OBJECT_H

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
#include <dali-ui-foundation/public-api/visuals/border-visual-properties.h>
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
 * @brief BorderVisual is a owner of Visual::Base with Visual::BORDER, to render simple rectangle borderline.
 *
 * It cannot use CornerRadius / CornerSquareness / Borderline feature.
 */
class DALI_UI_API BorderVisual : public VisualBase
{
public:
  /**
   * @brief Property indices for BorderVisual.
   *
   * These can be used with Dali::Ui::VisualBase::GetProperty() and SetProperty().
   */
  struct Property
  {
    enum
    {
      BORDER_SIZE   = BorderVisualPropertyIndex::BORDER_SIZE,
      ANTI_ALIASING = BorderVisualPropertyIndex::ANTI_ALIASING,
    };
  };

public:
  /**
   * @brief Creates a BorderVisual object.
   *
   * @return The newly created color visual object.
   */
  static BorderVisual New();

  /**
   * @brief Downcasts a handle to BorderVisual handle.
   *
   * If handle points to a BorderVisual, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle Handle to an object
   * @return A handle to a BorderVisual or an uninitialized handle
   */
  static BorderVisual DownCast(BaseHandle handle);

public: // Setters
  /**
   * @brief Gets the size of border of the BorderVisual.
   *
   * @return The size of border of the BorderVisual
   */
  float GetBorderSize() const;

  /**
   * @brief Sets the size of border of the BorderVisual.
   *
   * @param[in] borderSize The size of border to set
   */
  void SetBorderSize(float borderSize);

  /**
   * @brief Gets the soft anti-aliasing enabled of the BorderVisual.
   *
   * @return Whether soft anti-aliasing enabled of the BorderVisual or not
   */
  bool IsAntiAliasingEnabled() const;

  /**
   * @brief Sets the soft anti-aliasing enable of the BorderVisual.
   *
   * @param[in] enable True if soft anti-aliasing enabled
   */
  void SetEnableAntiAliasing(bool enable);

public:
  BorderVisual()                                       = default;
  ~BorderVisual()                                      = default;
  BorderVisual(const BorderVisual& rhs)                = default;
  BorderVisual& operator=(const BorderVisual& rhs)     = default;
  BorderVisual(BorderVisual&& rhs) noexcept            = default;
  BorderVisual& operator=(BorderVisual&& rhs) noexcept = default;

public: // Not intended for application developers
  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param[in] object A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL BorderVisual(Dali::Ui::Internal::VisualBaseImpl* object);

public:
};

/**
 * @}
 */
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_BORDER_VISUAL_OBJECT_H
