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
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Describes a View shadow.
 *
 * The shadow follows the View bounds and corner properties.
 */
class DALI_UI_API Shadow
{
public:
  /**
   * @brief Creates a shadow with default values.
   *
   * The default color is semi-transparent black, and the default blur radius,
   * offset, and extents are zero.
   */
  Shadow();

  /**
   * @brief Creates a shadow with custom values.
   *
   * @param[in] blurRadius The blur radius in pixels
   * @param[in] offset The shadow offset in pixels
   * @param[in] color The shadow color
   * @param[in] extents Extra size to add to the shadow visual in pixels
   * @param[in] cutoutPolicy How the View bounds area is cut out from the shadow
   */
  explicit Shadow(float blurRadius, const Vector2& offset = Vector2::ZERO, const UiColor& color = UiColor(0x000000, 0.5f), const Vector2& extents = Vector2::ZERO, CutoutPolicy cutoutPolicy = CutoutPolicy::NONE);

  /**
   * @brief Creates a shadow with custom blur radius and color.
   *
   * @param[in] blurRadius The blur radius in pixels
   * @param[in] color The shadow color
   */
  Shadow(float blurRadius, const UiColor& color);

  /**
   * @brief Creates a shadow from a property map.
   *
   * @param[in] map The color visual property map describing the shadow
   */
  explicit Shadow(const Property::Map& map);

  Shadow(const Shadow& rhs);
  Shadow(Shadow&& rhs) noexcept;
  Shadow& operator=(const Shadow& rhs);
  Shadow& operator=(Shadow&& rhs) noexcept;
  ~Shadow();

public:
  /**
   * @brief Sets the shadow color.
   *
   * @param[in] color The shadow color
   * @return This shadow
   */
  Shadow& SetColor(const UiColor& color);

  /**
   * @brief Gets the shadow color.
   *
   * @return The shadow color
   */
  const UiColor& GetColor() const;

  /**
   * @brief Sets the blur radius.
   *
   * Negative values are clamped to zero.
   *
   * @param[in] blurRadius The blur radius in pixels
   * @return This shadow
   */
  Shadow& SetBlurRadius(float blurRadius);

  /**
   * @brief Gets the blur radius.
   *
   * @return The blur radius in pixels
   */
  float GetBlurRadius() const;

  /**
   * @brief Sets how the View bounds area is cut out from the shadow.
   *
   * The cutout policy controls whether the area covered by the View bounds and
   * corner properties is removed from the shadow.
   *
   * @param[in] cutoutPolicy The cutout policy
   * @return This shadow
   */
  Shadow& SetCutoutPolicy(CutoutPolicy cutoutPolicy);

  /**
   * @brief Gets how the View bounds area is cut out from the shadow.
   *
   * @return The cutout policy
   */
  CutoutPolicy GetCutoutPolicy() const;

  /**
   * @brief Sets the shadow offset.
   *
   * @param[in] offset The offset in pixels
   * @return This shadow
   */
  Shadow& SetOffset(const Vector2& offset);

  /**
   * @brief Gets the shadow offset.
   *
   * @return The offset in pixels
   */
  const Vector2& GetOffset() const;

  /**
   * @brief Sets the extra shadow size.
   *
   * @param[in] extents Extra size in pixels
   * @return This shadow
   */
  Shadow& SetExtents(const Vector2& extents);

  /**
   * @brief Gets the extra shadow size.
   *
   * @return Extra size in pixels
   */
  const Vector2& GetExtents() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Ui
} // namespace Dali
