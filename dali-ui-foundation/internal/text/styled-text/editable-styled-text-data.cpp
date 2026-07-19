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
 */

// EXTERNAL INCLUDES
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/editable-styled-text-data.h>
#include <dali-ui-foundation/public-api/text/styled-text/replacement-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>

namespace Dali::Ui::Text
{
namespace
{
bool Intersects(uint32_t firstStart, uint32_t firstEnd, uint32_t secondStart, uint32_t secondEnd)
{
  return firstStart < secondEnd && secondStart < firstEnd;
}
} // unnamed namespace

void EditableStyledTextData::Set(const StyledText& styledText)
{
  mAttachments.clear();
  if(!styledText)
  {
    return;
  }

  const uint32_t count = styledText.GetSpanCount();
  mAttachments.reserve(count);
  for(uint32_t index = 0u; index < count; ++index)
  {
    Span span = styledText.GetSpanAt(index);
    if(span)
    {
      mAttachments.push_back({span,
                              styledText.GetSpanStartIndexAt(index),
                              styledText.GetSpanEndIndexAt(index),
                              static_cast<bool>(ReplacementSpan::DownCast(span))});
    }
  }
}

void EditableStyledTextData::ApplyEdit(uint32_t start, uint32_t removedLength, uint32_t insertedLength)
{
  const uint32_t removedEnd = start + removedLength;
  auto           iterator   = mAttachments.begin();
  while(iterator != mAttachments.end())
  {
    Attachment& attachment = *iterator;
    if(removedLength > 0u)
    {
      if(attachment.replacement && Intersects(start, removedEnd, attachment.start, attachment.end))
      {
        iterator = mAttachments.erase(iterator);
        continue;
      }

      if(attachment.end <= start)
      {
        // The attachment precedes the edit.
      }
      else if(attachment.start >= removedEnd)
      {
        attachment.start -= removedLength;
        attachment.end -= removedLength;
      }
      else
      {
        const uint32_t updatedStart = attachment.start < start ? attachment.start : start;
        const uint32_t updatedEnd   = attachment.end <= removedEnd ? start : attachment.end - removedLength;
        attachment.start            = updatedStart;
        attachment.end              = updatedEnd;
        if(attachment.start >= attachment.end)
        {
          iterator = mAttachments.erase(iterator);
          continue;
        }
      }
    }

    if(insertedLength > 0u)
    {
      if(start == 0u && attachment.start == 0u && !attachment.replacement)
      {
        attachment.end += insertedLength;
      }
      else if(start <= attachment.start)
      {
        attachment.start += insertedLength;
        attachment.end += insertedLength;
      }
      else if(!attachment.replacement && start <= attachment.end)
      {
        attachment.end += insertedLength;
      }
    }
    ++iterator;
  }
}

StyledText EditableStyledTextData::Build(const std::string& text) const
{
  StyledTextBuilder builder = StyledTextBuilder::New(Dali::String(text.c_str()));
  for(const Attachment& attachment : mAttachments)
  {
    builder.SetSpan(attachment.span, attachment.start, attachment.end);
  }
  return builder.Build();
}

} // namespace Dali::Ui::Text
