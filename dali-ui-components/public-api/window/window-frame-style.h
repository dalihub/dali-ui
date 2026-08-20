#pragma once

/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/types/insets.h>
#include <dali-ui-foundation/public-api/types/shadow.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali-ui-foundation/public-api/views/view-types.h>
#include <dali/public-api/common/dali-string-view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/unique-ptr.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>

namespace Dali::Ui
{

/**
 * @brief Source used to render the client-side frame shadow.
 */
enum class WindowFrameShadowSource
{
  NONE  = 0, ///< No client-side shadow
  COLOR = 1, ///< Shadow generated from Dali::Ui::Shadow color, blur, and offset
  IMAGE = 2  ///< Shadow rendered from an image, optionally using N-patch borders
};

/**
 * @brief Visual style of the client-rendered frame and shadow.
 */
class DALI_UI_COMPONENTS_API WindowFrameStyle
{
public:
  /**
   * @brief Creates the default transparent frame style.
   */
  WindowFrameStyle();
  WindowFrameStyle(const WindowFrameStyle& rhs);
  WindowFrameStyle(WindowFrameStyle&& rhs) noexcept;
  WindowFrameStyle& operator=(const WindowFrameStyle& rhs);
  WindowFrameStyle& operator=(WindowFrameStyle&& rhs) noexcept;
  ~WindowFrameStyle();

  /**
   * @brief Returns the configured shadow source.
   * @return Shadow source selected by SetShadow(), SetShadowImageUrl(), or ClearShadow()
   */
  WindowFrameShadowSource GetShadowSource() const;

  /**
   * @brief Disables the client-rendered shadow.
   */
  void ClearShadow();

  /**
   * @brief Sets surface space reserved outside the frame for shadow.
   * @param[in] outsets Non-negative outsets
   */
  void SetShadowOutsets(const Dali::Ui::Insets& outsets);

  /**
   * @brief Returns shadow outsets.
   * @return Configured outsets
   */
  Dali::Ui::Insets GetShadowOutsets() const;

  /**
   * @brief Sets and selects a generated color shadow.
   * @param[in] shadow Shadow color, blur, offset, extents, and cutout properties
   */
  void SetShadow(const Dali::Ui::Shadow& shadow);

  /**
   * @brief Returns the generated color shadow properties.
   * @return Configured shadow
   */
  Dali::Ui::Shadow GetShadow() const;

  /**
   * @brief Sets and selects a shadow image.
   *
   * An empty URL disables the client-rendered shadow.
   * @param[in] imageUrl Resource URL
   */
  void SetShadowImageUrl(Dali::StringView imageUrl);

  /**
   * @brief Returns the shadow image URL.
   * @return Resource URL
   */
  Dali::String GetShadowImageUrl() const;

  /**
   * @brief Sets the shadow image N-patch stretch borders.
   * @param[in] border Start, end, top, and bottom pixel insets
   */
  void SetShadowImageBorder(const Dali::Ui::Insets& border);

  /**
   * @brief Returns the shadow image N-patch stretch borders.
   * @return Configured start, end, top, and bottom insets
   */
  Dali::Ui::Insets GetShadowImageBorder() const;

  /**
   * @brief Sets whether only the shadow image border pixels are rendered.
   * @param[in] borderOnly true to omit the center
   */
  void SetShadowImageBorderOnly(bool borderOnly);

  /**
   * @brief Returns whether only the shadow image border pixels are rendered.
   * @return true when center rendering is disabled
   */
  bool IsShadowImageBorderOnly() const;

  /**
   * @brief Sets the frame background color.
   * @param[in] color Background color
   */
  void SetFrameBackgroundColor(const Dali::Ui::UiColor& color);

  /**
   * @brief Returns the frame background color.
   * @return Configured color
   */
  Dali::Ui::UiColor GetFrameBackgroundColor() const;

  /**
   * @brief Sets frame and child clipping radii.
   * @param[in] radius Per-corner radii
   */
  void SetFrameCornerRadius(const Dali::Vector4& radius);

  /**
   * @brief Returns frame corner radii.
   * @return Configured radii
   */
  Dali::Vector4 GetFrameCornerRadius() const;

  /**
   * @brief Sets radius interpretation policy.
   * @param[in] policy Radius policy
   */
  void SetFrameCornerRadiusPolicy(Dali::Ui::CornerRadiusPolicy policy);

  /**
   * @brief Returns radius interpretation policy.
   * @return Configured policy
   */
  Dali::Ui::CornerRadiusPolicy GetFrameCornerRadiusPolicy() const;

private:
  struct DALI_INTERNAL  Impl;
  Dali::UniquePtr<Impl> mImpl;
};

} // namespace Dali::Ui
