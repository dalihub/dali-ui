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

#include <dali-ui-foundation/public-api/text/style/outline.h>

#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t DEFAULT_COLOR       = 0xFFFFFF;
constexpr float    DEFAULT_WIDTH       = 0.0f;
constexpr float    DEFAULT_BLUR_RADIUS = 0.0f;
} // namespace

class Outline::Impl
{
public:
  Impl()
  : mColor(DEFAULT_COLOR),
    mOffset(Vector2::ZERO),
    mWidth(DEFAULT_WIDTH),
    mBlurRadius(DEFAULT_BLUR_RADIUS)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mOffset(rhs.mOffset),
    mWidth(rhs.mWidth),
    mBlurRadius(rhs.mBlurRadius)
  {
  }

  UiColor mColor;
  Vector2 mOffset;
  float   mWidth;
  float   mBlurRadius;
};

Outline::Outline()
: mImpl(new Impl())
{
}

Outline::Outline(const Outline& rhs)
: mImpl(new Impl(*rhs.mImpl))
{
}

Outline::Outline(Outline&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

Outline& Outline::operator=(const Outline& rhs)
{
  if(this != &rhs)
  {
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

Outline& Outline::operator=(Outline&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

Outline::~Outline()
{
  delete mImpl;
}

Outline& Outline::SetColor(const UiColor& color)
{
  mImpl->mColor = color;
  return *this;
}

const UiColor& Outline::GetColor() const
{
  return mImpl->mColor;
}

Outline& Outline::SetOffset(const Vector2& offset)
{
  mImpl->mOffset = offset;
  return *this;
}

const Vector2& Outline::GetOffset() const
{
  return mImpl->mOffset;
}

Outline& Outline::SetWidth(float width)
{
  mImpl->mWidth = std::max(0.0f, width);
  return *this;
}

float Outline::GetWidth() const
{
  return mImpl->mWidth;
}

Outline& Outline::SetBlurRadius(float blurRadius)
{
  mImpl->mBlurRadius = std::max(0.0f, blurRadius);
  return *this;
}

float Outline::GetBlurRadius() const
{
  return mImpl->mBlurRadius;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
