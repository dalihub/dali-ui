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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <algorithm>

#define DALI_ASSERT_VALID_SHADOW(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Shadow object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t DEFAULT_COLOR       = 0x000000;
constexpr float    DEFAULT_BLUR_RADIUS = 0.0f;
} // namespace

class Shadow::Impl
{
public:
  Impl()
  : mColor(DEFAULT_COLOR),
    mOffset(Vector2::ONE),
    mBlurRadius(DEFAULT_BLUR_RADIUS)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mOffset(rhs.mOffset),
    mBlurRadius(rhs.mBlurRadius)
  {
  }

  UiColor mColor;
  Vector2 mOffset;
  float   mBlurRadius;
};

Shadow::Shadow()
: mImpl(new Impl())
{
}

Shadow::Shadow(const Shadow& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_SHADOW(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Shadow::Shadow(Shadow&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Shadow& Shadow::operator=(const Shadow& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_SHADOW(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Shadow& Shadow::operator=(Shadow&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Shadow::~Shadow()
{
  delete mImpl;
}

Shadow& Shadow::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mColor = color;
  return *this;
}

const UiColor& Shadow::GetColor() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mColor;
}

Shadow& Shadow::SetOffset(const Vector2& offset)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mOffset = offset;
  return *this;
}

const Vector2& Shadow::GetOffset() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mOffset;
}

Shadow& Shadow::SetBlurRadius(float blurRadius)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mBlurRadius = std::max(0.0f, blurRadius);
  return *this;
}

float Shadow::GetBlurRadius() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mBlurRadius;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_SHADOW
