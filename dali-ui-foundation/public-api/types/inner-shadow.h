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
#include <dali/public-api/common/insets.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace Dali
{
namespace Ui
{

/**
 * @brief Describes a shadow drawn inside a View.
 *
 * The insets control how far the solid part of the shadow reaches inward from
 * each logical edge before it is blurred.
 */
class DALI_UI_API InnerShadow
{
public:
  /**
   * @brief Creates an inner shadow with default values.
   *
   * The default color is semi-transparent black, and the default blur radius
   * and insets are zero.
   */
  InnerShadow();

  /**
   * @brief Creates an inner shadow with custom values.
   *
   * @param[in] insets How far the shadow reaches inward from each logical edge
   * @param[in] blurRadius The blur radius in pixels
   * @param[in] color The shadow color
   */
  explicit InnerShadow(const Insets& insets, float blurRadius = 0.0f, const UiColor& color = UiColor(0x000000, 0.5f));

  /**
   * @brief Creates a no-op inner shadow.
   *
   * Use this to clear a View inner shadow or to indicate that no inner shadow
   * should be applied.
   *
   * @return An InnerShadow that represents no inner shadow
   */
  static InnerShadow None();

  InnerShadow(const InnerShadow& rhs);
  InnerShadow(InnerShadow&& rhs) noexcept;
  InnerShadow& operator=(const InnerShadow& rhs);
  InnerShadow& operator=(InnerShadow&& rhs) noexcept;
  ~InnerShadow();

public:
  /**
   * @brief Compares this inner shadow with another inner shadow.
   *
   * @param[in] rhs The inner shadow to compare with
   * @return True if both values are equal
   */
  bool operator==(const InnerShadow& rhs) const;

  /**
   * @brief Compares this inner shadow with another inner shadow.
   *
   * @param[in] rhs The inner shadow to compare with
   * @return True if both values are not equal
   */
  bool operator!=(const InnerShadow& rhs) const;

  /**
   * @brief Sets the inner shadow color.
   *
   * @param[in] color The shadow color
   * @return This inner shadow
   */
  InnerShadow& SetColor(const UiColor& color);

  /**
   * @brief Gets the inner shadow color.
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
   * @return This inner shadow
   */
  InnerShadow& SetBlurRadius(float blurRadius);

  /**
   * @brief Gets the blur radius.
   *
   * @return The blur radius in pixels
   */
  float GetBlurRadius() const;

  /**
   * @brief Sets how far the shadow reaches inward from each logical edge.
   *
   * @param[in] insets The start, end, top, and bottom insets in pixels
   * @return This inner shadow
   */
  InnerShadow& SetInsets(const Insets& insets);

  /**
   * @brief Gets how far the shadow reaches inward from each logical edge.
   *
   * @return The start, end, top, and bottom insets in pixels
   */
  const Insets& GetInsets() const;

private:
  class Impl;
  Impl* mImpl{nullptr};
};

} // namespace Ui
} // namespace Dali
