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
 * @brief Span payload that applies a foreground text color.
 */
class DALI_UI_API ForegroundColorSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized ForegroundColorSpan handle.
   */
  ForegroundColorSpan() = default;

  /**
   * @brief Creates a ForegroundColorSpan with the given color.
   *
   * @param[in] color The foreground color
   * @return A new ForegroundColorSpan
   */
  static ForegroundColorSpan New(const UiColor& color);

  /**
   * @brief Downcasts a handle to ForegroundColorSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized ForegroundColorSpan on success, otherwise empty
   */
  static ForegroundColorSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the foreground color payload.
   *
   * @return The foreground color
   */
  UiColor GetColor() const;

private:
  /**
   * @brief Creates a ForegroundColorSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL ForegroundColorSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
