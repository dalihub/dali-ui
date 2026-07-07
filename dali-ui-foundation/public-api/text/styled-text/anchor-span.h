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
#include <dali-ui-foundation/public-api/text/style/anchor-attributes.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

/**
 * @brief Span payload that marks a text range as an anchor.
 *
 * AnchorSpan stores only the anchor attribute payload. The text range is owned
 * by the StyledTextBuilder attachment that uses the span. The payload is
 * provided when the span is created and is not mutated through public setters.
 */
class DALI_UI_API AnchorSpan : public Span
{
public:
  /**
   * @brief Creates an uninitialized AnchorSpan handle.
   */
  AnchorSpan() = default;

  /**
   * @brief Creates an AnchorSpan with the given anchor attributes.
   *
   * The HREF attribute is required. An explicitly set empty HREF is valid, but
   * an unset HREF returns an empty handle.
   *
   * @param[in] attributes The anchor attribute payload
   * @return A new AnchorSpan, or an empty handle when HREF is unset
   */
  static AnchorSpan New(const Text::AnchorAttributes& attributes);

  /**
   * @brief Downcasts a handle to AnchorSpan.
   *
   * @param[in] handle The handle to downcast
   * @return An initialized AnchorSpan on success, otherwise empty
   */
  static AnchorSpan DownCast(BaseHandle handle);

  /**
   * @brief Gets the anchor attribute payload.
   *
   * @return The anchor attributes
   */
  Text::AnchorAttributes GetAnchorAttributes() const;

private:
  /**
   * @brief Creates an AnchorSpan handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL AnchorSpan(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
