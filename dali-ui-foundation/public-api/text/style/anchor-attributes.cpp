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

// CLASS HEADER
#include <dali-ui-foundation/public-api/text/style/anchor-attributes.h>

#define DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from AnchorAttributes object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t HREF_MASK          = 1u << 0u;
constexpr uint32_t COLOR_MASK         = 1u << 1u;
constexpr uint32_t CLICKED_COLOR_MASK = 1u << 2u;

uint32_t ToMask(AnchorAttributes::Attribute attribute)
{
  switch(attribute)
  {
    case AnchorAttributes::Attribute::HREF:
    {
      return HREF_MASK;
    }
    case AnchorAttributes::Attribute::COLOR:
    {
      return COLOR_MASK;
    }
    case AnchorAttributes::Attribute::CLICKED_COLOR:
    {
      return CLICKED_COLOR_MASK;
    }
  }

  return 0u;
}
} // namespace

class AnchorAttributes::Impl
{
public:
  Impl() = default;

  Impl(const Impl& rhs)
  : mHref(rhs.mHref),
    mColor(rhs.mColor),
    mClickedColor(rhs.mClickedColor),
    mDefinedMask(rhs.mDefinedMask)
  {
  }

  Dali::String mHref;
  UiColor      mColor;
  UiColor      mClickedColor;
  uint32_t     mDefinedMask{0u};
};

AnchorAttributes::AnchorAttributes()
: mImpl(new Impl())
{
}

AnchorAttributes::AnchorAttributes(const AnchorAttributes& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

AnchorAttributes::AnchorAttributes(AnchorAttributes&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

AnchorAttributes& AnchorAttributes::operator=(const AnchorAttributes& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

AnchorAttributes& AnchorAttributes::operator=(AnchorAttributes&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

AnchorAttributes::~AnchorAttributes()
{
  delete mImpl;
}

void AnchorAttributes::SetHref(const Dali::String& href)
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  mImpl->mHref = href;
  mImpl->mDefinedMask |= HREF_MASK;
}

Dali::String AnchorAttributes::GetHref() const
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  return mImpl->mHref;
}

void AnchorAttributes::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  mImpl->mColor = color;
  mImpl->mDefinedMask |= COLOR_MASK;
}

UiColor AnchorAttributes::GetColor() const
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  return mImpl->mColor;
}

void AnchorAttributes::SetClickedColor(const UiColor& color)
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  mImpl->mClickedColor = color;
  mImpl->mDefinedMask |= CLICKED_COLOR_MASK;
}

UiColor AnchorAttributes::GetClickedColor() const
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  return mImpl->mClickedColor;
}

bool AnchorAttributes::Has(Attribute attribute) const
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  const uint32_t mask = ToMask(attribute);
  return mask != 0u && ((mImpl->mDefinedMask & mask) != 0u);
}

bool AnchorAttributes::HasAttributes() const
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  return mImpl->mDefinedMask != 0u;
}

void AnchorAttributes::Unset(Attribute attribute)
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  const uint32_t mask = ToMask(attribute);
  if(mask != 0u)
  {
    mImpl->mDefinedMask &= ~mask;
  }
}

bool AnchorAttributes::operator==(const AnchorAttributes& rhs) const
{
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(mImpl);
  DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES(rhs.mImpl);

  if(mImpl->mDefinedMask != rhs.mImpl->mDefinedMask)
  {
    return false;
  }

  return (!Has(Attribute::HREF) || mImpl->mHref == rhs.mImpl->mHref) &&
         (!Has(Attribute::COLOR) || mImpl->mColor == rhs.mImpl->mColor) &&
         (!Has(Attribute::CLICKED_COLOR) || mImpl->mClickedColor == rhs.mImpl->mClickedColor);
}

bool AnchorAttributes::operator!=(const AnchorAttributes& rhs) const
{
  return !(*this == rhs);
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_ANCHOR_ATTRIBUTES
