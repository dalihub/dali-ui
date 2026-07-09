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

// CLASS HEADER
#include <dali-ui-foundation/internal/text/styled-text/styled-text-impl.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/text-utils.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

StyledText::StyledText() = default;

StyledText::StyledText(const Dali::String& text)
: mText(text),
  mUtf32Length(Dali::Ui::Text::Utf8ToUtf32Length(text))
{
}

StyledText::StyledText(const Dali::String& text, const std::vector<SpanAttachment>& attachments)
: mText(text),
  mAttachments(attachments),
  mUtf32Length(Dali::Ui::Text::Utf8ToUtf32Length(text))
{
  BuildAnnotationIndices();
}

StyledText::StyledText(const Dali::String& text, const std::vector<SpanAttachment>& attachments, uint32_t utf32Length)
: mText(text),
  mAttachments(attachments),
  mUtf32Length(utf32Length)
{
  BuildAnnotationIndices();
}

StyledText::StyledText(const Dali::String& text, std::vector<SpanAttachment>&& attachments, uint32_t utf32Length)
: mText(text),
  mAttachments(std::move(attachments)),
  mUtf32Length(utf32Length)
{
  BuildAnnotationIndices();
}

StyledText::~StyledText() = default;

const Dali::String& StyledText::GetText() const
{
  return mText;
}

uint32_t StyledText::GetUtf32Length() const
{
  return mUtf32Length;
}

uint32_t StyledText::GetSpanCount() const
{
  return static_cast<uint32_t>(mAttachments.size());
}

Dali::Ui::Text::Span StyledText::GetSpanAt(uint32_t index) const
{
  if(index >= mAttachments.size())
  {
    return Dali::Ui::Text::Span();
  }

  return mAttachments[index].span;
}

uint32_t StyledText::GetSpanStartIndexAt(uint32_t index) const
{
  if(index >= mAttachments.size())
  {
    return 0u;
  }

  return mAttachments[index].startIndex;
}

uint32_t StyledText::GetSpanEndIndexAt(uint32_t index) const
{
  if(index >= mAttachments.size())
  {
    return 0u;
  }

  return mAttachments[index].endIndex;
}

uint32_t StyledText::GetAnnotationCount() const
{
  return static_cast<uint32_t>(mAnnotationIndices.size());
}

Dali::Ui::Text::AnnotationSpan StyledText::GetAnnotationAt(uint32_t index) const
{
  if(index >= mAnnotationIndices.size())
  {
    return Dali::Ui::Text::AnnotationSpan();
  }

  return Dali::Ui::Text::AnnotationSpan::DownCast(mAttachments[mAnnotationIndices[index]].span);
}

uint32_t StyledText::GetAnnotationStartIndexAt(uint32_t index) const
{
  if(index >= mAnnotationIndices.size())
  {
    return 0u;
  }

  return mAttachments[mAnnotationIndices[index]].startIndex;
}

uint32_t StyledText::GetAnnotationEndIndexAt(uint32_t index) const
{
  if(index >= mAnnotationIndices.size())
  {
    return 0u;
  }

  return mAttachments[mAnnotationIndices[index]].endIndex;
}

bool StyledText::IsEmpty() const
{
  return mText.Empty();
}

const std::vector<SpanAttachment>& StyledText::GetAttachments() const
{
  return mAttachments;
}

void StyledText::BuildAnnotationIndices()
{
  mAnnotationIndices.clear();
  mAnnotationIndices.reserve(mAttachments.size());

  for(uint32_t index = 0u; index < mAttachments.size(); ++index)
  {
    if(Dali::Ui::Text::AnnotationSpan::DownCast(mAttachments[index].span))
    {
      mAnnotationIndices.push_back(index);
    }
  }
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
