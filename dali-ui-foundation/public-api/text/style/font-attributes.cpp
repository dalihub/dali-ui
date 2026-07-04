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
#include <dali/public-api/math/math-utils.h>

// CLASS HEADER
#include <dali-ui-foundation/public-api/text/style/font-attributes.h>

#define DALI_ASSERT_VALID_FONT_ATTRIBUTES(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from FontAttributes object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t FAMILY_MASK = 1u << 0u;
constexpr uint32_t SIZE_MASK   = 1u << 1u;
constexpr uint32_t WEIGHT_MASK = 1u << 2u;
constexpr uint32_t WIDTH_MASK  = 1u << 3u;
constexpr uint32_t SLANT_MASK  = 1u << 4u;

uint32_t ToMask(FontAttributes::Attribute attribute)
{
  switch(attribute)
  {
    case FontAttributes::Attribute::FAMILY:
    {
      return FAMILY_MASK;
    }
    case FontAttributes::Attribute::SIZE:
    {
      return SIZE_MASK;
    }
    case FontAttributes::Attribute::WEIGHT:
    {
      return WEIGHT_MASK;
    }
    case FontAttributes::Attribute::WIDTH:
    {
      return WIDTH_MASK;
    }
    case FontAttributes::Attribute::SLANT:
    {
      return SLANT_MASK;
    }
  }

  return 0u;
}
} // namespace

class FontAttributes::Impl
{
public:
  Impl()
  : mWeight(FontWeight::NORMAL),
    mWidth(FontWidth::NORMAL),
    mSlant(FontSlant::NORMAL)
  {
  }

  Impl(const Impl& rhs)
  : mFamily(rhs.mFamily),
    mSize(rhs.mSize),
    mWeight(rhs.mWeight),
    mWidth(rhs.mWidth),
    mSlant(rhs.mSlant),
    mDefinedMask(rhs.mDefinedMask)
  {
  }

  Dali::String mFamily;
  float        mSize{0.0f};
  FontWeight   mWeight;
  FontWidth    mWidth;
  FontSlant    mSlant;
  uint32_t     mDefinedMask{0u};
};

FontAttributes::FontAttributes()
: mImpl(new Impl())
{
}

FontAttributes::FontAttributes(const FontAttributes& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

FontAttributes::FontAttributes(FontAttributes&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

FontAttributes& FontAttributes::operator=(const FontAttributes& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_FONT_ATTRIBUTES(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

FontAttributes& FontAttributes::operator=(FontAttributes&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

FontAttributes::~FontAttributes()
{
  delete mImpl;
}

void FontAttributes::SetFamily(const Dali::String& family)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  mImpl->mFamily = family;
  mImpl->mDefinedMask |= FAMILY_MASK;
}

Dali::String FontAttributes::GetFamily() const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  return mImpl->mFamily;
}

void FontAttributes::SetSize(float size)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  mImpl->mSize = size;
  mImpl->mDefinedMask |= SIZE_MASK;
}

float FontAttributes::GetSize() const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  return mImpl->mSize;
}

void FontAttributes::SetWeight(FontWeight weight)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  mImpl->mWeight = weight;
  mImpl->mDefinedMask |= WEIGHT_MASK;
}

FontWeight FontAttributes::GetWeight() const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  return mImpl->mWeight;
}

void FontAttributes::SetWidth(FontWidth width)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  mImpl->mWidth = width;
  mImpl->mDefinedMask |= WIDTH_MASK;
}

FontWidth FontAttributes::GetWidth() const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  return mImpl->mWidth;
}

void FontAttributes::SetSlant(FontSlant slant)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  mImpl->mSlant = slant;
  mImpl->mDefinedMask |= SLANT_MASK;
}

FontSlant FontAttributes::GetSlant() const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  return mImpl->mSlant;
}

bool FontAttributes::Has(Attribute attribute) const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  const uint32_t mask = ToMask(attribute);
  return mask != 0u && ((mImpl->mDefinedMask & mask) != 0u);
}

bool FontAttributes::HasAttributes() const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  return mImpl->mDefinedMask != 0u;
}

void FontAttributes::Unset(Attribute attribute)
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  const uint32_t mask = ToMask(attribute);
  if(mask != 0u)
  {
    mImpl->mDefinedMask &= ~mask;
  }
}

bool FontAttributes::operator==(const FontAttributes& rhs) const
{
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(mImpl);
  DALI_ASSERT_VALID_FONT_ATTRIBUTES(rhs.mImpl);

  if(mImpl->mDefinedMask != rhs.mImpl->mDefinedMask)
  {
    return false;
  }

  return (!Has(Attribute::FAMILY) || mImpl->mFamily == rhs.mImpl->mFamily) &&
         (!Has(Attribute::SIZE) || Dali::Equals(mImpl->mSize, rhs.mImpl->mSize)) &&
         (!Has(Attribute::WEIGHT) || mImpl->mWeight == rhs.mImpl->mWeight) &&
         (!Has(Attribute::WIDTH) || mImpl->mWidth == rhs.mImpl->mWidth) &&
         (!Has(Attribute::SLANT) || mImpl->mSlant == rhs.mImpl->mSlant);
}

bool FontAttributes::operator!=(const FontAttributes& rhs) const
{
  return !(*this == rhs);
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_FONT_ATTRIBUTES
