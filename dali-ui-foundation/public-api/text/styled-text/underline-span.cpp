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
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/underline-span-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

UnderlineSpan UnderlineSpan::New(const Text::Underline& underline)
{
  return UnderlineSpan(new Internal::Text::UnderlineSpan(underline));
}

UnderlineSpan UnderlineSpan::DownCast(BaseHandle handle)
{
  return UnderlineSpan(dynamic_cast<Internal::Text::UnderlineSpan*>(handle.GetObjectPtr()));
}

Text::Underline UnderlineSpan::GetUnderline() const
{
  auto* impl = dynamic_cast<Internal::Text::UnderlineSpan*>(GetObjectPtr());
  return impl ? impl->GetUnderline() : Text::Underline();
}

UnderlineSpan::UnderlineSpan(BaseObject* impl)
: Span(impl)
{
}

} // namespace Text
} // namespace Ui
} // namespace Dali
