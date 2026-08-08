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
#include <dali-ui-foundation/public-api/types/inner-shadow.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

#include <algorithm>

#define DALI_ASSERT_VALID_INNER_SHADOW(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from InnerShadow object")

#define DALI_ASSERT_INNER_SHADOW_NOT_NONE(impl, message) \
  DALI_ASSERT_ALWAYS(!(impl)->mIsNone && message)

namespace Dali
{
namespace Ui
{
namespace
{
constexpr float DEFAULT_BLUR_RADIUS = 0.0f;
} // namespace

class InnerShadow::Impl
{
public:
  Impl()
  : mColor(0x000000, 0.5f),
    mBlurRadius(DEFAULT_BLUR_RADIUS),
    mInsets(),
    mIsNone(false)
  {
  }

  Impl(const Insets& insets, float blurRadius, const UiColor& color)
  : mColor(color),
    mBlurRadius(std::max(0.0f, blurRadius)),
    mInsets(insets),
    mIsNone(false)
  {
  }

  UiColor mColor;
  float   mBlurRadius;
  Insets  mInsets;
  bool    mIsNone;
};

InnerShadow::InnerShadow()
: mImpl(new Impl())
{
}

InnerShadow::InnerShadow(const Insets& insets, float blurRadius, const UiColor& color)
: mImpl(new Impl(insets, blurRadius, color))
{
}

InnerShadow::InnerShadow(const InnerShadow& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_INNER_SHADOW(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

InnerShadow::InnerShadow(InnerShadow&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

InnerShadow& InnerShadow::operator=(const InnerShadow& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_INNER_SHADOW(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

InnerShadow& InnerShadow::operator=(InnerShadow&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

InnerShadow::~InnerShadow()
{
  delete mImpl;
}

InnerShadow InnerShadow::None()
{
  InnerShadow shadow;
  shadow.mImpl->mIsNone = true;
  return shadow;
}

bool InnerShadow::operator==(const InnerShadow& rhs) const
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_VALID_INNER_SHADOW(rhs.mImpl);

  if(mImpl->mIsNone || rhs.mImpl->mIsNone)
  {
    return mImpl->mIsNone == rhs.mImpl->mIsNone;
  }

  return mImpl->mColor == rhs.mImpl->mColor &&
         Dali::Equals(mImpl->mBlurRadius, rhs.mImpl->mBlurRadius) &&
         mImpl->mInsets == rhs.mImpl->mInsets;
}

bool InnerShadow::operator!=(const InnerShadow& rhs) const
{
  return !(*this == rhs);
}

InnerShadow& InnerShadow::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_INNER_SHADOW_NOT_NONE(mImpl, "Cannot modify InnerShadow::None().");
  mImpl->mColor = color;
  return *this;
}

const UiColor& InnerShadow::GetColor() const
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_INNER_SHADOW_NOT_NONE(mImpl, "Cannot access InnerShadow::None() properties.");
  return mImpl->mColor;
}

InnerShadow& InnerShadow::SetBlurRadius(float blurRadius)
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_INNER_SHADOW_NOT_NONE(mImpl, "Cannot modify InnerShadow::None().");
  mImpl->mBlurRadius = std::max(0.0f, blurRadius);
  return *this;
}

float InnerShadow::GetBlurRadius() const
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_INNER_SHADOW_NOT_NONE(mImpl, "Cannot access InnerShadow::None() properties.");
  return mImpl->mBlurRadius;
}

InnerShadow& InnerShadow::SetInsets(const Insets& insets)
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_INNER_SHADOW_NOT_NONE(mImpl, "Cannot modify InnerShadow::None().");
  mImpl->mInsets = insets;
  return *this;
}

const Insets& InnerShadow::GetInsets() const
{
  DALI_ASSERT_VALID_INNER_SHADOW(mImpl);
  DALI_ASSERT_INNER_SHADOW_NOT_NONE(mImpl, "Cannot access InnerShadow::None() properties.");
  return mImpl->mInsets;
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_INNER_SHADOW_NOT_NONE
#undef DALI_ASSERT_VALID_INNER_SHADOW
