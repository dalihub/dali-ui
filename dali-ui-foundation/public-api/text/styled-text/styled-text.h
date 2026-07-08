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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/base-handle.h>
#include <cstdint>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/dali-ui-common.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

class StyledTextBuilder;

/**
 * @brief Immutable styled text snapshot.
 *
 * StyledText stores UTF-8 text together with span attachments. Span range
 * indexes are indexes in the decoded UTF-32 sequence of that UTF-8 text.
 *
 * Range indexes are not UTF-8 byte offsets. Use Text::Utf8ToUtf32Range() when
 * converting byte ranges from UTF-8 string APIs, and use GetUtf32Length() when
 * a range should cover this whole snapshot.
 */
class DALI_UI_API StyledText : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized StyledText handle.
   */
  StyledText() = default;

  /**
   * @brief Creates a StyledText snapshot with plain text and no spans.
   *
   * @param[in] text The text payload
   * @return A new StyledText
   */
  static StyledText New(const Dali::String& text);

  /**
   * @brief Converts supported DALi markup into a StyledText snapshot.
   *
   * Supported markup tags are converted into spans. Unsupported tags are skipped
   * on a best-effort basis while preserving their text content where possible.
   * Text content and attribute values are decoded for supported XHTML entities.
   *
   * @param[in] markup The markup string to convert
   * @return A new StyledText snapshot
   */
  static StyledText FromMarkup(const Dali::String& markup);

  /**
   * @brief Gets the text payload.
   *
   * @return The text payload
   */
  Dali::String GetText() const;

  /**
   * @brief Gets the decoded UTF-32 length of the text payload.
   *
   * The returned value is the same unit used by span range indexes. It is not
   * the UTF-8 byte size returned by Dali::String::Size().
   *
   * @return The decoded UTF-32 length
   */
  uint32_t GetUtf32Length() const;

  /**
   * @brief Gets the number of attached spans.
   *
   * @return The span attachment count
   */
  uint32_t GetSpanCount() const;

  /**
   * @brief Gets the span payload at the given attachment index.
   *
   * @param[in] index The attachment index
   * @return The span payload, or an empty Span if the index is invalid
   */
  Span GetSpanAt(uint32_t index) const;

  /**
   * @brief Gets the inclusive UTF-32 start index of the span attachment.
   *
   * The attachment index should be less than GetSpanCount().
   *
   * @param[in] index The attachment index
   * @return The UTF-32 start index
   */
  uint32_t GetSpanStartIndexAt(uint32_t index) const;

  /**
   * @brief Gets the exclusive UTF-32 end index of the span attachment.
   *
   * The attachment index should be less than GetSpanCount().
   *
   * @param[in] index The attachment index
   * @return The exclusive UTF-32 end index
   */
  uint32_t GetSpanEndIndexAt(uint32_t index) const;

  /**
   * @brief Returns whether this snapshot has empty text.
   *
   * @return true if the text payload is empty
   */
  bool IsEmpty() const;

private:
  /**
   * @brief Creates a StyledText handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL StyledText(BaseObject* impl);

  friend class StyledTextBuilder;
};

} // namespace Text
} // namespace Ui
} // namespace Dali
