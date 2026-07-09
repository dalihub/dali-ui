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
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/annotation-span-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

AnnotationSpan AnnotationSpan::New(const Dali::String& key, const Dali::String& value)
{
  return AnnotationSpan(new Internal::Text::AnnotationSpan(key, value));
}

AnnotationSpan AnnotationSpan::DownCast(BaseHandle handle)
{
  return AnnotationSpan(dynamic_cast<Internal::Text::AnnotationSpan*>(handle.GetObjectPtr()));
}

Dali::String AnnotationSpan::GetKey() const
{
  auto* impl = dynamic_cast<Internal::Text::AnnotationSpan*>(GetObjectPtr());
  return impl ? impl->GetKey() : Dali::String();
}

Dali::String AnnotationSpan::GetValue() const
{
  auto* impl = dynamic_cast<Internal::Text::AnnotationSpan*>(GetObjectPtr());
  return impl ? impl->GetValue() : Dali::String();
}

AnnotationSpan::AnnotationSpan(BaseObject* impl)
: Span(impl)
{
}

} // namespace Text
} // namespace Ui
} // namespace Dali
