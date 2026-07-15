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
#include <dali/public-api/common/dali-common.h>
#include <algorithm>

#define DALI_ASSERT_VALID_LAYOUT_PARAMS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use moved-from FlexLayoutParams")

namespace Dali
{
namespace Ui
{

class FlexLayoutParams::Impl
{
public:
  Impl()
  : mFlexGrow(0.0f),
    mFlexShrink(1.0f),
    mFlexBasis(WRAP_CONTENT),
    mAlignSelf(FlexAlign::AUTO)
  {
  }

  float     mFlexGrow;
  float     mFlexShrink;
  float     mFlexBasis;
  FlexAlign mAlignSelf;
};

FlexLayoutParams::FlexLayoutParams()
: mImpl(new Impl())
{
}

FlexLayoutParams::FlexLayoutParams(const FlexLayoutParams& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

FlexLayoutParams::FlexLayoutParams(FlexLayoutParams&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

FlexLayoutParams& FlexLayoutParams::operator=(const FlexLayoutParams& other)
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

FlexLayoutParams& FlexLayoutParams::operator=(FlexLayoutParams&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

FlexLayoutParams::~FlexLayoutParams()
{
  delete mImpl;
}

FlexLayoutParams FlexLayoutParams::New()
{
  return FlexLayoutParams();
}

FlexLayoutParams& FlexLayoutParams::SetFlexGrow(float grow)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mFlexGrow = std::max(0.0f, grow);
  return *this;
}

float FlexLayoutParams::GetFlexGrow() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mFlexGrow;
}

FlexLayoutParams& FlexLayoutParams::SetFlexShrink(float shrink)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mFlexShrink = std::max(0.0f, shrink);
  return *this;
}

float FlexLayoutParams::GetFlexShrink() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mFlexShrink;
}

FlexLayoutParams& FlexLayoutParams::SetFlexBasis(float basis)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mFlexBasis = basis;
  return *this;
}

float FlexLayoutParams::GetFlexBasis() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mFlexBasis;
}

FlexLayoutParams& FlexLayoutParams::SetAlignSelf(FlexAlign align)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mAlignSelf = align;
  return *this;
}

FlexAlign FlexLayoutParams::GetAlignSelf() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mAlignSelf;
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_LAYOUT_PARAMS
