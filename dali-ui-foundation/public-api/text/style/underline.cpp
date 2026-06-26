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
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <algorithm>

#define DALI_ASSERT_VALID_UNDERLINE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Underline object")

#define DALI_ASSERT_UNDERLINE_NOT_NONE(impl, message) \
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
constexpr float    DEFAULT_THICKNESS   = 1.0f;
constexpr float    DEFAULT_DASH_LENGTH = 2.0f;
constexpr float    DEFAULT_DASH_GAP    = 1.0f;
} // namespace

class Underline::Impl
{
public:
  Impl()
  : mColor(DEFAULT_COLOR),
    mThickness(DEFAULT_THICKNESS),
    mType(Type::SOLID),
    mDashLength(DEFAULT_DASH_LENGTH),
    mDashGap(DEFAULT_DASH_GAP),
    mIsNone(false)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mThickness(rhs.mThickness),
    mType(rhs.mType),
    mDashLength(rhs.mDashLength),
    mDashGap(rhs.mDashGap),
    mIsNone(rhs.mIsNone)
  {
  }

  UiColor mColor;
  float   mThickness;
  Type    mType;
  float   mDashLength;
  float   mDashGap;
  bool    mIsNone;
};

Underline::Underline()
: mImpl(new Impl())
{
}

Underline::Underline(const Underline& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_UNDERLINE(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Underline::Underline(Underline&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Underline& Underline::operator=(const Underline& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_UNDERLINE(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Underline& Underline::operator=(Underline&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Underline::~Underline()
{
  delete mImpl;
}

const Underline& Underline::None()
{
  static const Underline none = []()
  {
    Underline underline;
    underline.mImpl->mIsNone = true;
    return underline;
  }();

  return none;
}

bool Underline::operator==(const Underline& rhs) const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_VALID_UNDERLINE(rhs.mImpl);

  if(mImpl->mIsNone || rhs.mImpl->mIsNone)
  {
    return mImpl->mIsNone == rhs.mImpl->mIsNone;
  }

  return mImpl->mColor == rhs.mImpl->mColor &&
         Dali::Equals(mImpl->mThickness, rhs.mImpl->mThickness) &&
         mImpl->mType == rhs.mImpl->mType &&
         Dali::Equals(mImpl->mDashLength, rhs.mImpl->mDashLength) &&
         Dali::Equals(mImpl->mDashGap, rhs.mImpl->mDashGap);
}

bool Underline::operator!=(const Underline& rhs) const
{
  return !(*this == rhs);
}

void Underline::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot modify Text::Underline::None().");
  mImpl->mColor = color;
}

const UiColor& Underline::GetColor() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot access Text::Underline::None() properties.");
  return mImpl->mColor;
}

void Underline::SetThickness(float thickness)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot modify Text::Underline::None().");
  mImpl->mThickness = std::max(0.0f, thickness);
}

float Underline::GetThickness() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot access Text::Underline::None() properties.");
  return mImpl->mThickness;
}

void Underline::SetType(Type type)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot modify Text::Underline::None().");
  mImpl->mType = type;
}

Underline::Type Underline::GetType() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot access Text::Underline::None() properties.");
  return mImpl->mType;
}

void Underline::SetDashLength(float length)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot modify Text::Underline::None().");
  mImpl->mDashLength = std::max(0.0f, length);
}

float Underline::GetDashLength() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot access Text::Underline::None() properties.");
  return mImpl->mDashLength;
}

void Underline::SetDashGap(float gap)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot modify Text::Underline::None().");
  mImpl->mDashGap = std::max(0.0f, gap);
}

float Underline::GetDashGap() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  DALI_ASSERT_UNDERLINE_NOT_NONE(mImpl, "Cannot access Text::Underline::None() properties.");
  return mImpl->mDashGap;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_UNDERLINE_NOT_NONE
#undef DALI_ASSERT_VALID_UNDERLINE
