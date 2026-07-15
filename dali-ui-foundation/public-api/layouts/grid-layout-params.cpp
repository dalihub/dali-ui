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
#include <dali/public-api/common/dali-common.h>
#include <algorithm>

#define DALI_ASSERT_VALID_LAYOUT_PARAMS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use moved-from GridLayoutParams")

namespace Dali
{
namespace Ui
{

class GridLayoutParams::Impl
{
public:
  Impl()
  : mRow(0u),
    mColumn(0u),
    mRowSpan(1u),
    mColumnSpan(1u),
    mHorizontalAlignment(LayoutAlignment::FILL),
    mVerticalAlignment(LayoutAlignment::FILL)
  {
  }

  uint32_t        mRow;
  uint32_t        mColumn;
  uint32_t        mRowSpan;
  uint32_t        mColumnSpan;
  LayoutAlignment mHorizontalAlignment;
  LayoutAlignment mVerticalAlignment;
};

GridLayoutParams::GridLayoutParams()
: mImpl(new Impl())
{
}

GridLayoutParams::GridLayoutParams(const GridLayoutParams& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

GridLayoutParams::GridLayoutParams(GridLayoutParams&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

GridLayoutParams& GridLayoutParams::operator=(const GridLayoutParams& other)
{
  if(this != &other)
  {
    DALI_ASSERT_VALID_LAYOUT_PARAMS(other.mImpl);
    Impl* newImpl = new Impl(*other.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

GridLayoutParams& GridLayoutParams::operator=(GridLayoutParams&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

GridLayoutParams::~GridLayoutParams()
{
  delete mImpl;
}

GridLayoutParams GridLayoutParams::New()
{
  return GridLayoutParams();
}

GridLayoutParams& GridLayoutParams::SetRow(uint32_t row)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mRow = row;
  return *this;
}

uint32_t GridLayoutParams::GetRow() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mRow;
}

GridLayoutParams& GridLayoutParams::SetColumn(uint32_t column)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mColumn = column;
  return *this;
}

uint32_t GridLayoutParams::GetColumn() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mColumn;
}

GridLayoutParams& GridLayoutParams::SetRowSpan(uint32_t span)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mRowSpan = std::max(1u, span);
  return *this;
}

uint32_t GridLayoutParams::GetRowSpan() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mRowSpan;
}

GridLayoutParams& GridLayoutParams::SetColumnSpan(uint32_t span)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mColumnSpan = std::max(1u, span);
  return *this;
}

uint32_t GridLayoutParams::GetColumnSpan() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mColumnSpan;
}

GridLayoutParams& GridLayoutParams::SetHorizontalAlignment(LayoutAlignment alignment)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mHorizontalAlignment = alignment;
  return *this;
}

LayoutAlignment GridLayoutParams::GetHorizontalAlignment() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mHorizontalAlignment;
}

GridLayoutParams& GridLayoutParams::SetVerticalAlignment(LayoutAlignment alignment)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mVerticalAlignment = alignment;
  return *this;
}

LayoutAlignment GridLayoutParams::GetVerticalAlignment() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mVerticalAlignment;
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_LAYOUT_PARAMS
