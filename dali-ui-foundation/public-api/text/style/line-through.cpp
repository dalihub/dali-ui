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

#include <dali-ui-foundation/public-api/text/style/line-through.h>

#include <algorithm>

namespace Dali
{
namespace Ui
{
namespace Text
{
namespace
{
constexpr uint32_t DEFAULT_COLOR     = 0x000000;
constexpr float    DEFAULT_THICKNESS = 1.0f;
} // namespace

class LineThrough::Impl
{
public:
  Impl()
  : mColor(DEFAULT_COLOR),
    mThickness(DEFAULT_THICKNESS)
  {
  }

  Impl(const Impl& rhs)
  : mColor(rhs.mColor),
    mThickness(rhs.mThickness)
  {
  }

  UiColor mColor;
  float   mThickness;
};

LineThrough::LineThrough()
: mImpl(new Impl())
{
}

LineThrough::LineThrough(const LineThrough& rhs)
: mImpl(new Impl(*rhs.mImpl))
{
}

LineThrough::LineThrough(LineThrough&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

LineThrough& LineThrough::operator=(const LineThrough& rhs)
{
  if(this != &rhs)
  {
    Impl* newImpl = new Impl(*rhs.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

LineThrough& LineThrough::operator=(LineThrough&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

LineThrough::~LineThrough()
{
  delete mImpl;
}

LineThrough& LineThrough::SetColor(const UiColor& color)
{
  mImpl->mColor = color;
  return *this;
}

const UiColor& LineThrough::GetColor() const
{
  return mImpl->mColor;
}

LineThrough& LineThrough::SetThickness(float thickness)
{
  mImpl->mThickness = std::max(0.0f, thickness);
  return *this;
}

float LineThrough::GetThickness() const
{
  return mImpl->mThickness;
}

} // namespace Text
} // namespace Ui
} // namespace Dali
