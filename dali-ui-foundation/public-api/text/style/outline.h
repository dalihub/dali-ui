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

#include <dali-ui-foundation/public-api/types/ui-color.h>
#include <dali/public-api/math/vector2.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Text
{

/**
 * @brief Describes outline styling for text.
 *
 * This is a value-type object used to configure outline appearance.
 * @SINCE_3_0.0
 */
class DALI_UI_API Outline
{
public:
  /**
   * @brief Creates an outline with default values.
   * @SINCE_3_0.0
   */
  Outline();

  /**
   * @brief Creates an outline by copying another outline.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The outline to copy.
   */
  Outline(const Outline& rhs);

  /**
   * @brief Creates an outline by moving another outline.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The outline to move.
   */
  Outline(Outline&& rhs) noexcept;

  /**
   * @brief Copies another outline to this object.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The outline to copy.
   */
  Outline& operator=(const Outline& rhs);

  /**
   * @brief Moves another outline to this object.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The outline to move.
   */
  Outline& operator=(Outline&& rhs) noexcept;

  /**
   * @brief Destructor.
   * @SINCE_3_0.0
   */
  ~Outline();

  /**
   * @brief Returns a value representing no outline style.
   *
   * This value can be passed to SetTextOutline() to clear the outline style.
   *
   * @SINCE_3_0.0
   * @return A shared immutable none value.
   */
  static const Outline& None();

  /**
   * @brief Compares this outline with another outline.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The outline to compare with.
   * @return true if both values are equal.
   */
  bool operator==(const Outline& rhs) const;

  /**
   * @brief Compares this outline with another outline.
   *
   * @SINCE_3_0.0
   * @param[in] rhs The outline to compare with.
   * @return true if both values are not equal.
   */
  bool operator!=(const Outline& rhs) const;

public:
  /**
   * @brief Sets the outline color.
   *
   * @SINCE_3_0.0
   * @param[in] color The outline color.
   */
  void SetColor(const UiColor& color);

  /**
   * @brief Returns the outline color.
   *
   * @SINCE_3_0.0
   * @return The outline color.
   */
  const UiColor& GetColor() const;

  /**
   * @brief Sets the outline offset.
   *
   * @SINCE_3_0.0
   * @param[in] offset The outline offset in pixels.
   */
  void SetOffset(const Vector2& offset);

  /**
   * @brief Returns the outline offset.
   *
   * @SINCE_3_0.0
   * @return The outline offset in pixels.
   */
  const Vector2& GetOffset() const;

  /**
   * @brief Sets the outline width.
   *
   * @SINCE_3_0.0
   * @param[in] width The outline width in pixels.
   */
  void SetWidth(float width);

  /**
   * @brief Returns the outline width.
   *
   * @SINCE_3_0.0
   * @return The outline width in pixels.
   */
  float GetWidth() const;

  /**
   * @brief Sets the outline blur radius.
   *
   * @SINCE_3_0.0
   * @param[in] blurRadius The outline blur radius in pixels.
   */
  void SetBlurRadius(float blurRadius);

  /**
   * @brief Returns the outline blur radius.
   *
   * @SINCE_3_0.0
   * @return The outline blur radius in pixels.
   */
  float GetBlurRadius() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} //namespace DALI_NAMESPACE
