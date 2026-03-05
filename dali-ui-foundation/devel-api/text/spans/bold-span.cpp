/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/devel-api/text/spans/bold-span.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/spannable/spans/bold-span-impl.h>

namespace Dali
{
namespace UI
{
namespace Text
{

BoldSpan BoldSpan::New()
{
  return Internal::BoldSpan::New();
}

BoldSpan::BoldSpan() = default;

BoldSpan::BoldSpan(const BoldSpan& rhs) = default;

BoldSpan::BoldSpan(BoldSpan&& rhs) = default;

BoldSpan& BoldSpan::operator=(const BoldSpan& rhs) = default;

BoldSpan& BoldSpan::operator=(BoldSpan&& rhs) = default;

BoldSpan::~BoldSpan() = default;

BoldSpan::BoldSpan(Internal::BoldSpan* internal)
  : BaseSpan(internal)
{
}

BoldSpan BoldSpan::DownCast(BaseHandle handle)
{
  return BoldSpan(dynamic_cast<Dali::UI::Text::Internal::BoldSpan*>(handle.GetObjectPtr()));
}

} // namespace Text

} // namespace UI

} // namespace Dali
