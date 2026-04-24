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
#include <dali-ui-foundation/public-api/text/input-filter.h>

#define DALI_ASSERT_VALID_INPUT_FILTER(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from InputFilter object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
const Dali::String EMPTY_PATTERN;
} // namespace

class InputFilter::Impl
{
public:
  Impl()
  : mAllowPattern(EMPTY_PATTERN),
    mDenyPattern(EMPTY_PATTERN)
  {
  }

  Impl(const Impl& rhs)
  : mAllowPattern(rhs.mAllowPattern),
    mDenyPattern(rhs.mDenyPattern)
  {
  }

  Dali::String mAllowPattern;
  Dali::String mDenyPattern;
};

InputFilter::InputFilter()
: mImpl(new Impl())
{
}

InputFilter::InputFilter(const InputFilter& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_INPUT_FILTER(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

InputFilter::InputFilter(InputFilter&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

InputFilter& InputFilter::operator=(const InputFilter& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_INPUT_FILTER(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

InputFilter& InputFilter::operator=(InputFilter&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

InputFilter::~InputFilter()
{
  delete mImpl;
}

InputFilter& InputFilter::SetAllowPattern(const Dali::String& pattern)
{
  DALI_ASSERT_VALID_INPUT_FILTER(mImpl);
  mImpl->mAllowPattern = pattern;
  return *this;
}

const Dali::String& InputFilter::GetAllowPattern() const
{
  DALI_ASSERT_VALID_INPUT_FILTER(mImpl);
  return mImpl->mAllowPattern;
}

InputFilter& InputFilter::SetDenyPattern(const Dali::String& pattern)
{
  DALI_ASSERT_VALID_INPUT_FILTER(mImpl);
  mImpl->mDenyPattern = pattern;
  return *this;
}

const Dali::String& InputFilter::GetDenyPattern() const
{
  DALI_ASSERT_VALID_INPUT_FILTER(mImpl);
  return mImpl->mDenyPattern;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_INPUT_FILTER
