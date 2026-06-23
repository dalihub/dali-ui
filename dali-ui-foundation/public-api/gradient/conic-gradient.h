#ifndef DALI_UI_CONIC_GRADIENT_H
#define DALI_UI_CONIC_GRADIENT_H

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
#include <dali/public-api/math/radian.h>
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
 * @brief Authored conic gradient value.
 */
class DALI_UI_API Conic : public Base
{
public:
  /**
   * @brief Creates a conic gradient value.
   */
  Conic();

  /**
   * @brief Creates a conic gradient value.
   *
   * @param[in] center The center position
   * @param[in] startAngle The start angle
   */
  Conic(const Vector2& center, Radian startAngle);

  /**
   * @brief Sets the conic gradient center and start angle.
   *
   * @param[in] center The center position
   * @param[in] startAngle The start angle
   */
  void SetCenterAndStartAngle(const Vector2& center, Radian startAngle);

  /**
   * @brief Gets the center position.
   *
   * @return The center position
   */
  Vector2 GetCenter() const;

  /**
   * @brief Gets the start angle.
   *
   * @return The start angle
   */
  Radian GetStartAngle() const;

  /**
   * @brief Returns a conic typed value from a base gradient value.
   *
   * @param[in] base The source gradient value
   * @return A deep-copied conic value, or a value whose GetType() is Type::NONE if the source type does not match
   */
  static Conic DownCast(const Base& base);
};

} // namespace Gradient
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_CONIC_GRADIENT_H
