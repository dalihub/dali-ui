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
#include <cstdint>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/span.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace Text
{

struct SpanAttachment
{
  Dali::Ui::Text::Span span;
  uint32_t             startIndex;
  uint32_t             endIndex;
  uint32_t             insertionOrder;
};

/**
 * @brief Immutable styled text snapshot implementation.
 */
class StyledText : public BaseObject
{
public:
  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL StyledText* NewForBaseHandle(const Dali::String& text)
  {
    return new StyledText(text);
  }

  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL StyledText* NewForBaseHandle(const Dali::String& text, const std::vector<SpanAttachment>& attachments, uint32_t utf32Length)
  {
    return new StyledText(text, attachments, utf32Length);
  }

  /**
   * @brief Creates an unreferenced implementation only for BaseHandle construction.
   * @warning Pass the result directly to a BaseHandle-derived handle constructor.
   * The constructor must acquire its first reference (the returned reference count is zero).
   * Use New() when an IntrusivePtr owner is required.
   */
  static DALI_INTERNAL StyledText* NewForBaseHandle(const Dali::String& text, std::vector<SpanAttachment>&& attachments, uint32_t utf32Length)
  {
    return new StyledText(text, std::move(attachments), utf32Length);
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<StyledText> New()
  {
    return IntrusivePtr<StyledText>(new StyledText());
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<StyledText> New(const Dali::String& text)
  {
    return IntrusivePtr<StyledText>(new StyledText(text));
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<StyledText> New(const Dali::String& text, const std::vector<SpanAttachment>& attachments)
  {
    return IntrusivePtr<StyledText>(new StyledText(text, attachments));
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<StyledText> New(const Dali::String& text, const std::vector<SpanAttachment>& attachments, uint32_t utf32Length)
  {
    return IntrusivePtr<StyledText>(new StyledText(text, attachments, utf32Length));
  }

  /**
   * @brief Creates a reference-counted implementation.
   */
  static IntrusivePtr<StyledText> New(const Dali::String& text, std::vector<SpanAttachment>&& attachments, uint32_t utf32Length)
  {
    return IntrusivePtr<StyledText>(new StyledText(text, std::move(attachments), utf32Length));
  }

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
  bool                           IsEmpty() const;

  const std::vector<SpanAttachment>& GetAttachments() const;

protected:
  StyledText();
  explicit StyledText(const Dali::String& text);
  StyledText(const Dali::String& text, const std::vector<SpanAttachment>& attachments);
  StyledText(const Dali::String& text, const std::vector<SpanAttachment>& attachments, uint32_t utf32Length);
  StyledText(const Dali::String& text, std::vector<SpanAttachment>&& attachments, uint32_t utf32Length);
  ~StyledText() override;

private:
  StyledText(const StyledText&)            = delete;
  StyledText(StyledText&&)                 = delete;
  StyledText& operator=(const StyledText&) = delete;
  StyledText& operator=(StyledText&&)      = delete;

private:
  void BuildAnnotationIndices();

private:
  Dali::String                mText;
  std::vector<SpanAttachment> mAttachments;
  std::vector<uint32_t>       mAnnotationIndices;
  uint32_t                    mUtf32Length{0u};
};

} // namespace Text
} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
