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
#include <dali-ui-foundation/public-api/text/font-variation/font-variation-axis.h>

#define DALI_ASSERT_VALID_FONT_VARIATION_AXIS(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from FontVariation::Axis object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace FontVariation
{
namespace
{
constexpr float DEFAULT_VALUE = 0.0f;
} // namespace

class Axis::Impl
{
public:
  Impl()
  : mTag(),
    mValue(DEFAULT_VALUE)
  {
  }

  Impl(const Dali::String& tag, float value)
  : mTag(tag),
    mValue(value)
  {
  }

  Impl(const Impl& rhs)
  : mTag(rhs.mTag),
    mValue(rhs.mValue)
  {
  }

  Dali::String mTag;
  float        mValue;
};

Axis::Axis()
: mImpl(new Impl())
{
}

Axis::Axis(const Dali::String& tag, float value)
: mImpl(new Impl(tag, value))
{
}

Axis::Axis(const Axis& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Axis::Axis(Axis&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Axis& Axis::operator=(const Axis& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_FONT_VARIATION_AXIS(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Axis& Axis::operator=(Axis&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Axis::~Axis()
{
  delete mImpl;
}

void Axis::SetTag(const Dali::String& tag)
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  mImpl->mTag = tag;
}

const Dali::String& Axis::GetTag() const
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  return mImpl->mTag;
}

void Axis::SetValue(float value)
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  mImpl->mValue = value;
}

float Axis::GetValue() const
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  return mImpl->mValue;
}

} // namespace FontVariation
} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FONT_VARIATION_AXIS
