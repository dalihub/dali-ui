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

namespace Dali
{
namespace Ui
{

DALI_UI_API SelectableLottieImage::FrameRange::FrameRange(int32_t start, int32_t end)
: startFrame(start),
  endFrame(end)
{
}

SelectableLottieImage::SelectableLottieImage(const Dali::String& url,
                                             const FrameRange&   selectRange,
                                             const FrameRange&   deselectRange,
                                             const Dali::String& innerFillKeyPath)
: mUrl(url),
  mSelectRange(selectRange),
  mDeselectRange(deselectRange),
  mInnerFillKeyPath(innerFillKeyPath)
{
}

const Dali::String& SelectableLottieImage::GetUrl() const
{
  return mUrl;
}

const SelectableLottieImage::FrameRange& SelectableLottieImage::GetSelectRange() const
{
  return mSelectRange;
}

const SelectableLottieImage::FrameRange& SelectableLottieImage::GetDeselectRange() const
{
  return mDeselectRange;
}

const Dali::String& SelectableLottieImage::GetInnerFillKeyPath() const
{
  return mInnerFillKeyPath;
}

} // namespace Ui
} // namespace Dali
