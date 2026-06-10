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
#include <dali-ui-foundation/public-api/text/fit/text-fit-candidate.h>
#include <algorithm>

#define DALI_ASSERT_VALID_FIT_CANDIDATE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from FitCandidate object")

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

class FitCandidate::Impl
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

FitCandidate::FitCandidate()
: mImpl(new Impl())
{
}

FitCandidate::FitCandidate(float fontSize, float lineHeight)
: mImpl(new Impl(fontSize, lineHeight))
{
}

FitCandidate::FitCandidate(const FitCandidate& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

FitCandidate::FitCandidate(FitCandidate&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

FitCandidate& FitCandidate::operator=(const FitCandidate& rhs)
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

FitCandidate& FitCandidate::operator=(FitCandidate&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

FitCandidate::~FitCandidate()
{
  delete mImpl;
}

void FitCandidate::SetFontSize(float size)
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  mImpl->mFontSize = std::max(1.0f, size);
}

float FitCandidate::GetFontSize() const
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  return mImpl->mFontSize;
}

void FitCandidate::SetLineHeight(float height)
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  mImpl->mLineHeight = std::max(0.0f, height);
}

float FitCandidate::GetLineHeight() const
{
  DALI_ASSERT_VALID_FIT_CANDIDATE(mImpl);
  return mImpl->mLineHeight;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FIT_CANDIDATE
