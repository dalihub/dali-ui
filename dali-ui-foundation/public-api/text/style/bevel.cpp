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
#include <dali-ui-foundation/public-api/text/style/bevel.h>
#include <algorithm>

#define DALI_ASSERT_VALID_BEVEL(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Bevel object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr Vector2  DEFAULT_DIRECTION(-1.0f, -1.0f);
constexpr float    DEFAULT_INTENSITY    = 1.0f;
constexpr uint32_t DEFAULT_LIGHT_COLOR  = 0x808080; // Grey
constexpr uint32_t DEFAULT_SHADOW_COLOR = 0x0D0D0D; // Dark grey
} // namespace

class Bevel::Impl
{
public:
  Impl()
  : mDirection(DEFAULT_DIRECTION),
    mIntensity(DEFAULT_INTENSITY),
    mLightColor(DEFAULT_LIGHT_COLOR),
    mShadowColor(DEFAULT_SHADOW_COLOR)
  {
  }

  Impl(const Impl& rhs)
  : mDirection(rhs.mDirection),
    mIntensity(rhs.mIntensity),
    mLightColor(rhs.mLightColor),
    mShadowColor(rhs.mShadowColor)
  {
  }

  Vector2 mDirection;
  float   mIntensity;
  UiColor mLightColor;
  UiColor mShadowColor;
};

Bevel::Bevel()
: mImpl(new Impl())
{
}

Bevel::Bevel(const Bevel& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_BEVEL(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Bevel::Bevel(Bevel&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Bevel& Bevel::operator=(const Bevel& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_BEVEL(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Bevel& Bevel::operator=(Bevel&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Bevel::~Bevel()
{
  delete mImpl;
}

void Bevel::SetDirection(const Vector2& direction)
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  mImpl->mDirection = direction;
}

const Vector2& Bevel::GetDirection() const
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  return mImpl->mDirection;
}

void Bevel::SetIntensity(float intensity)
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  mImpl->mIntensity = std::max(0.0f, intensity);
}

float Bevel::GetIntensity() const
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  return mImpl->mIntensity;
}

void Bevel::SetLightColor(const UiColor& color)
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  mImpl->mLightColor = color;
}

const UiColor& Bevel::GetLightColor() const
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  return mImpl->mLightColor;
}

void Bevel::SetShadowColor(const UiColor& color)
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  mImpl->mShadowColor = color;
}

const UiColor& Bevel::GetShadowColor() const
{
  DALI_ASSERT_VALID_BEVEL(mImpl);
  return mImpl->mShadowColor;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_BEVEL
