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
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/styled-text-builder-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

namespace
{

Internal::Text::StyledTextBuilder* GetImplementation(const StyledTextBuilder& builder)
{
  return static_cast<Internal::Text::StyledTextBuilder*>(builder.GetObjectPtr());
}

Internal::Text::StyledText* GetImplementation(const StyledText& styledText)
{
  return static_cast<Internal::Text::StyledText*>(styledText.GetObjectPtr());
}

} // unnamed namespace

StyledTextBuilder StyledTextBuilder::New()
{
  return New(Dali::String());
}

StyledTextBuilder StyledTextBuilder::New(const Dali::String& text)
{
  return StyledTextBuilder(new Internal::Text::StyledTextBuilder(text));
}

StyledTextBuilder StyledTextBuilder::FromStyledText(const StyledText& styledText)
{
  return styledText ? StyledTextBuilder(new Internal::Text::StyledTextBuilder(*GetImplementation(styledText))) : StyledTextBuilder::New();
}

void StyledTextBuilder::SetText(const Dali::String& text)
{
  if(*this)
  {
    GetImplementation(*this)->SetText(text);
  }
}

void StyledTextBuilder::AppendText(const Dali::String& text)
{
  if(*this)
  {
    GetImplementation(*this)->AppendText(text);
  }
}

bool StyledTextBuilder::SetSpan(const Span& span, uint32_t utf32StartIndex, uint32_t utf32EndIndex)
{
  return *this ? GetImplementation(*this)->SetSpan(span, utf32StartIndex, utf32EndIndex) : false;
}

bool StyledTextBuilder::RemoveSpan(const Span& span)
{
  return *this ? GetImplementation(*this)->RemoveSpan(span) : false;
}

bool StyledTextBuilder::RemoveSpanAt(uint32_t index)
{
  return *this ? GetImplementation(*this)->RemoveSpanAt(index) : false;
}

void StyledTextBuilder::ClearSpans()
{
  if(*this)
  {
    GetImplementation(*this)->ClearSpans();
  }
}

Dali::String StyledTextBuilder::GetText() const
{
  return *this ? GetImplementation(*this)->GetText() : Dali::String();
}

uint32_t StyledTextBuilder::GetUtf32Length() const
{
  return *this ? GetImplementation(*this)->GetUtf32Length() : 0u;
}

uint32_t StyledTextBuilder::GetSpanCount() const
{
  return *this ? GetImplementation(*this)->GetSpanCount() : 0u;
}

Span StyledTextBuilder::GetSpanAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetSpanAt(index) : Span();
}

uint32_t StyledTextBuilder::GetSpanStartIndexAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetSpanStartIndexAt(index) : 0u;
}

uint32_t StyledTextBuilder::GetSpanEndIndexAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetSpanEndIndexAt(index) : 0u;
}

StyledText StyledTextBuilder::Build() const
{
  if(!*this)
  {
    return StyledText();
  }

  auto* impl = GetImplementation(*this);
  return StyledText(new Internal::Text::StyledText(impl->GetText(), impl->GetAttachments(), impl->GetUtf32Length()));
}

StyledTextBuilder::StyledTextBuilder(BaseObject* impl)
: BaseHandle(impl)
{
}

} // namespace Text
} // namespace Ui
} // namespace Dali
