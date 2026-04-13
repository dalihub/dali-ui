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
 * @brief Describes outline styling for text.
 *
 * This is a value-type object used to configure outline appearance.
 */
class DALI_UI_API Outline
{
public:
  /**
   * @brief Creates an outline with default values.
   */
  Outline();

  /**
   * @brief Creates an outline by copying another outline.
   *
   * @param[in] rhs The outline to copy.
   */
  Outline(const Outline& rhs);

  /**
   * @brief Creates an outline by moving another outline.
   *
   * @param[in] rhs The outline to move.
   */
  Outline(Outline&& rhs) noexcept;

  /**
   * @brief Copies another outline to this object.
   *
   * @param[in] rhs The outline to copy.
   * @return A reference to this outline.
   */
  Outline& operator=(const Outline& rhs);

  /**
   * @brief Moves another outline to this object.
   *
   * @param[in] rhs The outline to move.
   * @return A reference to this outline.
   */
  Outline& operator=(Outline&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Outline();

public:
  /**
   * @brief Sets the outline color.
   *
   * @param[in] color The outline color.
   * @return A reference to this outline.
   */
  Outline& SetColor(const UiColor& color);

  /**
   * @brief Returns the outline color.
   *
   * @return The outline color.
   */
  const UiColor& GetColor() const;

  /**
   * @brief Sets the outline offset.
   *
   * @param[in] offset The outline offset in pixels.
   * @return A reference to this outline.
   */
  Outline& SetOffset(const Vector2& offset);

  /**
   * @brief Returns the outline offset.
   *
   * @return The outline offset in pixels.
   */
  const Vector2& GetOffset() const;

  /**
   * @brief Sets the outline width.
   *
   * @param[in] width The outline width in pixels.
   * @return A reference to this outline.
   */
  Outline& SetWidth(float width);

  /**
   * @brief Returns the outline width.
   *
   * @return The outline width in pixels.
   */
  float GetWidth() const;

  /**
   * @brief Sets the outline blur radius.
   *
   * @param[in] blurRadius The outline blur radius in pixels.
   * @return A reference to this outline.
   */
  Outline& SetBlurRadius(float blurRadius);

  /**
   * @brief Returns the outline blur radius.
   *
   * @return The outline blur radius in pixels.
   */
  float GetBlurRadius() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
