/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-ui-foundation/internal/visuals/animated-image/image-cache.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <utility>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
ImageCache::LoadRequestObserver::LoadRequestObserver(LoadResultObserver& resultObserver, std::weak_ptr<ImageCache> cache, uint32_t frameIndex)
: mResultObserver(resultObserver),
  mCache(std::move(cache)),
  mFrameIndex(frameIndex)
{
}

void ImageCache::LoadRequestObserver::LoadComplete(bool loadSuccess, TextureInformation textureInformation)
{
  auto keepAlive = shared_from_this();
  auto cache     = mCache.lock();
  if(DALI_LIKELY(cache))
  {
    mResultObserver.OnLoadResult(*keepAlive, mFrameIndex, loadSuccess, textureInformation);
  }
}

ImageCache::ImageCache(TextureManager& textureManager, ImageDimensions size,
                       Dali::SamplingMode::Type samplingMode, TextureManager::MaskingDataPointer maskingData,
                       ImageCache::FrameReadyObserver& observer, uint32_t batchSize, uint32_t interval,
                       bool preMultiplyOnLoad)
: mTextureManager(textureManager),
  mObserver(observer),
  mMaskingData(std::move(maskingData)),
  mDesiredSize(size),
  mBatchSize(batchSize),
  mInterval(interval),
  mLoadState(TextureManager::LoadState::NOT_STARTED),
  mSamplingMode(samplingMode),
  mPreMultiplyOnLoad(preMultiplyOnLoad)
{
}

ImageCache::~ImageCache()
{
  ReleaseMaskingData(mTextureManager, mMaskingData);
}

void ImageCache::SetInterval(uint32_t interval)
{
  mInterval = interval;
}

void ImageCache::SetDesiredSize(Dali::ImageDimensions desiredSize)
{
  mDesiredSize = desiredSize;

  ClearCache();
}

void ImageCache::ReleaseMaskingData(TextureManager& textureManager, TextureManager::MaskingDataPointer& maskingData)
{
  if(Dali::Adaptor::IsAvailable() && maskingData)
  {
    maskingData->mAlphaMaskUrl.DecreaseExternalResourceReference(textureManager);
  }
  maskingData.reset();
}

TextureManager::MaskingDataPointer& ImageCache::GetMaskingData()
{
  return mMaskingData;
}

const TextureManager::MaskingDataPointer& ImageCache::GetMaskingData() const
{
  return mMaskingData;
}

TextureManager::MaskingDataPointer ImageCache::ReleaseMaskingDataOwnership()
{
  return std::move(mMaskingData);
}

void ImageCache::TakeMaskingDataOwnership(TextureManager::MaskingDataPointer maskingData)
{
  mMaskingData = std::move(maskingData);
}

} // namespace Internal
} // namespace Ui
} //namespace DALI_NAMESPACE
