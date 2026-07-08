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
#include <dali-ui-foundation/internal/text/styled-text/styled-text-builder-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cstdint>

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

namespace
{

bool SameSpanObject(const Dali::Ui::Text::Span& lhs, const Dali::Ui::Text::Span& rhs)
{
  return lhs.GetObjectPtr() == rhs.GetObjectPtr();
}

uint32_t GetNextInsertionOrder(const std::vector<SpanAttachment>& attachments)
{
  uint32_t nextInsertionOrder = 0u;
  for(const auto& attachment : attachments)
  {
    nextInsertionOrder = std::max(nextInsertionOrder, attachment.insertionOrder + 1u);
  }
  return nextInsertionOrder;
}

} // unnamed namespace

StyledTextBuilder::StyledTextBuilder(const Dali::String& text)
: mText(text),
  mUtf32Length(Dali::Ui::Text::Utf8ToUtf32Length(text))
{
}

StyledTextBuilder::StyledTextBuilder(const StyledText& styledText)
: mText(styledText.GetText()),
  mAttachments(styledText.GetAttachments()),
  mUtf32Length(styledText.GetUtf32Length()),
  mNextInsertionOrder(GetNextInsertionOrder(mAttachments))
{
}

StyledTextBuilder::~StyledTextBuilder() = default;

void StyledTextBuilder::SetText(const Dali::String& text)
{
  mText = text;
  mAttachments.clear();
  mUtf32Length        = Dali::Ui::Text::Utf8ToUtf32Length(text);
  mNextInsertionOrder = 0u;
}

void StyledTextBuilder::AppendText(const Dali::String& text)
{
  mText += text;
  mUtf32Length += Dali::Ui::Text::Utf8ToUtf32Length(text);
}

bool StyledTextBuilder::SetSpan(const Dali::Ui::Text::Span& span, uint32_t utf32StartIndex, uint32_t utf32EndIndex)
{
  if(!span || (utf32StartIndex >= utf32EndIndex) || (utf32EndIndex > mUtf32Length))
  {
    return false;
  }

  auto iter = std::find_if(mAttachments.begin(), mAttachments.end(), [&span](const SpanAttachment& attachment)
  {
    return SameSpanObject(attachment.span, span);
  });

  if(iter != mAttachments.end())
  {
    iter->startIndex = utf32StartIndex;
    iter->endIndex   = utf32EndIndex;
    return true;
  }

  mAttachments.push_back({span, utf32StartIndex, utf32EndIndex, mNextInsertionOrder++});
  return true;
}

bool StyledTextBuilder::RemoveSpan(const Dali::Ui::Text::Span& span)
{
  if(!span)
  {
    return false;
  }

  auto iter = std::find_if(mAttachments.begin(), mAttachments.end(), [&span](const SpanAttachment& attachment)
  {
    return SameSpanObject(attachment.span, span);
  });

  if(iter == mAttachments.end())
  {
    return false;
  }

  mAttachments.erase(iter);
  return true;
}

bool StyledTextBuilder::RemoveSpanAt(uint32_t index)
{
  if(index >= mAttachments.size())
  {
    return false;
  }

  mAttachments.erase(mAttachments.begin() + index);
  return true;
}

void StyledTextBuilder::ClearSpans()
{
  mAttachments.clear();
}

const Dali::String& StyledTextBuilder::GetText() const
{
  return mText;
}

uint32_t StyledTextBuilder::GetUtf32Length() const
{
  return mUtf32Length;
}

uint32_t StyledTextBuilder::GetSpanCount() const
{
  return static_cast<uint32_t>(mAttachments.size());
}

Dali::Ui::Text::Span StyledTextBuilder::GetSpanAt(uint32_t index) const
{
  if(index >= mAttachments.size())
  {
    return Dali::Ui::Text::Span();
  }

  return mAttachments[index].span;
}

uint32_t StyledTextBuilder::GetSpanStartIndexAt(uint32_t index) const
{
  if(index >= mAttachments.size())
  {
    return 0u;
  }

  return mAttachments[index].startIndex;
}

uint32_t StyledTextBuilder::GetSpanEndIndexAt(uint32_t index) const
{
  if(index >= mAttachments.size())
  {
    return 0u;
  }

  return mAttachments[index].endIndex;
}

const std::vector<SpanAttachment>& StyledTextBuilder::GetAttachments() const
{
  return mAttachments;
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
