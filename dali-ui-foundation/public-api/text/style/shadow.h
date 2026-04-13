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
 * @brief Describes shadow styling for text.
 *
 * This is a value-type object used to configure shadow appearance.
 */
class DALI_UI_API Shadow
{
public:
  /**
   * @brief Creates a shadow with default values.
   */
  Shadow();

  /**
   * @brief Creates a shadow by copying another shadow.
   *
   * @param[in] rhs The shadow to copy.
   */
  Shadow(const Shadow& rhs);

  /**
   * @brief Creates a shadow by moving another shadow.
   *
   * @param[in] rhs The shadow to move.
   */
  Shadow(Shadow&& rhs) noexcept;

  /**
   * @brief Copies another shadow to this object.
   *
   * @param[in] rhs The shadow to copy.
   * @return A reference to this shadow.
   */
  Shadow& operator=(const Shadow& rhs);

  /**
   * @brief Moves another shadow to this object.
   *
   * @param[in] rhs The shadow to move.
   * @return A reference to this shadow.
   */
  Shadow& operator=(Shadow&& rhs) noexcept;

  /**
   * @brief Destructor.
   */
  ~Shadow();

public:
  /**
   * @brief Sets the shadow color.
   *
   * @param[in] color The shadow color.
   * @return A reference to this shadow.
   */
  Shadow& SetColor(const UiColor& color);

  /**
   * @brief Returns the shadow color.
   *
   * @return The shadow color.
   */
  const UiColor& GetColor() const;

  /**
   * @brief Sets the shadow offset.
   *
   * @param[in] offset The shadow offset in pixels.
   * @return A reference to this shadow.
   */
  Shadow& SetOffset(const Vector2& offset);

  /**
   * @brief Returns the shadow offset.
   *
   * @return The shadow offset in pixels.
   */
  const Vector2& GetOffset() const;

  /**
   * @brief Sets the shadow blur radius.
   *
   * @param[in] blurRadius The shadow blur radius in pixels.
   * @return A reference to this shadow.
   */
  Shadow& SetBlurRadius(float blurRadius);

  /**
   * @brief Returns the shadow blur radius.
   *
   * @return The shadow blur radius in pixels.
   */
  float GetBlurRadius() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Text
} // namespace Ui
} // namespace Dali
