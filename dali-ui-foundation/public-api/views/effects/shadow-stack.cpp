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
#include <new>
#include <type_traits>
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
  using ShadowStorage = std::aligned_storage<sizeof(Shadow), alignof(Shadow)>::type;

  Impl() = default;

  Impl(const Impl& rhs)
  : mAdditionalShadows(rhs.mAdditionalShadows)
  {
    if(rhs.mHasFirstShadow)
    {
      ConstructFirstShadow(rhs.GetFirstShadow());
    }
  }

  Impl& operator=(const Impl& rhs)
  {
    if(this != &rhs)
    {
      Clear();
      if(rhs.mHasFirstShadow)
      {
        ConstructFirstShadow(rhs.GetFirstShadow());
      }
      mAdditionalShadows = rhs.mAdditionalShadows;
    }
    return *this;
  }

  ~Impl()
  {
    Clear();
  }

  void Add(const Shadow& shadow)
  {
    if(!mHasFirstShadow)
    {
      ConstructFirstShadow(shadow);
      return;
    }

    mAdditionalShadows.push_back(shadow);
  }

  void Clear()
  {
    mAdditionalShadows.clear();

    if(mHasFirstShadow)
    {
      GetFirstShadow().~Shadow();
      mHasFirstShadow = false;
    }
  }

  uint32_t GetShadowCount() const
  {
    return mHasFirstShadow ? static_cast<uint32_t>(mAdditionalShadows.size() + 1u) : 0u;
  }

  const Shadow& GetShadow(uint32_t index) const
  {
    DALI_ASSERT_ALWAYS(index < GetShadowCount() && "ShadowStack index is out of bounds");
    return index == 0u ? GetFirstShadow() : mAdditionalShadows[index - 1u];
  }

private:
  void ConstructFirstShadow(const Shadow& shadow)
  {
    new(&mFirstShadowStorage) Shadow(shadow);
    mHasFirstShadow = true;
  }

  Shadow& GetFirstShadow()
  {
    return *reinterpret_cast<Shadow*>(&mFirstShadowStorage);
  }

  const Shadow& GetFirstShadow() const
  {
    return *reinterpret_cast<const Shadow*>(&mFirstShadowStorage);
  }

  ShadowStorage       mFirstShadowStorage;
  std::vector<Shadow> mAdditionalShadows;
  bool                mHasFirstShadow{false};
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
  mImpl->Add(shadow);
  return *this;
}

void ShadowStack::Clear()
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  mImpl->Clear();
}

uint32_t ShadowStack::GetShadowCount() const
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  return mImpl->GetShadowCount();
}

const Shadow& ShadowStack::GetShadowAt(uint32_t index) const
{
  DALI_ASSERT_VALID_SHADOW_STACK(mImpl);
  return mImpl->GetShadow(index);
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_SHADOW_STACK
