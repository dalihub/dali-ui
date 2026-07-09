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
#include <dali-ui-foundation/internal/text/styled-text/annotation-span-impl.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
namespace Text
{

AnnotationSpan::AnnotationSpan(const Dali::String& key, const Dali::String& value)
: Span(),
  mKey(key),
  mValue(value)
{
}

AnnotationSpan::~AnnotationSpan() = default;

const Dali::String& AnnotationSpan::GetKey() const
{
  return mKey;
}

const Dali::String& AnnotationSpan::GetValue() const
{
  return mValue;
}

} // namespace Text
} // namespace Internal
} // namespace Ui
} // namespace Dali
