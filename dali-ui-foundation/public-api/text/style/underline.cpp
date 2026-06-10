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
#include <dali-ui-foundation/public-api/text/style/underline.h>
#include <algorithm>

#define DALI_ASSERT_VALID_UNDERLINE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Underline object")

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
    mDashGap(DEFAULT_DASH_GAP)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mThickness(rhs.mThickness),
    mType(rhs.mType),
    mDashLength(rhs.mDashLength),
    mDashGap(rhs.mDashGap)
  {
  }

  UiColor mColor;
  float   mThickness;
  Type    mType;
  float   mDashLength;
  float   mDashGap;
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

void Underline::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  mImpl->mColor = color;
}

const UiColor& Underline::GetColor() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  return mImpl->mColor;
}

void Underline::SetThickness(float thickness)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  mImpl->mThickness = std::max(0.0f, thickness);
}

float Underline::GetThickness() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  return mImpl->mThickness;
}

void Underline::SetType(Type type)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  mImpl->mType = type;
}

Underline::Type Underline::GetType() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  return mImpl->mType;
}

void Underline::SetDashLength(float length)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  mImpl->mDashLength = std::max(0.0f, length);
}

float Underline::GetDashLength() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  return mImpl->mDashLength;
}

void Underline::SetDashGap(float gap)
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  mImpl->mDashGap = std::max(0.0f, gap);
}

float Underline::GetDashGap() const
{
  DALI_ASSERT_VALID_UNDERLINE(mImpl);
  return mImpl->mDashGap;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_UNDERLINE
