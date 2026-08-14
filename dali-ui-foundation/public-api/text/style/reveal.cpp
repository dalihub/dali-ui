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

#include <dali-ui-foundation/public-api/text/style/reveal.h>

#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/math-utils.h>

#include <algorithm>
#include <cmath>

#define DALI_ASSERT_VALID_REVEAL(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Reveal object")

#define DALI_ASSERT_REVEAL_NOT_NONE(impl, message) \
  DALI_ASSERT_ALWAYS(!(impl)->mIsNone && message)

namespace Dali
{
namespace Ui
{
namespace Text
{
class Reveal::Impl
{
public:
  Impl()
  : mUnit(Unit::CHARACTER),
    mFadeDurationRatio(AUTO_FADE_DURATION_RATIO),
    mIsNone(false)
  {
  }

  Unit  mUnit;
  float mFadeDurationRatio;
  bool  mIsNone;
};

Reveal::Reveal()
: mImpl(new Impl())
{
}

Reveal::Reveal(const Reveal& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_REVEAL(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Reveal::Reveal(Reveal&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Reveal& Reveal::operator=(const Reveal& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_REVEAL(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Reveal& Reveal::operator=(Reveal&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Reveal::~Reveal()
{
  delete mImpl;
}

const Reveal& Reveal::None()
{
  static const Reveal none = []()
  {
    Reveal reveal;
    reveal.mImpl->mIsNone = true;
    return reveal;
  }();
  return none;
}

bool Reveal::operator==(const Reveal& rhs) const
{
  DALI_ASSERT_VALID_REVEAL(mImpl);
  DALI_ASSERT_VALID_REVEAL(rhs.mImpl);
  if(mImpl->mIsNone || rhs.mImpl->mIsNone)
  {
    return mImpl->mIsNone == rhs.mImpl->mIsNone;
  }
  return mImpl->mUnit == rhs.mImpl->mUnit && Dali::Equals(mImpl->mFadeDurationRatio, rhs.mImpl->mFadeDurationRatio);
}

bool Reveal::operator!=(const Reveal& rhs) const
{
  return !(*this == rhs);
}

void Reveal::SetUnit(Unit unit)
{
  DALI_ASSERT_VALID_REVEAL(mImpl);
  DALI_ASSERT_REVEAL_NOT_NONE(mImpl, "Cannot modify Text::Reveal::None().");
  mImpl->mUnit = unit;
}

Reveal::Unit Reveal::GetUnit() const
{
  DALI_ASSERT_VALID_REVEAL(mImpl);
  DALI_ASSERT_REVEAL_NOT_NONE(mImpl, "Cannot access Text::Reveal::None() properties.");
  return mImpl->mUnit;
}

void Reveal::SetFadeDurationRatio(float ratio)
{
  DALI_ASSERT_VALID_REVEAL(mImpl);
  DALI_ASSERT_REVEAL_NOT_NONE(mImpl, "Cannot modify Text::Reveal::None().");
  if(ratio == AUTO_FADE_DURATION_RATIO)
  {
    mImpl->mFadeDurationRatio = AUTO_FADE_DURATION_RATIO;
  }
  else if(std::isnan(ratio))
  {
    mImpl->mFadeDurationRatio = 0.0f;
  }
  else
  {
    mImpl->mFadeDurationRatio = std::max(0.0f, std::min(1.0f, ratio));
  }
}

float Reveal::GetFadeDurationRatio() const
{
  DALI_ASSERT_VALID_REVEAL(mImpl);
  DALI_ASSERT_REVEAL_NOT_NONE(mImpl, "Cannot access Text::Reveal::None() properties.");
  return mImpl->mFadeDurationRatio;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_REVEAL_NOT_NONE
#undef DALI_ASSERT_VALID_REVEAL
