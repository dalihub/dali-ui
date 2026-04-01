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

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Describes underline styling for text.
 *
 * This is a value-type object used to configure underline appearance.
 */
class DALI_UI_API Underline
{
public:
  /**
   * @brief The underline rendering type.
   */
  enum class Type : uint8_t
  {
    /**
     * @brief Draws a solid underline.
     */
    SOLID,
    /**
     * @brief Draws a dashed underline.
     *
     * Dash length and dash gap are applied when this type is used.
     */
    DASHED,
    /**
     * @brief Draws a double underline.
     */
    DOUBLE
  };

public:
  /**
   * @brief Creates an underline with default values.
   */
  Underline();

  /**
   * @brief Creates an underline by copying another underline.
   *
   * @param[in] rhs The underline to copy.
   */
  Underline(const Underline& rhs);

  /**
   * @brief Creates an underline by moving another underline.
   *
   * @param[in] rhs The underline to move.
   */
  Underline(Underline&& rhs) noexcept;

  /**
   * @brief Copies another underline to this object.
   *
   * @param[in] rhs The underline to copy.
   * @return A reference to this underline.
   */
  Underline& operator=(const Underline& rhs);

  /**
   * @brief Moves another underline to this object.
   *
   * @param[in] rhs The underline to move.
   * @return A reference to this underline.
   */
  Underline& operator=(Underline&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Underline();

public:
  /**
   * @brief Sets the underline color.
   *
   * @param[in] color The underline color.
   * @return A reference to this underline.
   */
  Underline& SetColor(const UiColor& color);

  /**
   * @brief Returns the underline color.
   *
   * @return The underline color.
   */
  const UiColor& GetColor() const;

  /**
   * @brief Sets the underline thickness.
   *
   * @param[in] thickness The underline thickness in pixels.
   * @return A reference to this underline.
   */
  Underline& SetThickness(float thickness);

  /**
   * @brief Returns the underline thickness.
   *
   * @return The underline thickness in pixels.
   */
  float GetThickness() const;

  /**
   * @brief Sets the underline type.
   *
   * @param[in] type The underline type.
   * @return A reference to this underline.
   */
  Underline& SetType(Type type);

  /**
   * @brief Returns the underline type.
   *
   * @return The underline type.
   */
  Type GetType() const;

  /**
   * @brief Sets the dash length for a dashed underline.
   *
   * This value is used only when the underline type is DASHED.
   *
   * @param[in] length The length of each dash in pixels.
   * @return A reference to this underline.
   */
  Underline& SetDashLength(float length);

  /**
   * @brief Returns the dash length for a dashed underline.
   *
   * @return The dash length in pixels.
   */
  float GetDashLength() const;

  /**
   * @brief Sets the gap between dashes for a dashed underline.
   *
   * This value is used only when the underline type is DASHED.
   *
   * @param[in] gap The gap between dashes in pixels.
   * @return A reference to this underline.
   */
  Underline& SetDashGap(float gap);

  /**
   * @brief Returns the gap between dashes for a dashed underline.
   *
   * @return The gap between dashes in pixels.
   */
  float GetDashGap() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
