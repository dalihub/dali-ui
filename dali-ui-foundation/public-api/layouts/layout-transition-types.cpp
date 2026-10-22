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
#include <dali-ui-foundation/public-api/layouts/layout-transition-types.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/layouts/layout-transition-types-impl.h>

#define DALI_ASSERT_VALID_LAYOUT_VALUE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from layout transition value")

namespace DALI_NAMESPACE
{
namespace Ui
{

LayoutTransitionTiming::LayoutTransitionTiming()
: LayoutTransitionTiming(Duration(0.3f))
{
}

LayoutTransitionTiming::LayoutTransitionTiming(Duration duration, AlphaFunction alpha, Duration delay)
: mImpl(new Impl{duration, alpha, delay})
{
}

LayoutTransitionTiming::LayoutTransitionTiming(const LayoutTransitionTiming& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

LayoutTransitionTiming::LayoutTransitionTiming(LayoutTransitionTiming&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

LayoutTransitionTiming& LayoutTransitionTiming::operator=(const LayoutTransitionTiming& other)
{
  if(this != &other)
  {
    DALI_ASSERT_VALID_LAYOUT_VALUE(other.mImpl);
    Impl* newImpl = new Impl(*other.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

LayoutTransitionTiming& LayoutTransitionTiming::operator=(LayoutTransitionTiming&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

LayoutTransitionTiming::~LayoutTransitionTiming()
{
  delete mImpl;
}

LayoutTransitionTiming& LayoutTransitionTiming::SetDuration(Duration duration)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->duration = duration;
  return *this;
}

Duration LayoutTransitionTiming::GetDuration() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->duration;
}

LayoutTransitionTiming& LayoutTransitionTiming::SetAlpha(AlphaFunction alpha)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->alpha = alpha;
  return *this;
}

const AlphaFunction& LayoutTransitionTiming::GetAlpha() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->alpha;
}

LayoutTransitionTiming& LayoutTransitionTiming::SetDelay(Duration delay)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->delay = delay;
  return *this;
}

Duration LayoutTransitionTiming::GetDelay() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->delay;
}

LayoutAnimatorTiming::LayoutAnimatorTiming()
: LayoutAnimatorTiming(Duration(0.3f))
{
}

LayoutAnimatorTiming::LayoutAnimatorTiming(Duration duration, AlphaFunction alpha, Duration delay)
: mImpl(new Impl{duration, alpha, delay})
{
}

LayoutAnimatorTiming::LayoutAnimatorTiming(const LayoutAnimatorTiming& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

LayoutAnimatorTiming::LayoutAnimatorTiming(LayoutAnimatorTiming&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

LayoutAnimatorTiming& LayoutAnimatorTiming::operator=(const LayoutAnimatorTiming& other)
{
  if(this != &other)
  {
    DALI_ASSERT_VALID_LAYOUT_VALUE(other.mImpl);
    Impl* newImpl = new Impl(*other.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

LayoutAnimatorTiming& LayoutAnimatorTiming::operator=(LayoutAnimatorTiming&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

LayoutAnimatorTiming::~LayoutAnimatorTiming()
{
  delete mImpl;
}

LayoutAnimatorTiming& LayoutAnimatorTiming::SetDuration(Duration duration)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->duration = duration;
  return *this;
}

Duration LayoutAnimatorTiming::GetDuration() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->duration;
}

LayoutAnimatorTiming& LayoutAnimatorTiming::SetAlpha(AlphaFunction alpha)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->alpha = alpha;
  return *this;
}

const AlphaFunction& LayoutAnimatorTiming::GetAlpha() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->alpha;
}

LayoutAnimatorTiming& LayoutAnimatorTiming::SetDelay(Duration delay)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->delay = delay;
  return *this;
}

Duration LayoutAnimatorTiming::GetDelay() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->delay;
}

LayoutBoundsEffect::LayoutBoundsEffect()
: mImpl(new Impl())
{
}

LayoutBoundsEffect::LayoutBoundsEffect(const LayoutBoundsEffect& other)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(other.mImpl);
  mImpl = new Impl(*other.mImpl);
}

LayoutBoundsEffect::LayoutBoundsEffect(LayoutBoundsEffect&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

LayoutBoundsEffect& LayoutBoundsEffect::operator=(const LayoutBoundsEffect& other)
{
  if(this != &other)
  {
    DALI_ASSERT_VALID_LAYOUT_VALUE(other.mImpl);
    Impl* newImpl = new Impl(*other.mImpl);
    delete mImpl;
    mImpl = newImpl;
  }
  return *this;
}

LayoutBoundsEffect& LayoutBoundsEffect::operator=(LayoutBoundsEffect&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

LayoutBoundsEffect::~LayoutBoundsEffect()
{
  delete mImpl;
}

LayoutBoundsEffect& LayoutBoundsEffect::SetTiming(const LayoutTransitionTiming& timing)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->timing = timing;
  return *this;
}

const LayoutTransitionTiming& LayoutBoundsEffect::GetTiming() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->timing;
}

LayoutBoundsEffect& LayoutBoundsEffect::SetOffset(LayoutBoundsOffset value)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->hasOffset = true;
  mImpl->offset    = value;
  return *this;
}

