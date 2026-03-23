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
#include <dali-ui-foundation/public-api/layouts/grid-layout-params.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/grid-layout-params-impl.h>

namespace Dali
{
namespace Ui
{

GridLayoutParams::GridLayoutParams()
{
}

GridLayoutParams GridLayoutParams::New()
{
  IntrusivePtr<Internal::GridLayoutParamsImpl> impl(new Internal::GridLayoutParamsImpl());
  return GridLayoutParams(impl.Get());
}

GridLayoutParams GridLayoutParams::New(const GridLayoutParams& other)
{
  IntrusivePtr<Internal::GridLayoutParamsImpl> impl(new Internal::GridLayoutParamsImpl(GetImpl(other)));
  return GridLayoutParams(impl.Get());
}

GridLayoutParams::GridLayoutParams(const GridLayoutParams& handle)
: LayoutParams(handle)
{
}

GridLayoutParams::~GridLayoutParams()
{
}

GridLayoutParams::GridLayoutParams(Internal::GridLayoutParamsImpl* implementation)
: LayoutParams(implementation)
{
}

GridLayoutParams GridLayoutParams::DownCast(BaseHandle handle)
{
  return GridLayoutParams(dynamic_cast<Internal::GridLayoutParamsImpl*>(handle.GetObjectPtr()));
}

LayoutParamsType GridLayoutParams::GetLayoutParamsType()
{
  return LayoutParamsType::GRID;
}

GridLayoutParams& GridLayoutParams::SetRow(uint32_t row)
{
  GetImpl(*this).SetRow(row);
  return *this;
}

uint32_t GridLayoutParams::GetRow() const
{
  return GetImpl(*this).GetRow();
}

GridLayoutParams& GridLayoutParams::SetColumn(uint32_t column)
{
  GetImpl(*this).SetColumn(column);
  return *this;
}

uint32_t GridLayoutParams::GetColumn() const
{
  return GetImpl(*this).GetColumn();
}

GridLayoutParams& GridLayoutParams::SetRowSpan(uint32_t span)
{
  GetImpl(*this).SetRowSpan(span);
  return *this;
}

uint32_t GridLayoutParams::GetRowSpan() const
{
  return GetImpl(*this).GetRowSpan();
}

GridLayoutParams& GridLayoutParams::SetColumnSpan(uint32_t span)
{
  GetImpl(*this).SetColumnSpan(span);
  return *this;
}

uint32_t GridLayoutParams::GetColumnSpan() const
{
  return GetImpl(*this).GetColumnSpan();
}

GridLayoutParams& GridLayoutParams::SetHorizontalAlignment(LayoutAlignment alignment)
{
  GetImpl(*this).SetHorizontalAlignment(alignment);
  return *this;
}

LayoutAlignment GridLayoutParams::GetHorizontalAlignment() const
{
  return GetImpl(*this).GetHorizontalAlignment();
}

GridLayoutParams& GridLayoutParams::SetVerticalAlignment(LayoutAlignment alignment)
{
  GetImpl(*this).SetVerticalAlignment(alignment);
  return *this;
}

LayoutAlignment GridLayoutParams::GetVerticalAlignment() const
{
  return GetImpl(*this).GetVerticalAlignment();
}

} // namespace Ui
} // namespace Dali
