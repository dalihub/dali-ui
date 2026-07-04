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
#include <dali-ui-foundation/public-api/text/style/line-through.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

/**
 * @brief Span payload that applies line-through styling to text.
 */
class DALI_UI_API LineThroughSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized LineThroughSpan handle.
   */
  LineThroughSpan() = default;

  /**
   * @brief Creates a LineThroughSpan with the given line-through style.
   *
   * @param[in] lineThrough The line-through style payload
   * @return A new LineThroughSpan
   */
  static LineThroughSpan New(const Text::LineThrough& lineThrough);

  /**
   * @brief Downcasts a handle to LineThroughSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized LineThroughSpan on success, otherwise empty
   */
  static LineThroughSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the line-through style payload.
   *
   * @return The line-through style
   */
  Text::LineThrough GetLineThrough() const;

private:
  /**
   * @brief Creates a LineThroughSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL LineThroughSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
