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

#define DALI_ASSERT_VALID_FIT(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Text::Fit object")

#define DALI_ASSERT_FIT_TYPE(actual, expected, message) \
  DALI_ASSERT_ALWAYS((actual) == (expected) && (message))

namespace Dali
{
namespace Ui
{
namespace Text
{

class Fit::Impl
{
public:
  Impl()
  : mType(Type::NONE),
    mRange(),
    mCandidates()
  {
  }

  explicit Impl(const Fit::Range& range)
  : mType(Type::RANGE),
    mRange(range),
    mCandidates()
  {
  }

  explicit Impl(const Dali::Vector<Fit::Candidate>& candidates)
  : mType(candidates.Empty() ? Type::NONE : Type::CANDIDATES),
    mRange(),
    mCandidates(candidates)
  {
    if(mType == Type::NONE)
    {
      mCandidates.Clear();
    }
  }

  Impl(const Impl& rhs)
  : mType(rhs.mType),
    mRange(rhs.mRange),
    mCandidates(rhs.mCandidates)
  {
  }

  Type                         mType;
  Fit::Range                   mRange;
  Dali::Vector<Fit::Candidate> mCandidates;
};

Fit::Fit()
: mImpl(new Impl())
{
}

Fit::Fit(const Fit::Range& range)
: mImpl(new Impl(range))
{
}

Fit::Fit(const Dali::Vector<Fit::Candidate>& candidates)
: mImpl(new Impl(candidates))
{
}

Fit::Fit(const Fit& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FIT(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Fit::Fit(Fit&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Fit& Fit::operator=(const Fit& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_FIT(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Fit& Fit::operator=(Fit&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Fit::~Fit()
{
  delete mImpl;
}

const Fit& Fit::None()
{
  static const Fit none;
  return none;
}

Fit Fit::FromRange(const Fit::Range& range)
{
  return Fit(range);
}

Fit Fit::FromCandidates(const Dali::Vector<Fit::Candidate>& candidates)
{
  if(candidates.Empty())
  {
    return None();
  }
  return Fit(candidates);
}

Fit::Type Fit::GetType() const
{
  DALI_ASSERT_VALID_FIT(mImpl);
  return mImpl->mType;
}

const Fit::Range& Fit::GetRange() const
{
  DALI_ASSERT_VALID_FIT(mImpl);
  DALI_ASSERT_FIT_TYPE(mImpl->mType, Type::RANGE, "Text::Fit does not contain a range.");
  return mImpl->mRange;
}

const Dali::Vector<Fit::Candidate>& Fit::GetCandidates() const
{
  DALI_ASSERT_VALID_FIT(mImpl);
  DALI_ASSERT_FIT_TYPE(mImpl->mType, Type::CANDIDATES, "Text::Fit does not contain candidates.");
  return mImpl->mCandidates;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FIT
#undef DALI_ASSERT_FIT_TYPE
