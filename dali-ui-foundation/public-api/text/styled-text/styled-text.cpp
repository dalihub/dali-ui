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
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>

// EXTERNAL INCLUDES
#include <utility>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/markup-to-styled-text.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

namespace
{

Internal::Text::StyledText* GetImplementation(const StyledText& styledText)
{
  return static_cast<Internal::Text::StyledText*>(styledText.GetObjectPtr());
}

} // unnamed namespace

StyledText StyledText::New(const Dali::String& text)
{
  return StyledText(new Internal::Text::StyledText(text));
}

StyledText StyledText::FromMarkup(const Dali::String& markup)
{
  Internal::Text::StyledTextMarkupResult result = Internal::Text::ParseStyledTextMarkup(markup, nullptr);
  return StyledText(new Internal::Text::StyledText(result.text, std::move(result.attachments), result.utf32Length));
}

Dali::String StyledText::GetText() const
{
  return *this ? GetImplementation(*this)->GetText() : Dali::String();
}

uint32_t StyledText::GetUtf32Length() const
{
  return *this ? GetImplementation(*this)->GetUtf32Length() : 0u;
}

uint32_t StyledText::GetSpanCount() const
{
  return *this ? GetImplementation(*this)->GetSpanCount() : 0u;
}

Span StyledText::GetSpanAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetSpanAt(index) : Span();
}

uint32_t StyledText::GetSpanStartIndexAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetSpanStartIndexAt(index) : 0u;
}

uint32_t StyledText::GetSpanEndIndexAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetSpanEndIndexAt(index) : 0u;
}

uint32_t StyledText::GetAnnotationCount() const
{
  return *this ? GetImplementation(*this)->GetAnnotationCount() : 0u;
}

AnnotationSpan StyledText::GetAnnotationAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetAnnotationAt(index) : AnnotationSpan();
}

uint32_t StyledText::GetAnnotationStartIndexAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetAnnotationStartIndexAt(index) : 0u;
}

uint32_t StyledText::GetAnnotationEndIndexAt(uint32_t index) const
{
  return *this ? GetImplementation(*this)->GetAnnotationEndIndexAt(index) : 0u;
}

bool StyledText::IsEmpty() const
{
  return *this ? GetImplementation(*this)->IsEmpty() : true;
}

StyledText::StyledText(BaseObject* impl)
: BaseHandle(impl)
{
}

} // namespace Text
} // namespace Ui
} // namespace Dali
