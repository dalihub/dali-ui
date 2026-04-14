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
#include <dali-ui-foundation/public-api/text/fit/text-fit-range.h>
#include <algorithm>

#define DALI_ASSERT_VALID_FIT_RANGE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from FitRange object")

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
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FIT_RANGE(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
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
    DALI_ASSERT_VALID_FIT_RANGE(rhs.mImpl);
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
  DALI_ASSERT_VALID_FIT_RANGE(mImpl);
  mImpl->mMinimumFontSize = std::max(1.0f, size);
  return *this;
}

float FitRange::GetMinimumFontSize() const
{
  DALI_ASSERT_VALID_FIT_RANGE(mImpl);
  return mImpl->mMinimumFontSize;
}

FitRange& FitRange::SetMaximumFontSize(float size)
{
  DALI_ASSERT_VALID_FIT_RANGE(mImpl);
  mImpl->mMaximumFontSize = std::max(1.0f, size);
  return *this;
}

float FitRange::GetMaximumFontSize() const
{
  DALI_ASSERT_VALID_FIT_RANGE(mImpl);
  return mImpl->mMaximumFontSize;
}

FitRange& FitRange::SetFontSizeStep(float step)
{
  DALI_ASSERT_VALID_FIT_RANGE(mImpl);
  mImpl->mFontSizeStep = std::max(1.0f, step);
  return *this;
}

float FitRange::GetFontSizeStep() const
{
  DALI_ASSERT_VALID_FIT_RANGE(mImpl);
  return mImpl->mFontSizeStep;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FIT_RANGE
