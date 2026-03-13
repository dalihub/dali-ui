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
#include <dali-ui-foundation/public-api/stack-layout-params.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layout/stack-layout-params-impl.h>

namespace Dali
{
namespace Ui
{

StackLayoutParams::StackLayoutParams()
{
}

StackLayoutParams StackLayoutParams::New()
{
  IntrusivePtr<Internal::StackLayoutParamsImpl> impl(new Internal::StackLayoutParamsImpl());
  return StackLayoutParams(impl.Get());
}

StackLayoutParams StackLayoutParams::New(const StackLayoutParams& other)
{
  IntrusivePtr<Internal::StackLayoutParamsImpl> impl(new Internal::StackLayoutParamsImpl(GetImpl(other)));
  return StackLayoutParams(impl.Get());
}

StackLayoutParams::StackLayoutParams(const StackLayoutParams& handle)
: LayoutParams(handle)
{
}

StackLayoutParams::~StackLayoutParams()
{
}

StackLayoutParams::StackLayoutParams(Internal::StackLayoutParamsImpl* implementation)
: LayoutParams(implementation)
{
}

StackLayoutParams StackLayoutParams::DownCast(BaseHandle handle)
{
  return StackLayoutParams(dynamic_cast<Internal::StackLayoutParamsImpl*>(handle.GetObjectPtr()));
}

LayoutParamsType StackLayoutParams::GetLayoutParamsType()
{
  return LayoutParamsType::STACK;
}

StackLayoutParams& StackLayoutParams::SetWeight(float weight)
{
  GetImpl(*this).SetWeight(weight);
  return *this;
}

float StackLayoutParams::GetWeight() const
{
  return GetImpl(*this).GetWeight();
}

} // namespace Ui
} // namespace Dali
