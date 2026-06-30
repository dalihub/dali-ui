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
#include <dali-ui-foundation/public-api/views/effects/shadow-stack.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <vector>

#define DALI_ASSERT_VALID_SHADOW_STACK(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from ShadowStack object")

namespace Dali
{
namespace Ui
{

class ShadowStack::Impl
{
public:
  std::vector<Shadow> mShadows;
};

ShadowStack::ShadowStack()
: mImpl(new Impl())
{
}

ShadowStack::ShadowStack(const ShadowStack& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_SHADOW_STACK(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

ShadowStack::ShadowStack(ShadowStack&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  // Match text style value objects: moving transfers ownership, and using the moved-from object asserts.
  rhs.mImpl = nullptr;
}

ShadowStack& ShadowStack::operator=(const ShadowStack& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_SHADOW_STACK(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

ShadowStack& ShadowStack::operator=(ShadowStack&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl = rhs.mImpl;
    // Match text style value objects: moving transfers ownership, and using the moved-from object asserts.
    rhs.mImpl = nullptr;
  }
  return *this;
}

ShadowStack::~ShadowStack()
{
  delete mImpl;
}

ShadowStack& ShadowStack::Add(const Shadow& shadow)
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  mImpl->mShadows.push_back(shadow);
  return *this;
}

void ShadowStack::Clear()
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  mImpl->mShadows.clear();
}

uint32_t ShadowStack::GetInternalShadowCount() const
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  return static_cast<uint32_t>(mImpl->mShadows.size());
}

const Shadow& ShadowStack::GetInternalShadow(uint32_t index) const
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  DALI_ASSERT_ALWAYS(index < mImpl->mShadows.size() && "ShadowStack index is out of bounds");
  return mImpl->mShadows[index];
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_SHADOW_STACK