LayoutBoundsEffect& LayoutBoundsEffect::SetOffset(LayoutBoundsLength x, LayoutBoundsLength y)
{
  return SetOffset(LayoutBoundsOffset{x, y});
}

LayoutBoundsEffect& LayoutBoundsEffect::ClearOffset()
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->hasOffset = false;
  mImpl->offset    = LayoutBoundsOffset{};
  return *this;
}

bool LayoutBoundsEffect::HasOffset() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->hasOffset;
}

const LayoutBoundsOffset& LayoutBoundsEffect::GetOffset() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->offset;
}

LayoutBoundsEffect& LayoutBoundsEffect::SetSizeFactor(float x, float y)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->hasSizeFactor = true;
  mImpl->sizeFactorX   = x;
  mImpl->sizeFactorY   = y;
  return *this;
}

LayoutBoundsEffect& LayoutBoundsEffect::ClearSizeFactor()
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->hasSizeFactor = false;
  mImpl->sizeFactorX   = 1.0f;
  mImpl->sizeFactorY   = 1.0f;
  return *this;
}

bool LayoutBoundsEffect::HasSizeFactor() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->hasSizeFactor;
}

float LayoutBoundsEffect::GetSizeFactorX() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->sizeFactorX;
}

float LayoutBoundsEffect::GetSizeFactorY() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->sizeFactorY;
}

LayoutBoundsEffect& LayoutBoundsEffect::SetAnchor(float x, float y)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->anchorX = x;
  mImpl->anchorY = y;
  return *this;
}

float LayoutBoundsEffect::GetAnchorX() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->anchorX;
}

float LayoutBoundsEffect::GetAnchorY() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->anchorY;
}

LayoutBoundsEffect& LayoutBoundsEffect::SetClipMode(LayoutBoundsClipMode mode)
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  mImpl->clipMode = mode;
  return *this;
}

LayoutBoundsClipMode LayoutBoundsEffect::GetClipMode() const
{
  DALI_ASSERT_VALID_LAYOUT_VALUE(mImpl);
  return mImpl->clipMode;
}

LayoutAnimatorContext::LayoutAnimatorContext(Impl* impl)
: mImpl(impl),
  mOwnsImpl(false)
{
  DALI_ASSERT_ALWAYS(mImpl);
}

LayoutAnimatorContext::LayoutAnimatorContext(const LayoutAnimatorContext& other)
: mImpl(new Impl(*other.mImpl)),
  mOwnsImpl(true)
{
}

LayoutAnimatorContext& LayoutAnimatorContext::operator=(const LayoutAnimatorContext& other)
{
  if(this != &other)
  {
    Impl* newImpl = new Impl(*other.mImpl);
    if(mOwnsImpl)
    {
      delete mImpl;
    }
    mImpl     = newImpl;
    mOwnsImpl = true;
  }
  return *this;
}

LayoutAnimatorContext::~LayoutAnimatorContext()
{
  if(mOwnsImpl)
  {
    delete mImpl;
  }
}

View LayoutAnimatorContext::GetView() const
{
  return mImpl->view;
}

LayoutTransitionSlot LayoutAnimatorContext::GetSlot() const
{
  return mImpl->slot;
}

LayoutChangeCause LayoutAnimatorContext::GetChangeCause() const
{
  return mImpl->changeCause;
}

float LayoutAnimatorContext::GetProgress() const
{
  return mImpl->progress;
}

float LayoutAnimatorContext::GetRawProgress() const
{
  return mImpl->rawProgress;
}

LayoutRect LayoutAnimatorContext::GetFromBounds() const
{
  return mImpl->fromBounds;
}

LayoutRect LayoutAnimatorContext::GetToBounds() const
{
  return mImpl->toBounds;
}

} // namespace Ui
} // namespace DALI_NAMESPACE
