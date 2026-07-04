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
#include <dali-ui-foundation/public-api/text/style/font-attributes.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

/**
 * @brief Span payload that applies partial font attribute overrides to text.
 *
 * FontSpan stores only the font attribute payload. The text range is owned by
 * the StyledTextBuilder attachment that uses the span. The payload is provided
 * when the span is created and is not mutated through public setters.
 */
class DALI_UI_API FontSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized FontSpan handle.
   */
  FontSpan() = default;

  /**
   * @brief Creates a FontSpan with the given font attributes.
   *
   * @param[in] attributes The partial font attribute override payload
   * @return A new FontSpan
   */
  static FontSpan New(const Text::FontAttributes& attributes);

  /**
   * @brief Downcasts a handle to FontSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized FontSpan on success, otherwise empty
   */
  static FontSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the font attribute override payload.
   *
   * @return The font attributes
   */
  Text::FontAttributes GetFontAttributes() const;

private:
  /**
   * @brief Creates a FontSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL FontSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
