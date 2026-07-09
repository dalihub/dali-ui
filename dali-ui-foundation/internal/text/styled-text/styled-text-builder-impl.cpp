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
#include <limits>
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

namespace
{
constexpr uint32_t INVALID_SPAN_TOKEN = 0u;

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

StyledTextBuilder::StyledTextBuilder(const Dali::String& text, std::vector<SpanAttachment>&& attachments, uint32_t utf32Length)
: mText(text),
  mAttachments(std::move(attachments)),
  mUtf32Length(utf32Length),
  mNextInsertionOrder(GetNextInsertionOrder(mAttachments))
{
}

StyledTextBuilder::~StyledTextBuilder() = default;

void StyledTextBuilder::SetText(const Dali::String& text)
{
  mText = text;
  mAttachments.clear();
  mOpenSpans.clear();
  mAnnotationIndices.clear();
  mUtf32Length            = Dali::Ui::Text::Utf8ToUtf32Length(text);
  mNextInsertionOrder     = 0u;
  mNextSpanToken          = 1u;
  mAnnotationIndicesDirty = false;
}

void StyledTextBuilder::AppendText(const Dali::String& text)
{
  mText += text;
  mUtf32Length += Dali::Ui::Text::Utf8ToUtf32Length(text);
}

uint32_t StyledTextBuilder::PushSpan(const Dali::Ui::Text::Span& span)
{
  if(!span)
  {
    return INVALID_SPAN_TOKEN;
  }

  const uint32_t token = AllocateSpanToken();
  if(token == INVALID_SPAN_TOKEN)
  {
    return INVALID_SPAN_TOKEN;
  }

  mOpenSpans.push_back({span, mUtf32Length, mNextInsertionOrder++, token});
  return token;
}

bool StyledTextBuilder::PopSpan()
{
  if(mOpenSpans.empty())
  {
    return false;
  }

  CloseOpenSpansFromIndex(mOpenSpans.size() - 1u);
  return true;
}

bool StyledTextBuilder::PopSpan(uint32_t token)
{
  if(token == INVALID_SPAN_TOKEN)
  {
    return false;
  }

  auto iter = std::find_if(mOpenSpans.begin(), mOpenSpans.end(), [token](const OpenSpanAttachment& attachment)
  {
    return attachment.token == token;
  });

  if(iter == mOpenSpans.end())
  {
    return false;
  }

  CloseOpenSpansFromIndex(static_cast<std::size_t>(iter - mOpenSpans.begin()));
  return true;
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

  AddAttachment(span, utf32StartIndex, utf32EndIndex, mNextInsertionOrder++);
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

  const uint32_t removedIndex = static_cast<uint32_t>(iter - mAttachments.begin());
  mAttachments.erase(iter);
  OnAttachmentRemoved(removedIndex);
  return true;
}

bool StyledTextBuilder::RemoveSpanAt(uint32_t index)
{
  if(index >= mAttachments.size())
  {
    return false;
  }

  mAttachments.erase(mAttachments.begin() + index);
  OnAttachmentRemoved(index);
  return true;
}

void StyledTextBuilder::ClearSpans()
{
  mAttachments.clear();
  mOpenSpans.clear();
  mAnnotationIndices.clear();
  mNextInsertionOrder     = 0u;
  mNextSpanToken          = 1u;
  mAnnotationIndicesDirty = false;
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

uint32_t StyledTextBuilder::GetAnnotationCount() const
{
  EnsureAnnotationIndices();
  return static_cast<uint32_t>(mAnnotationIndices.size());
}

Dali::Ui::Text::AnnotationSpan StyledTextBuilder::GetAnnotationAt(uint32_t index) const
{
  EnsureAnnotationIndices();
  if(index >= mAnnotationIndices.size())
  {
    return Dali::Ui::Text::AnnotationSpan();
  }

  return Dali::Ui::Text::AnnotationSpan::DownCast(mAttachments[mAnnotationIndices[index]].span);
}

uint32_t StyledTextBuilder::GetAnnotationStartIndexAt(uint32_t index) const
{
  EnsureAnnotationIndices();
  if(index >= mAnnotationIndices.size())
  {
    return 0u;
  }

  return mAttachments[mAnnotationIndices[index]].startIndex;
}

uint32_t StyledTextBuilder::GetAnnotationEndIndexAt(uint32_t index) const
{
  EnsureAnnotationIndices();
  if(index >= mAnnotationIndices.size())
  {
    return 0u;
  }

  return mAttachments[mAnnotationIndices[index]].endIndex;
}

const std::vector<SpanAttachment>& StyledTextBuilder::GetAttachments() const
{
  return mAttachments;
}

bool StyledTextBuilder::HasOpenSpans() const
{
  return !mOpenSpans.empty();
}

std::vector<SpanAttachment> StyledTextBuilder::CreateFinalizedAttachments() const
{
  std::vector<SpanAttachment> finalizedAttachments = mAttachments;
  finalizedAttachments.reserve(mAttachments.size() + mOpenSpans.size());

  for(std::size_t spanIndex = mOpenSpans.size(); spanIndex > 0u; --spanIndex)
  {
    const OpenSpanAttachment& openSpan = mOpenSpans[spanIndex - 1u];
    if(openSpan.startIndex >= mUtf32Length)
    {
      continue;
    }

    auto existing = std::find_if(finalizedAttachments.begin(), finalizedAttachments.end(), [&openSpan](const SpanAttachment& attachment)
    {
      return SameSpanObject(attachment.span, openSpan.span);
    });
    if(existing != finalizedAttachments.end())
    {
      existing->startIndex = openSpan.startIndex;
      existing->endIndex   = mUtf32Length;
      continue;
    }

    SpanAttachment attachment{openSpan.span, openSpan.startIndex, mUtf32Length, openSpan.insertionOrder};
    auto           iter = std::find_if(finalizedAttachments.begin(), finalizedAttachments.end(), [&openSpan](const SpanAttachment& existing)
              {
      return existing.insertionOrder > openSpan.insertionOrder;
    });

    finalizedAttachments.insert(iter, std::move(attachment));
  }

  return finalizedAttachments;
}

uint32_t StyledTextBuilder::AllocateSpanToken()
{
  for(uint32_t attempt = 0u; attempt < std::numeric_limits<uint32_t>::max(); ++attempt)
  {
    const uint32_t token = mNextSpanToken++;
    if(token == INVALID_SPAN_TOKEN)
    {
      continue;
    }

    const auto iter = std::find_if(mOpenSpans.begin(), mOpenSpans.end(), [token](const OpenSpanAttachment& attachment)
    {
      return attachment.token == token;
    });
    if(iter == mOpenSpans.end())
    {
      return token;
    }
  }

  return INVALID_SPAN_TOKEN;
}

void StyledTextBuilder::AddAttachment(const Dali::Ui::Text::Span& span, uint32_t startIndex, uint32_t endIndex, uint32_t insertionOrder)
{
  if(startIndex >= endIndex)
  {
    return;
  }

  auto existing = std::find_if(mAttachments.begin(), mAttachments.end(), [&span](const SpanAttachment& attachment)
  {
    return SameSpanObject(attachment.span, span);
  });
  if(existing != mAttachments.end())
  {
    existing->startIndex = startIndex;
    existing->endIndex   = endIndex;
    return;
  }

  SpanAttachment attachment{span, startIndex, endIndex, insertionOrder};
  auto           iter = std::find_if(mAttachments.begin(), mAttachments.end(), [insertionOrder](const SpanAttachment& existing)
            {
    return existing.insertionOrder > insertionOrder;
  });

  const uint32_t insertedIndex = static_cast<uint32_t>(iter - mAttachments.begin());
  mAttachments.insert(iter, std::move(attachment));
  OnAttachmentInserted(insertedIndex, span);
}

void StyledTextBuilder::CloseOpenSpansFromIndex(std::size_t index)
{
  for(std::size_t spanIndex = mOpenSpans.size(); spanIndex > index; --spanIndex)
  {
    const OpenSpanAttachment& openSpan = mOpenSpans[spanIndex - 1u];
    if(openSpan.startIndex < mUtf32Length)
    {
      AddAttachment(openSpan.span, openSpan.startIndex, mUtf32Length, openSpan.insertionOrder);
    }
  }

  mOpenSpans.erase(mOpenSpans.begin() + index, mOpenSpans.end());
}

bool StyledTextBuilder::IsAnnotationSpan(const Dali::Ui::Text::Span& span) const
{
  return static_cast<bool>(Dali::Ui::Text::AnnotationSpan::DownCast(span));
}

void StyledTextBuilder::OnAttachmentInserted(uint32_t insertedIndex, const Dali::Ui::Text::Span& span)
{
  if(mAnnotationIndicesDirty)
  {
    return;
  }

  for(uint32_t& annotationIndex : mAnnotationIndices)
  {
    if(annotationIndex >= insertedIndex)
    {
      ++annotationIndex;
    }
  }

  if(IsAnnotationSpan(span))
  {
    auto iter = std::lower_bound(mAnnotationIndices.begin(), mAnnotationIndices.end(), insertedIndex);
    mAnnotationIndices.insert(iter, insertedIndex);
  }
}

void StyledTextBuilder::OnAttachmentRemoved(uint32_t removedIndex)
{
  if(mAnnotationIndicesDirty)
  {
    return;
  }

  for(auto iter = mAnnotationIndices.begin(); iter != mAnnotationIndices.end();)
  {
    if(*iter == removedIndex)
    {
      iter = mAnnotationIndices.erase(iter);
      continue;
    }

    if(*iter > removedIndex)
    {
      --(*iter);
    }

    ++iter;
  }
}

void StyledTextBuilder::EnsureAnnotationIndices() const
{
  if(!mAnnotationIndicesDirty)
  {
    return;
  }

  mAnnotationIndices.clear();
  mAnnotationIndices.reserve(mAttachments.size());

  for(uint32_t index = 0u; index < mAttachments.size(); ++index)
  {
    if(IsAnnotationSpan(mAttachments[index].span))
    {
      mAnnotationIndices.push_back(index);
    }
  }

  mAnnotationIndicesDirty = false;
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
