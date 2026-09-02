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
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-impl.h>

namespace Dali
{
namespace Ui
{
namespace Text
{

GradientSpan GradientSpan::New(const Gradient::Base& gradient)
{
  return New(gradient, BoundsMode::SPAN_BOUND);
}

GradientSpan GradientSpan::New(const Gradient::Base& gradient, BoundsMode boundsMode)
{
  return GradientSpan(new Internal::Text::GradientSpan(gradient, boundsMode));
}

GradientSpan GradientSpan::DownCast(BaseHandle handle)
{
  return GradientSpan(dynamic_cast<Internal::Text::GradientSpan*>(handle.GetObjectPtr()));
}

Gradient::Base GradientSpan::GetGradient() const
{
  auto* impl = dynamic_cast<Internal::Text::GradientSpan*>(GetObjectPtr());
  return impl ? impl->GetGradient() : Gradient::Base();
}

GradientSpan::BoundsMode GradientSpan::GetBoundsMode() const
{
  auto* impl = dynamic_cast<Internal::Text::GradientSpan*>(GetObjectPtr());
  return impl ? impl->GetBoundsMode() : BoundsMode::SPAN_BOUND;
}

GradientSpan::GradientSpan(BaseObject* impl)
: Span(impl)
{
}

} // namespace Text
} // namespace Ui
} // namespace Dali
