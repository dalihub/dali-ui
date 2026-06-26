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
#include <dali/public-api/math/math-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/text/style/shadow.h>
#include <algorithm>

#define DALI_ASSERT_VALID_SHADOW(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Shadow object")

#define DALI_ASSERT_SHADOW_NOT_NONE(impl, message) \
  DALI_ASSERT_ALWAYS(!(impl)->mIsNone && message)

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
    mBlurRadius(DEFAULT_BLUR_RADIUS),
    mIsNone(false)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mOffset(rhs.mOffset),
    mBlurRadius(rhs.mBlurRadius),
    mIsNone(rhs.mIsNone)
  {
  }

  UiColor mColor;
  Vector2 mOffset;
  float   mBlurRadius;
  bool    mIsNone;
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

const Shadow& Shadow::None()
{
  static const Shadow none = []()
  {
    Shadow shadow;
    shadow.mImpl->mIsNone = true;
    return shadow;
  }();

  return none;
}

bool Shadow::operator==(const Shadow& rhs) const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_VALID_SHADOW(rhs.mImpl);

  if(mImpl->mIsNone || rhs.mImpl->mIsNone)
  {
    return mImpl->mIsNone == rhs.mImpl->mIsNone;
  }

  return mImpl->mColor == rhs.mImpl->mColor &&
         mImpl->mOffset == rhs.mImpl->mOffset &&
         Dali::Equals(mImpl->mBlurRadius, rhs.mImpl->mBlurRadius);
}

bool Shadow::operator!=(const Shadow& rhs) const
{
  return !(*this == rhs);
}

void Shadow::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_SHADOW_NOT_NONE(mImpl, "Cannot modify Text::Shadow::None().");
  mImpl->mColor = color;
}

const UiColor& Shadow::GetColor() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_SHADOW_NOT_NONE(mImpl, "Cannot access Text::Shadow::None() properties.");
  return mImpl->mColor;
}

void Shadow::SetOffset(const Vector2& offset)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_SHADOW_NOT_NONE(mImpl, "Cannot modify Text::Shadow::None().");
  mImpl->mOffset = offset;
}

const Vector2& Shadow::GetOffset() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_SHADOW_NOT_NONE(mImpl, "Cannot access Text::Shadow::None() properties.");
  return mImpl->mOffset;
}

void Shadow::SetBlurRadius(float blurRadius)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_SHADOW_NOT_NONE(mImpl, "Cannot modify Text::Shadow::None().");
  mImpl->mBlurRadius = std::max(0.0f, blurRadius);
}

float Shadow::GetBlurRadius() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  DALI_ASSERT_SHADOW_NOT_NONE(mImpl, "Cannot access Text::Shadow::None() properties.");
  return mImpl->mBlurRadius;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_SHADOW_NOT_NONE
#undef DALI_ASSERT_VALID_SHADOW
