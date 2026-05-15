/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/public-api/gradient/gradient-stop-node.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector4.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/public-api/ui-color.h>

namespace Dali
{
namespace Ui
{
namespace Gradient
{
class StopNode::Impl
{
public:
  Impl(float offset, const UiColor& color)
  : mOffset(offset),
    mStopColor(color)
  {
  }

  Impl()
  : Impl(0.0f, UiColor())
  {
  }

public:
  float   mOffset;
  UiColor mStopColor;
};

StopNode::StopNode()
: mImpl(new Impl())
{
}

StopNode::StopNode(float offset, const UiColor& color)
: mImpl(new Impl(offset, color))
{
}

StopNode::~StopNode()
{
  delete mImpl;
}

StopNode::StopNode(const StopNode& rhs)
: mImpl(new Impl(rhs.GetOffset(), rhs.GetColor()))
{
}

StopNode::StopNode(StopNode&& rhs) noexcept
: mImpl(rhs.mImpl)
{
  rhs.mImpl = nullptr;
}

StopNode& StopNode::operator=(const StopNode& rhs)
{
  if(DALI_UNLIKELY(!mImpl))
  {
    mImpl = new Impl(rhs.GetOffset(), rhs.GetColor());
  }
  else
  {
    mImpl->mOffset    = rhs.GetOffset();
    mImpl->mStopColor = rhs.GetColor();
  }
  return *this;
}

StopNode& StopNode::operator=(StopNode&& rhs) noexcept
{
  if(this != &rhs)
  {
    delete mImpl;
    mImpl     = rhs.mImpl;
    rhs.mImpl = nullptr;
  }
  return *this;
}

float StopNode::GetOffset() const
{
  return DALI_LIKELY(mImpl) ? mImpl->mOffset : 0.0f;
}

StopNode& StopNode::SetOffset(float offset)
{
  if(DALI_UNLIKELY(!mImpl))
  {
    mImpl = new Impl();
  }
  mImpl->mOffset = offset;
  return *this;
}

UiColor StopNode::GetColor() const
{
  return DALI_LIKELY(mImpl) ? mImpl->mStopColor : UiColor();
}

StopNode& StopNode::SetColor(const UiColor& color)
{
  if(DALI_UNLIKELY(!mImpl))
  {
    mImpl = new Impl();
  }
  mImpl->mStopColor = color;
  return *this;
}

} // namespace Gradient
} // namespace Ui
} // namespace Dali