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
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

namespace Text
{

/**
 * @brief Span payload that applies underline styling to text.
 * @SINCE_3_0.0
 */
class DALI_UI_API UnderlineSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized UnderlineSpan handle.
   * @SINCE_3_0.0
   */
  UnderlineSpan() = default;

  /**
   * @brief Creates an UnderlineSpan with the given underline style.
   *
   * @SINCE_3_0.0
   * @param[in] underline The underline style payload
   * @return A new UnderlineSpan
   */
  static UnderlineSpan New(const Text::Underline& underline);

  /**
   * @brief Downcasts a handle to UnderlineSpan.
   *
   * @SINCE_3_0.0
   * @param[in] handle The handle to downcast
   * @return An initialized UnderlineSpan on success, otherwise empty
   */
  static UnderlineSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the underline style payload.
   *
   * @SINCE_3_0.0
   * @return The underline style
   */
  Text::Underline GetUnderline() const;

private:
  /**
   * @brief Creates an UnderlineSpan handle from its implementation.
   *
   * @SINCE_3_0.0
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL UnderlineSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} //namespace DALI_NAMESPACE
