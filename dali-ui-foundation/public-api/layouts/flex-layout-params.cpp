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
#include <dali-ui-foundation/public-api/layouts/flex-layout-params.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/flex-layout-params-impl.h>

namespace Dali
{
namespace Ui
{

FlexLayoutParams::FlexLayoutParams()
{
}

FlexLayoutParams FlexLayoutParams::New()
{
  IntrusivePtr<Internal::FlexLayoutParamsImpl> impl(new Internal::FlexLayoutParamsImpl());
  return FlexLayoutParams(impl.Get());
}

FlexLayoutParams FlexLayoutParams::New(const FlexLayoutParams& other)
{
  IntrusivePtr<Internal::FlexLayoutParamsImpl> impl(new Internal::FlexLayoutParamsImpl(GetImpl(other)));
  return FlexLayoutParams(impl.Get());
}

FlexLayoutParams::FlexLayoutParams(const FlexLayoutParams& handle)
: LayoutParams(handle)
{
}

FlexLayoutParams::~FlexLayoutParams()
{
}

FlexLayoutParams::FlexLayoutParams(Internal::FlexLayoutParamsImpl* implementation)
: LayoutParams(implementation)
{
}

FlexLayoutParams FlexLayoutParams::DownCast(BaseHandle handle)
{
  return FlexLayoutParams(dynamic_cast<Internal::FlexLayoutParamsImpl*>(handle.GetObjectPtr()));
}

LayoutParamsType FlexLayoutParams::GetLayoutParamsType()
{
  return LayoutParamsType::FLEX;
}

FlexLayoutParams& FlexLayoutParams::SetFlexGrow(float grow)
{
  GetImpl(*this).SetFlexGrow(grow);
  return *this;
}

float FlexLayoutParams::GetFlexGrow() const
{
  return GetImpl(*this).GetFlexGrow();
}

FlexLayoutParams& FlexLayoutParams::SetFlexShrink(float shrink)
{
  GetImpl(*this).SetFlexShrink(shrink);
  return *this;
}

float FlexLayoutParams::GetFlexShrink() const
{
  return GetImpl(*this).GetFlexShrink();
}

FlexLayoutParams& FlexLayoutParams::SetFlexBasis(float basis)
{
  GetImpl(*this).SetFlexBasis(basis);
  return *this;
}

float FlexLayoutParams::GetFlexBasis() const
{
  return GetImpl(*this).GetFlexBasis();
}

FlexLayoutParams& FlexLayoutParams::SetAlignSelf(FlexAlign align)
{
  GetImpl(*this).SetAlignSelf(align);
  return *this;
}

FlexAlign FlexLayoutParams::GetAlignSelf() const
{
  return GetImpl(*this).GetAlignSelf();
}

} // namespace Ui
} // namespace Dali
