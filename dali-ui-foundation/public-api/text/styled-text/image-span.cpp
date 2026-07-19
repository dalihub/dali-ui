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

#include <dali-ui-foundation/internal/text/styled-text/image-span-impl.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>

namespace Dali
{
namespace Ui
{
namespace Text
{
ImageSpan ImageSpan::New(const ImageAttributes& attributes)
{
  return ImageSpan(new Internal::Text::ImageSpan(attributes));
}

ImageSpan ImageSpan::DownCast(BaseHandle handle)
{
  return ImageSpan(dynamic_cast<Internal::Text::ImageSpan*>(handle.GetObjectPtr()));
}

ImageAttributes ImageSpan::GetImageAttributes() const
{
  auto* impl = dynamic_cast<Internal::Text::ImageSpan*>(GetObjectPtr());
  return impl ? impl->GetImageAttributes() : ImageAttributes();
}

ImageSpan::ImageSpan(BaseObject* impl)
: ReplacementSpan(impl)
{
}
} // namespace Text
} // namespace Ui
} // namespace Dali
