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
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/absolute-layout-params-impl.h>

namespace Dali
{
namespace Ui
{

AbsoluteLayoutParams::AbsoluteLayoutParams()
{
}

AbsoluteLayoutParams::AbsoluteLayoutParams(const AbsoluteLayoutParams& handle)
: LayoutParams(handle)
{
}

AbsoluteLayoutParams::~AbsoluteLayoutParams()
{
}

AbsoluteLayoutParams::AbsoluteLayoutParams(Internal::AbsoluteLayoutParamsImpl* implementation)
: LayoutParams(implementation)
{
}

AbsoluteLayoutParams AbsoluteLayoutParams::New()
{
  IntrusivePtr<Internal::AbsoluteLayoutParamsImpl> impl(new Internal::AbsoluteLayoutParamsImpl());
  return AbsoluteLayoutParams(impl.Get());
}

AbsoluteLayoutParams AbsoluteLayoutParams::New(const AbsoluteLayoutParams& other)
{
  IntrusivePtr<Internal::AbsoluteLayoutParamsImpl> impl(new Internal::AbsoluteLayoutParamsImpl(GetImpl(other)));
  return AbsoluteLayoutParams(impl.Get());
}

AbsoluteLayoutParams AbsoluteLayoutParams::DownCast(BaseHandle handle)
{
  return AbsoluteLayoutParams(dynamic_cast<Internal::AbsoluteLayoutParamsImpl*>(handle.GetObjectPtr()));
}

LayoutParamsType AbsoluteLayoutParams::GetLayoutParamsType()
{
  return LayoutParamsType::ABSOLUTE;
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetBounds(const LayoutRect& bounds)
{
  GetImpl(*this).SetBounds(bounds);
  return *this;
}

LayoutRect AbsoluteLayoutParams::GetBounds() const
{
  return GetImpl(*this).GetBounds();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetX(float x)
{
  GetImpl(*this).SetX(x);
  return *this;
}

float AbsoluteLayoutParams::GetX() const
{
  return GetImpl(*this).GetBounds().GetX();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetY(float y)
{
  GetImpl(*this).SetY(y);
  return *this;
}

float AbsoluteLayoutParams::GetY() const
{
  return GetImpl(*this).GetBounds().GetY();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetWidth(float width)
{
  GetImpl(*this).SetWidth(width);
  return *this;
}

float AbsoluteLayoutParams::GetWidth() const
{
  return GetImpl(*this).GetBounds().GetWidth();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetHeight(float height)
{
  GetImpl(*this).SetHeight(height);
  return *this;
}

float AbsoluteLayoutParams::GetHeight() const
{
  return GetImpl(*this).GetBounds().GetHeight();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetFlags(AbsoluteLayoutFlags flags)
{
  GetImpl(*this).SetFlags(flags);
  return *this;
}

AbsoluteLayoutFlags AbsoluteLayoutParams::GetFlags() const
{
  return GetImpl(*this).GetFlags();
}

} // namespace Ui
} // namespace Dali
