/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "async-image-loader.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/image-loader/async-image-loader-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-url.h>

using Dali::Integration::ToStdString;

namespace Dali
{
namespace Ui
{
AsyncImageLoader::AsyncImageLoader()
{
}

AsyncImageLoader::~AsyncImageLoader()
{
}

AsyncImageLoader::AsyncImageLoader(Internal::AsyncImageLoader* impl)
: BaseHandle(impl)
{
}

AsyncImageLoader::AsyncImageLoader(const AsyncImageLoader& handle) = default;

AsyncImageLoader::AsyncImageLoader(AsyncImageLoader&& rhs) noexcept = default;

AsyncImageLoader& AsyncImageLoader::operator=(const AsyncImageLoader& handle) = default;

AsyncImageLoader& AsyncImageLoader::operator=(AsyncImageLoader&& rhs) noexcept = default;

AsyncImageLoader AsyncImageLoader::DownCast(BaseHandle handle)
{
  return AsyncImageLoader(dynamic_cast<Dali::Ui::Internal::AsyncImageLoader*>(handle.GetObjectPtr()));
}

AsyncImageLoader AsyncImageLoader::New()
{
  IntrusivePtr<Internal::AsyncImageLoader> internal = Internal::AsyncImageLoader::New();
  return AsyncImageLoader(internal.Get());
}

uint32_t AsyncImageLoader::Load(const Dali::String& url)
{
  return GetImplementation(*this).Load(Ui::Internal::VisualUrl(ToStdString(url)), ImageDimensions(),
                                       SamplingMode::BOX_THEN_LINEAR, true,
                                       DevelAsyncImageLoader::PreMultiplyOnLoad::OFF, false);
}

uint32_t AsyncImageLoader::Load(const Dali::String& url, ImageDimensions dimensions)
{
  return GetImplementation(*this).Load(Ui::Internal::VisualUrl(ToStdString(url)), dimensions,
                                       SamplingMode::BOX_THEN_LINEAR, true,
                                       DevelAsyncImageLoader::PreMultiplyOnLoad::OFF, false);
}

uint32_t AsyncImageLoader::Load(const Dali::String& url, ImageDimensions dimensions,
                                SamplingMode::Type samplingMode, bool orientationCorrection)
{
  return GetImplementation(*this).Load(Ui::Internal::VisualUrl(ToStdString(url)), dimensions, samplingMode,
                                       orientationCorrection, DevelAsyncImageLoader::PreMultiplyOnLoad::OFF, false);
}

bool AsyncImageLoader::Cancel(uint32_t loadingTaskId)
{
  return GetImplementation(*this).Cancel(loadingTaskId);
}

void AsyncImageLoader::CancelAll()
{
  GetImplementation(*this).CancelAll();
}

AsyncImageLoader::ImageLoadedSignalType& AsyncImageLoader::ImageLoadedSignal()
{
  return GetImplementation(*this).ImageLoadedSignal();
}
} // namespace Ui

} // namespace Dali
