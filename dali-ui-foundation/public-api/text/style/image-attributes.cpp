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
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// CLASS HEADER
#include <dali-ui-foundation/public-api/text/style/image-attributes.h>

#define DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from ImageAttributes object")

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t SOURCE_MASK           = 1u << 0u;
constexpr uint32_t RESERVED_SIZE_MASK    = 1u << 1u;
constexpr uint32_t ALIGNMENT_MASK        = 1u << 2u;
constexpr uint32_t VERTICAL_OFFSET_MASK  = 1u << 3u;
constexpr uint32_t ALTERNATIVE_TEXT_MASK = 1u << 4u;

uint32_t ToMask(ImageAttributes::Attribute attribute)
{
  switch(attribute)
  {
    case ImageAttributes::Attribute::SOURCE:
      return SOURCE_MASK;
    case ImageAttributes::Attribute::RESERVED_SIZE:
      return RESERVED_SIZE_MASK;
    case ImageAttributes::Attribute::ALIGNMENT:
      return ALIGNMENT_MASK;
    case ImageAttributes::Attribute::VERTICAL_OFFSET:
      return VERTICAL_OFFSET_MASK;
    case ImageAttributes::Attribute::ALTERNATIVE_TEXT:
      return ALTERNATIVE_TEXT_MASK;
  }
  return 0u;
}
} // unnamed namespace

class ImageAttributes::Impl
{
public:
  Dali::String    source;
  Vector2         reservedSize{};
  InlineAlignment alignment{InlineAlignment::TEXT_BOTTOM};
  float           verticalOffset{0.0f};
  Dali::String    alternativeText;
  uint32_t        definedMask{0u};
};

ImageAttributes::ImageAttributes()
: mImpl(new Impl())
{
}

ImageAttributes::ImageAttributes(const Dali::String& source, const Vector2& reservedSize)
: mImpl(new Impl())
{
  mImpl->source       = source;
  mImpl->reservedSize = reservedSize;
  mImpl->definedMask  = SOURCE_MASK | RESERVED_SIZE_MASK;
}

ImageAttributes::ImageAttributes(const ImageAttributes& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

ImageAttributes::ImageAttributes(ImageAttributes&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

ImageAttributes& ImageAttributes::operator=(const ImageAttributes& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(rhs.mImpl);
    Impl* replacement = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = replacement;
  }
  return *this;
}

ImageAttributes& ImageAttributes::operator=(ImageAttributes&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

ImageAttributes::~ImageAttributes()
{
  delete mImpl;
}

void ImageAttributes::SetSource(const Dali::String& source)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  mImpl->source = source;
  mImpl->definedMask |= SOURCE_MASK;
}

Dali::String ImageAttributes::GetSource() const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  return mImpl->source;
}

void ImageAttributes::SetReservedSize(const Vector2& size)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  mImpl->reservedSize = size;
  mImpl->definedMask |= RESERVED_SIZE_MASK;
}

Vector2 ImageAttributes::GetReservedSize() const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  return mImpl->reservedSize;
}

void ImageAttributes::SetAlignment(InlineAlignment alignment)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  mImpl->alignment = alignment;
  mImpl->definedMask |= ALIGNMENT_MASK;
}

ImageAttributes::InlineAlignment ImageAttributes::GetAlignment() const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  return mImpl->alignment;
}

void ImageAttributes::SetVerticalOffset(float offset)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  mImpl->verticalOffset = offset;
  mImpl->definedMask |= VERTICAL_OFFSET_MASK;
}

float ImageAttributes::GetVerticalOffset() const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  return mImpl->verticalOffset;
}

void ImageAttributes::SetAlternativeText(const Dali::String& text)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  mImpl->alternativeText = text;
  mImpl->definedMask |= ALTERNATIVE_TEXT_MASK;
}

Dali::String ImageAttributes::GetAlternativeText() const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  return mImpl->alternativeText;
}

bool ImageAttributes::Has(Attribute attribute) const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  const uint32_t mask = ToMask(attribute);
  return mask != 0u && (mImpl->definedMask & mask) != 0u;
}

bool ImageAttributes::HasAttributes() const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  return mImpl->definedMask != 0u;
}

void ImageAttributes::Unset(Attribute attribute)
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  mImpl->definedMask &= ~ToMask(attribute);
}

bool ImageAttributes::operator==(const ImageAttributes& rhs) const
{
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(mImpl);
  DALI_ASSERT_VALID_IMAGE_ATTRIBUTES(rhs.mImpl);
  if(mImpl->definedMask != rhs.mImpl->definedMask)
  {
    return false;
  }
  return (!Has(Attribute::SOURCE) || mImpl->source == rhs.mImpl->source) &&
         (!Has(Attribute::RESERVED_SIZE) || mImpl->reservedSize == rhs.mImpl->reservedSize) &&
         (!Has(Attribute::ALIGNMENT) || mImpl->alignment == rhs.mImpl->alignment) &&
         (!Has(Attribute::VERTICAL_OFFSET) || mImpl->verticalOffset == rhs.mImpl->verticalOffset) &&
         (!Has(Attribute::ALTERNATIVE_TEXT) || mImpl->alternativeText == rhs.mImpl->alternativeText);
}

bool ImageAttributes::operator!=(const ImageAttributes& rhs) const
{
  return !(*this == rhs);
}

} // namespace Text
} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_IMAGE_ATTRIBUTES
