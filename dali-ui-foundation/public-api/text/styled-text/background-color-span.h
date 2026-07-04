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

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/styled-text/span.h>
#include <dali-ui-foundation/public-api/types/ui-color.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

/**
 * @brief Span payload that applies a background color behind text.
 */
class DALI_UI_API BackgroundColorSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized BackgroundColorSpan handle.
   */
  BackgroundColorSpan() = default;

  /**
   * @brief Creates a BackgroundColorSpan with the given color.
   *
   * @param[in] color The background color
   * @return A new BackgroundColorSpan
   */
  static BackgroundColorSpan New(const UiColor& color);

  /**
   * @brief Downcasts a handle to BackgroundColorSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized BackgroundColorSpan on success, otherwise empty
   */
  static BackgroundColorSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the background color payload.
   *
   * @return The background color
   */
  UiColor GetColor() const;

private:
  /**
   * @brief Creates a BackgroundColorSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL BackgroundColorSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
