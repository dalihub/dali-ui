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

// CLASS HEADER
#include <dali-ui-foundation/public-api/shadow.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

#include <algorithm>

// INTERNAL INCLUDES
#include <dali-ui-foundation/devel-api/visuals/visual-properties-devel.h>

#define DALI_ASSERT_VALID_SHADOW(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from Shadow object")

namespace Dali
{
namespace Ui
{
namespace
{
constexpr float DEFAULT_BLUR_RADIUS = 0.0f;
} // namespace

class Shadow::Impl
{
public:
  Impl()
  : mColor(0x000000, 0.5f),
    mBlurRadius(DEFAULT_BLUR_RADIUS),
    mCutoutPolicy(CutoutPolicy::NONE),
    mOffset(Vector2::ZERO),
    mExtents(Vector2::ZERO)
  {
  }

  Impl(float blurRadius, const Vector2& offset, const UiColor& color, const Vector2& extents, CutoutPolicy cutoutPolicy)
  : mColor(color),
    mBlurRadius(std::max(0.0f, blurRadius)),
    mCutoutPolicy(cutoutPolicy),
    mOffset(offset),
    mExtents(extents)
  {
  }

  UiColor      mColor;
  float        mBlurRadius;
  CutoutPolicy mCutoutPolicy;
  Vector2      mOffset;
  Vector2      mExtents;
};

Shadow::Shadow()
: mImpl(new Impl())
{
}

Shadow::Shadow(float blurRadius, const Vector2& offset, const UiColor& color, const Vector2& extents, CutoutPolicy cutoutPolicy)
: mImpl(new Impl(blurRadius, offset, color, extents, cutoutPolicy))
{
}

Shadow::Shadow(float blurRadius, const UiColor& color)
: Shadow(blurRadius, Vector2::ZERO, color)
{
}

Shadow::Shadow(const Property::Map& map)
: Shadow()
{
  const Property::Value* colorValue = map.Find(VisualBasePropertyIndex::MIX_COLOR);
  if(colorValue)
  {
    Vector4 color;
    if(colorValue->Get(color))
    {
      mImpl->mColor = UiColor(color);
    }
  }

  const Property::Value* blurRadiusValue = map.Find(ColorVisualPropertyIndex::BLUR_RADIUS);
  if(blurRadiusValue)
  {
    float blurRadius = 0.0f;
    if(blurRadiusValue->Get(blurRadius))
    {
      mImpl->mBlurRadius = std::max(0.0f, blurRadius);
    }
  }

  const Property::Value* cutoutPolicyValue = map.Find(ColorVisualPropertyIndex::CUTOUT_POLICY);
  if(cutoutPolicyValue)
  {
    int cutoutPolicy = static_cast<int>(CutoutPolicy::NONE);
    if(cutoutPolicyValue->Get(cutoutPolicy))
    {
      mImpl->mCutoutPolicy = static_cast<CutoutPolicy>(cutoutPolicy);
    }
  }

  const Property::Value* transformValue = map.Find(VisualBasePropertyIndex::TRANSFORM);
  const Property::Map*   transformMap   = transformValue ? transformValue->GetMap() : nullptr;
  if(transformMap)
  {
    const Property::Value* offsetValue = transformMap->Find(Visual::Transform::Property::OFFSET);
    if(offsetValue)
    {
      offsetValue->Get(mImpl->mOffset);
    }

    const Property::Value* extraSizeValue = transformMap->Find(DevelVisual::Transform::Property::EXTRA_SIZE);
    if(extraSizeValue)
    {
      extraSizeValue->Get(mImpl->mExtents);
    }
  }
}

Shadow::Shadow(const Shadow& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_SHADOW(rhs.mImpl);
  mImpl = new Impl(*rhs.mImpl);
}

Shadow::Shadow(Shadow&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  // Match text style value objects: moving transfers ownership, and using the moved-from object asserts.
  rhs.mImpl = nullptr;
}

Shadow& Shadow::operator=(const Shadow& rhs)
{
  if(this != &rhs)
  {
    DALI_ASSERT_VALID_SHADOW(rhs.mImpl);
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Shadow& Shadow::operator=(Shadow&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl = rhs.mImpl;
    // Match text style value objects: moving transfers ownership, and using the moved-from object asserts.
    rhs.mImpl = nullptr;
  }
  return *this;
}

Shadow::~Shadow()
{
  delete mImpl;
}

Shadow& Shadow::SetColor(const UiColor& color)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mColor = color;
  return *this;
}

const UiColor& Shadow::GetColor() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mColor;
}

Shadow& Shadow::SetBlurRadius(float blurRadius)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mBlurRadius = std::max(0.0f, blurRadius);
  return *this;
}

float Shadow::GetBlurRadius() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mBlurRadius;
}

Shadow& Shadow::SetCutoutPolicy(CutoutPolicy cutoutPolicy)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mCutoutPolicy = cutoutPolicy;
  return *this;
}

CutoutPolicy Shadow::GetCutoutPolicy() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mCutoutPolicy;
}

Shadow& Shadow::SetOffset(const Vector2& offset)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mOffset = offset;
  return *this;
}

const Vector2& Shadow::GetOffset() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mOffset;
}

Shadow& Shadow::SetExtents(const Vector2& extents)
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  mImpl->mExtents = extents;
  return *this;
}

const Vector2& Shadow::GetExtents() const
{
  DALI_ASSERT_VALID_SHADOW(mImpl);
  return mImpl->mExtents;
}

} // namespace Ui
} // namespace Dali

#undef DALI_ASSERT_VALID_SHADOW
