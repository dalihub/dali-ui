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
 * @brief Describes line-through styling for text.
 *
 * This is a value-type object used to configure line-through appearance.
 */
class DALI_UI_API LineThrough
{
public:
  /**
   * @brief Creates a line-through with default values.
   */
  LineThrough();

  /**
   * @brief Creates a line-through by copying another line-through.
   *
   * @param[in] rhs The line-through to copy.
   */
  LineThrough(const LineThrough& rhs);

  /**
   * @brief Creates a line-through by moving another line-through.
   *
   * @param[in] rhs The line-through to move.
   */
  LineThrough(LineThrough&& rhs) noexcept;

  /**
   * @brief Copies another line-through to this object.
   *
   * @param[in] rhs The line-through to copy.
   */
  LineThrough& operator=(const LineThrough& rhs);

  /**
   * @brief Moves another line-through to this object.
   *
   * @param[in] rhs The line-through to move.
   */
  LineThrough& operator=(LineThrough&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~LineThrough();

  /**
   * @brief Returns a value representing no line-through style.
   *
   * This value can be passed to SetTextLineThrough() to clear the line-through style.
   *
   * @return A shared immutable none value.
   */
  static const LineThrough& None();

  /**
   * @brief Compares this line-through with another line-through.
   *
   * @param[in] rhs The line-through to compare with.
   * @return true if both values are equal.
   */
  bool operator==(const LineThrough& rhs) const;

  /**
   * @brief Compares this line-through with another line-through.
   *
   * @param[in] rhs The line-through to compare with.
   * @return true if both values are not equal.
   */
  bool operator!=(const LineThrough& rhs) const;

public:
  /**
   * @brief Sets the line-through color.
   *
   * @param[in] color The line-through color.
   */
  void SetColor(const UiColor& color);

  /**
   * @brief Returns the line-through color.
   *
   * @return The line-through color.
   */
  const UiColor& GetColor() const;

  /**
   * @brief Sets the line-through thickness.
   *
   * @param[in] thickness The line-through thickness in pixels.
   */
  void SetThickness(float thickness);

  /**
   * @brief Returns the line-through thickness.
   *
   * @return The line-through thickness in pixels.
   */
  float GetThickness() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
