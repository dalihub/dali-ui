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
#include <dali/public-api/object/base-object.h>
#include <cstddef>
#include <cstdint>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/styled-text-impl.h>
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

struct OpenSpanAttachment
{
  Dali::Ui::Text::Span span;
  uint32_t             startIndex;
  uint32_t             insertionOrder;
  uint32_t             token;
};

/**
 * @brief Mutable builder implementation for StyledText snapshots.
 */
class StyledTextBuilder : public BaseObject
{
public:
  explicit StyledTextBuilder(const Dali::String& text);
  explicit StyledTextBuilder(const StyledText& styledText);
  StyledTextBuilder(const Dali::String& text, std::vector<SpanAttachment>&& attachments, uint32_t utf32Length);
  ~StyledTextBuilder() override;

  void     SetText(const Dali::String& text);
  void     AppendText(const Dali::String& text);
  uint32_t PushSpan(const Dali::Ui::Text::Span& span);
  bool     PopSpan();
  bool     PopSpan(uint32_t token);
  bool     SetSpan(const Dali::Ui::Text::Span& span, uint32_t utf32StartIndex, uint32_t utf32EndIndex);
  bool     RemoveSpan(const Dali::Ui::Text::Span& span);
  bool     RemoveSpanAt(uint32_t index);
  void     ClearSpans();

  const Dali::String&            GetText() const;
  uint32_t                       GetUtf32Length() const;
  uint32_t                       GetSpanCount() const;
  Dali::Ui::Text::Span           GetSpanAt(uint32_t index) const;
  uint32_t                       GetSpanStartIndexAt(uint32_t index) const;
  uint32_t                       GetSpanEndIndexAt(uint32_t index) const;
  uint32_t                       GetAnnotationCount() const;
  Dali::Ui::Text::AnnotationSpan GetAnnotationAt(uint32_t index) const;
  uint32_t                       GetAnnotationStartIndexAt(uint32_t index) const;
  uint32_t                       GetAnnotationEndIndexAt(uint32_t index) const;

  const std::vector<SpanAttachment>& GetAttachments() const;
  bool                               HasOpenSpans() const;
  std::vector<SpanAttachment>        CreateFinalizedAttachments() const;

private:
  StyledTextBuilder(const StyledTextBuilder&)            = delete;
  StyledTextBuilder(StyledTextBuilder&&)                 = delete;
  StyledTextBuilder& operator=(const StyledTextBuilder&) = delete;
  StyledTextBuilder& operator=(StyledTextBuilder&&)      = delete;

private:
  uint32_t AllocateSpanToken();
  void     AddAttachment(const Dali::Ui::Text::Span& span, uint32_t startIndex, uint32_t endIndex, uint32_t insertionOrder);
  void     CloseOpenSpansFromIndex(std::size_t index);
  bool     IsAnnotationSpan(const Dali::Ui::Text::Span& span) const;
  void     OnAttachmentInserted(uint32_t insertedIndex, const Dali::Ui::Text::Span& span);
  void     OnAttachmentRemoved(uint32_t removedIndex);
  void     EnsureAnnotationIndices() const;

private:
  Dali::String                    mText;
  std::vector<SpanAttachment>     mAttachments;
  std::vector<OpenSpanAttachment> mOpenSpans;
  mutable std::vector<uint32_t>   mAnnotationIndices;
  uint32_t                        mUtf32Length{0u};
  uint32_t                        mNextInsertionOrder{0u};
  uint32_t                        mNextSpanToken{1u};
  mutable bool                    mAnnotationIndicesDirty{true};
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
