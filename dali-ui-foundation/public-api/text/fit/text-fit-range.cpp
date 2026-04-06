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

#include <dali-ui-foundation/public-api/text/fit/text-fit-range.h>

#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr float DEFAULT_MINIMUM_FONT_SIZE = 16.0f;
constexpr float DEFAULT_MAXIMUM_FONT_SIZE = 32.0f;
constexpr float DEFAULT_FONT_SIZE_STEP    = 4.0f;
} // namespace

class FitRange::Impl
{
public:
  Impl()
  : mMinimumFontSize(DEFAULT_MINIMUM_FONT_SIZE),
    mMaximumFontSize(DEFAULT_MAXIMUM_FONT_SIZE),
    mFontSizeStep(DEFAULT_FONT_SIZE_STEP)
  {
  }

  Impl(float minimumFontSize, float maximumFontSize, float fontSizeStep)
  : mMinimumFontSize(minimumFontSize),
    mMaximumFontSize(maximumFontSize),
    mFontSizeStep(fontSizeStep)
  {
  }

  Impl(const Impl& rhs)
  : mMinimumFontSize(rhs.mMinimumFontSize),
    mMaximumFontSize(rhs.mMaximumFontSize),
    mFontSizeStep(rhs.mFontSizeStep)
  {
  }

  float mMinimumFontSize;
  float mMaximumFontSize;
  float mFontSizeStep;
};

FitRange::FitRange()
: mImpl(new Impl())
{
}

FitRange::FitRange(float minimumFontSize, float maximumFontSize, float fontSizeStep)
: mImpl(new Impl(minimumFontSize, maximumFontSize, fontSizeStep))
{
}

FitRange::FitRange(const FitRange& rhs)
: mImpl(new Impl(*rhs.mImpl))
{
}

FitRange::FitRange(FitRange&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

FitRange& FitRange::operator=(const FitRange& rhs)
{
  if(this != &rhs)
  {
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

FitRange& FitRange::operator=(FitRange&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

FitRange::~FitRange()
{
  delete mImpl;
}

FitRange& FitRange::SetMinimumFontSize(float size)
{
  mImpl->mMinimumFontSize = std::max(1.0f, size);
  return *this;
}

float FitRange::GetMinimumFontSize() const
{
  return mImpl->mMinimumFontSize;
}

FitRange& FitRange::SetMaximumFontSize(float size)
{
  mImpl->mMaximumFontSize = std::max(1.0f, size);
  return *this;
}

float FitRange::GetMaximumFontSize() const
{
  return mImpl->mMaximumFontSize;
}

FitRange& FitRange::SetFontSizeStep(float step)
{
  mImpl->mFontSizeStep = std::max(1.0f, step);
  return *this;
}

float FitRange::GetFontSizeStep() const
{
  return mImpl->mFontSizeStep;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
