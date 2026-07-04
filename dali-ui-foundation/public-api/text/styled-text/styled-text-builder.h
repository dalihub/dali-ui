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
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>

namespace Dali
{
namespace Ui
{

namespace Text
{

/**
 * @brief Mutable builder for creating immutable StyledText snapshots.
 *
 * Span ranges use UTF-32 code point indices in the plain text.
 */
class DALI_UI_API StyledTextBuilder : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized StyledTextBuilder handle.
   */
  StyledTextBuilder() = default;

  /**
   * @brief Creates a builder with initial text and no spans.
   *
   * @param[in] text The initial text
   * @return A new StyledTextBuilder
   */
  static StyledTextBuilder New(const Dali::String& text = Dali::String());

  /**
   * @brief Creates a builder initialized from a StyledText snapshot.
   *
   * @param[in] styledText The source snapshot
   * @return A new StyledTextBuilder
   */
  static StyledTextBuilder FromStyledText(const StyledText& styledText);

  /**
   * @brief Replaces the text and clears all span attachments.
   *
   * @param[in] text The new text
   */
  void SetText(const Dali::String& text);

  /**
   * @brief Appends text without extending existing span attachments.
   *
   * @param[in] text The text to append
   */
  void AppendText(const Dali::String& text);

  /**
   * @brief Attaches or updates a span over a half-open UTF-32 code point range.
   *
   * The range is [startIndex, endIndex) in the builder's plain text.
   *
   * A builder stores at most one attachment for the same Span handle. Calling
   * SetSpan() again with the same Span handle updates the existing attachment
   * instead of adding another one. Multiple equal-looking span payloads can be
   * attached to different ranges by creating separate Span handles.
   *
   * @param[in] span The span payload
   * @param[in] startIndex The inclusive UTF-32 code point start index
   * @param[in] endIndex The exclusive UTF-32 code point end index
   * @return true if the attachment was stored, false if validation failed
   */
  bool SetSpan(const Span& span, uint32_t startIndex, uint32_t endIndex);

  /**
   * @brief Removes the attachment for the same Span handle.
   *
   * The match is based on Span handle identity.
   *
   * @param[in] span The span payload to remove
   * @return true if an attachment was removed
   */
  bool RemoveSpan(const Span& span);

  /**
   * @brief Removes the attachment at the given index.
   *
   * @param[in] index The attachment index
   * @return true if an attachment was removed
   */
  bool RemoveSpanAt(uint32_t index);

  /**
   * @brief Clears all span attachments.
   */
  void ClearSpans();

  /**
   * @brief Gets the current text payload.
   *
   * @return The current text payload
   */
  Dali::String GetText() const;

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
   * @brief Gets the inclusive UTF-32 code point start index of the span attachment.
   *
   * @param[in] index The attachment index
   * @return The UTF-32 code point start index, or 0 if the index is invalid
   */
  uint32_t GetSpanStartIndexAt(uint32_t index) const;

  /**
   * @brief Gets the exclusive UTF-32 code point end index of the span attachment.
   *
   * @param[in] index The attachment index
   * @return The UTF-32 code point end index, or 0 if the index is invalid
   */
  uint32_t GetSpanEndIndexAt(uint32_t index) const;

  /**
   * @brief Builds an immutable StyledText snapshot.
   *
   * @return A StyledText snapshot
   */
  StyledText Build() const;

private:
  /**
   * @brief Creates a StyledTextBuilder handle from its implementation.
   *
   * @param[in] impl The implementation object
   */
  explicit DALI_INTERNAL StyledTextBuilder(BaseObject* impl);
};

} // namespace Text
} // namespace Ui
} // namespace Dali
