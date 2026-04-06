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

#include <dali-ui-foundation/public-api/text/fit/text-fit-candidate.h>

#include <algorithm>

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
: mImpl(new Impl(*rhs.mImpl))
{
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

FitCandidate& FitCandidate::SetFontSize(float size)
{
  mImpl->mFontSize = std::max(1.0f, size);
  return *this;
}

float FitCandidate::GetFontSize() const
{
  return mImpl->mFontSize;
}

FitCandidate& FitCandidate::SetLineHeight(float height)
{
  mImpl->mLineHeight = std::max(0.0f, height);
  return *this;
}

float FitCandidate::GetLineHeight() const
{
  return mImpl->mLineHeight;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
