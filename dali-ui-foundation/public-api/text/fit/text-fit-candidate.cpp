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
#include <dali-ui-foundation/public-api/text/fit/text-fit.h>
#include <algorithm>

#define DALI_ASSERT_VALID_FIT_CANDIDATE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Text::Fit::Candidate object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr float DEFAULT_FONT_SIZE   = 16.0f;
constexpr float DEFAULT_LINE_HEIGHT = 0.0f;
} // unnamed namespace

class Fit::Candidate::Impl
{
public:
  Impl()
  : mFontSize(DEFAULT_FONT_SIZE),
    mLineHeight(DEFAULT_LINE_HEIGHT)
  {
  }

  Impl(float fontSize, float lineHeight)
  : mFontSize(fontSize),
    mLineHeight(lineHeight)
  {
  }

  Impl(const Impl& rhs)
  : mFontSize(rhs.mFontSize),
    mLineHeight(rhs.mLineHeight)
  {
  }

  float mFontSize;
  float mLineHeight;
};

Fit::Candidate::Candidate()
: mImpl(new Impl())
{
}

Fit::Candidate::Candidate(float fontSize, float lineHeight)
: mImpl(new Impl(fontSize, lineHeight))
{
}

Fit::Candidate::Candidate(const Candidate& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Fit::Candidate::Candidate(Candidate&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Fit::Candidate& Fit::Candidate::operator=(const Candidate& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_FIT_CANDIDATE(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Fit::Candidate& Fit::Candidate::operator=(Candidate&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Fit::Candidate::~Candidate()
{
  delete mImpl;
}

void Fit::Candidate::SetFontSize(float size)
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  mImpl->mFontSize = std::max(1.0f, size);
}

float Fit::Candidate::GetFontSize() const
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  return mImpl->mFontSize;
}

void Fit::Candidate::SetLineHeight(float height)
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  mImpl->mLineHeight = std::max(0.0f, height);
}

float Fit::Candidate::GetLineHeight() const
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  return mImpl->mLineHeight;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FIT_CANDIDATE
