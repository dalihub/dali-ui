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
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#define DALI_ASSERT_VALID_LAYOUT_PARAMS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use moved-from StackLayoutParams")

namespace Dali
{
namespace Ui
{

class StackLayoutParams::Impl
{
public:
  Impl()
  : mWeight(0.0f),
    mAlignment(LayoutAlignment::START)
  {
  }

  float           mWeight;
  LayoutAlignment mAlignment;
};

StackLayoutParams::StackLayoutParams()
: mImpl(new Impl())
{
}

StackLayoutParams::StackLayoutParams(const StackLayoutParams& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

StackLayoutParams::StackLayoutParams(StackLayoutParams&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

StackLayoutParams& StackLayoutParams::operator=(const StackLayoutParams& other)
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

StackLayoutParams& StackLayoutParams::operator=(StackLayoutParams&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

StackLayoutParams::~StackLayoutParams()
{
  delete mImpl;
}

StackLayoutParams StackLayoutParams::New()
{
  return StackLayoutParams();
}

StackLayoutParams& StackLayoutParams::SetWeight(float weight)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mWeight = weight;
  return *this;
}

float StackLayoutParams::GetWeight() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mWeight;
}

StackLayoutParams& StackLayoutParams::SetAlignment(LayoutAlignment alignment)
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  mImpl->mAlignment = alignment;
  return *this;
}

LayoutAlignment StackLayoutParams::GetAlignment() const
{
  DALI_ASSERT_VALID_LAYOUT_PARAMS(mImpl);
  return mImpl->mAlignment;
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_LAYOUT_PARAMS
