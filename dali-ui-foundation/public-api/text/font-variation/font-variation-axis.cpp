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
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from FontVariationAxis object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr float DEFAULT_VALUE = 0.0f;
} // namespace

class FontVariationAxis::Impl
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

FontVariationAxis::FontVariationAxis()
: mImpl(new Impl())
{
}

FontVariationAxis::FontVariationAxis(const Dali::String& tag, float value)
: mImpl(new Impl(tag, value))
{
}

FontVariationAxis::FontVariationAxis(const FontVariationAxis& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

FontVariationAxis::FontVariationAxis(FontVariationAxis&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

FontVariationAxis& FontVariationAxis::operator=(const FontVariationAxis& rhs)
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

FontVariationAxis& FontVariationAxis::operator=(FontVariationAxis&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

FontVariationAxis::~FontVariationAxis()
{
  delete mImpl;
}

FontVariationAxis& FontVariationAxis::SetTag(const Dali::String& tag)
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  mImpl->mTag = tag;
  return *this;
}

const Dali::String& FontVariationAxis::GetTag() const
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  return mImpl->mTag;
}

FontVariationAxis& FontVariationAxis::SetValue(float value)
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  mImpl->mValue = value;
  return *this;
}

float FontVariationAxis::GetValue() const
{
  DALI_ASSERT_VALID_FONT_VARIATION_AXIS(mImpl);
  return mImpl->mValue;
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FONT_VARIATION_AXIS
