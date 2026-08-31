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

// CLASS HEADER
#include <dali-ui-foundation/public-api/types/selectable-lottie-color-binding.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <algorithm>

namespace Dali::Ui
{

#define DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(impl) \
  DALI_ASSERT_ALWAYS((impl) && "Cannot use a moved-from SelectableLottieColorBinding object")

struct SelectableLottieColorBinding::Impl
{
  Impl(Dali::StringView                         bindingKeyPath,
       LottieAnimation::VectorProperty          bindingProperty,
       ColorPolicy                              bindingColorPolicy,
       const SelectableLottieImage::FrameRange& bindingSelectedColorRange)
  : keyPath(bindingKeyPath),
    property(bindingProperty),
    colorPolicy(bindingColorPolicy),
    selectedColorRange(std::min(bindingSelectedColorRange.startFrame, bindingSelectedColorRange.endFrame),
                       std::max(bindingSelectedColorRange.startFrame, bindingSelectedColorRange.endFrame))
  {
  }

  Dali::String                      keyPath;
  LottieAnimation::VectorProperty   property;
  ColorPolicy                       colorPolicy;
  SelectableLottieImage::FrameRange selectedColorRange;
};

SelectableLottieColorBinding::SelectableLottieColorBinding(
  Dali::StringView                         keyPath,
  LottieAnimation::VectorProperty          property,
  ColorPolicy                              colorPolicy,
  const SelectableLottieImage::FrameRange& selectedColorRange)
: mImpl(nullptr)
{
  DALI_ASSERT_ALWAYS(!keyPath.Empty() && "SelectableLottieColorBinding key path must not be empty");
  DALI_ASSERT_ALWAYS((property == LottieAnimation::VectorProperty::FILL_COLOR ||
                      property == LottieAnimation::VectorProperty::STROKE_COLOR) &&
                     "SelectableLottieColorBinding supports only fill and stroke color properties");
  mImpl = Dali::MakeUnique<Impl>(keyPath, property, colorPolicy, selectedColorRange);
}

SelectableLottieColorBinding::SelectableLottieColorBinding(const SelectableLottieColorBinding& rhs)
: mImpl(nullptr)
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(rhs.mImpl);
  mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
}

SelectableLottieColorBinding::SelectableLottieColorBinding(SelectableLottieColorBinding&& rhs) noexcept = default;

SelectableLottieColorBinding& SelectableLottieColorBinding::operator=(const SelectableLottieColorBinding& rhs)
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(rhs.mImpl);
  if(this != &rhs)
  {
    mImpl = Dali::MakeUnique<Impl>(*rhs.mImpl);
  }
  return *this;
}

SelectableLottieColorBinding& SelectableLottieColorBinding::operator=(SelectableLottieColorBinding&& rhs) noexcept = default;

SelectableLottieColorBinding::~SelectableLottieColorBinding() = default;

Dali::String SelectableLottieColorBinding::GetKeyPath() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(mImpl);
  return mImpl->keyPath;
}

LottieAnimation::VectorProperty SelectableLottieColorBinding::GetProperty() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(mImpl);
  return mImpl->property;
}

SelectableLottieColorBinding::ColorPolicy SelectableLottieColorBinding::GetColorPolicy() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(mImpl);
  return mImpl->colorPolicy;
}

SelectableLottieImage::FrameRange SelectableLottieColorBinding::GetSelectedColorRange() const
{
  DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING(mImpl);
  return mImpl->selectedColorRange;
}

#undef DALI_ASSERT_VALID_SELECTABLE_LOTTIE_COLOR_BINDING

} // namespace Dali::Ui
