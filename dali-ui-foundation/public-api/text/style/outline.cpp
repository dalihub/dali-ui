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
#include <dali-ui-foundation/public-api/text/style/outline.h>
#include <algorithm>

#define DALI_ASSERT_VALID_OUTLINE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Outline object")

#define DALI_ASSERT_OUTLINE_NOT_NONE(impl, message) \
  DALI_ASSERT_ALWAYS(!(impl)->mIsNone && message)

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t DEFAULT_COLOR       = 0xFFFFFF;
constexpr float    DEFAULT_WIDTH       = 1.0f;
constexpr float    DEFAULT_BLUR_RADIUS = 0.0f;
} // namespace

class Outline::Impl
{
public:
  Impl()
  : mColor(DEFAULT_COLOR),
    mOffset(Vector2::ZERO),
    mWidth(DEFAULT_WIDTH),
    mBlurRadius(DEFAULT_BLUR_RADIUS),
    mIsNone(false)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mOffset(rhs.mOffset),
    mWidth(rhs.mWidth),
    mBlurRadius(rhs.mBlurRadius),
    mIsNone(rhs.mIsNone)
  {
  }

  UiColor mColor;
  Vector2 mOffset;
  float   mWidth;
  float   mBlurRadius;
  bool    mIsNone;
};

Outline::Outline()
: mImpl(new Impl())
{
}

Outline::Outline(const Outline& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_OUTLINE(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Outline::Outline(Outline&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Outline& Outline::operator=(const Outline& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_OUTLINE(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Outline& Outline::operator=(Outline&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Outline::~Outline()
{
  delete mImpl;
}

const Outline& Outline::None()
{
  static const Outline none = []()
  {
    Outline outline;
    outline.mImpl->mIsNone = true;
    return outline;
  }();

  return none;
}

bool Outline::operator==(const Outline& rhs) const
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_VALID_OUTLINE(rhs.mImpl);

  if(mImpl->mIsNone || rhs.mImpl->mIsNone)
  {
    return mImpl->mIsNone == rhs.mImpl->mIsNone;
  }

  return mImpl->mColor == rhs.mImpl->mColor &&
         mImpl->mOffset == rhs.mImpl->mOffset &&
         Dali::Equals(mImpl->mWidth, rhs.mImpl->mWidth) &&
         Dali::Equals(mImpl->mBlurRadius, rhs.mImpl->mBlurRadius);
}

bool Outline::operator!=(const Outline& rhs) const
{
  return !(*this == rhs);
}

void Outline::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot modify Text::Outline::None().");
  mImpl->mColor = color;
}

const UiColor& Outline::GetColor() const
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot access Text::Outline::None() properties.");
  return mImpl->mColor;
}

void Outline::SetOffset(const Vector2& offset)
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot modify Text::Outline::None().");
  mImpl->mOffset = offset;
}

const Vector2& Outline::GetOffset() const
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot access Text::Outline::None() properties.");
  return mImpl->mOffset;
}

void Outline::SetWidth(float width)
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot modify Text::Outline::None().");
  mImpl->mWidth = std::max(0.0f, width);
}

float Outline::GetWidth() const
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot access Text::Outline::None() properties.");
  return mImpl->mWidth;
}

void Outline::SetBlurRadius(float blurRadius)
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot modify Text::Outline::None().");
  mImpl->mBlurRadius = std::max(0.0f, blurRadius);
}

float Outline::GetBlurRadius() const
{
  DALI_ASSERT_VALID_OUTLINE(mImpl);
  DALI_ASSERT_OUTLINE_NOT_NONE(mImpl, "Cannot access Text::Outline::None() properties.");
  return mImpl->mBlurRadius;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_OUTLINE_NOT_NONE
#undef DALI_ASSERT_VALID_OUTLINE
