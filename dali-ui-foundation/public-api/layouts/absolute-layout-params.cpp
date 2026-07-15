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
#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_LAYOUT_PARAMS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use moved-from AbsoluteLayoutParams")

namespace Dali
{
namespace Ui
{

class AbsoluteLayoutParams::Impl
{
public:
  Impl()
  : mBounds(0.0f, 0.0f, -1.0f, -1.0f),
    mFlags(AbsoluteLayoutFlags::NONE)
  {
  }

  LayoutRect          mBounds;
  AbsoluteLayoutFlags mFlags;
};

AbsoluteLayoutParams::AbsoluteLayoutParams()
: mImpl(new Impl())
{
}

AbsoluteLayoutParams::AbsoluteLayoutParams(const AbsoluteLayoutParams& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

AbsoluteLayoutParams::AbsoluteLayoutParams(AbsoluteLayoutParams&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

AbsoluteLayoutParams& AbsoluteLayoutParams::operator=(const AbsoluteLayoutParams& other)
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

AbsoluteLayoutParams& AbsoluteLayoutParams::operator=(AbsoluteLayoutParams&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

AbsoluteLayoutParams::~AbsoluteLayoutParams()
{
  delete mImpl;
}

AbsoluteLayoutParams AbsoluteLayoutParams::New()
{
  return AbsoluteLayoutParams();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetBounds(const LayoutRect& bounds)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mBounds = bounds;
  return *this;
}

LayoutRect AbsoluteLayoutParams::GetBounds() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mBounds;
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetX(float x)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mBounds.SetX(x);
  return *this;
}

float AbsoluteLayoutParams::GetX() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mBounds.GetX();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetY(float y)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mBounds.SetY(y);
  return *this;
}

float AbsoluteLayoutParams::GetY() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mBounds.GetY();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetWidth(float width)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mBounds.SetWidth(width);
  return *this;
}

float AbsoluteLayoutParams::GetWidth() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mBounds.GetWidth();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetHeight(float height)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mBounds.SetHeight(height);
  return *this;
}

float AbsoluteLayoutParams::GetHeight() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mBounds.GetHeight();
}

AbsoluteLayoutParams& AbsoluteLayoutParams::SetFlags(AbsoluteLayoutFlags flags)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mFlags = flags;
  return *this;
}

AbsoluteLayoutFlags AbsoluteLayoutParams::GetFlags() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mFlags;
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_LAYOUT_PARAMS
