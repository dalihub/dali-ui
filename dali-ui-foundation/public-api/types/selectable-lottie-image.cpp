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
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace DALI_NAMESPACE
{
namespace Ui
{

#define DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from SelectableLottieImage object")

struct SelectableLottieImage::Impl
{
  Impl(const Dali::String& url,
       const Int32Pair&    selectRange,
       const Int32Pair&    deselectRange,
       const Dali::String& innerFillKeyPath)
  : mUrl(url),
    mSelectRange(selectRange),
    mDeselectRange(deselectRange),
    mInnerFillKeyPath(innerFillKeyPath)
  {
  }

  Dali::String mUrl;
  Int32Pair    mSelectRange;
  Int32Pair    mDeselectRange;
  Dali::String mInnerFillKeyPath;
};

SelectableLottieImage::SelectableLottieImage(const Dali::String& url,
                                             const Int32Pair&    selectRange,
                                             const Int32Pair&    deselectRange,
                                             const Dali::String& innerFillKeyPath)
: mImpl(Dali::MakeUnique<Impl>(url, selectRange, deselectRange, innerFillKeyPath))
{
}

SelectableLottieImage::SelectableLottieImage(const SelectableLottieImage& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

SelectableLottieImage::SelectableLottieImage(SelectableLottieImage&& rhs) noexcept = default;

SelectableLottieImage& SelectableLottieImage::operator=(const SelectableLottieImage& rhs)
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(rhs.mImpl);
  if(this != &rhs)
  {
    mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

SelectableLottieImage& SelectableLottieImage::operator=(SelectableLottieImage&& rhs) noexcept = default;

SelectableLottieImage::~SelectableLottieImage() = default;

const Dali::String& SelectableLottieImage::GetUrl() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(mImpl);
  return mImpl->mUrl;
}

Int32Pair SelectableLottieImage::GetSelectRange() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(mImpl);
  return mImpl->mSelectRange;
}

Int32Pair SelectableLottieImage::GetDeselectRange() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(mImpl);
  return mImpl->mDeselectRange;
}

const Dali::String& SelectableLottieImage::GetInnerFillKeyPath() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE(mImpl);
  return mImpl->mInnerFillKeyPath;
}

#undef DALI_ASSERT_VALID_SELECTABLE_LOTTIE_IMAGE

} // namespace Ui
} //namespace DALI_NAMESPACE
