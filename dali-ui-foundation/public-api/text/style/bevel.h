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

#include <cstdint>

#include <dali-ui-foundation/public-api/ui-color.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Describes bevel styling for text.
 *
 * This is a value-type object used to configure bevel appearance.
 * Bevel creates a 3D-like effect on text using light and shadow colors.
 */
class DALI_UI_API Bevel
{
public:
  /**
   * @brief Creates a bevel with default values.
   */
  Bevel();

  /**
   * @brief Creates a bevel by copying another bevel.
   *
   * @param[in] rhs The bevel to copy.
   */
  Bevel(const Bevel& rhs);

  /**
   * @brief Creates a bevel by moving another bevel.
   *
   * @param[in] rhs The bevel to move.
   */
  Bevel(Bevel&& rhs) noexcept;

  /**
   * @brief Copies another bevel to this object.
   *
   * @param[in] rhs The bevel to copy.
   * @return A reference to this bevel.
   */
  Bevel& operator=(const Bevel& rhs);

  /**
   * @brief Moves another bevel to this object.
   *
   * @param[in] rhs The bevel to move.
   * @return A reference to this bevel.
   */
  Bevel& operator=(Bevel&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Bevel();

public:
  /**
   * @brief Sets the bevel direction.
   *
   * The direction vector determines the angle of the light source.
   * Typical values are (-1.0f, -1.0f) for top-left light or (1.0f, -1.0f) for top-right light.
   *
   * @param[in] direction The bevel direction vector.
   * @return A reference to this bevel.
   */
  Bevel& SetDirection(const Vector2& direction);

  /**
   * @brief Returns the bevel direction.
   *
   * @return The bevel direction vector.
   */
  const Vector2& GetDirection() const;

  /**
   * @brief Sets the bevel intensity.
   *
   * Higher values create a more pronounced bevel effect.
   *
   * @param[in] intensity The bevel intensity.
   * @return A reference to this bevel.
   */
  Bevel& SetIntensity(float intensity);

  /**
   * @brief Returns the bevel intensity.
   *
   * @return The bevel intensity.
   */
  float GetIntensity() const;

  /**
   * @brief Sets the light color for the bevel effect.
   *
   * This color is applied to the highlighted side of the text.
   *
   * @param[in] color The light color.
   * @return A reference to this bevel.
   */
  Bevel& SetLightColor(const UiColor& color);

  /**
   * @brief Returns the light color for the bevel effect.
   *
   * @return The light color.
   */
  const UiColor& GetLightColor() const;

  /**
   * @brief Sets the shadow color for the bevel effect.
   *
   * This color is applied to the shadowed side of the text.
   *
   * @param[in] color The shadow color.
   * @return A reference to this bevel.
   */
  Bevel& SetShadowColor(const UiColor& color);

  /**
   * @brief Returns the shadow color for the bevel effect.
   *
   * @return The shadow color.
   */
  const UiColor& GetShadowColor() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
