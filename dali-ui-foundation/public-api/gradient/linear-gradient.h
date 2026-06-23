#ifndef DALI_UI_LINEAR_GRADIENT_H
#define DALI_UI_LINEAR_GRADIENT_H

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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>

namespace Dali
{
namespace Ui
{
namespace Gradient
{

/**
 * @brief Authored linear gradient value.
 */
class DALI_UI_API Linear : public Base
{
public:
  /**
   * @brief Creates a linear gradient value.
   */
  Linear();

  /**
   * @brief Creates a linear gradient value.
   *
   * @param[in] startPosition The start position
   * @param[in] endPosition The end position
   */
  Linear(const Vector2& startPosition, const Vector2& endPosition);

  /**
   * @brief Sets the linear gradient start and end positions.
   *
   * @param[in] startPosition The start position
   * @param[in] endPosition The end position
   */
  void SetStartAndEndPosition(const Vector2& startPosition, const Vector2& endPosition);

  /**
   * @brief Gets the start position.
   *
   * @return The start position
   */
  Vector2 GetStartPosition() const;

  /**
   * @brief Gets the end position.
   *
   * @return The end position
   */
  Vector2 GetEndPosition() const;

  /**
   * @brief Returns a linear typed value from a base gradient value.
   *
   * @param[in] base The source gradient value
   * @return A deep-copied linear value, or a value whose GetType() is Type::NONE if the source type does not match
   */
  static Linear DownCast(const Base& base);
};

} // namespace Gradient
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_LINEAR_GRADIENT_H
