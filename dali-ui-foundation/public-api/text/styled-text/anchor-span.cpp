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
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/anchor-span-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

AnchorSpan AnchorSpan::New(const Text::AnchorAttributes& attributes)
{
  if(!attributes.Has(Text::AnchorAttributes::Attribute::HREF))
  {
    return AnchorSpan();
  }

  return AnchorSpan(new Internal::Text::AnchorSpan(attributes));
}

AnchorSpan AnchorSpan::DownCast(BaseHandle handle)
{
  return AnchorSpan(dynamic_cast<Internal::Text::AnchorSpan*>(handle.GetObjectPtr()));
}

Text::AnchorAttributes AnchorSpan::GetAnchorAttributes() const
{
  auto* impl = dynamic_cast<Internal::Text::AnchorSpan*>(GetObjectPtr());
  return impl ? impl->GetAnchorAttributes() : Text::AnchorAttributes();
}

AnchorSpan::AnchorSpan(BaseObject* impl)
: Span(impl)
{
}

} // namespace Text
} // namespace Ui
} // namespace Dali
