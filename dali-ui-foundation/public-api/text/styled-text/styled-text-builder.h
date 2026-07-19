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
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
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
 * StyledTextBuilder stores UTF-8 text while building a StyledText snapshot.
 * Span range indexes are indexes in the decoded UTF-32 sequence of the current
 * UTF-8 text.
 *
 * Range indexes are not UTF-8 byte offsets. Use Text::Utf8ToUtf32Range() when
 * converting byte ranges from UTF-8 string APIs. When building text
 * incrementally, use GetUtf32Length() before and after AppendText() to calculate
 * the range of appended text.
 */
class DALI_UI_API StyledTextBuilder : public BaseHandle
{
public:
  /**
   * @brief Invalid span token returned by PushSpan() on failure.
   */
  static constexpr uint32_t INVALID_SPAN_TOKEN = 0u;

  /**
   * @brief Creates an uninitialized StyledTextBuilder handle.
   */
  StyledTextBuilder() = default;

  /**
   * @brief Creates an empty StyledTextBuilder.
   *
   * @return A new StyledTextBuilder
   */
  static StyledTextBuilder New();

  /**
   * @brief Creates a StyledTextBuilder with initial UTF-8 text.
   *
   * @param[in] text The initial UTF-8 text payload
   * @return A new StyledTextBuilder
   */
  static StyledTextBuilder New(const Dali::String& text);

  /**
   * @brief Creates a builder initialized from a StyledText snapshot.
   *
   * @param[in] styledText The source snapshot
   * @return A new StyledTextBuilder
   */
  static StyledTextBuilder FromStyledText(const StyledText& styledText);

  /**
   * @brief Converts supported DALi markup into a StyledTextBuilder.
   *
   * Supported DALi markup tags are converted into spans. Each attribute in an
   * annotation markup tag is converted into a separate AnnotationSpan key/value
   * pair over the annotated text range. Unsupported tags are skipped on a
   * best-effort basis while preserving their text content where possible. Entity
   * references recognized by DALi markup are decoded in text content and
   * attribute values.
   *
   * An img start tag with a non-empty src and finite, positive logical-pixel
   * width and height values inserts an ImageSpan at its position. The img tag
   * is treated as a void element. A matching end tag is ignored, and text after
   * the start tag remains ordinary following text.
   *
   * @param[in] markup The markup string to convert
   * @return A new StyledTextBuilder
   */
  static StyledTextBuilder FromMarkup(const Dali::String& markup);

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
   * @brief Opens a span at the current decoded UTF-32 text end.
   *
   * The returned token is local to this builder and remains valid only while
   * the pushed span is still open. 0u is an invalid token.
   *
   * @param[in] span The span payload
   * @return A non-zero token on success, or 0u if validation failed
   */
  uint32_t PushSpan(const Span& span);

  /**
   * @brief Closes the most recently pushed open span.
   *
   * Empty ranges are closed without creating a span attachment.
   *
   * @return true if an open span was closed
   */
  bool PopSpan();

  /**
   * @brief Closes the open span matching the token and any spans above it.
   *
   * The token must have been returned by PushSpan() from this builder and must
   * still be open. 0u is an invalid token.
   *
   * @param[in] token The token returned by PushSpan()
   * @return true if the token was found and closed
   */
  bool PopSpan(uint32_t token);

  /**
   * @brief Attaches or updates a span over a half-open UTF-32 range.
   *
   * The range is [utf32StartIndex, utf32EndIndex) in the decoded UTF-32 sequence
   * of the builder's current UTF-8 text.
   *
   * The range is not a UTF-8 byte range. Use Text::Utf8ToUtf32Range() to convert
   * UTF-8 byte ranges, for example ranges returned from std::string::find().
   *
   * A builder stores at most one attachment for the same Span handle. Calling
   * SetSpan() again with the same Span handle updates the existing attachment
   * range without changing its insertion order. The same visual style can be
   * attached to different ranges by creating separate Span handles.
   *
   * @param[in] span The span payload
   * @param[in] utf32StartIndex The inclusive UTF-32 start index
   * @param[in] utf32EndIndex The exclusive UTF-32 end index
   * @return true if the attachment was stored, false if validation failed
   */
  bool SetSpan(const Span& span, uint32_t utf32StartIndex, uint32_t utf32EndIndex);

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
   * @brief Gets the number of attached annotation spans.
   *
   * @return The annotation span attachment count
   */
  uint32_t GetAnnotationCount() const;

  /**
   * @brief Gets the annotation span payload at the given annotation index.
   *
   * @param[in] index The annotation index
   * @return The annotation span payload, or an empty AnnotationSpan if the index is invalid
   */
  AnnotationSpan GetAnnotationAt(uint32_t index) const;

  /**
   * @brief Gets the inclusive UTF-32 start index of the annotation span attachment.
   *
   * The annotation index should be less than GetAnnotationCount().
   *
   * @param[in] index The annotation index
   * @return The UTF-32 start index
   */
  uint32_t GetAnnotationStartIndexAt(uint32_t index) const;

  /**
   * @brief Gets the exclusive UTF-32 end index of the annotation span attachment.
   *
   * The annotation index should be less than GetAnnotationCount().
   *
   * @param[in] index The annotation index
   * @return The exclusive UTF-32 end index
   */
  uint32_t GetAnnotationEndIndexAt(uint32_t index) const;

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
